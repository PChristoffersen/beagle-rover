#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#include <robotcontrol.h>

#include "rc_ext_pru.h"

#define EXT_PRU_CH		0 // PRU0
#define EXT_PRU_FW		"am335x-pru0-fbus-fw"
#define EXT_PRU_DEVICE "/dev/rpmsg_pru30"


static int g_pru_fd = -1;


static int set_pinmux(const char *dev, const char *state) {
	errno=0;
	int fd = open(dev, O_WRONLY);
	if(fd==-1){
		perror("ERORR opening pinmux driver");
		fprintf(stderr,"can't open: %s\n",dev);
		return -1;
	}

	int ret = write(fd, state, strlen(state));
	if(ret<0){
		perror("ERROR in rc_pinmux_set, failed to write to pinmux driver");
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}


int rc_ext_pru_init(void) {
    if (g_pru_fd!=-1) {
        return 0;
    }

    // Setup pin mux
    switch (rc_model()) {
        case MODEL_BB_BLUE:
            // LED (GPIO3_17)
            if (set_pinmux("/sys/devices/platform/ocp/ocp:P9_28_pinmux/state", "pruout")!=0) return -1;

            // UART1 to PRU
            if (set_pinmux("/sys/devices/platform/ocp/ocp:P9_24_pinmux/state", "pru_uart")!=0) return -1;
            if (set_pinmux("/sys/devices/platform/ocp/ocp:P9_26_pinmux/state", "pru_uart")!=0) return -1;
            break;

        case MODEL_BB_POCKET: 
            // LED (PRU0_5)
            if (set_pinmux("/sys/devices/platform/ocp/ocp:P9_28_pinmux/state", "pruout")!=0) return -1;

            // UART1 to PRU
            if (set_pinmux("/sys/devices/platform/ocp/ocp:P2_09_pinmux/state", "pru_uart")!=0) return -1;
            if (set_pinmux("/sys/devices/platform/ocp/ocp:P2_11_pinmux/state", "pru_uart")!=0) return -1;
            break;
    }

#if 0
	// start pru
	if(rc_pru_start(EXT_PRU_CH, EXT_PRU_FW)){
		fprintf(stderr,"ERROR in rc_ext_pru_init, failed to start PRU%d\n", EXT_PRU_CH);
		return -1;
	}
#endif

#if 1
    // Wait for PRU to get ready
  	errno = 0;
    g_pru_fd = open(EXT_PRU_DEVICE, O_RDWR);
	if(g_pru_fd==-1){
		perror("ERORR opening PRU0 driver");
		fprintf(stderr,"can't open: %s\n", EXT_PRU_DEVICE);
		return -1;
	}
#endif

    return 0;
}


void rc_ext_pru_cleanup(void) {
    if (g_pru_fd!=-1) {
        close(g_pru_fd);
        g_pru_fd = -1;
    }

    // TODO
    #if 0
    rc_pru_stop(EXT_PRU)
    #endif
}


int rc_ext_pru_fd(void) {
    return g_pru_fd;
}
