#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <rc/pru.h>
#include <rc/time.h>
#include <rc/encoder_eqep.h>

#include "rc_ext_pru.h"
#include "rc_ext_encoder.h"

#define MSG_TYPE_ENCODER_START  0x30
#define MSG_TYPE_ENCODER_STOP   0x31

#define ENCODER_MEM_OFFSET	16

// pru shared memory pointer
static volatile unsigned int* shared_mem_32bit_ptr = NULL;
static int init_flag=0;

static int encoder_stop() {
	message_t msg;
	msg.type = MSG_TYPE_ENCODER_STOP;
	return rc_ext_pru_send_message(&msg, sizeof(msg));
}

static int encoder_start() {
	message_t msg;
	msg.type = MSG_TYPE_ENCODER_START;
	return rc_ext_pru_send_message(&msg, sizeof(msg));
}



void rc_ext_encoder_pru_cleanup(void)
{
	encoder_stop();

	// zero out shared memory
	if(shared_mem_32bit_ptr != NULL){
		shared_mem_32bit_ptr[ENCODER_MEM_OFFSET]=0;
	}
#if 0
	rc_pru_stop(ENCODER_PRU_CH);
#endif
	shared_mem_32bit_ptr = NULL;
	init_flag=0;
	return;
}

int rc_ext_encoder_pru_init(void)
{
	int i;
	// map memory
	shared_mem_32bit_ptr = rc_pru_shared_mem_ptr();
	if(shared_mem_32bit_ptr==NULL){
		fprintf(stderr, "ERROR in rc_ext_encoder_pru_init, failed to map shared memory pointer\n");
		init_flag=0;
		return -1;
	}

	// set first channel to be nonzero, PRU binary will zero this out later
	if (encoder_stop()<0) {
		return -1;
	}
	shared_mem_32bit_ptr[ENCODER_MEM_OFFSET]=42;

	// make sure memory actually got zero'd out
	if (encoder_start()<0) {
		return -1;
	}
	for(i=0;i<40;i++){
		if(shared_mem_32bit_ptr[ENCODER_MEM_OFFSET]==0){
			init_flag=1;
			return 0;
		}
		rc_usleep(100000);
	}

	fprintf(stderr, "ERROR in rc_ext_encoder_pru_init, failed to wait for start\n");

	init_flag=0;
	return -1;
}




int32_t rc_ext_encoder_pru_read(void)
{
	if(shared_mem_32bit_ptr==NULL || init_flag==0){
		fprintf(stderr, "ERROR in rc_ext_encoder_pru_read, call rc_encoder_pru_init first\n");
		return -1;
	}
	return (int32_t)shared_mem_32bit_ptr[ENCODER_MEM_OFFSET];
}


int rc_ext_encoder_pru_write(int pos)
{
	if(shared_mem_32bit_ptr==NULL || init_flag==0){
		fprintf(stderr, "ERROR in rc_ext_encoder_pru_write, call rc_encoder_pru_init first\n");
		return -1;
	}
	shared_mem_32bit_ptr[ENCODER_MEM_OFFSET] = pos;
	return 0;
}




int rc_ext_encoder_init(void)
{
	if(rc_encoder_eqep_init()){
		fprintf(stderr,"ERROR: failed to run rc_encoder_eqep_init\n");
		return -1;
	}
	if(rc_ext_encoder_pru_init()){
		fprintf(stderr,"ERROR: failed to run rc_encoder_pru_init\n");
		return -1;
	}
	return 0;
}

int rc_ext_encoder_cleanup(void)
{
	rc_encoder_eqep_cleanup();
	rc_ext_encoder_pru_cleanup();
	return 0;
}



int32_t rc_ext_encoder_read(int ch)
{
	// sanity check
	if(ch<1 || ch >4){
		fprintf(stderr, "ERROR in rc_encoder_read, channel must be between 1 and 4\n");
		return -1;
	}
	if(ch==4) return rc_ext_encoder_pru_read();
	return rc_encoder_eqep_read(ch);
}

int rc_ext_encoder_write(int ch, int value)
{
	// sanity check1
	if(ch<1 || ch >4){
		fprintf(stderr, "ERROR in rc_encoder_write, channel must be between 1 and 4\n");
		return -1;
	}
	if(ch==4) return rc_ext_encoder_pru_write(value);
	return rc_encoder_eqep_write(ch,value);
}

