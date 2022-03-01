import logging
import asyncio
from aiohttp.web import Application, RouteTableDef, Request, Response
from socketio import AsyncServer
from dataclasses import dataclass

from robotsystem import Robot, Network, Power, PowerSource, PowerSourceBattery

from .util import to_enum
from .watches import WatchableNamespace, SubscriptionWatch
from .serializer import json_request, json_response


logger = logging.getLogger(__name__)

route = RouteTableDef()


SYSTEM_PROPERTIES = frozenset([
])


def system2dict(robot: Robot) -> dict:
    return {
        "network": network2dict(robot.network),
        "power": power2dict(robot.power)
    }


def network2dict(network: Network) -> dict:
    return {

    }


def power2dict(power: Power) -> dict:
    return {
        "system": powersource2dict(power.system)
    }

def powersource2dict(power_source: PowerSource) -> dict:
    res = {
        "type": str(power_source.type)
    }
    if isinstance(power_source, PowerSourceBattery):
        res["charging"] = power_source.charging
        res["on_battery"] = power_source.on_battery
        res["jack_voltage"] = power_source.jack_voltage
        res["percent"] = power_source.percent
        res["voltage"] = power_source.voltage

    return res
    


@route.get("")
async def index(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(system2dict(robot))




class SystemWatch(SubscriptionWatch):
    POWER_OFFSET = 1000
    GRACE_PERIOD = 2.0
    
    def data(self):
        data = system2dict(self.target)
        return data

    def _target_subscribe(self):
        if not self.sub:
            self.sub = self.target.network.subscribe()
            self.sub = self.target.power.subscribe(self.sub, self.POWER_OFFSET)


    async def emit(self, res: tuple):
        data = dict()
        for id in res:
            if id == Network.NOTIFY_DEFAULT:
                data["network"] = data.update(network2dict(self.target.network))
            if id == self.POWER_OFFSET+Power.NOTIFY_DEFAULT:
                data["power"] = power2dict(self.target.power)
        
        #logger.info(f"{data}")
        await self.owner.emit(self.name, data=data, room=self.name)
        await asyncio.sleep(self.GRACE_PERIOD)



class SystemNamespace(WatchableNamespace):
    NAME = "/system"

    def __init__(self, app: Application):
        super().__init__(logger=logger)
        self.app = app

    async def app_started(self):
        robot = self.app["root"]["robot"]
        await self._init_watches([
            SystemWatch(self, robot)
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

    ns = SystemNamespace(app)
    sio.register_namespace(ns)
    app["ns"] = ns

    return app