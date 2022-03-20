#!/usr/bin/env python3

import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent / '../build'))

import unittest
import time
import logging

from robotsystem import Robot, Subscription, LEDControl, LEDColorLayer, LEDAnimation, LEDIndicator

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%H:%M:%S', level=logging.INFO)
logger = logging.getLogger(__file__)


def wait_for_show(sub: Subscription, timeout: float, raise_timeout_error: bool = True):
    start = time.clock_gettime(time.CLOCK_MONOTONIC)
    while (time.clock_gettime(time.CLOCK_MONOTONIC)-start) < timeout:
        remaining_timeout = timeout - (time.clock_gettime(time.CLOCK_MONOTONIC)-start)
        res = sub.read(remaining_timeout)
        if LEDControl.NOTIFY_UPDATE in res:
            return
    if raise_timeout_error:
        raise TimeoutError("Timeout waiting for show")
    

class LedTestCase(unittest.TestCase):

    def setUp(self) -> None:
        self.robot = Robot()
        self.led_control = self.robot.led_control
        self.sub = self.led_control.subscribe( (LEDControl.NOTIFY_UPDATE,) )

        self.robot.init()
        wait_for_show(self.sub, 0.5, False)
        self.sub.clear()

    def tearDown(self) -> None:
        self.sub.unsubscribe()
        self.sub = None
        self.led_control = None
        self.robot.cleanup()
        self.robot = None


    def test_output(self):
        led_control = self.led_control
        output = led_control.output

        with output:
            # Check colors
            for c in output:
                self.assertIsInstance(c, str, "Unexpected color type")
            # Check values tuple
            values = output.values()
            self.assertIsInstance(values, tuple, "Unexpected value type")
            for v in values:
                self.assertIsInstance(v, str, "Unexpected color type")

        segments = output.segments
        self.assertEqual(len(segments), 2, "Unexpected number of segments")

        # Iterate through segments
        for segment in segments:
            self.assertIsInstance(segment.name, str, "Unexpected name type")
            with output:
                # Check colors
                for c in segment:
                    self.assertIsInstance(c, str, "Unexpected color type")
                # Check values tuple
                values = segment.values()
                self.assertIsInstance(values, tuple, "Unexpected value type")
                for v in values:
                    self.assertIsInstance(v, str, "Unexpected color type")


        # Iterate through segments by index
        for idx in range(len(segments)):
            segment = segments[idx]
            self.assertIsInstance(segment.name, str, "Unexpected name type")
            with output:
                # Check colors
                for c in segment:
                    self.assertIsInstance(c, str, "Unexpected color type")
                # Check values tuple
                values = segment.values()
                self.assertIsInstance(values, tuple, "Unexpected value type")
                for v in values:
                    self.assertIsInstance(v, str, "Unexpected color type")


        # Iterate through segments by name
        self.assertGreater(len(segments.keys()), 0, "No segment list keys")
        for name in segments.keys():
            segment = segments[name]
            self.assertIsInstance(name, str, "Unexpected name type")
            self.assertIsInstance(segment.name, str, "Unexpected name type")
            self.assertEqual(segment.name, name, "Expected name to match")
            with output:
                # Check colors
                for c in segment:
                    self.assertIsInstance(c, str, "Unexpected color type")
                # Check values tuple
                values = segment.values()
                self.assertIsInstance(values, tuple, "Unexpected value type")
                for v in values:
                    self.assertIsInstance(v, str, "Unexpected color type")



    def test_background(self):
        led_control = self.led_control

        col = "804020"
        led_control.background = col
        wait_for_show(self.sub, 1.0)
        leds = led_control.output.values()
        self.assertEqual(leds[0], col, f"Unexpected pixel color {leds[0]} != {col}")

        col = "FFFFFF80"
        expect = "FFFFFF"
        led_control.background = col
        wait_for_show(self.sub, 1.0)
        leds = led_control.output.values()
        self.assertEqual(leds[0], expect, f"Unexpected pixel color {leds[0]} != {expect}")


    def test_animations(self):
        for _, anim in LEDAnimation.values.items():
            if anim == LEDAnimation.NONE:
                continue
            self.sub.clear()
            self.led_control.animation = anim
            wait_for_show(self.sub, 3.0)
        
        self.led_control.animation = LEDAnimation.NONE
        wait_for_show(self.sub, 0.25)



    def test_indicators(self):
        for _, ind in LEDIndicator.values.items():
            if ind == LEDIndicator.NONE:
                continue
            logger.info(f"Testing indicator {ind}")
            self.sub.clear()
            self.led_control.indicators = ind
            wait_for_show(self.sub, 3.0)

        self.led_control.indicators = LEDIndicator.NONE


    def test_layer(self):
        self.led_control.background = "202020"

        self.led_control.animation = LEDAnimation.NONE
        self.led_control.indicators = LEDIndicator.NONE
        wait_for_show(self.sub, 1.0, False)
        self.sub.clear()

        layer = LEDColorLayer("layer", 15)
        layer.visible = True
        self.led_control.attach_layer(layer)
        wait_for_show(self.sub, 1.0)

        with layer:
            layer.visible = True
            layer.fill("FF0000")
            layer.update()

        wait_for_show(self.sub, 3.0)
        for idx, pix in enumerate(self.led_control.output.values()):
            self.assertEqual(pix, "FF0000", f"Unexpected colors on pixel {idx}")


        with layer:
            layer.fill("00000000")
            layer[0] = "FF0000"
            layer[1] = "00FF00"
            layer[2] = "0000FF"
            layer.update()

        wait_for_show(self.sub, 3.0)
        leds = self.led_control.output.values()
        self.assertEqual(leds[0], "FF0000", "Unexpected colors on pixel 0")
        self.assertEqual(leds[1], "00FF00", "Unexpected colors on pixel 1")
        self.assertEqual(leds[2], "0000FF", "Unexpected colors on pixel 2")
        self.assertEqual(leds[3], "202020", "Unexpected colors on pixel 3")

        with layer:
            layer.fill("00000000")
            layer[0] = "FF0000FF"
            layer[1] = "00FF00FF"
            layer[2] = "0000FFFF"
            layer.update()

        wait_for_show(self.sub, 3.0)
        leds = self.led_control.output.values()
        self.assertEqual(leds[0], "FF0000", "Unexpected colors on pixel 0")
        self.assertEqual(leds[1], "00FF00", "Unexpected colors on pixel 1")
        self.assertEqual(leds[2], "0000FF", "Unexpected colors on pixel 2")
        self.assertEqual(leds[3], "202020", "Unexpected colors on pixel 3")

        with layer:
            layer.fill("00000000")
            layer.segments[0][0] = "00FF00"
            layer.segments[0][1] = "0000FF"
            layer.segments[0][2] = "00FFFF"
            layer.segments[1][0] = "00FF00"
            layer.segments[1][1] = "0000FF"
            layer.segments[1][2] = "00FFFF"
            layer.update()

        wait_for_show(self.sub, 3.0)

        self.led_control.detach_layer(layer)

        self.led_control.background = "000000"



    def test_invalid_colors(self):
        led_control = self.led_control

        with self.assertRaises(ValueError):
            led_control.background = ""
        
        with self.assertRaises(ValueError):
            led_control.background = "Bad"
        
        with self.assertRaises(ValueError):
            led_control.background = "000"
        
        with self.assertRaises(ValueError):
            led_control.background = "00000"
        
        with self.assertRaises(ValueError):
            led_control.background = "0000000"







if __name__ == '__main__':
    unittest.main()
