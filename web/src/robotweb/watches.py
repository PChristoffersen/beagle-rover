import logging
import concurrent
import asyncio
from sys import exc_info
from typing import List
from dataclasses import dataclass
from socketio import AsyncNamespace

from beaglerover import Subscription


logger = logging.getLogger(__name__)


async def read_subscription(sub: Subscription) -> tuple:
    loop = asyncio.get_running_loop()
    fd = sub.fd
    fut = loop.create_future()

    def __callback():
        try:
            if fut.cancelled():
                return
            r = sub.read()
        except Exception as e:
            loop.remove_reader(fd)
            fut.set_exception(e)
        else:
            loop.remove_reader(fd)
            fut.set_result(r)

    loop.add_reader(fd, __callback)
    return await fut



@dataclass
class Watch:
    owner: AsyncNamespace
    name: str
    n_watches: int
  
    def __init__(self, owner: AsyncNamespace, name: str):
        self.owner = owner
        self.loop = owner.app.loop
        self.name = name
        self.n_watches = 0
        self.cnt_update = 0
        self.cnt_notify = 0

    def data(self):
        return None

    async def decrement(self):
        self.n_watches = self.n_watches - 1
        if self.n_watches == 0:
            self.owner.logger.info(f"No more watches")
            await self.on_stop()

    async def increment(self):
        self.n_watches = self.n_watches + 1
        if self.n_watches == 1:
            self.owner.logger.info(f"First watch on {self.name}")
            await self.on_start()

    async def on_start(self):
        pass

    async def on_stop(self):
        pass





@dataclass
class SubscriptionWatch(Watch):
    target: any
    sub: Subscription
    task: asyncio.Task

    def __init__(self, owner: AsyncNamespace, target, name: str = "update"):
        super().__init__(owner, name)
        self.target = target
        self.sub = None
        self.task = None

    async def on_start(self):
        self._target_subscribe()
        if not self.task:
            self.task = asyncio.create_task(self._task())

    async def on_stop(self):
        if self.task:
            self.task.cancel()
            await self.task
            self.task = None
        if self.sub:
            self.sub.unsubscribe()
            self.sub = None

    def _target_subscribe(self):
        if not self.sub:
            self.sub = self.target.subscribe()

    async def _task(self):
        try:
            self.owner.logger.info("StartTask")
            while True:
                res = await read_subscription(self.sub)
                #self.owner.logger.info(f"Notifications: {res}")
                if res:
                    await self.emit(res)
        except asyncio.CancelledError:
            return
        except Exception as e:
            self.owner.logger.error("Task failed", exc_info=e)
            raise


    async def emit(self, res: tuple):
        await self.owner.emit(self.name, data=self.data(), room=self.name)


class WatchableNamespace(AsyncNamespace):
    NAME = None

    def __init__(self, logger = logger):
        super().__init__(namespace=self.NAME)
        self.logger = logger
        self.__watches = dict()

    @property
    def n_watches(self) -> int:
        return sum(w.n_watches for k,w in self.__watches.items())

    async def _init_watches(self, watches: List[Watch]):
        for watch in watches:
            self.__watches[watch.name] = watch

    async def _destroy_watches(self):
        for watch in self.watches:
            await watch.on_stop()
        self.__watches = dict()

    async def _init_session(self, session: dict):
        session["subscriptions"] = set()

    async def _destroy_session(self, session: dict):
        subscriptions = session["subscriptions"]
        for sub in subscriptions:
            self.logger.info(f"Dest watch {sub}")
            watch = self.__watches[sub]
            await watch.decrement()


    # -----------------------------------------
    # Events
    # -----------------------------------------
    async def on_add_watch(self, sid, sub: str):
        self.logger.info(f"Add watch: {sid} {sub}")
        if sub in self.__watches:
            async with self.session(sid) as session:
                subscriptions = session["subscriptions"]
                if sub in subscriptions:
                    return
                watch = self.__watches[sub]
                self.enter_room(sid, sub, namespace=self.NAME)
                subscriptions.add(sub)
                await watch.increment()
            return watch.data()

    async def on_remove_watch(self, sid, sub: str):
        self.logger.info(f"Remove watch: {sid} {sub}")
        if sub in self.__watches:
            async with self.session(sid) as session:
                watch = self.__watches[sub]
                self.leave_room(sid, sub, namespace=self.NAME)
                session["subscriptions"].remove(sub)
                await watch.decrement()
                
    async def on_connect(self, sid, environ):
        self.logger.info(f"Connection  sid={sid}")
        async with self.session(sid) as session:
            await self._init_session(session)

    async def on_disconnect(self, sid):
        self.logger.info(f"Disconnect  sid={sid}")
        async with self.session(sid) as session:
            await self._destroy_session(session)

    # -----------------------------------------
    # Events
    # -----------------------------------------

    @property
    def watches(self) -> List[Watch]:
        return self.__watches.values()


