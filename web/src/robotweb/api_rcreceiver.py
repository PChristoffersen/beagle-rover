import logging
import asyncio
from aiohttp.web import Application, RouteTableDef, Request, Response
from socketio import AsyncServer
from dataclasses import dataclass

from beaglerover import RCReceiver, Subscription

from .util import to_enum
from .watches import WatchableNamespace, SubscriptionWatch
from .serializer import json_request, json_response


logger = logging.getLogger(__name__)

route = RouteTableDef()

RC_PROPERTIES = frozenset([
    "enabled"
])


def rc2dict(rc: RCReceiver) -> dict:
    if rc is None:
        return {
            "enabled": False,
            "connected": False,
            "rssi": 0,
        }
    return {
        "enabled": rc.enabled,
        "connected": rc.connected,
        "rssi": rc.rssi,
    }


def set_rc_from_dict(rc: RCReceiver, json: dict):
    if rc is None:
        return
    for key, value in json.items():
        if key in RC_PROPERTIES:
            setattr(rc, key, value)



@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(rc2dict(robot.rc_receiver))


@route.put("")
async def put(request: Request) -> Response:
    robot = request.config_dict["robot"]
    rc = robot.rc_receiver
    json = await json_request(request)
    set_rc_from_dict(rc, json)
    return json_response(rc2dict(rc))


@route.get("/channels")
async def channels(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(robot.rc_receiver.channels)




class RCWatch(SubscriptionWatch):
    UPDATE_GRACE_PERIOD = 0.5

    def data(self):
        return rc2dict(self.target)

    def _target_subscribe(self):
        if not self.sub:
            self.sub = self.target.subscribe( (self.target.NOTIFY_DEFAULT,) )

    async def emit(self, res: tuple):
        await super().emit(res)
        await asyncio.sleep(self.UPDATE_GRACE_PERIOD)


class RCChannelWatch(SubscriptionWatch):
    UPDATE_GRACE_PERIOD = 0.1

    def data(self):
        return self.target.channels

    def _target_subscribe(self):
        if not self.sub:
            self.sub = self.target.subscribe( (self.target.NOTIFY_CHANNELS,) )

    async def emit(self, res: tuple):
        await super().emit(res)
        await asyncio.sleep(self.UPDATE_GRACE_PERIOD)


class RCNamespace(WatchableNamespace):
    NAME = "/rcreceiver"

    def __init__(self, app: Application):
        super().__init__(logger=logger)
        self.app = app

    async def app_started(self):
        robot = self.app["root"]["robot"]
        await self._init_watches([
            RCWatch(owner=self, target=robot.rc_receiver),
            RCChannelWatch(owner=self, target=robot.rc_receiver, name="update_channels")
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

    ns = RCNamespace(app)
    sio.register_namespace(ns)
    app["ns"] = ns

    return app