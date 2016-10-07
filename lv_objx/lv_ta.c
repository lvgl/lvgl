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
static bool lv_ta_design(lv_obj_t * ta, const area_t * mask, lv_design_mode_t mode);
static bool lv_ta_label_design(lv_obj_t * label, const area_t * mask, lv_design_mode_t mode);
static void lv_ta_save_valid_cursor_x(lv_obj_t * ta);
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

    if(ancestor_design_f == NULL) {
    	ancestor_design_f = lv_obj_get_design_f(new_ta);
    }

    lv_obj_set_signal_f(new_ta, lv_ta_signal);
    lv_obj_set_design_f(new_ta, lv_ta_design);

    ext->cursor_valid_x = 0;
    ext->cursor_pos = 0;

    /*Init the new text area object*/
    if(copy == NULL) {
    	ext->label = lv_label_create(new_ta, NULL);
    	if(label_design_f == NULL) {
    		label_design_f = lv_obj_get_design_f(ext->label);
    	}
    	lv_obj_set_design_f(ext->label, lv_ta_label_design);
    	lv_label_set_fixw(ext->label, true);
    	lv_label_set_text(ext->label, "abc aaaa bbbb ccc\n123\nABC\nxyz\nwww\n007\nalma\n:)\naaaaaa");
    	lv_page_glue_obj(ext->label, true);
    	lv_obj_set_click(ext->label, true);
    	lv_obj_set_style(new_ta, lv_tas_get(LV_TAS_DEF, NULL));
    }
    /*Copy an existing object*/
    else {
    	lv_ta_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->label = lv_label_create(new_ta, copy_ext->label);
    	lv_obj_set_design_f(ext->label, lv_ta_label_design);
    	lv_page_glue_obj(ext->label, true);

    	/*Refresh the style when everything is ready*/
    	lv_obj_set_style(new_ta, lv_obj_get_style(copy));
    }
    
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
    	lv_tas_t * style = lv_obj_get_style(ta);
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			lv_obj_del(ext->label);
    			break;
    		case LV_SIGNAL_STYLE_CHG:
    			lv_obj_set_style(ext->label, &style->label);
    	    	lv_obj_set_width(ext->label, lv_obj_get_width(ta) - 2 *
    	    			(style->pages.bg_rects.hpad + style->pages.scrable_rects.hpad));
    	    	lv_label_set_text(ext->label, NULL);
    			break;
    		/*Set the label width according to the text area width*/
    		case LV_SIGNAL_CORD_CHG:
    	    	lv_obj_set_width(ext->label, lv_obj_get_width(ta) - 2 *
    	    			(style->pages.bg_rects.hpad + style->pages.scrable_rects.hpad));
    	    	lv_label_set_text(ext->label, NULL);
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
 * @param ta pointer to a text area object
 * @param c a character
 */
void lv_ta_add_char(lv_obj_t * ta, char c)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);

	/*Insert the character*/
	char buf[LV_TA_MAX_LENGTH];
	const char * label_txt = lv_label_get_text(ext->label);

	memcpy(buf, label_txt, ext->cursor_pos);
	buf[ext->cursor_pos] = c;
	memcpy(buf+ext->cursor_pos+1, label_txt+ext->cursor_pos, strlen(label_txt) - ext->cursor_pos + 1);

	/*Refresh the label*/
	lv_label_set_text(ext->label, buf);

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

	/*Insert the text*/
	char buf[LV_TA_MAX_LENGTH];
	const char * label_txt = lv_label_get_text(ext->label);
	uint16_t label_len = strlen(label_txt);
	uint16_t txt_len = strlen(txt);

	memcpy(buf, label_txt, ext->cursor_pos);
	memcpy(buf+ext->cursor_pos, txt, txt_len);
	memcpy(buf+ext->cursor_pos + txt_len, label_txt+ext->cursor_pos, label_len - ext->cursor_pos + 1);

	/*Refresh the label*/
	lv_label_set_text(ext->label, buf);

	/*Move the cursor after the new text*/
	lv_ta_set_cursor_pos(ta, lv_ta_get_cursor_pos(ta) + txt_len);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(ta);
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

	/*Delete a character*/
	char buf[LV_TA_MAX_LENGTH];
	const char * label_txt = lv_label_get_text(ext->label);
	uint16_t label_len = strlen(label_txt);
	memcpy(buf, label_txt, cur_pos - 1);
	memcpy(buf+cur_pos - 1, label_txt + cur_pos, label_len - cur_pos + 1);

	/*Refresh the label*/
	lv_label_set_text(ext->label, buf);

	/*Move the cursor to the place of the deleted character*/
	lv_ta_set_cursor_pos(ta, lv_ta_get_cursor_pos(ta) - 1);

	/*It is a valid x step so save it*/
	lv_ta_save_valid_cursor_x(ta);
}

/**
 * Set the cursor position
 * @param obj pointer to a text area object
 * @param pos the new cursor position in character index
 */
void lv_ta_set_cursor_pos(lv_obj_t * ta, uint16_t pos)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	uint16_t txt_len = strlen(lv_label_get_text(ext->label));

	if(pos > txt_len) pos = txt_len;

	ext->cursor_pos = pos;

	/*Position the label to make the cursor visible*/
	lv_obj_t * label_par = lv_obj_get_parent(ext->label);
	point_t cur_pos;
	lv_tas_t * style = lv_obj_get_style(ta);
	const font_t * font_p = font_get(style->label.font);
	lv_label_get_letter_pos(ext->label, pos, &cur_pos);

	/*Check the top*/
	if(lv_obj_get_y(label_par) + cur_pos.y < 0) {
		lv_obj_set_y(label_par, - cur_pos.y);
	}

	/*Check the bottom*/
	if(lv_obj_get_y(label_par) + cur_pos.y + font_get_height(font_p) > lv_obj_get_height(ta)) {
		lv_obj_set_y(label_par, -(cur_pos.y - lv_obj_get_height(ta) +
				                     font_get_height(font_p) + style->pages.scrable_rects.vpad * 2));
	}

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
	lv_labels_t * label_style = lv_obj_get_style(ext->label);
	const font_t * font_p = font_get(label_style->font);
	pos.y += font_get_height(font_p) + label_style->line_space + 1;
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
	lv_labels_t * label_style = lv_obj_get_style(ext->label);
	const font_t * font = font_get(label_style->font);
	pos.y -= font_get_height(font) + label_style->line_space - 1;
	pos.x = ext->cursor_valid_x;

	/*Get the letter index on the new cursor position and set it*/
	uint16_t new_cur_pos = lv_label_get_letter_on(ext->label, &pos);
	lv_ta_set_cursor_pos(ta, new_cur_pos);
}
/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of the i the text area
 * @param ta obj pointer to a text area object
 * @return pointer to the text
 */
const char * lv_ta_get_txt(lv_obj_t * ta)
{
	lv_ta_ext_t * ext = lv_obj_get_ext(ta);
	return lv_label_get_text(ext->label);
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
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_tas_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_tas_t style
 */
lv_tas_t * lv_tas_get(lv_tas_builtin_t style, lv_tas_t * copy)
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

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_tas_t));
		else memcpy(copy, &lv_tas_def, sizeof(lv_tas_t));
	}

	return style_p;
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
 * An extended label design. Calls the normal label design function and it draws a cursor.
 * @param label pointer to a text area object
 * @param mask  the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW_MAIN: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @return return true/false, depends on 'mode'
 */
static bool lv_ta_label_design(lv_obj_t * label, const area_t * mask, lv_design_mode_t mode)
{
	if(mode == LV_DESIGN_COVER_CHK) {
		/*Return false if the object is not covers the mask_p area*/
		return label_design_f(label, mask, mode);
	} else if(mode == LV_DESIGN_DRAW_MAIN) {
		/*Draw the object*/
		label_design_f(label, mask, mode);
	} else if(mode == LV_DESIGN_DRAW_POST) {
		label_design_f(label, mask, mode);

		/*Draw the cursor too*/
		lv_obj_t * ta = lv_obj_get_parent(lv_obj_get_parent(label));
		lv_ta_ext_t * ta_ext = lv_obj_get_ext(ta);
		lv_tas_t * ta_style = lv_obj_get_style(ta);

		if(ta_style->cursor_show != 0) {
			uint16_t cur_pos = lv_ta_get_cursor_pos(ta);
			point_t letter_pos;
			lv_label_get_letter_pos(label, cur_pos, &letter_pos);

			area_t cur_area;
			lv_labels_t * labels_p = lv_obj_get_style(ta_ext->label);
			cur_area.x1 = letter_pos.x + label->cords.x1 - (ta_style->cursor_width >> 1);
			cur_area.y1 = letter_pos.y + label->cords.y1;
			cur_area.x2 = letter_pos.x + label->cords.x1 + (ta_style->cursor_width >> 1);
			cur_area.y2 = letter_pos.y + label->cords.y1 + font_get_height(font_get(labels_p->font));

			lv_rects_t cur_rects;
			lv_rects_get(LV_RECTS_DEF, &cur_rects);
			cur_rects.round = 0;
			cur_rects.bwidth = 0;
			cur_rects.objs.color = ta_style->cursor_color;
			cur_rects.gcolor = ta_style->cursor_color;
			lv_draw_rect(&cur_area, mask, &cur_rects, OPA_COVER);
		}
	}

	return true;
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

/**
 * Initialize the text area styles
 */
static void lv_tas_init(void)
{
	/*Default style*/
	lv_pages_get(LV_PAGES_DEF, &lv_tas_def.pages);

	lv_labels_get(LV_LABELS_TXT, &lv_tas_def.label);
	lv_tas_def.label.objs.color = COLOR_MAKE(0x20, 0x20, 0x20);

	lv_tas_def.cursor_color = COLOR_MAKE(0x10, 0x10, 0x10);
	lv_tas_def.cursor_width = 2 * LV_STYLE_MULT;	/*>1 px for visible cursor*/
}
#endif
