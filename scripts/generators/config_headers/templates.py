"""Static text blocks for the generated headers.

These are the hand-maintained, non-derived parts of each file (include logic,
attribute fallbacks, platform bridges).  The option *bodies* are generated from
the typed model; everything here is verbatim boilerplate.
"""

# ============================================================================
# lv_conf_template.h
# ============================================================================

TEMPLATE_HEADER = """\
/**
 * @file lv_conf.h
 * Configuration file for v{ver}
 */

/*
 * Copy this file as `lv_conf.h`
 * 1. simply next to `lvgl` folder
 * 2. or to any other place and
 *    - define `LV_CONF_INCLUDE_SIMPLE`;
 *    - add the path as an include path.
 */

/* clang-format off */
#if 0 /* Set this to "1" to enable content */

#ifndef LV_CONF_H
#define LV_CONF_H

/* If you need to include anything here, do it inside the `__ASSEMBLY__` guard */
#if 0 && defined(__ASSEMBLY__)
#include "my_include.h"
#endif
"""

TEMPLATE_FOOTER = """\
/*--END OF LV_CONF_H--*/

#endif /*LV_CONF_H*/

#endif /*End of "Content enable"*/
"""

# ============================================================================
# lv_conf_internal.h
# ============================================================================

INTERNAL_PREAMBLE = """\
/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_internal.h
 * This file ensures all defines of lv_conf.h have a default value.
 */

#ifndef LV_CONF_INTERNAL_H
#define LV_CONF_INTERNAL_H
/* clang-format off */

/* Config options */
__CONFIG_OPTIONS__

/** Handle special Kconfig options. */
#ifndef LV_KCONFIG_IGNORE
    #include "lv_conf_kconfig.h"
    #if defined(CONFIG_LV_CONF_SKIP) && !defined(LV_CONF_SKIP)
        #define LV_CONF_SKIP
    #endif
#endif

/* If "lv_conf.h" is available from here try to use it later. */
#ifdef __has_include
    #if __has_include("lv_conf.h")
        #ifndef LV_CONF_INCLUDE_SIMPLE
            #define LV_CONF_INCLUDE_SIMPLE
        #endif
    #endif
#endif

/* If lv_conf.h is not skipped, include it. */
#if !defined(LV_CONF_SKIP) || defined(LV_CONF_PATH)
    #ifdef LV_CONF_PATH                           /* If there is a path defined for lv_conf.h, use it */
        #include LV_CONF_PATH                     /* Note: Make sure to define custom CONF_PATH as a string */
    #elif defined(LV_CONF_INCLUDE_SIMPLE)         /* Or simply include lv_conf.h is enabled. */
        #include "lv_conf.h"
    #else
        #include "../../../../lv_conf.h"                /* Else assume lv_conf.h is next to the lvgl folder. */
    #endif
    #if !defined(LV_CONF_H) && !defined(LV_CONF_SUPPRESS_DEFINE_CHECK)
        /* #include will sometimes silently fail when __has_include is used */
        /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80753 */
        #pragma message("Possible failure to include lv_conf.h, please read the comment in this file if you get errors")
    #endif
#endif

#ifdef CONFIG_LV_COLOR_DEPTH
    #define LV_KCONFIG_PRESENT
#endif

/* 
 * Detect if the user is using the new calendar day/month configuration
 * in order to avoid warnings for users that have migrated.
 */
#ifdef LV_MONDAY_STR
#define LV_CALENDAR_DISABLE_DEFAULT_DAY_NAMES 1
#else
#define LV_CALENDAR_DISABLE_DEFAULT_DAY_NAMES 0
#endif

/* 
 * Detect if the user is using the new calendar day/month configuration
 * in order to avoid warnings for users that have migrated.
 */
#ifdef LV_JANUARY_STR
#define LV_CALENDAR_DISABLE_DEFAULT_MONTH_NAMES 1
#else
#define LV_CALENDAR_DISABLE_DEFAULT_MONTH_NAMES 0
#endif

/* 
 * Detect if the user is using the xkb keymap configuration
 * in order to avoid warnings for users that have migrated.
 */
#ifdef LV_LIBINPUT_XKB_RULES
#define LV_LIBINPUT_XKB_DISABLE_KEY_MAP 1
#else
#define LV_LIBINPUT_XKB_DISABLE_KEY_MAP 0
#endif

/*----------------------------------
 * Start parsing lv_conf_template.h
 -----------------------------------*/
"""

INTERNAL_COMPATIBILITY_BLOCK = r"""
/*----------------------------------
 * End of parsing lv_conf_template.h
 -----------------------------------*/

/*----------------------------------
 * Start of compatibility block
 -----------------------------------*/

/*  
 *  TODO: Remove this for v10.
 */

/*
 *  Before the user selected either LV_USE_LZ4_INTERNAL or LV_USE_LZ4_EXTERNAL
 *  For v9.6 LV_USE_LZ4_EXTERNAL doesn't exist anymore, instead the user 
 *  enables LV_USE_LZ4 and disables LV_USE_LZ4_INTERNAL
 *  To support users using LV_USE_LZ4_EXTERNAL from before v9.6 we 
 *  we enable LV_USE_LZ4 for them
 */
#if defined(LV_USE_LZ4_EXTERNAL) && LV_USE_LZ4_EXTERNAL
#if !LV_USE_LZ4
    #warning LV_USE_LZ4_EXTERNAL is deprecated and will be removed in a future release. Enable LV_USE_LZ4 and disable LV_USE_LZ4_INTERNAL to continue using an external version of LZ4
    #undef LV_USE_LZ4
    #define LV_USE_LZ4 1
#endif /*!LV_USE_LZ4*/
#endif /*defined(LV_USE_LZ4_EXTERNAL) && LV_USE_LZ4_EXTERNAL*/

/*  
 *  Before the user selected either LV_USE_THORVG_INTERNAL or LV_USE_THORVG_EXTERNAL
 *  For v9.6 LV_USE_THORVG_EXTERNAL doesn't exist anymore, instead the user 
 *  enables LV_USE_THORVG and disables LV_USE_THORVG_INTERNAL
 *  To support users using LV_USE_THORVG_EXTERNAL from before v9.6 we 
 *  we enable LV_USE_THORVG for them
 */
#if defined(LV_USE_THORVG_EXTERNAL) && LV_USE_THORVG_EXTERNAL
#if !LV_USE_THORVG
    #warning LV_USE_THORVG_EXTERNAL is deprecated and will be removed in a future release. Enable LV_USE_THORVG and disable LV_USE_THORVG_INTERNAL to continue using an external version of THORVG
    #undef LV_USE_THORVG
    #define LV_USE_THORVG 1
#endif /*!LV_USE_THORVG*/
#endif /*defined(LV_USE_THORVG_EXTERNAL) && LV_USE_THORVG_EXTERNAL*/

/*  
 *  Backward compatibility. Before the user selected either 
 *  LV_X11_RENDER_MODE_PARTIAL or LV_X11_RENDER_MODE_DIRECT or
 *  LV_X11_RENDER_MODE_FULL. For v9.6, this becomes a single choice:
 *  LV_X11_RENDER_MODE which maps to a LV_DISPLAY_RENDER_MODE value.
 */
#if defined(LV_X11_RENDER_MODE_PARTIAL) && LV_X11_RENDER_MODE_PARTIAL
    #warning LV_X11_RENDER_MODE_PARTIAL is deprecated and will be removed in a future release. Set LV_X11_RENDER_MODE to LV_DISPLAY_RENDER_MODE_PARTIAL instead.
    #undef LV_X11_RENDER_MODE
    #define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_PARTIAL
#endif /*defined(LV_X11_RENDER_MODE_PARTIAL) && LV_X11_RENDER_MODE_PARTIAL*/

#if defined(LV_X11_RENDER_MODE_DIRECT) && LV_X11_RENDER_MODE_DIRECT
    #warning LV_X11_RENDER_MODE_DIRECT is deprecated and will be removed in a future release. Set LV_X11_RENDER_MODE to LV_DISPLAY_RENDER_MODE_DIRECT instead.
    #undef LV_X11_RENDER_MODE
    #define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_DIRECT
#endif /*defined(LV_X11_RENDER_MODE_DIRECT) && LV_X11_RENDER_MODE_DIRECT*/

#if defined(LV_X11_RENDER_MODE_FULL) && LV_X11_RENDER_MODE_FULL
    #warning LV_X11_RENDER_MODE_FULL is deprecated and will be removed in a future release. Set LV_X11_RENDER_MODE to LV_DISPLAY_RENDER_MODE_FULL instead.
    #undef LV_X11_RENDER_MODE
    #define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_FULL
#endif /*defined(LV_X11_RENDER_MODE_FULL) && LV_X11_RENDER_MODE_FULL*/

/*
 *  Before, the VG-Lite GPU was chosen with LV_VG_LITE_HAL_GPU_SERIES (a bare
 *  token such as gc255) and LV_VG_LITE_HAL_GPU_REVISION (a hex revision), which
 *  were pasted into the options include path.  For v9.x these are replaced by the
 *  LV_VG_LITE_GPU choice.  Map the old hex revisions (each unique to one series)
 *  to it; anything else falls back to the GC255 default.
 */
#if defined(LV_VG_LITE_HAL_GPU_REVISION)
    /* Only remap when LV_VG_LITE_GPU is still at its default, i.e. the user has not
    * migrated to it yet*/
    #if LV_VG_LITE_GPU == LV_VG_LITE_GPU_GC255_0X40A
        #warning LV_VG_LITE_HAL_GPU_SERIES/LV_VG_LITE_HAL_GPU_REVISION are deprecated and will be removed in a future release. Select your GPU with LV_VG_LITE_GPU instead.
        #undef LV_VG_LITE_GPU
        #if LV_VG_LITE_HAL_GPU_REVISION == 0x423
            #define LV_VG_LITE_GPU LV_VG_LITE_GPU_GC555_0X423
        #elif LV_VG_LITE_HAL_GPU_REVISION == 0x1003
            #define LV_VG_LITE_GPU LV_VG_LITE_GPU_GCNANOULTRAV_0X1003
        #else
            #define LV_VG_LITE_GPU LV_VG_LITE_GPU_GC255_0X40A
        #endif
    #endif /*LV_VG_LITE_GPU == LV_VG_LITE_GPU_GC255_0X40A*/
#endif /*defined(LV_VG_LITE_HAL_GPU_REVISION)*/

/*
 *  Legacy backend inference for the Linux DRM and SDL drivers.  Historically the
 *  EGL backend was turned on automatically from LV_USE_OPENGLES; for v9.x each
 *  driver gets an explicit LV_<DRIVER>_BACKEND choice instead.  While the
 *  deprecated LV_<DRIVER>_AUTO_BACKEND is set (its default) we reproduce the old
 *  inference here and pre-define the per-driver flag, so the Derived-capability
 *  ladder below (which is #ifndef-guarded) leaves it untouched.  Runs before that
 *  ladder.  The #warning only fires when inference actually turns EGL on.
 */
#if LV_USE_LINUX_DRM && LV_LINUX_DRM_AUTO_BACKEND
    #ifndef LV_LINUX_DRM_USE_EGL
        #if LV_USE_OPENGLES
            #warning LV_LINUX_DRM_AUTO_BACKEND is deprecated and will be removed in a future release. Set it to 0 and select a backend with LV_LINUX_DRM_BACKEND.
            #define LV_LINUX_DRM_USE_EGL 1
        #else
            #define LV_LINUX_DRM_USE_EGL 0
        #endif
    #endif
    #ifndef LV_USE_LINUX_DRM_GBM_BUFFERS
        #define LV_USE_LINUX_DRM_GBM_BUFFERS LV_LINUX_DRM_USE_EGL
    #endif
#endif /*LV_USE_LINUX_DRM && LV_LINUX_DRM_AUTO_BACKEND*/

#if LV_USE_SDL && LV_SDL_AUTO_BACKEND
    #ifndef LV_SDL_USE_EGL
        #if LV_USE_OPENGLES && (LV_USE_DRAW_OPENGLES || LV_USE_DRAW_NANOVG)
            #warning LV_SDL_AUTO_BACKEND is deprecated and will be removed in a future release. Set it to 0 and select a backend with LV_SDL_BACKEND.
            #define LV_SDL_USE_EGL 1
        #else
            #define LV_SDL_USE_EGL 0
        #endif
    #endif
#endif /*LV_USE_SDL && LV_SDL_AUTO_BACKEND*/

/* Wayland never inferred its backend from LV_USE_OPENGLES; the legacy interface
 * was setting LV_WAYLAND_USE_* directly.  While LV_WAYLAND_AUTO_BACKEND is set we
 * honor any such define, default to SHM, and keep the three mutually exclusive.
 * The #warning fires only if a legacy LV_WAYLAND_USE_* was set by hand. */
#if LV_USE_WAYLAND && LV_WAYLAND_AUTO_BACKEND
    #if defined(LV_WAYLAND_USE_EGL) || defined(LV_WAYLAND_USE_G2D) || defined(LV_WAYLAND_USE_SHM)
        #warning Setting LV_WAYLAND_USE_* directly is deprecated and will be removed in a future release. Set LV_WAYLAND_AUTO_BACKEND to 0 and select a backend with LV_WAYLAND_BACKEND.
    #endif
    #ifndef LV_WAYLAND_USE_EGL
        #define LV_WAYLAND_USE_EGL 0
    #endif
    #ifndef LV_WAYLAND_USE_G2D
        #define LV_WAYLAND_USE_G2D 0
    #endif
    #ifndef LV_WAYLAND_USE_SHM
        #if LV_WAYLAND_USE_EGL || LV_WAYLAND_USE_G2D
            #define LV_WAYLAND_USE_SHM 0
        #else
            #define LV_WAYLAND_USE_SHM 1
        #endif
    #endif
#endif /*LV_USE_WAYLAND && LV_WAYLAND_AUTO_BACKEND*/

#if defined(LV_ASSERT_HANDLER_INCLUDE) && !LV_DISABLE_ASSERT_HANDLER_INCLUDE_WARNING
#warning "LV_ASSERT_HANDLER_INCLUDE is deprecated and will be removed in a future release. Use LV_ASSERT_CUSTOM_INCLUDE and define LV_ASSERT_HANDLER inside. To suppress this warning, remove LV_ASSERT_HANDLER_INCLUDE or enable LV_DISABLE_ASSERT_HANDLER_INCLUDE_WARNING."
#include LV_ASSERT_HANDLER_INCLUDE
#endif


/*----------------------------------
 * End of compatibility block
 -----------------------------------*/
"""

INTERNAL_FOOTER = r"""
#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TICK_INC
#endif

#ifndef LV_ATTRIBUTE_TIMER_HANDLER
#define LV_ATTRIBUTE_TIMER_HANDLER
#endif

#ifndef LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_FLUSH_READY
#endif

#ifndef LV_ATTRIBUTE_SYNC_READY
#define LV_ATTRIBUTE_SYNC_READY
#endif

#ifndef LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_CONST
#endif

#ifndef LV_ATTRIBUTE_LARGE_RAM_ARRAY
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY
#endif

#ifndef LV_ATTRIBUTE_FAST_MEM
#define LV_ATTRIBUTE_FAST_MEM
#endif

#ifndef LV_ATTRIBUTE_EXTERN_DATA
#define LV_ATTRIBUTE_EXTERN_DATA
#endif

#ifndef LV_EXPORT_CONST_INT
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning
#endif

/*Fix inconsistent name*/
#define LV_USE_ANIMIMAGE LV_USE_ANIMIMG

#ifndef __ASSEMBLY__
LV_EXPORT_CONST_INT(LV_DPI_DEF);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_STRIDE_ALIGN);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_ALIGN);
#endif

#undef LV_KCONFIG_PRESENT


#if LV_USE_OS
    #if (LV_USE_FREETYPE || LV_USE_THORVG) && LV_DRAW_THREAD_STACK_SIZE < (32 * 1024)
        #error "Increase LV_DRAW_THREAD_STACK_SIZE to at least 32KB for FreeType or ThorVG."
    #endif

    #if defined(LV_DRAW_THREAD_STACKSIZE) && !defined(LV_DRAW_THREAD_STACK_SIZE)
        #warning "LV_DRAW_THREAD_STACKSIZE was renamed to LV_DRAW_THREAD_STACK_SIZE. Please update lv_conf.h or run menuconfig again."
        #define LV_DRAW_THREAD_STACK_SIZE LV_DRAW_THREAD_STACKSIZE
    #endif
#endif

/*Allow only upper case letters and '/'  ('/' is a special case for backward compatibility)*/
#define LV_FS_IS_VALID_LETTER(l) ((l) == '/' || ((l) >= 'A' && (l) <= 'Z'))

/* If running without lv_conf.h, add typedefs with default value. */
#ifdef LV_CONF_SKIP
    #if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /*Disable warnings for Visual Studio*/
        #define _CRT_SECURE_NO_WARNINGS
    #endif
#endif  /*defined(LV_CONF_SKIP)*/

#ifndef LV_CHECK_ARG_LOG_MODE
    #define LV_CHECK_ARG_LOG_MODE   0
#endif
"""

# Closing guard of lv_conf_internal.h.  Kept separate from INTERNAL_FOOTER so the
# generated `select` guards can be emitted between the footer derivations and the
# final `#endif`.
INTERNAL_CLOSE = """
#endif  /*LV_CONF_INTERNAL_H*/
"""


# ============================================================================
# lv_conf_kconfig.h  (the CONFIG_* -> enum/token bridge)
# ============================================================================

KCONFIG_BRIDGE_PREAMBLE = """\
/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_kconfig.h
 * Configs that need special handling when LVGL is used with Kconfig
 */

#ifndef LV_CONF_KCONFIG_H
#define LV_CONF_KCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LV_CONF_KCONFIG_EXTERNAL_INCLUDE
#include LV_CONF_KCONFIG_EXTERNAL_INCLUDE
#else

#ifdef ESP_PLATFORM
#include "sdkconfig.h"
#include "esp_attr.h"
#endif

#ifdef __NuttX__
#include <nuttx/config.h>
/*
 * Make sure version number in Kconfig file is correctly set.
 * Mismatch can happen when user manually copy lvgl/Kconfig file to their project, like what NuttX does.
 */
#include "../lv_version.h"

#if CONFIG_LVGL_VERSION_MAJOR != LVGL_VERSION_MAJOR || CONFIG_LVGL_VERSION_MINOR != LVGL_VERSION_MINOR \\
        || CONFIG_LVGL_VERSION_PATCH != LVGL_VERSION_PATCH
#warning "Version mismatch between Kconfig and lvgl/lv_version.h"
#endif
#elif defined(__RTTHREAD__)
#define LV_CONF_INCLUDE_SIMPLE
#include <lv_rt_thread_conf.h>
#endif

#endif /*LV_CONF_KCONFIG_EXTERNAL_INCLUDE*/

"""

KCONFIG_BRIDGE_DEPRECATIONS = """\

/*******************
 * LV_MEM_SIZE
 *******************/

#if defined(CONFIG_LV_MEM_SIZE_KILOBYTES) && CONFIG_LV_MEM_SIZE_KILOBYTES > 0
#warning "LV_MEM_SIZE_KILOBYTES is deprecated, use LV_MEM_SIZE instead (value in bytes)"
#ifndef CONFIG_LV_MEM_SIZE
#define CONFIG_LV_MEM_SIZE (LV_MEM_SIZE_KILOBYTES * 1024U)
#else
#warning "Both LV_MEM_SIZE and LV_MEM_SIZE_KILOBYTES are defined. Using LV_MEM_SIZE"
#endif
#undef CONFIG_LV_MEM_SIZE_KILOBYTES
#endif

#if defined(CONFIG_LV_MEM_POOL_EXPAND_SIZE_KILOBYTES) && CONFIG_LV_MEM_POOL_EXPAND_SIZE_KILOBYTES > 0
#warning "LV_MEM_POOL_EXPAND_SIZE_KILOBYTES is deprecated, set the full memory size with LV_MEM_SIZE instead (value in bytes)"
#define CONFIG_LV_MEM_POOL_EXPAND_SIZE (CONFIG_LV_MEM_POOL_EXPAND_SIZE_KILOBYTES * 1024U)
#endif

/*******************
 * LV_ASSERT_HANDLER_INCLUDE
 *******************/

#if defined(CONFIG_LV_WARN_ABOUT_ASSERT_HANDLER_INCLUDE) && CONFIG_LV_WARN_ABOUT_ASSERT_HANDLER_INCLUDE
#warning LV_ASSERT_HANDLER_INCLUDE is deprecated and will be removed in a future release. Use LV_ASSERT_CUSTOM_INCLUDE instead.
#define LV_ASSERT_HANDLER_INCLUDE CONFIG_LV_ASSERT_HANDLER_INCLUDE
#endif

/*******************
 * LV_SDL_BUF_COUNT
 *******************/

#if defined(CONFIG_LV_SDL_SINGLE_BUFFER)
#warning "LV_SDL_SINGLE_BUFFER is deprecated, use LV_SDL_BUF_COUNT instead"
#undef CONFIG_LV_SDL_BUF_COUNT
#define CONFIG_LV_SDL_BUF_COUNT 1
#elif defined(CONFIG_LV_SDL_DOUBLE_BUFFER)
#warning "LV_SDL_DOUBLE_BUFFER is deprecated, use LV_SDL_BUF_COUNT instead"
#undef CONFIG_LV_SDL_BUF_COUNT
#define CONFIG_LV_SDL_BUF_COUNT 2
#endif

/*******************
 * LV_CONF_MINIMAL
 *******************/

#if defined(CONFIG_LV_CONF_MINIMAL)
#warning "LV_CONF_MINIMAL has been removed and no longer has any effect. Start from configs/defconfigs/minimal.defconfig instead."
#undef CONFIG_LV_CONF_MINIMAL
#endif
"""

KCONFIG_BRIDGE_FOOTER = """\

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CONF_KCONFIG_H*/
"""


# ============================================================================
# Hard-coded data
# ============================================================================
# The token -> font-pointer mapping for LV_FONT_DEFAULT.  This is the only
# table that cannot come from Kconfig: the `&lv_font_*` pointers are C symbols,
# not config values.  LV_FONT_DEFAULT carries the *token* on both build paths
# (e.g. `LV_FONT_DEFAULT_MONTSERRAT_14`); these defines resolve it to the
# pointer, so they are emitted into the internal "Config options" block.
BUILTIN_FONTS: dict = {
    f"LV_FONT_DEFAULT_MONTSERRAT_{n}": f"&lv_font_montserrat_{n}"
    for n in (
        8,
        10,
        12,
        14,
        16,
        18,
        20,
        22,
        24,
        26,
        28,
        30,
        32,
        34,
        36,
        38,
        40,
        42,
        44,
        46,
        48,
    )
}
BUILTIN_FONTS.update(
    {
        "LV_FONT_DEFAULT_MONTSERRAT_28_COMPRESSED": "&lv_font_montserrat_28_compressed",
        "LV_FONT_DEFAULT_DEJAVU_16_PERSIAN_HEBREW": "&lv_font_dejavu_16_persian_hebrew",
        "LV_FONT_DEFAULT_SOURCE_HAN_SANS_SC_14_CJK": "&lv_font_source_han_sans_sc_14_cjk",
        "LV_FONT_DEFAULT_SOURCE_HAN_SANS_SC_16_CJK": "&lv_font_source_han_sans_sc_16_cjk",
        "LV_FONT_DEFAULT_UNSCII_8": "&lv_font_unscii_8",
        "LV_FONT_DEFAULT_UNSCII_16": "&lv_font_unscii_16",
    }
)


def version_string(kconf) -> str:
    def val(name, default):
        sym = kconf.syms.get(name)
        return sym.str_value if sym else default

    return ".".join(
        (
            val("LVGL_VERSION_MAJOR", "9"),
            val("LVGL_VERSION_MINOR", "0"),
            val("LVGL_VERSION_PATCH", "0"),
        )
    )
