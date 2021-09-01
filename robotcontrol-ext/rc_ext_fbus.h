#ifndef _RC_EXT_FBUS_H_
#define _RC_EXT_FBUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define FBUS_CHANNELS 24
#define FBUS_CHANNEL_MIN 8
#define FBUS_CHANNEL_MAX 1976

#define FBUS_SERVO_UNMAPPED 0xFF

typedef struct {
    volatile uint16_t counter;
    volatile uint8_t flags;
    volatile uint8_t rssi;
    volatile uint32_t channels[FBUS_CHANNELS];
} __attribute__((__packed__)) shm_fbus_t;


int rc_ext_fbus_init(void);
void rc_ext_fbus_cleanup(void);
volatile shm_fbus_t *rc_ext_fbus_get_channels(void);

void rc_ext_fbus_set_servo_map(uint8_t map[FBUS_CHANNELS]);

void rc_ext_fbus_telemetry_send_battery(uint8_t id, float *cells, uint8_t n_cells);

#ifdef __cplusplus
}
#endif

#endif 

