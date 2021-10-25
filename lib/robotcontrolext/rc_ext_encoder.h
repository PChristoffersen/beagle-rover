#ifndef _RC_EXT_ENCODER_H_
#define _RC_EXT_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

int rc_ext_encoder_init(void);
int rc_ext_encoder_cleanup(void);
int rc_ext_encoder_read(int ch);
int rc_ext_encoder_write(int ch, int pos);

#ifdef __cplusplus
}
#endif

#endif
