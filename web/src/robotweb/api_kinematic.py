from ast import Or
import logging
import asyncio
from aiohttp.web import Application, RouteTableDef, Request, Response, HTTPBadRequest
from socketio import AsyncServer
from dataclasses import dataclass

from beaglerover import Kinematic, Subscription, DriveMode, Orientation

from .util import to_enum
from .watches import WatchableNamespace, SubscriptionWatch
from .serializer import json_request, json_response


logger = logging.getLogger(__name__)

route = RouteTableDef()


RESET_ODOMETER_ACTION = "resetOdometer"

KINEMATIC_PROPERTIES = frozenset([
    "drive_mode",
    "orientation",
])


DRIVE_MODES = [
    { "key": str(DriveMode.NONE),        "disabled": False,  "name": "None" },
    { "key": str(DriveMode.ALL),         "disabled": False,  "name": "All wheel steer" },
    { "key": str(DriveMode.FRONT),       "disabled": False,  "name": "Front wheel steer" },
    { "key": str(DriveMode.REAR),        "disabled": False,  "name": "Rear wheel steer" },
    { "key": str(DriveMode.SKID),        "disabled": False,  "name": "Skid steer" },
    { "key": str(DriveMode.SPINNING),    "disabled": False,  "name": "Spinning" },
    { "key": str(DriveMode.BALANCING),   "disabled": False,  "name": "Balancing" },
]

ORIENTATIONS = [
    { "key": str(Orientation.NORTH), "name": "Default" },
    { "key": str(Orientation.SOUTH), "name": "Reverse" },
    { "key": str(Orientation.EAST),  "name": "Left" },
    { "key": str(Orientation.WEST),  "name": "Right" },
]


def kinematic2dict(kinematic: Kinematic) -> dict:
    return {
        "drive_mode": str(kinematic.drive_mode),
        "orientation": str(kinematic.orientation),
    }



def set_kinematic_from_dict(kinematic: Kinematic, json: dict):
    for key, value in json.items():
        if key in KINEMATIC_PROPERTIES:
            if key == "drive_mode":
                value = to_enum(DriveMode, value)
            elif key == "orientation":
                logger.info(f"SetOrientation: {value} {to_enum(Orientation, value)}")
                value = to_enum(Orientation, value)
            setattr(kinematic, key, value)


@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(kinematic2dict(robot.kinematic))

@route.put("")
async def put(request: Request) -> Response:
    robot = request.config_dict["robot"]
    kinematic = robot.kinematic
    json = await json_request(request)
    set_kinematic_from_dict(kinematic, json)
    return json_response(kinematic2dict(kinematic))


@route.post("/actions")
async def actions(request: Request) -> Response:
    robot = request.config_dict["robot"]
    action = await json_request(request)
    if "id" in action:
        id = action["id"]
        if id == RESET_ODOMETER_ACTION:
            robot.kinematic.reset_odometer()
            return Response()
    raise HTTPBadRequest()


@route.get("/drive-modes")
async def indicators(request: Request) -> Response:
    return json_response(DRIVE_MODES)

@route.get("/orientations")
async def indicators(request: Request) -> Response:
    return json_response(ORIENTATIONS)





class KinematicWatch(SubscriptionWatch):
    def data(self):
        return kinematic2dict(self.target)


class KinematicNamespace(WatchableNamespace):
    NAME = "/kinematic"
    WATCH_TYPE = KinematicWatch

    def __init__(self, app: Application):
        super().__init__(logger=logger)
        self.app = app
        self.kinematic = None

    async def app_started(self):
        robot = self.app["root"]["robot"]
        await self._init_watches([
            KinematicWatch(self, robot.kinematic)
        ])

    async def app_cleanup(self):
        await self._destroy_watches()

    # 
    # Event handlers
    #





async def app_on_startup(app: Application):
    logger.info("Startup")
    ns = app["ns"]
    await ns.app_started()


async def app_on_cleanup(app: Application):
    logger.info("Cleanup")
    ns = app["ns"]
    await ns.app_cleanup()


def create_app(root: Application, sio: AsyncServer) -> Application:
    app = Application()
    app.add_routes(route)
    app.on_startup.append(app_on_startup)
    app.on_cleanup.append(app_on_cleanup)

    app["root"] = root

    ns = KinematicNamespace(app)
    sio.register_namespace(ns)
    app["ns"] = ns

    return app