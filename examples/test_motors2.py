#!/usr/bin/env python3

import sys
import logging
from math import pi as PI
sys.path.append('../build')

from time import sleep
from beaglerover import Robot, TelemetryListener, DriveMode

from datetime import datetime

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")



def main():
    robot = Robot()

    robot.init()

    control = robot.motor_control
    motors = control.motors

    for motor in robot.motor_control.motors:
        log.info("Enabling "+str(motor))
        motor.enabled = True


    motors[0].target_rpm = 100.0

    try:
        while True:
            sleep(5)
    except KeyboardInterrupt:
        log.info("Shutdown")

    control = None
    motors = None

    robot.cleanup()
    robot = None
    log.info("Done")


    return


if __name__ == '__main__':
    main()
