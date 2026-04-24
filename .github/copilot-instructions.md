# LVGL Code Review Instructions

LVGL is an embedded C99 graphics library for MCUs (64KB+ RAM), MPUs, and simulators.
Code must be portable, memory-efficient, and bare-metal safe.

## Critical Issues (always flag)

- NULL dereference: every `lv_malloc`/`lv_realloc` result must be checked
- Buffer overflow or out-of-bounds array access
- Memory leak: every `lv_malloc` needs matching `lv_free` on all exit paths
- Use-after-free, uninitialized variable use
- Integer overflow in size/coordinate calculations
- Missing `#if LV_USE_xxx` / `#endif` guards for optional features
- Global variables not in `lv_global_t` — must use `LV_GLOBAL_DEFAULT()`
- Test headers included in production source (`#include "lv_test_..."` in `src/`)

## Embedded Performance (flag in hot paths)

- No heap allocation in draw loops, event handlers, timer callbacks
- Avoid `lv_obj_is_valid()` in internal callbacks — walks entire object tree
- `lv_free(NULL)` is safe — no redundant NULL check before it
- No `LV_ASSERT_MALLOC` after functions that already check internally

## Naming & Style

- `lv_<module>_<action>_<subject>` for public API; `_lv_` prefix for internal
- ALL_CAPS for enums/defines; `_t` suffix for typedefs
- 4 spaces, no tabs; function brace on new line; `if`/`for` brace on same line
- `<stdint.h>` types; block comments `/* */` only, never `//` (C++ style); `"%" LV_PRId32` for format strings
- File-scope variables must be `static`; no globals outside `lv_global_t`

## Code Organization

- Extract helpers when functions exceed ~50 lines, but don't over-engineer single-use helpers
- Eliminate code duplication across branches
- Struct layout: pointers first, then int32, then small types/bitfields (minimize padding)
- New files follow `src/misc/lv_templ.c` section template
- `#if LV_USE_xxx` near top, `#endif /* LV_USE_xxx */` at bottom

## Memory & Error Patterns

- Prefer graceful degradation over assert for recoverable failures (e.g. cache alloc fail → log + disable, not crash)
- `LV_LOG_WARN` for unexpected recoverable conditions; `LV_LOG_ERROR` for bugs
- `LV_ASSERT_NULL` for debug invariants; `LV_CHECK_ARG` for public API validation

## GPU / Draw Units

- All `vg_lite_*` calls: check with `LV_VG_LITE_CHECK_ERROR`
- Use current API names (`vg_lite_set_path_type` not deprecated variants)
- GL state: save and restore (scissor, framebuffer binding, clear color)
- `#if LV_USE_DRAW_*` guards required

## PR Requirements

- Commit: `<type>(<scope>): <subject>` — imperative, lowercase, no period, max 90 chars
- Types: `fix`, `feat`, `arch`, `perf`, `example`, `docs`, `test`, `chore`
- CI enforces ≥ 50% patch coverage on 5+ new coverable lines
- New features need tests; bug fixes need regression tests when feasible
- Doxygen `/** */` with `@param`/`@return` for all public functions in `.h` files
- Comments explain **why**, not what
