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

#include "src/lv_version.h"

#include "src/lv_misc/lv_log.h"
#include "src/lv_misc/lv_task.h"
#include "src/lv_misc/lv_math.h"

#include "src/lv_hal/lv_hal.h"

#include "src/lv_core/lv_obj.h"
#include "src/lv_core/lv_group.h"

#include "src/lv_core/lv_refr.h"
#include "src/lv_core/lv_disp.h"

#include "src/lv_themes/lv_theme.h"

#include "src/lv_font/lv_font.h"
#include "src/lv_font/lv_font_fmt_txt.h"

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
#include "src/lv_objx/lv_gauge.h"
#include "src/lv_objx/lv_lmeter.h"
#include "src/lv_objx/lv_sw.h"
#include "src/lv_objx/lv_kb.h"
#include "src/lv_objx/lv_arc.h"
#include "src/lv_objx/lv_preload.h"
#include "src/lv_objx/lv_calendar.h"
#include "src/lv_objx/lv_spinbox.h"

#include "src/lv_draw/lv_img_cache.h"

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

#ifdef __cplusplus
}
#endif

#endif /*LVGL_H*/
