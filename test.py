#!/usr/bin/env python3

import sys
sys.path.append('build')

from time import sleep
from build.beaglerover import Robot, LEDColor, TelemetryListener
#from beaglerover import Robot

col = LEDColor(0xFF, 0x00, 0x00)

colors = [
    LEDColor(0xFF, 0x00, 0xFF),
    LEDColor(0xFF, 0x01, 0xFF),
    LEDColor(0xFF, 0x02, 0xFF),
    LEDColor(0xFF, 0x03, 0xFF),
    LEDColor(0xFF, 0x04, 0xFF),
    LEDColor(0xFF, 0x05, 0xFF),
    LEDColor(0xFF, 0x06, 0xFF),
    LEDColor(0xFF, 0x07, 0xFF),
    LEDColor(0xFF, 0x08, 0xFF),
    LEDColor(0xFF, 0x09, 0xFF),
    LEDColor(0xFF, 0x0A, 0xFF),
    LEDColor(0xFF, 0x0B, 0xFF),
    LEDColor(0xFF, 0x0C, 0xFF),
    LEDColor(0xFF, 0x0D, 0xFF),
    LEDColor(0xFF, 0x0E, 0xFF),
    LEDColor(0xFF, 0x0F, 0xFF),
]



#robot = Robot()

#robot.led_control.update_pixels(col2)
#robot.led_control.update_pixels(colors)
#robot.init()

#sleep(2)
#robot.armed = True

def fusk(event): 
    try:
        print(type(event))
        print(event.battery_id)
        print(len(event.cell_voltages))
        for v in event.cell_voltages:
            print(f"{v}")
    except Exception as e:
        print(f"Yikes {e}")


def main():
    robot = Robot()

    class Listener(TelemetryListener):
        def on_event(self, event):
            print(event)

    #listener = TelemetryListener(robot, fusk)
    listener = Listener(robot.telemetry)

    robot.init()
    #for motor in robot.motor_control.motors:
    #    print(""+str(motor) + "  "+ str(motor.index))

    sleep(2)
    robot.cleanup()

    return



if __name__ == '__main__':
    main()