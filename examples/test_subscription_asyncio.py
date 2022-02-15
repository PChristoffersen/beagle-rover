#!/usr/bin/env python3

import sys
import logging
import asyncio
sys.path.append('../build')

from time import sleep
from robotsystem import Robot, Subscription, Motor

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%H:%M:%S', level=logging.DEBUG)
logger = logging.getLogger("test")



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


async def notification_task(motor: Motor):
    sub = motor.subscribe()
    motor.servo.subscribe_attach(sub, 1000)
    try:
        while True:
            res = await read_async(sub)
            logger.info(f"Notifications: {res}")
            await asyncio.sleep(0.25)
    except asyncio.CancelledError:
        return
    finally:
        sub.unsubscribe()



async def main():
    robot = Robot()
    robot.init()

    motor = robot.motor_control.motors[0]

    task = asyncio.create_task(notification_task(motor))

    await asyncio.sleep(1)
    motor.enabled = True
    motor.servo.enabled = True

    await asyncio.sleep(1)
    motor.duty = 1.0
    motor.servo.enabled = False

    await asyncio.sleep(1)
    motor.enabled = False

    task.cancel()
    await task
    task = None

    logger.info("Clear: motor")
    motor = None

    robot.cleanup()
    robot = None
    logger.info("Done")



if __name__ == '__main__':
    loop = asyncio.get_event_loop()
    try:
        loop.run_until_complete(main())
    finally:
        loop.close()
