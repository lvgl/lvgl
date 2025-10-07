import argparse

import gdb


class Debugger(gdb.Command):
    """Start debugpy server or connect to a debug server, so we can debug python code from IDE like PyCharm/VSCode"""

    def __init__(self, host="localhost", port=11451):
        self.__host = host
        self.__port = port
        super().__init__("debugger", gdb.COMMAND_USER)

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description=Debugger.__doc__)
        parser.add_argument(
            "--host",
            type=str,
            help="Server listening host",
        )
        parser.add_argument(
            "-p",
            "--port",
            type=int,
            help="Server listening port",
        )
        parser.add_argument(
            "-t",
            "--type",
            choices=["pycharm", "vscode", "eclipse"],
            help="Debugger type",
            required=True,
        )

        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        if args.host:
            self.__host = args.host

        if args.port:
            self.__port = args.port

        if args.type == "pycharm":
            self.connect_to_pycharm()
        elif args.type == "vscode":
            self.connect_to_vscode()
        elif args.type == "eclipse":
            self.connect_to_eclipse()

    def connect_to_pycharm(self):
        try:
            import pydevd_pycharm
        except ImportError:
            print("pydevd_pycharm module not found. Please install it using pip.")
            return

        pydevd_pycharm.settrace(
            self.__host, port=self.__port, stdoutToServer=True, stderrToServer=True
        )

    def connect_to_vscode(self):
        try:
            import debugpy
        except ImportError:
            print("debugpy module not found. Please install it using pip.")
            return

        debugpy.listen((self.__host, self.__port))
        debugpy.wait_for_client()

    def connect_to_eclipse(self):
        print("Eclipse is not implemented yet")
