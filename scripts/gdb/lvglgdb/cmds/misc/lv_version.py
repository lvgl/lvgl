import gdb

import lvglgdb


def _call(expression):
    try:
        return gdb.parse_and_eval(expression)
    except gdb.error:
        return None


def lvgl_version():
    """The LVGL version of the target, as (major, minor, patch, info).

    lv_version_major() and friends in lvgl.h are the intended way to ask. They
    are `static inline`, so whether each one survives into the binary depends
    on the build; the version macros are used for anything missing, and those
    need -g3. Fields that cannot be read come back as None.
    """
    parts = []
    for name in ("major", "minor", "patch"):
        value = _call(f"lv_version_{name}()")
        if value is None:
            value = _call(f"LVGL_VERSION_{name.upper()}")
        parts.append(int(value) if value is not None else None)

    info = _call("lv_version_info()") or _call("LVGL_VERSION_INFO")
    try:
        info = info.string() if info is not None else None
    except gdb.error:
        info = None

    return (*parts, info)


def version_string():
    """"9.6.0-dev", or as much of it as the build makes readable."""
    major, minor, patch, info = lvgl_version()
    if major is None:
        return ("unknown: lv_version_major() was inlined away and the version "
                "macros need a -g3 build")
    numbers = ".".join("?" if p is None else str(p) for p in (major, minor, patch))
    return numbers + (f"-{info}" if info else "")


class InfoVersion(gdb.Command):
    """print the LVGL version of the target, and this plugin's version"""

    def __init__(self):
        super(InfoVersion, self).__init__(
            "info lvgl_version", gdb.COMMAND_USER, gdb.COMPLETE_NONE
        )

    def invoke(self, args, from_tty):
        print(f"LVGL:    {version_string()}")
        print(f"lvglgdb: {lvglgdb.__version__}")
