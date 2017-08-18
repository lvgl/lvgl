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
#include "misc/misc.h"
#include "lv_obj/lv_obj.h"
#include "lv_obj/lv_group.h"
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
#include "lv_objx/lv_ddlist.h"
#include "lv_objx/lv_ta.h"
#include "lv_objx/lv_win.h"
#include "lv_objx/lv_mbox.h"
#include "lv_objx/lv_gauge.h"
#include "lv_objx/lv_lmeter.h"

#include "lv_app/lv_app.h"

/*********************
 *      DEFINES
 *********************/
/*Current version of LittlevGL*/
#define LVGL_VERSION_MAJOR  4
#define LVGL_VERSION_MINOR  2
#define LVGL_VERSION_PATH   0

/*Required misc. library version*/
#define LV_MISC_REQ_MAJOR   4
#define LV_MISC_REQ_MINOR   2
#define LV_MISC_REQ_PATCH   0

#if MISC_VERSION_MAJOR != LV_MISC_REQ_MAJOR /*The version major has to match*/
#error "LV: incompatible misc. module version! See lvgl.h"
#endif

#if MISC_VERSION_MINOR < LV_MISC_REQ_MINOR /*The version minor has to be the same or greater*/
#error "LV: incompatible misc. module version! See lvgl.h"
#endif

#if MISC_VERSION_PATCH < LV_MISC_REQ_PATCH /*The version patch has to be the same or greater*/
#error "LV: incompatible misc. module version! See lvgl.h"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif
