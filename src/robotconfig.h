#ifndef _ROBOT_CONFIG_H_
#define _ROBOT_CONFIG_H_

#include <cmath>
#include <algorithm>

#include <config.h>

#define ROBOT_PLATFORM_PC 0
#define ROBOT_PLATFORM_BEAGLEBONE 1
#define ROBOT_PLATFORM_RASPBERRY 2

#define ROBOT_BOARD_PC 0
#define ROBOT_BOARD_BEAGLEBONE_BLUE 10
#define ROBOT_BOARD_BEAGLEBONE_POCKET 11


#if defined(__arm__)
#ifndef HAVE_ROBOTCONTROL
#error "Beaglebone builds requires the robotcontrol library"
#endif
#include <robotcontrol.h>
#include <robotcontrolext.h>

#include <configs/board-beaglebone-blue.h>
//#include <configs/board-beaglebone-pocket.h>

#include <configs/chassis-beaglerover.h>



#elif defined(__x86_64)
#ifdef HAVE_ROBOTCONTROL
#include <robotcontrol.h>
#endif
#include <configs/board-pc.h>
#include <configs/chassis-beaglerover.h>

#else
#error "Unsupported platform"
#endif

#endif
