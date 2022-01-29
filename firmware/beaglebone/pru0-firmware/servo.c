#include "servo.h"
#include "sharedmem.h"

#define PRU_SERVO_LOOP_INSTRUCTIONS 48 // instructions per PRU servo timer loop


static volatile shm_servo_t *shm_servo = SHM_SERVO;


typedef struct {
    uint32_t low;
    uint32_t high;
} servo_limit_t;


static servo_limit_t servo_limits[SERVO_CHANNELS];


void servo_begin() {
    int i;
    for (i=0; i<SERVO_CHANNELS; i++) {
        servo_limits[i].low = SERVO_MIN;
        servo_limits[i].high = SERVO_MAX;
    }
}

void servo_end() {

}

void servo_pulse_us(int channel, uint32_t us) {
    if (us<servo_limits[channel].low) {
        us = servo_limits[channel].low;
    }
    if (us>servo_limits[channel].high) {
        us = servo_limits[channel].high;
    }
  	uint32_t num_loops = ((us*200)/PRU_SERVO_LOOP_INSTRUCTIONS);
    if (shm_servo->channel[channel]!=0) {
        return;
    }
    shm_servo->channel[channel] = num_loops;
}



void servo_limit_message(message_t *msg, uint16_t len) {
    if (len == sizeof(message_t)+SERVO_CHANNELS*sizeof(servo_limit_t)) {
        servo_limit_t *data = (servo_limit_t*)msg->data;
        int i;
        for (i=0; i<SERVO_CHANNELS; i++) {
            servo_limits[i].low = data[i].low;
            servo_limits[i].high = data[i].high;
        }
    }
}
