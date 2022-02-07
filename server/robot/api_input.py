import logging
import asyncio
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response
from socketio import AsyncServer
from dataclasses import dataclass

from robotsystem import InputControl, InputSource, InputInterface, Subscription

from .util import WatchableNamespace, Watch, to_enum


logger = logging.getLogger(__name__)


route = RouteTableDef()

INPUT_PROPERTIES = frozenset([
    "source"
])

STATE_PROPERTIES = frozenset([
    "steering",
    "throttle",
    "aux_x",
    "aux_y"
])


@dataclass
class InputState:
    controller: str
    steering: float
    throttle: float
    aux_x: float
    aux_y: float 
   
def clamp(num, min, max):
    return min if num < min else max if num > max else num

def input2dict(input) -> dict:
    return {
        "source": str(input.source),
    }

def state2dict(state: InputState) -> dict:
    return vars(state)

def set_input_from_dict(input, json: dict):
    for key, value in json.items():
        if key in INPUT_PROPERTIES:
            if key == "source":
                value = to_enum(InputSource, value)
            setattr(input, key, value)

def set_state_from_dict(iface: InputInterface, state: InputState, json: dict):
    if "web" in json:
        new_state = json["web"]
        for key, value in new_state.items():
            if key in STATE_PROPERTIES:
                setattr(state, key, clamp(float(value), -1.0, 1.0))
        iface.steer(state.steering, state.throttle, state.aux_x, state.aux_y)


@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(input2dict(robot.input))


@route.put("")
async def put(request: Request) -> Response:
    robot = request.config_dict["robot"]
    input = robot.input
    json = await request.json()
    set_input_from_dict(input, json)
    return json_response(input2dict(input))


@route.get("/sources")
async def animations(request: Request) -> Response:
    return json_response([ str(v) for k,v in InputSource.values.items()])


@route.get("/steer")
async def steer(request: Request) -> Response:
    state = request.config_dict["state"]
    return json_response(state2dict(state))


@route.put("/steer")
async def put(request: Request) -> Response:
    robot = request.config_dict["robot"]
    state = request.config_dict["state"]
    json = await request.json()
    if state.controller is None:
        # Only set state if no one is controlling from web socket
        set_state_from_dict(robot.input.web, state, json)
        request.app["ns"].notify_state()
    return json_response(input2dict(input, state))



@dataclass
class InputWatch(Watch):
    input: InputControl
    sub: Subscription
    update: int
    notify: int


class InputNamespace(WatchableNamespace):
    NAME = "/input"
    WATCH_TYPE = InputWatch
    STATE_UPDATE_GRACE_PERIOD = 0.2

    def __init__(self, app: Application):
        super().__init__()
        self.app = app
        self.input_state = None
        self.state_watch = None
        self.web_source = None

    def app_started(self):
        robot = self.app["root"]["robot"]
        self.input_state = self.app["state"]
        self._init_watches([
            ("update",       robot.input, None, 0, 0),
            ("update_state", None,        None, 0, 0)
        ])
        self.state_watch = self.find_watch("update_state")
        self.web_source = robot.input.web

    def app_cleanup(self):
        self._destroy_watches()
        self.input_state = None


    def notify_state(self):
        with self.state_watch.lock:
            self.state_watch.update += 1
            if not self.state_watch.future:
                self.state_watch.future = asyncio.run_coroutine_threadsafe(self.__on_input_state_notify(self.state_watch), self.app.loop)


    # 
    # Event handlers
    #

    async def on_connect(self, sid, environ):
        logger.info(f"Input connection  sid={sid}")
        async with self.session(sid) as session:
            self._init_session(session)

    async def on_disconnect(self, sid):
        logger.info(f"Input disconnect  sid={sid}")
        async with self.session(sid) as session:
            self._destroy_session(session)
        if self.input_state.controller == sid:
            self.input_state.controller = None
            self.notify_state()

    async def on_take_control(self, sid):
        logger.info(f"Take control {sid}")
        if self.input_state.controller != sid:
            self.input_state.controller = sid
            self.notify_state()
        return state2dict(self.input_state)

    async def on_release_control(self, sid):
        logger.info(f"Release control {sid}")
        if self.input_state.controller == sid:
            self.input_state.controller = None
            self.notify_state()
        return state2dict(self.input_state)

    async def on_steer(self, sid, data):
        logger.info(f"Steer {sid} {data}")
        if self.input_state.controller == sid:
            set_state_from_dict(self.web_source, self.input_state, data)
            self.notify_state()
        return state2dict(self.input_state)


    # 
    # Private
    #

    def _watch_updated(self, watch: InputWatch):
        logger.info(f"Watch updated {watch.name}")
        if watch.input:
            if watch.n_watches == 1:
                logger.info(f"First watch on input")
                if not watch.sub:
                    watch.sub = watch.input.subscribe(lambda what: self.__on_input_notify_thread(watch))
            elif watch.n_watches <= 0:
                logger.info(f"No more watches on input")
                self.__stop_watch(watch)

    def _watch_destroyed(self, watch: InputWatch):
        logger.info(f"Watch destroyed {watch.name}")
        self.__stop_watch(self)
        watch.input = None

    def _watch_data(self, watch: InputWatch):
        if watch.name == "update":
            return input2dict(watch.input)
        else:
            return state2dict(self.input_state)

    def __stop_watch(self, watch: InputWatch):
        if watch.sub:
            watch.sub.unsubscribe()
            watch.sub = None
        if watch.future:
            watch.future.cancel()
            watch.future = None

    def __on_input_notify_thread(self, watch: InputWatch):
        with watch.lock:
            watch.update += 1
            if not watch.future:
                watch.future = asyncio.run_coroutine_threadsafe(self.__on_input_notify(watch), self.app.loop)


    async def __on_input_notify(self, watch: InputWatch):
        input = watch.input
        while True:
            with watch.lock:
                update = watch.update
                to_update = watch.update != watch.notify
                if not to_update:
                    watch.future = None
                    break
                watch.notify = update
                
            data = input2dict(input)
            await self.emit(watch.name, data=data, room=watch.name)


    async def __on_input_state_notify(self, watch: InputWatch):
        while True:
            with watch.lock:
                update = watch.update
                to_update = watch.update != watch.notify
                if not to_update:
                    watch.future = None
                    break
                watch.notify = update
                
            data = state2dict(self.input_state)
            await self.emit(watch.name, data=data, room=watch.name)
            
            await asyncio.sleep(self.STATE_UPDATE_GRACE_PERIOD)



async def app_on_startup(app: Application):
    logger.info("Startup")

    app["state"] = InputState(controller=None, steering=0.0, throttle=0.0, aux_x=0.0, aux_y=0.0)

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

    ns = InputNamespace(app)
    sio.register_namespace(ns)
    app["ns"] = ns

    return app
