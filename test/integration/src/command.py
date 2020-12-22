import asyncio
import os
import pickle
import random
import stat
from threading import Thread
from pathlib import Path
import tempfile
import uuid


class Run(object):
    def __init__(self):
        self._environment = {}
        self._working_dir = None

    @property
    def environment(self):
        return self._environment

    @environment.setter
    def environment(self, value):
        self._environment = value

    @property
    def working_dir(self):
        return self._working_dir

    @working_dir.setter
    def working_dir(self, value):
        self._working_dir = value


class Server(Thread):
    def __init__(self, host, port):
        Thread.__init__(self)
        self._host = host
        self._port = port
        self._runs = []
        self._loop = None

    @property
    def host(self):
        return self._host

    @property
    def port(self):
        return self._port

    async def get_characteristics(self, reader, writer):
        print("hello")
        serialized = await reader.read(10000)
        data = pickle.loads(serialized)
        print("Received data!")
        run = Run()
        run.environment = data["env"]
        run.working_dir = Path(data["working_dir"])
        self._runs.append(run)

        writer.close()
        await writer.wait_closed()

    @property
    def runs(self):
        return self._runs

    def run(self):
        self._loop = asyncio.new_event_loop()
        coro = asyncio.start_server(self.get_characteristics, self._host, self._port)
        server = self._loop.run_until_complete(coro)

        self._loop.run_forever()

        server.close()
        self._loop.run_until_complete(server.wait_closed())
        self._loop.close()
        self._loop = None

    def stop(self):
        if self._loop is not None:
            self._loop.call_soon_threadsafe(self._loop.stop)


class Command(object):
    _prefix = "binary-"
    _suffix = ".exec-helper"

    def __init__(self, id, plugin_id, directory, return_code=0):
        self._id = id
        self._plugin_id = plugin_id
        self._binary = Path(directory).joinpath(self._prefix + str(uuid.uuid4()) + self._suffix)
        self._env = dict()
        self._patterns = []
        self._return_code = return_code

        port = random.randint(49152, 65535)
        self._server = Server("localhost", port)

    def __del__(self):
        self.stop()
        # self.remove()

    @property
    def id(self):
        return self._id

    @property
    def runs(self):
        return self._server.runs

    @property
    def patterns(self):
        return self._patterns

    def set_environment(self, envs):
        self._env = envs

    def add_pattern(self, pattern):
        self._patterns.append(pattern)

    def to_dict(self):
        result = dict()
        result[self._id] = self._plugin_id
        result[self._plugin_id] = dict()
        result[self._plugin_id][self._id] = dict()
        result[self._plugin_id][self._id]["command-line"] = [str(self._binary)]
        if self._env:
            result[self._plugin_id][self._id]["environment"] = self._env
        if self._patterns:
            result[self._plugin_id][self._id]["patterns"] = [
                pattern.id for pattern in self._patterns
            ]
        return result

    def write_binary(self):
        with open(self._binary, "w") as f:
            f.write("#!/usr/bin/env python3\n")
            f.write("import asyncio\n")
            f.write("import os\n")
            f.write("import pickle\n")
            f.write("import sys\n")
            f.write("\n")
            f.write("async def set_characteristics(loop):\n")
            f.write(
                "    reader,writer = await asyncio.open_connection('{host}', {port})\n".format(
                    host=self._server.host, port=self._server.port
                )
            )
            f.write("\n")
            f.write("    run_data = dict()\n")
            f.write("    run_data['env'] = { key: value for key, value in os.environ.items()}\n")
            f.write("    run_data['working_dir'] = f'{os.getcwd()}'\n")
            f.write("    serialized = pickle.dumps(run_data)\n")
            f.write("\n")
            f.write("    writer.write(serialized)\n")
            f.write("    writer.close()\n")
            f.write("    await writer.wait_closed()\n")
            f.write("\n")
            f.write("loop = asyncio.get_event_loop()\n")
            f.write("loop.run_until_complete(set_characteristics(loop))\n")
            f.write("loop.close()\n")
            f.write("sys.exit({RETURN_CODE})\n".format(RETURN_CODE=self._return_code))

        os.chmod(self._binary, stat.S_IREAD | stat.S_IEXEC)

        self._server.start()

    def remove(self):
        self._binary.unlink()

    def stop(self):
        if self._server.is_alive():
            self._server.stop()
            self._server.join()

    @staticmethod
    def _permutate_patterns(patterns):
        if not patterns:
            yield dict()
            return

        pattern = patterns[0]
        if not pattern.values:
            for permutation in Command._permutate_patterns(patterns[1:]):
                yield permutation

        for value in pattern.values:
            for permutation in Command._permutate_patterns(patterns[1:]):
                permutation[pattern.id] = value
                yield permutation

    def pattern_generator(self):
        for permutation in Command._permutate_patterns(self._patterns):
            yield permutation
