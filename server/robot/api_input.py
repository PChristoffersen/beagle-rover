from json import JSONDecodeError
import logging
import asyncio
from aiohttp.web import Application, RouteTableDef, Request, Response
from socketio import AsyncServer
from dataclasses import dataclass

from robotsystem import InputControl, InputSource, InputInterface, Subscription

from .util import to_enum
from .watches import WatchableNamespace, Watch, SubscriptionWatch
from .serializer import json_request, json_response


logger = logging.getLogger(__name__)


route = RouteTableDef()

INPUT_PROPERTIES = frozenset([
    "axis_source",
    "kinematic_source",
    "led_source"
])

SOURCE_PROPERTIES = frozenset([
    "steering",
    "throttle",
    "aux_x",
    "aux_y"
])


def input2dict(input) -> dict:
    return {
        "axis_source": str(input.axis_source),
        "kinematic_source": str(input.kinematic_source),
        "led_source": str(input.led_source),
    }

def interface2dict(interface: InputInterface) -> dict:
    return {
        "steering": interface.steering,
        "throttle": interface.throttle,
        "aux_x": interface.aux_x,
        "aux_y": interface.aux_y,
    }

def set_input_from_dict(input, json: dict):
    for key, value in json.items():
        if key in INPUT_PROPERTIES:
            if key in ["axis_source", "kinematic_source", "led_source"]:
                value = to_enum(InputSource, value)
            setattr(input, key, value)

def set_state_from_dict(interface: InputInterface, json: dict):
    for key, value in json.items():
        if key in SOURCE_PROPERTIES:
            setattr(interface, key, value)


@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(input2dict(robot.input))


@route.put("")
async def put(request: Request) -> Response:
    robot = request.config_dict["robot"]
    input = robot.input
    json = await json_request(request)
    set_input_from_dict(input, json)
    return json_response(input2dict(input))


@route.get("/sources")
async def animations(request: Request) -> Response:
    return json_response([ str(v) for k,v in InputSource.values.items()])


@route.get("/state")
async def steer(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(interface2dict(robot.input.web))


@route.put("/state")
async def put(request: Request) -> Response:
    robot = request.config_dict["robot"]
    interface = robot.input.web
    json = await json_request(request)
    set_state_from_dict(interface, json)
    return json_response(interface2dict(interface))



class InputWatch(SubscriptionWatch):
    def data(self):
        return input2dict(self.target)


class InputStateWatch(SubscriptionWatch):
    def data(self):
        return interface2dict(self.target)


class InputNamespace(WatchableNamespace):
    NAME = "/input"

    def __init__(self, app: Application):
        super().__init__(logger=logger)
        self.app = app

    async def app_started(self):
        robot = self.app["root"]["robot"]
        await self._init_watches([
            InputWatch(self, robot.input),
            InputStateWatch(self, robot.input.web, "update_state")
        ])

    async def app_cleanup(self):
        await self._destroy_watches()


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
        #if self.input_state.controller == sid:
        #    self.input_state.controller = None
        #    self.notify_state()

    #async def on_take_control(self, sid):
    #    logger.info(f"Take control {sid}")
    #    if self.input_state.controller != sid:
    #        self.input_state.controller = sid
    #        self.notify_state()
    #    return state2dict(self.input_state)

    #async def on_release_control(self, sid):
    #    logger.info(f"Release control {sid}")
    #    if self.input_state.controller == sid:
    #        self.input_state.controller = None
    #        self.notify_state()
    #    return state2dict(self.input_state)

    #async def on_steer(self, sid, data):
    #    logger.info(f"Steer {sid} {data}")
    #    if self.input_state.controller == sid:
    #        set_state_from_dict(self.web_source, self.input_state, data)
    #        self.notify_state()
    #    return state2dict(self.input_state)





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

    ns = InputNamespace(app)
    sio.register_namespace(ns)
    app["ns"] = ns

    return app
