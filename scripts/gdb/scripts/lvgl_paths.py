"""Where LVGL is, for the generators that read its headers.

Walking a fixed number of parents up from the generator's own file only works
while the plugin sits in the layout it ships in. The plugin is also vendored
into other projects, and an installed LVGL keeps its headers somewhere else
again, so the checkout is searched for instead of assumed - and a generator that
cannot find one stops rather than writing an empty table over a good one.
"""

import os
from pathlib import Path


def _is_lvgl(path):
    return (path / "lv_version.h").is_file() and (path / "src").is_dir()


def lvgl_root(start=None):
    """The LVGL checkout to read: $LVGL_ROOT, or the nearest one above `start`."""
    from_env = os.environ.get("LVGL_ROOT")
    if from_env:
        root = Path(from_env).expanduser().resolve()
        if not _is_lvgl(root):
            raise SystemExit(
                f"LVGL_ROOT is {root}, which has no lv_version.h and src/"
            )
        return root

    here = Path(start or __file__).resolve()
    for parent in here.parents:
        if _is_lvgl(parent):
            return parent
    raise SystemExit(
        f"no LVGL checkout above {here}. Set LVGL_ROOT to one, for example\n"
        f"  LVGL_ROOT=~/lvgl python3 {Path(here).name}"
    )


def include_dir(root):
    """Where the public headers are: include/lvgl if installed, else src/."""
    installed = root / "include" / "lvgl"
    return installed if installed.is_dir() else root / "src"
