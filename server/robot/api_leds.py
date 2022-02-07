import enum
import logging
import asyncio
from dataclasses import dataclass
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response
from socketio import AsyncServer
from typing import List

from robotsystem import Subscription, LEDControl, LEDAnimation, LEDIndicator, LEDColorLayer, LEDSegment

from .util import to_enum
from .watches import WatchableNamespace, SubscriptionWatch

logger = logging.getLogger(__name__)

route = RouteTableDef()

LAYER_DEPTHS = [ 5, 15 ]

LED_PROPERTIES = frozenset([
    "brightness",
    "background",
    "animation",
    "indicators"
])

LAYER_PROPERTIES = frozenset([
    "visible",
])


def segment2list(segment: LEDSegment) -> list:
    return [col for col in segment]

# TODO Better segment mapping
def layer2dict(layer: LEDColorLayer) -> dict:
    return {
        "name": layer.name,
        "internal": layer.internal,
        "visible": layer.visible,
        "depth": layer.depth,
        "segments": {
            "front": segment2list(layer.segments[0]),
            "back": segment2list(layer.segments[1]),
        }
    }

def leds2dict(led_control) -> dict:
    res = {
        "brightness": led_control.brightness,
        "background": led_control.background,
        "animation": str(led_control.animation),
        "indicators": str(led_control.indicators),
        "layers": [layer.name for layer in led_control.layers]
    }
    return res



def set_leds_from_dict(led_control, json: dict):
    for key, value in json.items():
        if key in LED_PROPERTIES:
            if key == "animation":
                value = to_enum(LEDAnimation, value)
            elif key == "indicators":
                value = to_enum(LEDIndicator, value)
            setattr(led_control, key, value)


def set_segment_from_list(segment: LEDSegment, json: list):
    for idx, color in enumerate(json):
        segment[idx] = color

def set_layer_from_dict(layer: LEDColorLayer, json: dict):
    changed = False
    for key, value in json.items():
        if key in LAYER_PROPERTIES:
            setattr(layer, key, value)
            changed = True
    return changed


@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    led_control = robot.led_control
    return json_response(leds2dict(led_control))


@route.put("")
async def put(request: Request) -> Response:
    robot = request.config_dict["robot"]
    led_control = robot.led_control
    t = await request.text()
    json = await request.json()
    set_leds_from_dict(led_control, json)
    return json_response(leds2dict(led_control))


@route.get("/layers")
async def layers(request: Request) -> Response:
    robot = request.config_dict["robot"]
    led_control = robot.led_control
    return json_response([layer2dict(layer) for layer in led_control.layers])


@route.get("/layers/{index:\d+}")
async def get_layer(request: Request) -> Response:
    index = int(request.match_info["index"])
    layer = request.config_dict["layers"][index]
    return json_response(layer2dict(index, layer))

@route.put("/layers/{index:\d+}")
async def put_layer(request: Request) -> Response:
    index = int(request.match_info["index"])
    layer = request.config_dict["layers"][index]
    json = await request.json()
    changed = False
    with layer:
        changed = set_layer_from_dict(layer, json)
        if "segments" in json:
            segments = json["segments"]
            if "front" in segments:
                set_segment_from_list(layer.segments[0], segments["front"])
                changed = True
            if "back" in segments:
                set_segment_from_list(layer.segments[0], segments["back"])
                changed = True            
        if changed and layer.visible:
            layer.show()
    if changed:
        request.app["ns"].notify_layer(index)

    return json_response(layer2dict(index, layer))



@route.get("/animations")
async def animations(request: Request) -> Response:
    return json_response([ str(v) for k,v in LEDAnimation.values.items()])

@route.get("/indicators")
async def indicators(request: Request) -> Response:
    return json_response([ str(v) for k,v in LEDIndicator.values.items()])





class LEDWatch(SubscriptionWatch):
    def data(self):
        return leds2dict(self.target)

class LEDNamespace(WatchableNamespace):
    NAME = "/leds"

    def __init__(self, app: Application):
        super().__init__(logger=logger)
        self.app = app

    async def app_started(self):
        robot = self.app["root"]["robot"]
        await self._init_watches([
            LEDWatch(self, robot.led_control)
        ])

    async def app_cleanup(self):
        await self._destroy_watches()

    def notify_layer(self, index: int):
        pass

    # 
    # Event handlers
    #




async def app_on_startup(app: Application):
    logger.info("Startup")

    robot = app["root"]["robot"]
    led_control = robot.led_control

    #layers = [LEDColorLayer(depth) for depth in LAYER_DEPTHS]
    #for layer in layers:
    #    layer.visible = False
    #    led_control.attach_layer(layer)
    #app["layers"] = layers

    ns = app["ns"]
    await ns.app_started()


async def app_on_cleanup(app: Application):
    logger.info("Cleanup")
    ns = app["ns"]
    await ns.app_cleanup()
    
    #for layer in app["layers"]:
    #    layer.detach()
    #app["layers"] = None



def create_app(root: Application, sio: AsyncServer) -> Application:
    app = Application()
    app.add_routes(route)
    app.on_startup.append(app_on_startup)
    app.on_cleanup.append(app_on_cleanup)

    app["root"] = root

    ns = LEDNamespace(app)
    sio.register_namespace(ns)
    app["ns"] = ns

    return app
