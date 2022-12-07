#!/usr/bin/env python3

import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent / '../build'))

import unittest
import time
import logging

from beaglerover import LEDColorLayer, LEDColorSegment

FORMAT = '[%(asctime)s.%(msecs)03dxxx] [%(threadName)-18s] [%(levelname)s] >  %(message)s'
logging.basicConfig(format=FORMAT, datefmt='%H:%M:%S', level=logging.INFO)
logger = logging.getLogger(__file__)


class ColorLayerTestCase(unittest.TestCase):

    def test(self):
        layer = LEDColorLayer("test", 10)
        
        self.assertEqual(layer.name, "test", "Unexpected layer name")
        self.assertEqual(layer.depth, 10, "Unexpected layer depth")
        self.assertGreater(len(layer), 0, "Expected layer length to be greater than 0")

        # Test direct iterate
        for idx,col in enumerate(layer):
            self.assertEqual(col, "00000000", f"Unexpected color {idx}")

        # Test iterate by index
        for idx in range(len(layer)):
            self.assertEqual(layer[idx], "00000000", f"Unexpected color {idx}")

        # Test values() tuple
        values = layer.values()
        self.assertEqual(len(values), len(layer), "Expected values to be the same size as layer len()")
        for col in values:
            self.assertEqual(col, "00000000", f"Unexpected color {idx}")

    
    def test_modify(self):
        layer = LEDColorLayer("test", 10)

        # Check that initial color is transparent
        for col in layer:
            self.assertEqual(col, "00000000", f"Unexpected color")

        # Set fill layer
        layer.fill("FF884422")
        for col in layer:
            self.assertEqual(col, "FF884422", f"Unexpected color")
        layer.fill("00000000")
        for col in layer:
            self.assertEqual(col, "00000000", f"Unexpected color")

        # Set color by index
        for idx in range(len(layer)):
            layer[idx] = "FF884422"
        for col in layer:
            self.assertEqual(col, "FF884422", f"Unexpected color")
        layer.fill("00000000")
        
        # Set a few colors without alpha and check result
        layer[0] = "FF0000"
        layer[1] = "00FF00"
        layer[2] = "0000FF"
        layer[3] = "FF0000FF"
        layer[4] = "00FF00FF"
        layer[5] = "0000FFFF"
        self.assertEqual(layer[0], "FF0000")
        self.assertEqual(layer[1], "00FF00")
        self.assertEqual(layer[2], "0000FF")
        self.assertEqual(layer[3], "FF0000")
        self.assertEqual(layer[4], "00FF00")
        self.assertEqual(layer[5], "0000FF")



    def test_locking(self):
        layer = LEDColorLayer("test", 10)
        
        # Take layer lock, and get colors
        with layer:
            for idx,col in enumerate(layer):
                self.assertEqual(col, "00000000", f"Unexpected color {idx}")
            for col in layer.values():
                self.assertEqual(col, "00000000", f"Unexpected color {idx}")


    def test_segments(self):
        layer = LEDColorLayer("test", 10)
        segments = layer.segments

        self.assertGreater(len(segments), 0, "Expected segments to be greater than 0")
        
        # Test names
        names = segments.keys()
        self.assertEqual(len(names), len(segments), "Expected segment names to be same length as segment")


        # Test iterate
        for segment in segments:
            self.assertIsInstance(segment, LEDColorSegment, "Expected to get color segment")
            self.assertIsInstance(segment.name, str, "Expected name to be a string")
            self.assertIsInstance(segment.offset, int, "Expected offset to b a number")
            self.assertIsInstance(segment.size, int, "Expected size to b a number")
            self.assertEqual(segment.size, len(segment), "Expected segment size to be equal to len()")
            self.assertGreater(len(segment), 0, "Expected segment to have size greater than 0")
            # Test direct iterate
            for idx,col in enumerate(segment):
                self.assertEqual(col, "00000000", f"Unexpected color {idx}")
            # Test iterate by index
            for idx in range(len(segment)):
                self.assertEqual(segment[idx], "00000000", f"Unexpected color {idx}")
            # Test values() tuple
            values = segment.values()
            self.assertEqual(len(values), len(segment), "Expected values to be the same size as segment len()")
            for col in values:
                self.assertEqual(col, "00000000", f"Unexpected color {idx}")

        # Test iterate by index
        for idx in range(len(segments)):
            segment = segments[idx]
            self.assertIsInstance(segment, LEDColorSegment, "Expected to get color segment")

        # Test iterate by name
        for name in segments.keys():
            segment = segments[name]
            self.assertIsInstance(segment, LEDColorSegment, "Expected to get color segment")
            self.assertEqual(segment.name, name, "Expected segment name to match")


    def test_segment_modify(self):
        layer = LEDColorLayer("test", 10)
        segments = layer.segments

        # Iterate through segments and fill them
        for segment in segments:
            segment_range = range(segment.offset, segment.offset+len(segment))
            layer.fill("00000000")
            segment.fill("FF884422")
            
            # Check segment colors
            for col in segment:
                self.assertEqual(col, "FF884422", "Unexpected color")
            # Check layer colors
            for idx,col in enumerate(layer):
                if idx in segment_range:
                    self.assertEqual(col, "FF884422", "Unexpected color")
                else:
                    self.assertEqual(col, "00000000", "Unexpected color")


if __name__ == '__main__':
    unittest.main()
