#!/usr/bin/env python3

import sys
import logging
from math import pi as PI
from pathlib import Path
sys.path.append(str(Path(__file__).parent / '../build'))

from time import sleep
from beaglerover import Robot, DriveMode, LEDColorLayer, LEDAnimation, DriveMode, InputSource, Orientation

from datetime import datetime

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")

robot = Robot()
robot.init()

sleep(2)


for interface in robot.network.interfaces:
    log.info("--------------------------")
    log.info(f"{interface}")
    log.info(f"    name: {interface.name}")
    log.info(f"    type: {interface.type}")
    log.info(f"  active: {interface.active}")
    log.info(f"     mac: {interface.mac}")
    log.info(f"    addr: {interface.addresses}")
    log.info("")

