# PRU_CGT environment variable must point to the TI PRU code gen tools directory. E.g.:
#(Desktop Linux) export PRU_CGT=/path/to/pru/code/gen/tools/ti-cgt-pru_2.1.2
#(Windows) set PRU_CGT=C:/path/to/pru/code/gen/tools/ti-cgt-pru_2.1.2
#(ARM Linux*) export PRU_CGT=/usr/share/ti/cgt-pru
#
# *ARM Linux also needs to create a symbolic link to the /usr/bin/ directory in
# order to use the same Makefile
#(ARM Linux) ln -s /usr/bin/ /usr/share/ti/cgt-pru/bin

ifndef PRU_CGT
define ERROR_BODY

*******************************************************************************
PRU_CGT environment variable is not set. Examples given:
(Desktop Linux) export PRU_CGT=/path/to/pru/code/gen/tools/ti-cgt-pru_2.1.2
(Windows) set PRU_CGT=C:/path/to/pru/code/gen/tools/ti-cgt-pru_2.1.2
(ARM Linux*) export PRU_CGT=/usr/share/ti/cgt-pru

*ARM Linux also needs to create a symbolic link to the /usr/bin/ directory in
order to use the same Makefile
(ARM Linux) ln -s /usr/bin/ /usr/share/ti/cgt-pru/bin
*******************************************************************************

endef
$(error $(ERROR_BODY))
endif


ifndef PRU_SUPPORT
define ERROR_BODY

*******************************************************************************
PRU_SUPPORT environment variable is not set. Examples given:
(Desktop Linux) export PRU_SUPPORT=/path/to/pru/support/package/pru-software-support-package-5.8.0
(Windows) set PRU_CGT=C:/path/to/pru/support/package/pru-software-support-package-5.8.0
(ARM Linux*) export PRU_CGT=/usr/lib/ti/pru-software-support-package
*******************************************************************************

endef
$(error $(ERROR_BODY))
endif


FW_POSTFIX:=rc-ext-fbus-fw

MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))
PROJ_NAME=$(CURRENT_DIR)
LINKER_COMMAND_FILE=./AM335x_PRU.cmd
LIBS=--library=$(PRU_SUPPORT)/lib/rpmsg_lib.lib
INCLUDE=--include_path=$(PRU_SUPPORT)/include --include_path=$(PRU_SUPPORT)/include/am335x
STACK_SIZE=0x100
HEAP_SIZE=0x100
GEN_DIR=gen

PRU0:=/dev/remoteproc/pruss-core0
PRU1:=/dev/remoteproc/pruss-core1

#Common compiler and linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
CFLAGS=-v3 -O2 --display_error_number --endian=little --hardware_mac=on --obj_directory=$(GEN_DIR) --pp_directory=$(GEN_DIR) -ppd -ppa
#Linker flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
LFLAGS=--reread_libs --warn_sections --stack_size=$(STACK_SIZE) --heap_size=$(HEAP_SIZE)

TARGET=$(GEN_DIR)/$(PROJ_NAME).out
MAP=$(GEN_DIR)/$(PROJ_NAME).map
SOURCES=$(wildcard *.c)
HEADERS=$(wildcard *.h)
#Using .object instead of .obj in order to not conflict with the CCS build process
OBJECTS=$(patsubst %,$(GEN_DIR)/%,$(SOURCES:.c=.object))

all: printStart $(TARGET) printEnd

printStart:
	@echo ''
	@echo '************************************************************'
	@echo 'Building project: $(PROJ_NAME)'

printEnd:
	@echo ''
	@echo 'Output files can be found in the "$(GEN_DIR)" directory'
	@echo ''
	@echo 'Finished building project: $(PROJ_NAME)'
	@echo '************************************************************'
	@echo ''

install_PRU0:
	@echo ''
	@echo 'Stopping current PRU0 application ($(PRU0))'
	@echo stop | sudo tee $(PRU0)/state || echo Cannot stop $(PRU0)
	@echo 'Installing firmware'
	@sudo cp $(TARGET) /lib/firmware/am335x-pru0-$(FW_POSTFIX)
	@echo 'Deploying firmware'
	@echo am335x-pru0-$(FW_POSTFIX) | sudo tee $(PRU0)/firmware
	@echo 'Starting new PRU0 application'
	@echo start | sudo tee $(PRU0)/state

install_PRU1:
	@echo ''
	@echo 'Stopping current PRU1 application ($(PRU1))'
	@echo stop | sudo tee $(PRU1)/state || echo Cannot stop $(PRU1)
	@echo 'Installing firmware'
	@sudo cp $(TARGET) /lib/firmware/am335x-pru1-$(FW_POSTFIX)
	@echo 'Deploying firmware'
	@echo am335x-pru1-$(FW_POSTFIX) | sudo tee $(PRU1)/firmware
	@echo 'Starting new PRU1 application'
	@echo start | sudo tee $(PRU1)/state

stop_PRU0:
	@echo ''
	@echo 'Stopping current PRU0 application ($(PRU0))'
	@echo stop | sudo tee $(PRU0)/state || echo Cannot stop $(PRU0)

stop_PRU1:
	@echo ''
	@echo 'Stopping current PRU1 application ($(PRU1))'
	@echo stop | sudo tee $(PRU1)/state || echo Cannot stop $(PRU1)

# Invokes the linker (-z flag) to make the .out file
$(TARGET): $(OBJECTS) $(LINKER_COMMAND_FILE)
	@echo ''
	@echo 'Building target: $@'
	@echo 'Invoking: PRU Linker'
	$(PRU_CGT)/bin/clpru $(CFLAGS) -z -i$(PRU_CGT)/lib -i$(PRU_CGT)/include $(LFLAGS) -o $(TARGET) $(OBJECTS) -m$(MAP) $(LINKER_COMMAND_FILE) --library=libc.a $(LIBS)
	@echo 'Finished building target: $@'

# Invokes the compiler on all c files in the directory to create the object files
$(GEN_DIR)/%.object: %.c $(HEADERS)
	@mkdir -p $(GEN_DIR)
	@echo ''
	@echo 'Building file: $<'
	@echo 'Invoking: PRU Compiler'
	$(PRU_CGT)/bin/clpru --include_path=$(PRU_CGT)/include $(INCLUDE) $(CFLAGS) -fe $@ $<

.PHONY: all clean

# Remove the $(GEN_DIR) directory
clean:
	@echo ''
	@echo '************************************************************'
	@echo 'Cleaning project: $(PROJ_NAME)'
	@echo ''
	@echo 'Removing files in the "$(GEN_DIR)" directory'
	@rm -rf $(GEN_DIR)
	@echo ''
	@echo 'Finished cleaning project: $(PROJ_NAME)'
	@echo '************************************************************'
	@echo ''

# Includes the dependencies that the compiler creates (-ppd and -ppa flags)
-include $(OBJECTS:%.object=%.pp)

