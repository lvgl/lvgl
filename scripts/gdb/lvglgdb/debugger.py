class Debugger(object):
    def __init__(self, host="localhost", port=11451):
        self.__host = host
        self.__port = port

    def connect_to_pycharm(self):
        try:
            import pydevd_pycharm
        except ImportError:
            print("pydevd_pycharm module not found. Please install it using pip.")
            return

        pydevd_pycharm.settrace(self.__host, port=self.__port, stdoutToServer=True, stderrToServer=True)

    def connect_to_vscode(self):
        raise NotImplementedError()

    def connect_to_eclipse(self):
        raise NotImplementedError()
