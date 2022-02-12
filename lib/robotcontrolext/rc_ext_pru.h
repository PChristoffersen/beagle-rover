#ifndef _RC_EXT_PRU_H_
#define _RC_EXT_PRU_H_

#include <stdint.h>
#include <stddef.h> 

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t type;
    uint8_t _reserved[3];
} __attribute__((__packed__)) message_t;


int rc_ext_pru_init(void);
void rc_ext_pru_cleanup(void);
int rc_ext_pru_send_message(void *data, size_t size);


#ifdef __cplusplus
}
#endif

#endif 

