"""Pixel conversion and PNG/BMP encoding with the standard library only.

GDB embeds whatever Python it was built against, and on many setups nothing
can be installed into it, so the plugin cannot rely on numpy or Pillow. The
conversions below use bytes slicing, `bytes.translate` and big-integer bit
operations, which all run in C, so even a large framebuffer converts in a
fraction of a second.
"""

import struct
import zlib
from typing import NamedTuple


class RawImage(NamedTuple):
    """Packed pixels, row after row, top row first.

    `mode` is "RGB" (3 bytes per pixel) or "RGBA" (4 bytes per pixel).
    """

    width: int
    height: int
    mode: str
    data: bytes


_BYTES_PER_PIXEL = {"RGB": 3, "RGBA": 4}

# RGB565 split into its two bytes (little endian: lo, hi):
#   hi = RRRRRGGG   lo = GGGBBBBB
_R_FROM_HI = bytes((v & 0xF8) for v in range(256))
_G_FROM_HI = bytes(((v & 0x07) << 5) for v in range(256))
_G_FROM_LO = bytes(((v & 0xE0) >> 3) for v in range(256))
_B_FROM_LO = bytes(((v & 0x1F) << 3) for v in range(256))


def _or_bytes(a: bytes, b: bytes) -> bytes:
    """Bitwise OR of two equally long byte strings."""
    n = len(a)
    return (int.from_bytes(a, "big") | int.from_bytes(b, "big")).to_bytes(n, "big")


def _interleave(channels, count: int) -> bytes:
    out = bytearray(count * len(channels))
    step = len(channels)
    for i, ch in enumerate(channels):
        out[i::step] = ch
    return bytes(out)


def convert(pixel_data: bytes, width: int, height: int, fmt: str) -> RawImage:
    """Convert packed LVGL pixels to RGB or RGBA.

    `fmt` is the LVGL color format name without the prefix: RGB565, RGB888,
    ARGB8888 or XRGB8888. LVGL stores colors little endian, so RGB888 is laid
    out B, G, R in memory and the 32-bit formats B, G, R, A.
    """
    count = width * height
    if fmt == "RGB565":
        data = pixel_data[: count * 2]
        lo, hi = data[0::2], data[1::2]
        r = hi.translate(_R_FROM_HI)
        g = _or_bytes(hi.translate(_G_FROM_HI), lo.translate(_G_FROM_LO))
        b = lo.translate(_B_FROM_LO)
        return RawImage(width, height, "RGB", _interleave((r, g, b), count))

    if fmt == "RGB888":
        data = pixel_data[: count * 3]
        return RawImage(width, height, "RGB",
                        _interleave((data[2::3], data[1::3], data[0::3]), count))

    if fmt == "ARGB8888":
        data = pixel_data[: count * 4]
        return RawImage(width, height, "RGBA",
                        _interleave((data[2::4], data[1::4], data[0::4], data[3::4]),
                                    count))

    if fmt == "XRGB8888":
        data = pixel_data[: count * 4]
        return RawImage(width, height, "RGB",
                        _interleave((data[2::4], data[1::4], data[0::4]), count))

    raise ValueError(f"Unsupported color format: {fmt}")


def _png_chunk(tag: bytes, payload: bytes) -> bytes:
    return (struct.pack(">I", len(payload)) + tag + payload
            + struct.pack(">I", zlib.crc32(tag + payload) & 0xFFFFFFFF))


def encode_png(img: RawImage) -> bytes:
    """Encode as an 8-bit truecolor PNG (with alpha for RGBA)."""
    bpp = _BYTES_PER_PIXEL[img.mode]
    row = img.width * bpp
    # Every scanline starts with its filter type; 0 means no filter.
    scanlines = b"".join(
        b"\x00" + img.data[y * row:(y + 1) * row] for y in range(img.height)
    )
    color_type = 6 if img.mode == "RGBA" else 2
    ihdr = struct.pack(">IIBBBBB", img.width, img.height, 8, color_type, 0, 0, 0)
    return (b"\x89PNG\r\n\x1a\n"
            + _png_chunk(b"IHDR", ihdr)
            + _png_chunk(b"IDAT", zlib.compress(scanlines, 6))
            + _png_chunk(b"IEND", b""))


def encode_bmp(img: RawImage) -> bytes:
    """Encode as an uncompressed 96 DPI BMP: 24-bit for RGB, 32-bit for RGBA.

    Like Pillow, the 32-bit variant has no alpha mask, so most viewers show it
    opaque; use PNG to keep the alpha channel.
    """
    bpp = _BYTES_PER_PIXEL[img.mode]
    count = img.width * img.height
    data = img.data
    # BMP stores B, G, R(, A).
    if bpp == 3:
        swapped = _interleave((data[2::3], data[1::3], data[0::3]), count)
    else:
        swapped = _interleave((data[2::4], data[1::4], data[0::4], data[3::4]), count)

    row = img.width * bpp
    padded_row = (row + 3) & ~3
    pad = b"\x00" * (padded_row - row)
    # Rows go bottom-up.
    pixels = b"".join(
        swapped[y * row:(y + 1) * row] + pad for y in range(img.height - 1, -1, -1)
    )

    header_size = 14 + 40
    file_header = struct.pack("<2sIHHI", b"BM", header_size + len(pixels), 0, 0,
                              header_size)
    info_header = struct.pack("<IiiHHIIiiII", 40, img.width, img.height, 1,
                              bpp * 8, 0, len(pixels), 3780, 3780, 0, 0)
    return file_header + info_header + pixels


ENCODERS = {"PNG": encode_png, "BMP": encode_bmp}
