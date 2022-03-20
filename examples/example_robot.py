#!/usr/bin/env python3

import sys
import logging
import asyncio
from pathlib import Path
sys.path.append(str(Path(__file__).parent / '../build'))

from time import sleep
from robotsystem import Robot, InputSource, Subscription

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%Y-%m-%d %H:%M:%S', level=logging.DEBUG)
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



async def main():
    robot = Robot()
    robot.init()

    robot.input.axis_source = InputSource.RC
    robot.input.kinematic_source = InputSource.RC
    robot.input.led_source = InputSource.RC

    try:
        while True:
            await asyncio.sleep(2)
    except KeyboardInterrupt:
        logger.info("Shutdown")

    robot.cleanup()
    robot = None
    logger.info("Done")


if __name__ == '__main__':
    loop = asyncio.get_event_loop()
    loop.run_until_complete(main())
