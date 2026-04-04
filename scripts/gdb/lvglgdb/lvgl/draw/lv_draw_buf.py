from pathlib import Path
from typing import Optional
import gdb

import numpy as np
from PIL import Image

from lvglgdb.value import Value, ValueInput


class LVDrawBuf(Value):
    """LVGL draw buffer"""

    _color_formats = {
        "RGB565": None,
        "RGB888": None,
        "ARGB8888": None,
        "XRGB8888": None,
    }

    _bpp = {
        "RGB565": 16,
        "RGB888": 24,
        "ARGB8888": 32,
        "XRGB8888": 32,
    }

    def __init__(self, draw_buf: ValueInput):
        super().__init__(Value.normalize(draw_buf, "lv_draw_buf_t"))
        self._init_color_formats()

    def _init_color_formats(self):
        """init color formats from enum"""
        for fmt in self._color_formats.keys():
            if self._color_formats[fmt] is None:
                try:
                    self._color_formats[fmt] = int(
                        gdb.parse_and_eval(f"LV_COLOR_FORMAT_{fmt}")
                    )
                except gdb.error:
                    print(f"Warning: Failed to get LV_COLOR_FORMAT_{fmt}")
                    self._color_formats[fmt] = -1

    def color_format_info(self) -> dict:
        """get color format info"""
        header = self.super_value("header")
        cf = int(header["cf"])

        fmt_name = "UNKNOWN"
        for name, val in self._color_formats.items():
            if val == cf:
                fmt_name = name
                break

        return {"value": cf, "name": fmt_name, "bpp": self._bpp.get(fmt_name, 0)}

    @property
    def header(self) -> dict:
        """Get the image header information as a dictionary"""
        header = self.super_value("header")
        return {
            "w": header["w"],
            "h": header["h"],
            "stride": header["stride"],
            "cf": header["cf"],  # Color format
        }

    @property
    def data_size(self) -> int:
        """Get the total buffer size in bytes"""
        return self.super_value("data_size")

    @property
    def data(self) -> bytes:
        """Get the buffer data ptr"""
        return self.super_value("data")

    def _read_image(self, strict: bool = False) -> Optional[Image.Image]:
        """Read buffer data and convert to PIL Image.

        Args:
            strict: If True, raise on any size mismatch instead of ignoring.

        Returns:
            PIL.Image or None on failure.
        """
        header = self.super_value("header")
        stride = int(header["stride"])
        height = int(header["h"])
        cf_info = self.color_format_info()
        data_ptr = self.super_value("data")
        data_size = int(self.super_value("data_size"))
        width = (stride * 8) // cf_info["bpp"] if cf_info["bpp"] else 0
        expected_data_size = stride * height

        if not data_ptr or width <= 0 or height <= 0:
            if strict:
                raise ValueError(f"Invalid buffer: ptr={data_ptr}, {width}x{height}")
            return None
        if data_size < expected_data_size:
            if strict:
                raise ValueError(
                    f"Data too small: expected at least {expected_data_size},"
                    f" got {data_size}"
                )
            return None
        if data_size > expected_data_size and strict:
            gdb.write(
                f"Warning: data_size {data_size} exceeds expected"
                f" {expected_data_size}, extra bytes will be ignored\n"
            )

        pixel_data = (
            gdb.selected_inferior()
            .read_memory(int(data_ptr), expected_data_size)
            .tobytes()
        )
        return self._convert_to_image(pixel_data, width, height, cf_info["value"])

    def data_dump(self, filename: str, format: str = None) -> bool:
        """Dump the buffer data to an image file.

        Args:
            filename: Output file path
            format: Image format (None for auto-detection from filename)

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            if not filename:
                raise ValueError("Output filename cannot be empty")

            img = self._read_image(strict=True)
            if img is None:
                return False

            output_format = (
                format.upper() if format else Path(filename).suffix[1:].upper() or "BMP"
            )
            img.save(filename, format=output_format)

            cf_info = self.color_format_info()
            print(
                f"Successfully saved {cf_info['name']} buffer as {output_format} to {filename}"
            )
            return True

        except gdb.MemoryError:
            print("Error: Failed to access memory")
            return False
        except ValueError as e:
            print(f"Validation error: {str(e)}")
            return False
        except Exception as e:
            print(f"Unexpected error: {str(e)}")
            return False

    def to_png_bytes(self) -> Optional[bytes]:
        """Convert buffer to PNG bytes in memory. Returns None on failure."""
        import io

        try:
            img = self._read_image(strict=False)
            if img is None:
                return None
            buf = io.BytesIO()
            img.save(buf, format="PNG")
            return buf.getvalue()
        except Exception:
            return None

    def _convert_to_image(
        self, pixel_data: bytes, width: int, height: int, color_format: int
    ) -> Optional[Image.Image]:
        """
        Convert raw pixel data to PIL Image based on color format.

        Args:
            pixel_data: Raw pixel bytes
            width: Image width
            height: Image height
            color_format: LVGL color format value

        Returns:
            PIL.Image or None if conversion fails
        """
        try:
            if color_format == self._color_formats["RGB565"]:
                # Convert RGB565 to RGB888
                arr = np.frombuffer(pixel_data, dtype=np.uint8)
                arr = arr.reshape((height, width, 2))
                rgb565 = np.frombuffer(pixel_data, dtype=np.uint16).reshape(
                    (height, width)
                )
                r = (((rgb565 & 0xF800) >> 11) << 3).astype(np.uint8)
                g = ((rgb565 & 0x07E0) >> 3).astype(np.uint8)
                b = ((rgb565 & 0x001F) << 3).astype(np.uint8)
                return Image.fromarray(np.dstack((r, g, b)), "RGB")

            elif color_format == self._color_formats["RGB888"]:
                arr = np.frombuffer(pixel_data, dtype=np.uint8).reshape(
                    height, width, 3
                )
                rgb_arr = arr[:, :, [2, 1, 0]]  # BGR -> RGB
                return Image.fromarray(rgb_arr, "RGB")

            elif color_format == self._color_formats["ARGB8888"]:
                # lv_color32_t memory layout: [blue, green, red, alpha]
                arr = np.frombuffer(pixel_data, dtype=np.uint8).reshape(-1, 4)
                rgba = arr[:, [2, 1, 0, 3]]  # BGRA -> RGBA
                return Image.frombytes("RGBA", (width, height), rgba.tobytes())

            elif color_format == self._color_formats["XRGB8888"]:
                arr = np.frombuffer(pixel_data, dtype=np.uint8).reshape(-1, 4)
                rgb = arr[:, [2, 1, 0]]  # BGR -> RGB
                return Image.fromarray(rgb.reshape(height, width, 3), "RGB")

            raise ValueError(f"Unsupported color format: 0x{color_format:x}")

        except Exception as e:
            print(f"Conversion error: {str(e)}")
            return None
