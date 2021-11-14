#!/usr/bin/env python3

import sys
from math import pi as PI
sys.path.append('build')

from time import sleep
from build.beaglerover import Robot, LEDColor, TelemetryListener, DriveMode
#from beaglerover import Robot

col = LEDColor(0xFF, 0x00, 0x00)

colors = [
    LEDColor(0xFF, 0x00, 0x00),
    LEDColor(0xFF, 0x01, 0x00),
    LEDColor(0xFF, 0x02, 0x00),
    LEDColor(0xFF, 0x03, 0x00),

    LEDColor(0xFF, 0x04, 0x00),
    LEDColor(0xFF, 0x05, 0x00),
    LEDColor(0xFF, 0x06, 0x00),
    LEDColor(0xFF, 0x07, 0x00),

    LEDColor(0xFF, 0x08, 0xFF),
    LEDColor(0xFF, 0x09, 0xFF),
    LEDColor(0xFF, 0x0A, 0xFF),
    LEDColor(0xFF, 0x0B, 0xFF),
    
    LEDColor(0xFF, 0x0C, 0xFF),
    LEDColor(0xFF, 0x0D, 0xFF),
    LEDColor(0xFF, 0x0E, 0xFF),
    LEDColor(0xFF, 0x0F, 0xFF),
]


def set_leds(robot: Robot):
    leds = robot.led_control
    leds.update_pixels(colors)

def main():
    robot = Robot()

    robot.init()

    set_leds(robot)

    try:
        while True:
            sleep(2)
    except KeyboardInterrupt:
        print("Shutdown")

    robot.cleanup()
    robot = None
    print("Done")

    robot.cleanup()
    robot = None
    print("Done")


if __name__ == '__main__':
    main()
