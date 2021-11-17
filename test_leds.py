#!/usr/bin/env python3

import sys
import logging
sys.path.append('build')

from time import sleep
from build.beaglerover import Robot, LEDControl, LEDColorLayer
#from beaglerover import Robot

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")



BLACK = ( 0x00, 0x00, 0x00 )
RED   = ( 0x20, 0x00, 0x00 )
GREEN = ( 0x00, 0x20, 0x00 )
BLUE  = ( 0x00, 0x00, 0x20 )



def colors_shift(led_control: LEDControl, layer: LEDColorLayer, color):
    for i in range(len(layer)):
        log.info(f"{i} - {color}")
        
        with led_control:
            layer.fill(BLACK)
            layer[i] = color
            led_control.show()
        
        sleep(0.1)


def main():
    robot = Robot()
    robot.init()

    led_control = robot.led_control

    layer = LEDColorLayer(0)
    led_control.add_layer(layer)

    try:
        while True:
            log.info("RED")
            colors_shift(led_control, layer, RED)
            log.info("GREEN")
            colors_shift(led_control, layer, GREEN)
            log.info("BLUE")
            colors_shift(led_control, layer, BLUE)

    except KeyboardInterrupt:
        log.info("Shutdown")

    led_control = None

    robot.cleanup()
    robot = None
    log.info("Done")



if __name__ == '__main__':
    main()
