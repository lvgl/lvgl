/**
 * @file lv_ta.c
 * 
 */


/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_TA != 0

#include "lv_ta.h"
#include "../lv_obj/lv_group.h"
#include "../lv_draw/lv_draw.h"
#include "misc/gfx/anim.h"
#include "misc/gfx/text.h"
#include "misc/math/math_base.h"

/*********************
 *      DEFINES
 *********************/
/*Test configuration*/
#ifndef LV_TA_MAX_LENGTH
#define LV_TA_MAX_LENGTH    256
#endif

#ifndef LV_TA_CUR_BLINK_TIME
#define LV_TA_CUR_BLINK_TIME 400    /*ms*/
#endif

#ifndef LV_TA_PWD_SHOW_TIME
#define LV_TA_PWD_SHOW_TIME 1500    /*ms*/
#endif

#define LV_TA_DEF_WIDTH     (2 * LV_DPI)
#define LV_TA_DEF_HEIGHT    (1 * LV_DPI)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_ta_design(lv_obj_t * ta, const area_t * mask, lv_design_mode_t mode);
static bool lv_ta_scrling_design(lv_obj_t * scrl, const area_t * mask, lv_design_mode_t mode);
static void cursor_blink_anim(lv_obj_t * ta, uint8_t show);
static void pwd_char_hider_anim(lv_obj_t * ta, int32_t x);
static void pwd_char_hider(lv_obj_t * ta);
static void lv_ta_save_valid_cursor_x(lv_obj_t * ta);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_design_f_t ancestor_design_f;
lv_design_f_t scrl_design_f;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*----------------- 
 * Create function
 *-----------------*/

/**
 * Create a text area objects
 * @param par pointer to an object, it will be the parent of the new text area
 * @param copy pointer to a text area object, if not NULL then the new object will be copied from it
 * @return pointer to the created text area
 */
lv_obj_t * lv_ta_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor object*/
    lv_obj_t * new_ta = lv_page_create(par, copy);
    dm_assert(new_ta);
    
    /*Allocate the object type specific extended data*/
    lv_ta_ext_t * ext = lv_obj_alloc_ext(new_ta, sizeof(lv_ta_ext_t));
    dm_assert(ext);
    ext->cursor_show = 1;
    ext->cursor_state = 0;
    ext->pwd_mode = 0;
    ext->pwd_tmp = NULL;
    ext->cursor_style = NULL;
    ext->cursor_pos = 0;
    ext->cursor_type = LV_TA_CURSOR_LINE;
    ext->cursor_valid_x = 0;
    ext->label = NULL;

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_ta);
    if(scrl_design_f == NULL) scrl_design_f = lv_obj_get_design_f(ext->page.scrl);

    lv_obj_set_signal_f(new_ta, lv_ta_signal);
    lv_obj_set_signal_f(lv_page_get_scrl(new_ta), lv_ta_scrl_signal);
    lv_obj_set_design_f(new_ta, lv_ta_design);

    /*Init the new text area object*/
    if(copy == NULL) {
    	ext->label = lv_label_create(new_ta, NULL);

    	lv_obj_set_design_f(ext->page.scrl, lv_ta_scrling_design);
    	lv_label_set_long_mode(ext->label, LV_LABEL_LONG_BREAK);
    	lv_label_set_text(ext->label, "Text.area");
    	lv_page_glue_obj(ext->label, true);
    	lv_obj_set_click(ext->label, false);
    	lv_obj_set_style(new_ta, lv_style_get(LV_STYLE_PRETTY, NULL));
        lv_page_set_sb_mode(new_ta, LV_PAGE_SB_MODE_AUTO);
        lv_obj_set_style(lv_page_get_scrl(new_ta), lv_style_get(LV_STYLE_TRANSP_TIGHT, NULL));
    	lv_obj_set_size(new_ta, LV_TA_DEF_WIDTH, LV_TA_DEF_HEIGHT);
    }
    /*Copy an existing object*/
    else {
    	lv_obj_set_design_f(ext->page.scrl, lv_ta_scrling_design);
    	lv_ta_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->label = lv_label_create(new_ta, copy_ext->label);
        ext->cursor_show = copy_ext->cursor_show;
        ext->pwd_mode = copy_ext->pwd_mode;
    	lv_page_glue_obj(ext->label, true);
    	if(copy_ext->one_line) lv_ta_set_one_line(new_ta, true);

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_ta);
    }
    
    /*Create a cursor blinker animation*/
    anim_t a;
    a.var = new_ta;
    a.fp = (anim_fp_t)cursor_blink_anim;
    a.time = LV_TA_CUR_BLINK_TIME;
    a.act_time = 0;
    a.end_cb = NULL;
    a.start = 1;
    a.end = 0;
    a.repeat = 1;
    a.repeat_pause = 0;
    a.playback = 1;
    a.playback_pause = 0;
    a.path = anim_get_path(ANIM_PATH_STEP);
    anim_create(&a);

    return new_ta;
}

/**
 * Signal function of the text area
 * @param ta pointer to a text area object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_ta_signal(lv_obj_t * ta, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_page_signal(ta, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    	if(sign == LV_SIGNAL_CLEANUP) {
    	    if(ext->pwd_tmp != NULL) dm_free(ext->pwd_tmp);

            /* (The created label will be deleted automatically) */
    	} else if(sign == LV_SIGNAL_STYLE_CHG) {
            if(ext->label) {
            	lv_obj_t * scrl = lv_page_get_scrl(ta);
            	lv_style_t * style_scrl = lv_obj_get_style(scrl);
                lv_obj_set_width(ext->label, lv_obj_get_width(scrl) - 2 * style_scrl->hpad);
                lv_obj_set_pos(ext->label, style_scrl->hpad, style_scrl->vpad);
                lv_label_set_text(ext->label, NULL);

                lv_obj_refr_ext_size(lv_page_get_scrl(ta));
            }
    	} else if(sign == LV_SIGNAL_CORD_CHG) {
    		/*Set the label width according to the text area width*/
            if(ext->label) {
                if(lv_obj_get_width(ta) != area_get_width(param) ||
                  lv_obj_get_height(ta) != area_get_height(param)) {
                	lv_obj_t * scrl = lv_page_get_scrl(ta);
                	lv_style_t * style_scrl = lv_obj_get_style(scrl);
                    lv_obj_set_width(ext->label, lv_obj_get_width(scrl) - 2 * style_scrl->hpad);
                    lv_obj_set_pos(ext->label, style_scrl->hpad, style_scrl->vpad);
                    lv_label_set_text(ext->label, NULL);    /*Refresh the label*/
                }
            }
    	}
        else if (sign == LV_SIGNAL_CONTROLL) {
            char c = *((char*)param);
            if(c == LV_GROUP_KEY_RIGHT) {
                lv_ta_cursor_right(ta);
            } else if(c == LV_GROUP_KEY_LEFT) {
                lv_ta_cursor_left(ta);
            } else if(c == LV_GROUP_KEY_UP) {
                lv_ta_cursor_up(ta);
            } else if(c == LV_GROUP_KEY_DOWN) {
                lv_ta_cursor_down(ta);
            }
        }
    }
    return valid;
}

/**
 * Signal function of the scrollable part of the text area
 * @param scrl pointer to scrollable part of a text area object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_ta_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_page_scrl_signal(scrl, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        if(sign == LV_SIGNAL_REFR_EXT_SIZE) {
            /*Set ext. size because the cursor might be out of this object*/
            lv_obj_t * ta = lv_obj_get_parent(scrl);
            lv_ta_ext_t * ext = lv_obj_get_ext(ta);
            lv_style_t * style_label = lv_obj_get_style(ext->label);

            scrl->ext_size = MATH_MAX(scrl->ext_size, style_label->line_space + font_get_height(style_label->font));
        }
    }
    return valid;
}
/*=====================
 * Setter functions
 *====================*/

/**
 * Insert a character to the current cursor position
 * @param ta pointer to a text area object
 * @param c a character (could but UTF-8 code as well: 'Á' or txt_unicode_to_utf8(0x047C)
 */
void lv_ta_add_char(lv_obj_t * ta, uint32_t c)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);

    if(ext->pwd_mode != 0) pwd_char_hider(ta);  /*Make sure all the current text contains only '*'*/
#if TXT_UTF8 == 0
    char letter_buf[2];
    letter_buf[0] = c;
    letter_buf[1] = '\0';
#else
    /*Swap because of UTF-8 is "big-endian-like" */
    if(((c >> 8) & 0b11100000) == 0b11000000) {
        c = (c & 0xFF) << 8 | ((c >> 8) & 0xFF);
    }
    if(((c >> 16) & 0b11110000) == 0b11100000) {
        c = (c & 0xFF) << 16 | ((c >> 16) & 0xFF);
    }
    if(((c >> 24) & 0b11111000) == 0b11110000) {
        c = ((c & 0xFF) << 24) | (((c >> 8) & 0xFF) >> 16) | (((c >> 16) & 0xFF) >> 8) | ((c >> 24) & 0xFF);
    }

    char letter_buf[8] = {0};
    memcpy(letter_buf, &c, txt_utf8_size(c));
#endif
    lv_label_ins_text(ext->label, ext->cursor_pos, letter_buf);    /*Insert the character*/

    if(ext->pwd_mode != 0) {

        ext->pwd_tmp = dm_realloc(ext->pwd_tmp, strlen(ext->pwd_tmp) + 2);  /*+2: the new char + \0 */
        dm_assert(ext->pwd_tmp);
        txt_ins(ext->pwd_tmp, ext->cursor_pos, letter_buf);

        anim_t a;
        a.var = ta;
        a.fp = (anim_fp_t)pwd_char_hider_anim;
        a.time = LV_TA_PWD_SHOW_TIME;
        a.act_time = 0;
        a.end_cb = (anim_cb_t)pwd_char_hider;
        a.start = 0;
        a.end = 1;
        a.repeat = 0;
        a.repeat_pause = 0;
        a.playback = 0;
        a.playback_pause = 0;
        a.path = anim_get_path(ANIM_PATH_STEP);
        anim_create(&a);
    }

    /*Move the cursor after the new character*/
    lv_ta_set_cursor_pos(ta, lv_ta_get_cursor_pos(ta) + 1);

    /*It is a valid x step so save it*/
    lv_ta_save_valid_cursor_x(ta);

}

/**
 * Insert a text to the current cursor position
 * @param ta pointer to a text area object
 * @param txt a '\0' terminated string to insert
 */
void lv_ta_add_text(lv_obj_t * ta, const char * txt)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    uint16_t txt_len = strlen(txt);

    if(ext->pwd_mode != 0) pwd_char_hider(ta);  /*Make sure all the current text contains only '*'*/
    /*Insert the text*/

    lv_label_ins_text(ext->label, ext->cursor_pos, txt);

    if(ext->pwd_mode != 0) {
        ext->pwd_tmp = dm_realloc(ext->pwd_tmp, strlen(ext->pwd_tmp) + txt_len + 1);
        dm_assert(ext->pwd_tmp);

        txt_ins(ext->pwd_tmp, ext->cursor_pos, txt);

        anim_t a;
        a.var = ta;
        a.fp = (anim_fp_t)pwd_char_hider_anim;
        a.time = LV_TA_PWD_SHOW_TIME;
        a.act_time = 0;
        a.end_cb = (anim_cb_t)pwd_char_hider;
        a.start = 0;
        a.end = 1;
        a.repeat = 0;
        a.repeat_pause = 0;
        a.playback = 0;
        a.playback_pause = 0;
        a.path = anim_get_path(ANIM_PATH_STEP);
        anim_create(&a);
    }

    /*Move the cursor after the new text*/
    lv_ta_set_cursor_pos(ta, lv_ta_get_cursor_pos(ta) + txt_len);

    /*It is a valid x step so save it*/
    lv_ta_save_valid_cursor_x(ta);
}

/**
 * Set the text of a text area
 * @param ta pointer to a text area
 * @param txt pointer to the text
 */
void lv_ta_set_text(lv_obj_t * ta, const char * txt)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	lv_label_set_text(ext->label, txt);
	lv_ta_set_cursor_pos(ta, LV_TA_CUR_LAST);

	/*Don't let 'width == 0' because cursor will not be visible*/
	if(lv_obj_get_width(ext->label) == 0) {
	    lv_style_t * style = lv_obj_get_style(ext->label);
	    lv_obj_set_width(ext->label, font_get_width(style->font, ' '));
	}

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(ta);

    if(ext->pwd_mode != 0) {
        ext->pwd_tmp = dm_realloc(ext->pwd_tmp, strlen(txt) + 1);
        strcpy(ext->pwd_tmp, txt);

        anim_t a;
        a.var = ta;
        a.fp = (anim_fp_t)pwd_char_hider_anim;
        a.time = LV_TA_PWD_SHOW_TIME;
        a.act_time = 0;
        a.end_cb = (anim_cb_t)pwd_char_hider;
        a.start = 0;
        a.end = 1;
        a.repeat = 0;
        a.repeat_pause = 0;
        a.playback = 0;
        a.playback_pause = 0;
        a.path = anim_get_path(ANIM_PATH_STEP);
        anim_create(&a);
    }
}

/**
 * Delete a the left character from the current cursor position
 * @param ta pointer to a text area object
 */
void lv_ta_del(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	uint16_t cur_pos = ext->cursor_pos;

	if(cur_pos == 0) return;

    char * label_txt = lv_label_get_text(ext->label);
	/*Delete a character*/
#if TXT_UTF8 == 0
    txt_cut(label_txt, ext->cursor_pos - 1, 1);
#else
    uint32_t byte_pos = txt_utf8_get_id(label_txt, ext->cursor_pos - 1);
    txt_cut(label_txt, ext->cursor_pos - 1, txt_utf8_size(label_txt[byte_pos]));
#endif
	/*Refresh the label*/
	lv_label_set_text(ext->label, label_txt);

	/*Don't let 'width == 0' because cursor will not be visible*/
    if(lv_obj_get_width(ext->label) == 0) {
        lv_style_t * style = lv_obj_get_style(ext->label);
        lv_obj_set_width(ext->label, style->line_width);
    }

    if(ext->pwd_mode != 0) {
#if TXT_UTF8 == 0
        txt_cut(ext->pwd_tmp, ext->cursor_pos - 1, 1);
#else
        uint32_t byte_pos = txt_utf8_get_id(ext->pwd_tmp, ext->cursor_pos - 1);
        txt_cut(ext->pwd_tmp, ext->cursor_pos - 1, txt_utf8_size(label_txt[byte_pos]));
#endif
        ext->pwd_tmp = dm_realloc(ext->pwd_tmp, strlen(ext->pwd_tmp) + 1);
        dm_assert(ext->pwd_tmp);
    }

	/*Move the cursor to the place of the deleted character*/
	lv_ta_set_cursor_pos(ta, ext->cursor_pos - 1);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(ta);
}


/**
 * Set the cursor position
 * @param obj pointer to a text area object
 * @param pos the new cursor position in character index
 *             < 0 : index from the end of the text
 *             LV_TA_CUR_LAST: go after the last character
 */
void lv_ta_set_cursor_pos(lv_obj_t * ta, int16_t pos)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    lv_obj_t * scrl = lv_page_get_scrl(ta);
    lv_style_t * style_scrl = lv_obj_get_style(scrl);
	uint16_t len = txt_len(lv_label_get_text(ext->label));

	if(pos < 0) pos = len + pos;

	if(pos > len || pos == LV_TA_CUR_LAST) pos = len;

	ext->cursor_pos = pos;

	/*Position the label to make the cursor visible*/
	lv_obj_t * label_par = lv_obj_get_parent(ext->label);
	point_t cur_pos;
	lv_style_t * style = lv_obj_get_style(ta);
	const font_t * font_p = style->font;
	area_t label_cords;
    area_t ta_cords;
	lv_label_get_letter_pos(ext->label, pos, &cur_pos);
	lv_obj_get_cords(ta, &ta_cords);
    lv_obj_get_cords(ext->label, &label_cords);

	/*Check the top*/
	if(lv_obj_get_y(label_par) + cur_pos.y < 0) {
		lv_obj_set_y(label_par, - cur_pos.y);
	}

	/*Check the bottom*/
	cord_t font_h = font_get_height(font_p) >> FONT_ANTIALIAS;
	if(label_cords.y1 + cur_pos.y + font_h + style_scrl->vpad > ta_cords.y2) {
		lv_obj_set_y(label_par, -(cur_pos.y - lv_obj_get_height(ta) +
				                     font_h + 2 * style_scrl->vpad));
	}
	/*Check the left (use the font_h as general unit)*/
    if(lv_obj_get_x(label_par) + cur_pos.x < font_h) {
        lv_obj_set_x(label_par, - cur_pos.x + font_h);
    }

    /*Check the right (use the font_h as general unit)*/
    if(label_cords.x1 + cur_pos.x + font_h + style_scrl->hpad > ta_cords.x2) {
        lv_obj_set_x(label_par, -(cur_pos.x - lv_obj_get_width(ta) +
                                     font_h + 2 * style_scrl->hpad));
    }

    /*Reset cursor blink animation*/
    anim_t a;
    a.var = ta;
    a.fp = (anim_fp_t)cursor_blink_anim;
    a.time = LV_TA_CUR_BLINK_TIME;
    a.act_time = 0;
    a.end_cb = NULL;
    a.start = 1;
    a.end= 0;
    a.repeat = 1;
    a.repeat_pause = 0;
    a.playback = 1;
    a.playback_pause = 0;
    a.path = anim_get_path(ANIM_PATH_STEP);
    anim_create(&a);

	lv_obj_inv(ta);
}


/**
 * Move the cursor one character right
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_right(lv_obj_t * ta)
{
	uint16_t cp = lv_ta_get_cursor_pos(ta);
	cp++;
	lv_ta_set_cursor_pos(ta, cp);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(ta);
}

/**
 * Move the cursor one character left
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_left(lv_obj_t * ta)
{
	uint16_t cp = lv_ta_get_cursor_pos(ta);
	if(cp > 0)  {
		cp--;
		lv_ta_set_cursor_pos(ta, cp);

		/*It is a valid x step so save it*/
		lv_ta_save_valid_cursor_x(ta);
	}
}

/**
 * Move the cursor one line down
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_down(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	point_t pos;

	/*Get the position of the current letter*/
	lv_label_get_letter_pos(ext->label, lv_ta_get_cursor_pos(ta), &pos);

	/*Increment the y with one line and keep the valid x*/
	lv_style_t * label_style = lv_obj_get_style(ext->label);
	const font_t * font_p = label_style->font;
    cord_t font_h = font_get_height(font_p) >> FONT_ANTIALIAS;
	pos.y += font_h + label_style->line_space + 1;
	pos.x = ext->cursor_valid_x;

	/*Do not go below he last line*/
	if(pos.y < lv_obj_get_height(ext->label)) {
		/*Get the letter index on the new cursor position and set it*/
		uint16_t new_cur_pos = lv_label_get_letter_on(ext->label, &pos);
		lv_ta_set_cursor_pos(ta, new_cur_pos);
	}
}

/**
 * Move the cursor one line up
 * @param ta pointer to a text area object
 */
void lv_ta_cursor_up(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	point_t pos;

	/*Get the position of the current letter*/
	lv_label_get_letter_pos(ext->label, lv_ta_get_cursor_pos(ta), &pos);

	/*Decrement the y with one line and keep the valid x*/
	lv_style_t * label_style = lv_obj_get_style(ext->label);
	const font_t * font = label_style->font;
    cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
	pos.y -= font_h + label_style->line_space - 1;
	pos.x = ext->cursor_valid_x;

	/*Get the letter index on the new cursor position and set it*/
	uint16_t new_cur_pos = lv_label_get_letter_on(ext->label, &pos);
	lv_ta_set_cursor_pos(ta, new_cur_pos);
}

/**
 * Set the cursor visibility.
 * @param ta pointer to a text area object
 * @return show true: show the cursor and blink it, false: hide cursor
 */
void lv_ta_set_cursor_show(lv_obj_t * ta, bool show)
{
    lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    ext->cursor_show = show == false ? 0 : 1;
    ext->cursor_state = 0;
    lv_obj_inv(ta);
}

/**
 * Set the cursor type.
 * @param ta pointer to a text area object
 * @param cur_type: element of 'lv_ta_cursor_type_t'
 */
void lv_ta_set_cursor_type(lv_obj_t * ta, lv_ta_cursor_type_t cur_type)
{
    lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    ext->cursor_type = cur_type;
    lv_obj_inv(ta);
}

/**
 * Set the style of the cursor (NULL to use label's style)
 * @param ta pointer to a text area object
 * @param style pointer to the new cursor style
 */
void lv_ta_set_cursor_style(lv_obj_t * ta, lv_style_t * style)
{
    lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    ext->cursor_style = style;
    lv_obj_inv(ta);
}


/**
 * Enable/Disable password mode
 * @param ta ointer to a text area object
 * @param en true: enable, false: disable
 */
void lv_ta_set_pwd_mode(lv_obj_t * ta, bool en)
{
    lv_ta_ext_t * ext = lv_obj_get_ext(ta);

    /*Pwd mode is now enabled*/
    if(ext->pwd_mode == 0 && en != false) {
        char * txt = lv_label_get_text(ext->label);
        uint16_t len = strlen(txt);
        ext->pwd_tmp = dm_alloc(len + 1);
        strcpy(ext->pwd_tmp, txt);

        uint16_t i;
        for(i = 0; i < len; i++) {
            txt[i] = '*';       /*All char to '*'*/
        }
        txt[i] = '\0';

        lv_label_set_text(ext->label, NULL);
    }
    /*Pwd mode is now disabled*/
    else if(ext->pwd_mode == 1 && en == false) {
        lv_label_set_text(ext->label, ext->pwd_tmp);
        dm_free(ext->pwd_tmp);
        ext->pwd_tmp = NULL;
    }

    ext->pwd_mode = en == false ? 0 : 1;
}

/**
 * Configure the text area to one line or back to normal
 * @param ta pointer to a Text area object
 * @param en true: one line, false: normal
 */
void lv_ta_set_one_line(lv_obj_t * ta, bool en)
{
    if(en != false) {
        lv_ta_ext_t * ext = lv_obj_get_ext(ta);
        lv_style_t * style_ta = lv_obj_get_style(ta);
        lv_style_t * style_label = lv_obj_get_style(ext->label);
        cord_t font_h =  font_get_height(style_label->font) >> FONT_ANTIALIAS;

        ext->one_line = 1;
        lv_cont_set_fit(lv_page_get_scrl(ta), true, true);
        lv_obj_set_height(ta, font_h + style_ta->vpad * 2);
        lv_label_set_long_mode(ext->label, LV_LABEL_LONG_EXPAND);
        lv_label_set_no_break(ext->label, true);
        lv_obj_set_pos(lv_page_get_scrl(ta), style_ta->hpad, style_ta->vpad);
    } else {
        lv_ta_ext_t * ext = lv_obj_get_ext(ta);
        lv_style_t * style_ta = lv_obj_get_style(ta);

        ext->one_line = 0;
        lv_cont_set_fit(lv_page_get_scrl(ta), false, true);
        lv_label_set_long_mode(ext->label, LV_LABEL_LONG_BREAK);
        lv_label_set_no_break(ext->label, false);
        lv_obj_set_height(ta, LV_TA_DEF_HEIGHT);
        lv_obj_set_pos(lv_page_get_scrl(ta), style_ta->hpad, style_ta->vpad);
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a text area
 * @param ta pointer to a text area object
 * @return pointer to the text
 */
const char * lv_ta_get_txt(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);

	const char * txt;
	if(ext->pwd_mode == 0) {
	    txt = lv_label_get_text(ext->label);
	} else {
	    txt = ext->pwd_tmp;
	}

	return txt;
}


/**
 * Get the label of a text area
 * @param ta pointer to a text area object
 * @return pointer to the label object
 */
lv_obj_t * lv_ta_get_label(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    return ext->label;
}


/**
 * Get the current cursor position in character index
 * @param ta pointer to a text area object
 * @return the cursor position
 */
uint16_t lv_ta_get_cursor_pos(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	return ext->cursor_pos;
}

/**
 * Get the current cursor visibility.
 * @param ta pointer to a text area object
 * @return true: the cursor is drawn, false: the cursor is hidden
 */
bool lv_ta_get_cursor_show(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    return ext->cursor_show;
}

/**
 * Get the current cursor type.
 * @param ta pointer to a text area object
 * @return element of 'lv_ta_cursor_type_t'
 */
lv_ta_cursor_type_t lv_ta_get_cursor_type(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    return ext->cursor_type;
}

/**
 * Get the style of the cursor
 * @param ta pointer to a text area object
 * @return style pointer to the new cursor style
 */
lv_style_t *  lv_ta_get_cursor_style(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    return ext->cursor_style;
}

/**
 * Get the password mode
 * @param ta pointer to a text area object
 * @return true: password mode is enabled, false: disabled
 */
bool lv_ta_get_pwd_mode(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    return ext->pwd_mode;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the text areas
 * @param ta pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW_MAIN: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_ta_design(lv_obj_t * ta, const area_t * masp, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return ancestor_design_f(ta, masp, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Draw the object*/
		ancestor_design_f(ta, masp, mode);

    } else if(mode == LV_DESIGN_DRAW_POST) {
		ancestor_design_f(ta, masp, mode);
    }
    return true;
}

/**
 * An extended scrollable design of the page. Calls the normal design function and draws a cursor.
 * @param scrl pointer to the scrollable part of the Text area
 * @param mask  the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW_MAIN: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @return return true/false, depends on 'mode'
 */
static bool lv_ta_scrling_design(lv_obj_t * scrl, const area_t * mask, lv_design_mode_t mode)
{
	if(mode == LV_DESIGN_COVER_CHK) {
		/*Return false if the object is not covers the mask_p area*/
		return scrl_design_f(scrl, mask, mode);
	} else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Draw the object*/
		scrl_design_f(scrl, mask, mode);
	} else if(mode == LV_DESIGN_DRAW_POST) {
		scrl_design_f(scrl, mask, mode);

		/*Draw the cursor too*/
		lv_obj_t * ta = lv_obj_get_parent(scrl);
		lv_ta_ext_t * ta_ext = lv_obj_get_ext(ta);
		if(ta_ext->cursor_show == 0 || ta_ext->cursor_state == 0) return true; 	/*The cursor is not visible now*/

        lv_style_t * label_style = lv_obj_get_style(ta_ext->label);

        lv_style_t cur_style;
        if(ta_ext->cursor_style != NULL) {
            lv_style_cpy(&cur_style, ta_ext->cursor_style);
        }
        else {
            /*If cursor style is not specified then use the modified label style */
        	lv_style_cpy(&cur_style, label_style);
        	color_t ccolor_tmp = cur_style.ccolor;		/*Make letter color to cursor color*/
        	cur_style.ccolor = cur_style.mcolor;		/*In block mode the letter color will be current background color*/
        	cur_style.mcolor = ccolor_tmp;
        	cur_style.gcolor = ccolor_tmp;
        	cur_style.bcolor = ccolor_tmp;
        	cur_style.bopa = OPA_COVER;
        	cur_style.bwidth = 1 * LV_DOWNSCALE;
        	cur_style.line_width = 1 * LV_DOWNSCALE;
        	cur_style.swidth = 0;
        	cur_style.radius = 0;
        	cur_style.empty = 0;
        	cur_style.opa = OPA_COVER;
        }

		uint16_t cur_pos = lv_ta_get_cursor_pos(ta);
		const char * txt = lv_label_get_text(ta_ext->label);

        uint32_t byte_pos;
#if TXT_UTF8 != 0
        byte_pos = txt_utf8_get_id(txt, cur_pos);
#else
        byte_pos = cur_pos;
#endif

		uint32_t letter = txt_utf8_next(&txt[byte_pos], NULL);
		cord_t letter_h = font_get_height(label_style->font) >> FONT_ANTIALIAS;
		/*Set letter_w (set not 0 on non printable but valid chars)*/
        cord_t letter_w;
		if(letter == '\0' || letter == '\n' || letter == '\r') {
		    letter_w = font_get_width(label_style->font, ' ');
		} else {
            letter_w = font_get_width(label_style->font, letter);
		}

		point_t letter_pos;
		lv_label_get_letter_pos(ta_ext->label, cur_pos, &letter_pos);

		/*If the cursor is out of the text (most right) draw it to the next line*/
		if(letter_pos.x + ta_ext->label->cords.x1 + letter_w > ta_ext->label->cords.x2 && ta_ext->one_line == 0) {
		    letter_pos.x = 0;
		    letter_pos.y += letter_h + label_style->line_space;

		    if(letter != '\0'){
		        byte_pos += txt_utf8_size(txt[byte_pos]);
		        letter = txt_utf8_next(&txt[byte_pos], NULL);
		    }

		    if(letter == '\0' || letter == '\n' || letter == '\r') {
                letter_w = font_get_width(label_style->font, ' ');
            } else {
                letter_w = font_get_width(label_style->font, letter);
            }
		}

		/*Draw he cursor according to the type*/
		area_t cur_area;
		if(ta_ext->cursor_type == LV_TA_CURSOR_LINE) {
			cur_area.x1 = letter_pos.x + ta_ext->label->cords.x1 - (cur_style.line_width >> 1) - (cur_style.line_width & 0x1);
			cur_area.y1 = letter_pos.y + ta_ext->label->cords.y1;
			cur_area.x2 = letter_pos.x + ta_ext->label->cords.x1 + (cur_style.line_width >> 1);
			cur_area.y2 = letter_pos.y + ta_ext->label->cords.y1 + letter_h;
			lv_draw_rect(&cur_area, mask, &cur_style);
		} else if(ta_ext->cursor_type == LV_TA_CURSOR_BLOCK) {
			cur_area.x1 = letter_pos.x + ta_ext->label->cords.x1;
			cur_area.y1 = letter_pos.y + ta_ext->label->cords.y1;
			cur_area.x2 = letter_pos.x + ta_ext->label->cords.x1 + letter_w;
			cur_area.y2 = letter_pos.y + ta_ext->label->cords.y1 + letter_h;

			lv_draw_rect(&cur_area, mask, &cur_style);

			/*Get the current letter*/
#if TXT_UTF8 == 0
			char letter_buf[2];
			letter_buf[0] = txt[byte_pos];
            letter_buf[1] = '\0';
#else
            char letter_buf[8] = {0};
            memcpy(letter_buf, &txt[byte_pos], txt_utf8_size(txt[byte_pos]));
#endif
			lv_draw_label(&cur_area, mask, &cur_style, letter_buf, TXT_FLAG_NONE, 0);

		} else if(ta_ext->cursor_type == LV_TA_CURSOR_OUTLINE) {
			cur_area.x1 = letter_pos.x + ta_ext->label->cords.x1;
			cur_area.y1 = letter_pos.y + ta_ext->label->cords.y1;
			cur_area.x2 = letter_pos.x + ta_ext->label->cords.x1 + letter_w;
			cur_area.y2 = letter_pos.y + ta_ext->label->cords.y1 + letter_h;

			cur_style.empty = 1;
			if(cur_style.bwidth == 0) cur_style.bwidth = 1 * LV_DOWNSCALE; /*Be sure the border will be drawn*/
			lv_draw_rect(&cur_area, mask, &cur_style);
		} else if(ta_ext->cursor_type == LV_TA_CURSOR_UNDERLINE) {
			cur_area.x1 = letter_pos.x + ta_ext->label->cords.x1;
			cur_area.y1 = letter_pos.y + ta_ext->label->cords.y1 + letter_h - (cur_style.line_width >> 1);
			cur_area.x2 = letter_pos.x + ta_ext->label->cords.x1 + letter_w;
			cur_area.y2 = letter_pos.y + ta_ext->label->cords.y1 + letter_h + (cur_style.line_width >> 1) + (cur_style.line_width & 0x1);

			lv_draw_rect(&cur_area, mask, &cur_style);
		}

	}

	return true;
}


/**
 * Called to blink the cursor
 * @param ta pointer to a text area
 * @param hide 1: hide the cursor, 0: show it
 */
static void cursor_blink_anim(lv_obj_t * ta, uint8_t show)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	if(show != ext->cursor_state) {
        ext->cursor_state = show == 0 ? 0 : 1;
        if(ext->cursor_show != 0) lv_obj_inv(ta);
	}
}


/**
 * Dummy function to animate char hiding in pwd mode.
 * Does nothing, but a function is required in car hiding anim.
 * (pwd_char_hider callback do the real job)
 * @param ta unused
 * @param x unused
 */
static void pwd_char_hider_anim(lv_obj_t * ta, int32_t x)
{

}

/**
 * Hide all characters (convert them to '*')
 * @param ta: pointer to text area object
 */
static void pwd_char_hider(lv_obj_t * ta)
{
    lv_ta_ext_t * ext = lv_obj_get_ext(ta);
    if(ext->pwd_mode != 0) {
        char * txt = lv_label_get_text(ext->label);
        int16_t len = txt_len(txt);
        bool refr = false;
        uint16_t i;
        for(i = 0; i < len; i++) txt[i] = '*';

        txt[i] = '\0';

        if(refr != false) lv_label_set_text(ext->label, txt);
    }
}

/**
 * Save the cursor x position as valid. It is important when jumping up/down to a shorter line
 * @param ta pointer to a text area object
 */
static void lv_ta_save_valid_cursor_x(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	point_t cur_pos;
	lv_label_get_letter_pos(ext->label, ext->cursor_pos, &cur_pos);
	ext->cursor_valid_x = cur_pos.x;
}

#endif
