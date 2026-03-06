#!/usr/bin/env python3
"""Generate event constant tables from LVGL headers."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from enum_parser import parse_enum, generate_dict_module

LVGL_SRC = Path(__file__).parent.parent.parent.parent.parent / "src"
OUTPUT = Path(__file__).parent.parent.parent / "lvglgdb" / "lvgl" / "misc" / "lv_event_consts.py"

SKIP_EVENTS = {"LV_EVENT_LAST", "LV_EVENT_PREPROCESS", "LV_EVENT_MARKED_DELETING"}


def main():
    event_codes = parse_enum(
        LVGL_SRC / "misc" / "lv_event.h",
        "lv_event_code_t",
        "LV_EVENT_",
        skip=SKIP_EVENTS,
    )
    src = generate_dict_module(
        "event constants from LVGL headers",
        {"EVENT_CODE_NAMES": event_codes},
    )
    OUTPUT.write_text(src)
    print(f"Generated {OUTPUT.name} ({len(event_codes)} event codes)")


if __name__ == "__main__":
    main()
