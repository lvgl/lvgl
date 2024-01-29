#!/usr/bin/env python3
import os
import logging
import argparse
import subprocess
from os import path
from enum import Enum
from typing import List
from pathlib import Path

try:
    import png
except ImportError:
    raise ImportError("Need pypng package, do `pip3 install pypng`")

try:
    import lz4.block
except ImportError:
    raise ImportError("Need lz4 package, do `pip3 install lz4`")


def uint8_t(val) -> bytes:
    return val.to_bytes(1, byteorder='little')


def uint16_t(val) -> bytes:
    return val.to_bytes(2, byteorder='little')


def uint24_t(val) -> bytes:
    return val.to_bytes(3, byteorder='little')


def uint32_t(val) -> bytes:
    try:
        return val.to_bytes(4, byteorder='little')
    except OverflowError:
        raise ParameterError(f"overflow: {hex(val)}")


def color_pre_multiply(r, g, b, a, background):
    bb = background & 0xff
    bg = (background >> 8) & 0xff
    br = (background >> 16) & 0xff

    return ((r * a + (255 - a) * br) >> 8, (g * a + (255 - a) * bg) >> 8,
            (b * a + (255 - a) * bb) >> 8, a)


class Error(Exception):

    def __str__(self):
        return self.__class__.__name__ + ': ' + ' '.join(self.args)


class FormatError(Error):
    """
    Problem with input filename format.
    BIN filename does not conform to standard lvgl bin image format
    """


class ParameterError(Error):
    """
    Parameter for LVGL image not correct
    """


class PngQuant:
    """
    Compress PNG file to 8bit mode using `pngquant`
    """

    def __init__(self, ncolors=256, dither=True, exec_path="") -> None:
        executable = path.join(exec_path, "pngquant")
        self.cmd = (f"{executable} {'--nofs' if not dither else ''} "
                    f"{ncolors}  --force - < ")

    def convert(self, filename) -> bytes:
        if not os.path.isfile(filename):
            raise BaseException(f"file not found: {filename}")

        try:
            compressed = subprocess.check_output(
                f'{self.cmd} "{str(filename)}"',
                stderr=subprocess.STDOUT,
                shell=True)
        except subprocess.CalledProcessError:
            raise BaseException(
                "cannot find pngquant tool, install it via "
                "`sudo apt install pngquant` for debian "
                "or `brew install pngquant` for macintosh "
                "For windows, you may need to download pngquant.exe from "
                "https://pngquant.org/, and put it in your PATH.")

        return compressed


class CompressMethod(Enum):
    NONE = 0x00
    RLE = 0x01
    LZ4 = 0x02


class ColorFormat(Enum):
    UNKNOWN = 0x00
    L8 = 0x06
    I1 = 0x07
    I2 = 0x08
    I4 = 0x09
    I8 = 0x0A
    A1 = 0x0B
    A2 = 0x0C
    A4 = 0x0D
    A8 = 0x0E
    ARGB8888 = 0x10
    XRGB8888 = 0x11
    RGB565 = 0x12
    RGB565A8 = 0x14
    RGB888 = 0x0F

    @property
    def bpp(self) -> int:
        """
        Return bit per pixel for this cf
        """
        cf_map = {
            ColorFormat.UNKNOWN: 0x00,
            ColorFormat.L8: 8,
            ColorFormat.I1: 1,
            ColorFormat.I2: 2,
            ColorFormat.I4: 4,
            ColorFormat.I8: 8,
            ColorFormat.A1: 1,
            ColorFormat.A2: 2,
            ColorFormat.A4: 4,
            ColorFormat.A8: 8,
            ColorFormat.ARGB8888: 32,
            ColorFormat.XRGB8888: 32,
            ColorFormat.RGB565: 16,
            ColorFormat.RGB565A8: 16,  # 16bpp + a8 map
            ColorFormat.RGB888: 24,
        }

        return cf_map[self]

    @property
    def ncolors(self) -> int:
        """
        Return number of colors in palette if cf is indexed1/2/4/8.
        Return zero if cf is not indexed format
        """

        cf_map = {
            ColorFormat.I1: 2,
            ColorFormat.I2: 4,
            ColorFormat.I4: 16,
            ColorFormat.I8: 256,
        }
        return cf_map.get(self, 0)

    @property
    def is_indexed(self) -> bool:
        """
        Return if cf is indexed color format
        """
        return self.ncolors != 0

    @property
    def is_alpha_only(self) -> bool:
        return ColorFormat.A1.value <= self.value <= ColorFormat.A8.value

    @property
    def has_alpha(self) -> bool:
        return self.is_alpha_only or self in (
            ColorFormat.ARGB8888,
            ColorFormat.XRGB8888,  # const alpha: 0xff
            ColorFormat.RGB565A8)

    @property
    def is_colormap(self) -> bool:
        return self in (ColorFormat.ARGB8888, ColorFormat.RGB888,
                        ColorFormat.XRGB8888, ColorFormat.RGB565A8,
                        ColorFormat.RGB565)

    @property
    def is_luma_only(self) -> bool:
        return self in (ColorFormat.L8,)


def unpack_colors(data: bytes, cf: ColorFormat, w) -> List:
    """
    Unpack lvgl 1/2/4/8/16/32 bpp color to png color: alpha map, grey scale,
    or R,G,B,(A) map
    """
    ret = []
    bpp = cf.bpp
    if bpp == 8:
        ret = data
    elif bpp == 4:
        if cf == ColorFormat.A4:
            values = [x * 17 for x in range(16)]
        else:
            values = [x for x in range(16)]

        for p in data:
            for i in range(2):
                ret.append(values[(p >> (4 - i * 4)) & 0x0f])
                if len(ret) % w == 0:
                    break

    elif bpp == 2:
        if cf == ColorFormat.A2:
            values = [x * 85 for x in range(4)]
        else:  # must be ColorFormat.I2
            values = [x for x in range(4)]
        for p in data:
            for i in range(4):
                ret.append(values[(p >> (6 - i * 2)) & 0x03])
                if len(ret) % w == 0:
                    break
    elif bpp == 1:
        if cf == ColorFormat.A1:
            values = [0, 255]
        else:
            values = [0, 1]
        for p in data:
            for i in range(8):
                ret.append(values[(p >> (7 - i)) & 0x01])
                if len(ret) % w == 0:
                    break
    elif bpp == 16:
        #  This is RGB565
        pixels = [(data[2 * i + 1] << 8) | data[2 * i]
                  for i in range(len(data) // 2)]

        values_5bit = [x * 8 for x in range(32)]
        values_5bit[-1] = 255
        values_6bit = [x * 4 for x in range(64)]
        values_6bit[-1] = 255
        for p in pixels:
            ret.append(values_5bit[(p >> 11) & 0x1f])  # R
            ret.append(values_6bit[(p >> 5) & 0x3f])  # G
            ret.append(values_5bit[(p >> 0) & 0x1f])  # B
    elif bpp == 24:
        if cf == ColorFormat.RGB888:
            B = data[0::3]
            G = data[1::3]
            R = data[2::3]
            for r, g, b in zip(R, G, B):
                ret += [r, g, b]
        elif cf == ColorFormat.RGB565A8:
            alpha_size = len(data) // 3
            pixel_alpha = data[-alpha_size:]
            pixel_data = data[:-alpha_size]
            pixels = [(pixel_data[2 * i + 1] << 8) | pixel_data[2 * i]
                      for i in range(len(pixel_data) // 2)]

            values_5bit = [x * 8 for x in range(32)]
            values_5bit[-1] = 255
            values_6bit = [x * 4 for x in range(64)]
            values_6bit[-1] = 255
            for a, p in zip(pixel_alpha, pixels):
                ret.append(values_5bit[(p >> 11) & 0x1f])  # R
                ret.append(values_6bit[(p >> 5) & 0x3f])  # G
                ret.append(values_5bit[(p >> 0) & 0x1f])  # B
                ret.append(a)
    elif bpp == 32:
        B = data[0::4]
        G = data[1::4]
        R = data[2::4]
        A = data[3::4]
        for r, g, b, a in zip(R, G, B, A):
            ret += [r, g, b, a]
    else:
        assert 0

    return ret


class LVGLImageHeader:

    def __init__(self,
                 cf: ColorFormat = ColorFormat.UNKNOWN,
                 w: int = 0,
                 h: int = 0,
                 stride: int = 0,
                 align: int = 1,
                 flags: int = 0):
        self.cf = cf
        self.flags = flags
        self.w = w & 0xffff
        self.h = h & 0xffff
        if w > 0xffff or h > 0xffff:
            raise ParameterError(f"w, h overflow: {w}x{h}")
        if align < 1:
            # stride align in bytes must be larger than 1
            raise ParameterError(f"Invalid stride align: {align}")

        self.stride = self.stride_align(align) if stride == 0 else stride

    def stride_align(self, align: int) -> int:
        stride = self.stride_default
        if align == 1:
            pass
        elif align > 1:
            stride = (stride + align - 1) // align
            stride *= align
        else:
            raise ParameterError(f"Invalid stride align: {align}")

        self.stride = stride
        return stride

    @property
    def stride_default(self) -> int:
        return (self.w * self.cf.bpp + 7) // 8

    @property
    def binary(self) -> bytearray:
        binary = bytearray()
        binary += uint8_t(0x19)  # magic number for lvgl version 9
        binary += uint8_t(self.cf.value)
        binary += uint16_t(self.flags)  # 16bits flags

        binary += uint16_t(self.w)  # 16bits width
        binary += uint16_t(self.h)  # 16bits height
        binary += uint16_t(self.stride)  # 16bits stride

        binary += uint16_t(0)  # 16bits reserved
        return binary

    def from_binary(self, data: bytes):
        if len(data) < 12:
            raise FormatError("invalid header length")

        try:
            self.cf = ColorFormat(data[1] & 0x1f)  # color format
        except ValueError as exc:
            raise FormatError(f"invalid color format: {hex(data[0])}") from exc
        self.w = int.from_bytes(data[4:6], 'little')
        self.h = int.from_bytes(data[6:8], 'little')
        self.stride = int.from_bytes(data[8:10], 'little')
        return self


class LVGLCompressData:

    def __init__(self,
                 cf: ColorFormat,
                 method: CompressMethod,
                 raw_data: bytes = b''):
        self.blk_size = (cf.bpp + 7) // 8
        self.compress = method
        self.raw_data = raw_data
        self.raw_data_len = len(raw_data)
        self.compressed = self._compress(raw_data)

    def _compress(self, raw_data: bytes) -> bytearray:
        if self.compress == CompressMethod.NONE:
            return raw_data

        if self.compress == CompressMethod.RLE:
            # RLE compression performs on pixel unit, pad data to pixel unit
            pad = b'\x00' * (self.blk_size - self.raw_data_len % self.blk_size)
            self.raw_data_len += len(pad)
            compressed = RLEImage().rle_compress(raw_data + pad, self.blk_size)
        elif self.compress == CompressMethod.LZ4:
            compressed = lz4.block.compress(raw_data, store_size=False)
        else:
            raise ParameterError(f"Invalid compress method: {self.compress}")

        self.compressed_len = len(compressed)

        bin = bytearray()
        bin += uint32_t(self.compress.value)
        bin += uint32_t(self.compressed_len)
        bin += uint32_t(self.raw_data_len)
        bin += compressed
        return bin


class LVGLImage:

    def __init__(self,
                 cf: ColorFormat = ColorFormat.UNKNOWN,
                 w: int = 0,
                 h: int = 0,
                 data: bytes = b'') -> None:
        self.stride = 0  # default no valid stride value
        self.set_data(cf, w, h, data)

    def __repr__(self) -> str:
        return (f"'LVGL image {self.w}x{self.h}, {self.cf.name},"
                f" (12+{self.data_len})Byte'")

    def adjust_stride(self, stride: int = 0, align: int = 1):
        """
        Stride can be set directly, or by stride alignment in bytes
        """
        if self.stride == 0:
            #  stride can only be 0, when LVGLImage is created with empty data
            logging.warning("Cannot adjust stride for empty image")
            return

        if align >= 1 and stride == 0:
            # The header with specified stride alignment
            header = LVGLImageHeader(self.cf, self.w, self.h, align=align)
            stride = header.stride
        elif stride > 0:
            pass
        else:
            raise ParameterError(f"Invalid parameter, align:{align},"
                                 f" stride:{stride}")

        if self.stride == stride:
            return  # no stride adjustment

        # if current image is empty, no need to do anything
        if self.data_len == 0:
            self.stride = 0
            return

        current = LVGLImageHeader(self.cf, self.w, self.h, stride=self.stride)

        if stride < current.stride_default:
            raise ParameterError(f"Stride is too small:{stride}, "
                                 f"minimal:{current.stride_default}")

        def change_stride(data: bytearray, h, current_stride, new_stride):
            data_in = data
            data_out = []  # stride adjusted new data
            if new_stride < current_stride:  # remove padding byte
                for i in range(h):
                    start = i * current_stride
                    end = start + new_stride
                    data_out.append(data_in[start:end])
            else:  # adding more padding bytes
                padding = b'\x00' * (new_stride - current_stride)
                for i in range(h):
                    data_out.append(data_in[i * current_stride:(i + 1) *
                                                               current_stride])
                    data_out.append(padding)
            return b''.join(data_out)

        palette_size = self.cf.ncolors * 4
        data_out = [self.data[:palette_size]]
        data_out.append(
            change_stride(self.data[palette_size:], self.h, current.stride,
                          stride))

        # deal with alpha map for RGB565A8
        if self.cf == ColorFormat.RGB565A8:
            logging.warning("handle RGB565A8 alpha map")
            a8_stride = self.stride // 2
            a8_map = self.data[-a8_stride * self.h:]
            data_out.append(
                change_stride(a8_map, self.h, current.stride // 2,
                              stride // 2))

        self.stride = stride
        self.data = b''.join(data_out)

    @property
    def data_len(self) -> int:
        """
        Return data_len in byte of this image, excluding image header
        """

        # palette is always in ARGB format, 4Byte per color
        p = self.cf.ncolors * 4 if self.is_indexed and self.w * self.h else 0
        p += self.stride * self.h
        a8_stride = self.stride // 2
        p += a8_stride * self.h if self.cf == ColorFormat.RGB565A8 else 0
        return p

    @property
    def header(self) -> bytearray:
        return LVGLImageHeader(self.cf, self.w, self.h)

    @property
    def is_indexed(self):
        return self.cf.is_indexed

    def set_data(self,
                 cf: ColorFormat,
                 w: int,
                 h: int,
                 data: bytes,
                 stride: int = 0):
        """
        Directly set LVGL image parameters
        """

        if w > 0xffff or h > 0xffff:
            raise ParameterError(f"w, h overflow: {w}x{h}")

        self.cf = cf
        self.w = w
        self.h = h

        # if stride is 0, then it's aligned to 1byte by default,
        # let image header handle it
        self.stride = LVGLImageHeader(cf, w, h, stride, align=1).stride

        if self.data_len != len(data):
            raise ParameterError(f"{self} data length error got: {len(data)}, "
                                 f"expect: {self.data_len}")

        self.data = data

        return self

    def from_data(self, data: bytes):
        header = LVGLImageHeader().from_binary(data)
        return self.set_data(header.cf, header.w, header.h,
                             data[len(header.binary):], header.stride)

    def from_bin(self, filename: str):
        """
        Read from existing bin file and update image parameters
        """

        if not filename.endswith(".bin"):
            raise FormatError("filename not ended with '.bin'")

        with open(filename, "rb") as f:
            data = f.read()
            return self.from_data(data)

    def _check_ext(self, filename: str, ext):
        if not filename.lower().endswith(ext):
            raise FormatError(f"filename not ended with {ext}")

    def _check_dir(self, filename: str):
        dir = path.dirname(filename)
        if dir and not path.exists(dir):
            logging.info(f"mkdir of {dir} for {filename}")
            os.makedirs(dir)

    def to_bin(self,
               filename: str,
               compress: CompressMethod = CompressMethod.NONE):
        """
        Write this image to file, filename should be ended with '.bin'
        """
        self._check_ext(filename, ".bin")
        self._check_dir(filename)

        with open(filename, "wb+") as f:
            bin = bytearray()
            flags = 0
            flags |= 0x08 if compress != CompressMethod.NONE else 0

            header = LVGLImageHeader(self.cf,
                                     self.w,
                                     self.h,
                                     self.stride,
                                     flags=flags)
            bin += header.binary
            compressed = LVGLCompressData(self.cf, compress, self.data)
            bin += compressed.compressed

            f.write(bin)

        return self

    def to_c_array(self,
                   filename: str,
                   compress: CompressMethod = CompressMethod.NONE):
        self._check_ext(filename, ".c")
        self._check_dir(filename)

        varname = path.basename(filename).split('.')[0]
        varname = varname.replace("-", "_")
        varname = varname.replace(".", "_")

        flags = "0"
        if compress is not CompressMethod.NONE:
            flags += " | LV_IMAGE_FLAGS_COMPRESSED"

        compressed = LVGLCompressData(self.cf, compress, self.data)

        header = f'''
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_BUILD_TEST)
#include "../lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_DUST
#define LV_ATTRIBUTE_IMG_DUST
#endif

static const
LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_DUST
uint8_t {varname}_map[] = {{
'''

        ending = f'''
}};

const lv_img_dsc_t {varname} = {{
  .header.magic = LV_IMAGE_HEADER_MAGIC,
  .header.cf = LV_COLOR_FORMAT_{self.cf.name},
  .header.flags = {flags},
  .header.w = {self.w},
  .header.h = {self.h},
  .header.stride = {self.stride},
  .data_size = {len(compressed.compressed)},
  .data = {varname}_map,
}};

'''

        def write_binary(f, data, stride):
            for i, v in enumerate(data):
                if i % stride == 0:
                    f.write("\n    ")
                f.write(f"0x{v:02x},")
            f.write("\n")

        with open(filename, "w+") as f:
            f.write(header)

            if compress is not CompressMethod.NONE:
                write_binary(f, compressed.compressed, 16)
            else:
                # write palette separately
                ncolors = self.cf.ncolors
                if ncolors:
                    write_binary(f, self.data[:ncolors * 4], 16)

                write_binary(f, self.data[ncolors * 4:], self.stride)

            f.write(ending)

        return self

    def to_png(self, filename: str):
        self._check_ext(filename, ".png")
        self._check_dir(filename)

        old_stride = self.stride
        self.adjust_stride(align=1)
        if self.cf.is_indexed:
            data = self.data
            # Separate lvgl bin image data to palette and bitmap
            # The palette is in format of [(RGBA), (RGBA)...].
            # LVGL palette is in format of B,G,R,A,...
            palette = [(data[i * 4 + 2], data[i * 4 + 1], data[i * 4 + 0],
                        data[i * 4 + 3]) for i in range(self.cf.ncolors)]

            data = data[self.cf.ncolors * 4:]

            encoder = png.Writer(self.w,
                                 self.h,
                                 palette=palette,
                                 bitdepth=self.cf.bpp)
            # separate packed data to plain data
            data = unpack_colors(data, self.cf, self.w)
        elif self.cf.is_alpha_only:
            # separate packed data to plain data
            transparency = unpack_colors(self.data, self.cf, self.w)
            data = []
            for a in transparency:
                data += [0, 0, 0, a]
            encoder = png.Writer(self.w, self.h, greyscale=False, alpha=True)
        elif self.cf == ColorFormat.L8:
            # to grayscale
            encoder = png.Writer(self.w,
                                 self.h,
                                 bitdepth=self.cf.bpp,
                                 greyscale=True,
                                 alpha=False)
            data = self.data
        elif self.cf.is_colormap:
            encoder = png.Writer(self.w,
                                 self.h,
                                 alpha=self.cf.has_alpha,
                                 greyscale=False)
            data = unpack_colors(self.data, self.cf, self.w)
        else:
            logging.warning(f"missing logic: {self.cf.name}")
            return

        with open(filename, "wb") as f:
            encoder.write_array(f, data)

        self.adjust_stride(stride=old_stride)

    def from_png(self,
                 filename: str,
                 cf: ColorFormat = None,
                 background: int = 0x00_00_00):
        """
        Create lvgl image from png file.
        If cf is none, used I1/2/4/8 based on palette size
        """

        self.background = background

        if cf is None:  # guess cf from filename
            # split filename string and match with ColorFormat to check
            # which cf to use
            names = str(path.basename(filename)).split(".")
            for c in names[1:-1]:
                if c in ColorFormat.__members__:
                    cf = ColorFormat[c]
                    break

        if cf is None or cf.is_indexed:  # palette mode
            self._png_to_indexed(cf, filename)
        elif cf.is_alpha_only:
            self._png_to_alpha_only(cf, filename)
        elif cf.is_luma_only:
            self._png_to_luma_only(cf, filename)
        elif cf.is_colormap:
            self._png_to_colormap(cf, filename)
        else:
            logging.warning(f"missing logic: {cf.name}")

        logging.info(f"from png: {filename}, cf: {self.cf.name}")
        return self

    def _png_to_indexed(self, cf: ColorFormat, filename: str):
        # convert to palette mode
        auto_cf = cf is None
        reader = png.Reader(
            bytes=PngQuant(256 if auto_cf else cf.ncolors).convert(filename))
        w, h, rows, _ = reader.read()
        palette = reader.palette(alpha="force")  # always return alpha

        palette_len = len(palette)
        if auto_cf:
            if palette_len <= 2:
                cf = ColorFormat.I1
            elif palette_len <= 4:
                cf = ColorFormat.I2
            elif palette_len <= 16:
                cf = ColorFormat.I4
            else:
                cf = ColorFormat.I8

        if palette_len != cf.ncolors:
            if not auto_cf:
                logging.warning(
                    f"{path.basename(filename)} palette: {palette_len}, "
                    f"extended to: {cf.ncolors}")
            palette += [(255, 255, 255, 0)] * (cf.ncolors - palette_len)

        # Assemble lvgl image palette from PNG palette.
        # PNG palette is a list of tuple(R,G,B,A)

        rawdata = bytearray()
        for (r, g, b, a) in palette:
            rawdata += uint32_t((a << 24) | (r << 16) | (g << 8) | (b << 0))

        # pack data if not in I8 format
        if cf == ColorFormat.I8:
            for e in rows:
                rawdata += e
        else:
            for e in png.pack_rows(rows, cf.bpp):
                rawdata += e

        self.set_data(cf, w, h, rawdata)

    def _png_to_alpha_only(self, cf: ColorFormat, filename: str):
        reader = png.Reader(str(filename))
        w, h, rows, info = reader.asRGBA8()
        if not info['alpha']:
            raise FormatError(f"{filename} has no alpha channel")

        rawdata = bytearray()
        if cf == ColorFormat.A8:
            for row in rows:
                A = row[3::4]
                for e in A:
                    rawdata += uint8_t(e)
        else:
            shift = 8 - cf.bpp
            mask = 2 ** cf.bpp - 1
            rows = [[(a >> shift) & mask for a in row[3::4]] for row in rows]
            for row in png.pack_rows(rows, cf.bpp):
                rawdata += row

        self.set_data(cf, w, h, rawdata)

    def _png_to_luma_only(self, cf: ColorFormat, filename: str):
        reader = png.Reader(str(filename))
        w, h, rows, info = reader.asRGBA8()
        rawdata = bytearray()
        for row in rows:
            R = row[0::4]
            G = row[1::4]
            B = row[2::4]
            A = row[3::4]
            for r, g, b, a in zip(R, G, B, A):
                r, g, b, a = color_pre_multiply(r, g, b, a, self.background)
                luma = 0.2126 * r + 0.7152 * g + 0.0722 * b
                rawdata += uint8_t(int(luma))

        self.set_data(ColorFormat.L8, w, h, rawdata)

    def _png_to_colormap(self, cf, filename: str):

        if cf == ColorFormat.ARGB8888:

            def pack(r, g, b, a):
                return uint32_t((a << 24) | (r << 16) | (g << 8) | (b << 0))
        elif cf == ColorFormat.XRGB8888:

            def pack(r, g, b, a):
                r, g, b, a = color_pre_multiply(r, g, b, a, self.background)
                return uint32_t((0xff << 24) | (r << 16) | (g << 8) | (b << 0))
        elif cf == ColorFormat.RGB888:

            def pack(r, g, b, a):
                r, g, b, a = color_pre_multiply(r, g, b, a, self.background)
                return uint24_t((r << 16) | (g << 8) | (b << 0))
        elif cf == ColorFormat.RGB565:

            def pack(r, g, b, a):
                r, g, b, a = color_pre_multiply(r, g, b, a, self.background)
                color = (r >> 3) << 11
                color |= (g >> 2) << 5
                color |= (b >> 3) << 0
                return uint16_t(color)
        elif cf == ColorFormat.RGB565A8:

            def pack(r, g, b, a):
                color = (r >> 3) << 11
                color |= (g >> 2) << 5
                color |= (b >> 3) << 0
                return uint16_t(color)
        else:
            assert (0)

        reader = png.Reader(str(filename))
        w, h, rows, _ = reader.asRGBA8()
        rawdata = bytearray()
        alpha = bytearray()
        for row in rows:
            R = row[0::4]
            G = row[1::4]
            B = row[2::4]
            A = row[3::4]
            for r, g, b, a in zip(R, G, B, A):
                if cf == ColorFormat.RGB565A8:
                    alpha += uint8_t(a)
                rawdata += pack(r, g, b, a)

        if cf == ColorFormat.RGB565A8:
            rawdata += alpha

        self.set_data(cf, w, h, rawdata)


class RLEHeader:

    def __init__(self, blksize: int, len: int):
        self.blksize = blksize
        self.len = len

    @property
    def binary(self):
        magic = 0x5aa521e0

        rle_header = self.blksize
        rle_header |= (self.len & 0xffffff) << 4

        binary = bytearray()
        binary.extend(uint32_t(magic))
        binary.extend(uint32_t(rle_header))
        return binary


class RLEImage(LVGLImage):

    def __init__(self,
                 cf: ColorFormat = ColorFormat.UNKNOWN,
                 w: int = 0,
                 h: int = 0,
                 data: bytes = b'') -> None:
        super().__init__(cf, w, h, data)

    def to_rle(self, filename: str):
        """
        Compress this image to file, filename should be ended with '.rle'
        """
        self._check_ext(filename, ".rle")
        self._check_dir(filename)

        # compress image data excluding lvgl image header
        blksize = (self.cf.bpp + 7) // 8
        compressed = self.rle_compress(self.data, blksize)
        with open(filename, "wb+") as f:
            header = RLEHeader(blksize, len(self.data)).binary
            header.extend(self.header.binary)
            f.write(header)
            f.write(compressed)

    def rle_compress(self, data: bytearray, blksize: int, threshold=16):
        index = 0
        data_len = len(data)
        compressed_data = []
        memview = memoryview(data)
        while index < data_len:
            repeat_cnt = self.get_repeat_count(memview[index:], blksize)
            if repeat_cnt == 0:
                # done
                break
            elif repeat_cnt < threshold:
                nonrepeat_cnt = self.get_nonrepeat_count(
                    memview[index:], blksize, threshold)
                ctrl_byte = uint8_t(nonrepeat_cnt | 0x80)
                compressed_data.append(ctrl_byte)
                compressed_data.append(memview[index:index +
                                                     nonrepeat_cnt * blksize])
                index += nonrepeat_cnt * blksize
            else:
                ctrl_byte = uint8_t(repeat_cnt)
                compressed_data.append(ctrl_byte)
                compressed_data.append(memview[index:index + blksize])
                index += repeat_cnt * blksize

        return b"".join(compressed_data)

    def get_repeat_count(self, data: bytearray, blksize: int):
        if len(data) < blksize:
            return 0

        start = data[:blksize]
        index = 0
        repeat_cnt = 0
        value = 0

        while index < len(data):
            value = data[index:index + blksize]

            if value == start:
                repeat_cnt += 1
                if repeat_cnt == 127:  # limit max repeat count to max value of signed char.
                    break
            else:
                break
            index += blksize

        return repeat_cnt

    def get_nonrepeat_count(self, data: bytearray, blksize: int, threshold):
        if len(data) < blksize:
            return 0

        pre_value = data[:blksize]

        index = 0
        nonrepeat_count = 0

        repeat_cnt = 0
        while True:
            value = data[index:index + blksize]
            if value == pre_value:
                repeat_cnt += 1
                if repeat_cnt > threshold:
                    # repeat found.
                    break
            else:
                pre_value = value
                nonrepeat_count += 1 + repeat_cnt
                repeat_cnt = 0
                if nonrepeat_count >= 127:  # limit max repeat count to max value of signed char.
                    nonrepeat_count = 127
                    break

            index += blksize  # move to next position
            if index >= len(data):  # data end
                nonrepeat_count += repeat_cnt
                break

        return nonrepeat_count


class OutputFormat(Enum):
    C_ARRAY = "C"
    BIN_FILE = "BIN"
    RAW_DATA = "RAW"  # option of not writing any file
    PNG_FILE = "PNG"  # convert to lvgl image and then to png


class PNGConverter:

    def __init__(self,
                 files: List,
                 cf: ColorFormat,
                 ofmt: OutputFormat,
                 odir: str,
                 background: int = 0x00,
                 align: int = 1,
                 compress: CompressMethod = CompressMethod.NONE,
                 keep_folder=True) -> None:
        self.files = files
        self.cf = cf
        self.ofmt = ofmt
        self.output = odir
        self.pngquant = None
        self.keep_folder = keep_folder
        self.align = align
        self.compress = compress
        self.background = background

    def _replace_ext(self, input, ext):
        if self.keep_folder:
            name, _ = path.splitext(input)
        else:
            name, _ = path.splitext(path.basename(input))
        output = name + ext
        output = path.join(self.output, output)
        return output

    def convert(self):
        output = []
        for f in self.files:
            img = LVGLImage().from_png(f, self.cf, background=self.background)
            img.adjust_stride(align=self.align)
            output.append((f, img))
            if self.ofmt == OutputFormat.BIN_FILE:
                img.to_bin(self._replace_ext(f, ".bin"),
                           compress=self.compress)
            elif self.ofmt == OutputFormat.C_ARRAY:
                img.to_c_array(self._replace_ext(f, ".c"),
                               compress=self.compress)
            elif self.ofmt == OutputFormat.PNG_FILE:
                img.to_png(self._replace_ext(f, ".png"))

        return output


def main():
    parser = argparse.ArgumentParser(description='LVGL PNG to bin image tool.')
    parser.add_argument('--ofmt',
                        help="output filename format, C or BIN",
                        default="BIN",
                        choices=["C", "BIN", "PNG"])
    parser.add_argument(
        '--cf',
        help=("bin image color format, use AUTO for automatically "
              "choose from I1/2/4/8"),
        default="I8",
        choices=[
            "L8", "I1", "I2", "I4", "I8", "A1", "A2", "A4", "A8", "ARGB8888",
            "XRGB8888", "RGB565", "RGB565A8", "RGB888", "AUTO"
        ])

    parser.add_argument('--compress',
                        help=("Binary data compress method, default to NONE"),
                        default="NONE",
                        choices=["NONE", "RLE", "LZ4"])

    parser.add_argument('--align',
                        help="stride alignment in bytes for bin image",
                        default=1,
                        type=int,
                        metavar='byte',
                        nargs='?')
    parser.add_argument('--background',
                        help="Background color for formats without alpha",
                        default=0x00_00_00,
                        type=lambda x: int(x, 0),
                        metavar='color',
                        nargs='?')
    parser.add_argument('-o',
                        '--output',
                        default="./output",
                        help="Select the output folder, default to ./output")
    parser.add_argument('-v', '--verbose', action='store_true')
    parser.add_argument(
        'input', help="the filename or folder to be recursively converted")

    args = parser.parse_args()

    if path.isfile(args.input):
        files = [args.input]
    elif path.isdir(args.input):
        files = list(Path(args.input).rglob("*.[pP][nN][gG]"))
    else:
        raise BaseException(f"invalid input: {args.input}")

    if args.verbose:
        logging.basicConfig(level=logging.INFO)

    logging.info(f"options: {args.__dict__}, files:{[str(f) for f in files]}")

    if args.cf == "AUTO":
        cf = None
    else:
        cf = ColorFormat[args.cf]

    ofmt = OutputFormat(args.ofmt)
    compress = CompressMethod[args.compress]

    converter = PNGConverter(files,
                             cf,
                             ofmt,
                             args.output,
                             background=args.background,
                             align=args.align,
                             compress=compress,
                             keep_folder=False)
    output = converter.convert()
    for f, img in output:
        logging.info(f"len: {img.data_len} for {path.basename(f)} ")

    print(f"done {len(files)} files")


def test():
    logging.basicConfig(level=logging.INFO)
    f = "pngs/cogwheel.RGB565A8.png"
    img = LVGLImage().from_png(f, cf=ColorFormat.RGB888, background=0xFF_FF_00)
    img.adjust_stride(align=16)
    img.to_bin("output/cogwheel.RGB888.bin")
    img.to_c_array("output/cogwheel-abc.c")  # file name is used as c var name
    img.to_png("output/cogwheel.RGB888.png.png")  # convert back to png


if __name__ == "__main__":
    # test()
    main()
