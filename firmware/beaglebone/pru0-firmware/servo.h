#ifndef _SERVO_H_
#define _SERVO_H_

#include <stdint.h>
#include "message.h"

#define SERVO_COUNT 8
#define SERVO_MIN 500
#define SERVO_MAX 2500
#define SERVO_MID (SERVO_MIN+(SERVO_MAX-SERVO_MIN)/2)
#define SERVO_UNSET 0

void servo_begin();
void servo_end();
void servo_pulse_us(int channel, uint32_t us);
void servo_limit_message(message_t *msg, uint16_t len);

#endif
