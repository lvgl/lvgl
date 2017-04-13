/**
 * @file lv_app_notice.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_notice.h"
#if LV_APP_ENABLE != 0

#include "lvgl/lv_objx/lv_rect.h"
#include "lvgl/lv_objx/lv_label.h"

#include "lvgl/lv_misc/anim.h"
#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
/*Add the required configurations*/
#ifndef LV_APP_NOTICE_SHOW_TIME
#define LV_APP_NOTICE_SHOW_TIME   4000
#endif

#ifndef LV_APP_NOTICE_MAX_NUM
#define LV_APP_NOTICE_MAX_NUM   6
#endif

#ifndef LV_APP_NOTICE_MAX_LEN
#define LV_APP_NOTICE_MAX_LEN   256
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * notice_h;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * Initialize the Notifications
 */
void lv_app_notice_init(void)
{
    notice_h = lv_rect_create(lv_scr_act(), NULL);
    lv_obj_set_size(notice_h, LV_HOR_RES, LV_VER_RES - LV_DPI);
    lv_obj_set_y(notice_h, LV_DPI);
    lv_obj_set_click(notice_h, false);
    lv_obj_set_style(notice_h, lv_style_get(LV_STYLE_TRANSP, NULL));
    lv_rect_set_layout(notice_h, LV_RECT_LAYOUT_COL_R);
}

/**
 * Add a notification with a given text
 * @param format pritntf-like format string
 * @return pointer the notice which is a message box (lv_mbox) object
 */
lv_obj_t *  lv_app_notice_add(const char * format, ...)
{
    char txt[LV_APP_NOTICE_MAX_LEN];

    va_list va;
    va_start(va, format);
    vsprintf(txt,format, va);
    va_end(va);

    lv_obj_t * mbox;
    mbox = lv_mbox_create(notice_h, NULL);
   // lv_obj_set_style(mbox, lv_mboxs_get(LV_MBOXS_INFO, NULL));
    lv_mbox_set_text(mbox, txt);

#if LV_APP_NOTICE_SHOW_TIME != 0
    lv_mbox_start_auto_close(mbox, LV_APP_NOTICE_SHOW_TIME);
#endif

    /*Delete the last children if there are too many*/
    uint32_t child_num = lv_obj_get_child_num(notice_h);
    if(child_num > LV_APP_NOTICE_MAX_NUM) {
        lv_obj_t * last_child = ll_get_tail(&notice_h->child_ll);
        lv_obj_del(last_child);
    }

    /*make sure the notices are on the top*/
    lv_obj_set_parent(notice_h, lv_scr_act());

    return mbox;

}
/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
