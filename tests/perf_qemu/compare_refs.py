#!/usr/bin/env python3
"""
Diff the framebuffers main_render.c dumped against the committed reference images.

The comparison is done here rather than on the target because the reference images are
800x480 PNGs and decoding one inside QEMU costs more than the render being measured, and
because LVGL's own screenshot compare stops at the first differing pixel of the first
differing row, which says "different" and nothing about how different.

The RGB565 expansion below is the same arithmetic as buf_to_xrgb8888() in
src/debugging/test/lv_test_screenshot_compare.c. It has to stay that way: the reference
images were written through it.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

import numpy as np
from PIL import Image

DUMP_MAGIC = 0x3144564C  # "LVD1"
HEADER = struct.Struct("<5I")  # magic, width, height, colour format, stride


def _expand565(px: np.ndarray) -> np.ndarray:
    """RGB565 to R, G, B, with the rounding lv_test_screenshot_compare.c uses."""
    out = np.empty(px.shape + (3,), dtype=np.uint8)
    out[..., 0] = ((px >> 11 & 0x1F).astype(np.uint32) * 2106) >> 8
    out[..., 1] = ((px >> 5 & 0x3F).astype(np.uint32) * 1037) >> 8
    out[..., 2] = ((px & 0x1F).astype(np.uint32) * 2106) >> 8
    return out


def expand(cf: str, rows: np.ndarray, w: int) -> np.ndarray:
    """One row-major byte array to an (h, w, 3) uint8 array of R, G, B.

    Every branch is buf_to_xrgb8888() in src/debugging/test/lv_test_screenshot_compare.c,
    which is what wrote the reference images. Alpha is dropped, because that function's
    caller only compares the three colour channels.
    """
    h = rows.shape[0]

    if cf in ("rgb565", "rgb565_swapped"):
        px = rows[:, :w * 2].reshape(h, w, 2)
        # RGB565_SWAPPED is byte swapped in place before the same expansion
        lo, hi = (1, 0) if cf == "rgb565_swapped" else (0, 1)
        return _expand565(px[:, :, lo].astype(np.uint16) |
                          (px[:, :, hi].astype(np.uint16) << 8))

    if cf in ("argb8888", "xrgb8888", "argb8888_premultiplied"):
        # lv_color32_t is blue, green, red, alpha
        return rows[:, :w * 4].reshape(h, w, 4)[:, :, [2, 1, 0]].copy()

    if cf == "rgb888":
        return rows[:, :w * 3].reshape(h, w, 3)[:, :, ::-1].copy()

    if cf == "l8":
        return np.repeat(rows[:, :w].reshape(h, w, 1), 3, axis=2)

    if cf == "al88":
        # Two bytes per pixel, luminance then alpha; the colour is the luminance
        return np.repeat(rows[:, :w * 2].reshape(h, w, 2)[:, :, :1], 3, axis=2)

    if cf == "i1":
        # One bit per pixel, most significant bit first, black or white
        bits = np.unpackbits(rows, axis=1)[:, :w]
        return np.repeat((bits * 255).reshape(h, w, 1), 3, axis=2)

    raise ValueError(f"no expansion for colour format {cf}")


# Bits per pixel per colour format, used to check that a dump really is in the format it
# is being read as. Checked against the stride rather than against the dump header's
# LV_COLOR_FORMAT_* number, because that number would have to be mirrored here and would
# then quietly drift out of step with lv_color.h.
CF_BPP = {
    "rgb565": 16, "rgb565_swapped": 16, "rgb888": 24, "xrgb8888": 32,
    "argb8888": 32, "argb8888_premultiplied": 32, "l8": 8, "al88": 16, "i1": 1,
}


def read_dump(path: Path, cf: str) -> np.ndarray:
    """Return the dumped framebuffer as an (h, w, 3) uint8 array of R, G, B."""
    raw = path.read_bytes()
    magic, w, h, _cf, stride = HEADER.unpack_from(raw)
    if magic != DUMP_MAGIC:
        raise ValueError(f"{path}: not a framebuffer dump")
    bpp = CF_BPP.get(cf)
    if bpp is None:
        raise ValueError(f"{path}: no expansion for colour format {cf}")
    if stride * 8 < w * bpp:
        raise ValueError(f"{path}: a stride of {stride} bytes cannot hold {w} pixels of "
                         f"{cf} ({bpp} bpp). A leftover dump from another format?")

    body = raw[HEADER.size:]
    expected = stride * h
    if len(body) != expected:
        raise ValueError(f"{path}: {len(body)} bytes of pixels, expected {expected}")

    rows = np.frombuffer(body, dtype=np.uint8).reshape(h, stride)
    return expand(cf, rows, w)


def read_ref(path: Path) -> np.ndarray:
    """Return a reference PNG as an (h, w, 3) uint8 array of R, G, B.

    Decoded as RGBA and then narrowed, which is what lodepng_decode32_file() does in
    lv_test_screenshot_compare.c. Going straight to RGB composites a palette image's
    transparency against black instead, which is a different picture.
    """
    with Image.open(path) as img:
        return np.asarray(img.convert("RGBA"))[:, :, :3]


def compare(dump: Path, ref: Path, cf: str, tolerance: int,
            err_dir: Path | None) -> dict:
    got = read_dump(dump, cf)
    result = {"digest": hashlib.sha256(dump.read_bytes()).hexdigest()[:16]}

    if ref is None or not ref.exists():
        result["ref"] = "missing"
        return result

    want = read_ref(ref)
    if want.shape != got.shape:
        result["ref"] = f"size {want.shape[1]}x{want.shape[0]} != {got.shape[1]}x{got.shape[0]}"
        return result

    delta = np.abs(got.astype(np.int16) - want.astype(np.int16))
    per_px = delta.max(axis=2)
    result["ref"] = "ok"
    result["max_delta"] = int(per_px.max())
    result["bad_px"] = int((per_px > tolerance).sum())

    if result["bad_px"] and err_dir is not None:
        err_dir.mkdir(parents=True, exist_ok=True)
        out = err_dir / (dump.stem + "_err.png")
        Image.fromarray(got).save(out)
        result["err_png"] = str(out)

    return result


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--dumps", required=True, type=Path,
                    help="directory of <scene>_opa_<opa>.bin files for one colour format, "
                         "named after that format")
    ap.add_argument("--cf", help="the colour format; defaults to the --dumps directory name")
    ap.add_argument("--refs", type=Path,
                    help="matching tests/ref_imgs/draw/render/<cf> directory; "
                         "without it only digests are reported")
    ap.add_argument("--output", required=True, type=Path)
    ap.add_argument("--err-dir", type=Path,
                    help="write the rendered image here for every scene that differs")
    ap.add_argument("--tolerance", type=int, default=0,
                    help="per-channel difference that still counts as equal (default 0)")
    args = ap.parse_args()

    # Outside this range the comparison stops meaning anything: below zero every pixel
    # differs, above 255 none can.
    if not 0 <= args.tolerance <= 255:
        print(f"--tolerance must be between 0 and 255, not {args.tolerance}",
              file=sys.stderr)
        return 1

    dumps = sorted(args.dumps.glob("*.bin"))
    if not dumps:
        print(f"no dumps in {args.dumps}", file=sys.stderr)
        return 1

    cf = args.cf or args.dumps.name
    scenes = {}
    for dump in dumps:
        ref = args.refs / f"demo_render_{dump.stem}.png" if args.refs else None
        scenes[dump.stem] = compare(dump, ref, cf, args.tolerance, args.err_dir)

    differing = sorted(k for k, v in scenes.items() if v.get("bad_px"))
    no_ref = sorted(k for k, v in scenes.items() if v.get("ref") not in (None, "ok"))

    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(
        {"scenes": scenes, "differing": differing, "no_ref": no_ref}, indent=1) + "\n")

    print(f"{len(scenes)} scene{'' if len(scenes) == 1 else 's'}, "
          f"{len(differing)} differ from the reference, "
          f"{len(no_ref)} without a usable reference")
    for name in differing:
        s = scenes[name]
        print(f"  {name}: max delta {s['max_delta']}, {s['bad_px']} pixels")
    for name in no_ref:
        print(f"  {name}: reference {scenes[name]['ref']}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
