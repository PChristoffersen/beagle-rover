#!/usr/bin/env python3

import sys
from math import pi as PI
sys.path.append('build')

from time import sleep
from build.robotcontrol import Robot, TelemetryListener, DriveMode



def print_battery(event): 
    try:
        print(event.battery_id)
        print(len(event.cell_voltages))
        for v in event.cell_voltages:
            print(f"{v}")
    except Exception as e:
        print(f"Yikes {e}")


def set_motors(robot: Robot, pos):
    print(f"Setting pulse {pos} us")
    for motor in robot.motor_control.motors:
        motor.gimbal.pulse_us = pos
    


def main():
    robot = Robot()
    robot.init()

    robot.kinematic.drive_mode = DriveMode.NORMAL
    robot.kinematic.drive_mode = DriveMode.NONE
    robot.kinematic.drive_mode = DriveMode.SPINNING
    robot.kinematic.drive_mode = DriveMode.BALANCING
    robot.kinematic.drive_mode = DriveMode.NONE

    print("")
    sleep(1)
    print("")
    print("BALANCING ------------------------")
    robot.kinematic.drive_mode = DriveMode.BALANCING
    print("BALANCING <")
    sleep(1)

    print("")
    print("NONE -----------------------------")
    robot.kinematic.drive_mode = DriveMode.NONE
    print("NONE <")
    sleep(6)

    print("")
    print("BALANCING ------------------------")
    robot.kinematic.drive_mode = DriveMode.BALANCING
    print("BALANCING <")
    sleep(1)

    #try:
    #    sleep(1)
    #except KeyboardInterrupt:
    #    print("Shutdown")

    robot.cleanup()
    robot = None
    print("Done")


    return


if __name__ == '__main__':
    main()
