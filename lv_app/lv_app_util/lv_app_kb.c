/**
 * @file lv_app_kb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_kb.h"
#if LV_APP_ENABLE != 0

#include "lvgl/lv_objx/lv_btnm.h"
#include "lvgl/lv_objx/lv_ta.h"

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
"\0051#", "\004q", "\004w", "\004e", "\004r", "\004t", "\004y", "\004u", "\004i", "\004o", "\004p", "\007Del", "\n",
"\007ABC", "\004a", "\004s", "\004d", "\004f", "\004g", "\004h", "\004j", "\004k", "\004l", "\010Enter", "\n",
"*", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ";", "\n",
"\002Hide", "\002Left", "\006 ", "\002Right", "\002Ok", ""
};

static const char * kb_map_uc[] = {
"\0051#", "\004Q", "\004W", "\004E", "\004R", "\004T", "\004Y", "\004U", "\004I", "\004O", "\004P", "\007Del", "\n",
"\007abc", "\004A", "\004S", "\004D", "\004F", "\004G", "\004H", "\004J", "\004K", "\004L", "\010Enter", "\n",
"*", "/", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ";", "\n",
"\002Hide", "\002Left", "\006 ", "\002Right", "\002Ok", ""
};

static const char * kb_map_spec[] = {
"0", "1", "2", "3", "4", "5", "6", "4", "8", "9", "\002Del", "\n",
"\002abc", "+", "-", "=", "%", "!", "?", "#", "<", ">", "\002Enter", "\n",
"\\", "@", "$", "_", "(", ")", "{", "}", "[", "]", ":", "\"", "'", "\n",
"\002Hide", "\002Left", "\006 ", "\002Right", "\002Ok", ""
};

static const char * kb_map_num[] = {
"1", "2", "3", "\002Hide","\n",
"4", "5", "6", "\002Ok", "\n",
"7", "8", "9", "\002Del", "\n",
"+/-", "0", ".", "Left", "Right", ""
};

static cord_t kb_ta_ori_size;
static uint8_t kb_mode;
static void (*kb_close_action)(lv_obj_t *);
static void (*kb_ok_action)(lv_obj_t *);
static lv_btnms_t kb_btnms;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_app_kb_init(void)
{
    lv_btnms_get(LV_BTNMS_DEF, &kb_btnms);
    kb_btnms.rects.opad = 4 + LV_DOWNSCALE;
    kb_btnms.rects.vpad = 3 + LV_DOWNSCALE;
    kb_btnms.rects.hpad = 3 + LV_DOWNSCALE;
    kb_btnms.rects.round = 0;
}

/**
 * Open a keyboard for a text area object
 * @param ta pointer to a text area object
 * @param mode 'OR'd values of 'lv_app_kb_mode_t' enum
 * @param close a function to call when the keyboard is closed
 * @param ok a function to called when the "Ok" button is pressed
 */
void lv_app_kb_open(lv_obj_t * ta, lv_app_kb_mode_t mode, void (*close)(lv_obj_t *), void (*ok)(lv_obj_t *))
{
	/*Close the previous keyboard*/
    if(kb_btnm != NULL) {
        lv_app_kb_close(false);
    }

    /*Save some parameters*/
    kb_ta = ta;
    kb_mode = mode;
    kb_close_action = close;
    kb_ok_action = ok;

    /*Create a button matrix for the keyboard  */
    kb_btnm = lv_btnm_create(lv_scr_act(), NULL);
    lv_obj_set_size(kb_btnm, LV_HOR_RES, LV_VER_RES / 2);
    lv_obj_align(kb_btnm, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_btnm_set_cb(kb_btnm, lv_app_kb_action);
    if(mode & LV_APP_KB_MODE_TXT) {
		kb_btnms.labels.font = LV_APP_FONT_MEDIUM;
    	lv_btnm_set_map(kb_btnm, kb_map_lc);
    }
    else if(mode & LV_APP_KB_MODE_NUM) {
		kb_btnms.labels.font = LV_APP_FONT_LARGE;
    	lv_btnm_set_map(kb_btnm, kb_map_num);
    }
    lv_obj_set_style(kb_btnm, &kb_btnms);

    /*Reduce teh size of the window and align it to the top*/
    kb_win = lv_app_win_get_from_obj(kb_ta);
    lv_obj_set_height(kb_win, LV_VER_RES / 2);
    lv_obj_set_y(kb_win, 0);

    /*If the text area is higher then the new size of the window redus its size too*/
	lv_app_style_t * app_style = lv_app_style_get();
    cord_t win_cont_h = lv_obj_get_height(lv_win_get_content(kb_win)) -  2 * app_style->win_style.content.scrl_rects.vpad;
	kb_ta_ori_size = lv_obj_get_height(kb_ta);
    if(lv_obj_get_height(kb_ta)  > win_cont_h) {
    	lv_obj_set_height(kb_ta, win_cont_h);
    }

    lv_ta_set_cursor_pos(kb_ta, LV_TA_CUR_LAST);

#if LV_APP_ANIM_LEVEL != 0
    lv_page_focus(lv_win_get_content(kb_win), kb_ta, true);
#else
    lv_page_focus(lv_win_get_content(kb_win), kb_ta, false);
#endif
}

/**
 * Close the keyboard
 * @param ok true: call the ok function, false: call the close function
 */
void lv_app_kb_close(bool ok)
{
	if(kb_btnm == NULL) return;

	if(ok == false) {
		if(kb_close_action != NULL) kb_close_action(kb_ta);
	} else {
		if(kb_ok_action != NULL) kb_ok_action(kb_ta);
	}

	/*Reset the modified sizes*/

	lv_obj_set_height(kb_ta, kb_ta_ori_size);

	lv_obj_set_size(kb_win, LV_HOR_RES, LV_VER_RES);
	kb_win = NULL;

    lv_obj_del(kb_btnm);
    kb_btnm = NULL;

    kb_ta = NULL;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Called when a button of 'kb_btnm' is released
 * @param btnm pointer to 'kb_btnm'
 * @param i the index of the released button from the current btnm map
 * @return LV_ACTION_RES_INV if the btnm is deleted else LV_ACTION_RES_OK
 */
static lv_action_res_t lv_app_kb_action(lv_obj_t * btnm, uint16_t i)
{
    const char ** map = lv_btnm_get_map(btnm);
    const char * txt = map[i];

    /*Ignore the unit size number of the text*/
    if(txt[0] <= '\011') txt++;

    /*Do the corresponding action according to the text of the button*/
    if(strcmp(txt, "abc") == 0) {
        lv_btnm_set_map(btnm, kb_map_lc);
    } else if(strcmp(txt, "ABC") == 0) {
        lv_btnm_set_map(btnm, kb_map_uc);
    } else if(strcmp(txt, "1#") == 0) {
        lv_btnm_set_map(btnm, kb_map_spec);
    }  else if(strcmp(txt, "Enter") == 0) {
        lv_ta_add_char(kb_ta, '\n');
    } else if(strcmp(txt, "Left") == 0) {
        lv_ta_cursor_left(kb_ta);
    } else if(strcmp(txt, "Right") == 0) {
        lv_ta_cursor_right(kb_ta);
    } else if(strcmp(txt, "Del") == 0) {
        lv_ta_del(kb_ta);
    } else if(strcmp(txt, "+/-") == 0) {
        uint16_t cur = lv_ta_get_cursor_pos(kb_ta);
        const char * ta_txt = lv_ta_get_txt(kb_ta);
        if(ta_txt[0] == '-') {
            lv_ta_set_cursor_pos(kb_ta, 1);
            lv_ta_del(kb_ta);
            lv_ta_add_char(kb_ta, '+');
            lv_ta_set_cursor_pos(kb_ta, cur);
        } else if(ta_txt[0] == '+') {
            lv_ta_set_cursor_pos(kb_ta, 1);
            lv_ta_del(kb_ta);
            lv_ta_add_char(kb_ta, '-');
            lv_ta_set_cursor_pos(kb_ta, cur);
        } else {
            lv_ta_set_cursor_pos(kb_ta, 0);
            lv_ta_add_char(kb_ta, '-');
            lv_ta_set_cursor_pos(kb_ta, cur + 1);
        }
    } else if(strcmp(txt, "Hide") == 0) {
        lv_app_kb_close(false);
        return LV_ACTION_RES_INV;
    } else if(strcmp(txt, "Ok") == 0) {
        lv_app_kb_close(true);
        return LV_ACTION_RES_INV;
    } else {
        lv_ta_add_text(kb_ta, txt);
    }

#if LV_APP_ANIM_LEVEL != 0
    lv_page_focus(lv_win_get_content(kb_win), kb_ta, true);
#else
    lv_page_focus(lv_win_get_content(kb_win), kb_ta, false);
#endif
    return LV_ACTION_RES_OK;
}

#endif /*LV_APP_ENABLE != 0*/
