#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <pru_rpmsg.h>

#include "resource_table_0.h"

#include "message.h"
#include "fbus.h"
#include "encoder.h"
#include "neopixel.h"
#include "servo.h"
#include "debug.h"


/*
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
#define VIRTIO_CONFIG_S_DRIVER_OK	4

/* Host-0 Interrupt sets bit 30 in register R31 */
#define HOST_INT			((uint32_t) 1 << 30)

/* The PRU-ICSS system events used for RPMsg are defined in the Linux device tree
 * PRU0 uses system event 16 (To ARM) and 17 (From ARM)
 * PRU1 uses system event 18 (To ARM) and 19 (From ARM)
 */
#define TO_ARM_HOST		16
#define FROM_ARM_HOST	17

/*
 * Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
 * at linux-x.y.z/drivers/rpmsg/rpmsg_pru.c
 */
#define CHAN_NAME			"rpmsg-pru"





volatile register uint32_t __R30;
volatile register uint32_t __R31;

static struct pru_rpmsg_transport transport;
static uint8_t msg_in[RPMSG_BUF_SIZE];
static uint8_t msg_out[RPMSG_BUF_SIZE];



static void message_begin() {
  	volatile uint8_t *status;

  	// Clear the status of the PRU-ICSS system event that the ARM will use to 'kick' us 
#ifdef CHIP_IS_am57xx
	CT_INTC.SICR_bit.STATUS_CLR_INDEX = FROM_ARM_HOST;
#else
	CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;
#endif

	// Make sure the Linux drivers are ready for RPMsg communication 
	status = &resourceTable.rpmsg_vdev.status;
	while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

	// Initialize the RPMsg transport structure 
	pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0, &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);

	// Create the RPMsg channel between the PRU and ARM user space using the transport structure. 
	while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHANNEL_DESC, CHANNEL_PORT) != PRU_RPMSG_SUCCESS);
}


static void message_end() {
  	pru_rpmsg_channel(RPMSG_NS_DESTROY, &transport, CHAN_NAME, CHANNEL_DESC, CHANNEL_PORT);
}


static void message_update() {
	uint16_t src, dst, len;

	/* Check bit 30 of register R31 to see if the ARM has kicked us */
	if (__R31 & HOST_INT) {
		/* Clear the event status */
		CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;
		/* Receive all available messages, multiple messages can be sent per kick */
		while (pru_rpmsg_receive(&transport, &src, &dst, msg_in, &len) == PRU_RPMSG_SUCCESS) {
			/* Echo the message back to the same address from which we just received */
			message_t *msg = (message_t*)msg_in;
			switch (msg->type) {
				case MSG_TYPE_ECHO:
					debug_print("Echo %hu", len);
					message_send(msg, len);
					break;
				case MSG_TYPE_FBUS_CONFIG:
					fbus_config_message(msg, len);
					break;
				case MSG_TYPE_FBUS_SERVO:
					fbus_servo_message(msg, len);
					break;
				case MSG_TYPE_FBUS_TELEMERTY:
					fbus_telemetry_message(msg, len);
					break;
				case MSG_TYPE_NEOPIXEL_SET:
					neopixel_set_message(msg, len);
					break;
				case MSG_TYPE_ENCODER_START:
					encoder_start(msg, len);
					break;
				case MSG_TYPE_ENCODER_STOP:
					encoder_stop(msg, len);
					break;
				case MSG_TYPE_SERVO_LIMIT:
					servo_limit_message(msg, len);
					break;
			}
		}
	}
}

message_t *message_out_buffer() {
	return (message_t*)msg_out;
}

void message_send(message_t *data, uint16_t len) {
    pru_rpmsg_send(&transport, CHANNEL_PORT, CHANNEL_SRC, (void*)data, len);
}

#define DEBUG_PIN (1<<3)				// Bit number to output on PRU0_3

void main(void) {
	int running = 1;

	// Clear SYSCFG[STANDBY_INIT] to enable OCP master port
    CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
	
	// C28 defaults to 0x00000000, we need to set bits 23:8 to 0x0100 in order to have it point to 0x00010000        
    PRU0_CTRL.CTPPR0_bit.C28_BLK_POINTER = 0x0100;
	
	__R30 &= ~(1U<<2);

	message_begin();
	debug_begin();
	encoder_begin();
	neopixel_begin();
	servo_begin();
	fbus_begin();

	// Enable instruction counter
	PRU0_CTRL.CTRL_bit.CTR_EN = 1;

	debug_print("Starting main loop");

	while (running) {
		encoder_update();
		message_update();
		encoder_update();
		//__R30 |= DEBUG_PIN;
		fbus_update();
		//__R30 &= ~DEBUG_PIN;

		// Check instruction counter
		if (!PRU0_CTRL.CTRL_bit.CTR_EN) {
			PRU0_CTRL.CYCLE = 0x00000000;
			PRU0_CTRL.CTRL_bit.CTR_EN = 1;
		}
	}

	fbus_end();
	servo_end();
	neopixel_end();
	encoder_end();
	message_end();


	__halt();
}

