#!/usr/bin/env python3

import sys
import logging
sys.path.append('../build')

from time import sleep
from robotsystem import Robot, TelemetryListener, DriveMode, InputSource

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")


def main():
    robot = Robot()

    log.info("Init ----------------------------------------")
    robot.init()

    #robot.kinematic.drive_mode = DriveMode.FRONT
    robot.kinematic.drive_mode = DriveMode.SKID
    #robot.kinematic.drive_mode = DriveMode.SPINNING
    #robot.kinematic.drive_mode = DriveMode.BALANCING
    #robot.kinematic.drive_mode = DriveMode.NONE
    #robot.kinematic.drive_mode = DriveMode.IDLE

    log.info("Main ----------------------------------------")

    #sleep(1)
    #for i in range(10):
    #    for s in range(-100, 101):
    #        robot.kinematic.test(s/100.0)
    #        sleep(0.005)
    #    sleep(1)
    #    for s in range(100, -101, -1):
    #        robot.kinematic.test(s/100.0)
    #        sleep(0.005)
    #    sleep(1)

    #robot.input.manual.steer(-1.0, 0.0)
    #sleep(5)
    #robot.input.manual.steer(0.0, 0.0)
    #sleep(2)
    #robot.input.manual.steer(1.0, 0.0)
    #sleep(5)

    robot.rc_receiver.enabled = True
    robot.input.source = InputSource.RC

    try:
        while True:
            sleep(1)
    except KeyboardInterrupt:
        print("Shutdown")

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
