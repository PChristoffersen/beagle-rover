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

    layer2 = LEDColorLayer(15)
    led_control.attach_layer(layer2)
    print("Plonk1_", flush=True)

    layer.visible = True

    with layer:
        layer[0] = (0xFF, 0x00, 0x00)
        layer[1] = (0x00, 0xFF, 0x00)
        layer[2] = (0x00, 0x00, 0xFF)
        layer.show()

    sleep(4)

    print("Plonk2", flush=True)
    led_control.detach_layer(layer)
    print("Plonk3", flush=True)


    layer = None
    log.info("<Layer Layer")


    led_control = None

    robot.cleanup()
    robot = None
    log.info("Done")



if __name__ == '__main__':
    main()
