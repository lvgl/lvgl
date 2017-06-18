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
#include "misc/gfx/text.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_obj/lv_refr.h"

/*********************
 *      DEFINES
 *********************/
#define LV_BTNM_PR_NONE 0xFFFF

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_btnm_design(lv_obj_t * btnm, const area_t * mask, lv_design_mode_t mode);
static uint8_t lv_btnm_get_width_unit(const char * btn_str);
static uint16_t lv_btnm_get_btn_from_point(lv_obj_t * btnm, point_t * p);
static void lv_btnm_create_btns(lv_obj_t * btnm, const char ** map);

/**********************
 *  STATIC VARIABLES
 **********************/
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
    lv_obj_t * new_btnm = lv_cont_create(par, copy);
    dm_assert(new_btnm);
    
    /*Allocate the object type specific extended data*/
    lv_btnm_ext_t * ext = lv_obj_alloc_ext(new_btnm, sizeof(lv_btnm_ext_t));
    dm_assert(ext);
    ext->btn_cnt = 0;
    ext->btn_pr = LV_BTNM_PR_NONE;
    ext->btn_areas = NULL;
    ext->cb = NULL;
    ext->map_p = NULL;
    ext->style_btn_rel = lv_style_get(LV_STYLE_BTN_REL, NULL);
    ext->style_btn_pr =  lv_style_get(LV_STYLE_BTN_PR, NULL);

    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_btnm);

    lv_obj_set_signal_f(new_btnm, lv_btnm_signal);
    lv_obj_set_design_f(new_btnm, lv_btnm_design);

    /*Init the new button matrix object*/
    if(copy == NULL) {
    	lv_obj_set_size(new_btnm, LV_HOR_RES, LV_VER_RES / 2);
    	lv_obj_set_style(new_btnm, lv_style_get(LV_STYLE_PLAIN, NULL));
    	lv_btnm_set_map(new_btnm, lv_btnm_def_map);
    }
    /*Copy an existing object*/
    else {
        lv_btnm_ext_t * copy_ext = lv_obj_get_ext(copy);
        ext->style_btn_rel = copy_ext->style_btn_rel;
        ext->style_btn_pr = copy_ext->style_btn_pr;
        lv_btnm_set_map(new_btnm, lv_btnm_get_map(copy));
        ext->cb = copy_ext->cb;
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
    valid = lv_cont_signal(btnm, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);
    	area_t btnm_area;
        area_t btn_area;
    	point_t p;
    	if(sign == LV_SIGNAL_CLEANUP) {
            dm_free(ext->btn_areas);
    	}
    	else if(sign == LV_SIGNAL_STYLE_CHG || sign == LV_SIGNAL_CORD_CHG) {
            lv_btnm_set_map(btnm, ext->map_p);
    	}
    	else if(sign == LV_SIGNAL_PRESSING) {
            uint16_t btn_pr;
            /*Search the pressed area*/
            lv_dispi_get_point(param, &p);
            btn_pr = lv_btnm_get_btn_from_point(btnm, &p);
            /*Invalidate to old and the new areas*/;
            lv_obj_get_cords(btnm, &btnm_area);
            if(btn_pr != ext->btn_pr) {
                lv_dispi_reset_lpr(param);
                if(ext->btn_pr != LV_BTNM_PR_NONE) {
                    area_cpy(&btn_area, &ext->btn_areas[ext->btn_pr]);
                    btn_area.x1 += btnm_area.x1;
                    btn_area.y1 += btnm_area.y1;
                    btn_area.x2 += btnm_area.x1;
                    btn_area.y2 += btnm_area.y1;
                    lv_inv_area(&btn_area);
                }
                if(btn_pr != LV_BTNM_PR_NONE) {
                    area_cpy(&btn_area, &ext->btn_areas[btn_pr]);
                    btn_area.x1 += btnm_area.x1;
                    btn_area.y1 += btnm_area.y1;
                    btn_area.x2 += btnm_area.x1;
                    btn_area.y2 += btnm_area.y1;
                    lv_inv_area(&btn_area);
                }
            }

            ext->btn_pr = btn_pr;
    	}
    	else if(sign ==  LV_SIGNAL_RELEASED || sign == LV_SIGNAL_LONG_PRESS_REP) {
            if(ext->cb != NULL && ext->btn_pr != LV_BTNM_PR_NONE) {
                uint16_t txt_i = 0;
                uint16_t btn_i = 0;

                /* Search the text of ext->btn_pr the buttons text in the map
                 * Skip "\n"-s*/
                while(btn_i != ext->btn_pr) {
                    btn_i ++;
                    txt_i ++;
                    if(strcmp(ext->map_p[txt_i], "\n") == 0) txt_i ++;
                }

                ext->cb(btnm, txt_i);
            }

            if(sign == LV_SIGNAL_RELEASED && ext->btn_pr != LV_BTNM_PR_NONE) {
                /*Invalidate to old area*/;
                lv_obj_get_cords(btnm, &btnm_area);
                area_cpy(&btn_area, &ext->btn_areas[ext->btn_pr]);
                btn_area.x1 += btnm_area.x1;
                btn_area.y1 += btnm_area.y1;
                btn_area.x2 += btnm_area.x1;
                btn_area.y2 += btnm_area.y1;
                lv_inv_area(&btn_area);

                ext->btn_pr = LV_BTNM_PR_NONE;
            }
    	}
        else if(sign == LV_SIGNAL_PRESS_LOST) {
            ext->btn_pr = LV_BTNM_PR_NONE;
            lv_obj_inv(btnm);

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

	lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);
	ext->map_p = map;

	/*Analyze the map and create the required number of buttons*/
	lv_btnm_create_btns(btnm, map);

	/*Set size and positions of the buttons*/
	lv_style_t * btnms = lv_obj_get_style(btnm);
	cord_t max_w = lv_obj_get_width(btnm) - 2 * btnms->hpad;
	cord_t max_h = lv_obj_get_height(btnm) - 2 * btnms->vpad;
	cord_t act_y = btnms->vpad;

	/*Count the lines to calculate button height*/
	uint8_t line_cnt = 1;
	uint8_t li;
	for(li = 0; strlen(map[li]) != 0; li++) {
			if(strcmp(map[li], "\n") == 0) line_cnt ++;
	}

	cord_t btn_h = max_h - ((line_cnt - 1) * btnms->opad);
	btn_h = btn_h / line_cnt;

	/* Count the units and the buttons in a line
	 * (A button can be 1,2,3... unit wide)*/
	uint16_t unit_cnt;      /*Number of units in a row*/
    uint16_t unit_act_cnt;  /*Number of units currently put in a row*/
	uint16_t btn_cnt;		/*Number of buttons in a row*/
	uint16_t i_tot = 0;		/*Act. index in the str map*/
	uint16_t btn_i = 0;		/*Act. index of button areas*/
	const char  ** map_p_tmp = map;

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
			cord_t all_unit_w = max_w - ((btn_cnt-1) * btnms->opad);

			/*Set the button size and positions and set the texts*/
			uint16_t i;
			cord_t act_x = btnms->hpad;
			cord_t act_unit_w;
			unit_act_cnt = 0;
			for(i = 0; i < btn_cnt; i++) {
				/* one_unit_w = all_unit_w / unit_cnt
				 * act_unit_w = one_unit_w * button_width
				 * do this two operation but the multiply first to divide a greater number */
				act_unit_w = (all_unit_w * lv_btnm_get_width_unit(map_p_tmp[i])) / unit_cnt;

				/*Always recalculate act_x because of rounding errors */
				act_x = (unit_act_cnt * all_unit_w) / unit_cnt + i * btnms->opad + btnms->hpad;

				area_set(&ext->btn_areas[btn_i], act_x,
						                         act_y,
						                         act_x + act_unit_w,
				                                 act_y + btn_h);

				unit_act_cnt += lv_btnm_get_width_unit(map_p_tmp[i]);

				i_tot ++;
				btn_i ++;
			}
		}
		act_y += btn_h + btnms->opad;
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
void lv_btnm_set_action(lv_obj_t * btnm, lv_btnm_callback_t cb)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);
	ext->cb = cb;
}

/**
 * Set the styles of the buttons of the button matrox
 * @param btnm pointer to a button matrix object
 * @param state style in this state (LV_BTN_STATE_PR or LV_BTN_STATE_REL)
 * @param style pointer to style
 */
void lv_btnm_set_styles_btn(lv_obj_t * btnm, lv_style_t *  rel, lv_style_t *  pr)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);
    ext->style_btn_rel = rel;
    ext->style_btn_pr = pr;

    lv_obj_inv(btnm);

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
    lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);
	return ext->map_p;
}

/**
 * Get a the callback function of the buttons on a button matrix
 * @param btnm: pointer to button matrix object
 * @return pointer to the callback function
 */
lv_btnm_callback_t lv_btnm_get_action(lv_obj_t * btnm)
{
    lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);
	return ext->cb;
}

/**
 * Get the style of buttons in button matrix
 * @param btnm pointer to a button matrix object
 * @param state style in this state (LV_BTN_STATE_PR or LV_BTN_STATE_REL)
 * @return pointer the button style in the given state
 */
lv_style_t * lv_btnm_get_style_btn(lv_obj_t * btnm, lv_btn_state_t state)
{
    lv_style_t * style;
    lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);

    switch(state) {
        case LV_BTN_STATE_PR:
            style = ext->style_btn_pr;
            break;
        case LV_BTN_STATE_REL:
            style = ext->style_btn_rel;
            break;
        default:
            style = NULL;
    }

    if(style == NULL) style = lv_obj_get_style(btnm);

    return style;
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
        lv_style_t * btn_style;

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
			btn_style = lv_btnm_get_style_btn(btnm, ext->btn_pr == btn_i ? LV_BTN_STATE_PR : LV_BTN_STATE_REL);

			lv_draw_rect(&area_tmp, mask, btn_style);

			/*Search the next valid text in the map*/
			while(strcmp(ext->map_p[txt_i], "\n") == 0) txt_i ++;

			/*Calculate the size of the text*/
			const font_t * font = btn_style->font;
			point_t txt_size;
			txt_get_size(&txt_size, ext->map_p[txt_i], font,
			             btn_style->letter_space, btn_style->line_space,
					     area_get_width(&area_btnm), TXT_FLAG_NONE);

			area_tmp.x1 += (btn_w - txt_size.x) / 2;
			area_tmp.y1 += (btn_h - txt_size.y) / 2;
			area_tmp.x2 = area_tmp.x1 + txt_size.x;
			area_tmp.y2 = area_tmp.y1 + txt_size.y;

			lv_draw_label(&area_tmp, mask, btn_style, ext->map_p[txt_i], TXT_FLAG_NONE, NULL);
			txt_i ++;
    	}
    }


    return true;
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
		ext->btn_areas = NULL;
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

static uint16_t lv_btnm_get_btn_from_point(lv_obj_t * btnm, point_t * p)
{
    area_t btnm_cords;
    area_t btn_area;
    lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);
    uint16_t i;
    lv_obj_get_cords(btnm, &btnm_cords);

    for(i = 0; i < ext->btn_cnt; i++) {
        area_cpy(&btn_area, &ext->btn_areas[i]);
        btn_area.x1 += btnm_cords.x1;
        btn_area.y1 += btnm_cords.y1;
        btn_area.x2 += btnm_cords.x1;
        btn_area.y2 += btnm_cords.y1;
        if(area_is_point_on(&btn_area, p) != false) {
            break;
        }
    }

    if(i == ext->btn_cnt) i = LV_BTNM_PR_NONE;

    return i;
}


#endif
