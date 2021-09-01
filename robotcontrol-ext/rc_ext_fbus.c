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
    uint16_t app_id;
    uint32_t data;
} __attribute__((__packed__)) telemetry_msg_t;




static int send_message(void *msg, size_t len) {
    int fd = rc_ext_pru_fd();
    int ret = write(fd, msg, len);
    if (ret<0) {
   		perror("ERROR in rc_ext_fbus, failed to write to PRU device");
        return -1;
    }
    return 0;
}


static int send_telemetry(uint16_t app_id, uint32_t data) {
    telemetry_msg_t msg;
    msg.msg.type = MSG_TYPE_FBUS_TELEMERTY;
    msg.app_id = app_id;
    msg.data = data;
    return send_message(&msg, sizeof(msg));
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

volatile shm_fbus_t *rc_ext_fbus_get_channels(void) {
    return shm_ptr;
}


void rc_ext_fbus_set_servo_map(uint8_t map[FBUS_CHANNELS]) {
    servo_msg_t msg;
    msg.msg.type = MSG_TYPE_FBUS_SERVO;
    memcpy(msg.map, map, sizeof(msg.map));
    send_message(&msg, sizeof(msg));
}


void rc_ext_fbus_telemetry_send_battery(uint8_t id, float *cells, uint8_t n_cells) {
    for (int i=0; i<n_cells; i+=2) {
        uint32_t cv1 = cells[i]*500;
        uint32_t cv2 = 0;
        
        if (i+1<n_cells) {
            cv2 = cells[i]*500;
        }
        
        uint32_t data = ((uint32_t) cv1 & 0x0fff) << 20 | ((uint32_t) cv2 & 0x0fff) << 8 | n_cells << 4 | id;
        send_telemetry(0x0300 + i/2, data);
    }
}
