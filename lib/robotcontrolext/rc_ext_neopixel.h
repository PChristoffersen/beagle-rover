#ifndef _RC_EXT_NEOPIXEL_H_
#define _RC_EXT_NEOPIXEL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RC_EXT_NEOPIXEL_COUNT 16u

int rc_ext_neopixel_init();
void rc_ext_neopixel_cleanup();
int rc_ext_neopixel_set(uint32_t pixels[RC_EXT_NEOPIXEL_COUNT]);

#ifdef __cplusplus
}
#endif

#endif 

