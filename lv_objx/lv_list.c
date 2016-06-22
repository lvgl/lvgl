/**
 * @file lv_list.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_LIST != 0

#include "lv_list.h"
#include "lv_rect.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_list_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_lists_t lv_lists_def =
{
	/*Page style*/
	.pages.bg_rects.objs.color = COLOR_WHITE, .pages.bg_rects.gcolor = COLOR_SILVER, .pages.bg_rects.bcolor = COLOR_GRAY,
	.pages.bg_rects.bopa = 50, .pages.bg_rects.bwidth = 0 * LV_STYLE_MULT, .pages.bg_rects.round = 2 * LV_STYLE_MULT,
	.pages.bg_rects.empty = 0,
	.pages.bg_rects.vpad = 40,
	.pages.bg_rects.hpad = 20,
	.pages.bg_rects.opad = 10,

	.pages.sb_rects.objs.color = COLOR_BLACK, .pages.sb_rects.gcolor = COLOR_BLACK, .pages.sb_rects.bcolor = COLOR_WHITE,
	.pages.sb_rects.bopa = 50, .pages.sb_rects.bwidth = 1 * LV_STYLE_MULT, .pages.sb_rects.round = 5 * LV_STYLE_MULT,
	.pages.sb_rects.empty = 0, .pages.sb_width= 8 * LV_STYLE_MULT, .pages.sb_opa=50, .pages.sb_mode = LV_PAGE_SB_MODE_ON,

	.pages.margin_ver = 0 * LV_STYLE_MULT,
	.pages.margin_ver = 0 * LV_STYLE_MULT,

	/*List style*/
	.list_layout = LV_LAYOUT_CENTER,

	/*List element style*/
	.liste_btns.mcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0xa0, 0xa0, 0xa0), .liste_btns.gcolor[LV_BTN_STATE_REL] = COLOR_WHITE, .liste_btns.bcolor[LV_BTN_STATE_REL] = COLOR_WHITE,
	.liste_btns.mcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0x60, 0x80, 0xa0), .liste_btns.gcolor[LV_BTN_STATE_PR] = COLOR_MAKE(0xd0, 0xd0, 0xd0), .liste_btns.bcolor[LV_BTN_STATE_PR] = COLOR_WHITE,
	.liste_btns.mcolor[LV_BTN_STATE_TGL_REL] = COLOR_MAKE(0x80,0x00,0x00), .liste_btns.gcolor[LV_BTN_STATE_TGL_REL] = COLOR_MAKE(0x20, 0x20, 0x20), .liste_btns.bcolor[LV_BTN_STATE_TGL_REL] = COLOR_WHITE,
	.liste_btns.mcolor[LV_BTN_STATE_TGL_PR] = COLOR_MAKE(0xf0, 0x26, 0x26), .liste_btns.gcolor[LV_BTN_STATE_TGL_PR] = COLOR_MAKE(0x40, 0x40, 0x40), .liste_btns.bcolor[LV_BTN_STATE_TGL_PR] = COLOR_WHITE,
	.liste_btns.mcolor[LV_BTN_STATE_INA] = COLOR_SILVER, .liste_btns.gcolor[LV_BTN_STATE_INA] = COLOR_GRAY, .liste_btns.bcolor[LV_BTN_STATE_INA] = COLOR_WHITE,
	.liste_btns.rects.bwidth = 2 * LV_STYLE_MULT, .liste_btns.rects.bopa = 50,
	.liste_btns.rects.empty = 0, .liste_btns.rects.round = 4 * LV_STYLE_MULT,
	.liste_btns.rects.hpad = 10 * LV_STYLE_MULT,
	.liste_btns.rects.vpad = 20 * LV_STYLE_MULT,
	.liste_btns.rects.opad = 5 * LV_STYLE_MULT,

	.liste_layout = LV_LAYOUT_ROW_M,
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
 * Create a list objects
 * @param par_dp pointer to an object, it will be the parent of the new list
 * @param copy_dp pointer to a list object, if not NULL then the new object will be copied from it
 * @return pointer to the created list
 */
lv_obj_t* lv_list_create(lv_obj_t* par_dp, lv_obj_t * copy_dp)
{
    /*Create the ancestor basic object*/
    lv_obj_t * new_obj_dp = lv_page_create(par_dp, NULL);
    dm_assert(new_obj_dp);
    
    /*Init the new list object*/
    lv_obj_set_style(new_obj_dp, &lv_lists_def.pages);
    lv_rect_set_layout(new_obj_dp, lv_lists_def.list_layout);
    
    return new_obj_dp;
}

/**
 * Signal function of the list
 * @param obj_dp pointer to a list object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_list_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_list_signal(obj_dp, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	switch(sign) {
    		default:
    			break;
    	}
    }
    
    return valid;
}


void lv_list_add(lv_obj_t * obj_dp, const char * img_fn, const char * txt, void (*release) (lv_obj_t *))
{
	lv_obj_t * liste;
	liste = lv_btn_create(obj_dp, NULL);
	lv_obj_set_style(liste, &lv_lists_def.liste_btns);
	//lv_btn_set_rel_action(liste, release);
	lv_page_glue_obj(liste, true);
	lv_rect_set_layout(liste, lv_lists_def.liste_layout);
	lv_rect_set_fit(liste, true, true);


	if(img_fn != NULL) {

	}

	lv_obj_t * label = lv_label_create(liste, NULL);
	lv_label_set_text(label, txt);
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_lists_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_lists_t style
 */
lv_lists_t * lv_lists_get(lv_lists_builtin_t style, lv_lists_t * copy_p)
{
	lv_lists_t  *style_p;

	switch(style) {
		case LV_LISTS_DEF:
			style_p = &lv_lists_def;
			break;
		default:
			style_p = &lv_lists_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_lists_t));
		else memcpy(copy_p, &lv_lists_def, sizeof(lv_lists_t));
	}

	return style_p;
}
/*=====================
 * Setter functions 
 *====================*/


/*=====================
 * Getter functions 
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Handle the drawing related tasks of the lists
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'
 */
static bool lv_list_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return false;
    }


    /*Draw the object*/

    return true;
}


#endif
