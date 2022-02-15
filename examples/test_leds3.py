#!/usr/bin/env python3

import sys
import logging
sys.path.append('../build')

from time import sleep
from robotsystem import Robot, LEDControl, LEDColorLayer, LEDAnimation, LEDIndicator

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")



def main():
    robot = Robot()
    robot.init()

    led_control = robot.led_control

    layer = LEDColorLayer(15)
    layer.visible = True
    led_control.attach_layer(layer)

    led_control.detach_layer(layer)

    layer.visible = False
    led_control.attach_layer(layer)
    led_control.detach_layer(layer)



    layer = None
    log.info("<Layer Layer")


    led_control = None

    robot.cleanup()
    robot = None
    log.info("Done")



if __name__ == '__main__':
    main()
