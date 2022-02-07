import logging
import asyncio
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response
from socketio import AsyncServer
from dataclasses import dataclass

from robotsystem import RCReceiver, Subscription

from .util import WatchableNamespace, Watch, to_enum


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
    json = await request.json()
    set_rc_from_dict(rc, json)
    return json_response(rc2dict(rc))




@dataclass
class RCWatch(Watch):
    rc: RCReceiver
    sub: Subscription
    update: int
    notify: int


class RCNamespace(WatchableNamespace):
    NAME = "/rcreceiver"
    WATCH_TYPE = RCWatch

    def __init__(self, app: Application):
        super().__init__()
        self.app = app

    def app_started(self):
        robot = self.app["root"]["robot"]
        self._init_watches([
            ("update",       robot.rc_receiver, None, 0, 0),
        ])

    def app_cleanup(self):
        self._destroy_watches()

    # 
    # Event handlers
    #

    async def on_connect(self, sid, environ):
        logger.info(f"RC connection  sid={sid}")
        async with self.session(sid) as session:
            self._init_session(session)

    async def on_disconnect(self, sid):
        logger.info(f"RC disconnect  sid={sid}")
        async with self.session(sid) as session:
            self._destroy_session(session)


    # 
    # Private
    #

    def _watch_updated(self, watch: RCWatch):
        logger.info(f"Watch updated {watch.name}")
        if watch.rc:
            if watch.n_watches == 1:
                logger.info(f"First watch on RC")
                if not watch.sub:
                    watch.sub = watch.rc.subscribe(lambda what: self.__on_notify_thread(watch))
            elif watch.n_watches <= 0:
                logger.info(f"No more watches on RC")
                self.__stop_watch(watch)

    def _watch_destroyed(self, watch: RCWatch):
        logger.info(f"Watch destroyed {watch.name}")
        self.__stop_watch(self)
        watch.rc = None

    def _watch_data(self, watch: RCWatch):
        return rc2dict(watch.rc)

    def __stop_watch(self, watch: RCWatch):
        if watch.sub:
            watch.sub.unsubscribe()
            watch.sub = None
        if watch.future:
            watch.future.cancel()
            watch.future = None

    def __on_notify_thread(self, watch: RCWatch):
        with watch.lock:
            watch.update += 1
            if not watch.future:
                watch.future = asyncio.run_coroutine_threadsafe(self.__on_notify(watch), self.app.loop)


    async def __on_notify(self, watch: RCWatch):
        rc = watch.rc
        while True:
            with watch.lock:
                update = watch.update
                to_update = watch.update != watch.notify
                if not to_update:
                    watch.future = None
                    break
                watch.notify = update
                
            data = rc2dict(rc)
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

    ns = RCNamespace(app)
    sio.register_namespace(ns)
    app["ns"] = ns

    return app