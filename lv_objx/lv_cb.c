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
static bool lv_cb_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_cbs_t lv_cbs_def =
{ 		/*Bg style*/
		.bg.rects.objs.transp = 1,
		.bg.mcolor[LV_BTN_STATE_REL] = COLOR_SILVER,
		.bg.gcolor[LV_BTN_STATE_REL] = COLOR_SILVER,
		.bg.bcolor[LV_BTN_STATE_REL] = COLOR_SILVER,

		.bg.mcolor[LV_BTN_STATE_PR] = COLOR_SILVER,
		.bg.gcolor[LV_BTN_STATE_PR] = COLOR_SILVER,
		.bg.bcolor[LV_BTN_STATE_PR] = COLOR_SILVER,

		.bg.mcolor[LV_BTN_STATE_TGL_REL] = COLOR_SILVER,
		.bg.gcolor[LV_BTN_STATE_TGL_REL] = COLOR_SILVER,
		.bg.bcolor[LV_BTN_STATE_TGL_REL] = COLOR_SILVER,

		.bg.mcolor[LV_BTN_STATE_TGL_PR] = COLOR_SILVER,
		.bg.gcolor[LV_BTN_STATE_TGL_PR] = COLOR_SILVER,
		.bg.bcolor[LV_BTN_STATE_TGL_PR] = COLOR_SILVER,

		.bg.mcolor[LV_BTN_STATE_INA] = COLOR_SILVER,
		.bg.gcolor[LV_BTN_STATE_INA] = COLOR_SILVER,
		.bg.bcolor[LV_BTN_STATE_INA] = COLOR_SILVER,

		.bg.rects.bwidth = 0 * LV_STYLE_MULT,
		.bg.rects.bopa = 50,
		.bg.rects.empty = 1,
		.bg.rects.round = 0,
		.bg.rects.hpad = 0 * LV_STYLE_MULT,
		.bg.rects.vpad = 0 * LV_STYLE_MULT,
		.bg.rects.opad = 5 * LV_STYLE_MULT,

		/*Bullet style*/
		.bullet.mcolor[LV_BTN_STATE_REL] = COLOR_WHITE,
		.bullet.gcolor[LV_BTN_STATE_REL] = COLOR_SILVER,
		.bullet.bcolor[LV_BTN_STATE_REL] = COLOR_BLACK,

		.bullet.mcolor[LV_BTN_STATE_PR] = COLOR_SILVER,
		.bullet.gcolor[LV_BTN_STATE_PR] = COLOR_GRAY,
		.bullet.bcolor[LV_BTN_STATE_PR] = COLOR_BLACK,

		.bullet.mcolor[LV_BTN_STATE_TGL_REL] = COLOR_MAKE(0x20, 0x30, 0x40),
		.bullet.gcolor[LV_BTN_STATE_TGL_REL] = COLOR_MAKE(0x10, 0x20, 0x30),
		.bullet.bcolor[LV_BTN_STATE_TGL_REL] = COLOR_WHITE,

		.bullet.mcolor[LV_BTN_STATE_TGL_PR] = COLOR_MAKE(0x50, 0x70, 0x90),
		.bullet.gcolor[LV_BTN_STATE_TGL_PR] = COLOR_MAKE(0x20, 0x30, 0x40),
		.bullet.bcolor[LV_BTN_STATE_TGL_PR] = COLOR_WHITE,

		.bullet.mcolor[LV_BTN_STATE_INA] = COLOR_SILVER,
		.bullet.gcolor[LV_BTN_STATE_INA] = COLOR_GRAY,
		.bullet.bcolor[LV_BTN_STATE_INA] = COLOR_WHITE,

		.bullet.rects.bwidth = 2 * LV_STYLE_MULT,
		.bullet.rects.bopa = 70,
		.bullet.rects.empty = 0,
		.bullet.rects.round = LV_OBJ_DEF_WIDTH / 3 / 4,
		.bullet.rects.hpad = 0 * LV_STYLE_MULT,
		.bullet.rects.vpad = 0 * LV_STYLE_MULT,
		.bullet.rects.opad = 0 * LV_STYLE_MULT,

		/*Label*/
	   .label.font = LV_FONT_DEFAULT, .label.objs.color = COLOR_MAKE(0x10, 0x18, 0x20),
	   .label.letter_space = 2 * LV_STYLE_MULT, .label.line_space =  2 * LV_STYLE_MULT,
	   .label.mid =  0,

	   /*Others*/
	   .bullet_size = LV_OBJ_DEF_WIDTH / 3
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
 * @param par_dp pointer to an object, it will be the parent of the new check box
 * @param copy_dp pointer to a check box object, if not NULL then the new object will be copied from it
 * @return pointer to the created check box
 */
lv_obj_t * lv_cb_create(lv_obj_t* par_dp, lv_obj_t * copy_dp)
{
    /*Create the ancestor basic object*/
    lv_obj_t* new_obj_dp = lv_btn_create(par_dp, copy_dp);
    dm_assert(new_obj_dp);
    
    lv_cb_ext_t * ext = lv_obj_alloc_ext(new_obj_dp, sizeof(lv_cb_ext_t));
    dm_assert(ext);

    /*Init the new checkbox object*/
    if(copy_dp == NULL) {
        lv_rect_set_layout(new_obj_dp, LV_RECT_LAYOUT_ROW_M);
        lv_rect_set_fit(new_obj_dp, true, true);
        lv_btn_set_tgl(new_obj_dp, true);

        ext->bullet = lv_btn_create(new_obj_dp, NULL);
        lv_obj_set_click(ext->bullet, false);

        ext->label = lv_label_create(new_obj_dp, NULL);
        lv_label_set_text(ext->label, "Check box");

        lv_obj_set_signal_f(new_obj_dp, lv_cb_signal);
        lv_obj_set_style(new_obj_dp, &lv_cbs_def);
    } else {
    	lv_cb_ext_t * copy_ext = lv_obj_get_ext(copy_dp);
    	ext->bullet = lv_btn_create(new_obj_dp, copy_ext->bullet);
    	ext->label = lv_label_create(new_obj_dp, copy_ext->label);
    }

    lv_obj_align_us(new_obj_dp, NULL, LV_ALIGN_CENTER, 0, 0);
    
    return new_obj_dp;
}


/**
 * Signal function of the check box
 * @param obj_dp pointer to a check box object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_cb_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_btn_signal(obj_dp, sign, param);

    lv_cb_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
    lv_cbs_t * style_p = lv_obj_get_style(obj_dp);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	switch(sign) {
    	case LV_SIGNAL_PRESSED:
    	case LV_SIGNAL_RELEASED:
    	case LV_SIGNAL_LONG_PRESS:
    		lv_btn_set_state(ext_dp->bullet, lv_btn_get_state(obj_dp));
    		break;
    	case LV_SIGNAL_STYLE_CHG:
    		lv_obj_set_size(ext_dp->bullet, style_p->bullet_size, style_p->bullet_size);
    		lv_obj_set_style(ext_dp->bullet, &style_p->bullet);
    		lv_obj_set_style(ext_dp->label, &style_p->label);
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
 * @param obj_dp pointer to a check box
 * @param txt the text of the check box
 */
void lv_cb_set_text(lv_obj_t * obj_dp, const char * txt)
{
	lv_cb_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	lv_label_set_text(ext_dp->label, txt);
}


/*=====================
 * Getter functions
 *====================*/

/**
 * Get the text of a check box
 * @param obj_dp pointer to check box object
 * @return pointer to the text of the check box
 */
const char * lv_cb_get_text(lv_obj_t * obj_dp)
{
	lv_cb_ext_t * ext_dp = lv_obj_get_ext(obj_dp);
	return lv_label_get_text(ext_dp->label);
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_cbs_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_cbs_t style
 */
lv_cbs_t * lv_cbs_get(lv_cbs_builtin_t style, lv_cbs_t * copy_p)
{
	lv_cbs_t  *style_p;

	switch(style) {
		case LV_CBS_DEF:
			style_p = &lv_cbs_def;
			break;
		default:
			style_p = &lv_cbs_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_cbs_t));
		else memcpy(copy_p, &lv_cbs_def, sizeof(lv_cbs_t));
	}

	return style_p;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

#if 0 /*THe ancestor design function is used */
/**
 * Handle the drawing related tasks of the check boxes
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'
 */
static bool lv_cb_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return false;
    }


    /*Draw the object*/

    return true;
}
#endif

#endif
