/**
 * @file lv_init.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "core/lv_obj.h"
#include "disp/lv_disp_private.h"
#include "indev/lv_indev_private.h"
#include "layouts/lv_layouts.h"
#include "libs/bmp/lv_bmp.h"
#include "libs/ffmpeg/lv_ffmpeg.h"
#include "libs/freetype/lv_freetype.h"
#include "libs/fsdrv/lv_fsdrv.h"
#include "libs/gif/lv_gif.h"
#include "libs/png/lv_png.h"
#include "libs/sjpg/lv_sjpg.h"
#include "draw/lv_draw.h"
#include "draw/lv_img_cache_builtin.h"
#include "misc/lv_anim.h"
#include "misc/lv_timer.h"
#include "misc/lv_async.h"
#include "misc/lv_fs.h"
#include "misc/lv_gc.h"
#if LV_USE_DRAW_SW
    #include "draw/sw/lv_draw_sw.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static bool lv_initialized = false;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_is_initialized(void)
{
    return lv_initialized;
}

void lv_init(void)
{
    /*Do nothing if already initialized*/
    if(lv_initialized) {
        LV_LOG_WARN("lv_init: already inited");
        return;
    }

    LV_LOG_INFO("begin");

    /*First initialize Garbage Collection if needed*/
#ifdef LV_GC_INIT
    LV_GC_INIT();
#endif

    lv_mem_init();

#if LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN
    lv_profiler_builtin_config_t profiler_config;
    lv_profiler_builtin_config_init(&profiler_config);
    lv_profiler_builtin_init(&profiler_config);
#endif

    _lv_timer_core_init();

    _lv_fs_init();

    _lv_anim_core_init();

    _lv_group_init();

    lv_draw_init();

#if LV_USE_DRAW_SW
    lv_draw_sw_init();
#endif

    _lv_obj_style_init();
    _lv_ll_init(&LV_GC_ROOT(_lv_disp_ll), sizeof(lv_disp_t));
    _lv_ll_init(&LV_GC_ROOT(_lv_indev_ll), sizeof(lv_indev_t));

    /*Initialize the screen refresh system*/
    _lv_refr_init();

#if LV_USE_SYSMON
    _lv_sysmon_builtin_init();
#endif

    _lv_img_decoder_init();

    _lv_img_cache_builtin_init();

    /*Test if the IDE has UTF-8 encoding*/
    const char * txt = "Á";

    uint8_t * txt_u8 = (uint8_t *)txt;
    if(txt_u8[0] != 0xc3 || txt_u8[1] != 0x81 || txt_u8[2] != 0x00) {
        LV_LOG_WARN("The strings have no UTF-8 encoding. Non-ASCII characters won't be displayed.");
    }

    uint32_t endianess_test = 0x11223344;
    uint8_t * endianess_test_p = (uint8_t *) &endianess_test;
    bool big_endian = endianess_test_p[0] == 0x11 ? true : false;

    if(big_endian) {
        LV_ASSERT_MSG(LV_BIG_ENDIAN_SYSTEM == 1,
                      "It's a big endian system but LV_BIG_ENDIAN_SYSTEM is not enabled in lv_conf.h");
    }
    else {
        LV_ASSERT_MSG(LV_BIG_ENDIAN_SYSTEM == 0,
                      "It's a little endian system but LV_BIG_ENDIAN_SYSTEM is enabled in lv_conf.h");
    }

#if LV_USE_ASSERT_MEM_INTEGRITY
    LV_LOG_WARN("Memory integrity checks are enabled via LV_USE_ASSERT_MEM_INTEGRITY which makes LVGL much slower");
#endif

#if LV_USE_ASSERT_OBJ
    LV_LOG_WARN("Object sanity checks are enabled via LV_USE_ASSERT_OBJ which makes LVGL much slower");
#endif

#if LV_USE_ASSERT_STYLE
    LV_LOG_WARN("Style sanity checks are enabled that uses more RAM");
#endif

#if LV_LOG_LEVEL == LV_LOG_LEVEL_TRACE
    LV_LOG_WARN("Log level is set to 'Trace' which makes LVGL much slower");
#endif


#if LV_USE_FLEX
    lv_flex_init();
#endif

#if LV_USE_GRID
    lv_grid_init();
#endif

#if LV_USE_MSG
    lv_msg_init();
#endif

#if LV_USE_FS_FATFS != '\0'
    lv_fs_fatfs_init();
#endif

#if LV_USE_FS_STDIO != '\0'
    lv_fs_stdio_init();
#endif

#if LV_USE_FS_POSIX != '\0'
    lv_fs_posix_init();
#endif

#if LV_USE_FS_WIN32 != '\0'
    lv_fs_win32_init();
#endif

#if LV_USE_PNG
    lv_png_init();
#endif

#if LV_USE_SJPG
    lv_split_jpeg_init();
#endif

#if LV_USE_BMP
    lv_bmp_init();
#endif

    /*Make FFMPEG last because the last converter will be checked first and
     *it's superior to any other */
#if LV_USE_FFMPEG
    lv_ffmpeg_init();
#endif

#if LV_USE_FREETYPE
    /*Init freetype library*/
#  if LV_FREETYPE_CACHE_SIZE >= 0
    lv_freetype_init(LV_FREETYPE_CACHE_FT_FACES, LV_FREETYPE_CACHE_FT_SIZES, LV_FREETYPE_CACHE_SIZE);
#  else
    lv_freetype_init(0, 0, 0);
#  endif
#endif

    lv_initialized = true;

    LV_LOG_TRACE("finished");
}

#if LV_ENABLE_GC || LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN

void lv_deinit(void)
{
    _lv_gc_clear_roots();

    lv_disp_set_default(NULL);

    lv_mem_deinit();
    lv_initialized = false;

    LV_LOG_INFO("lv_deinit done");

#if LV_USE_LOG
    lv_log_register_print_cb(NULL);
#endif
}
#endif



/**********************
 *   STATIC FUNCTIONS
 **********************/
