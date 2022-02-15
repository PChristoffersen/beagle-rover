#!/usr/bin/env python3

import sys
import logging
import json
sys.path.append('../build')

from time import sleep
from robotsystem import Robot, Telemetry

from datetime import datetime

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")



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

    motors = robot.motor_control.motors
    telemetry = robot.telemetry

    try:
        while True:
            motors[0].servo.angle = 20.0
            log.info(json.dumps(telemetry.values, indent=4))
            #log.info(telemetry.values)
            sleep(5)
            motors[0].servo.angle = 00.0
            log.info(json.dumps(telemetry.values, indent=4))
            #log.info(telemetry.values)
            sleep(5)

    except KeyboardInterrupt:
        print("Shutdown")

    telemetry = None
    motors = None

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
