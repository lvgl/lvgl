/**
 * @file lvgl.h
 * Include all LittleV GL related headers
 */

#ifndef LVGL_H
#define LVGL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "src/lv_misc/lv_log.h"
#include "src/lv_misc/lv_task.h"
#include "src/lv_misc/lv_math.h"
#include "src/lv_misc/lv_async.h"

#include "src/lv_hal/lv_hal.h"

#include "src/lv_core/lv_obj.h"
#include "src/lv_core/lv_group.h"
#include "src/lv_core/lv_indev.h"

#include "src/lv_core/lv_refr.h"
#include "src/lv_core/lv_disp.h"
#include "src/lv_core/lv_debug.h"

#include "src/lv_themes/lv_theme.h"

#include "src/lv_font/lv_font.h"
#include "src/lv_font/lv_font_fmt_txt.h"
#include "src/lv_misc/lv_bidi.h"
#include "src/lv_misc/lv_printf.h"

#include "src/lv_objx/lv_btn.h"
#include "src/lv_objx/lv_imgbtn.h"
#include "src/lv_objx/lv_img.h"
#include "src/lv_objx/lv_label.h"
#include "src/lv_objx/lv_line.h"
#include "src/lv_objx/lv_page.h"
#include "src/lv_objx/lv_cont.h"
#include "src/lv_objx/lv_list.h"
#include "src/lv_objx/lv_chart.h"
#include "src/lv_objx/lv_table.h"
#include "src/lv_objx/lv_cb.h"
#include "src/lv_objx/lv_cpicker.h"
#include "src/lv_objx/lv_bar.h"
#include "src/lv_objx/lv_slider.h"
#include "src/lv_objx/lv_led.h"
#include "src/lv_objx/lv_btnm.h"
#include "src/lv_objx/lv_kb.h"
#include "src/lv_objx/lv_ddlist.h"
#include "src/lv_objx/lv_roller.h"
#include "src/lv_objx/lv_ta.h"
#include "src/lv_objx/lv_canvas.h"
#include "src/lv_objx/lv_win.h"
#include "src/lv_objx/lv_tabview.h"
#include "src/lv_objx/lv_tileview.h"
#include "src/lv_objx/lv_mbox.h"
#include "src/lv_objx/lv_objmask.h"
#include "src/lv_objx/lv_gauge.h"
#include "src/lv_objx/lv_linemeter.h"
#include "src/lv_objx/lv_sw.h"
#include "src/lv_objx/lv_kb.h"
#include "src/lv_objx/lv_arc.h"
#include "src/lv_objx/lv_preload.h"
#include "src/lv_objx/lv_calendar.h"
#include "src/lv_objx/lv_spinbox.h"

#include "src/lv_draw/lv_img_cache.h"

#include "src/lv_api_map.h"

/*********************
 *      DEFINES
 *********************/
/*Current version of LittlevGL*/
#define LVGL_VERSION_MAJOR   7
#define LVGL_VERSION_MINOR   0
#define LVGL_VERSION_PATCH   0
#define LVGL_VERSION_INFO    "dev"

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
 * */
#define LV_VERSION_CHECK(x,y,z) (x == LVGL_VERSION_MAJOR && (y < LVGL_VERSION_MINOR || (y == LVGL_VERSION_MINOR && z <= LVGL_VERSION_PATCH)))


#ifdef __cplusplus
}
#endif

#endif /*LVGL_H*/
