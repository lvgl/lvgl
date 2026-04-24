---
applyTo: "src/**/*.c,src/**/*.h"
---

# LVGL C Source Review Rules

## Memory Safety

- Every `lv_malloc` / `lv_realloc` must have a NULL check immediately after
- Every allocation must have a matching `lv_free` on all exit paths (including error paths)
- `lv_free(NULL)` is safe — don't add redundant NULL checks before it
- Don't add `LV_ASSERT_MALLOC` after functions that already check internally (e.g. `lv_vg_lite_path_create`)
- Pointer parameters: NULL-check before dereference when caller could reasonably pass NULL
- Array access must be bounds-checked

## Error Handling

- Functions returning `lv_result_t` must be checked by callers
- Error paths must clean up all allocated resources before returning
- Use `LV_ASSERT_*` for debug-time invariant checks, not as runtime error handling
- Public API functions should validate arguments with `LV_ASSERT_OBJ`, `LV_ASSERT_NULL`, etc. at entry
- Graceful runtime fallback is preferred over assert for recoverable failures (e.g. cache allocation failure should degrade, not crash)
- Add `LV_LOG_WARN` for unexpected but recoverable conditions
- Add `LV_LOG_ERROR` for conditions that indicate a bug

## Performance

- No heap allocation in hot paths (draw loops, event handlers, timer callbacks)
- Prefer stack allocation for small temporary buffers
- Avoid `lv_obj_is_valid()` in internal callbacks — it walks the entire object tree
- Cache-friendly access: sequential over random
- Struct member ordering: group pointers together, then smaller types, to minimize padding bytes

## Portability

- No platform headers without `#if` guards
- No compiler extensions without fallback
- No `float`/`double` in core code unless behind `LV_USE_FLOAT`
- Use `LV_ATTRIBUTE_*` macros for alignment/section placement
- Use `"%" LV_PRId32` for `int32_t` format strings, not `"%d"`

## Include Structure

- `.h` files: `#ifndef LV_<MODULE>_H` / `#define` / `#endif`
- Feature-gated: `#if LV_USE_<FEATURE>` near top, `#endif` at bottom
- Only the matching `.h` include above the feature guard
- Never include test headers in production source files
