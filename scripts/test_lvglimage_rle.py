#!/usr/bin/env python3
"""
Regression test for the RLE encoder in LVGLImage.py.

RLEImage.get_nonrepeat_count() clamped the literal-run counter on the
"threshold reached" exit path but not on the "end of data" exit path, so an
input with more than 127 - repeat_cnt trailing distinct blocks made it return
a value above 127. The caller emits the control byte as
`uint8_t(nonrepeat_cnt | 0x80)`, truncating e.g. 128 to 0x80: a literal run
of 0 blocks followed by 128 blocks of payload. LVGL's own
lv_rle_decompress() rejects such a stream.

Each case compresses data whose final literal-run length is only known at the
end of the data and checks that
  - every control byte describes 1..127 blocks (no 0x80), and
  - the stream decodes back to exactly the input, walking the control bytes
    with the same semantics as lv_rle_decompress().

Usage: python scripts/test_lvglimage_rle.py
"""

import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from LVGLImage import RLEImage, ColorFormat  # noqa: E402

BLK = 4  # ARGB8888 -> 4 bytes per block


def pixel(i):
    """A block whose value only depends on i, so consecutive blocks differ."""
    return bytes([i, (i * 3) & 0xFF, (i * 5) & 0xFF, 0xFF])


def walk(stream, blk, out_len):
    """Walk the stream like lv_rle_decompress() does, return error or None."""
    rd = wr = 0
    n = len(stream)
    while rd < n:
        ctrl = stream[rd]
        rd += 1
        if ctrl == 0x80:
            return "control byte 0x80 = literal run of 0 blocks at offset %d" % (rd - 1)
        if ctrl & 0x80:  # literal run of (ctrl & 0x7F) blocks
            cnt = ctrl & 0x7F
            if rd + cnt * blk > n:
                return "input overrun: ctrl 0x%02X at %d claims %d byte(s), %d left" \
                    % (ctrl, rd - 1, cnt * blk, n - rd)
            rd += cnt * blk
            wr += cnt * blk
        else:  # repeat run: ctrl copies of one block
            if rd + blk > n:
                return "input overrun: ctrl 0x%02X at %d needs one block, %d left" \
                    % (ctrl, rd - 1, blk)
            rd += blk
            wr += ctrl * blk
        if wr > out_len:
            return "output overrun at offset %d" % rd
    if wr != out_len:
        return "decoded size %d does not match input size %d" % (wr, out_len)
    return None


def rle_decode(stream, blk, out_len):
    """Reference decoder with lv_rle_decompress() semantics."""
    out = bytearray()
    rd = 0
    n = len(stream)
    while rd < n:
        ctrl = stream[rd]
        rd += 1
        if ctrl & 0x80:
            cnt = ctrl & 0x7F
            out.extend(stream[rd:rd + cnt * blk])
            rd += cnt * blk
        else:
            out.extend(stream[rd:rd + blk] * ctrl)
            rd += blk
    assert len(out) == out_len, "decoder produced %d bytes, expected %d" % (len(out), out_len)
    return bytes(out)


def check_case(name, data):
    img = RLEImage(cf=ColorFormat.ARGB8888, w=len(data) // BLK, h=1, data=bytes(data))
    stream = img.rle_compress(data, BLK)

    err = walk(stream, BLK, len(data))
    assert err is None, "%s: invalid stream: %s" % (name, err)

    assert 0x80 not in _control_bytes(stream, BLK), \
        "%s: control byte 0x80 (literal run of 0 blocks) emitted" % name

    assert rle_decode(stream, BLK, len(data)) == bytes(data), \
        "%s: stream does not round-trip to the original data" % name

    print("%-28s OK  (%d blocks in, %d bytes out, first ctrl byte 0x%02X)"
          % (name, len(data) // BLK, len(stream), stream[0]))


def _control_bytes(stream, blk):
    """Yield the control bytes of a stream, skipping literal payloads."""
    rd = 0
    n = len(stream)
    ctrls = []
    while rd < n:
        ctrl = stream[rd]
        rd += 1
        ctrls.append(ctrl)
        if ctrl & 0x80:
            rd += (ctrl & 0x7F) * blk
        else:
            rd += blk
    return ctrls


def main():
    # 125 pairwise different blocks + 3 identical ones at the very end:
    # the minimal reproducer from the issue report.
    data = bytearray()
    for i in range(125):
        data += pixel(i)
    data += bytes([0x11, 0x22, 0x33, 0xFF]) * 3
    assert len(data) == 128 * BLK
    check_case("125 distinct + repeat tail", data)

    # 120 distinct blocks + a short repeat run at the end: another end-of-data
    # path overshoot (120 literals + 9 repeats = 129 > 127).
    data = bytearray()
    for i in range(120):
        data += pixel(i)
    data += bytes([0xAA, 0xBB, 0xCC, 0xDD]) * 10
    check_case("120 distinct + repeat tail", data)

    # More than 127 purely distinct blocks: split by the threshold path, must
    # stay valid and round-trip as well.
    data = bytearray()
    for i in range(200):
        data += pixel(i)
    check_case("200 distinct blocks", data)

    print("all RLE encoder regression checks passed")


if __name__ == "__main__":
    main()
