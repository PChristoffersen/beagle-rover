import logging
from pathlib import Path
from typing import Dict
from aiohttp.web import Application, RouteTableDef, Request, Response, FileResponse
from socketio import AsyncServer

from .api_input import create_app as input_create_app
from .api_motors import create_app as motors_create_app
from .api_telemetry import create_app as telemetry_create_app
from .api_kinematic import create_app as kinematic_create_app
from .api_leds import create_app as leds_create_app
from .api_rcreceiver import create_app as rc_create_app
from .api_system import create_app as system_create_app
from .error_handler import error_middleware
from .serializer import json_request, json_response

from beaglerover import Robot, InputSource, DriveMode, robot_version_full

logger = logging.getLogger(__name__)

route = RouteTableDef()

def robot2dict(robot: Robot) -> Dict:
    return {

    }


@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(robot2dict(robot))


@route.get("/properties")
async def properties(request: Request) -> Response:
    robot = request.config_dict["robot"]
    properties = robot.properties
    res = dict()
    for group in properties.groups():
        res[group] = properties[group]
    return json_response(res)

@route.put("/properties")
async def properties_update(request: Request) -> Response:
    robot = request.config_dict["robot"]
    properties = robot.properties
    json = await request.json()
    for k,v in json.items():
        properties.update(k, v)
    return Response()

@route.get("/versions")
async def versions(request: Request) -> Response:
    return json_response({
        "version": request.config_dict["version"],
        "beaglerover": robot_version_full(),
    })


@route.get('/openapi.yaml')
async def openapi(request: Request) -> Response:
    file = Path(__file__).parent / "data" / "openapi.yaml"
    return FileResponse(file)




async def app_on_startup(app: Application):
    logger.info("Startup")
    robot = Robot()
    robot.init()
    robot.input.led_source = InputSource.RC
    robot.input.axis_source = InputSource.RC
    robot.input.kinematic_source = InputSource.RC
    robot.kinematic.drive_mode = DriveMode.ALL
    app['robot'] = robot


async def app_on_cleanup(app: Application):
    logger.info("Cleanup")
    app["robot"] = None


async def app_on_shutdown(app: Application):
    logger.info("Shutdown")
    robot = app["robot"]
    if robot:
        robot.cleanup()


def create_app(root: Application, sio: AsyncServer) -> Application:
    app = Application(middlewares=[error_middleware])
    app.on_startup.append(app_on_startup)
    app.on_cleanup.append(app_on_cleanup)
    app.on_shutdown.append(app_on_shutdown)

    app.add_routes(route)
    app.add_subapp("/input", input_create_app(app, sio))
    app.add_subapp("/kinematic", kinematic_create_app(app, sio))
    app.add_subapp("/motors", motors_create_app(app, sio))
    app.add_subapp("/telemetry", telemetry_create_app(app, sio))
    app.add_subapp("/leds", leds_create_app(app, sio))
    app.add_subapp("/rcreceiver", rc_create_app(app, sio))
    app.add_subapp("/system", system_create_app(app, sio))
    
    return app

