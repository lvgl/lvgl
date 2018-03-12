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

/*Test misc. module version*/
#include "lv_misc/lv_task.h"

#include "lv_hal/lv_hal.h"

#include "lv_core/lv_obj.h"
#include "lv_core/lv_group.h"
#include "lv_core/lv_vdb.h"

#include "lv_themes/lv_theme.h"

#include "lv_objx/lv_btn.h"
#include "lv_objx/lv_img.h"
#include "lv_objx/lv_label.h"
#include "lv_objx/lv_line.h"
#include "lv_objx/lv_page.h"
#include "lv_objx/lv_cont.h"
#include "lv_objx/lv_list.h"
#include "lv_objx/lv_chart.h"
#include "lv_objx/lv_cb.h"
#include "lv_objx/lv_bar.h"
#include "lv_objx/lv_slider.h"
#include "lv_objx/lv_led.h"
#include "lv_objx/lv_btnm.h"
#include "lv_objx/lv_kb.h"
#include "lv_objx/lv_ddlist.h"
#include "lv_objx/lv_roller.h"
#include "lv_objx/lv_ta.h"
#include "lv_objx/lv_win.h"
#include "lv_objx/lv_tabview.h"
#include "lv_objx/lv_mbox.h"
#include "lv_objx/lv_gauge.h"
#include "lv_objx/lv_lmeter.h"
#include "lv_objx/lv_sw.h"
#include "lv_objx/lv_kb.h"

/*********************
 *      DEFINES
 *********************/
/*Current version of LittlevGL*/
#define LVGL_VERSION_MAJOR   5
#define LVGL_VERSION_MINOR   1
#define LVGL_VERSION_PATCH   1
#define LVGL_VERSION_INFO    "beta"

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
