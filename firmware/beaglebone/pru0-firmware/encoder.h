#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "message.h"

void encoder_begin();
void encoder_update();
void encoder_end();

void encoder_start(message_t *msg, uint16_t len);
void encoder_stop(message_t *msg, uint16_t len);

#endif
