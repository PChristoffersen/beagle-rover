#!/usr/bin/env python3

import sys
import logging
from math import pi as PI
from pathlib import Path
sys.path.append(str(Path(__file__).parent / '../build'))

from time import sleep
from beaglerover import Robot, DriveMode, LEDColorLayer, LEDAnimation, DriveMode, InputSource, Orientation

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

kinematic = robot.kinematic
input = robot.input

input.source = InputSource.MANUAL


kinematic.drive_mode = DriveMode.ALL
#kinematic.drive_mode = DriveMode.FRONT
#kinematic.drive_mode = DriveMode.REAR
#kinematic.drive_mode = DriveMode.SKID
#kinematic.drive_mode = DriveMode.SPINNING
#kinematic.drive_mode = DriveMode.BALANCING
#kinematic.drive_mode = DriveMode.PASSTHROUGH

#kinematic.orientation = Orientation.NORTH
#kinematic.orientation = Orientation.SOUTH
#kinematic.orientation = Orientation.EAST
kinematic.orientation = Orientation.WEST


