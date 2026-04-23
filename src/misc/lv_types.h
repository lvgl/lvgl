#warning Include public headers from the `src` folder is deprecated. This file will be removed soon. To ensure your application keeps working, you can choose to include the new header in <include/lvgl/lv_types.h> or include the main header file <include/lvgl/lvgl.h>
#include "../../include/lvgl/lvgl.h"
/**
 * Mark a function as deprecated so the compiler emits a warning at every call site.
 *
 * Usage — functions:
 * @code
 *   LV_DEPRECATED("Use lv_foo_new() instead")
 *   void lv_foo_old(void);
 * @endcode
 *
 * Usage — macros: wrap the macro body with LV_DEPRECATED_MACRO_WARN() so the
 * warning fires when the macro is expanded:
 * @code
 *   #define MY_OLD_MACRO(x)  (LV_DEPRECATED_MACRO_WARN("MY_OLD_MACRO is deprecated"), (x))
 * @endcode
 */
#ifndef LV_DEPRECATED
#if defined(PYCPARSER)
    #define LV_DEPRECATED(msg)
#elif defined(__GNUC__) || defined(__clang__)
    #define LV_DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
    #define LV_DEPRECATED(msg) __declspec(deprecated(msg))
#else
    #define LV_DEPRECATED(msg)
#endif
#endif /* LV_DEPRECATED not defined */

/**
 * Helper used inside deprecated macro bodies to emit a compiler warning at the
 * expansion site in user code.
 *
 * Works by declaring a local typedef tagged as deprecated and immediately using
 * it, which triggers the warning without any runtime overhead.
 *
 * Example:
 * @code
 *   #define MY_OLD_MACRO(x) \
 *       do { LV_DEPRECATED_MACRO_WARN("MY_OLD_MACRO is deprecated, use MY_NEW_MACRO"); \
 *            (x); } while(0)
 * @endcode
 */
#if defined(PYCPARSER)
    #define LV_DEPRECATED_MACRO_WARN(msg) ((void)0)
#elif defined(__GNUC__) || defined(__clang__)
    #define LV_DEPRECATED_MACRO_WARN(msg) \
        do { \
            typedef int __attribute__((deprecated(msg))) __lv_deprecated_t; \
            __lv_deprecated_t __lv_deprecated_dummy; \
            (void)__lv_deprecated_dummy; \
        } while(0)
#else
    /* Fallback: no warning, but the macro still compiles */
    #define LV_DEPRECATED_MACRO_WARN(msg) ((void)0)
#endif

