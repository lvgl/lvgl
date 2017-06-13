/**
 * @file lv_app_kb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_app_kb.h"
#if USE_LV_APP_KB != 0

#include "lvgl/lv_objx/lv_btnm.h"
#include "lvgl/lv_objx/lv_ta.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_APP_KB_ANIM_TIME
#define LV_APP_KB_ANIM_TIME     300     /*ms*/
#endif

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
"\006ABC", "\003a", "\003s", "\003d", "\003f", "\003g", "\003h", "\003j", "\003k", "\003l", "\010Enter", "\n",
"_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
"\003Hide", "\003Left", "\006 ", "\003Right", "\003Ok", ""
};

static const char * kb_map_uc[] = {
"\0051#", "\004Q", "\004W", "\004E", "\004R", "\004T", "\004Y", "\004U", "\004I", "\004O", "\004P", "\007Del", "\n",
"\006abc", "\003A", "\003S", "\003D", "\003F", "\003G", "\003H", "\003J", "\003K", "\003L", "\010Enter", "\n",
"_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
"\003Hide", "\003Left", "\006 ", "\003Right", "\003Ok", ""
};

static const char * kb_map_spec[] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "\002Del", "\n",
"\002abc", "+", "-", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
"\\", "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
"\003Hide", "\003Left", "\006 ", "\003Right", "\003Ok", ""
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
static lv_style_t style_bg;
static lv_style_t style_btn_rel;
static lv_style_t style_btn_pr;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the application keyboard
 */
void lv_app_kb_init(void)
{
    lv_app_style_t * app_style = lv_app_style_get();

    memcpy(&style_bg, &app_style->menu, sizeof(lv_style_t));
    style_bg.opa = OPA_COVER;
    style_bg.hpad = 0;
    style_bg.vpad = 0;
    style_bg.opad = 0;

    memcpy(&style_btn_rel, &app_style->menu_btn_rel, sizeof(lv_style_t));
    style_btn_rel.radius = 0;
    style_btn_rel.bwidth = 1;

    memcpy(&style_btn_pr, &app_style->menu_btn_pr, sizeof(lv_style_t));
    style_btn_pr.radius = 0;
    style_btn_pr.bwidth = 1;
}

/**
 * Open a keyboard for a text area object
 * @param ta pointer to a text area object
 * @param mode 'OR'd values of 'lv_app_kb_mode_t' enum
 * @param close a function to call when the keyboard is closed
 * @param ok a function to called when the "Ok" button is pressed
 * @return the created button matrix objects
 */
lv_obj_t * lv_app_kb_open(lv_obj_t * ta, lv_app_kb_mode_t mode, void (*close)(lv_obj_t *), void (*ok)(lv_obj_t *))
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
    lv_obj_set_style(kb_btnm, &style_bg);
    lv_obj_set_size(kb_btnm, LV_HOR_RES, LV_VER_RES / 2);
    lv_obj_align(kb_btnm, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_btnm_set_action(kb_btnm, lv_app_kb_action);
    if(mode & LV_APP_KB_MODE_TXT) {
        style_btn_rel.font = font_get(LV_APP_FONT_MEDIUM);
        style_btn_pr.font = font_get(LV_APP_FONT_MEDIUM);
    	lv_btnm_set_map(kb_btnm, kb_map_lc);
    }
    else if(mode & LV_APP_KB_MODE_NUM) {
        style_btn_rel.font = font_get(LV_APP_FONT_LARGE);
        style_btn_pr.font = font_get(LV_APP_FONT_LARGE);
    	lv_btnm_set_map(kb_btnm, kb_map_num);
    }
    lv_btnm_set_styles_btn(kb_btnm, &style_btn_rel, &style_btn_pr);

    kb_win = NULL;
    kb_ta_ori_size = 0;
    if(mode & LV_APP_KB_MODE_WIN_RESIZE) {
        /*Reduce the size of the window and align it to the top*/
        kb_win = lv_app_win_get_from_obj(kb_ta);
        lv_obj_set_height(kb_win, LV_VER_RES / 2);
        lv_obj_set_y(kb_win, 0);

        /*If the text area is higher then the new size of the window reduce its size too*/
        cord_t cont_h = lv_obj_get_height(kb_win) - lv_obj_get_height(lv_win_get_header(kb_win));
        kb_ta_ori_size = lv_obj_get_height(kb_ta);
        if(lv_obj_get_height(kb_ta)  > cont_h - LV_DPI / 10) {
            lv_obj_set_height(kb_ta, cont_h - LV_DPI / 10);
        }
        lv_page_focus(lv_win_get_page(kb_win), kb_ta, 0);
    }

    lv_ta_set_cursor_pos(kb_ta, LV_TA_CUR_LAST);
    if(kb_mode & LV_APP_KB_MODE_CURSOR_MANAGE) {
        lv_ta_set_cursor_show(kb_ta, true);
    }

    if(kb_mode & LV_APP_KB_MODE_ANIM) {
        lv_obj_anim(kb_btnm, LV_ANIM_FLOAT_BOTTOM | ANIM_IN, LV_APP_KB_ANIM_TIME, 0, NULL);
    }


    return kb_btnm;

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
	if((kb_mode & LV_APP_KB_MODE_WIN_RESIZE) && kb_win != NULL) {
        lv_obj_set_height(kb_ta, kb_ta_ori_size);
        lv_obj_set_size(kb_win, LV_HOR_RES, LV_VER_RES);
        kb_win = NULL;
	}
    
    if(kb_mode & LV_APP_KB_MODE_CURSOR_MANAGE) {
        lv_ta_set_cursor_show(kb_ta, false);
    }

    if(kb_mode & LV_APP_KB_MODE_ANIM) {
        lv_obj_anim(kb_btnm, LV_ANIM_FLOAT_BOTTOM | ANIM_OUT, LV_APP_KB_ANIM_TIME, 0, lv_obj_del);
    } else {
        lv_obj_del(kb_btnm);
    }
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

    if(kb_mode & LV_APP_KB_MODE_WIN_RESIZE) {
#if LV_APP_ANIM_LEVEL != 0
        lv_page_focus(lv_win_get_content(kb_win), kb_ta, true);
#else
        lv_page_focus(lv_win_get_page(kb_win), kb_ta, 0);
#endif
    }
    return LV_ACTION_RES_OK;
}

#endif /*LV_APP_ENABLE != 0*/
