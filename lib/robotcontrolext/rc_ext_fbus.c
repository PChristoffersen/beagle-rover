#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include <robotcontrol.h>

#include "rc_ext_fbus.h"
#include "rc_ext_pru.h"

#define FBUS_SHM_OFFSET (0x0080/sizeof(uint32_t))
#define MSG_TYPE_FBUS_CONFIG    0x10
#define MSG_TYPE_FBUS_SERVO     0x11
#define MSG_TYPE_FBUS_TELEMERTY 0x12
#define MSG_TYPE_SERVO_LIMIT    0x40

#define FBUS_DEFAULT_DEVICE_ID 0x67
#define FBUS_SERVO_UNMAPPED 0xFF


static volatile shm_fbus_t *shm_ptr = NULL;
static int init_flag=0;

typedef struct {
    message_t msg;
    uint8_t device_id;
} __attribute__((__packed__)) config_msg_t;

typedef struct {
    message_t msg;
    uint8_t map[FBUS_CHANNELS];
} __attribute__((__packed__)) servo_msg_t;

typedef struct {
    message_t msg;
    fbus_servo_limit_t limits[RC_SERVO_CH_MAX];
} __attribute__((__packed__)) servo_limit_message_t;

typedef struct {
    message_t msg;
    uint16_t app_id;
    uint32_t data;
} __attribute__((__packed__)) telemetry_msg_t;




static int send_telemetry(uint16_t app_id, uint32_t data) {
    telemetry_msg_t msg;
    msg.msg.type = MSG_TYPE_FBUS_TELEMERTY;
    msg.app_id = app_id;
    msg.data = data;
    return rc_ext_pru_send_message(&msg, sizeof(msg));
}


int rc_ext_fbus_init(void) {
    switch (rc_model_category()) {
        case CATEGORY_BEAGLEBONE: {
            volatile uint32_t *shared_mem_32bit_ptr = rc_pru_shared_mem_ptr();
            if(shared_mem_32bit_ptr==NULL){
                fprintf(stderr, "ERROR in rc_ext_fbus_init, failed to map shared memory pointer\n");
                init_flag=0;
                return -1;
            }
            shm_ptr = (volatile shm_fbus_t*)(shared_mem_32bit_ptr+FBUS_SHM_OFFSET);
            break;
        }
        default: {
            shm_ptr = malloc(sizeof(shm_fbus_t));
            memset((void*)shm_ptr, 0x00, sizeof(shm_fbus_t));
            for (int i=0; i<FBUS_CHANNELS; i++) {
                shm_ptr->channels[i] = (FBUS_CHANNEL_MAX-FBUS_CHANNEL_MIN)/2;
            }
            shm_ptr->channels[0] = 8;
            shm_ptr->channels[1] = 1234;
            break;
        }
    }

    init_flag=1;
    return 0;
}


void rc_ext_fbus_cleanup(void) {
    switch (rc_model_category()) {
        case CATEGORY_BEAGLEBONE:
        	shm_ptr = NULL;
            break;
        default:
            free((void*)shm_ptr);
            shm_ptr = NULL;
            break;
    }

	init_flag=0;
}

volatile shm_fbus_t *rc_ext_fbus_get_shm(void) {
    return shm_ptr;
}


void rc_ext_fbus_set_servo_map(const uint8_t map[FBUS_CHANNELS]) {
    servo_msg_t msg;
    msg.msg.type = MSG_TYPE_FBUS_SERVO;
    memcpy(msg.map, map, sizeof(msg.map));
    rc_ext_pru_send_message(&msg, sizeof(msg));
}


void rc_ext_fbus_set_servo_limit(const fbus_servo_limit_t limits[RC_SERVO_CH_MAX]) {
    servo_limit_message_t msg;
    msg.msg.type = MSG_TYPE_SERVO_LIMIT;
    memcpy(msg.limits, limits, sizeof(msg.limits));
    rc_ext_pru_send_message(&msg, sizeof(msg));
}


void rc_ext_fbus_send_telemetry(uint16_t app_id, uint32_t data) {
    telemetry_msg_t msg;
    msg.msg.type = MSG_TYPE_FBUS_TELEMERTY;
    msg.app_id = app_id;
    msg.data = data;
    rc_ext_pru_send_message(&msg, sizeof(msg));
}

