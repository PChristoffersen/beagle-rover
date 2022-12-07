#!/usr/bin/env python3

import sys
import logging
from pathlib import Path
sys.path.append(str(Path(__file__).parent.parent / 'build'))

print(sys.path)

from time import sleep
from beaglerover import Robot, DriveMode

from datetime import datetime

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")



def main():
    robot = Robot()
    robot.init()

    log.info(f"{robot.properties}")

    log.info(f"{robot.properties.groups()}")
    log.info(f"{robot.properties['test1']}")
    robot.properties['test1'] = {
        "value_str": "flop",
        "value_bool": False,
        "value_int": 56,
        "value_float": 3.1415,
    }
    log.info(f"{robot.properties['test1']}")

    sleep(1)
    
    robot.cleanup()
    robot = None


if __name__ == '__main__':
    main()
