#!/usr/bin/env python3

import sys
import logging
from math import pi as PI
sys.path.append('build')

from time import sleep
from build.robotcontrol import Robot, TelemetryListener, DriveMode, LEDColorLayer, LEDAnimation

from datetime import datetime

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")


robot = Robot()

robot.init()


motor0 = robot.motor_control.motors[0]
gimbal0 = motor0.gimbal

motor1 = robot.motor_control.motors[1]
gimbal1 = motor1.gimbal

motor2 = robot.motor_control.motors[2]
gimbal2 = motor2.gimbal

motor3 = robot.motor_control.motors[3]
gimbal3 = motor3.gimbal
