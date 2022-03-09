#include <stdlib.h>
#include <stdarg.h>

#include "debug.h"
#include "message.h"
#include "printf.h"

#include "sharedmem.h"

#define DEBUG_CHUNK_SIZE 64
#define DEBUG_CHUNK_MASK 0xF

static uint32_t next_chunk = 0;

static volatile shm_debug_t *shm_debug = SHM_DEBUG;

void debug_begin() {
    int i; 
    shm_debug->ctl = 0;
    for (i=0; i<sizeof(shm_debug->buffer); i++) {
        shm_debug->buffer[i] = '\0';
    }
}


void debug_print(const char *fmt, ...) {
#if 0
    va_list args;
    va_start(args, fmt);
#if 0
    message_t *msg = message_out_buffer();
    msg->type = MSG_TYPE_DEBUG;
    int len = vsnprintf_((char*)msg->data, RPMSG_BUF_SIZE, fmt, args);
    message_send(msg, sizeof(message_t)+len);
#else
    char *buf = (char*)(shm_debug->buffer+(DEBUG_CHUNK_SIZE*next_chunk));
    vsnprintf_(buf, DEBUG_CHUNK_SIZE, fmt, args);
    buf[DEBUG_CHUNK_SIZE-1] = '\0';
    shm_debug->ctl = next_chunk;
    next_chunk = (next_chunk+1) & DEBUG_CHUNK_MASK;
#endif
    va_end(args);
#endif
}

