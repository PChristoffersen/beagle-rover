#!/usr/bin/env python3

import sys
from math import pi as PI
sys.path.append('build')

from time import sleep
from build.beaglerover import Robot, TelemetryListener, DriveMode


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
    robot = Robot()
    #class Listener(TelemetryListener):
    #    def on_event(self, event):
    #        print(event)

    #listener = TelemetryListener(robot, fusk)
    #listener = Listener()
    #listener.connect(robot.telemetry)

    robot.init()




    #robot.motor_control.motors[0].gimbal.enabled = True
    robot.motor_control.motors[0].gimbal.pulse_us = 1500
    #robot.motor_control.motors[0].gimbal.pulse_us = 2500
    print(f"Angle: {robot.motor_control.motors[0].gimbal.angle*180/PI}")
    robot.motor_control.motors[0].gimbal.angle = -PI/2
    print(f"Angle: {robot.motor_control.motors[0].gimbal.angle*180.0/PI}")
    robot.motor_control.motors[0].gimbal.angle = 0.0
    print(f"Angle: {robot.motor_control.motors[0].gimbal.angle*180/PI}")
    robot.motor_control.motors[0].gimbal.angle = PI/2
    print(f"Angle: {robot.motor_control.motors[0].gimbal.angle*180.0/PI}")
    return

    #for motor in robot.motor_control.motors:
    #    print("Enabling motor "+str(motor.index))
    #    motor.gimbal.pulse_us = 1500
    #    motor.gimbal.enabled = True
        #print(""+str(motor) + "  "+ str(motor.index) + "  gimbal "+str(motor.gimbal) + " " + str(motor.gimbal.index))
    #print("")



    robot.kinematic.drive_mode = DriveMode.NORMAL
    robot.kinematic.drive_mode = DriveMode.NONE
    robot.kinematic.drive_mode = DriveMode.SPINNING
    robot.kinematic.drive_mode = DriveMode.BALANCING
    robot.kinematic.drive_mode = DriveMode.NONE

    print("")
    sleep(1)
    print("")
    robot.kinematic.drive_mode = DriveMode.BALANCING
    sleep(1)
    print("NONE")
    robot.kinematic.drive_mode = DriveMode.NONE
    sleep(6)
    robot.kinematic.drive_mode = DriveMode.BALANCING
    sleep(1)
    
    print("Cleanup")
    robot.cleanup()
    print("")

    return

    #for i in range(3):
    #    print("Sleep - "+str(i))
    #    sleep(1)

    #robot.kinematic.drive_mode = DriveMode.NONE

    try:
        toggle = 0
        while True:
            sleep(2)
            print(f"Toggle: {toggle}")
            if toggle == 1:
                set_motors(robot, 1000)
            elif toggle == 3:
                set_motors(robot, 2000)
            else:
                set_motors(robot, 1500)
            toggle = toggle +1
            if toggle > 3:
                toggle = 0
    except KeyboardInterrupt:
        print("Shutdown")

    robot.cleanup()
    robot = None
    print("Done")


    return


if __name__ == '__main__':
    main()
