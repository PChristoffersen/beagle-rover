#ifndef _FBUS_H_
#define _FBUS_H_

#include <stdint.h>

#include "message.h"

void fbus_begin();
void fbus_update();
void fbus_end();
void fbus_telemetry_message(message_t *msg, uint16_t len);
void fbus_config_message(message_t *msg, uint16_t len);
void fbus_servo_message(message_t *msg, uint16_t len);
void fbus_reset_message(message_t *msg, uint16_t len);

#endif
