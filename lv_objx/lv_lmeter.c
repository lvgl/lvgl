/**
 * @file lv_lmeter.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_LMETER != 0

#include "lv_lmeter.h"
#include "../misc/math/trigo.h"
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_lmeter_design(lv_obj_t * lmeter, const area_t * mask, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/

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
 * Create a line meter objects
 * @param par pointer to an object, it will be the parent of the new line meter
 * @param copy pointer to a line meter object, if not NULL then the new object will be copied from it
 * @return pointer to the created line meter
 */
lv_obj_t * lv_lmeter_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of line meter*/
    lv_obj_t * new_lmeter = lv_bar_create(par, copy);
    dm_assert(new_lmeter);
    
    /*Allocate the line meter type specific extended data*/
    lv_lmeter_ext_t * ext = lv_obj_alloc_ext(new_lmeter, sizeof(lv_lmeter_ext_t));
    dm_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->scale_num = 31;    /*Odd scale number looks better*/
    ext->scale_angle = 240; /*(scale_num - 1) * N looks better */

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_f(new_lmeter, lv_lmeter_signal);
    lv_obj_set_design_f(new_lmeter, lv_lmeter_design);

    /*Init the new line meter line meter*/
    if(copy == NULL) {
        lv_obj_set_size(new_lmeter, 1 * LV_DPI, 1 * LV_DPI);
        lv_obj_set_style(new_lmeter, lv_style_get(LV_STYLE_PRETTY_COLOR, NULL));
    }
    /*Copy an existing line meter*/
    else {
    	lv_lmeter_ext_t * copy_ext = lv_obj_get_ext(copy);
    	ext->scale_angle = copy_ext->scale_angle;
        ext->scale_num = copy_ext->scale_num;


        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_lmeter);
    }
    
    return new_lmeter;
}

/**
 * Signal function of the line meter
 * @param lmeter pointer to a line meter object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_lmeter_signal(lv_obj_t * lmeter, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_bar_signal(lmeter, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	if(sign == LV_SIGNAL_CLEANUP) {
            /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    	}
    }
    
    return valid;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the scale settings of a line meter
 * @param lmeter pointer to a line meter object
 * @param angle angle of the scale (0..360)
 * @param num number of scale units
 */
void lv_lmeter_set_scale(lv_obj_t * lmeter, uint16_t angle, uint8_t num)
{
    lv_lmeter_ext_t * ext = lv_obj_get_ext(lmeter);
    ext->scale_angle = angle;
    ext->scale_num = num;

    lv_obj_inv(lmeter);
}


/*=====================
 * Getter functions
 *====================*/

/**
 * Get the scale number of a line meter
 * @param lmeter pointer to a line meter object
 * @return number of the scale units
 */
uint8_t lv_lmeter_get_scale_num(lv_obj_t * lmeter)
{
    lv_lmeter_ext_t * ext = lv_obj_get_ext(lmeter);
    return ext->scale_num ;
}

/**
 * Get the scale angle of a line meter
 * @param lmeter pointer to a line meter object
 * @return angle of the scale
 */
uint16_t lv_lmeter_get_scale_angle(lv_obj_t * lmeter)
{
    lv_lmeter_ext_t * ext = lv_obj_get_ext(lmeter);
    return ext->scale_angle;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * Handle the drawing related tasks of the line meters
 * @param lmeter pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_lmeter_design(lv_obj_t * lmeter, const area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_lmeter_ext_t * ext = lv_obj_get_ext(lmeter);
        lv_style_t * style = lv_obj_get_style(lmeter);

        lv_style_t style_tmp;
        memcpy(&style_tmp, style, sizeof(lv_style_t));

         cord_t r_out = lv_obj_get_width(lmeter) / 2;
         cord_t r_in =r_out - style->hpad;
         cord_t x_ofs = lv_obj_get_width(lmeter) / 2 + lmeter->cords.x1;
         cord_t y_ofs = lv_obj_get_height(lmeter) / 2 + lmeter->cords.y1;
         int16_t angle_ofs = 90 + (360 - ext->scale_angle) / 2;
         int16_t min = lv_bar_get_min_value(lmeter);
         int16_t max = lv_bar_get_max_value(lmeter);
         int16_t level = (int32_t)((int32_t)(lv_bar_get_value(lmeter) - min) * ext->scale_num) / (max - min);
         uint8_t i;

         style_tmp.ccolor = style->mcolor;

         for(i = 0; i < ext->scale_num; i++) {
             /*Calculate the position a scale label*/
             int16_t angle = (i * ext->scale_angle) / (ext->scale_num - 1) + angle_ofs;

             cord_t y_out = (int32_t)((int32_t)trigo_sin(angle) * r_out) / TRIGO_SIN_MAX;
             cord_t x_out = (int32_t)((int32_t)trigo_sin(angle + 90) * r_out) / TRIGO_SIN_MAX;
             cord_t y_in = (int32_t)((int32_t)trigo_sin(angle) * r_in) / TRIGO_SIN_MAX;
             cord_t x_in = (int32_t)((int32_t)trigo_sin(angle + 90) * r_in) / TRIGO_SIN_MAX;

             point_t p1;
             point_t p2;

             p2.x = x_in + x_ofs;
             p2.y = y_in +  y_ofs;

             p1.x = x_out+ x_ofs;
             p1.y = y_out + y_ofs;

             if(i > level) style_tmp.ccolor = style->ccolor;
             else {
                 style_tmp.ccolor=color_mix(style->gcolor, style->mcolor, (255 * i) /  ext->scale_num);
             }

             lv_draw_line(&p1, &p2, mask, &style_tmp);
         }

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}


#endif
