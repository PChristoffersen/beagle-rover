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


static int send_pixels(int fd, uint32_t *pixels, int count) {
    neopixel_message_t msg;
    msg.msg.type = MSG_TYPE_NEOPIXEL_SET;
    memcpy(&msg.pixels, pixels, sizeof(uint32_t)*count);
    if (count<RC_EXT_NEOPIXEL_COUNT) {
        memset(&msg.pixels[count], 0x00, sizeof(uint32_t)*(RC_EXT_NEOPIXEL_COUNT-count));
    }
    int ret = write(fd, &msg, sizeof(msg));
    if (ret<0) {
   		perror("ERROR in rc_ext_neopixels, failed to write to PRU device");
        return -1;
    }
    return 0;
}

static int clear_strip(int fd) {
    uint32_t pixels[RC_EXT_NEOPIXEL_COUNT];
    for (int i=0; i<RC_EXT_NEOPIXEL_COUNT; i++) {
        pixels[i] = 0x00000000;
    }
    return send_pixels(fd, pixels, RC_EXT_NEOPIXEL_COUNT);
}

int rc_ext_neopixel_init() {
    int fd = rc_ext_pru_fd();

    if (clear_strip(fd)<0) {
        return -1;
    }

    return 0;
}


void rc_ext_neopixel_cleanup() {
    int fd = rc_ext_pru_fd();
    if (fd!=-1) {
        clear_strip(fd);
    }
}


int rc_ext_neopixel_set(uint32_t *pixels, int count) {
    int fd = rc_ext_pru_fd();
    return send_pixels(fd, pixels, count);
}
