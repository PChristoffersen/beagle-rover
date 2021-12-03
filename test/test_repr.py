#!/usr/bin/env python3

import sys
import logging
from math import pi as PI
sys.path.append('../build')

from time import sleep
from robotsystem import Robot, TelemetryListener, DriveMode, LEDColorLayer, LEDAnimation, DriveMode, InputSource

from datetime import datetime

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")

robot = Robot()
robot.init()

motor0 = robot.motor_control.motors[0]
servo0 = motor0.servo

motor1 = robot.motor_control.motors[1]
servo1 = motor1.servo

motor2 = robot.motor_control.motors[2]
servo2 = motor2.servo

motor3 = robot.motor_control.motors[3]
servo3 = motor3.servo

robot.kinematic.drive_mode = DriveMode.STANDARD

input = robot.input
