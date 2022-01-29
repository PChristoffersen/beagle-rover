#ifndef _SHAREDMEM_H_
#define _SHAREDMEM_H_

#include <stdint.h>


#ifndef PRU_SRAM
#define PRU_SRAM __far __attribute__((cregister("PRU_SHAREDMEM", near)))
#endif

#define SERVO_CHANNELS 8
#define FBUS_CHANNELS 24
#define DEBUG_BUFFER_SIZE 1024

typedef struct {
    volatile uint32_t channel[SERVO_CHANNELS];
} __attribute__((__packed__)) shm_servo_t;


typedef struct {
    volatile uint32_t value;
} __attribute__((__packed__)) shm_encoder_t;


typedef struct {
    volatile uint32_t counter;
    volatile uint8_t flags;
    volatile uint8_t rssi;
    volatile uint8_t n_channels;
    volatile uint8_t _reserved;
    volatile uint32_t channels[FBUS_CHANNELS];
} __attribute__((__packed__)) shm_fbus_t;


typedef struct {
    uint32_t ctl;
    uint8_t  buffer[DEBUG_BUFFER_SIZE];
} __attribute__((__packed__)) shm_debug_t;

#define SHM_BASE  0x10000
#define SHM_SERVO     ((shm_servo_t*)(SHM_BASE))
#define SHM_ENCODER ((shm_encoder_t*)(SHM_BASE+0x0040))
#define SHM_FBUS       ((shm_fbus_t*)(SHM_BASE+0x0080))
#define SHM_DEBUG     ((shm_debug_t*)(SHM_BASE+0x0100))


#endif
