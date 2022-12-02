# Target operating system name.
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING TRUE)

# Name of C compiler.
set(CMAKE_AR            "/usr/bin/arm-linux-gnueabihf-ar")
set(CMAKE_ASM_COMPILER  "/usr/bin/arm-linux-gnueabihf-as")
set(CMAKE_C_COMPILER    "/usr/bin/arm-linux-gnueabihf-gcc-8" CACHE STRING "C Compiler")
set(CMAKE_CXX_COMPILER  "/usr/bin/arm-linux-gnueabihf-g++-8" CACHE STRING "C++ Compiler")
set(CMAKE_LINKER        "/usr/bin/arm-linux-gnueabihf-ld" )
set(CMAKE_OBJCOPY       "/usr/bin/arm-linux-gnueabihf-objcopy")
set(CMAKE_RANLIB        "/usr/bin/arm-linux-gnueabihf-ranlib")
set(CMAKE_SIZE          "/usr/bin/arm-linux-gnueabihf-size")
set(CMAKE_STRIP         "/usr/bin/arm-linux-gnueabihf-strip")

set(CMAKE_EXE_LINKER_FLAGS    "-lrt" CACHE STRING "executable linker flags")

set(CMAKE_SYSROOT "/opt/beaglebone/am33xx-debian-10.3-iot-armhf")
#SET(CMAKE_FIND_ROOT_PATH "/opt/rootfs/am33xx-debian-10.3-iot-armhf")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

