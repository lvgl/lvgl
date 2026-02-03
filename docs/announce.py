"""announce.py
Manage logging announcements to `stdout`

It is the designer's intention that:

1.  The variable `__file__` be passed as the first argument in
    `announce()` and `announce_start()`.
    (Unfortunately, there is no way this author knows of yet to
    have this module know what Python module is importing it.  So
    this is a hold-over requirement until that need is fulfilled.)

2.  `announce_start()` and `announce_finish()` should be used
    in pairs like this:

    announce_start(__file__, 'something is running...')
    # do something that takes a while here
    announce_finish()

3.  If this is used in a module that sometimes has a need to
    not have anything output to STDOUT, when that is known,
    call `announce_set_silent_mode()`.  To turn "silent mode"
    off, call `announce_set_silent_mode(False)`.

"""
import os
import datetime

__all__ = ('announce', 'announce_colored', 'announce_start', 'announce_finish', 'announce_set_silent_mode', 'is_silent_mode')
_announce_start_time: datetime.datetime
_announce_silent_mode: bool = False
_console_color_commands = {
    'default'       : '\x1b[0m',
    'black'         : '\x1b[30m',
    'red'           : '\x1b[31m',
    'green'         : '\x1b[32m',
    'yellow'        : '\x1b[33m',
    'blue'          : '\x1b[34m',
    'majenta'       : '\x1b[35m',
    'cyan'          : '\x1b[36m',
    'white'         : '\x1b[37m',
    'bright_black'  : '\x1b[90m',
    'bright_red'    : '\x1b[91m',
    'bright_green'  : '\x1b[92m',
    'bright_yellow' : '\x1b[93m',
    'bright_blue'   : '\x1b[94m',
    'bright_majenta': '\x1b[95m',
    'bright_cyan'   : '\x1b[96m',
    'bright_white'  : '\x1b[97m'
}


def _announce(file: str, args: tuple, start: bool, box: bool, box_char: str):
    if _announce_silent_mode:
        return

    _args = []

    for arg in args:
        # Avoid the single quotes `repr()` puts around strings.
        if type(arg) is str:
            _args.append(arg)
        else:
            _args.append(repr(arg))

    msg = f'{os.path.basename(file)}: ' + ' '.join(_args)
    msg_len = len(msg)

    # `start` takes precedence over `box` argument.
    if start:
        print(msg, end='', flush=True)
    else:
        if box:
            line = box_char * msg_len
            print(line)
            print(msg)
            print(line, flush=True)
        else:
            print(msg, flush=True)


def announce(file: str, *args, box: bool = False, box_char: str = '*'):
    global _announce_start_time
    _announce_start_time = None
    _announce(file, args, False, box, box_char)


def announce_colored(file: str, clr: str, *args, box: bool = False, box_char: str = '*'):
    global _announce_start_time
    _announce_start_time = None
    if len(args) > 0 and clr in _console_color_commands:
        # Tuples are non-mutable so we have to build a new one -- can't insert new elements.
        new_args_tuple = (_console_color_commands[clr],) + args + (_console_color_commands['default'],)
        _announce(file, new_args_tuple, False, box, box_char)
    else:
        _announce(file, args, False, box, box_char)


def announce_start(file: str, *args, box: bool = False, box_char: str = '*'):
    global _announce_start_time
    _announce_start_time = datetime.datetime.now()
    _announce(file, args, True, box, box_char)


def announce_finish():
    # Just output line ending to terminate output for `announce_start()`.
    global _announce_start_time
    if _announce_start_time is not None:
        if not _announce_silent_mode:
            print('  Elapsed: ', datetime.datetime.now() - _announce_start_time, flush=True)
        _announce_start_time = None
    else:
        if not _announce_silent_mode:
            print(flush=True)


def announce_set_silent_mode(mode=True):
    global _announce_silent_mode
    _announce_silent_mode = mode


def is_silent_mode(mode=True):
    global _announce_silent_mode
    return _announce_silent_mode
