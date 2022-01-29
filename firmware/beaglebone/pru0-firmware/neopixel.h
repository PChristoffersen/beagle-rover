#ifndef _NEOPIXEL_H_
#define _NEOPIXEL_H_

#include <stdint.h>

#include "message.h"

void neopixel_begin();
void neopixel_end();
void neopixel_set_message(message_t *msg, uint16_t len);

#endif 
