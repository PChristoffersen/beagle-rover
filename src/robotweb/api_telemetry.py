import logging
import asyncio
from aiohttp.web import Application, RouteTableDef, Request, Response
from socketio import AsyncServer
from .watches import WatchableNamespace, SubscriptionWatch
from .serializer import json_response

logger = logging.getLogger(__name__)


route = RouteTableDef()

@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(robot.telemetry.values)

@route.get("/imu")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(robot.telemetry.imu)




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


class TelemetryNamespace(WatchableNamespace):
    NAME = "/telemetry"

    def __init__(self, app: Application):
        super().__init__(logger=logger)
        self.app = app

    async def app_started(self):
        robot = self.app["root"]["robot"]
        logger.info(f"Robot: {robot} {robot.telemetry}")
        await self._init_watches([
            IMUWatch(self, robot.telemetry, f"update_imu")
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
