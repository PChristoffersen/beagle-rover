import logging
import concurrent
import threading
from typing import List
from dataclasses import dataclass
from socketio import AsyncNamespace


logger = logging.getLogger(__name__)


def to_enum(enum_type, value):
    if isinstance(value, int):
        return enum_type(value)
    for k, v in enum_type.values.items():
        if value == str(v):
            return v
    raise ValueError(f"Unknown enum value {value}")



@dataclass
class Watch:
    n_watches: int
    lock: threading.Lock
    future: concurrent.futures.Future
    name: str
    
    def decrement(self):
        self.n_watches = self.n_watches - 1

    def increment(self):
        self.n_watches = self.n_watches + 1


class WatchableNamespace(AsyncNamespace):
    NAME = None
    WATCH_TYPE = Watch

    def __init__(self):
        super().__init__(namespace=self.NAME)
        self.__watches = dict()

    @property
    def n_watches(self) -> int:
        return sum(w.n_watches for k,w in self.__watches.items())

    def _init_watches(self, args: List[any]):
        for arg in args:
            if isinstance(arg, str):
                self.__watches[arg] = self.WATCH_TYPE(0, threading.Lock(), None, arg)
            else:
                self.__watches[arg[0]] = self.WATCH_TYPE(0, threading.Lock(), None, *arg)

    def _destroy_watches(self):
        pass

    def _init_session(self, session: dict):
        session["subscriptions"] = set()

    def _destroy_session(self, session: dict):
        subscriptions = session["subscriptions"]
        for sub in subscriptions:
            logger.info(f"Dest watch {sub}")
            watch = self.__watches[sub]
            watch.decrement()
            self._watch_updated(watch)

        
    async def on_add_watch(self, sid, sub: str):
        logger.info(f"Add watch: {sid} {sub}")
        if sub in self.__watches:
            async with self.session(sid) as session:
                subscriptions = session["subscriptions"]
                if sub in subscriptions:
                    return
                watch = self.__watches[sub]
                self.enter_room(sid, sub, namespace=self.NAME)
                subscriptions.add(sub)
                watch.increment()
                self._watch_updated(watch)
            return self._watch_data(watch)

    async def on_remove_watch(self, sid, sub: str):
        logger.info(f"Remove watch: {sid} {sub}")
        if sub in self.__watches:
            async with self.session(sid) as session:
                watch = self.__watches[sub]
                self.leave_room(sid, sub, namespace=self.NAME)
                session["subscriptions"].remove(sub)
                watch.decrement()
                self._watch_updated(watch)
                

    @property
    def watches(self) -> List[Watch]:
        return self.__watches.values()

    def find_watch(self, name: str) -> Watch:
        return self.__watches[name]

    def _watch_updated(self, watch):
        pass

    def _watch_destroyed(self, watch):
        pass

    def _watch_data(self, watch):
        return None

