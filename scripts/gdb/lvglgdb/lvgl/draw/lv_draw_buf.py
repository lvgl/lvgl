from pathlib import Path
from typing import Optional
import gdb

import numpy as np
from PIL import Image

from lvglgdb.value import Value


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

    def __init__(self, draw_buf: Value):
        super().__init__(draw_buf)
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

    def data_dump(self, filename: str, format: str = None) -> bool:
        """
        Dump the buffer data to an image file.

        Args:
            filename: Output file path
            format: Image format (None for auto-detection from filename)

        Returns:
            bool: True if successful, False otherwise
        """
        try:
            # Validate input parameters
            if not filename:
                raise ValueError("Output filename cannot be empty")

            # Get buffer metadata
            header = self.super_value("header")
            stride = int(header["stride"])
            height = int(header["h"])
            cf_info = self.color_format_info()
            data_ptr = self.super_value("data")
            data_size = int(self.super_value("data_size"))
            width = (stride * 8) // cf_info["bpp"] if cf_info["bpp"] else 0
            expected_data_size = stride * height

            # Validate buffer data
            if not data_ptr:
                raise ValueError("Data pointer is NULL")
            if width <= 0 or height <= 0:
                raise ValueError(f"Invalid dimensions: {width}x{height}")
            if data_size <= 0:
                raise ValueError(f"Invalid data size: {data_size}")
            if data_size < expected_data_size:
                raise ValueError(
                    f"Data size mismatch: expected {expected_data_size}, got {data_size}"
                )
            elif data_size > expected_data_size:
                gdb.write(
                    f"\033[93mData size mismatch: expected {expected_data_size}, got {data_size}\033[0m\n"
                )

            # Read pixel data
            pixel_data = (
                gdb.selected_inferior()
                .read_memory(int(data_ptr), expected_data_size)
                .tobytes()
            )
            if not pixel_data:
                raise ValueError("Failed to read pixel data")

            # Process based on color format
            img = self._convert_to_image(pixel_data, width, height, cf_info["value"])
            if img is None:
                return False

            # Determine output format
            output_format = (
                format.upper() if format else Path(filename).suffix[1:].upper() or "BMP"
            )

            # Save image
            img.save(filename, format=output_format)
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
                arr = np.frombuffer(pixel_data, dtype=np.uint8).reshape(-1, 4)
                rgba = np.column_stack((arr[:, 1:4], arr[:, 0]))  # ARGB -> RGBA
                return Image.frombytes("RGBA", (width, height), rgba.tobytes())

            elif color_format == self._color_formats["XRGB8888"]:
                arr = np.frombuffer(pixel_data, dtype=np.uint8).reshape(-1, 4)
                rgb = arr[:, [2, 1, 0]]  # BGR -> RGB
                return Image.fromarray(rgb.reshape(height, width, 3), "RGB")

            raise ValueError(f"Unsupported color format: 0x{color_format:x}")

        except Exception as e:
            print(f"Conversion error: {str(e)}")
            return None
