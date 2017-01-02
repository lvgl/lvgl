/**
 * @file lv_btnm.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_BTNM != 0

#include "lv_btnm.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_misc/text.h"

/*********************
 *      DEFINES
 *********************/
#define LV_BTNM_BTN_PR_INVALID 0xFFFF

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool lv_btnm_design(lv_obj_t * btnm, const area_t * mask, lv_design_mode_t mode);
static uint8_t lv_btnm_get_width_unit(const char * btn_str);
static void lv_btnm_create_btns(lv_obj_t * btnm, const char ** map);
static void lv_btnms_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_btnms_t lv_btnms_def;

static const char * lv_btnm_def_map[] = {"Btn1","Btn2", "Btn3","\n",
										 "\002Btn4","Btn5", ""};

static lv_design_f_t ancestor_design_f;

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
 * Create a button matrix objects
 * @param par pointer to an object, it will be the parent of the new button matrix
 * @param copy pointer to a button matrix object, if not NULL then the new object will be copied from it
 * @return pointer to the created button matrix
 */
lv_obj_t * lv_btnm_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor object*/
    lv_obj_t * new_btnm = lv_rect_create(par, copy);
    dm_assert(new_btnm);
    
    /*Allocate the object type specific extended data*/
    lv_btnm_ext_t * ext = lv_obj_alloc_ext(new_btnm, sizeof(lv_btnm_ext_t));
    dm_assert(ext);
    ext->btn_cnt = 0;
    ext->btn_pr = LV_BTNM_BTN_PR_INVALID;
    ext->btn_areas = NULL;
    ext->cb = NULL;
    ext->map_p = NULL;

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_btnm);

    lv_obj_set_signal_f(new_btnm, lv_btnm_signal);
    lv_obj_set_design_f(new_btnm, lv_btnm_design);


    /*Init the new button matrix object*/
    if(copy == NULL) {
    	lv_obj_set_size(new_btnm, LV_HOR_RES / 2, LV_VER_RES / 2);
    	lv_obj_set_style(new_btnm, lv_btnms_get(LV_BTNMS_DEF, NULL));
    	lv_btnm_set_map(new_btnm, lv_btnm_def_map);
    }
    /*Copy an existing object*/
    else {
        /*Set the style of 'copy' and isolate it if it is necessary*/
        if(lv_obj_get_style_iso(new_btnm) == false) {
            lv_obj_set_style(new_btnm, lv_obj_get_style(copy));
        } else {
            lv_obj_set_style(new_btnm, lv_obj_get_style(copy));
            lv_obj_iso_style(new_btnm, sizeof(lv_btnms_t));
        }
        lv_btnm_set_map(new_btnm, lv_btnm_get_map(copy));
    }
    
    return new_btnm;
}

/**
 * Signal function of the button matrix
 * @param btnm pointer to a button matrix object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_btnm_signal(lv_obj_t * btnm, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_rect_signal(btnm, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);
    	uint16_t i;
    	point_t p;
    	area_t btn_area;
    	area_t btnm_cords;
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			dm_free(ext->btn_areas);
    			break;
    		case LV_SIGNAL_STYLE_CHG:
    		case LV_SIGNAL_CORD_CHG:
    			lv_btnm_set_map(btnm, LV_EA(btnm, lv_btnm_ext_t)->map_p);
    			break;
    		case LV_SIGNAL_PRESSING:
    			/*Search the pressed area*/
    			ext->btn_pr = LV_BTNM_BTN_PR_INVALID;
    			lv_dispi_get_point(param, &p);
    			lv_obj_get_cords(btnm, &btnm_cords);
    			for(i = 0; i < ext->btn_cnt; i++) {
    				area_cpy(&btn_area, &ext->btn_areas[i]);
    				btn_area.x1 += btnm_cords.x1;
    				btn_area.y1 += btnm_cords.y1;
    				btn_area.x2 += btnm_cords.x1;
    				btn_area.y2 += btnm_cords.y1;
    				if(area_is_point_on(&btn_area, &p) != false) {
    					ext->btn_pr = i;
    					lv_obj_inv(btnm);
    					break;
    				}
    			}
    			break;
    		case LV_SIGNAL_RELEASED:
            case LV_SIGNAL_LONG_PRESS_REP:
    			if(ext->cb != NULL &&
    			   ext->btn_pr != LV_BTNM_BTN_PR_INVALID) {
    				uint16_t txt_i = 0;
    				uint16_t btn_i = 0;
    				/*Search the next valid text in the map*/
    				while(btn_i != ext->btn_pr) {
    					btn_i ++;
    					txt_i ++;
    					if(strcmp(ext->map_p[txt_i], "\n") == 0) txt_i ++;
    				}

    				ext->cb(btnm, txt_i);
    			}
    			if(sign == LV_SIGNAL_RELEASED) ext->btn_pr = LV_BTNM_BTN_PR_INVALID;
				lv_obj_inv(btnm);
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
 * Set a new map. Buttons will be created/deleted according to the map.
 * @param btnm pointer to a button matrix object
 * @param map pointer a string array. The last string has to be: "".
 *            Use "\n" to begin a new line.
 *            Use octal numbers (e.g. "\003") to set the relative
 *            width of a button. (max. 9 -> \011)
 *            (e.g. const char * str[] = {"a", "b", "\n", "\004c", "d", ""}).
 *            The button do not copy the array so it can not be a local variable.
 */
void lv_btnm_set_map(lv_obj_t * btnm, const char ** map)
{
	if(map == NULL) return;

	LV_EA(btnm, lv_btnm_ext_t)->map_p = map;

	/*Analyze the map and create the required number of buttons*/
	lv_btnm_create_btns(btnm, map);

	/*Set size and positions of the buttons*/
	lv_btnms_t * btnms = lv_obj_get_style(btnm);
	cord_t max_w = lv_obj_get_width(btnm) - 2 * btnms->rects.hpad;
	cord_t max_h = lv_obj_get_height(btnm) - 2 * btnms->rects.vpad;
	cord_t act_y = btnms->rects.vpad;

	/*Count the lines to calculate button height*/
	uint8_t line_cnt = 1;
	uint8_t li;
	for(li = 0; strlen(map[li]) != 0; li++) {
			if(strcmp(map[li], "\n") == 0) line_cnt ++;
	}

	cord_t btn_h = max_h - ((line_cnt - 1) * btnms->rects.opad);
	btn_h = btn_h / line_cnt;

	/* Count the units and the buttons in a line
	 * (A button can be 1,2,3... unit wide)*/
	uint16_t unit_cnt;
	uint16_t btn_cnt;		/*Number of buttons in a row*/
	uint16_t i_tot = 0;		/*Act. index in the str map*/
	uint16_t btn_i = 0;		/*Act. index of button areas*/
	const char  ** map_p_tmp = map;
	lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);

	/*Count the units and the buttons in a line*/
	while(1) {
		unit_cnt = 0;
		btn_cnt = 0;
		/*Count the buttons in a line*/
		while(strcmp(map_p_tmp[btn_cnt], "\n") != 0 &&
			  strlen(map_p_tmp[btn_cnt]) != 0) { /*Check a line*/
			unit_cnt += lv_btnm_get_width_unit(map_p_tmp[btn_cnt]);
			btn_cnt ++;
		}

		/*Only deal with the non empty lines*/
		if(btn_cnt != 0) {
			/*Calculate the width of all units*/
			cord_t all_unit_w = max_w - ((btn_cnt-1) * btnms->rects.opad);

			/*Set the button size and positions and set the texts*/
			uint16_t i;
			cord_t act_x = btnms->rects.hpad;
			cord_t act_unit_w;
			for(i = 0; i < btn_cnt; i++) {
				/* one_unit_w = all_unit_w / unit_cnt
				 * act_unit_w = one_unit_w * button_width
				 * do this two operation but the multiplications first to divide a greater number */
				act_unit_w = (all_unit_w * lv_btnm_get_width_unit(map_p_tmp[i])) / unit_cnt;
				area_set(&ext->btn_areas[btn_i], act_x,
						                         act_y,
						                         act_x + act_unit_w,
				                                 act_y + btn_h);

				act_x += act_unit_w + btnms->rects.opad;

				i_tot ++;
				btn_i ++;
			}
		}
		act_y += btn_h + btnms->rects.opad;
		if(strlen(map_p_tmp[btn_cnt]) == 0) break; /*Break on end of map*/
		map_p_tmp = &map_p_tmp[btn_cnt + 1]; /*Set the map to the next line*/
		i_tot ++;	/*Skip the '\n'*/
	}

	lv_obj_inv(btnm);

}

/**
 * Set a new callback function for the buttons (It will be called when a button is released)
 * @param btnm: pointer to button matrix object
 * @param cb pointer to a callback function
 */
void lv_btnm_set_cb(lv_obj_t * btnm, lv_btnm_callback_t cb)
{
	LV_EA(btnm, lv_btnm_ext_t)->cb = cb;
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current map of a button matrix
 * @param btnm pointer to a button matrix object
 * @return the current map
 */
const char ** lv_btnm_get_map(lv_obj_t * btnm)
{
	return LV_EA(btnm, lv_btnm_ext_t)->map_p;
}

/**
 * Get a the callback function of the buttons on a button matrix
 * @param btnm: pointer to button matrix object
 * @return pointer to the callback function
 */
lv_btnm_callback_t lv_btnm_get_cb(lv_obj_t * btnm)
{
	return LV_EA(btnm, lv_btnm_ext_t)->cb;
}


/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_btnms_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_btnms_t style
 */
lv_btnms_t * lv_btnms_get(lv_btnms_builtin_t style, lv_btnms_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_btnms_init();
		style_inited = true;
	}

	lv_btnms_t  *style_p;

	switch(style) {
		case LV_BTNMS_DEF:
			style_p = &lv_btnms_def;
			break;
		default:
			style_p = &lv_btnms_def;
	}

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_btnms_t));
		else memcpy(copy, &lv_btnms_def, sizeof(lv_btnms_t));
	}

	return style_p;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the button matrixs
 * @param btnm pointer to a button matrix object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_btnm_design(lv_obj_t * btnm, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_design_f(btnm, mask, mode);
    	/*Return false if the object is not covers the mask_p area*/
    }
    /*Draw the object*/
    else if (mode == LV_DESIGN_DRAW_MAIN) {
    	ancestor_design_f(btnm, mask, mode);

    	lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);
    	lv_btnms_t * style = lv_obj_get_style(btnm);

    	area_t area_btnm;
    	area_t area_tmp;
    	cord_t btn_w;
    	cord_t btn_h;

    	uint16_t btn_i = 0;
    	uint16_t txt_i = 0;
    	for(btn_i = 0; btn_i < ext->btn_cnt; btn_i ++) {
			lv_obj_get_cords(btnm, &area_btnm);

			area_cpy(&area_tmp, &ext->btn_areas[btn_i]);
			area_tmp.x1 += area_btnm.x1;
			area_tmp.y1 += area_btnm.y1;
			area_tmp.x2 += area_btnm.x1;
			area_tmp.y2 += area_btnm.y1;

			btn_w = area_get_width(&area_tmp);
			btn_h = area_get_height(&area_tmp);

			/*Load the style*/
			lv_rects_t new_rects;
			lv_btn_state_t state;
			state = ext->btn_pr == btn_i ? LV_BTN_STATE_PR : LV_BTN_STATE_REL;
			memcpy(&new_rects, &style->rects, sizeof(lv_rects_t));
			new_rects.objs.color = style->btns.mcolor[state];
			new_rects.gcolor = style->btns.gcolor[state];
			new_rects.bcolor = style->btns.bcolor[state];
			new_rects.lcolor = style->btns.lcolor[state];
			new_rects.empty = style->btns.flags[state].empty;
			new_rects.objs.transp = style->btns.flags[state].transp;

			if(style->btns.flags[state].light_en != 0) new_rects.light = style->rects.light;
			else new_rects.light = 0;

			lv_draw_rect(&area_tmp, mask, &new_rects, OPA_COVER);

			/*Search the next valid text in the map*/
			while(strcmp(ext->map_p[txt_i], "\n") == 0) txt_i ++;

			/*Calculate the size of the text*/
			const font_t * font = font_get(style->labels.font);
			point_t txt_size;
			txt_get_size(&txt_size, ext->map_p[txt_i], font,
					     style->labels.letter_space, style->labels.line_space, area_get_width(&area_btnm));

			area_tmp.x1 += (btn_w - txt_size.x) / 2;
			area_tmp.y1 += (btn_h - txt_size.y) / 2;
			area_tmp.x2 = area_tmp.x1 + txt_size.x;
			area_tmp.y2 = area_tmp.y1 + txt_size.y;

			lv_draw_label(&area_tmp, mask, &style->labels, OPA_COVER, ext->map_p[txt_i]);
			txt_i ++;
    	}
    }


    return true;
}


/**
 * Initialize the button matrix styles
 */
static void lv_btnms_init(void)
{
	/*Default style*/
	lv_rects_get(LV_RECTS_DEF, &lv_btnms_def.rects);	 /*Background rectangle style*/
	lv_btns_get(LV_BTNS_DEF, &lv_btnms_def.btns);	 	 /*Button style*/
	lv_labels_get(LV_LABELS_BTN, &lv_btnms_def.labels);	 /*BUtton label style*/
}

/**
 * Create the required number of buttons according to a map
 * @param btnm pointer to button matrix object
 * @param map_p pointer to a string array
 */
static void lv_btnm_create_btns(lv_obj_t * btnm, const char ** map)
{
	/*Count the buttons in the map*/
	uint16_t btn_cnt = 0;
	uint16_t i = 0;
	while(strlen(map[i]) != 0) {
			if(strcmp(map[i], "\n") != 0) { /*Do not count line breaks*/
			btn_cnt ++;
		}
		i++;
	}

	lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);

	if(ext->btn_areas != NULL) {
		dm_free(ext->btn_areas);
	}

	ext->btn_areas = dm_alloc(sizeof(area_t) * btn_cnt);
	ext->btn_cnt = btn_cnt;
}

/**
 * Get the width of a button in units. It comes from the first "letter".
 * @param btn_str The descriptor string of a button. E.g. "apple" or "\004banana"
 * @return the width of the button in units
 */
static uint8_t lv_btnm_get_width_unit(const char * btn_str)
{
	if(btn_str[0] <= '\011') return btn_str[0];

	return 1;

}


#endif
