import enum
import logging
import asyncio
from dataclasses import dataclass
from aiohttp.web import Application, RouteTableDef, Request, Response
from socketio import AsyncServer
from typing import List

from robotsystem import LEDControl, LEDAnimation, LEDIndicator, LEDColorArray, LEDColorLayer, LEDColorSegment, LEDColorSegmentList

from .util import to_enum
from .watches import WatchableNamespace, SubscriptionWatch
from .serializer import json_request, json_response

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


ANIMATION_MODES = [
    { "key": str(LEDAnimation.NONE),         "disabled": False, "name": "None"},
    { "key": str(LEDAnimation.HEADLIGHTS),   "disabled": False, "name": "Headlights" },
    { "key": str(LEDAnimation.CONSTRUCTION), "disabled": False, "name": "Construction" },
    { "key": str(LEDAnimation.POLICE),       "disabled": False, "name": "Police" },
    { "key": str(LEDAnimation.AMBULANCE),    "disabled": False, "name": "Ambulance" },
    { "key": str(LEDAnimation.RUNNING_LIGHT),"disabled": False, "name": "Running Light" },
    { "key": str(LEDAnimation.KNIGHT_RIDER), "disabled": False, "name": "Knight Rider" },
    { "key": str(LEDAnimation.RAINBOW),      "disabled": False, "name": "Rainbow" },
    { "key": str(LEDAnimation.RAINBOW_WAVE), "disabled": False, "name": "Rainbow Wave" },
]

INDICATOR_MODES = [
    { "key": str(LEDIndicator.NONE),   "disabled": False, "name": "None" },
    { "key": str(LEDIndicator.LEFT),   "disabled": False, "name": "Turn Left" },
    { "key": str(LEDIndicator.RIGHT),  "disabled": False, "name": "Turn Right" },
    { "key": str(LEDIndicator.HAZARD), "disabled": False, "name": "Hazard" },
]



def segment2list(segment: LEDColorSegment) -> list:
    return [col for col in segment]

def segments2dict(segments: LEDColorSegmentList) -> dict:
    return {
        segment.name: segment.values() for segment in segments
    }


def layer2dict(layer: LEDColorLayer) -> dict:
    return {
        "name": layer.name,
        "internal": layer.internal,
        "visible": layer.visible,
        "depth": layer.depth,
        "segments": segments2dict(layer.segments)
    }

def leds2dict(led_control) -> dict:
    res = {
        "brightness": led_control.brightness,
        "background": led_control.background,
        "animation": str(led_control.animation),
        "indicators": str(led_control.indicators),
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


def set_segment_from_list(segment: LEDColorSegment, json: list):
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
    json = await json_request(request)
    set_leds_from_dict(led_control, json)
    return json_response(leds2dict(led_control))


@route.get("/output")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    output = robot.led_control.output
    return json_response(segments2dict(output.segments))

@route.get("/strip-output")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    output = robot.led_control.output
    return json_response(output.values())


@route.get("/layers")
async def layers(request: Request) -> Response:
    robot = request.config_dict["robot"]
    led_control = robot.led_control
    return json_response([layer2dict(layer) for layer in led_control.layers])


@route.get("/layers/{name}")
async def get_layer(request: Request) -> Response:
    name = str(request.match_info["name"])
    robot = request.config_dict["robot"]
    led_control = robot.led_control
    layer = led_control.layers.find(name)
    return json_response(layer2dict(layer))

@route.put("/layers/{name}")
async def put_layer(request: Request) -> Response:
    name = str(request.match_info["name"])
    robot = request.config_dict["robot"]
    led_control = robot.led_control
    layer = led_control.layers.find(name)

    json = await json_request(request)


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
    return json_response(ANIMATION_MODES)

@route.get("/indicators")
async def indicators(request: Request) -> Response:
    return json_response(INDICATOR_MODES)



class LEDWatch(SubscriptionWatch):
    def data(self):
        return leds2dict(self.target)

    def _target_subscribe(self):
        if not self.sub:
            # We only want the default notification, not the NOTIFY_UPDATE event since it triggers very fast
            self.sub = self.target.subscribe( (LEDControl.NOTIFY_DEFAULT, ) )


class LEDOutputWatch(SubscriptionWatch):
    UPDATE_GRACE_PERIOD = 0.1

    def data(self):
        return segments2dict(self.target.output.segments)

    def _target_subscribe(self):
        if not self.sub:
            # We only want the default notification, not the NOTIFY_UPDATE event since it triggers very fast
            self.sub = self.target.subscribe( (LEDControl.NOTIFY_UPDATE, ) )
    
    async def emit(self, res: tuple):
        await super().emit(res)
        await asyncio.sleep(self.UPDATE_GRACE_PERIOD)



class LEDNamespace(WatchableNamespace):
    NAME = "/leds"

    def __init__(self, app: Application):
        super().__init__(logger=logger)
        self.app = app

    async def app_started(self):
        robot = self.app["root"]["robot"]
        await self._init_watches([
            LEDWatch(self, robot.led_control),
            LEDOutputWatch(self, robot.led_control, "update_output")
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
