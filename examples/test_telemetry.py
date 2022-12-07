#!/usr/bin/env python3

import sys
import logging
from pathlib import Path
sys.path.append(str(Path(__file__).parent / '../build'))

#import json
from time import sleep
from beaglerover import Robot, Telemetry

from datetime import datetime

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")



def main():
    robot = Robot()

    robot.init()

    motors = robot.motor_control.motors
    telemetry = robot.telemetry

    try:
        sleep(1)
        #log.info(telemetry.history_imu)
        #log.info(telemetry.history_motor_duty)
        log.info(telemetry.odometer)
        #log.info(telemetry.test)


    except KeyboardInterrupt:
        log.info("Shutdown")

    telemetry = None
    motors = None

    robot.cleanup()
    robot = None
    log.info("Done")


    return


if __name__ == '__main__':
    main()
