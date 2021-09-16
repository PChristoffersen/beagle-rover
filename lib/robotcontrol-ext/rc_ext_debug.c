#include <stdint.h>
#include <robotcontrol.h>

#include "rc_ext_debug.h"

#define DEBUG_OFFSET 0x0100
#define DEBUG_BUFFER_SIZE 1024
#define DEBUG_CHUNK_SIZE 64
#define DEBUG_CHUNK_MASK 0xF


typedef struct {
    volatile uint32_t ctl;
    volatile uint8_t  buffer[DEBUG_BUFFER_SIZE];
} __attribute__((__packed__)) shm_debug_t;

// pru shared memory pointer
static volatile shm_debug_t* shared_mem_ptr = NULL;
static uint32_t debug_ctl_last = 0;
static int init_flag=0;


int rc_ext_debug_init() {
  	volatile uint8_t *ptr = (uint8_t*)rc_pru_shared_mem_ptr();
	if(ptr==NULL){
		fprintf(stderr, "ERROR in rc_ext_debug_init, failed to map shared memory pointer\n");
		init_flag=0;
		return -1;
	}

    shared_mem_ptr = (volatile shm_debug_t*)(ptr+DEBUG_OFFSET);

    init_flag=1;
    return 0;
}


void rc_ext_debug_cleanup() {
	shared_mem_ptr = NULL;
	init_flag=0;
	return;
}


const char *rc_ext_debug_next() {
    uint32_t ctl = shared_mem_ptr->ctl;
    if (ctl==debug_ctl_last) {
        return NULL;
    }
    const char *msg = (const char*)(shared_mem_ptr->buffer+(ctl*DEBUG_CHUNK_SIZE));
    debug_ctl_last = (debug_ctl_last + 1) & DEBUG_CHUNK_MASK;
    return msg;
}
