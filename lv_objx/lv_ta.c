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

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_tas_t lv_tas_def =
{ /*Create a default style*/ };

lv_design_f_t ancestor_design_f;

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

    /*Init the new text area object*/
    if(copy_dp == NULL) {
    	ext_dp->label_dp = lv_label_create(new_obj_dp, NULL);
    	lv_label_set_text(ext_dp->label_dp, "0123456789 abcdef\nABCDEF\nG\nHIJKLM\n\nnopqrs");
    	//lv_obj_set_style(ext_dp->label_dp, lv_labels_get(LV_LABELS_TXT, NULL));
    //	lv_label_set_fixw(ext_dp->label_dp, true);
    	lv_obj_set_width_us(ext_dp->label_dp, 200);
    }
    /*Copy an existing object*/
    else {

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
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			/*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
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

void lv_ta_add_char(lv_obj_t * obj_dp, char c)
{
	lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);

	char buf[LV_TA_MAX_LENGTH];
	const char * label_txt = lv_label_get_text(ta_dp->label_dp);

	memcpy(buf, label_txt, ta_dp->cursor_pos);
	buf[ta_dp->cursor_pos] = c;
	memcpy(buf+ta_dp->cursor_pos+1, label_txt+ta_dp->cursor_pos, strlen(label_txt) - ta_dp->cursor_pos + 1);

	lv_label_set_text(ta_dp->label_dp, buf);

	lv_ta_set_cursor_pos(obj_dp, lv_ta_get_cursor_pos(obj_dp) + 1);
}

void lv_ta_add_text(lv_obj_t * obj_dp, const char * txt)
{
	lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);

	char buf[LV_TA_MAX_LENGTH];
	const char * label_txt = lv_label_get_text(ta_dp->label_dp);
	uint16_t label_len = strlen(label_txt);
	uint16_t txt_len = strlen(txt);

	memcpy(buf, label_txt, ta_dp->cursor_pos);
	memcpy(buf+ta_dp->cursor_pos, txt, txt_len);
	memcpy(buf+ta_dp->cursor_pos + txt_len, label_txt+ta_dp->cursor_pos, label_len - ta_dp->cursor_pos + 1);

	lv_label_set_text(ta_dp->label_dp, buf);

	lv_ta_set_cursor_pos(obj_dp, lv_ta_get_cursor_pos(obj_dp) + txt_len);
}

void lv_ta_del(lv_obj_t * obj_dp)
{
	lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);
	uint16_t cur_pos = ta_dp->cursor_pos;

	if(cur_pos == 0) return;

	char buf[LV_TA_MAX_LENGTH];
	const char * label_txt = lv_label_get_text(ta_dp->label_dp);
	uint16_t label_len = strlen(label_txt);
	memcpy(buf, label_txt, cur_pos - 1);
	memcpy(buf+cur_pos - 1, label_txt + cur_pos, label_len - cur_pos + 1);

	lv_label_set_text(ta_dp->label_dp, buf);

	lv_ta_set_cursor_pos(obj_dp, lv_ta_get_cursor_pos(obj_dp) - 1);
}

void lv_ta_set_cursor_pos(lv_obj_t * obj_dp, uint16_t pos)
{
	lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);
	ta_dp->cursor_pos = pos;

	lv_obj_inv(obj_dp);
}

void lv_ta_cursor_down(lv_obj_t * obj_dp)
{
	lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);
	point_t pos;

	lv_label_get_letter_pos(ta_dp->label_dp, lv_ta_get_cursor_pos(obj_dp), &pos);

	lv_labels_t * labels_p = lv_obj_get_style(ta_dp->label_dp);
	const font_t * font_p = font_get(labels_p->font);
	pos.y += font_get_height(font_p) + labels_p->line_space + 1;

	uint16_t new_cur_pos = lv_label_get_letter_on(ta_dp->label_dp, &pos);

	lv_ta_set_cursor_pos(obj_dp, new_cur_pos);
}

void lv_ta_cursor_up(lv_obj_t * obj_dp)
{
	lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);
	point_t pos;

	lv_label_get_letter_pos(ta_dp->label_dp, lv_ta_get_cursor_pos(obj_dp), &pos);

	lv_labels_t * labels_p = lv_obj_get_style(ta_dp->label_dp);
	const font_t * font_p = font_get(labels_p->font);
	pos.y -= font_get_height(font_p) + labels_p->line_space - 1;

	uint16_t new_cur_pos = lv_label_get_letter_on(ta_dp->label_dp, &pos);

	lv_ta_set_cursor_pos(obj_dp, new_cur_pos);
}
/*=====================
 * Getter functions
 *====================*/

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_tas_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_tas_t style
 */
lv_tas_t * lv_tas_get(lv_tas_builtin_t style, lv_tas_t * copy_p)
{
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

uint16_t lv_ta_get_cursor_pos(lv_obj_t * obj_dp)
{
	lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);
	return ta_dp->cursor_pos;
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
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'
 */
static bool lv_ta_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return ancestor_design_f(obj_dp, mask_p, mode);
    }

    /*Draw the object*/
    ancestor_design_f(obj_dp, mask_p, mode);

    lv_ta_ext_t * ta_dp = lv_obj_get_ext(obj_dp);
    uint16_t cur_pos = lv_ta_get_cursor_pos(obj_dp);
    point_t letter_pos;
    lv_label_get_letter_pos(ta_dp->label_dp, cur_pos, &letter_pos);

    area_t cur_area;
    lv_labels_t * labels_p = lv_obj_get_style(ta_dp->label_dp);
    cur_area.x1 = letter_pos.x + ta_dp->label_dp->cords.x1 - 2;
    cur_area.y1 = letter_pos.y + ta_dp->label_dp->cords.y1;
    cur_area.x2 = letter_pos.x + ta_dp->label_dp->cords.x1 + 2;
    cur_area.y2 = letter_pos.y + ta_dp->label_dp->cords.y1 + font_get_height(font_get(labels_p->font));

    lv_rects_t rects;
    lv_rects_get(LV_RECTS_DEF, &rects);
    rects.round = 0;
    rects.bwidth = 0;
    rects.objs.color = COLOR_RED;
    rects.gcolor = COLOR_RED;
    lv_draw_rect(&cur_area, mask_p, &rects, OPA_COVER);

    return true;
}


#endif
