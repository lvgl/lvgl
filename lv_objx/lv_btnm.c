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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if 0 /*Not necessary*/
static bool lv_btnm_design(lv_obj_t * btnm, const area_t * mask, lv_design_mode_t mode);
#endif
static uint8_t lv_btnm_get_width_unit(const char * btn_str);
static void lv_btnm_create_btns(lv_obj_t * btnm, const char ** map);
static bool lv_btnm_btn_release_action(lv_obj_t * btnm, lv_dispi_t * dispi);
static void lv_btnms_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_btnms_t lv_btnms_def;

static const char * lv_btnm_def_map[] = {"Btn1","Btn2", "Btn3","\n",
										 "\002Btn4","Btn5", ""};

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

    lv_obj_set_signal_f(new_btnm, lv_btnm_signal);

    /* Keep the rectangle design function
     * lv_obj_set_design_f(new_obj, lv_btnm_design); */

    /*Init the new button matrix object*/
    if(copy == NULL) {
    	lv_obj_set_size(new_btnm, LV_HOR_RES / 2, LV_VER_RES / 2);
    	lv_obj_set_style(new_btnm, lv_btnms_get(LV_BTNMS_DEF, NULL));
    	lv_btnm_set_map(new_btnm, lv_btnm_def_map);
    }
    /*Copy an existing object*/
    else {

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
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			/*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    			break;
    		case LV_SIGNAL_STYLE_CHG:
    		case LV_SIGNAL_CORD_CHG:
    			lv_btnm_set_map(btnm, LV_EA(btnm, lv_btnm_ext_t)->map_p);
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
 * @param map pointer a string array. Tha last string hast be: "".
 *            Use "\n" to begin a new line.
 *            Use "\003" octal numbers to set relative
 *            the width of a button. (max. 9 -> \011)
 *            (e.g. const char * str[] = {"a", "b", "\n", "\004c", "d", ""}).
 */
void lv_btnm_set_map(lv_obj_t * btnm, const char ** map)
{
	if(map == NULL) return;

	LV_EA(btnm, lv_btnm_ext_t)->map_p = map;

	/*Analyze the map and create the required number of buttons*/
	lv_btnm_create_btns(btnm, map);

	/*Set size and positions of the buttons*/
	lv_btnms_t * btnms = lv_obj_get_style(btnm);
	cord_t max_w = lv_obj_get_width(btnm) - 2 * btnms->bg.hpad;
	cord_t max_h = lv_obj_get_height(btnm) - 2 * btnms->bg.vpad;
	cord_t act_y = btnms->bg.vpad;
	uint8_t btn_cnt_tot = 0;	/*Used to set free number of the buttons*/

	/*Count the lines to calculate button height*/
	uint8_t line_cnt = 1;
	uint8_t li;
	for(li = 0; strlen(map[li]) != 0; li++) {
			if(strcmp(map[li], "\n") == 0) line_cnt ++;
	}

	cord_t btn_h = max_h - ((line_cnt - 1) * btnms->bg.opad);
	btn_h = btn_h / line_cnt;

	/* Count the units and the buttons in a line
	 * (A button can be 1,2,3... unit wide)*/
	uint16_t unit_cnt;
	uint16_t btn_cnt;
	const char  ** map_p_tmp = map;
	lv_obj_t * btn;
	btn = ll_get_head(&btnm->child_ll);

	/*Count the units and the buttons in a line*/
	while(1) {
		unit_cnt = 0;
		btn_cnt = 0;
		while(strcmp(map_p_tmp[btn_cnt], "\n") != 0 &&
			  strlen(map_p_tmp[btn_cnt]) != 0) { /*Check a line*/
			unit_cnt += lv_btnm_get_width_unit(map_p_tmp[btn_cnt]);
			btn_cnt ++;
		}

		/*Only deal with the non empty lines*/
		if(btn_cnt != 0) {
			/*Calculate the unit width*/
			cord_t unit_w = max_w - ((btn_cnt-1) * btnms->bg.opad);
			unit_w = unit_w / unit_cnt;

			/*Set the button size and positions and set the texts*/
			uint16_t i;
			lv_obj_t * label;
			cord_t act_x = btnms->bg.hpad;
			for(i = 0; i < btn_cnt; i++) {
				lv_obj_set_size(btn, unit_w * lv_btnm_get_width_unit(map_p_tmp[i]), btn_h);
				lv_obj_align(btn, NULL, LV_ALIGN_IN_TOP_LEFT, act_x, act_y);
				lv_obj_set_free_num(btn, btn_cnt_tot);
				lv_obj_set_style(btn, &btnms->btn);
				act_x += lv_obj_get_width(btn) + btnms->bg.opad;

				label = lv_obj_get_child(btn, NULL); /*Get the label on the button (the only child)*/
				lv_obj_set_style(label, &btnms->btn_label);
				lv_label_set_text(label, map_p_tmp[i]);

				btn = ll_get_next(&btnm->child_ll, btn); /*Go to the next button*/
				btn_cnt_tot ++;
			}
		}
		act_y += btn_h + btnms->bg.opad;
		if(strlen(map_p_tmp[btn_cnt]) == 0) break; /*Break on end of map*/
		map_p_tmp = &map_p_tmp[btn_cnt + 1]; /*Set the map to the next line*/
	}

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

#if 0 /*Not necessary*/
/**
 * Handle the drawing related tasks of the button matrixs
 * @param obj pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_btnm_design(lv_obj_t * obj, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return false;
    }

    /*Draw the object*/

    return true;
}
#endif


/**
 * Initialize the button matrix styles
 */
static void lv_btnms_init(void)
{
	/*Default style*/
	lv_rects_get(LV_RECTS_DEF, &lv_btnms_def.bg);	 /*Background rectangle style*/
	lv_btns_get(LV_BTNS_DEF, &lv_btnms_def.btn);	 	 /*Button style*/
	lv_labels_get(LV_LABELS_BTN, &lv_btnms_def.btn_label);	 /*BUtton label style*/
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
			if(strcmp(map[i], "\n") != 0) { /*Not count line breaks*/
			btn_cnt ++;
		}
		i++;
	}

	/*Get the current number of children of the button matrix*/
	uint16_t child_cnt = 0;
	lv_obj_t * child = NULL;
	while(1) {
		child = lv_obj_get_child(btnm, child);
		if(child != NULL) child_cnt ++;
		else break;
	}

	/*Create or delete buttons to finally get 'btn_cnt' children*/
	if(child_cnt < btn_cnt) { 	/*Create buttons*/
		for(i = 0; i < btn_cnt - child_cnt; i++) {
			lv_obj_t * btn = lv_btn_create(btnm, NULL);
			lv_btn_set_rel_action(btn, lv_btnm_btn_release_action);
			lv_label_create(btn, NULL);
		}
	} else if(child_cnt > btn_cnt) { /*Delete buttons*/
		for(i = 0; i < child_cnt - btn_cnt; i++) {
			lv_obj_del(lv_obj_get_child(btnm, NULL));
		}
	}
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


/**
 * Called when button is released
 * @param btn pointer to the released button of the button matrix
 * @param dispi pointer to the caller display input.
 * @return true: if the button remains valid (the button matrix or the button is not deleted)
 */
static bool lv_btnm_btn_release_action(lv_obj_t * btn, lv_dispi_t * dispi)
{
	lv_obj_t * btnm = lv_obj_get_parent(btn);
	lv_btnm_ext_t * ext = lv_obj_get_ext(btnm);
	uint8_t id = lv_obj_get_free_num(btn);
	bool ret;
	if(ext->cb != NULL) {
		ret = ext->cb(btnm, btn, id); /*Call the set callback function*/
	}

	return ret;
}

#endif
