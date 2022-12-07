#!/usr/bin/env python3

import sys
import logging
sys.path.append('../build')

from time import sleep
from beaglerover import Robot, DriveMode

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")


def main():
    robot = Robot()

    log.info("Init ----------------------------------------")
    robot.init()

    sleep(5)

    #robot.kinematic.drive_mode = DriveMode.STANDARD
    robot.kinematic.drive_mode = DriveMode.SPINNING
    #robot.kinematic.drive_mode = DriveMode.BALANCING
    #robot.kinematic.drive_mode = DriveMode.NONE
    #robot.kinematic.drive_mode = DriveMode.IDLE

    log.info("Main ----------------------------------------")

    sleep(1)
    for i in range(10):
        sleep(1)

    #robot.kinematic.test(0.5)
    #sleep(5)
    #robot.kinematic.test(1.0)
    #sleep(5)
    #robot.kinematic.test(-1.0)
    #sleep(5)

    #try:
    #    sleep(1)
    #except KeyboardInterrupt:
    #    print("Shutdown")

    log.info("Cleanup ----------------------------------------")
    robot.kinematic.drive_mode = DriveMode.NONE
    sleep(2)
    robot.cleanup()
    robot = None
    log.info("Done")


    return


if __name__ == '__main__':
    main()
