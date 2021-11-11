#!/usr/bin/env python3
import logging
import argparse
from pathlib import Path
import os
from os.path import isfile, isdir, join
from enum import Enum


class LVGLColorFmt(Enum):
    LV_COLOR_FORMAT_UNKNOWN = 0
    LV_COLOR_FORMAT_L8 = 1
    LV_COLOR_FORMAT_A8 = 2
    LV_COLOR_FORMAT_I1 = 3
    LV_COLOR_FORMAT_I2 = 4
    LV_COLOR_FORMAT_I4 = 5
    LV_COLOR_FORMAT_I8 = 6
    LV_COLOR_FORMAT_RGB565 = 7
    LV_COLOR_FORMAT_RGB565A8 = 8
    LV_COLOR_FORMAT_RGB888 = 9
    LV_COLOR_FORMAT_ARGB8888 = 10
    LV_COLOR_FORMAT_XRGB8888 = 11
    LV_COLOR_FORMAT_NATIVE = 12

def uint8_t(val) -> bytes:
    return val.to_bytes(1, byteorder='little')


def uint32_t(val) -> bytes:
    return val.to_bytes(4, byteorder='little')


def lvgl_get_px_size_from_cf(cf: LVGLColorFmt, bpp=32):
    if cf == LVGLColorFmt.LV_IMG_CF_TRUE_COLOR or cf == LVGLColorFmt.LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED:
        return bpp

    elif cf == LVGLColorFmt.LV_IMG_CF_TRUE_COLOR_ALPHA:
        if bpp == 1 or bpp == 8:
            return 2 * 8
        elif bpp == 16:
            return 3 * 8
        elif bpp == 32:
            return 4 * 8
        else:
            return 0
    elif cf == LVGLColorFmt.LV_IMG_CF_INDEXED_1BIT or cf == LVGLColorFmt.LV_IMG_CF_ALPHA_1BIT:
        return 1
    elif cf == LVGLColorFmt.LV_IMG_CF_INDEXED_2BIT or cf == LVGLColorFmt.LV_IMG_CF_ALPHA_2BIT:
        return 2
    elif cf == LVGLColorFmt.LV_IMG_CF_INDEXED_4BIT or cf == LVGLColorFmt.LV_IMG_CF_ALPHA_4BIT:
        return 4
    elif cf == LVGLColorFmt.LV_IMG_CF_INDEXED_8BIT or cf == LVGLColorFmt.LV_IMG_CF_ALPHA_8BIT:
        return 8
    else:
        return 0


class RLEHeader:
    def __init__(self, cf: LVGLColorFmt, w: int, h: int, blksize: int, len: int):
        self.cf = cf
        self.w = w
        self.h = h
        self.blksize = blksize
        self.len = len

    @property
    def binary(self):
        img_header = self.cf
        img_header |= (self.w << 10)
        img_header |= (self.h << 21)

        rle_header = self.blksize
        rle_header |= (self.len & 0xffffff) << 4
        magic = 0x5aa521e0

        binary = bytearray()
        binary.extend(uint32_t(img_header))
        binary.extend(uint32_t(magic))
        binary.extend(uint32_t(rle_header))
        return binary


def get_repeat_data_count(data: bytearray, byte_per_data: int):
    if len(data) < byte_per_data:
        return 0

    start = data[:byte_per_data]
    index = 0
    repeat_cnt = 0
    value = 0

    while index < len(data):
        value = data[index: index+byte_per_data]
        if value != start:
            break

        repeat_cnt += 1
        if repeat_cnt == 127:  # limit max repeat count to max value of signed char.
            break
        index += byte_per_data

    return repeat_cnt


def get_nonrepeat_data_count(data: bytearray, byte_per_data: int, repeat_threshold):
    if len(data) < byte_per_data:
        return 0

    pre_value = data[:byte_per_data]

    index = 0
    nonrepeat_count = 0

    repeat_cnt = 0
    while True:
        value = data[index: index + byte_per_data]
        if value == pre_value:
            repeat_cnt += 1
            if repeat_cnt > repeat_threshold:
                # repeat found.
                break
        else:
            pre_value = value
            nonrepeat_count += 1 + repeat_cnt
            repeat_cnt = 0
            if nonrepeat_count >= 127:  # limit max repeat count to max value of signed char.
                nonrepeat_count = 127
                break

        index += byte_per_data  # move to next position
        if index >= len(data):  # data end
            nonrepeat_count += repeat_cnt
            break

    return nonrepeat_count


def lvgl_image_rle_compress(img_data: bytes, bpp, repeat_threshold):
    header_u32 = int.from_bytes(img_data[:4], byteorder="little")
    cf = header_u32 & 0x1f
    w = (header_u32 >> 10) & 0x7ff
    h = (header_u32 >> 21) & 0x7ff
    img_data = bytearray(img_data[4:])
    px_size = lvgl_get_px_size_from_cf(LVGLColorFmt(cf), bpp)
    if px_size == 0:
        logging.error(f"unknown format for cf:{cf}, bpp:{bpp}")
        return None, None

    byte_per_data = (px_size + 7) // 8
    data_len = len(img_data)
    fill_bytes = data_len - data_len // byte_per_data * byte_per_data
    img_data.extend(b'\x00' * fill_bytes)
    data_len = len(img_data)

    rleheader = RLEHeader(cf, w, h, byte_per_data, data_len)

    index = 0
    compressed_data = bytearray()
    while len(img_data[index:]):
        repeat_cnt = get_repeat_data_count(img_data[index:], byte_per_data)
        if repeat_cnt == 0:
            break
        elif repeat_cnt < repeat_threshold:
            nonrepeat_cnt = get_nonrepeat_data_count(
                img_data[index:], byte_per_data, repeat_threshold)
            ctrl_byte = uint8_t(nonrepeat_cnt | 0x80)
            compressed_data.extend(ctrl_byte)
            compressed_data.extend(
                img_data[index: index + nonrepeat_cnt*byte_per_data])
            index += nonrepeat_cnt * byte_per_data
        else:
            ctrl_byte = uint8_t(repeat_cnt)
            compressed_data.extend(ctrl_byte)
            compressed_data.extend(img_data[index: index + byte_per_data])
            index += repeat_cnt * byte_per_data

        if index >= len(img_data):
            break

    binary = bytearray()
    header = rleheader.binary
    binary.extend(header)
    binary.extend(compressed_data)
    return binary, rleheader


def lvgl_rle_compress_to_binfile(binfile, bpp=32, repeat_threshold=10):
    rlefile, ext = os.path.splitext(binfile)
    rlefile += ".rle"
    with open(binfile, "rb") as fin, open(rlefile, "wb+") as fout:
        image_data = fin.read()
        compressed, _ = lvgl_image_rle_compress(
            image_data, bpp, repeat_threshold)
        fout.write(compressed)

        data_len = len(image_data)
        compressed_len = len(compressed)
        saved = 100 - compressed_len / data_len * 100
        logging.info(
            f"{binfile} {data_len} --> {compressed_len}, saved: {saved: .2f}%")


def lvgl_rle_compress_to_c_file(binfile, bpp=32, repeat_threshold=10):
    head, filename = os.path.split(binfile)
    image_name = os.path.splitext(filename)[0]
    output_file = join(head, image_name + ".c")
    with open(binfile, "rb") as fin, open(output_file, "w+") as fout:
        image_data = fin.read()
        compressed, rleheader = lvgl_image_rle_compress(
            image_data, bpp, repeat_threshold)
        compressed = compressed[4:]  # remove original image header
        head = f'''
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_{image_name.upper()}
#define LV_ATTRIBUTE_IMG_{image_name.upper()}
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_{image_name.upper()} uint8_t {image_name}_map[] = {'{'}
    '''

        end = f'''

const lv_img_dsc_t {image_name} = {'{'}
    .header.always_zero = 0,
    .header.w = {rleheader.w},
    .header.h = {rleheader.h},
    .data_size = {len(compressed)},
    .header.cf = {LVGLColorFmt(rleheader.cf).name},
    .data = {image_name}_map,
{'}'};
'''
        file_string = []
        file_string += head

        data_count = 0
        for b in compressed:
            file_string += f"0x{b:02x}, "
            data_count += 1
            if data_count % 16 == 0:
                file_string += "\n    "

        file_string += "\n};"

        file_string = ''.join(file_string)
        file_string += end
        fout.write(file_string)

        data_len = len(image_data)
        compressed_len = len(compressed)
        saved = 100 - compressed_len / data_len * 100
        logging.info(
            f"{binfile} {data_len} --> {compressed_len}, saved: {saved: .2f}%")


def parser():
    parser = argparse.ArgumentParser(description='LVGL bin file RLE encoder.')
    parser.add_argument(
        '--bpp', help="specify color depth, defaults to 32bpp", default=32)
    parser.add_argument('-t', '--threshold',
                        help="minimal repeat count", default=8)
    parser.add_argument('-o', '--ofmt', help="output file format, C or BIN",
                        default="BIN", choices=["C", "BIN"])
    parser.add_argument('input', help="the folder or file to be converted")
    args = parser.parse_args()
    return args


def main():
    logging.basicConfig(level=logging.DEBUG)
    args = parser()
    bpp = int(args.bpp)
    threshold = int(args.threshold)
    if isfile(args.input):
        files = [args.input]
    elif isdir(args.input):
        files = list(Path(args.input).rglob("*.[bB][iI][nN]"))
    else:
        raise BaseException(f"invalid input: {args.input}")

    for f in files:
        if args.ofmt == "C":
            lvgl_rle_compress_to_c_file(f, bpp, repeat_threshold=threshold)
        else:
            lvgl_rle_compress_to_binfile(
                f, bpp, repeat_threshold=threshold)


if __name__ == "__main__":
    main()
