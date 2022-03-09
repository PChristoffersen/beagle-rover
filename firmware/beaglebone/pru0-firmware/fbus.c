#include <stdint.h>
#include <stdlib.h>
#include <pru_uart.h>
#include <pru_ctrl.h>

#include "servo.h"
#include "message.h"
#include "debug.h"
#include "sharedmem.h"

#include "fbus.h"
#include "fbus_types.h"

#define UART_TX_FIFO_SIZE	16

#define FBUS_CHANNELS  24
#define FBUS_CHANNEL_MIN 8
#define FBUS_CHANNEL_MAX 1976
#define FBUS_SERVO_VALUE(VAL) (SERVO_MIN+(((uint32_t)(VAL)-FBUS_CHANNEL_MIN)*(SERVO_MAX-SERVO_MIN))/(FBUS_CHANNEL_MAX-FBUS_CHANNEL_MIN))

#define FBUS_INIT_FLAGS 0x04

#define FBUS_CONTROL_HDR 0xFF
#define FBUS_CONTROL_8CH_SIZE  0x0D
#define FBUS_CONTROL_16CH_SIZE 0x18
#define FBUS_CONTROL_24CH_SIZE 0x23
#define FBUS_CONTROL_HDR_SIZE 2
#define FBUS_CONTROL_SIZE(BUF) (FBUS_CONTROL_HDR_SIZE+BUF[0]+1)
#define FBUS_CONTROL_CRC(BUF) (BUF[BUF[0]+FBUS_CONTROL_HDR_SIZE])
#define FBUS_CONTROL_FLAGS(BUF) (BUF[BUF[0]+FBUS_CONTROL_HDR_SIZE-2])
#define FBUS_CONTROL_RSSI(BUF) (BUF[BUF[0]+FBUS_CONTROL_HDR_SIZE-1])

#define FBUS_DOWNLINK_HDR 0x08
#define FBUS_DOWNLINK_HDR_SIZE 1
#define FBUS_DOWNLINK_SIZE (FBUS_DOWNLINK_HDR_SIZE+FBUS_DOWNLINK_HDR+1)
#define FBUS_DOWNLINK_CRC(BUF) (BUF[BUF[0]+1])

#define FBUS_UPLINK_HDR 0x08
#define FBUS_UPLINK_HDR_SIZE 1
#define FBUS_UPLINK_SIZE (FBUS_UPLINK_HDR_SIZE+FBUS_UPLINK_HDR+1)
#define FBUS_UPLINK_CRC(BUF) (BUF[BUF[0]+1])
#define FBUS_ULINK_DELAY_MIN (200UL*500) // Delay (in cycles) to wait before sending uplink (500us)
#define FBUS_ULINK_DELAY_MAX (200UL*2000) // Max delay - before skipping uplink


#define FBUS_BUFFER_SIZE 64

#define CHANNEL_OFFSET 880
#define CHANNEL_DIV 64
#define CHANNEL_MUL 40


#define FBUS_DEFAULT_DEVICE_ID 0x67
#define FBUS_SERVO_UNMAPPED 0xFF

enum FBusState {
    FBUS_SYNCING,
    FBUS_READ_CONTROL,
    FBUS_READ_DOWNLINK,
    FBUS_WAIT_WRITE_UPLINK,
    FBUS_WRITE_UPLINK,
    FBUS_READ_UPLINK
};

enum FBusUARTState {
    FBUS_IDLE,
    FBUS_RX,
    FBUS_TX,
};


#define FBUS_MSG_TYPE_CONFIG    0x10
#define FBUS_MSG_TYPE_TELEMETRY 0x11
#define FBUS_MSG_TYPE_CONTROL   0x12

#define FBUS_TELEMETRY_PENDING_MAX 8
#define FBUS_TELEMETRY_PENDING_MASK (FBUS_TELEMETRY_PENDING_MAX-1)


#define FBUS_DEBUG(X...) debug_print(X)


typedef struct {
    uint8_t rssi;
    uint8_t flags;
    uint16_t channels[FBUS_CHANNELS];
} fbus_ctl_data_t;

typedef struct {
    uint8_t device_id;
} fbus_config_data_t;

typedef struct {
    uint8_t map[FBUS_CHANNELS];
} fbus_servo_data_t;

typedef struct {
    uint16_t app_id;
    uint32_t data;
} fbus_telemetry_data_t;


static volatile shm_fbus_t *shm_fbus = SHM_FBUS;

static uint8_t fbus_buffer[FBUS_BUFFER_SIZE];
static int fbus_buffer_pos = 0;
static int fbus_required_size = 2;
static uint32_t fbus_write_start;
static enum FBusState fbus_state = FBUS_SYNCING;
static enum FBusUARTState fbus_uart_state = FBUS_RX;

static fbus_config_data_t fbus_config;
static fbus_servo_data_t fbus_servo;
static fbus_telemetry_data_t fbus_telemetry_pending[FBUS_TELEMETRY_PENDING_MAX];
static uint32_t fbus_telemetry_head = 0;
static uint32_t fbus_telemetry_tail = 0;



static uint8_t fbus_checksum(const uint8_t *buf, int sz) {
    int i;
    uint16_t sum = 0x00;
    for (i=0; i<sz; i++) {
        sum += buf[i];
    }
    while (sum > 0xFF) {
        sum = (sum & 0xFF) + (sum >>8);
    }
    return 0xFF - sum;
}


static void fbus_sync_begin() {
    //debug_print("Sync");
    fbus_state = FBUS_SYNCING;
    fbus_uart_state = FBUS_RX;
    fbus_required_size = 2;
}


static void fbus_read_control_begin() {
    //debug_print("Control");
    fbus_state = FBUS_READ_CONTROL;
    fbus_uart_state = FBUS_RX;
    fbus_required_size = 2;
}

static void fbus_read_downlink_begin() {
    //debug_print("Downlink");
    fbus_state = FBUS_READ_DOWNLINK;
    fbus_uart_state = FBUS_RX;
    fbus_required_size = FBUS_DOWNLINK_SIZE;
}

static void fbus_wait_write_uplink_begin() {
    //debug_print("WUplink");
    fbus_state = FBUS_WAIT_WRITE_UPLINK;
    fbus_uart_state = FBUS_IDLE;
    fbus_required_size = FBUS_UPLINK_SIZE;
    fbus_write_start = PRU0_CTRL.CYCLE;
}

static void fbus_write_uplink_begin() {
    //debug_print("WUplink");
    fbus_state = FBUS_WRITE_UPLINK;
    fbus_uart_state = FBUS_TX;
    fbus_required_size = FBUS_UPLINK_SIZE;
}

static void fbus_read_uplink_begin() {
    //debug_print("RUplink");
    fbus_state = FBUS_READ_UPLINK;
    fbus_uart_state = FBUS_RX;
    fbus_required_size = 1;
}


static void fbus_sync_drop_bytes(int n) {
    int i;
    fbus_buffer_pos -= n;
    for (i=0; i<fbus_buffer_pos; i++) {
        fbus_buffer[i] = fbus_buffer[i+n];
    }
}


static int fbus_fill_buffer() {
	while (fbus_buffer_pos<fbus_required_size && CT_UART.LSR_bit.DR) {
#if 0
        if (CT_UART.LSR_bit.RXFIFOE) {
        }
        if (CT_UART.LSR_bit.FE) {
        }
        if (CT_UART.LSR_bit.PE) {
        }
        if (CT_UART.LSR_bit.OE) {
        }
#endif
        fbus_buffer[fbus_buffer_pos++] = CT_UART.RBR_bit.DATA;
    }
    return fbus_buffer_pos>=fbus_required_size;
}


static int fbus_drain_buffer() {
    if (fbus_buffer_pos<fbus_required_size) {
		if (CT_UART.LSR_bit.TEMT) {
            int cnt = 0;
            while (fbus_buffer_pos<fbus_required_size && cnt<UART_TX_FIFO_SIZE) {
            	CT_UART.THR = fbus_buffer[fbus_buffer_pos++];
                cnt++;
            }
        }
        return 0;
    }
    return !!(CT_UART.LSR_bit.TEMT);
}



static void fbus_telemetry_push(const fbus_telemetry_data_t *data) {
    fbus_telemetry_pending[fbus_telemetry_head].app_id = data->app_id;
    fbus_telemetry_pending[fbus_telemetry_head].data = data->data;
    
    fbus_telemetry_head = (fbus_telemetry_head+1) & FBUS_TELEMETRY_PENDING_MASK;
    if (fbus_telemetry_tail==fbus_telemetry_head) {
        fbus_telemetry_tail = (fbus_telemetry_tail+1) & FBUS_TELEMETRY_PENDING_MASK;
    }

}

static fbus_telemetry_data_t *fbus_telemetry_pop() {
    if (fbus_telemetry_head==fbus_telemetry_tail) {
        return NULL;
    }
    uint8_t tail = fbus_telemetry_tail;
    fbus_telemetry_tail = (fbus_telemetry_tail+1) & FBUS_TELEMETRY_PENDING_MASK;
    return &fbus_telemetry_pending[tail];
}


static void set_8channel(volatile uint32_t *channels, uint8_t *data, int offset) {
    channels[offset]   = FBUS_SERVO_VALUE(data[0] + ((0x07 & data[1])<<8));
    channels[offset+1] = FBUS_SERVO_VALUE(((0xF8 & data[1])>>3) + ((0x3F & data[2])<<5));
    channels[offset+2] = FBUS_SERVO_VALUE(((0xC0 & data[2])>>6) + (data[3]<<2) + ((0x01 & data[4])<<10));
    channels[offset+3] = FBUS_SERVO_VALUE(((0xFE & data[4])>>1) + ((0x0F & data[5])<<7));
    channels[offset+4] = FBUS_SERVO_VALUE(((0xF0 & data[5])>>4) + ((0x7F & data[6])<<4));
    channels[offset+5] = FBUS_SERVO_VALUE(((0x80 & data[6])>>7) + (data[7]<<1) + ((0x03 & data[8])<<9));
    channels[offset+6] = FBUS_SERVO_VALUE(((0xFC & data[8])>>2) + ((0x1F & data[9])<<6));
    channels[offset+7] = FBUS_SERVO_VALUE(((0xF0 & data[9])>>5 ) + ((data[10])<<3));
}

static void fbus_parse_control() {
    uint8_t n_channels = 8;

    if (fbus_buffer[1]!=FBUS_CONTROL_HDR) {
        // Lost sync
        fbus_sync_begin();
        return;
    }

    if (fbus_buffer[1]!=FBUS_CONTROL_HDR || (fbus_buffer[0]!=FBUS_CONTROL_8CH_SIZE && fbus_buffer[0]!=FBUS_CONTROL_16CH_SIZE && fbus_buffer[0]!=FBUS_CONTROL_24CH_SIZE)) {
        // Lost sync
        fbus_sync_begin();
        return;
    }

    fbus_required_size = FBUS_CONTROL_SIZE(fbus_buffer);
    if (!fbus_fill_buffer()) {
        return;
    }

    // Validate CRC
    if (fbus_checksum(fbus_buffer+FBUS_CONTROL_HDR_SIZE, fbus_buffer[0])!=FBUS_CONTROL_CRC(fbus_buffer)) {
        fbus_sync_begin();
        return;
    }

    // Process package
    shm_fbus->flags = FBUS_CONTROL_FLAGS(fbus_buffer);
    shm_fbus->rssi = FBUS_CONTROL_RSSI(fbus_buffer);
    set_8channel(shm_fbus->channels, fbus_buffer+FBUS_CONTROL_HDR_SIZE, 0);
    if (fbus_buffer[0]>=FBUS_CONTROL_16CH_SIZE) {
        set_8channel(shm_fbus->channels, fbus_buffer+FBUS_CONTROL_HDR_SIZE+11, 8);
        n_channels=16;
    }
    if (fbus_buffer[0]>=FBUS_CONTROL_24CH_SIZE) {
        set_8channel(shm_fbus->channels, fbus_buffer+FBUS_CONTROL_HDR_SIZE+22, 16);
        n_channels=24;
    }
    shm_fbus->n_channels = n_channels;
    shm_fbus->counter++;
    // Send servo pulses to PRU1
    int i;
    for (i=0; i<FBUS_CHANNELS; i++) {
        if (fbus_servo.map[i]!=FBUS_SERVO_UNMAPPED) {
            servo_pulse_us(fbus_servo.map[i], shm_fbus->channels[i]);
        }
    }

    fbus_buffer_pos = 0;
    fbus_read_downlink_begin();
}



static void fbus_parse_downlink() {
    if (fbus_buffer[0]!=FBUS_DOWNLINK_HDR) {
        fbus_sync_begin();
        return;
    }

    // Validate CRC
    if (fbus_checksum(fbus_buffer+FBUS_DOWNLINK_HDR_SIZE, fbus_buffer[0])!=FBUS_DOWNLINK_CRC(fbus_buffer)) {
        fbus_sync_begin();
        return;
    }

    fbus_downlink_t *downlink = (fbus_downlink_t*)fbus_buffer;
    // Check if we need to respond to downlink
    if (downlink->id == fbus_config.device_id) {
        fbus_buffer_pos = 0;
        fbus_wait_write_uplink_begin();
        return;
    }

    fbus_buffer_pos = 0;
    fbus_read_uplink_begin();
}


static inline void fbus_wait_write_uplink() {
    uint32_t delta = (PRU0_CTRL.CYCLE-fbus_write_start);
    
    if (delta<FBUS_ULINK_DELAY_MIN) {
        // Wait longer
        return;
    }

    if (delta>FBUS_ULINK_DELAY_MAX) {
        // We missed the window
        fbus_read_control_begin();
        return;
    }


    // Start sending uplink    
    fbus_uplink_t *uplink = (fbus_uplink_t*)fbus_buffer;

    uplink->size = FBUS_UPLINK_HDR;
    uplink->id = fbus_config.device_id;
    uplink->prim = 0x10;

    fbus_telemetry_data_t *tel = fbus_telemetry_pop();
    if (tel!=NULL) {
        uplink->app_id = tel->app_id;
        uplink->data = tel->data;
    }
    else {
        uplink->app_id = 0x0000;
        uplink->data = 0x00000000;
    }
    uplink->crc = fbus_checksum(fbus_buffer+FBUS_UPLINK_HDR_SIZE, uplink->size);

    fbus_buffer_pos = 0;
    fbus_write_uplink_begin();
    fbus_drain_buffer();
}

static inline void fbus_write_uplink() {
    fbus_buffer_pos = 0;
    fbus_read_uplink_begin();
}


static void fbus_parse_uplink() {
    if (fbus_buffer[0]!=FBUS_UPLINK_HDR) {
        // No uplink
        fbus_read_control_begin();
        return;
    }

    fbus_required_size = FBUS_UPLINK_SIZE;
    if (!fbus_fill_buffer()) {
        return;
    }

    // Validate CRC
    if (fbus_checksum(fbus_buffer+FBUS_UPLINK_HDR_SIZE, fbus_buffer[0])!=FBUS_UPLINK_CRC(fbus_buffer)) {
        fbus_sync_begin();
        return;
    }


    fbus_buffer_pos = 0;
    fbus_read_control_begin();
}



static void fbus_sync() {
    // Look for header byte
    if (fbus_buffer[1]!=FBUS_CONTROL_HDR) {
        fbus_sync_drop_bytes(2);
        return;
    }

    // Check size 
    if (fbus_buffer[0]!=FBUS_CONTROL_8CH_SIZE && fbus_buffer[0]!=FBUS_CONTROL_16CH_SIZE && fbus_buffer[0]!=FBUS_CONTROL_24CH_SIZE) {
        // Incorrect size - drop the first two bytes 
        fbus_sync_drop_bytes(2);
        return;
    }

    // Update required size to size of control package
    fbus_required_size = FBUS_CONTROL_SIZE(fbus_buffer);
    if (!fbus_fill_buffer()) {
        return;
    }

    // Check CRC
    if (fbus_checksum(fbus_buffer+FBUS_CONTROL_HDR_SIZE, fbus_buffer[0])!=FBUS_CONTROL_CRC(fbus_buffer)) {
        fbus_sync_drop_bytes(2);
        return;
    }
    // Control package OK

    // Everything checks out - we are in sync
    fbus_parse_control();
}







void fbus_begin() {
    int i;

	/* Set up UART to function at 115200 baud - DLL divisor is 104 at 16x oversample
	 * 192MHz / 104 / 16 = ~115200 */
	CT_UART.DLL = 104;
	CT_UART.DLH = 0;
	CT_UART.MDR_bit.OSM_SEL = 0x0;

	/* Enable Interrupts in UART module. This allows the main thread to poll for
	 * Receive Data Available and Transmit Holding Register Empty */
	CT_UART.IER = 0x7;

	/* If FIFOs are to be used, select desired trigger level and enable
	 * FIFOs by writing to FCR. FIFOEN bit in FCR must be set first before
	 * other bits are configured */
	/* Enable FIFOs for now at 1-byte, and flush them */
	CT_UART.FCR = (0x00) | (0x8) | (0x4) | (0x2) | (0x01); // 8-byte RX FIFO trigger

	/* Choose desired protocol settings by writing to LCR */
	/* 8-bit word, 1 stop bit, no parity, no break control and no divisor latch */
	CT_UART.LCR = 3;

	/* If flow control is desired write appropriate values to MCR. */
	/* No flow control for now */
	CT_UART.MCR = 0x00;

	/* Choose desired response to emulation suspend events by configuring
	 * FREE bit and enable UART by setting UTRST and URRST in PWREMU_MGMT */
	/* Allow UART to run free, enable UART TX/RX */
	CT_UART.PWREMU_MGMT_bit.FREE = 0x1;
	CT_UART.PWREMU_MGMT_bit.URRST = 0x1;
	CT_UART.PWREMU_MGMT_bit.UTRST = 0x1;

	/* Turn off RTS and CTS functionality */
	CT_UART.MCR_bit.AFE = 0x0;
	CT_UART.MCR_bit.RTS = 0x0;

    // Reset config data
    fbus_config.device_id = FBUS_DEFAULT_DEVICE_ID;
    for (i=0; i<FBUS_CHANNELS; i++) {
        fbus_servo.map[i] = FBUS_SERVO_UNMAPPED;
        shm_fbus->channels[i] = SERVO_UNSET;
    }
    shm_fbus->counter = 0;
    shm_fbus->flags = FBUS_INIT_FLAGS;
    shm_fbus->rssi = 0x00;
    shm_fbus->n_channels = 16;

    fbus_buffer_pos = 0;
    fbus_sync_begin();
}


void fbus_end() {
	/* Disable UART before halting */
	CT_UART.PWREMU_MGMT = 0x0;
}


void fbus_update() {
    switch (fbus_uart_state) {
        case FBUS_RX:
            if (!fbus_fill_buffer()) {
                return;
            }
            break;
        case FBUS_TX:
            if (!fbus_drain_buffer()) {
                return;
            }
            break;
    }

    switch (fbus_state) {
    case FBUS_SYNCING:
        fbus_sync();
        break;
    case FBUS_READ_CONTROL:
        fbus_parse_control();
        break;
    case FBUS_READ_DOWNLINK:
        fbus_parse_downlink();
        break;
    case FBUS_WAIT_WRITE_UPLINK:
        fbus_wait_write_uplink();
        break;
    case FBUS_WRITE_UPLINK:
        fbus_write_uplink();
        break;
    case FBUS_READ_UPLINK:
        fbus_parse_uplink();
        break;
    }
}


void fbus_telemetry_message(message_t *msg, uint16_t len) {
    if (len == sizeof(message_t)+sizeof(fbus_telemetry_data_t)) {
        fbus_telemetry_push((fbus_telemetry_data_t*)msg->data);
    }
}


void fbus_config_message(message_t *msg, uint16_t len) {
    if (len == sizeof(message_t)+sizeof(fbus_config_data_t)) {
        fbus_config_data_t *data = (fbus_config_data_t*)msg->data;
        fbus_config.device_id = data->device_id;
    }
}


void fbus_servo_message(message_t *msg, uint16_t len) {
    if (len == sizeof(message_t)+sizeof(fbus_servo_data_t)) {
        fbus_servo_data_t *data = (fbus_servo_data_t*)msg->data;
        int i;
        for (i=0; i<FBUS_CHANNELS; i++) {
            fbus_servo.map[i] = data->map[i];
        }
    }
}


void fbus_reset_message(message_t *msg, uint16_t len)
{
    shm_fbus->counter = 0;
    shm_fbus->flags = FBUS_INIT_FLAGS;
    shm_fbus->rssi = 0x00;
}
