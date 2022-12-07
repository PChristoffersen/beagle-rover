#!/usr/bin/env python3

import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent / '../build'))

import unittest
import asyncio
import logging

from beaglerover import TestComponent, Subscription

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

    def test_sync(self):
        sut = TestComponent()
        sub = sut.subscribe()

        
        sut.ping(0)
        sut.ping(1)
        expect = (0,1, )
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

        sut.ping(0)
        expect = (0,)
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")


    def test_multiple(self):
        sut = TestComponent()
        sub0 = sut.subscribe()
        sub1 = sut.subscribe()
        sub2 = sut.subscribe()

        sut.ping(0)
        expect = (0,)
        res = sub0.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")
        res = sub1.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")
        res = sub2.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

        sub1.unsubscribe()
        sub2.unsubscribe()

        sut.ping(0)
        sut.ping(1)
        expect = (0,1)
        res = sub0.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")
        res = sub1.read()
        self.assertEqual( set(res), set(), f"Expected {expect} got {res}")
        res = sub2.read()
        self.assertEqual( set(res), set(), f"Expected {expect} got {res}")


    def test_filtered(self):
        sut = TestComponent()
        sub = sut.subscribe( (10, 11, 12) )

        sut.ping(0)
        sut.ping(1)
        expect = tuple()
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

        sut.ping(10)
        expect = (10,)
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

        for i in range(100):
            sut.ping(i)
        expect = (10,11,12)
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")
        

    def test_attach(self):
        sut = TestComponent()
        sut2 = TestComponent()
        offset = 100
        sub = sut.subscribe()
        sub = sut2.subscribe(sub, offset)

        sut.ping(0)
        sut2.ping(0)
        expect = (0, 0+offset)
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

        sut.ping(0)
        expect = (0,)
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

        sut2.ping(0)
        expect = (0+offset,)
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

        sub.unsubscribe()


    
    def test_attach_filtered(self):
        sut = TestComponent()
        sut2 = TestComponent()
        offset = 100
        sub = sut.subscribe( (10,11,12) )
        sub = sut2.subscribe(sub, offset, (10,20,21))

        sut.ping(0)
        sut2.ping(0)
        expect = tuple()
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

        sut.ping(10)
        sut2.ping(10)
        expect = (10, 10+offset)
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

        sut.ping(11)
        sut2.ping(11)
        expect = (11,)
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

        sut.ping(20)
        sut2.ping(20)
        expect = (20+offset,)
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

        for i in range(100):
            sut.ping(i)
            sut2.ping(i)
        expect = (10,11,12,10+offset,20+offset,21+offset)
        res = sub.read()
        self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")
        

        sub.unsubscribe()


    def test_async(self):
        async def test():
            sut = TestComponent()
            sub = sut.subscribe()

            sut.ping(0)

            # Notify default should be signaled by just changing animation
            expect = (0, )
            res = await read_async(sub)
            self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

            # Start an asyncio task that calls ping(1) eight times
            async def task_func(sut: TestComponent):
                for i in range(8):
                    sut.ping(1)
                    await asyncio.sleep(0.1)
            task = asyncio.create_task(task_func(sut))

            # Wait for 8 notify updates
            for i in range(8):
                expect = (1,)
                res = await read_async(sub)
                self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")

            await task

            # Start task again and wait for it to complete
            task = asyncio.create_task(task_func(sut))
            await task

            # Check that we get 1 notify result with event 1 from the 8 taks notifies
            expect = (1,)
            res = await read_async(sub)
            self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")


            sut.ping(0)
            expect = (0,)
            res = await read_async(sub)
            self.assertEqual( set(res), set(expect), f"Expected {expect} got {res}")


        loop = asyncio.get_event_loop()
        try:
            loop.run_until_complete(test())
        finally:
            loop.close()






if __name__ == '__main__':
    unittest.main()
