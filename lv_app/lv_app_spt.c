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
static lv_action_res_t lv_app_kb_action(lv_obj_t * btnm, uint16_t i);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * kb_btnm;
static lv_obj_t * kb_win;
static lv_obj_t * kb_ta;
static const char * kb_map_lc[] = {
"\0021#", "\002q", "\002w", "\002e", "\002r", "\002t", "\002y", "\002u", "\002i", "\002o", "\002p", "\003Del", "\n",
"\002ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", "\003Enter", "\n",
"*", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ";", "\n",
"\002Free", "\002<", "\006 ", "\002>", "\002Free", ""
};
static lv_btnms_t kb_btnms;
static bool kb_inited;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_app_kb_open(lv_obj_t * ta)
{
	if(kb_inited == false) {
		lv_btnms_get(LV_BTNMS_DEF, &kb_btnms);
		kb_btnms.rects.opad = 4 + LV_STYLE_MULT;
		kb_btnms.rects.vpad = 3 + LV_STYLE_MULT;
		kb_btnms.rects.hpad = 3 + LV_STYLE_MULT;
		kb_inited = true;
	}

    if(kb_btnm != NULL) {
        lv_app_kb_close();
    }

    kb_ta = ta;

    kb_btnm = lv_btnm_create(lv_scr_act(), NULL);
    lv_obj_set_size(kb_btnm, LV_HOR_RES, LV_VER_RES / 2);
    lv_obj_align(kb_btnm, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_obj_set_style(kb_btnm, &kb_btnms);
    lv_btnm_set_map(kb_btnm, kb_map_lc);
    lv_btnm_set_cb(kb_btnm, lv_app_kb_action);

    kb_win = lv_app_get_win_from_obj(kb_ta);
    lv_obj_set_height(kb_win, LV_VER_RES / 2);
    lv_obj_set_y(kb_win, 0);

  //  lv_obj_set_height(kb_ta, LV_VER_RES / 2 - 50);

    lv_ta_add_text(kb_ta, "alma\n\nkorte\n\ncitrom\nbarack\nbanaaaan\n\n");
    lv_ta_set_cursor_pos(kb_ta, LV_TA_CUR_LAST);

    lv_page_focus(lv_win_get_content(kb_win), kb_ta, true);
}

void lv_app_kb_close(void)
{
	if(kb_btnm == NULL) return;

	lv_obj_set_size(kb_win, LV_HOR_RES, LV_VER_RES);
	kb_win = NULL;

    lv_obj_del(kb_btnm);
    kb_btnm = NULL;

    kb_ta = NULL;
}

static lv_action_res_t lv_app_kb_action(lv_obj_t * btnm, uint16_t i)
{
	lv_ta_add_text(kb_ta, kb_map_lc[i]);
	lv_page_focus(lv_win_get_content(kb_win), kb_ta, true);
	return LV_ACTION_RES_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
