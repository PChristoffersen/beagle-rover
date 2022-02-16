#!/usr/bin/env python3

import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent / '../build'))

import unittest
import asyncio
import logging

from robotsystem import Robot, Subscription, Motor, LEDControl, LEDAnimation

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%H:%M:%S', level=logging.INFO)
logger = logging.getLogger(__file__)




async def read_async(sub: Subscription) -> tuple:
    loop = asyncio.get_running_loop()
    fd = sub.fd
    fut = loop.create_future()

    def __callback():
        try:
            if fut.cancelled():
                return
            r = sub.read()
        except Exception as e:
            loop.remove_reader(fd)
            fut.set_exception(e)
        else:
            loop.remove_reader(fd)
            fut.set_result(r)

    loop.add_reader(fd, __callback)
    return await fut



class SubscriptionTestCase(unittest.TestCase):

    def setUp(self) -> None:
        self.robot = Robot()
        self.robot.init()

    def tearDown(self) -> None:
        self.robot.cleanup()
        self.robot = None

    
    def test_sync(self):
        motor = self.robot.motor_control.motors[0]
        sub = motor.subscribe()

        motor.duty = 0.5
        res = sub.read()
        self.assertIn(Motor.NOTIFY_DEFAULT, res, f"Expected NOTIFY_DEFAULT in {res}")

        motor.duty = 0.0
        res = sub.read()
        self.assertIn(Motor.NOTIFY_DEFAULT, res, f"Expected NOTIFY_DEFAULT in {res}")


    def test_multiple(self):
        motor = self.robot.motor_control.motors[0]
        sub0 = motor.subscribe()
        sub1 = motor.subscribe()
        sub2 = motor.subscribe()

        motor.duty = 0.5
        res = sub0.read()
        self.assertIn(Motor.NOTIFY_DEFAULT, res, f"0 Expected NOTIFY_DEFAULT in {res}")
        res = sub1.read()
        self.assertIn(Motor.NOTIFY_DEFAULT, res, f"1 Expected NOTIFY_DEFAULT in {res}")
        res = sub2.read()
        self.assertIn(Motor.NOTIFY_DEFAULT, res, f"2 Expected NOTIFY_DEFAULT in {res}")

        sub1.unsubscribe()
        sub2.unsubscribe()

        motor.duty = 0.0
        res = sub0.read()
        self.assertIn(Motor.NOTIFY_DEFAULT, res, f"0 Expected NOTIFY_DEFAULT in {res}")
        res = sub1.read()
        self.assertNotIn(Motor.NOTIFY_DEFAULT, res, f"1 Did not expect NOTIFY_DEFAULT in {res}")
        res = sub2.read()
        self.assertNotIn(Motor.NOTIFY_DEFAULT, res, f"2 Did not expect NOTIFY_DEFAULT in {res}")


    def test_attach(self):
        motor0 = self.robot.motor_control.motors[0]
        motor1 = self.robot.motor_control.motors[1]
        motor1_off = 100
        sub = motor0.subscribe()
        sub = motor1.subscribe(sub, motor1_off)

        motor0.duty = 0.5
        motor1.duty = 0.5
        res = sub.read()
        self.assertIn(Motor.NOTIFY_DEFAULT, res, f"Expected NOTIFY_DEFAULT in {res}")
        self.assertIn(Motor.NOTIFY_DEFAULT+motor1_off, res, f"Expected NOTIFY_DEFAULT+motor1_off in {res}")

        motor0.duty = 0.0
        res = sub.read()
        self.assertIn(Motor.NOTIFY_DEFAULT, res, f"Expected NOTIFY_DEFAULT in {res}")
        self.assertNotIn(Motor.NOTIFY_DEFAULT+motor1_off, res, f"Did not expect NOTIFY_DEFAULT+motor1_off in {res}")

        motor1.duty = 0.0
        res = sub.read()
        self.assertNotIn(Motor.NOTIFY_DEFAULT, res, f"Did not expected NOTIFY_DEFAULT in {res}")
        self.assertIn(Motor.NOTIFY_DEFAULT+motor1_off, res, f"Expected NOTIFY_DEFAULT+motor1_off in {res}")

        sub.unsubscribe()


    


    def test_async(self):
        async def test():
            led_control = self.robot.led_control
            sub = led_control.subscribe()

            led_control.animation = LEDAnimation.POLICE

            # Notify default should be signaled by just changing animation
            res = await read_async(sub)
            logger.info(f"Notify returned {res}")
            self.assertIn(LEDControl.NOTIFY_DEFAULT, res, f"Expected NOTIFY_DEFAULT in {res}")

            # Wait for 8 notify updates
            for i in range(8):
                res = await read_async(sub)
                logger.info(f"Notify returned {res}")
                self.assertIn(LEDControl.NOTIFY_UPDATE, res, f"{i} Expected NOTIFY_UPDATE in {res}")

            led_control.animation = LEDAnimation.NONE
            res = await read_async(sub)
            logger.info(f"Notify returned {res}")
            self.assertIn(LEDControl.NOTIFY_DEFAULT, res, f"Expected NOTIFY_DEFAULT in {res}")


        loop = asyncio.get_event_loop()
        try:
            loop.run_until_complete(test())
        finally:
            loop.close()






if __name__ == '__main__':
    unittest.main()
