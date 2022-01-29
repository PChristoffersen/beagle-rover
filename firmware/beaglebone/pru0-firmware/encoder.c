#include <stdint.h>

#include "encoder.h"
#include "sharedmem.h"

#include "message.h"
#include "debug.h"

#define PIN_A (1<<14)
#define PIN_B (1<<15)


volatile register uint32_t __R31;

static volatile shm_encoder_t *shm_encoder = SHM_ENCODER;

static uint32_t old;
static int started = 0;

void encoder_update() {
    if (!started) {
        return;
    }
    uint32_t state = __R31 & (PIN_A|PIN_B);
    uint32_t changed = state ^ old;

    if (changed & PIN_A) {
        old = old ^ changed;
        if (changed & PIN_B) { // A and B have both changed
            return;
        }

        // A has fallen
        if ( (old & PIN_A) == 0 ) {
            if ((old & PIN_B) == 0) { // B is clear (low) decrement
                shm_encoder->value -= 1;
            }
            else {
                shm_encoder->value += 1;
            }
            return;
        }

        // A has risen, if B is HIGH, decrement
        if (old & PIN_B) {
            shm_encoder->value -= 1;
        }
        else {
            shm_encoder->value += 1;
        }
        return;
    }
    
    if (changed & PIN_B) {
        old = old ^ changed;
        
        // B has fallen
        if ( (old & PIN_B) == 0 ) {
            if ( (old & PIN_A) == 0) { // CHA is clear (low) increment
                shm_encoder->value += 1;
            }
            else {
                shm_encoder->value -= 1;
            }
            return;
        }

        // B has risen, if A is HIGH, increment
        if (old & PIN_A) {
            shm_encoder->value += 1;
            return;
        }
        else {
            shm_encoder->value -= 1;
        }
        return;
    }
}


void encoder_begin() {
    old = __R31;
    shm_encoder->value = 0;
}

void encoder_end() {
    started = 0;
}

void encoder_start(message_t *msg, uint16_t len) {
    old = __R31;
    shm_encoder->value = 0;
    started = 1;
}

void encoder_stop(message_t *msg, uint16_t len) {
    started = 0;
}
