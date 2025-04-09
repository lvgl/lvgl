#ifndef LV_TEST_CONF_FULL_H
#define LV_TEST_CONF_FULL_H

#define LV_MEM_SIZE                     (32 * 1024 * 1024)
#define LV_DRAW_SW_SHADOW_CACHE_SIZE    8
#define LV_DRAW_THREAD_STACK_SIZE    (64 * 1024) /*Increase stack size to 64KB in order to run ThorVG*/
#define LV_USE_LOG              1
#define LV_LOG_LEVEL            LV_LOG_LEVEL_TRACE
#define LV_LOG_PRINTF           1
#define LV_USE_ASSERT_NULL      1
#define LV_USE_ASSERT_MALLOC    1
#define LV_USE_ASSERT_MEM_INTEGRITY     1
#define LV_USE_ASSERT_OBJ               1
#define LV_USE_ASSERT_STYLE             1
#define LV_USE_FLOAT      1
#define LV_USE_MATRIX     1

#define LV_FONT_MONTSERRAT_8    1
#define LV_FONT_MONTSERRAT_10   1
#define LV_FONT_MONTSERRAT_12   1
#define LV_FONT_MONTSERRAT_14   1
#define LV_FONT_MONTSERRAT_16   1
#define LV_FONT_MONTSERRAT_18   1
#define LV_FONT_MONTSERRAT_20   1
#define LV_FONT_MONTSERRAT_22   1
#define LV_FONT_MONTSERRAT_24   1
#define LV_FONT_MONTSERRAT_26   1
#define LV_FONT_MONTSERRAT_28   1
#define LV_FONT_MONTSERRAT_30   1
#define LV_FONT_MONTSERRAT_32   1
#define LV_FONT_MONTSERRAT_34   1
#define LV_FONT_MONTSERRAT_36   1
#define LV_FONT_MONTSERRAT_38   1
#define LV_FONT_MONTSERRAT_40   1
#define LV_FONT_MONTSERRAT_42   1
#define LV_FONT_MONTSERRAT_44   1
#define LV_FONT_MONTSERRAT_46   1
#define LV_FONT_MONTSERRAT_48   1
#define LV_FONT_MONTSERRAT_28_COMPRESSED    1
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW    1
#define LV_FONT_SIMSUN_14_CJK   1
#define LV_FONT_SIMSUN_16_CJK   1
#define LV_FONT_UNSCII_8        1
#define LV_FONT_UNSCII_16       1
#define LV_FONT_DEFAULT         &lv_font_montserrat_14
#define LV_FONT_FMT_TXT_LARGE   1
#define LV_USE_FONT_COMPRESSED  1
#define LV_USE_BIDI 1
#define LV_USE_ARABIC_PERSIAN_CHARS 1
#define LV_USE_PERF_MONITOR         1
#define LV_USE_MEM_MONITOR          1
#define LV_LABEL_TEXT_SELECTION     1

#define LV_USE_CALENDAR_CHINESE 1
#define LV_USE_LOTTIE 1

#define LV_USE_FLEX 1
#define LV_USE_GRID 1

#define LV_USE_FS_STDIO     1
#define LV_FS_STDIO_LETTER  'A'
#define LV_FS_STDIO_CACHE_SIZE 512
#ifndef _WIN32
    #define LV_USE_FS_POSIX     1
    #define LV_FS_POSIX_LETTER  'B'
#else
    #define LV_USE_FS_WIN32 1
    #define LV_FS_WIN32_LETTER 'C'
#endif
#define LV_USE_FS_MEMFS     1
#define LV_FS_MEMFS_LETTER  'M'

#define LV_FS_DEFAULT_DRIVER_LETTER 'A'

#define LV_USE_MONKEY       1
#define LV_USE_RLE          1
#define LV_USE_LODEPNG      1
#define LV_USE_LIBPNG       1
#define LV_USE_BMP          1
#define LV_USE_TJPGD        1
#ifndef _WIN32
    #define LV_USE_LIBJPEG_TURBO   1
#endif
#define LV_USE_GIF          1
#define LV_USE_QRCODE       1
#define LV_USE_BARCODE      1
#define LV_USE_FRAGMENT     1
#define LV_USE_IMGFONT      1
#define LV_USE_IME_PINYIN       1
#define LV_USE_OBSERVER         1
#define LV_USE_FILE_EXPLORER    1
#define LV_USE_TINY_TTF         1
#define LV_TINY_TTF_FILE_SUPPORT 1
#define LV_USE_SYSMON           1
#define LV_USE_MEM_MONITOR      1
#define LV_USE_PERF_MONITOR     1
#define LV_USE_SNAPSHOT         1
#define LV_USE_THORVG_INTERNAL  1
#define LV_USE_LZ4_INTERNAL     1
#define LV_USE_VECTOR_GRAPHIC   1
#define LV_USE_SVG              1
#define LV_USE_SVG_ANIMATION    1
#define LV_USE_SVG_DEBUG        1
#define LV_USE_PROFILER         1
#define LV_PROFILER_INCLUDE     "lv_profiler_builtin.h"
#define LV_USE_GRIDNAV          1
#define LV_USE_XML              1
#define LV_USE_TEST             1
#define LV_USE_TEST_SCREENSHOT_COMPARE  1

#define LV_BUILD_EXAMPLES       1
#define LV_USE_DEMO_WIDGETS     1
#define LV_USE_DEMO_KEYPAD_AND_ENCODER     1
#define LV_USE_DEMO_FLEX_LAYOUT            1
#define LV_USE_DEMO_STRESS      1
#define LV_USE_DEMO_TRANSFORM   1
#define LV_USE_DEMO_MULTILANG   1
#define LV_USE_DEMO_RENDER      1
#define LV_USE_DEMO_MUSIC       1
#define LV_USE_DEMO_BENCHMARK   1
#define LV_USE_DEMO_SCROLL      1
#define LV_USE_DEMO_EBIKE       1
#define LV_USE_DEMO_VECTOR_GRAPHIC  1
#define LV_USE_DEMO_HIGH_RES    1
#define LV_USE_DEMO_SMARTWATCH      1

#define LV_USE_OBJ_ID           1
#define LV_OBJ_ID_AUTO_ASSIGN    1
#define LV_USE_OBJ_ID_BUILTIN   1

#define LV_USE_OBJ_NAME         1

#define LV_CACHE_DEF_SIZE       (10 * 1024 * 1024)

#ifndef LV_USE_LINUX_DRM
    #define LV_USE_LINUX_DRM    1
#endif

#ifndef LV_USE_LINUX_FBDEV
    #define LV_USE_LINUX_FBDEV  1
#endif

#ifndef LV_USE_WAYLAND
    #define LV_USE_WAYLAND  1
    #define LV_WAYLAND_WINDOW_DECORATIONS 1
#endif

#define LV_USE_ILI9341      1
#define LV_USE_ST7735       1
#define LV_USE_ST7789       1
#define LV_USE_ST7796       1

#ifndef LV_USE_LIBINPUT
    #define LV_USE_LIBINPUT     1
#endif

#ifndef LV_LIBINPUT_XKB
    #define LV_LIBINPUT_XKB     1
#endif

#ifndef LV_USE_OPENGLES
    #if !defined(NON_AMD64_BUILD) && !defined(_MSC_VER) && !defined(_WIN32)
        #define LV_USE_OPENGLES 1
    #endif
#endif

#define LV_USE_FREETYPE 1
#define LV_FREETYPE_USE_LVGL_PORT 0
#define LV_FREETYPE_CACHE_FT_GLYPH_CNT 64

#define LV_USE_FONT_MANAGER 1

#define LV_USE_DRAW_SW_COMPLEX_GRADIENTS    1

#define LV_USE_GESTURE_RECOGNITION 1

#endif /* LV_TEST_CONF_FULL_H */
