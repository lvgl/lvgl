#!/usr/bin/env python3
"""
LVGL header consistency checker.

Subcommands (each exits 0 on success, 1 on failure):
  public-api          Check every header in include/lvgl/ is included by include/lvgl/lvgl.h
  private-api         Check every header in src/ is included by src/lvgl_private.h
  include-paths       Check every #include inside src/ references a valid relative path
                      (angle-bracket includes must appear in ALLOWED_EXTERNAL_HEADERS)
  allowed-list-audit  Report usage of every entry in ALLOWED_EXTERNAL_HEADERS across
                      src/ and include/lvgl/; fail if any entry is unused anywhere
  deprecated          Check no source file includes a deprecated header
  no-direct-public-include
                      Check src/ never includes include/lvgl/ headers directly
"""

from __future__ import annotations

import argparse
import fnmatch
import os
import re
import subprocess
import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# Colors
# ---------------------------------------------------------------------------


def _colors_enabled() -> bool:
    if os.environ.get("NO_COLOR"):
        return False
    return sys.stdout.isatty() or os.environ.get("FORCE_COLOR") == "1"


class C:
    _on = _colors_enabled()
    RED = "\033[31m" if _on else ""
    YELLOW = "\033[33m" if _on else ""
    GREEN = "\033[32m" if _on else ""
    BOLD = "\033[1m" if _on else ""
    RESET = "\033[0m" if _on else ""


def log_ok(msg: str) -> None:
    print(f"{C.GREEN}{C.BOLD}OK:{C.RESET} {msg}")


def log_fatal(msg: str) -> None:
    print(f"{C.RED}{C.BOLD}ERROR:{C.RESET} {C.RED}{msg}{C.RESET}")


def _error(location: str, msg: str) -> None:
    print(f"{C.RED}{C.BOLD}{location}:{C.RESET} {msg}")


def _warn(location: str, msg: str) -> None:
    print(f"{C.YELLOW}{C.BOLD}{location}:{C.RESET} {msg}")


# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

SOURCE_EXTENSIONS = {".c", ".cpp", ".h", ".hpp"}

DEPRECATION_MARKERS = (
    "#warning Include public headers from the `src` folder is deprecated",
    "is no longer part of the public API",
)

# Angle-bracket includes that are explicitly forbidden.
# Files should instead use the LVGL indirection macros shown below.
#
# Instead of:                    Use:
#   #include <stdint.h>          #include LV_STDINT_INCLUDE
#   #include <stddef.h>          #include LV_STDDEF_INCLUDE
#   #include <stdbool.h>         #include LV_STDBOOL_INCLUDE
#   #include <inttypes.h>        #include LV_INTTYPES_INCLUDE
#   #include <limits.h>          #include LV_LIMITS_INCLUDE
#   #include <stdarg.h>          #include LV_STDARG_INCLUDE
#
# Add any other headers that should NEVER appear as raw angle-bracket
# includes to FORBIDDEN_SYSTEM_HEADERS.  Everything not in this set that
# appears as an angle-bracket include is reported as an error (the
# developer must either add it to ALLOWED_EXTERNAL_HEADERS or switch to a
# quoted / macro include).
#
# To allow an angle-bracket header unconditionally, add it to
# ALLOWED_EXTERNAL_HEADERS instead.

FORBIDDEN_SYSTEM_HEADERS: set[str] = {
    "stdint.h",
    "stddef.h",
    "stdbool.h",
    "inttypes.h",
    "limits.h",
    "stdarg.h",
}

# Headers that are explicitly permitted inside <>.
# This is the exhaustive allow-list; anything not here (and not forbidden)
# is still an error so the list stays intentional and auditable.
ALLOWED_EXTERNAL_HEADERS: set[str] = {
    "FreeRTOS.h",
    "GL/glew.h",
    "GLES2/gl2.h",
    "GLES3/gl3.h",
    "GLFW/glfw3.h",
    "LVGL_thread.h",
    "LittleFS.h",
    "Pre_Include_Global.h",
    "SD.h",
    "SDL2/SDL.h",
    "SDL2/SDL_syswm.h",
    "SPI.h",
    "TFT_eSPI.h",
    "X11/Xlib.h",
    "X11/Xutil.h",
    "__arm_2d_impl.h",
    "algorithm",
    "arm_2d.h",
    "arm_neon.h",
    "assert.h",
    "atomic.h",
    "bsp_api.h",
    "cmsis_os2.h",
    "cstdint",
    "ctype.h",
    "dave_driver.h",
    "debug.h",
    "dev/evdev/input.h",
    "direct.h",
    "dirent.h",
    "dlfcn.h",
    "driver/ppa.h",
    "drm/drm_fourcc.h",
    "drm_fourcc.h",
    "errno.h",
    "esp_cache.h",
    "esp_err.h",
    "esp_heap_caps.h",
    "esp_log.h",
    "fastgltf/core.hpp",
    "fastgltf/math.hpp",
    "fastgltf/tools.hpp",
    "fastgltf/types.hpp",
    "fastgltf/util.hpp",
    "fcntl.h",
    "ff.h",
    "float.h",
    "freertos/FreeRTOS.h",
    "freertos/atomic.h",
    "freertos/semphr.h",
    "freertos/task.h",
    "freetype/config/ftheader.h",
    "freetype/fterrors.h",
    "freetype/ftsystem.h",
    "freetype/fttypes.h",
    "freetype/internal/ftdebug.h",
    "freetype/internal/ftstream.h",
    "fsl_cache.h",
    "fsl_elcdif.h",
    "fsl_pxp.h",
    "fsl_video_common.h",
    "ft2build.h",
    "functional",
    "g2d.h",
    "g2dExt.h",
    "gbm.h",
    "glib.h",
    "gst/gst.h",
    "gst/gstelementfactory.h",
    "gst/video/video.h",
    "hal/color_hal.h",
    "hal_data.h",
    "include/lv_mp_mem_custom_include.h",
    "intrin.h",
    "jpegint.h",
    "jpeglib.h",
    "lfs.h",
    "libavcodec/avcodec.h",
    "libavformat/avformat.h",
    "libavutil/imgutils.h",
    "libavutil/samplefmt.h",
    "libavutil/timestamp.h",
    "libinput.h",
    "libswscale/swscale.h",
    "libyuv/convert_argb.h",
    "linux/dma-buf.h",
    "linux/fb.h",
    "linux/input-event-codes.h",
    "linux/input.h",
    "linux/limits.h",
    "lv_conf_cmsis.h",
    "lvgl_support.h",
    "lz4.h",
    "main.h",
    "malloc.h",
    "map",
    "math.h",
    "mqx.h",
    "mutex.h",
    "nuttx/arch.h",
    "nuttx/cache.h",
    "nuttx/clock.h",
    "nuttx/input/mouse.h",
    "nuttx/input/touchscreen.h",
    "nuttx/lcd/lcd_dev.h",
    "nuttx/mm/mm.h",
    "nuttx/tls.h",
    "nuttx/video/fb.h",
    "platform.h",
    "png.h",
    "poll.h",
    "ppc_ghs.h",
    "ppcintrinsics.h",
    "process.h",
    "pthread.h",
    "r_glcdc_rx_if.h",
    "r_glcdc_rx_pinset.h",
    "riscv_vector.h",
    "rlottie_capi.h",
    "rtthread.h",
    "screen/screen.h",
    "sdkconfig.h",
    "semaphore.h",
    "semphr.h",
    "setjmp.h",
    "stdio.h",
    "stdlib.h",
    "string",
    "string.h",
    "sys/consio.h",
    "sys/fbio.h",
    "sys/fcntl.h",
    "sys/inotify.h",
    "sys/ioctl.h",
    "sys/keycodes.h",
    "sys/mman.h",
    "sys/param.h",
    "sys/poll.h",
    "sys/stat.h",
    "sys/syscall.h",
    "sys/types.h",
    "syslog.h",
    "task.h",
    "thorvg_capi.h",
    "thread",
    "time.h",
    "unistd.h",
    "uv.h",
    "vector",
    "vg_lite.h",
    "wayland-client-protocol.h",
    "wayland-client.h",
    "wayland-cursor.h",
    "wayland-egl.h",
    "wayland_linux_dmabuf.h",
    "wayland_xdg_shell.h",
    "webp/decode.h",
    "windows.h",
    "windowsx.h",
    "xf86drm.h",
    "xf86drmMode.h",
    "xkbcommon/xkbcommon.h",
    "zephyr/irq.h",
    "zephyr/kernel.h",
}

HEADER_CHECK_WHITELIST: set[str] = {
    "debugging/vg_lite_tvg",
    "drivers/opengles/assets/lv_opengles_shader.c",
    "drivers/opengles/opengl_shader/lv_opengl_shader_manager.c",
    "drivers/opengles/glad",
    "font/lv_font_dejavu_16_persian_hebrew.c",
    "font/lv_font_montserrat_10.c",
    "font/lv_font_montserrat_12.c",
    "font/lv_font_montserrat_12_subpx.c",
    "font/lv_font_montserrat_14.c",
    "font/lv_font_montserrat_14_aligned.c",
    "font/lv_font_montserrat_16.c",
    "font/lv_font_montserrat_18.c",
    "font/lv_font_montserrat_20.c",
    "font/lv_font_montserrat_22.c",
    "font/lv_font_montserrat_24.c",
    "font/lv_font_montserrat_26.c",
    "font/lv_font_montserrat_28.c",
    "font/lv_font_montserrat_28_compressed.c",
    "font/lv_font_montserrat_30.c",
    "font/lv_font_montserrat_32.c",
    "font/lv_font_montserrat_34.c",
    "font/lv_font_montserrat_36.c",
    "font/lv_font_montserrat_38.c",
    "font/lv_font_montserrat_40.c",
    "font/lv_font_montserrat_42.c",
    "font/lv_font_montserrat_44.c",
    "font/lv_font_montserrat_46.c",
    "font/lv_font_montserrat_48.c",
    "font/lv_font_montserrat_8.c",
    "font/lv_font_source_han_sans_sc_14_cjk.c",
    "font/lv_font_source_han_sans_sc_16_cjk.c",
    "font/lv_font_unscii_16.c",
    "font/lv_font_unscii_8.c",
    "libs/FT800-FT813",
    "libs/thorvg",
    "libs/vg_lite_driver",
    "libs/barcode/code128.h",
    "libs/tiny_ttf/stb_truetype_htcw.h",
    "libs/tiny_ttf/stb_rect_pack.h",
    "libs/nanovg",
    "libs/lz4",
    "libs/lodepng/lodepng.h",
    "libs/lodepng/lodepng.c",
    "libs/gltf/stb_image/stb_image.h",
    "libs/gltf/gltf_view/assets/lv_gltf_view_shader.c",
}
# Regex helpers
INCLUDE_ANGLE_RE = re.compile(r"#\s*include\s*<([^>]+)>")
INCLUDE_QUOTED_RE = re.compile(r'#\s*include\s*"([^"]+)"')
INCLUDE_MACRO_RE = re.compile(r"#\s*include\s+LV_\w+_INCLUDE")


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def _source_files(directory: Path):
    """Yield all source/header files under *directory*."""
    for path in sorted(directory.rglob("*")):
        if path.suffix in SOURCE_EXTENSIONS:
            yield path


def _read_lines(path: Path) -> list[str]:
    try:
        return path.read_text(encoding="utf-8", errors="ignore").splitlines()
    except OSError:
        return []


def _collect_includes(umbrella: Path) -> set[str]:
    """
    Return the set of header filenames directly included by *umbrella*.
    Single-level only -- the umbrella is expected to include everything directly.
    """
    includes: set[str] = set()
    for line in _read_lines(umbrella):
        m = INCLUDE_QUOTED_RE.search(line) or INCLUDE_ANGLE_RE.search(line)
        if m:
            includes.add(Path(m.group(1)).name)
    return includes


def _grep(pattern: str, directory: Path) -> list[tuple[Path, int, str]]:
    """
    Fast search using the system grep binary with a pure-Python fallback.
    Returns [(path, lineno, line), ...] for every matching line.
    """
    try:
        result = subprocess.run(
            [
                "grep",
                "-rn",
                "--include=*.h",
                "--include=*.c",
                "--include=*.cpp",
                "--include=*.hpp",
                "-E",
                pattern,
                str(directory),
            ],
            capture_output=True,
            text=True,
        )
        hits: list[tuple[Path, int, str]] = []
        for raw in result.stdout.splitlines():
            # grep output format: path:lineno:line
            parts = raw.split(":", 2)
            if len(parts) == 3:
                try:
                    hits.append((Path(parts[0]), int(parts[1]), parts[2]))
                except ValueError:
                    pass
        return hits
    except FileNotFoundError:
        # grep not available -- fall back to pure Python
        compiled = re.compile(pattern)
        hits = []
        for path in _source_files(directory):
            for lineno, line in enumerate(_read_lines(path), start=1):
                if compiled.search(line):
                    hits.append((path, lineno, line))
        return hits


def _find_deprecated_headers(folder: Path) -> set[Path]:
    deprecated: set[Path] = set()
    for path in folder.rglob("*.h"):
        try:
            content = path.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        if any(marker in content for marker in DEPRECATION_MARKERS):
            # Store the path relative to the folder so it matches include directives
            deprecated.add(path.relative_to(folder))
    return deprecated


# ---------------------------------------------------------------------------
# Check 1 -- public API umbrella
# ---------------------------------------------------------------------------


def check_public_api(repo_root: Path) -> bool:
    """Every non-deprecated .h under include/lvgl/ must be included by include/lvgl/lvgl.h."""
    include_dir = repo_root / "include" / "lvgl"
    umbrella = include_dir / "lvgl.h"

    if not umbrella.exists():
        log_fatal(f"umbrella header not found: {umbrella}")
        return False

    covered = _collect_includes(umbrella)
    ok = True

    for header in sorted(include_dir.rglob("*.h")):
        if header == umbrella:
            continue
        if header.name not in covered:
            _error(
                str(header.relative_to(include_dir)),
                "not included by include/lvgl/lvgl.h",
            )
            ok = False

    if ok:
        log_ok("all public headers are included by include/lvgl/lvgl.h")
    return ok


# ---------------------------------------------------------------------------
# Check 2 -- private API umbrella
# ---------------------------------------------------------------------------


def check_private_api(repo_root: Path) -> bool:
    """Every non-deprecated .h under src/ must be included by src/lvgl_private.h."""
    src_dir = repo_root / "src"
    umbrella = src_dir / "lvgl_private.h"

    if not umbrella.exists():
        log_fatal(f"private umbrella not found: {umbrella}")
        return False

    exceptions = {umbrella.name, "lv_templ.h", "lv_objx_templ.h"}
    deprecated = [p.name for p in _find_deprecated_headers(repo_root)]
    covered = _collect_includes(umbrella)
    ok = True

    for header in sorted(src_dir.rglob("lv_*.h")):
        if header.name in deprecated or header.name in exceptions:
            continue
        if header.name not in covered:
            _error(
                str(header.relative_to(repo_root)), "not included by src/lvgl_private.h"
            )
            ok = False

    if ok:
        log_ok("all private headers are included by src/lvgl_private.h")
    return ok


# ---------------------------------------------------------------------------
# Check 3 -- include path validity + angle-bracket policy
# ---------------------------------------------------------------------------
def is_allowed_header(header: str) -> bool:
    return any(fnmatch.fnmatch(header, pattern) for pattern in ALLOWED_EXTERNAL_HEADERS)


def check_include_paths(repo_root: Path) -> bool:
    """
    For every source file under src/:
      - Quoted includes must resolve to an existing file.
      - Angle-bracket includes must be in ALLOWED_EXTERNAL_HEADERS and must
        NOT be in FORBIDDEN_SYSTEM_HEADERS (use LV_*_INCLUDE macros instead).
      - Macro-style includes (LV_*_INCLUDE) are always fine.
    """
    src_dir = repo_root / "src"
    ok = True
    disallowed_headers = set()
    for path in _source_files(src_dir):
        relative = path.relative_to(src_dir)
        # Is this specific file in the whitelist ?
        if any(relative == Path(entry) for entry in HEADER_CHECK_WHITELIST):
            continue

        # Is this file inside a whitelisted folder ?
        if not path.name.startswith("lv") and any(
            relative == Path(entry) or relative.is_relative_to(entry)
            for entry in HEADER_CHECK_WHITELIST
        ):
            continue

        for lineno, line in enumerate(_read_lines(path), start=1):
            if INCLUDE_MACRO_RE.search(line):
                continue

            angle = INCLUDE_ANGLE_RE.search(line)
            if angle:
                header_name = angle.group(1)
                loc = f"{path.relative_to(repo_root)}:{lineno}"
                if header_name in FORBIDDEN_SYSTEM_HEADERS:
                    _error(
                        loc,
                        f"forbidden angle-bracket include <{angle.group(1)}>. "
                        "Use the LV_*_INCLUDE macro instead.",
                    )
                    ok = False
                elif not is_allowed_header(header_name):
                    disallowed_headers.add(header_name)
                    _error(
                        loc,
                        f"unlisted angle-bracket include <{angle.group(1)}>. '{header_name}' "
                        "Add to ALLOWED_EXTERNAL_HEADERS if intentional.",
                    )
                    ok = False
                continue

            quoted = INCLUDE_QUOTED_RE.search(line)
            if quoted:
                include_path = quoted.group(1)
                resolved = (path.parent / include_path).resolve()
                if not resolved.exists():
                    _error(
                        f"{path.relative_to(repo_root)}:{lineno}",
                        f'missing quoted include "{include_path}" '
                        f"(resolved to {resolved})",
                    )
                    ok = False

    disallowed_headers = list(disallowed_headers)
    disallowed_headers.sort()
    for header in disallowed_headers:
        print(f"'{header}',")
    if ok:
        log_ok("all include paths in src/ are valid")
    return ok


# ---------------------------------------------------------------------------
# Check 4 -- deprecated headers
# ---------------------------------------------------------------------------


def check_deprecated(repo_root: Path) -> bool:
    """No source file anywhere in the repo may include a deprecated header."""
    src_dir = repo_root / "src"
    deprecated = _find_deprecated_headers(repo_root)

    if not deprecated:
        log_ok("no deprecated headers found in repo")
        return True

    include_re = re.compile(r'#\s*include\s*[<"]([^>"]+)[>"]')
    offenders: dict[Path, list[tuple[int, str]]] = {}

    for path in _source_files(repo_root):
        hits = []
        for lineno, line in enumerate(_read_lines(path), start=1):
            m = include_re.search(line)
            if m:
                included_path = Path(m.group(1))
                # Match against full relative paths, not just filenames
                if included_path in deprecated:
                    hits.append((lineno, line.strip()))
        if hits:
            offenders[path] = hits

    if not offenders:
        log_ok("no files include deprecated headers")
        return True

    for path, hits in sorted(offenders.items()):
        for lineno, line in hits:
            _error(
                f"{path.relative_to(repo_root)}:{lineno}", f"deprecated include: {line}"
            )
    return False


# ---------------------------------------------------------------------------
# Check 5 -- internal code must not include include/lvgl/* directly
# ---------------------------------------------------------------------------


def check_no_direct_public_include(repo_root: Path) -> bool:
    """
    No file under src/ may include a header from include/lvgl/ directly.
    Internal code must go through lvgl_private.h which pulls in lvgl.h.
    """
    src_dir = repo_root / "src"
    include_dir = (repo_root / "include" / "lvgl").resolve()
    ok = True
    deprecated = [p.name for p in _find_deprecated_headers(src_dir)]

    for path in _source_files(src_dir):
        for lineno, line in enumerate(_read_lines(path), start=1):
            quoted = INCLUDE_QUOTED_RE.search(line)
            if not quoted:
                continue
            resolved = (path.parent / quoted.group(1)).resolve()
            if path.name == "lvgl_public.h":
                continue
            if resolved.is_relative_to(include_dir) and path.name not in deprecated:
                _error(
                    f"{path.relative_to(repo_root)}:{lineno}",
                    f'direct public include "{quoted.group(1)}". '
                    "Internal code must include via lvgl_public.h, not include/lvgl/ directly.",
                )
                ok = False

    if ok:
        log_ok("no src/ file includes include/lvgl/ directly")
    return ok


# ---------------------------------------------------------------------------
# Check 6 -- allowed-list audit
# ---------------------------------------------------------------------------


def check_allowed_list_audit(repo_root: Path) -> bool:
    """
    For every header in ALLOWED_EXTERNAL_HEADERS, search for literal
    #include <header> in both src/ and include/lvgl/.

    Report:
      green  -- used, all usages are in src/
      yellow -- used, but at least one usage is in include/lvgl/ (leaks to users)
      red    -- never used anywhere (should be removed from the allow-list)

    Fails if any header is unused.
    """
    if not ALLOWED_EXTERNAL_HEADERS:
        log_ok("ALLOWED_EXTERNAL_HEADERS is empty, nothing to audit")
        return True

    src_dir = repo_root / "src"
    include_dir = repo_root / "include" / "lvgl"
    ok = True

    col = max(len(h) for h in ALLOWED_EXTERNAL_HEADERS) + 4  # padding for <header>

    print(f"\n{C.BOLD}Allowed system header usage report:{C.RESET}\n")

    for header in sorted(ALLOWED_EXTERNAL_HEADERS):
        pattern = rf"#\s*include\s*<{re.escape(header)}>"
        src_hits = _grep(pattern, src_dir)
        inc_hits = _grep(pattern, include_dir)

        padded = f"<{header}>".ljust(col)

        if not src_hits and not inc_hits:
            print(
                f"  {C.RED}{C.BOLD}{padded}{C.RESET}  "
                f"{C.RED}unused -- remove from ALLOWED_EXTERNAL_HEADERS{C.RESET}"
            )
            ok = False

        elif inc_hits:
            print(
                f"  {C.YELLOW}{C.BOLD}{padded}{C.RESET}  "
                f"{C.YELLOW}leaks into public headers "
                f"({len(src_hits)} src, {len(inc_hits)} include){C.RESET}"
            )
            for path, lineno, line in sorted(inc_hits):
                _warn(f"  {path.relative_to(repo_root)}:{lineno}", line.strip())

        else:
            print(
                f"  {C.GREEN}{padded}{C.RESET}  " f"used in {len(src_hits)} src file(s)"
            )

    print()
    if ok:
        log_ok("all allowed system headers are in use")
    return ok


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

CHECKS = {
    "public-api": check_public_api,
    "private-api": check_private_api,
    "include-paths": check_include_paths,
    "allowed-list-audit": check_allowed_list_audit,
    "deprecated": check_deprecated,
    "no-direct-public-include": check_no_direct_public_include,
}


def main() -> None:
    parser = argparse.ArgumentParser(
        description="LVGL header consistency checker",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="Subcommands:\n" + "\n".join(f"  {k}" for k in CHECKS),
    )
    parser.add_argument(
        "check",
        choices=list(CHECKS),
        help="Which check to run",
    )
    parser.add_argument(
        "--root",
        default=".",
        help="Repository root (default: current directory)",
    )
    args = parser.parse_args()

    repo_root = Path(args.root).resolve()
    passed = CHECKS[args.check](repo_root)
    sys.exit(0 if passed else 1)


if __name__ == "__main__":
    main()
