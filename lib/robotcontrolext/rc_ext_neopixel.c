#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <robotcontrol.h>

#include "rc_ext_neopixel.h"
#include "rc_ext_pru.h"

#define MSG_TYPE_NEOPIXEL_SET   0x20


typedef struct {
    message_t msg;
    uint32_t pixels[RC_EXT_NEOPIXEL_COUNT];
} __attribute__((__packed__)) neopixel_message_t;


static int clear_strip() {
    uint32_t pixels[RC_EXT_NEOPIXEL_COUNT];
    for (size_t i=0; i<RC_EXT_NEOPIXEL_COUNT; i++) {
        pixels[i] = 0x00000000;
    }
    return rc_ext_neopixel_set(pixels);
}

int rc_ext_neopixel_init() {
    if (clear_strip()<0) {
        return -1;
    }
    return 0;
}


void rc_ext_neopixel_cleanup() {
    clear_strip();
}


int rc_ext_neopixel_set(uint32_t pixels[RC_EXT_NEOPIXEL_COUNT]) {
    neopixel_message_t msg;
    msg.msg.type = MSG_TYPE_NEOPIXEL_SET;
    // Reorder pixels in a way that makes more sense
    // [0-8]  = "Front" led strip left to right
    // [9-15] = "Back"  led strip left to right
    for (size_t i=0; i<RC_EXT_NEOPIXEL_COUNT/2; ++i) {
        // Front
        msg.pixels[RC_EXT_NEOPIXEL_COUNT/2-i-1] = pixels[i+RC_EXT_NEOPIXEL_COUNT/2];
        // Back
        msg.pixels[RC_EXT_NEOPIXEL_COUNT-i-1]   = pixels[i];
    }
    return rc_ext_pru_send_message(&msg, sizeof(msg));
}
