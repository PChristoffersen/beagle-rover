#!/usr/bin/env python3

import sys
import logging
sys.path.append('../build')

from time import sleep
from robotsystem import Robot, LEDControl, LEDColorLayer, LEDAnimation, LEDIndicator

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")



BLACK = ( 0x00, 0x00, 0x00 )
RED   = ( 0xFF, 0x00, 0x00 )
GREEN = ( 0x00, 0xFF, 0x00 )
BLUE  = ( 0x00, 0x00, 0xFF )



def colors_shift(layer: LEDColorLayer, color):
    for i in range(len(layer)):
        #log.info(f"{i} - {color}")
        
        with layer:
            layer.fill(BLACK)
            layer[i] = color
            layer.show()

        sleep(0.05)

def shift_loop(led_control):
    layer = LEDColorLayer(0)
    led_control.attach_layer(layer)

    layer.fill( ( 0xFF, 0x00, 0x00, 0x80 ))
    layer.show()

    try:
        while True:
            log.info("RED")
            colors_shift(layer, RED)
            log.info("GREEN")
            colors_shift(layer, GREEN)
            log.info("BLUE")
            colors_shift(layer, BLUE)
    except KeyboardInterrupt:
        log.info("Shutdown")

    layer.detach()

def keyboard_loop():
    try:
        while True:
            sleep(2.0)
    except KeyboardInterrupt:
        log.info("Shutdown")




def main():
    robot = Robot()
    robot.init()

    led_control = robot.led_control

    sleep(6)

    try:
        while True:
            #led_control.background = ( 0x00, 0x00, 0x20 )
            led_control.animation = LEDAnimation.HEADLIGHTS
            sleep(10)
            led_control.animation = LEDAnimation.NONE
            led_control.indicators = LEDIndicator.LEFT
            sleep(10)
            led_control.indicators = LEDIndicator.RIGHT
            sleep(10)
            led_control.indicators = LEDIndicator.HAZARD
            sleep(10)
            led_control.indicators = LEDIndicator.NONE
            sleep(2)

            led_control.animation = LEDAnimation.AMBULANCE
            sleep(10)
            led_control.animation = LEDAnimation.POLICE
            sleep(10)
            #led_control.animation = LEDAnimation.CONSTRUCTION
            #sleep(10)
            led_control.animation = LEDAnimation.KNIGHT_RIDER
            sleep(30)
            #led_control.animation = LEDAnimation.RAINBOW
            #led_control.indicators = LEDIndicator.LEFT
            #led_control.indicators = LEDIndicator.RIGHT
    except KeyboardInterrupt:
        log.info("Shutdown")

    #shift_loop(led_control)
    #keyboard_loop()

    led_control = None

    robot.cleanup()
    robot = None
    log.info("Done")



if __name__ == '__main__':
    main()
