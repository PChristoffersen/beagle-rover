from json import JSONDecodeError
import logging
import asyncio
from aiohttp.web import Application, RouteTableDef, Request, Response, json_response
from socketio import AsyncServer
from dataclasses import dataclass

from robotsystem import InputControl, InputSource, InputInterface, Subscription

from .util import to_enum
from .watches import WatchableNamespace, Watch, SubscriptionWatch


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



class InputWatch(SubscriptionWatch):
    def data(self):
        return input2dict(self.target)


class InputStateWatch(Watch):
    UPDATE_GRACE_PERIOD = 0.2

    def data(self):
        return state2dict(self.owner.input_state)

    async def emit(self):
        await self.owner.emit(self.name, data=self.data(), room=self.name)
        await asyncio.sleep(self.UPDATE_GRACE_PERIOD)


class InputNamespace(WatchableNamespace):
    NAME = "/input"

    def __init__(self, app: Application):
        super().__init__(logger=logger)
        self.app = app
        self.web_source = None
        self.input = None
        self.watch = None
        self.state_watch = None
        self.input_state = None

    async def app_started(self):
        robot = self.app["root"]["robot"]
        self.input_state = self.app["state"]
        self.watch = InputWatch(self, robot.input)
        self.state_watch = InputStateWatch(self, "update_state")
        self.web_source = robot.input.web
        await self._init_watches([
            self.watch,
            self.state_watch,
        ])

    async def app_cleanup(self):
        await self._destroy_watches()
        self.input_state = None
        self.watch = None
        self.state_watch = None

    def notify_state(self):
        self.state_watch.notify()


    # 
    # Event handlers
    #

    async def on_connect(self, sid, environ):
        logger.info(f"Input connection  sid={sid}")
        async with self.session(sid) as session:
            await self._init_session(session)

    async def on_disconnect(self, sid):
        logger.info(f"Input disconnect  sid={sid}")
        async with self.session(sid) as session:
            await self._destroy_session(session)
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





async def app_on_startup(app: Application):
    logger.info("Startup")

    app["state"] = InputState(controller=None, steering=0.0, throttle=0.0, aux_x=0.0, aux_y=0.0)

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

    ns = InputNamespace(app)
    sio.register_namespace(ns)
    app["ns"] = ns

    return app
