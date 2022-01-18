#!/usr/bin/env python3

import sys
import logging
from math import pi as PI
from datetime import date, datetime
sys.path.append('../build')

from time import sleep
from robotsystem import Robot

from datetime import datetime

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
log = logging.getLogger("test")



def main():
    robot = Robot()

    robot.init()

    control = robot.motor_control
    motors = control.motors

    motor = motors[0]
    #motor.enabled = True
    #motor.target_rpm = 100.0

    with open("plot.dat", "w") as f:
        start = datetime.now()

        runtime = 2000.0

        while True:
            now = datetime.now()
            delta = (now-start).total_seconds()*1000.0
            if delta > runtime:
                break

            if delta > 200 and not motor.enabled:
                motor.enabled = True
                motor.target_rpm = 50.0
            if delta > runtime-200.0 and motor.target_rpm != 0.0:
                motor.enabled = True
                motor.target_rpm = 0.0
            f.write(f"{delta:.2f} {motor.target_rpm:.2f} {motor.rpm:.2f} {motor.duty*100.0:.2f} {motor.target_rpm-motor.rpm:.2f}\n")
            sleep(0.02)

#        try:
#            while True:
#                sleep(5)
#        except KeyboardInterrupt:
#            log.info("Shutdown")

    control = None
    motors = None

    robot.cleanup()
    robot = None
    log.info("Done")


    return


if __name__ == '__main__':
    main()
