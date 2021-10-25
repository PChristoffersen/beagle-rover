#ifndef _RC_EXT_DEBUG_H_
#define _RC_EXT_DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

int rc_ext_debug_init();
void rc_ext_debug_cleanup();
volatile const char *rc_ext_debug_next();

#ifdef __cplusplus
}
#endif

#endif 

