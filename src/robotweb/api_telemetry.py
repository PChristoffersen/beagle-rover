import logging
from aiohttp.web import Application, RouteTableDef, Request, Response
from socketio import AsyncServer

from .serializer import json_request, json_response

logger = logging.getLogger(__name__)


route = RouteTableDef()

@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(robot.telemetry.values)

@route.get("/version")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response({ "version": robot.telemetry.values_version })


def create_app(root: Application, sio: AsyncServer) -> Application:
    app = Application()
    app.add_routes(route)

    return app