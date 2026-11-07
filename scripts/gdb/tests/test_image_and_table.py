"""Tests for the dependency-free image encoder and table renderer.

Both modules use the standard library only and do not import gdb, so they are
loaded straight from their files, without the lvglgdb package around them.

Run from the GDB script root with:
    python3 -m unittest discover tests
"""

import importlib.util
import struct
import unittest
import zlib
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def _load(name, relpath):
    spec = importlib.util.spec_from_file_location(name, ROOT / relpath)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


codec = _load("image_codec", "lvglgdb/lvgl/draw/image_codec.py")
text_table = _load("text_table", "lvglgdb/lvgl/text_table.py")


def _decode_png(data):
    """Minimal PNG reader for unfiltered 8-bit RGB/RGBA images."""
    assert data[:8] == b"\x89PNG\r\n\x1a\n"
    pos, idat, ihdr = 8, b"", None
    while pos < len(data):
        (length,) = struct.unpack(">I", data[pos:pos + 4])
        tag = data[pos + 4:pos + 8]
        payload = data[pos + 8:pos + 8 + length]
        (crc,) = struct.unpack(">I", data[pos + 8 + length:pos + 12 + length])
        assert crc == zlib.crc32(tag + payload) & 0xFFFFFFFF
        if tag == b"IHDR":
            ihdr = struct.unpack(">IIBBBBB", payload)
        elif tag == b"IDAT":
            idat += payload
        pos += 12 + length
    width, height, depth, color_type = ihdr[:4]
    bpp = 4 if color_type == 6 else 3
    raw = zlib.decompress(idat)
    row = width * bpp
    pixels = b""
    for y in range(height):
        line = raw[y * (row + 1):(y + 1) * (row + 1)]
        assert line[0] == 0
        pixels += line[1:]
    return width, height, bpp, pixels


class TestConvert(unittest.TestCase):
    def test_rgb565(self):
        # 0xF800 red, 0x07E0 green, 0x001F blue, little endian.
        data = bytes([0x00, 0xF8, 0xE0, 0x07, 0x1F, 0x00])
        img = codec.convert(data, 3, 1, "RGB565")
        self.assertEqual(img.mode, "RGB")
        self.assertEqual(img.data, bytes([248, 0, 0, 0, 252, 0, 0, 0, 248]))

    def test_rgb888_is_stored_bgr(self):
        img = codec.convert(bytes([1, 2, 3]), 1, 1, "RGB888")
        self.assertEqual(img.data, bytes([3, 2, 1]))

    def test_argb8888_keeps_alpha(self):
        img = codec.convert(bytes([1, 2, 3, 4]), 1, 1, "ARGB8888")
        self.assertEqual((img.mode, img.data), ("RGBA", bytes([3, 2, 1, 4])))

    def test_xrgb8888_drops_x(self):
        img = codec.convert(bytes([1, 2, 3, 99]), 1, 1, "XRGB8888")
        self.assertEqual((img.mode, img.data), ("RGB", bytes([3, 2, 1])))

    def test_unknown_format(self):
        with self.assertRaises(ValueError):
            codec.convert(b"\x00", 1, 1, "L8")


class TestEncoders(unittest.TestCase):
    def test_png_round_trip(self):
        for mode, bpp in (("RGB", 3), ("RGBA", 4)):
            data = bytes(range(5 * 3 * bpp))
            png = codec.encode_png(codec.RawImage(5, 3, mode, data))
            self.assertEqual(_decode_png(png), (5, 3, bpp, data))

    def test_bmp_layout(self):
        # 3x2 RGB: rows padded to 12 bytes, bottom row first, B G R order.
        data = bytes(range(18))
        bmp = codec.encode_bmp(codec.RawImage(3, 2, "RGB", data))
        self.assertEqual(bmp[:2], b"BM")
        self.assertEqual(struct.unpack("<I", bmp[2:6])[0], len(bmp))
        width, height, _, bits = struct.unpack("<iiHH", bmp[18:30])
        self.assertEqual((width, height, bits), (3, 2, 24))
        pixels = bmp[54:]
        self.assertEqual(pixels[:12], bytes([11, 10, 9, 14, 13, 12, 17, 16, 15, 0, 0, 0]))
        self.assertEqual(pixels[12:24], bytes([2, 1, 0, 5, 4, 3, 8, 7, 6, 0, 0, 0]))


class TestTextTable(unittest.TestCase):
    def test_basic(self):
        table = text_table.TextTable(["#", "name"], align="l")
        table.add_row([0, "timer"])
        self.assertEqual(str(table), "\n".join([
            "+---+-------+",
            "| # | name  |",
            "+---+-------+",
            "| 0 | timer |",
            "+---+-------+",
        ]))

    def test_alignment_and_override(self):
        table = text_table.TextTable(["a", "b"], align="r", col_align={"b": "c"})
        table.add_row(["x", "y"])
        table.add_row(["long", "wide"])
        self.assertEqual(str(table).splitlines()[3:5], [
            "|    x |  y   |",
            "| long | wide |",
        ])

    def test_multiline_and_wide_chars(self):
        table = text_table.TextTable(["k", "v"], align="l")
        table.add_row(["表示", "a\nbc"])
        self.assertEqual(str(table).splitlines()[3:5], [
            "| 表示 | a  |",
            "|      | bc |",
        ])

    def test_no_rows_keeps_header(self):
        table = text_table.TextTable(["a"])
        self.assertEqual(table.rows, [])
        self.assertEqual(str(table).count("\n"), 2)

    def test_row_length_checked(self):
        table = text_table.TextTable(["a", "b"])
        with self.assertRaises(ValueError):
            table.add_row([1])


if __name__ == "__main__":
    unittest.main()
