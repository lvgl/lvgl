from pathlib import Path
from typing import Optional
import gdb

from lvglgdb.value import Value, ValueInput
from .image_codec import ENCODERS, RawImage, convert, encode_png


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

    def _read_image(self, strict: bool = False) -> Optional[RawImage]:
        """Read buffer data and convert it to RGB or RGBA pixels.

        Args:
            strict: If True, raise on any size mismatch instead of ignoring.

        Returns:
            RawImage or None on failure.
        """
        header = self.super_value("header")
        stride = int(header["stride"])
        height = int(header["h"])
        cf_info = self.color_format_info()
        data_ptr = self.super_value("data")
        data_size = int(self.super_value("data_size"))
        # The width is in the header. Deriving it from the stride instead turns
        # any row padding into extra pixels, which shears the whole image.
        width = int(header["w"])
        bpp = cf_info["bpp"]
        row_bytes = (width * bpp + 7) // 8 if bpp else 0
        if not stride:
            stride = row_bytes
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
        if stride > row_bytes:
            # _convert_to_image expects rows packed back to back, so drop the
            # padding at the end of each row.
            pixel_data = b"".join(
                pixel_data[y * stride:y * stride + row_bytes]
                for y in range(height)
            )
        return self._convert_to_image(pixel_data, width, height, cf_info["name"])

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
            encoder = ENCODERS.get(output_format)
            if encoder is None:
                raise ValueError(
                    f"Unsupported image format {output_format}, use BMP or PNG"
                )
            with open(filename, "wb") as f:
                f.write(encoder(img))

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
        try:
            img = self._read_image(strict=False)
            if img is None:
                return None
            return encode_png(img)
        except Exception:
            return None

    def _convert_to_image(
        self, pixel_data: bytes, width: int, height: int, color_format: str
    ) -> Optional[RawImage]:
        """
        Convert raw pixel data to RGB or RGBA based on color format.

        Args:
            pixel_data: Raw pixel bytes, rows packed back to back
            width: Image width
            height: Image height
            color_format: LVGL color format name, e.g. "RGB565"

        Returns:
            RawImage or None if conversion fails
        """
        try:
            return convert(pixel_data, width, height, color_format)
        except Exception as e:
            print(f"Conversion error: {str(e)}")
            return None
