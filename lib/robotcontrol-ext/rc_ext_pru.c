#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

#include <robotcontrol.h>

#include "rc_ext_pru.h"

#define EXT_PRU_CH		0 // PRU0
#define EXT_PRU_FW		"am335x-pru0-rc-ext-fbus-fw"
#define EXT_PRU_DEVICE "/dev/rpmsg_pru30"


static int initialized = 0;
static int pru_fd = -1;
static pthread_mutex_t pru_mutex = PTHREAD_MUTEX_INITIALIZER;


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

    initialized = 1;
	return 0;
}


int rc_ext_pru_init(void) {
    if (pru_fd!=-1) {
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

	// start pru
	if(rc_pru_start(EXT_PRU_CH, EXT_PRU_FW)){
		fprintf(stderr,"ERROR in rc_ext_pru_init, failed to start PRU%d\n", EXT_PRU_CH);
		return -1;
	}

    // Wait for PRU to get ready
    int dev_found = 0;
	for(int i=0;i<40;i++) {
        struct stat st;
        if (stat(EXT_PRU_DEVICE, &st)==0 && (st.st_mode&S_IFMT)==S_IFCHR) {
            dev_found = 1;
            break;
        }
 		rc_usleep(100000);
	}
    if (!dev_found) {
		fprintf(stderr,"Error finding: %s\n", EXT_PRU_DEVICE);
        return -1;
    }

  	errno = 0;
    pru_fd = open(EXT_PRU_DEVICE, O_RDWR);
	if(pru_fd==-1){
		perror("ERORR opening PRU0 driver");
		fprintf(stderr,"can't open: %s\n", EXT_PRU_DEVICE);
		return -1;
	}

    initialized = 1;
    return 0;
}


void rc_ext_pru_cleanup(void) {
    if (pru_fd!=-1) {
        close(pru_fd);
        pru_fd = -1;
    }

    rc_pru_stop(EXT_PRU_CH);

    pthread_mutex_destroy(&pru_mutex);

    initialized = 0;
}


int rc_ext_pru_send_message(void *data, size_t size) {
    pthread_mutex_lock( &pru_mutex );
    int ret = write(pru_fd, data, size);
    if (ret<0) {
   		perror("ERROR in rc_ext_pru, failed to write to PRU device");
        pthread_mutex_unlock( &pru_mutex );
        return -1;
    }
    pthread_mutex_unlock( &pru_mutex );
    return 0;
}
