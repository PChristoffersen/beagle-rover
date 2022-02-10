#!/usr/bin/env python3

import sys
import logging
sys.path.append('../build')

from time import sleep
from robotsystem import Robot

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%H:%M:%S', level=logging.DEBUG)
logger = logging.getLogger("test")



def main():
    robot = Robot()
    robot.init()

    motor = robot.motor_control.motors[0]

    sub = motor.subscribe()

    logger.info(f"Created subscription {sub}")

    try:
        while True:
            sleep(1)
            motor.enabled = True
            logger.info(f"Notifications: {sub.read()}")
            sleep(1)
            motor.duty = 1.0
            logger.info(f"Notifications: {sub.read()}")
            sleep(1)
            motor.enabled = False
            logger.info(f"Notifications: {sub.read()}")

            break

    except KeyboardInterrupt:
        logger.info("Shutdown")

    logger.info(f"Notifications: {sub.read()}")

    logger.info("Clear: sub")
    sub.unsubscribe()
    sub = None
    logger.info("Clear: motor")
    motor = None

    robot.cleanup()
    robot = None
    logger.info("Done")



if __name__ == '__main__':
    main()
