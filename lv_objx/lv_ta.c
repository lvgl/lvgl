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
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define LV_TA_MAX_LENGTH	512

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_ta_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);
static bool lv_ta_label_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);
static void lv_ta_save_valid_cursor_x(lv_obj_t * obj_dp);
static void lv_tas_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_tas_t lv_tas_def;

lv_design_f_t ancestor_design_f;
lv_design_f_t label_design_f;

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
 * @param par_dp pointer to an object, it will be the parent of the new text area
 * @param copy_dp pointer to a text area object, if not NULL then the new object will be copied from it
 * @return pointer to the created text area
 */
lv_obj_t* lv_ta_create(lv_obj_t* par_dp, lv_obj_t * copy_dp)
{
    /*Create the ancestor object*/
    lv_obj_t* new_obj_dp = lv_page_create(par_dp, copy_dp);
    dm_assert(new_obj_dp);
    
    /*Allocate the object type specific extended data*/
    lv_ta_ext_t * ext_dp = lv_obj_alloc_ext(new_obj_dp, sizeof(lv_ta_ext_t));
    dm_assert(ext_dp);

    if(ancestor_design_f == NULL) {
    	ancestor_design_f = lv_obj_get_design_f(new_obj_dp);
    }

    lv_obj_set_signal_f(new_obj_dp, lv_ta_signal);
    lv_obj_set_design_f(new_obj_dp, lv_ta_design);

    ext_dp->cursor_valid_x = 0;
    ext_dp->cursor_pos = 0;

    /*Init the new text area object*/
    if(copy_dp == NULL) {
    	ext_dp->label_dp = lv_label_create(new_obj_dp, NULL);
    	if(label_design_f == NULL) {
    		label_design_f = lv_obj_get_design_f(ext_dp->label_dp);
    	}
    	lv_obj_set_design_f(ext_dp->label_dp, lv_ta_label_design);
    	lv_label_set_fixw(ext_dp->label_dp, true);
    	lv_label_set_text(ext_dp->label_dp, "abc aaaa bbbb ccc\n123\nABC\nxyz\nwww\n007\nalma\n:)\naaaaaa");
    	lv_page_glue_obj(ext_dp->label_dp, true);
    	lv_obj_set_click(ext_dp->label_dp, true);
    	lv_obj_set_style(new_obj_dp, lv_tas_get(LV_TAS_DEF, NULL));

    }
    /*Copy an existing object*/
    else {
    	lv_ta_ext_t * copy_ext_dp = lv_obj_get_ext(copy_dp);
    	ext_dp->label_dp = lv_label_create(new_obj_dp, copy_ext_dp->label_dp);
    	lv_obj_set_design_f(ext_dp->label_dp, lv_ta_label_design);
    	lv_page_glue_obj(ext_dp->label_dp, true);

    	/*Refresh the style when everything is ready*/
    	lv_obj_set_style(new_obj_dp, lv_obj_get_style(copy_dp));
    }
    
    return new_obj_dp;
}

/**
 * Signal function of the text area
 * @param obj_dp pointer to a text area object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_ta_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_page_signal(obj_dp, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	lv_ta_ext_t * ta_ext_dp = lv_obj_get_ext(obj_dp);
    	lv_tas_t * tas_p = lv_obj_get_style(obj_dp);
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			lv_obj_del(ta_ext_dp->label_dp);
    			break;
    		case LV_SIGNAL_STYLE_CHG:
    			lv_obj_set_style(ta_ext_dp->label_dp, &tas_p->labels);
    	    	lv_obj_set_width(ta_ext_dp->label_dp, lv_obj_get_width(obj_dp) - 2 *
    	    			(tas_p->pages.bg_rects.hpad + tas_p->pages.scrable_rects.hpad));
    	    	lv_label_set_text(ta_ext_dp->label_dp, NULL);
    			break;
    		/*Set the label width according to the text area width*/
    		case LV_SIGNAL_CORD_CHG:
    	    	lv_obj_set_width(ta_ext_dp->label_dp, lv_obj_get_width(obj_dp) - 2 *
    	    			(tas_p->pages.bg_rects.hpad + tas_p->pages.scrable_rects.hpad));
    	    	lv_label_set_text(ta_ext_dp->label_dp, NULL);
    			break;
    		default:
    			break;
    	}
    }
    
    return valid;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Insert a character to the current cursor position
 * @param obj_dp pointer to a text area object
 * @param c a character
 */
void lv_ta_add_char(lv_obj_t * obj_dp, char c)
{
	lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);

	/*Insert the character*/
	char buf[LV_TA_MAX_LENGTH];
	const char * label_txt = lv_label_get_text(ta_dp->label_dp);

	memcpy(buf, label_txt, ta_dp->cursor_pos);
	buf[ta_dp->cursor_pos] = c;
	memcpy(buf+ta_dp->cursor_pos+1, label_txt+ta_dp->cursor_pos, strlen(label_txt) - ta_dp->cursor_pos + 1);

	/*Refresh the label*/
	lv_label_set_text(ta_dp->label_dp, buf);

	/*Move the cursor after the new character*/
	lv_ta_set_cursor_pos(obj_dp, lv_ta_get_cursor_pos(obj_dp) + 1);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(obj_dp);
}

/**
 * Insert a text to the current cursor position
 * @param obj_dp pointer to a text area object
 * @param txt a '\0' terminated string to insert
 */
void lv_ta_add_text(lv_obj_t * obj_dp, const char * txt)
{
	lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);

	/*Insert the text*/
	char buf[LV_TA_MAX_LENGTH];
	const char * label_txt = lv_label_get_text(ta_dp->label_dp);
	uint16_t label_len = strlen(label_txt);
	uint16_t txt_len = strlen(txt);

	memcpy(buf, label_txt, ta_dp->cursor_pos);
	memcpy(buf+ta_dp->cursor_pos, txt, txt_len);
	memcpy(buf+ta_dp->cursor_pos + txt_len, label_txt+ta_dp->cursor_pos, label_len - ta_dp->cursor_pos + 1);

	/*Refresh the label*/
	lv_label_set_text(ta_dp->label_dp, buf);

	/*Move the cursor after the new text*/
	lv_ta_set_cursor_pos(obj_dp, lv_ta_get_cursor_pos(obj_dp) + txt_len);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(obj_dp);
}

/**
 * Delete a the left character from the current cursor position
 * @param obj_dp pointer to a text area object
 */
void lv_ta_del(lv_obj_t * obj_dp)
{
	lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);
	uint16_t cur_pos = ta_dp->cursor_pos;

	if(cur_pos == 0) return;

	/*Delete a character*/
	char buf[LV_TA_MAX_LENGTH];
	const char * label_txt = lv_label_get_text(ta_dp->label_dp);
	uint16_t label_len = strlen(label_txt);
	memcpy(buf, label_txt, cur_pos - 1);
	memcpy(buf+cur_pos - 1, label_txt + cur_pos, label_len - cur_pos + 1);

	/*Refresh the label*/
	lv_label_set_text(ta_dp->label_dp, buf);

	/*Move the cursor to the place of the deleted character*/
	lv_ta_set_cursor_pos(obj_dp, lv_ta_get_cursor_pos(obj_dp) - 1);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(obj_dp);
}

/**
 * Set the cursor position
 * @param obj_dp pointer to a text area object
 * @param pos the new cursor position in character index
 */
void lv_ta_set_cursor_pos(lv_obj_t * obj_dp, uint16_t pos)
{
	lv_ta_ext_t * ta_ext_dp = lv_obj_get_ext(obj_dp);
	uint16_t txt_len = strlen(lv_label_get_text(ta_ext_dp->label_dp));

	if(pos > txt_len) pos = txt_len;

	ta_ext_dp->cursor_pos = pos;

	/*Position the label to make the cursor visible*/
	lv_obj_t * label_par_dp = lv_obj_get_parent(ta_ext_dp->label_dp);
	point_t cur_pos;
	lv_tas_t * tas_p = lv_obj_get_style(obj_dp);
	const font_t * font_p = font_get(tas_p->labels.font);
	lv_label_get_letter_pos(ta_ext_dp->label_dp, pos, &cur_pos);

	/*Check the top*/
	if(lv_obj_get_y(label_par_dp) + cur_pos.y < 0) {
		lv_obj_set_y(label_par_dp, - cur_pos.y);
	}

	/*Check the bottom*/
	if(lv_obj_get_y(label_par_dp) + cur_pos.y + font_get_height(font_p) > lv_obj_get_height(obj_dp)) {
		lv_obj_set_y(label_par_dp, -(cur_pos.y - lv_obj_get_height(obj_dp) +
				                     font_get_height(font_p) + tas_p->pages.scrable_rects.vpad * 2));
	}

	lv_obj_inv(obj_dp);
}


/**
 * Move the cursor one character right
 * @param obj_dp pointer to a text area object
 */
void lv_ta_cursor_right(lv_obj_t * obj_dp)
{
	uint16_t cp = lv_ta_get_cursor_pos(obj_dp);
	cp++;
	lv_ta_set_cursor_pos(obj_dp, cp);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(obj_dp);
}

/**
 * Move the cursor one character left
 * @param obj_dp pointer to a text area object
 */
void lv_ta_cursor_left(lv_obj_t * obj_dp)
{
	uint16_t cp = lv_ta_get_cursor_pos(obj_dp);
	if(cp > 0)  {
		cp--;
		lv_ta_set_cursor_pos(obj_dp, cp);

		/*It is a valid x step so save it*/
		lv_ta_save_valid_cursor_x(obj_dp);
	}
}

/**
 * Move the cursor one line down
 * @param obj_dp pointer to a text area object
 */
void lv_ta_cursor_down(lv_obj_t * obj_dp)
{
	lv_ta_ext_t * ta_ext_dp = lv_obj_get_ext(obj_dp);
	point_t pos;

	/*Get the position of the current letter*/
	lv_label_get_letter_pos(ta_ext_dp->label_dp, lv_ta_get_cursor_pos(obj_dp), &pos);

	/*Increment the y with one line and keep the valid x*/
	lv_labels_t * labels_p = lv_obj_get_style(ta_ext_dp->label_dp);
	const font_t * font_p = font_get(labels_p->font);
	pos.y += font_get_height(font_p) + labels_p->line_space + 1;
	pos.x = ta_ext_dp->cursor_valid_x;

	/*Do not go below he last line*/
	if(pos.y < lv_obj_get_height(ta_ext_dp->label_dp)) {
		/*Get the letter index on the new cursor position and set it*/
		uint16_t new_cur_pos = lv_label_get_letter_on(ta_ext_dp->label_dp, &pos);
		lv_ta_set_cursor_pos(obj_dp, new_cur_pos);
	}
}

/**
 * Move the cursor one line up
 * @param obj_dp pointer to a text area object
 */
void lv_ta_cursor_up(lv_obj_t * obj_dp)
{
	lv_ta_ext_t * ta_ext_dp = lv_obj_get_ext(obj_dp);
	point_t pos;

	/*Get the position of the current letter*/
	lv_label_get_letter_pos(ta_ext_dp->label_dp, lv_ta_get_cursor_pos(obj_dp), &pos);

	/*Decrement the y with one line and keep the valid x*/
	lv_labels_t * labels_p = lv_obj_get_style(ta_ext_dp->label_dp);
	const font_t * font_p = font_get(labels_p->font);
	pos.y -= font_get_height(font_p) + labels_p->line_space - 1;
	pos.x = ta_ext_dp->cursor_valid_x;

	/*Get the letter index on the new cursor position and set it*/
	uint16_t new_cur_pos = lv_label_get_letter_on(ta_ext_dp->label_dp, &pos);
	lv_ta_set_cursor_pos(obj_dp, new_cur_pos);
}
/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of the i the text area
 * @param obj_dp obj_dp pointer to a text area object
 * @return pointer to the text
 */
const char * lv_ta_get_text(lv_obj_t * obj_dp)
{
	lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);
	return lv_label_get_text(ta_dp->label_dp);
}

/**
 * Get the current cursor position in character index
 * @param obj_dp pointer to a text area object
 * @return the cursor position
 */
uint16_t lv_ta_get_cursor_pos(lv_obj_t * obj_dp)
{
	lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);
	return ta_dp->cursor_pos;
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_tas_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_tas_t style
 */
lv_tas_t * lv_tas_get(lv_tas_builtin_t style, lv_tas_t * copy_p)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_tas_init();
		style_inited = true;
	}

	lv_tas_t  *style_p;

	switch(style) {
		case LV_TAS_DEF:
			style_p = &lv_tas_def;
			break;
		default:
			style_p = &lv_tas_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_tas_t));
		else memcpy(copy_p, &lv_tas_def, sizeof(lv_tas_t));
	}

	return style_p;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Handle the drawing related tasks of the text areas
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW_MAIN: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_ta_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return ancestor_design_f(obj_dp, mask_p, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Draw the object*/
		ancestor_design_f(obj_dp, mask_p, mode);

    } else if(mode == LV_DESIGN_DRAW_POST) {
		ancestor_design_f(obj_dp, mask_p, mode);
    }
    return true;
}


static bool lv_ta_label_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
	if(mode == LV_DESIGN_COVER_CHK) {
		/*Return false if the object is not covers the mask_p area*/
		return label_design_f(obj_dp, mask_p, mode);
	} else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Draw the object*/
		label_design_f(obj_dp, mask_p, mode);
	} else if(mode == LV_DESIGN_DRAW_POST) {
		label_design_f(obj_dp, mask_p, mode);

		/*Draw the cursor too*/
		lv_obj_t * ta_dp = lv_obj_get_parent(lv_obj_get_parent(obj_dp));
		lv_ta_ext_t * ta_ext_dp = lv_obj_get_ext(ta_dp);
		lv_tas_t * tas_p = lv_obj_get_style(ta_dp);
		uint16_t cur_pos = lv_ta_get_cursor_pos(ta_dp);
		point_t letter_pos;
		lv_label_get_letter_pos(obj_dp, cur_pos, &letter_pos);

		area_t cur_area;
		lv_labels_t * labels_p = lv_obj_get_style(ta_ext_dp->label_dp);
		cur_area.x1 = letter_pos.x + obj_dp->cords.x1 - (tas_p->cursor_width >> 1);
		cur_area.y1 = letter_pos.y + obj_dp->cords.y1;
		cur_area.x2 = letter_pos.x + obj_dp->cords.x1 + (tas_p->cursor_width >> 1);
		cur_area.y2 = letter_pos.y + obj_dp->cords.y1 + font_get_height(font_get(labels_p->font));

		lv_rects_t cur_rects;
		lv_rects_get(LV_RECTS_DEF, &cur_rects);
		cur_rects.round = 0;
		cur_rects.bwidth = 0;
		cur_rects.objs.color = tas_p->cursor_color;
		cur_rects.gcolor = tas_p->cursor_color;
		lv_draw_rect(&cur_area, mask_p, &cur_rects, OPA_COVER);
	}

	return true;
}

/**
 * Save the cursor x position as valid. It is important when jumping up/down to a shorter line
 * @param obj_dp pointer to a text area object
 */
static void lv_ta_save_valid_cursor_x(lv_obj_t * obj_dp)
{
	lv_ta_ext_t * ta_ext_dp = lv_obj_get_ext(obj_dp);
	point_t cur_pos;
	lv_label_get_letter_pos(ta_ext_dp->label_dp, ta_ext_dp->cursor_pos, &cur_pos);
	ta_ext_dp->cursor_valid_x = cur_pos.x;
}

/**
 * Initialize the text area styles
 */
static void lv_tas_init(void)
{
	/*Default style*/
	lv_pages_get(LV_PAGES_DEF, &lv_tas_def.pages);

	lv_labels_get(LV_LABELS_TXT, &lv_tas_def.labels);
	lv_tas_def.labels.objs.color = COLOR_MAKE(0x20, 0x20, 0x20);

	lv_tas_def.cursor_color = COLOR_MAKE(0x10, 0x10, 0x10);
	lv_tas_def.cursor_width = 2 * LV_STYLE_MULT;	/*>1 px for visible cursor*/
}
#endif
