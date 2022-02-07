import logging
import asyncio
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response
from socketio import AsyncServer
from dataclasses import dataclass

from robotsystem import Kinematic, Subscription, DriveMode, Orientation

from .util import WatchableNamespace, Watch, to_enum


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


@dataclass
class KinematicWatch(Watch):
    kinematic: Kinematic
    sub: Subscription
    update: int
    notify: int


class KinematicNamespace(WatchableNamespace):
    NAME = "/kinematic"
    WATCH_TYPE = KinematicWatch

    def __init__(self, app: Application):
        super().__init__()
        self.app = app

    def app_started(self):
        robot = self.app["root"]["robot"]
        self._init_watches([
            ("update",       robot.kinematic, None, 0, 0),
        ])

    def app_cleanup(self):
        self._destroy_watches()

    # 
    # Event handlers
    #

    async def on_connect(self, sid, environ):
        logger.info(f"Kinematic connection  sid={sid}")
        async with self.session(sid) as session:
            self._init_session(session)

    async def on_disconnect(self, sid):
        logger.info(f"Kinematic disconnect  sid={sid}")
        async with self.session(sid) as session:
            self._destroy_session(session)


    # 
    # Private
    #

    def _watch_updated(self, watch: KinematicWatch):
        logger.info(f"Watch updated {watch.name}")
        if watch.n_watches == 1:
            logger.info(f"First watch on kinematic")
            if not watch.sub:
                watch.sub = watch.kinematic.subscribe(lambda what: self.__on_notify_thread(watch))
        elif watch.n_watches <= 0:
            logger.info(f"No more watches on kinematic")
            self.__stop_watch(watch)

    def _watch_destroyed(self, watch: KinematicWatch):
        logger.info(f"Watch destroyed {watch.name}")
        self.__stop_watch(self)
        watch.kinematic = None

    def _watch_data(self, watch: KinematicWatch):
        return kinematic2dict(watch.kinematic)

    def __stop_watch(self, watch: KinematicWatch):
        if watch.sub:
            watch.sub.unsubscribe()
            watch.sub = None
        if watch.future:
            watch.future.cancel()
            watch.future = None

    def __on_notify_thread(self, watch: KinematicWatch):
        with watch.lock:
            watch.update += 1
            if not watch.future:
                watch.future = asyncio.run_coroutine_threadsafe(self.__on_notify(watch), self.app.loop)


    async def __on_notify(self, watch: KinematicWatch):
        kinematic = watch.kinematic
        while True:
            with watch.lock:
                update = watch.update
                to_update = watch.update != watch.notify
                if not to_update:
                    watch.future = None
                    break
                watch.notify = update
                
            data = kinematic2dict(kinematic)
            await self.emit(watch.name, data=data, room=watch.name)




async def app_on_startup(app: Application):
    logger.info("Startup")
    ns = app["ns"]
    ns.app_started()


async def app_on_cleanup(app: Application):
    logger.info("Cleanup")
    ns = app["ns"]
    ns.app_cleanup()


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