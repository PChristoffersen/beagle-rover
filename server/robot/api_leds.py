from dataclasses import dataclass
import logging
from multiprocessing.pool import ApplyResult
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response

from robotsystem import LEDAnimation, LEDIndicator

from .util import to_enum

logger = logging.getLogger(__name__)

route = RouteTableDef()

LED_PROPERTIES = frozenset([
    "background",
    "animation",
    "indicators"
])


def leds2dict(led_control) -> dict:
    return {
        "background": led_control.background,
        "animation": str(led_control.animation),
        "indicators": str(led_control.indicators)
    }

def set_leds_from_dict(led_control, json: dict):
    for key, value in json.items():
        if key in LED_PROPERTIES:
            if key == "animation":
                value = to_enum(LEDAnimation, value)
            elif key == "indicators":
                value = to_enum(LEDIndicator, value)
            elif key == "background":
                value = tuple(value)
            setattr(led_control, key, value)



@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    led_control = robot.led_control
    return json_response(leds2dict(led_control))


@route.put("")
async def put(request: Request) -> Response:
    robot = request.config_dict["robot"]
    led_control = robot.led_control
    json = await request.json()
    set_leds_from_dict(led_control, json)
    return json_response(leds2dict(led_control))


@route.get("/animations")
async def animations(request: Request) -> Response:
    return json_response([ str(v) for k,v in LEDAnimation.values.items()])

@route.get("/indicators")
async def indicators(request: Request) -> Response:
    return json_response([ str(v) for k,v in LEDIndicator.values.items()])



app = Application()
app.add_routes(route)
