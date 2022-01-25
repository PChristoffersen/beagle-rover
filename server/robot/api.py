from typing import List
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response
from socketio import AsyncServer

from .api_input import app as app_input
from .api_motors import create_app as motors_create_app
from .api_telemetry import app as app_telemetry
from .api_leds import app as app_leds

route = RouteTableDef()

@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response({})


def create_app(root: Application, sio: AsyncServer) -> Application:
    app = Application()
    app.add_subapp("/input", app_input)
    app.add_subapp("/motors", motors_create_app(root, sio))
    app.add_subapp("/telemetry", app_telemetry)
    app.add_subapp("/leds", app_leds)
    
    return app

