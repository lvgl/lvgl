/**
 * @file lv_app_spt.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_spt.h"
#include "../lv_objx/lv_btnm.h"
#include "../lv_objx/lv_ta.h"

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
static lv_obj_t * kb_btnm;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_app_kb_open(lv_obj_t * ta)
{
    if(kb_btnm != NULL) {
        lv_app_kb_close();
    }

    kb_btnm = lv_btnm_create(lv_scr_act(), NULL);
    lv_obj_set_size(kb_btnm, LV_HOR_RES, LV_VER_RES / 2);
    lv_obj_align(kb_btnm, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    lv_ta_add_text(ta, "alma\n\nkorte\n\ncitrom\nbarack\nbanaaaan\n\n");
    lv_ta_set_cursor_pos(ta, LV_TA_CUR_LAST);

    lv_obj_t * win = lv_app_get_win_from_obj(ta);
    lv_obj_set_height(win, LV_VER_RES / 2);
    lv_obj_set_y(win, 0);

    lv_page_focus(lv_win_get_content(win), ta, true);
}

void lv_app_kb_close(void)
{
    lv_obj_del(kb_btnm);
    kb_btnm = NULL;

}

/**********************
 *   STATIC FUNCTIONS
 **********************/
