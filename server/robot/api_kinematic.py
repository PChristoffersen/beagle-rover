import logging
import asyncio
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response
from socketio import AsyncServer
from dataclasses import dataclass

from robotsystem import Kinematic, Subscription, DriveMode, Orientation

from .util import to_enum
from .watches import WatchableNamespace, SubscriptionWatch


logger = logging.getLogger(__name__)

route = RouteTableDef()


KINEMATIC_PROPERTIES = frozenset([
    "drive_mode",
    "orientation",
])



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
    json = await request.json()
    set_kinematic_from_dict(kinematic, json)
    return json_response(kinematic2dict(kinematic))


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