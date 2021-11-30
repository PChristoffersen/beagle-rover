#!/usr/bin/env python3

import sys
import logging
from math import pi as PI
sys.path.append('../build')

from time import sleep
from build.robotcontrol import Robot, TelemetryListener, DriveMode

from datetime import datetime

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")


def set_motors(robot: Robot, duty):
    motors = robot.motor_control.motors

    for motor in motors:
        motor.duty = duty


def set_servos(robot: Robot, dir: bool):
    motors = robot.motor_control.motors

    if dir:
        motors[0].servo.angle = -45.0
        motors[1].servo.angle = 45.0
        motors[2].servo.angle = 45.0
        motors[3].servo.angle = -45.0
    else:
        motors[0].servo.angle = 45.0
        motors[1].servo.angle = -45.0
        motors[2].servo.angle = -45.0
        motors[3].servo.angle = 45.0



def main():
    robot = Robot()

    robot.init()

    for motor in robot.motor_control.motors:
        log.info("Enabling "+str(motor))
        motor.servo.enabled = True
        motor.enabled = True
    #test_motor_angles(robot)

    for motor in robot.motor_control.motors:
        motor.duty = 0.0
        motor.servo.angle = 0.0
    sleep(10)

    try:
        toggle = 0
        while True:
            set_servos(robot, False)
            sleep(2)
            set_motors(robot, 0.7)
            sleep(3)
            set_motors(robot, 0.0)
            sleep(2)
            set_motors(robot, -0.3)
            sleep(3)
            set_motors(robot, 0.0)
            sleep(1)

            set_servos(robot, True)
            sleep(2)
            set_motors(robot, 0.7)
            sleep(3)
            set_motors(robot, 0.0)
            sleep(2)
            set_motors(robot, -0.3)
            sleep(3)
            set_motors(robot, 0.0)
            sleep(1)

    except KeyboardInterrupt:
        print("Shutdown")

    for motor in robot.motor_control.motors:
        motor.duty = 0.0
        motor.servo.angle = 0.0
    sleep(1)

    robot.cleanup()
    robot = None
    print("Done")


    return


if __name__ == '__main__':
    main()
