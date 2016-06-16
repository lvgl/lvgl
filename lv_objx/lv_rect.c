/**
 * @file lv_rect.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_RECT != 0

#include "lv_rect.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_draw/lv_draw_vbasic.h"
#include "misc/math/math_base.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_rect_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_rects_t lv_rects_def =
{ .objs.color = COLOR_MAKE(0x50, 0x70, 0x90), .gcolor = COLOR_MAKE(0x20, 0x40, 0x60),
  .bcolor = COLOR_WHITE, .bwidth = 2 * LV_STYLE_MULT, .bopa = 50,
  .round = 4 * LV_STYLE_MULT, .empty = 0,
  .hpad = 0, .vpad = 0 };

static lv_rects_t lv_rects_transp =
{ .bwidth = 0, .empty = 1,
  .hpad = 0, .vpad = 0  };

static lv_rects_t lv_rects_border =
{ .bcolor = COLOR_BLACK, .bwidth = 2 * LV_STYLE_MULT, .bopa = 100,
  .round = 4 * LV_STYLE_MULT, .empty = 1,
  .hpad = 0, .vpad = 0};

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
 * Create a label objects
 * @param par_dp pointer to an object, it will be the parent of the new label
 * @param copy_dp pointer to a rectangle object, if not NULL then the new object will be copied from it
 * @return pointer to the created label
 */
lv_obj_t* lv_rect_create(lv_obj_t* par_dp, lv_obj_t * copy_dp)
{
    /*Create a basic object*/
    lv_obj_t* new_obj_dp = lv_obj_create(par_dp, copy_dp);
    dm_assert(new_obj_dp);
    lv_obj_alloc_ext(new_obj_dp, sizeof(lv_rect_ext_t));
    lv_rect_ext_t * rect_ext_dp = lv_obj_get_ext(new_obj_dp);
    lv_obj_set_design_f(new_obj_dp, lv_rect_design);
    lv_obj_set_signal_f(new_obj_dp, lv_rect_signal);

    /*Init the new rectangle*/
    if(copy_dp == NULL) {
		lv_obj_set_style(new_obj_dp, &lv_rects_def);
		rect_ext_dp->hpad_en = 0;
		rect_ext_dp->vpad_en = 0;
    } else {
    	lv_rect_ext_t * ori_rect_ext = lv_obj_get_ext(copy_dp);
    	rect_ext_dp->hpad_en = ori_rect_ext->hpad_en;
    	rect_ext_dp->vpad_en = ori_rect_ext->vpad_en;
    }

    return new_obj_dp;
}

/**
 * Signal function of the rectangle
 * @param obj_dp pointer to a rectangle object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_rect_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_obj_signal(obj_dp, sign, param);
    area_t rect_cords;
    lv_rects_t * rests_p = lv_obj_get_style(obj_dp);
    lv_rect_ext_t * ext_p = lv_obj_get_ext(obj_dp);
    lv_obj_t * i;

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	switch(sign) {
    	case LV_SIGNAL_STYLE_CHG: /*Recalculate the padding if the style changed*/
        case LV_SIGNAL_CHILD_CHG:
        	if(ext_p->hpad_en == 0 &&
			   ext_p->vpad_en == 0) {
        		break;
        	}
        	/*Search the side coordinates of the children*/
        	lv_obj_get_cords(obj_dp, &rect_cords);
        	rect_cords.x1 = LV_CORD_MAX;
        	rect_cords.y1 = LV_CORD_MAX;
        	rect_cords.x2 = LV_CORD_MIN;
        	rect_cords.y2 = LV_CORD_MIN;

            LL_READ(obj_dp->child_ll, i) {
            	rect_cords.x1 = min(rect_cords.x1, i->cords.x1);
            	rect_cords.y1 = min(rect_cords.y1, i->cords.y1);
                rect_cords.x2 = max(rect_cords.x2, i->cords.x2);
                rect_cords.y2 = max(rect_cords.y2, i->cords.y2);
            }

            /*If the value is not the init value then the page has >=1 child.*/
            if(rect_cords.x1 != LV_CORD_MAX) {
            	if(rests_p->hpad != 0) {
					rect_cords.x1 -= rests_p->hpad;
					rect_cords.x2 += rests_p->hpad;
            	} else {
            		rect_cords.x1 = obj_dp->cords.x1;
            		rect_cords.x2 = obj_dp->cords.x2;
            	}
            	if(rests_p->vpad != 0) {
					rect_cords.y1 -= rests_p->vpad;
					rect_cords.y2 += rests_p->vpad;
            	} else {
            		rect_cords.y1 = obj_dp->cords.y1;
            		rect_cords.y2 = obj_dp->cords.y2;
            	}
                area_cpy(&obj_dp->cords, &rect_cords);

                lv_obj_set_pos(obj_dp, lv_obj_get_x(obj_dp),
                                       lv_obj_get_y(obj_dp));
            } else {
                lv_obj_set_size(obj_dp, LV_OBJ_DEF_WIDTH, LV_OBJ_DEF_HEIGHT);
            }

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
 * Enable the horizontal or vertical padding
 * @param obj_dp pointer to a rectangle object
 * @param hor_en true: enable the horizontal padding
 * @param ver_en true: enable the vertical padding
 */
void lv_rect_set_pad_en(lv_obj_t * obj_dp, bool hor_en, bool ver_en)
{
	lv_rect_ext_t * ext_p = lv_obj_get_ext(obj_dp);
	ext_p->hpad_en = hor_en == false ? 0 : 1;
	ext_p->vpad_en = ver_en == false ? 0 : 1;

	obj_dp->signal_f(obj_dp, LV_SIGNAL_STYLE_CHG, obj_dp);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get horizontal padding enable attribute of a rectangle
 * @param obj_dp pointer to a rectangle object
 * @return true: horizontal padding is enabled
 */
bool lv_rect_get_hpad_en(lv_obj_t * obj_dp)
{
	lv_rect_ext_t * ext_p = lv_obj_get_ext(obj_dp);
	return ext_p->hpad_en == 0 ? false : true;
}

/**
 * Get vertical padding enable attribute of a rectangle
 * @param obj_dp pointer to a rectangle object
 * @return true: vertical padding is enabled
 */
bool lv_rect_get_vpad_en(lv_obj_t * obj_dp)
{
	lv_rect_ext_t * ext_p = lv_obj_get_ext(obj_dp);
	return ext_p->vpad_en == 0 ? false : true;
}


/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_rects_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_rects_t style
 */
lv_rects_t * lv_rects_get(lv_rects_builtin_t style, lv_rects_t * copy_p)
{
	lv_rects_t * style_p;

	switch(style) {
		case LV_RECTS_DEF:
			style_p = &lv_rects_def;
			break;
		case LV_RECTS_BORDER:
			style_p = &lv_rects_border;
			break;
		case LV_RECTS_TRANSP:
			style_p = &lv_rects_transp;
			break;
		default:
			style_p = &lv_rects_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_rects_t));
		else memcpy(copy_p, &lv_rects_def, sizeof(lv_rects_t));
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
 * Handle the drawing related tasks of the labels
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'
 */
static bool lv_rect_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    /* Because of the radius it is not sure the area is covered*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	if(LV_SA(obj_dp, lv_rects_t)->empty != 0) return false;

    	uint16_t r = LV_SA(obj_dp, lv_rects_t)->round;
    	area_t area_tmp;

    	/*Check horizontally without radius*/
    	lv_obj_get_cords(obj_dp, &area_tmp);
    	area_tmp.x1 += r;
    	area_tmp.x2 -= r;
    	if(area_is_in(mask_p, &area_tmp) == true) return true;

    	/*Check vertically without radius*/
    	lv_obj_get_cords(obj_dp, &area_tmp);
    	area_tmp.y1 += r;
    	area_tmp.y2 -= r;
    	if(area_is_in(mask_p, &area_tmp) == true) return true;

    	return false;
    }
    
    opa_t opa = lv_obj_get_opa(obj_dp);
    area_t area;
    lv_obj_get_cords(obj_dp, &area);

    /*Draw the rectangle*/
    lv_draw_rect(&area, mask_p, lv_obj_get_style(obj_dp), opa);
    
    return true;
}


#endif
