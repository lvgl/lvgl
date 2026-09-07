import os
import re

import gdb

import lvglgdb


def _call(expression):
    try:
        return gdb.parse_and_eval(expression)
    except gdb.error:
        return None


_VERSION_MACRO = re.compile(
    r"#define\s+LVGL_VERSION_(MAJOR|MINOR|PATCH|INFO)\s+(\S+)")


def version_from_sources():
    """The version read from the lv_version.h the target was built against.

    Neither way of asking the binary is dependable. lv_version_major() is
    `static inline`, so it is dropped unless something calls it, and the macros
    need -g3 *and* a stop inside a translation unit that included lv_version.h -
    lv_timer.c, where a breakpoint on lv_timer_handler lands, does not.

    DWARF does record the path of every source file, so the LVGL tree can be
    found through any of its symbols and the version header read from disk.
    Returns (major, minor, patch, info) or None.
    """
    for symbol in ("lv_obj_create", "lv_timer_handler", "lv_init"):
        try:
            found = gdb.lookup_global_symbol(symbol)
            if found is None or found.symtab is None:
                continue
            # DWARF records whatever separator the compiler used, which is not
            # always the host's: a MinGW build writes forward slashes.
            path = found.symtab.fullname().replace("\\", "/")
            marker = "/src/"
            if marker not in path:
                continue
            root = path[:path.rindex(marker)]
            # Since 9.6 the public headers live in include/lvgl/ and the
            # top-level lv_version.h is a deprecated wrapper that only includes
            # the real one, so the defines are not in it. Read that one first
            # and keep the top-level file for the releases that still have it.
            for header in (os.path.join(root, "include", "lvgl", "lv_version.h"),
                           os.path.join(root, "lv_version.h")):
                if not os.path.exists(header):
                    continue
                with open(header) as f:
                    macros = dict(_VERSION_MACRO.findall(f.read()))
                if not {"MAJOR", "MINOR", "PATCH"} <= set(macros):
                    continue
                return (int(macros["MAJOR"]), int(macros["MINOR"]),
                        int(macros["PATCH"]), macros.get("INFO", '""').strip('"'))
        except (gdb.error, OSError, ValueError):
            continue
    return None


def lvgl_version():
    """The LVGL version of the target, as (major, minor, patch, info).

    lv_version_major() and friends in lvgl.h are the intended way to ask, with
    the version macros as a second try. Both depend on how the target was
    built, so lv_version.h itself is the third and most dependable one.
    Fields that cannot be read come back as None.
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

    # `info` counts as missing too: a build that hides it loses the "-dev" that
    # tells a development version from a release.
    if any(p is None for p in parts) or info is None:
        from_sources = version_from_sources()
        if from_sources is not None:
            return from_sources

    return (*parts, info)


def version_string():
    """"9.6.0-dev", or as much of it as the target makes readable."""
    major, minor, patch, info = lvgl_version()
    if major is None:
        return ("unknown: lv_version_major() was inlined away, the version "
                "macros need a -g3 build, and LVGL's sources are not on disk")
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
