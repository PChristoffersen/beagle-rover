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
    volatile uint32_t counter;
    volatile uint8_t flags;
    volatile uint8_t rssi;
    volatile uint8_t n_channels;
    volatile uint8_t _reserved;
    volatile uint32_t channels[FBUS_CHANNELS];
} __attribute__((__packed__)) shm_fbus_t;


int rc_ext_fbus_init(void);
void rc_ext_fbus_cleanup(void);
volatile shm_fbus_t *rc_ext_fbus_get_shm(void);

void rc_ext_fbus_set_servo_map(uint8_t map[FBUS_CHANNELS]);
void rc_ext_fbus_send_telemetry(uint16_t app_id, uint32_t data);

#ifdef __cplusplus
}
#endif

#endif 

