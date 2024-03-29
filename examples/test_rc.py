#!/usr/bin/env python3

import sys
import logging
sys.path.append('../build')

from time import sleep
from beaglerover import Robot

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")



def main():
    robot = Robot()
    robot.init()
    rc_receiver = robot.rc_receiver

    rc_receiver.enabled = True

    sleep(1)

    log.info(f"{rc_receiver.channels}")
    return


    rc_receiver.enabled = True

    sleep(6)

    try:
        while True:
            sleep(1)
    except KeyboardInterrupt:
        log.info("Shutdown")

    rc_receiver.enabled = False
    rc_receiver = None

    robot.cleanup()
    robot = None
    log.info("Done")



if __name__ == '__main__':
    main()
