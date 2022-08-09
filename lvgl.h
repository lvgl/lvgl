/**
 * @file lvgl.h
 * Include all LVGL related headers
 */

#ifndef LVGL_H
#define LVGL_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************
 * CURRENT VERSION OF LVGL
 ***************************/
#define LVGL_VERSION_MAJOR 9
#define LVGL_VERSION_MINOR 0
#define LVGL_VERSION_PATCH 0
#define LVGL_VERSION_INFO "dev"

/*********************
 *      INCLUDES
 *********************/

#include "src/misc/lv_log.h"
#include "src/misc/lv_timer.h"
#include "src/misc/lv_math.h"
#include "src/misc/lv_mem.h"
#include "src/misc/lv_async.h"
#include "src/misc/lv_anim_timeline.h"
#include "src/misc/lv_printf.h"

#include "src/hal/lv_hal.h"

#include "src/core/lv_obj.h"
#include "src/core/lv_group.h"
#include "src/core/lv_indev.h"
#include "src/core/lv_refr.h"
#include "src/core/lv_disp.h"
#include "src/core/lv_theme.h"

#include "src/font/lv_font.h"
#include "src/font/lv_font_loader.h"
#include "src/font/lv_font_fmt_txt.h"

#include "src/widgets/animimg/lv_animimg.h"
#include "src/widgets/arc/lv_arc.h"
#include "src/widgets/bar/lv_bar.h"
#include "src/widgets/btn/lv_btn.h"
#include "src/widgets/btnmatrix/lv_btnmatrix.h"
#include "src/widgets/calendar/lv_calendar.h"
#include "src/widgets/canvas/lv_canvas.h"
#include "src/widgets/chart/lv_chart.h"
#include "src/widgets/checkbox/lv_checkbox.h"
#include "src/widgets/colorwheel/lv_colorwheel.h"
#include "src/widgets/dropdown/lv_dropdown.h"
#include "src/widgets/img/lv_img.h"
#include "src/widgets/imgbtn/lv_imgbtn.h"
#include "src/widgets/keyboard/lv_keyboard.h"
#include "src/widgets/label/lv_label.h"
#include "src/widgets/led/lv_led.h"
#include "src/widgets/line/lv_line.h"
#include "src/widgets/list/lv_list.h"
#include "src/widgets/menu/lv_menu.h"
#include "src/widgets/meter/lv_meter.h"
#include "src/widgets/msgbox/lv_msgbox.h"
#include "src/widgets/roller/lv_roller.h"
#include "src/widgets/slider/lv_slider.h"
#include "src/widgets/span/lv_span.h"
#include "src/widgets/spinbox/lv_spinbox.h"
#include "src/widgets/spinner/lv_spinner.h"
#include "src/widgets/switch/lv_switch.h"
#include "src/widgets/table/lv_table.h"
#include "src/widgets/tabview/lv_tabview.h"
#include "src/widgets/textarea/lv_textarea.h"
#include "src/widgets/tileview/lv_tileview.h"
#include "src/widgets/win/lv_win.h"

#include "src/others/snapshot/lv_snapshot.h"
#include "src/others/monkey/lv_monkey.h"
#include "src/others/gridnav/lv_gridnav.h"
#include "src/others/fragment/lv_fragment.h"
#include "src/others/imgfont/lv_imgfont.h"
#include "src/others/msg/lv_msg.h"
#include "src/others/ime/lv_ime_pinyin.h"

#include "src/libs/bmp/lv_bmp.h"
#include "src/libs/fsdrv/lv_fsdrv.h"
#include "src/libs/png/lv_png.h"
#include "src/libs/gif/lv_gif.h"
#include "src/libs/qrcode/lv_qrcode.h"
#include "src/libs/sjpg/lv_sjpg.h"
#include "src/libs/freetype/lv_freetype.h"
#include "src/libs/rlottie/lv_rlottie.h"
#include "src/libs/ffmpeg/lv_ffmpeg.h"

#include "src/layouts/flex/lv_flex.h"
#include "src/layouts/grid/lv_grid.h"

#include "src/draw/lv_draw.h"

#include "src/themes/lv_themes.h"

#include "src/lv_api_map.h"

/*-----------------
 * EXTRAS
 *----------------*/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/** Gives 1 if the x.y.z version is supported in the current version
 * Usage:
 *
 * - Require v6
 * #if LV_VERSION_CHECK(6,0,0)
 *   new_func_in_v6();
 * #endif
 *
 *
 * - Require at least v5.3
 * #if LV_VERSION_CHECK(5,3,0)
 *   new_feature_from_v5_3();
 * #endif
 *
 *
 * - Require v5.3.2 bugfixes
 * #if LV_VERSION_CHECK(5,3,2)
 *   bugfix_in_v5_3_2();
 * #endif
 *
 */
#define LV_VERSION_CHECK(x,y,z) (x == LVGL_VERSION_MAJOR && (y < LVGL_VERSION_MINOR || (y == LVGL_VERSION_MINOR && z <= LVGL_VERSION_PATCH)))

/**
 * Wrapper functions for VERSION macros
 */

static inline int lv_version_major(void)
{
    return LVGL_VERSION_MAJOR;
}

static inline int lv_version_minor(void)
{
    return LVGL_VERSION_MINOR;
}

static inline int lv_version_patch(void)
{
    return LVGL_VERSION_PATCH;
}

static inline const char *lv_version_info(void)
{
    return LVGL_VERSION_INFO;
}

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_H*/
