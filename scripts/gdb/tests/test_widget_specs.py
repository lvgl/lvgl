"""Tests for the widget field metadata used by `info widget`.

The module only reads widget_specs.json and formats values, so it needs no
target and no GDB - it is imported straight from its file to keep the lvglgdb
package (and its `import gdb`) out of the way.

Run from the GDB script root with:
    python3 -m unittest discover tests
"""

import importlib.util
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

_spec = importlib.util.spec_from_file_location(
    "widget_specs_under_test", ROOT / "lvglgdb" / "lvgl" / "widgets" / "specs.py"
)
specs = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(specs)


class TestSpecLookup(unittest.TestCase):
    def test_a_class_name_without_the_prefix_still_finds_its_spec(self):
        # Before LVGL 6d999331d the class names carried no 'lv_'.
        self.assertEqual(
            specs.widget_spec("label"), specs.widget_spec("lv_label")
        )

    def test_an_unknown_class_has_an_empty_spec_rather_than_raising(self):
        self.assertEqual(specs.widget_spec("lv_nope"), {})


class TestFormatValue(unittest.TestCase):
    def test_an_enum_is_named(self):
        self.assertEqual(
            specs.format_value("lv_label", "long_mode", 2), "SCROLL (2)"
        )

    def test_an_out_of_range_enum_stays_a_number(self):
        self.assertEqual(specs.format_value("lv_label", "long_mode", 99), "99")

    def test_a_bool_field_reads_as_a_bool_not_as_1(self):
        self.assertEqual(specs.format_value("lv_label", "recolor", 1), "True")

    def test_points_and_areas_are_rendered_inline(self):
        self.assertEqual(
            specs.format_value("lv_label", "offset", {"x": 1, "y": 2}), "(1,2)"
        )
        self.assertEqual(
            specs.format_value(
                "lv_slider", "left_knob_area",
                {"x1": 1, "y1": 2, "x2": 3, "y2": 4},
            ),
            "(1,2)-(3,4)",
        )

    def test_a_missing_value_is_a_dash(self):
        self.assertEqual(specs.format_value("lv_label", "text", None), "-")

    def test_a_multiline_text_stays_on_one_line(self):
        self.assertEqual(
            specs.format_value("lv_label", "text", "a\nb"), '"a\\nb"'
        )


class TestSummary(unittest.TestCase):
    def test_the_template_is_filled_from_widget_data(self):
        self.assertEqual(
            specs.summary("lv_arc", {"value": 50, "min_value": 0, "max_value": 100}),
            "50 [0..100]",
        )

    def test_a_missing_field_leaves_a_gap_rather_than_a_none(self):
        self.assertEqual(specs.summary("lv_label", {"text": None}), "")

    def test_a_long_value_is_cut(self):
        self.assertEqual(
            specs.summary("lv_label", {"text": "x" * 40}, limit=8), "xxxxxxxx…"
        )

    def test_a_widget_without_a_template_has_no_summary(self):
        self.assertEqual(specs.summary("lv_obj", {"whatever": 1}), "")


class TestOrderFields(unittest.TestCase):
    def test_primary_fields_come_first_and_nothing_is_lost(self):
        keys = ["rotation", "value", "dragging", "max_value"]
        ordered = specs.order_fields("lv_arc", keys)
        self.assertEqual(ordered[0], "value")
        self.assertEqual(sorted(ordered), sorted(keys))

    def test_an_unknown_class_keeps_the_given_order(self):
        keys = ["b", "a"]
        self.assertEqual(specs.order_fields("lv_nope", keys), keys)


if __name__ == "__main__":
    unittest.main()
