LVGL uses the following third-party libraries.
Each entry lists its SPDX license expression below. The full license text is, for most libraries, found in the corresponding library folder (see the paths); some libraries only reference their license rather than shipping the full text. A machine-readable inventory is available in `sbom/`.

**Barcode (Barcode generator)**
- Path: src/libs/barcode
- Source: https://github.com/fhunleth/code128
- License: BSD-2-Clause

**FreeType (Font rendering library)**
- Path: src/libs/freetype
- Source: https://github.com/freetype/freetype
- License: FTL OR GPL-2.0-or-later
- Note: Only the interfaces are used; FreeType itself is not part of LVGL.

**Liberation Sans (default demo font)**
- Path: src/libs/freetype/LiberationSans-Regular.ttf
- Source: https://github.com/liberationfonts/liberation-fonts
- License: OFL-1.1
- Note: Font asset bundled alongside the FreeType interface; licensed separately from FreeType.

**LodePNG (PNG decoder)**
- Path: src/libs/lodepng
- Source: https://github.com/lvandeve/lodepng
- License: Zlib

**LZ4 (Compression/Decompression)**
- Path: src/libs/lz4
- Source: https://github.com/lz4/lz4
- License: BSD-2-Clause

**QR Code (QR code generator)**
- Path: src/libs/qrcode
- Source: https://github.com/nayuki/QR-Code-generator
- License: MIT

**ThorVG (Vector graphics rendering)**
- Path: src/libs/thorvg
- Source: https://github.com/thorvg/thorvg
- License: MIT

**RapidJSON (JSON parser/generator)**
- Path: src/libs/thorvg/rapidjson
- Source: https://github.com/Tencent/rapidjson
- License: MIT AND BSD-3-Clause
- Note: Bundled inside the ThorVG tree (used by ThorVG's Lottie parser); licensed separately from ThorVG. The bundled msinttypes helper (rapidjson/msinttypes) is BSD-3-Clause (Copyright (c) 2006-2013 Alexander Chemeris), hence the combined license expression.

**TinyTTF**
- Path: src/libs/tiny_ttf
- Source:
    - https://github.com/nothings/stb (*Only parts are integrated*)
    - https://github.com/codewitch-honey-crisis/tiny_ttf (*Modified version of the original STB library*)
- License: MIT OR Unlicense

**TJPGD (JPEG decoder)**
- Path: src/libs/tjpgd
- Source: http://elm-chan.org/fsw/tjpgd/00index.html
- License: LicenseRef-TJpgDec

**TLSF (Two-Level Segregate Fit memory allocator)**
- Path: src/stdlib/builtin
- Source: https://github.com/mattconte/tlsf
- License: BSD-3-Clause

**Printf (Printf formatting library)**
- Path: src/stdlib/builtin
- Source: https://github.com/mpaland/printf
- License: MIT

**FT800-FT813 (EVE GPU driver)**
- Path: src/libs/FT800-FT813
- Source: https://github.com/RudolphRiedel/FT800-FT813
- License: MIT

**AnimatedGIF (GIF decoder library)**
- Path: src/libs/gif
- Source: https://github.com/bitbank2/AnimatedGIF
- License: Apache-2.0

**NanoVG (Anti-aliased vector graphics rendering)**
- Path: src/libs/nanovg
- Source: https://github.com/memononen/nanovg
- License: Zlib
- Note: Compiled when LV_USE_NANOVG (or the LV_USE_DRAW_NANOVG backend) is enabled.

**FrogFS (read-only filesystem)**
- Path: src/libs/frogfs
- Source: https://github.com/jkent/frogfs
- License: MPL-2.0
