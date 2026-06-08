"""
The test script will only run the one test on micropython. It is not a
framework that can be used for other tests. This has a reduced code footprint.
entrypoint API is almost the same as the framework.


Requirements
Python >= 3.10

pillow library

add the following to the CI prior to the test running

python3 -m pip install pillow

Example command line to run the test. I suggest doing this from the root of the
binding directory. It is just a simple location to do it from.

Paths that are passed in MUST be relative to the current working directory.
python3 lib/lv_bindings/lvgl/tests/micropy_test/__init__.py --artifact-path=lib/lv_bindings/lvgl/tests/micropy_test/artifacts --mpy-path=ports/unix/build-standard/micropython

"""
import os
import time
import signal
import argparse
import binascii
import unittest
import threading
import subprocess

from PIL import Image as Image


DEBUG = 0

debug_log = None

saved_test_data = []


def format_error_data(data):
    output = ''
    for line in data.split('\n'):
        output += f'\033[31;1m{line}\033[0m\n'
    return output


def log(*args):
    args = ' '.join(repr(arg) for arg in args)
    debug_log.write(args + '\n')

    if DEBUG:
        sys.stdout.write('\033[31;1m' + args + '\033[0m\n')
        sys.stdout.flush()


BASE_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__)))
TEST_PATH = os.path.join(BASE_PATH, 'micropy.py')
IMG_PATH = os.path.join(BASE_PATH, '../ref_imgs/binding.png')

CTRL_C = b'\x03'  # 2 times to exit any running code
CTRL_D = b'\x04'  # exit paste mode committing and running what has been pasted
CTRL_E = b'\x05'  # enter paste mode

PASTE_PROMPT = b'==='
REPL_PROMPT = b'>>>'

os.environ['MICROPYINSPECT'] = '1'


class TestData:

    def __init__(self):
        self.watchdog_timer = time.time()
        self.result = None
        self.error_data = ''
        self.event = threading.Event()


class MicroPython_Test(unittest.TestCase):
    # these are here simply to make an IDE happy. Their values get dynamically
    # set when the class gets constructed
    process: subprocess.Popen = None
    exit_event: threading.Event = None

    @classmethod
    def send(cls, cmd):
        if cls.process is None:
            return

        log('<---', cmd)
        cls.process.stdin.write(cmd)
        cls.process.stdin.flush()

    @classmethod
    def read_until(cls, marker):
        micropy_data = b''
        error_data = b''

        log('MARKER', marker)

        logged = False

        while (
            not micropy_data.endswith(marker) and
            not cls.exit_event.is_set()
        ):
            try:
                char = cls.process.stdout.read(1)
            except:  # NOQA
                break

            if char:
                micropy_data += char
                logged = False
            else:
                logged = True
                log('--->', micropy_data)

            if b'\nERROR END\n' in micropy_data:
                error_data = micropy_data.split(b'\nERROR START\n')[-1].split(b'\nERROR END\n')[0]
                micropy_data = b''
                try:
                    log('---> ERROR: ', error_data.decode('utf-8'))
                except UnicodeDecodeError:
                    log('---> ERROR: ', error_data)

                logged = True
                break

        if not logged:
            log('--->', micropy_data)

        if b'\nERROR END\n' in micropy_data:
            error_data = micropy_data.split(b'\nERROR START\n')[-1].split(b'\nERROR END\n')[0]
            micropy_data = b''
            try:
                log('---> ERROR: ', error_data.decode('utf-8'))
            except UnicodeDecodeError:
                log('---> ERROR: ', error_data)

        if cls.exit_event.is_set():
            log('--EXIT EVENT SET')

        saved_test_data.append((micropy_data, error_data))

        return micropy_data.replace(marker, b''), error_data

    @classmethod
    def setUpClass(cls):
        os.chdir(os.path.dirname(__file__))
        cls.exit_event = threading.Event()

        log(f'--SETTING UP')
        cls.process = subprocess.Popen(
            ['bash'],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            stdin=subprocess.PIPE,
            env=os.environ,
            shell=True,
            preexec_fn=os.setsid
        )
        log(f'--RUNNING MICROPYTHON ({MICROPYTHON_PATH})')

        cls.send(b'cd ' + os.path.dirname(__file__).encode('utf-8') + b'\n')
        cls.send(MICROPYTHON_PATH.encode('utf-8') + b'\n')
        _, error_data = cls.read_until(b'>>>')

        if error_data:
            raise RuntimeError(error_data)

        log('--MICROPYTHON STARTED')

    @classmethod
    def tearDownClass(cls):
        log('--TEARDOWN STARTED')

        if cls.process is not None:
            cls.send(CTRL_C)
            cls.send(CTRL_C)
            cls.send(CTRL_D)

            if not cls.process.stdin.closed:
                cls.process.stdin.close()

            os.killpg(os.getpgid(cls.process.pid), signal.SIGTERM)

            cls.process.wait()

            if not cls.process.stdout.closed:
                cls.process.stdout.close()

            if not cls.process.stderr.closed:
                cls.process.stderr.close()

            cls.process = None

        log(f'--TEARDOWN FINISHED')

    def test_1_image_compare(self):
        image = Image.open(IMG_PATH)
        res_img = image.convert('RGB')
        image.close()
        res_data = list(res_img.getdata())
        res_img.close()

        with open(TEST_PATH, 'rb') as f:
            test_code = f.read()

        log(f'--RUNNING TEST ({TEST_PATH})')

        test_code = test_code.strip()

        if self.__class__.process is None:
            self.fail('MicroPython failure.')

        self.send(CTRL_E)

        _, error_data = self.read_until(PASTE_PROMPT)
        if error_data:
            self.fail(error_data)

        test_code = test_code.split(b'\r\n')

        for i, line in enumerate(test_code):
            self.send(line + b'\n')
            _, error_data = self.read_until(b'\n')

            if error_data:
                self.fail(error_data)

            time.sleep(0.002)

        # self.read_until(b'# end\n')

        def _do(td: TestData):
            self.send(CTRL_D)
            td.error_data = b''
            td.watchdog_timer = time.time()

            td.result = []

            try:
                _, td.error_data = self.read_until(b'FRAME START\n')
                td.watchdog_timer = time.time()

                lne, td.error_data = self.read_until(b'\n')

                while (
                    b'FRAME END' not in lne and
                    not td.error_data and
                    not self.__class__.exit_event.is_set()
                ):
                    td.watchdog_timer = time.time()
                    td.result.append(lne)
                    lne, td.error_data = self.read_until(b'\n')

                if td.error_data:
                    return

                if self.__class__.exit_event.is_set():
                    return

            except:  # NOQA
                import traceback

                traceback.print_exc()

                td.error_data = traceback.format_exc()
                return

            td.event.set()

        test_data = TestData()

        t = threading.Thread(
            target=_do,
            args=(test_data,)
        )
        t.daemon = True
        test_data.watchdog_timer = time.time()

        t.start()

        while (
            (time.time() - test_data.watchdog_timer) * 1000 <= 20000 and
            not test_data.event.is_set()
        ):
            test_data.event.wait(0.05)

        if not test_data.event.is_set():
            self.__class__.exit_event.set()
            # self.read_until(REPL_PROMPT)

        self.send(CTRL_C)
        self.send(CTRL_C)

        width = 800
        height = 480

        if test_data.error_data:
            self.fail(test_data.error_data)

        try:
            frame = bytearray(
                b''.join(binascii.unhexlify(lne) for lne in test_data.result)
            )

            # I don't exactly know why the byte order is backwards but it is
            frame = bytes(bytearray([
                item for j in range(0, len(frame), 3)
                for item in [frame[j + 2], frame[j + 1], frame[j]]
            ]))

            image = Image.new('RGB', (width, height))
            image.frombytes(frame)
            img = image.convert('RGB')
            image.close()

            byte_data = list(img.getdata())
            img.save(os.path.join(ARTIFACT_PATH, f'frame.png'), 'PNG')
            img.close()

            with open(os.path.join(ARTIFACT_PATH, f'frame.bin'), 'wb') as f:
                # have to flatten the data and remove the alpha
                # from the PIL image it is formatted as
                # [(r, g, b), (r, g, b)]
                f.write(bytes(bytearray([
                    item for sublist in byte_data
                    for item in sublist
                ])))

        except binascii.Error:
            error = []
            for line, err in saved_test_data:
                if err:
                    try:
                        error.append(err.decode('utf-8'))
                    except UnicodeDecodeError:
                        error.append(str(err))

            error = '\n'.join(error)
            if error:
                self.fail(format_error_data(error))
            else:
                self.fail(b'\n'.join(test_data.result))

        except:  # NOQA
            import traceback

            self.fail(traceback.format_exc())

        self.assertEqual(res_data, byte_data, 'Frames do not match')


cwd = os.path.abspath(os.getcwd())

ARTIFACT_PATH = os.path.join(cwd, 'artifacts')
MICROPYTHON_PATH = os.path.join(cwd, 'micropython')


if __name__ == '__main__':
    import sys

    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument(
        '--artifact-path',
        dest='artifact_path',
        help='path to save artifacts to',
        action='store'
    )
    arg_parser.add_argument(
        '--mpy-path',
        dest='mpy_path',
        help='path to micropython',
        action='store'
    )
    arg_parser.add_argument(
        '--debug',
        dest='debug',
        help='debug output',
        action='store_true'
    )

    args = arg_parser.parse_args()

    ARTIFACT_PATH = os.path.join(cwd, args.artifact_path)
    MICROPYTHON_PATH = os.path.join(cwd, args.mpy_path)
    DEBUG = args.debug

    if not os.path.exists(ARTIFACT_PATH):
        raise RuntimeError(f'Artifact path does not exist ({ARTIFACT_PATH})')

    if not os.path.exists(MICROPYTHON_PATH):
        raise RuntimeError(f'MicroPython binary not found ({MICROPYTHON_PATH})')

    debug_log_path = os.path.join(ARTIFACT_PATH, 'debug.log')
    debug_log = open(debug_log_path, 'w')

    unittest.main(argv=[sys.argv[0], '-v'])

    debug_log.close()
    print(f'View the debug output in "{debug_log_path}"')
