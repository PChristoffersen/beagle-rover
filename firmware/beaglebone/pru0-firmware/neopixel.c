#include <stdint.h>
#include <pru_cfg.h>

#include "debug.h"

#include "neopixel.h"
#include "encoder.h"

#define STRIP_LEN 16

#define OUT_PIN (1<<3)				// Bit number to output on PRU0_3


#define	ONE_CYCLES_ON	700/5	// Stay on for 700ns
#define ONE_CYCLES_OFF	600/5
#define ZERO_CYCLES_ON	350/5
#define ZERO_CYCLES_OFF	800/5
#define RESET_CYCLES	51000/5	// Must be at least 50u, use 51u

static uint32_t strip_color[STRIP_LEN];	// green, red, blue

volatile register uint32_t __R30;


static void update_strip() {
    int i,j;
    for(j=0; j<STRIP_LEN; j++) {
        // Cycle through each bit
        for(i=23; i>=0; i--) {
            if(strip_color[j] & (0x1<<i)) {
                __R30 |= OUT_PIN;		// Set the GPIO pin to 1
                __delay_cycles(ONE_CYCLES_ON-1);
                __R30 &= ~OUT_PIN;	// Clear the GPIO pin
                __delay_cycles(ONE_CYCLES_OFF-14);
            } else {
                __R30 |= OUT_PIN;		// Set the GPIO pin to 1
                __delay_cycles(ZERO_CYCLES_ON-1);
                __R30 &= ~OUT_PIN;	// Clear the GPIO pin
                __delay_cycles(ZERO_CYCLES_OFF-14);
            }
        }
        // Update encoder between pixels so we don't miss ticks
        encoder_update();
    }
    // Send Reset
    __R30 &= ~OUT_PIN;	// Clear the GPIO pin

}

void neopixel_begin() {
	int i;
	// Set everything to background
	for(i=0; i<STRIP_LEN; i++) {
		strip_color[i] = 0x00000000;
	}
    update_strip();
}


void neopixel_end() {
    int i;
	for(i=0; i<STRIP_LEN; i++) {
		strip_color[i] = 0x00000000;
	}
    update_strip();

}


void neopixel_set_message(message_t *msg, uint16_t len) {
    uint32_t *data = (uint32_t*)msg->data;

    int n_pixels = (len-sizeof(message_t))/sizeof(uint32_t);
    int i;

    for (i=0; i<n_pixels && i<STRIP_LEN; i++) {
        strip_color[i] = data[i];
    }
    update_strip();
}

