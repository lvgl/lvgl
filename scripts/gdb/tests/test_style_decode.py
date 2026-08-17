"""Tests for the style value decoders.

The decoders are pure functions, so they are tested without a live target. GDB
is stubbed because importing lvglgdb.value needs a subclassable gdb.Value.

Run from the GDB script root with:
    python3 -m unittest discover tests
"""

import sys
import types
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def _install_stubs():
    """Make lvglgdb.lvgl.misc.lv_style importable outside GDB."""
    if "gdb" not in sys.modules:
        gdb = types.ModuleType("gdb")

        class _Error(Exception):
            pass

        class _Value:
            def __init__(self, *args, **kwargs):
                pass

        class _Type:
            pass

        gdb.Value = _Value
        gdb.Type = _Type
        gdb.error = _Error
        gdb.MemoryError = _Error
        for code, name in enumerate(
            ("PTR", "STRUCT", "UNION", "INT", "ENUM", "ARRAY", "TYPEDEF")
        ):
            setattr(gdb, f"TYPE_CODE_{name}", code)
        sys.modules["gdb"] = gdb

    # Bind the packages to their directories without running their __init__,
    # which pulls in the GDB command layer.
    for name in ("lvglgdb", "lvglgdb.lvgl", "lvglgdb.lvgl.misc"):
        if name not in sys.modules:
            module = types.ModuleType(name)
            module.__path__ = [str(ROOT.joinpath(*name.split(".")))]
            sys.modules[name] = module


sys.path.insert(0, str(ROOT))
_install_stubs()

from lvglgdb.lvgl.misc.lv_style import decode_coord, decode_enum  # noqa: E402
from lvglgdb.lvgl.misc.lv_style_consts import (  # noqa: E402
    COORD_PROPS,
    COORD_TYPE_SHIFT,
    ENUM_PROP_VALUES,
    POINTER_PROPS,
    STYLE_PROP_NAMES,
)

SPEC = 1 << COORD_TYPE_SHIFT
COORD_MAX = SPEC - 1
PCT_POS_MAX = (COORD_MAX - 1) // 2


def lv_pct(value: int) -> int:
    """The LV_PCT() macro, so the tests encode the way LVGL does."""
    stored = PCT_POS_MAX - value if value < 0 else value
    return SPEC | stored


def prop_id(name: str) -> int:
    return next(k for k, v in STYLE_PROP_NAMES.items() if v == name)


class TestDecodeCoord(unittest.TestCase):
    def test_plain_pixels(self):
        self.assertEqual(decode_coord(0), "0")
        self.assertEqual(decode_coord(100), "100")
        self.assertEqual(decode_coord(32767), "32767")

    def test_negative_pixels(self):
        self.assertEqual(decode_coord(-5), "-5")
        self.assertEqual(decode_coord(-1), "-1")

    def test_percent(self):
        self.assertEqual(decode_coord(lv_pct(100)), "100%")
        self.assertEqual(decode_coord(lv_pct(50)), "50%")
        self.assertEqual(decode_coord(lv_pct(0)), "0%")

    def test_negative_percent(self):
        self.assertEqual(decode_coord(lv_pct(-20)), "-20%")

    def test_size_content(self):
        self.assertEqual(decode_coord(SPEC | COORD_MAX), "content")

    def test_measured_value_from_a_real_target(self):
        """536871012 is what a widgets-demo screen actually reports for HEIGHT."""
        self.assertEqual(decode_coord(536871012), "100%")


class TestDecodeEnum(unittest.TestCase):
    def test_plain_enum(self):
        flex_flow = prop_id("FLEX_FLOW")
        self.assertEqual(decode_enum(flex_flow, 0), "ROW")
        self.assertEqual(decode_enum(flex_flow, 1), "COLUMN")

    def test_bitmask_is_decomposed(self):
        border_side = prop_id("BORDER_SIDE")
        names = ENUM_PROP_VALUES[border_side]
        bottom = next(bit for bit, n in names.items() if n == "BOTTOM")
        top = next(bit for bit, n in names.items() if n == "TOP")
        self.assertEqual(decode_enum(border_side, bottom | top), "BOTTOM|TOP")

    def test_exact_match_wins_over_decomposition(self):
        border_side = prop_id("BORDER_SIDE")
        full = next(bit for bit, n in ENUM_PROP_VALUES[border_side].items()
                    if n == "FULL")
        self.assertEqual(decode_enum(border_side, full), "FULL")

    def test_unknown_value_falls_back_to_the_number(self):
        self.assertEqual(decode_enum(prop_id("FLEX_FLOW"), 99), "99")


class TestGeneratedTables(unittest.TestCase):
    def test_sizes_are_classified_as_coordinates(self):
        for name in ("WIDTH", "HEIGHT", "MIN_WIDTH", "MAX_HEIGHT", "PAD_LEFT"):
            self.assertIn(prop_id(name), COORD_PROPS, name)

    def test_grid_templates_are_pointers_not_numbers(self):
        for name in ("GRID_COLUMN_DSC_ARRAY", "GRID_ROW_DSC_ARRAY"):
            self.assertIn(prop_id(name), POINTER_PROPS, name)

    def test_a_property_is_never_both_a_coordinate_and_an_enum(self):
        self.assertEqual(COORD_PROPS & set(ENUM_PROP_VALUES), set())


if __name__ == "__main__":
    unittest.main()
