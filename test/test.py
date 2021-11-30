#!/usr/bin/env python3

import sys
import logging
from math import pi as PI
sys.path.append('../build')

from time import sleep
from build.robotcontrol import Robot, TelemetryListener, DriveMode

from datetime import datetime

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")


def print_battery(event): 
    log.info(f"Battery {event.battery_id}:")
    for v in event.cell_voltages:
        log.info(f"  {v}v")


def set_motors(robot: Robot, pos):
    log.info(f"Setting pulse {pos} us")
    for motor in robot.motor_control.motors:
        motor.servo.pulse_us = pos
    

def test_motor_angles(robot: Robot):
    motors = robot.motor_control.motors
    motor = motors[0]
    servo = motor.servo

    angles = [ -PI/2, 0, PI/2 ]
    for angle in angles:
        servo.angle = angle
        log.info(f"Motor: pulse={servo.pulse_us}  angle_rad={servo.angle}  angle_deg={servo.angle_degrees}")



def main():
    robot = Robot()

    #class Listener(TelemetryListener):
    #    def on_event(self, event):
    #        print(event)

    #listener = TelemetryListener(robot, print_battery)
    #listener = Listener()
    #listener.connect(robot.telemetry)

    robot.init()

    #for motor in robot.motor_control.motors:
    #    log.info("Enabling motor "+str(motor.index))
    #    motor.servo.pulse_us = 1500
    #    motor.servo.angle = PI/2
    #    motor.servo.enabled = True
    #    log.info(f"{motor} {motor.index} servo {motor.servo} {motor.servo.index}")
    #log.info("")

    #test_motor_angles(robot)

    #robot.kinematic.drive_mode = DriveMode.NORMAL
    #robot.kinematic.drive_mode = DriveMode.NONE
    #robot.kinematic.drive_mode = DriveMode.SPINNING
    #robot.kinematic.drive_mode = DriveMode.BALANCING
    #robot.kinematic.drive_mode = DriveMode.NONE

    #print("")
    #sleep(1)
    #print("")
    #robot.kinematic.drive_mode = DriveMode.BALANCING
    #sleep(1)
    #print("NONE")
    #robot.kinematic.drive_mode = DriveMode.NONE
    #sleep(6)
    #robot.kinematic.drive_mode = DriveMode.BALANCING
    #sleep(1)
    
    #print("Cleanup")
    #robot.cleanup()
    #print("")

    #return

    #for i in range(3):
    #    print("Sleep - "+str(i))
    #    sleep(1)

    #robot.kinematic.drive_mode = DriveMode.NONE

    try:
        toggle = 0
        while True:
            sleep(2)
            #print(f"Toggle: {toggle}")
            #if toggle == 1:
            #    set_motors(robot, 1000)
            #elif toggle == 3:
            #    set_motors(robot, 2000)
            #else:
            #    set_motors(robot, 1500)
            #toggle = toggle +1
            #if toggle > 3:
            #    toggle = 0
    except KeyboardInterrupt:
        print("Shutdown")

    robot.cleanup()
    robot = None
    print("Done")


    return


if __name__ == '__main__':
    main()
