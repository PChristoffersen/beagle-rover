#!/usr/bin/env python3

from time import sleep
from build.beaglerover import Robot

robot = Robot()

robot.init()

sleep(2)
sleep(2)

robot.stop()

print("Plonk")