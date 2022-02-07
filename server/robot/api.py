from typing import List
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response
from socketio import AsyncServer

from .api_input import create_app as input_create_app
from .api_motors import create_app as motors_create_app
from .api_telemetry import create_app as telemetry_create_app
from .api_kinematic import create_app as kinematic_create_app
from .api_leds import create_app as leds_create_app
from .api_rcreceiver import create_app as rc_create_app

route = RouteTableDef()

@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response({})


def create_app(root: Application, sio: AsyncServer) -> Application:
    app = Application()
    app.add_subapp("/input", input_create_app(root, sio))
    app.add_subapp("/kinematic", kinematic_create_app(root, sio))
    app.add_subapp("/motors", motors_create_app(root, sio))
    app.add_subapp("/telemetry", telemetry_create_app(root, sio))
    app.add_subapp("/leds", leds_create_app(root, sio))
    app.add_subapp("/rcreceiver", rc_create_app(root, sio))
    
    return app

