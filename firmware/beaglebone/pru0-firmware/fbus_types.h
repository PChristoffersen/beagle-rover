#ifndef _FBUS_TYPES_H_
#define _FBUS_TYPES_H_

#include <stdint.h>

typedef struct {
    uint8_t size;
    uint8_t id;
    uint8_t prim;
    uint16_t app_id;
    uint32_t data;
    uint8_t crc;
} __attribute__((__packed__)) fbus_downlink_t;


typedef struct {
    uint8_t size;
    uint8_t id;
    uint8_t prim;
    uint16_t app_id;
    uint32_t data;
    uint8_t crc;
} __attribute__((__packed__)) fbus_uplink_t;


#endif 
