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

__all__ = ('announce', 'announce_start', 'announce_finish', 'announce_set_silent_mode')
_announce_start_time: datetime.datetime
_announce_silent_mode: bool = False


def _announce(file: str, args: tuple, start=False):
    if _announce_silent_mode:
        return

    _args = []

    for arg in args:
        # Avoid the single quotes `repr()` puts around strings.
        if type(arg) is str:
            _args.append(arg)
        else:
            _args.append(repr(arg))

    if start:
        _end = ''
    else:
        _end = '\n'

    print(f'{os.path.basename(file)}: ', ' '.join(_args), end=_end, flush=True)


def announce(file: str, *args, start=False, finish=False):
    global _announce_start_time
    _announce_start_time = None
    _announce(file, args)


def announce_start(file: str, *args, start=False, finish=False):
    global _announce_start_time
    _announce_start_time = datetime.datetime.now()
    _announce(file, args, start=True)


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
