#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdint.h>
#include <pru_rpmsg.h>

#define CHANNEL_PORT 30
#define CHANNEL_DESC "PRU Channel"
#define CHANNEL_SRC  1024


#define MSG_TYPE_ECHO    0x00
#define MSG_TYPE_DEBUG   0x01
#define MSG_TYPE_FBUS_CONFIG    0x10
#define MSG_TYPE_FBUS_SERVO     0x11
#define MSG_TYPE_FBUS_TELEMERTY 0x12
#define MSG_TYPE_FBUS_RESET     0x13
#define MSG_TYPE_NEOPIXEL_SET   0x20
#define MSG_TYPE_ENCODER_START  0x30
#define MSG_TYPE_ENCODER_STOP   0x31
#define MSG_TYPE_SERVO_LIMIT    0x40

typedef struct {
    uint8_t type;
    uint8_t _reserved[3];
    uint8_t data[];
} message_t;

void message_send(message_t *data, uint16_t len);

message_t *message_out_buffer();

#endif 
