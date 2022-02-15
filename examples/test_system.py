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

    with robot.power.system as system:
        logger.info(f"Power: {system}, {type(system)} {system.name}")
        logger.info(f"System power type: {system.type}")

    robot.cleanup()
    robot = None
    logger.info("Done")



if __name__ == '__main__':
    main()
