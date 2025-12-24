# VG-Lite Decoder Color Format Conversion

This document describes the color format conversion rules in `lv_vg_lite_decoder.c`.

## Color Format Mapping Table

| Source Format | Target Format | Description |
|---------------|---------------|-------------|
| `LV_COLOR_FORMAT_I1` | `LV_COLOR_FORMAT_I8` | VG_LITE index formats require endian + bit flipping, converted to I8 for simplicity |
| `LV_COLOR_FORMAT_I2` | `LV_COLOR_FORMAT_I8` | Same as above |
| `LV_COLOR_FORMAT_I4` | `LV_COLOR_FORMAT_I8` | Same as above |
| `LV_COLOR_FORMAT_A1` | `LV_COLOR_FORMAT_A8` | Alpha format expanded to 8-bit |
| `LV_COLOR_FORMAT_A2` | `LV_COLOR_FORMAT_A8` | Alpha format expanded to 8-bit |
| `LV_COLOR_FORMAT_RGB888` | `LV_COLOR_FORMAT_XRGB8888` | Converted when GPU doesn't support 24-bit (returns UNKNOWN if supported, handled by bin_decoder) |
| `LV_COLOR_FORMAT_ARGB8565` | `LV_COLOR_FORMAT_ARGB8888` | Converted when GPU doesn't support 24-bit (returns UNKNOWN if supported, handled by bin_decoder) |
| `LV_COLOR_FORMAT_RGB565A8` | `LV_COLOR_FORMAT_ARGB8888` | Separate RGB + Alpha planes merged into ARGB8888 |
| `LV_COLOR_FORMAT_AL88` | `LV_COLOR_FORMAT_ARGB8888` | Alpha + Luminance converted to ARGB8888 |
| `LV_COLOR_FORMAT_RGB565_SWAPPED` | `LV_COLOR_FORMAT_RGB565` | Byte order swapped |
| Other formats | `LV_COLOR_FORMAT_UNKNOWN` | Not supported, passed to other decoders |

## Notes

- Formats returning `LV_COLOR_FORMAT_UNKNOWN` will be rejected by this decoder and handled by other decoders in the chain.
- The 24-bit format conversion (`RGB888`, `ARGB8565`) depends on GPU capability queried via `vg_lite_query_feature(gcFEATURE_BIT_VG_24BIT)`.
- Index formats (`I1`, `I2`, `I4`) maintain their palette but expand index values to 8-bit for GPU compatibility.
- Alpha formats (`A1`, `A2`) are linearly scaled to 8-bit (e.g., A1: 0→0, 1→255; A2: 0→0, 1→85, 2→170, 3→255).
