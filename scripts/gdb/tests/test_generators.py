"""Tests for the generator helpers under scripts/.

These read C headers and the filesystem, so they need no target and no GDB -
unlike test_style_decode.py, nothing here imports lvglgdb.

Run from the GDB script root with:
    python3 -m unittest discover tests
"""

import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "scripts"))

from enum_parser import parse_enum  # noqa: E402
from lvgl_paths import _is_lvgl, include_dir  # noqa: E402


def write_enum(directory: Path, body: str) -> Path:
    """A header holding one typedef enum, as parse_enum expects to find it."""
    path = directory / "lv_test.h"
    path.write_text("typedef enum {\n" + body + "\n} lv_test_t;\n")
    return path


class TestParseEnum(unittest.TestCase):
    def test_a_trailing_comment_is_not_part_of_the_value(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = write_enum(Path(tmp), """
                LV_TEST_A = 0x10,  /** the first one, which runs
                                    * on to the next line */
                LV_TEST_B = 0x20,
            """)
            self.assertEqual(
                parse_enum(path, "lv_test_t", "LV_TEST_"),
                {0x10: "A", 0x20: "B"},
            )

    def test_a_comment_between_operands_does_not_truncate_the_value(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = write_enum(Path(tmp), """
                LV_TEST_A = 0x10,
                LV_TEST_B = LV_TEST_A /* keep going */ | 0x01,
            """)
            self.assertEqual(
                parse_enum(path, "lv_test_t", "LV_TEST_"),
                {0x10: "A", 0x11: "B"},
            )

    def test_a_skipped_member_can_still_be_referenced(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = write_enum(Path(tmp), """
                LV_TEST_A = 0x10,
                LV_TEST_HIDDEN = 0x20,
                LV_TEST_B = LV_TEST_HIDDEN | 0x01,
            """)
            entries = parse_enum(path, "lv_test_t", "LV_TEST_",
                                 skip={"LV_TEST_HIDDEN"})
            self.assertEqual(entries, {0x10: "A", 0x21: "B"})

    def test_an_unevaluable_value_is_an_error_not_a_guess(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = write_enum(Path(tmp), """
                LV_TEST_A = some_call(3),
            """)
            with self.assertRaises(ValueError):
                parse_enum(path, "lv_test_t", "LV_TEST_")


class TestLvglPaths(unittest.TestCase):
    def make_tree(self, tmp: str, *files: str) -> Path:
        root = Path(tmp)
        for name in files:
            path = root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text("")
        return root

    def test_a_checkout_is_found(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = self.make_tree(tmp, "lv_version.h", "src/lv_init.c")
            self.assertTrue(_is_lvgl(root))
            self.assertEqual(include_dir(root), root / "src")

    def test_an_installed_tree_is_found(self):
        """No top-level lv_version.h, only the installed public headers."""
        with tempfile.TemporaryDirectory() as tmp:
            root = self.make_tree(tmp, "include/lvgl/lv_version.h")
            self.assertTrue(_is_lvgl(root))
            self.assertEqual(include_dir(root), root / "include" / "lvgl")

    def test_an_unrelated_directory_is_not_lvgl(self):
        with tempfile.TemporaryDirectory() as tmp:
            self.assertFalse(_is_lvgl(self.make_tree(tmp, "src/main.c")))


if __name__ == "__main__":
    unittest.main()
