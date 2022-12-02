import logging
import asyncio
from re import I
from aiohttp.web import Application, RouteTableDef, Request, Response
from socketio import AsyncServer
from dataclasses import dataclass

from robotsystem import Robot, Network, NetworkInterface, Power, PowerSource, PowerSourceBattery

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


def interface2dict(interface: NetworkInterface, signal_only) -> dict:
    with interface:
        wifi = interface.wifi
        if signal_only:
            res = {
                "name": interface.name,
                "type": str(interface.type),
            }
            if wifi:
                res.update({
                    "signal_dbm": wifi.signal_dbm,
                    "signal_rssi": wifi.signal_rssi,
                })
        else:
            res = {
                "name": interface.name,
                "type": str(interface.type),
                "mac": interface.mac,
                "addresses": interface.addresses,
                "active": interface.active,
            }
            if wifi:
                res.update({
                    "ssid": wifi.ssid,
                    "frequency": wifi.frequency,
                    "channel": wifi.channel,
                    "channel_width": wifi.channel_width,
                    "signal_dbm": wifi.signal_dbm,
                    "signal_rssi": wifi.signal_rssi,
                })
        return res

def network2dict(network: Network, signal_only=False) -> dict:
    res = dict()
    for interface in network.interfaces():
        res[interface.name] = interface2dict(interface, signal_only)
    return res



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
    return json_response({})


@route.get("/power")
async def power(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(power2dict(robot.power))

@route.get("/network")
async def power(request: Request) -> Response:
    robot = request.config_dict["robot"]
    return json_response(network2dict(robot.network))





class PowerWatch(SubscriptionWatch):
    def data(self):
        return power2dict(self.target)

class NetworkWatch(SubscriptionWatch):
    def data(self):
        return network2dict(self.target)

    async def emit(self, res: tuple):
        if Network.NOTIFY_DEFAULT in res:
            data = network2dict(self.target)
            pass
        elif Network.NOTIFY_SIGNAL in res:
            data = network2dict(self.target, True)
            pass
        await self.owner.emit(self.name, data=data, room=self.name)



class SystemNamespace(WatchableNamespace):
    NAME = "/system"

    def __init__(self, app: Application):
        super().__init__(logger=logger)
        self.app = app

    async def app_started(self):
        robot = self.app["root"]["robot"]
        await self._init_watches([
            PowerWatch(self, robot.power, "power_update"),
            NetworkWatch(self, robot.network, "network_update")
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

    ns = SystemNamespace(app)
    sio.register_namespace(ns)
    app["ns"] = ns

    return app