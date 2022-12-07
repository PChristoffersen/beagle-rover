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

    robot.kinematic.drive_mode = DriveMode.SKID
    #robot.kinematic.drive_mode = DriveMode.SPINNING
    #robot.kinematic.drive_mode = DriveMode.BALANCING
    #robot.kinematic.drive_mode = DriveMode.NONE

    log.info("Main ----------------------------------------")

    robot.input.manual.steer(0.0,  0.0)
    robot.input.manual.steer(0.0,  1.0)
    robot.input.manual.steer(0.0, -1.0)

    log.info("")

    log.info("Zero Throttle:")
    robot.input.manual.steer(-1.0,  0.0)
    robot.input.manual.steer(-0.5,  0.0)
    robot.input.manual.steer( 0.0,  0.0)
    robot.input.manual.steer( 0.5,  0.0)
    robot.input.manual.steer( 1.0,  0.0)

    log.info("Full Throttle:")
    robot.input.manual.steer(-1.0,  1.0)
    robot.input.manual.steer(-0.5,  1.0)
    robot.input.manual.steer( 0.0,  1.0)
    robot.input.manual.steer( 0.5,  1.0)
    robot.input.manual.steer( 1.0,  1.0)

    #log.info("")
    robot.input.manual.steer( 1.0,  0.5)
    robot.input.manual.steer(-1.0,  0.5)
    robot.input.manual.steer( 1.0, -0.5)
    robot.input.manual.steer(-1.0, -0.5)

    #log.info("")
    robot.input.manual.steer( 0.5,  1.0)
    robot.input.manual.steer(-0.5,  1.0)
    robot.input.manual.steer( 0.5, -1.0)
    robot.input.manual.steer(-0.5, -1.0)


    log.info("Cleanup ----------------------------------------")

    robot.cleanup()
    robot = None
    log.info("Done")


    return


if __name__ == '__main__':
    main()
