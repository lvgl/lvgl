/**
 * @file lv_cb.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_CB != 0

#include "lv_cb.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if 0 /*The ancestor design function is used */
static bool lv_cb_design(lv_obj_t * cb, const area_t * mask, lv_design_mode_t mode);
#endif
static void lv_cbs_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_cbs_t lv_cbs_def =
{
};

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
 * Create a check box objects
 * @param par pointer to an object, it will be the parent of the new check box
 * @param copy pointer to a check box object, if not NULL then the new object will be copied from it
 * @return pointer to the created check box
 */
lv_obj_t * lv_cb_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor basic object*/
    lv_obj_t * new_cb = lv_btn_create(par, copy);
    dm_assert(new_cb);
    
    lv_cb_ext_t * ext = lv_obj_alloc_ext(new_cb, sizeof(lv_cb_ext_t));
    dm_assert(ext);

    lv_obj_set_signal_f(new_cb, lv_cb_signal);

    /*Init the new checkbox object*/
    if(copy == NULL) {
        lv_rect_set_layout(new_cb, LV_RECT_LAYOUT_ROW_M);
        lv_rect_set_fit(new_cb, true, true);
        lv_btn_set_tgl(new_cb, true);

        ext->bullet = lv_btn_create(new_cb, NULL);
        lv_obj_set_click(ext->bullet, false);

        ext->label = lv_label_create(new_cb, NULL);
        lv_label_set_text(ext->label, "Check box");

        lv_obj_set_style(new_cb, lv_cbs_get(LV_CBS_DEF, NULL));
    } else {
    	lv_cb_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->bullet = lv_btn_create(new_cb, copy_ext->bullet);
    	ext->label = lv_label_create(new_cb, copy_ext->label);
    }

    lv_obj_align_us(new_cb, NULL, LV_ALIGN_CENTER, 0, 0);
    
    return new_cb;
}


/**
 * Signal function of the check box
 * @param cb pointer to a check box object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_cb_signal(lv_obj_t * cb, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_btn_signal(cb, sign, param);

    lv_cb_ext_t * ext = lv_obj_get_ext(cb);
    lv_cbs_t * cbs = lv_obj_get_style(cb);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	switch(sign) {
    	case LV_SIGNAL_PRESSED:
    	case LV_SIGNAL_RELEASED:
    	case LV_SIGNAL_LONG_PRESS:
    	case LV_SIGNAL_PRESS_LOST:
    		lv_btn_set_state(ext->bullet, lv_btn_get_state(cb));
    		break;
    	case LV_SIGNAL_STYLE_CHG:
    		lv_obj_set_size(ext->bullet, cbs->bullet_size, cbs->bullet_size);
    		lv_obj_set_style(ext->bullet, &cbs->bullet);
    		lv_obj_set_style(ext->label, &cbs->label);
    		break;
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

/**
 * Set the text of a check box
 * @param cb pointer to a check box
 * @param txt the text of the check box
 */
void lv_cb_set_text(lv_obj_t * cb, const char * txt)
{
	lv_cb_ext_t * ext = lv_obj_get_ext(cb);
	lv_label_set_text(ext->label, txt);
}


/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a check box
 * @param cb pointer to check box object
 * @return pointer to the text of the check box
 */
const char * lv_cb_get_text(lv_obj_t * cb)
{
	lv_cb_ext_t * ext = lv_obj_get_ext(cb);
	return lv_label_get_text(ext->label);
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_cbs_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_cbs_t style
 */
lv_cbs_t * lv_cbs_get(lv_cbs_builtin_t style, lv_cbs_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_cbs_init();
		style_inited = true;
	}

	lv_cbs_t  *style_p;

	switch(style) {
		case LV_CBS_DEF:
			style_p = &lv_cbs_def;
			break;
		default:
			style_p = &lv_cbs_def;
	}

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_cbs_t));
		else memcpy(copy, &lv_cbs_def, sizeof(lv_cbs_t));
	}

	return style_p;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

#if 0 /*The ancestor design function is used */
/**
 * Handle the drawing related tasks of the check boxes
 * @param cb pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_cb_design(lv_obj_t * cb, const area_t * mask, lv_design_mode_t mode)
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
 * Initialize the rectangle styles
 */
static void lv_cbs_init(void)
{
	/*Default style*/

	/*Bg style*/
	lv_btns_get(LV_RECTS_TRANSP, &lv_cbs_def.bg);
	lv_cbs_def.bg.rects.hpad = 0 * LV_STYLE_MULT;
	lv_cbs_def.bg.rects.vpad = 0 * LV_STYLE_MULT;
	lv_cbs_def.bg.rects.opad = 5 * LV_STYLE_MULT;

	/*Bullet style*/
	lv_btns_get(LV_BTNS_DEF, &lv_cbs_def.bullet);

	lv_cbs_def.bullet.mcolor[LV_BTN_STATE_REL] = COLOR_WHITE;
	lv_cbs_def.bullet.gcolor[LV_BTN_STATE_REL] = COLOR_SILVER;
	lv_cbs_def.bullet.bcolor[LV_BTN_STATE_REL] = COLOR_BLACK;

	lv_cbs_def.bullet.mcolor[LV_BTN_STATE_PR] = COLOR_SILVER;
	lv_cbs_def.bullet.gcolor[LV_BTN_STATE_PR] = COLOR_GRAY;
	lv_cbs_def.bullet.bcolor[LV_BTN_STATE_PR] = COLOR_BLACK;

	lv_cbs_def.bullet.mcolor[LV_BTN_STATE_TGL_REL] = COLOR_MAKE(0x20, 0x30, 0x40);
	lv_cbs_def.bullet.gcolor[LV_BTN_STATE_TGL_REL] = COLOR_MAKE(0x10, 0x20, 0x30);
	lv_cbs_def.bullet.bcolor[LV_BTN_STATE_TGL_REL] = COLOR_WHITE;

	lv_cbs_def.bullet.mcolor[LV_BTN_STATE_TGL_PR] = COLOR_MAKE(0x50, 0x70, 0x90);
	lv_cbs_def.bullet.gcolor[LV_BTN_STATE_TGL_PR] = COLOR_MAKE(0x20, 0x30, 0x40);
	lv_cbs_def.bullet.bcolor[LV_BTN_STATE_TGL_PR] = COLOR_WHITE;

	lv_cbs_def.bullet.mcolor[LV_BTN_STATE_INA] = COLOR_SILVER;
	lv_cbs_def.bullet.gcolor[LV_BTN_STATE_INA] = COLOR_GRAY;
	lv_cbs_def.bullet.bcolor[LV_BTN_STATE_INA] = COLOR_WHITE;

	lv_cbs_def.bullet.rects.bwidth = 2 * LV_STYLE_MULT;
	lv_cbs_def.bullet.rects.bopa = 70;
	lv_cbs_def.bullet.rects.empty = 0;
	lv_cbs_def.bullet.rects.round = LV_OBJ_DEF_WIDTH / 3 / 4;
	lv_cbs_def.bullet.rects.hpad = 0 * LV_STYLE_MULT;
	lv_cbs_def.bullet.rects.vpad = 0 * LV_STYLE_MULT;
	lv_cbs_def.bullet.rects.opad = 0 * LV_STYLE_MULT;

	/*Label*/
	lv_labels_get(LV_LABELS_TXT, &lv_cbs_def.label);
	lv_cbs_def.label.objs.color = COLOR_MAKE(0x10, 0x18, 0x20);

	/*Others*/
	lv_cbs_def.bullet_size = LV_OBJ_DEF_WIDTH / 3;
}
#endif
