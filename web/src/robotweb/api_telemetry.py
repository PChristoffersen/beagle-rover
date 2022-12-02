import logging
import asyncio
from aiohttp.web import Application, RouteTableDef, Request, Response, HTTPNotFound
from socketio import AsyncServer
from .watches import WatchableNamespace, SubscriptionWatch
from .serializer import json_response

from robotsystem import Telemetry

logger = logging.getLogger(__name__)


def get_history(telemetry: Telemetry, name: str) -> dict:
    ts, head,buffer = getattr(telemetry, "history_"+name)
    return {
        "time": ts,
        "interval": telemetry.history_interval_ms,
        "head": head,
        "buffer": buffer,
    }


route = RouteTableDef()

@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response({})

@route.get("/imu")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(robot.telemetry.imu)

@route.get("/odometer")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(robot.telemetry.odometer)


@route.get("/history")
async def history(request: Request) -> Response:
    robot = request.config_dict["robot"]
    telemetry = robot.telemetry

    qname = request.rel_url.query.get('names', None)
    if qname:
        names = set(qname.split(','))
    else:
        names = set()

    try:
        with telemetry:
            res = dict()
            for name in names:
                res[name] = get_history(telemetry, name)

            logger.info(f"name: {type(names)} {names} ")
            return json_response(res)
    except AttributeError:
        raise HTTPNotFound()


@route.get("/history/{name}")
async def history_named(request: Request) -> Response:
    name = request.match_info["name"]
    robot = request.config_dict["robot"]
    telemetry = robot.telemetry
    try:
        with telemetry:
            return json_response(get_history(telemetry, name))
    except AttributeError:
        raise HTTPNotFound()



class IMUWatch(SubscriptionWatch):
    UPDATE_GRACE_PERIOD = 0.5

    def data(self):
        return self.target.imu

    def _target_subscribe(self):
        if not self.sub:
            self.sub = self.target.subscribe( (self.target.NOTIFY_IMU,) )

    async def emit(self, res: tuple):
        await super().emit(res)
        await asyncio.sleep(self.UPDATE_GRACE_PERIOD)


class OdometerWatch(SubscriptionWatch):
    UPDATE_GRACE_PERIOD = 0.5

    def data(self):
        return self.target.odometer

    def _target_subscribe(self):
        if not self.sub:
            self.sub = self.target.subscribe( (self.target.NOTIFY_ODOMETER,) )

    async def emit(self, res: tuple):
        await super().emit(res)
        await asyncio.sleep(self.UPDATE_GRACE_PERIOD)


class TelemetryNamespace(WatchableNamespace):
    NAME = "/telemetry"

    def __init__(self, app: Application):
        super().__init__(logger=logger)
        self.app = app

    async def app_started(self):
        robot = self.app["root"]["robot"]
        logger.info(f"Robot: {robot} {robot.telemetry}")
        await self._init_watches([
            IMUWatch(self, robot.telemetry, f"update_imu"),
            OdometerWatch(self, robot.telemetry, f"update_odometer")
        ])


    async def app_cleanup(self):
        await self._destroy_watches()




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
    app["sio"] = sio

    ns = TelemetryNamespace(app)
    sio.register_namespace(ns)
    app["ns"] = ns

    return app
