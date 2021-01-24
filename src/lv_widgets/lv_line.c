/**
 * @file lv_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_line.h"

#if LV_USE_LINE != 0
#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_misc/lv_math.h"
#include "../lv_themes/lv_theme.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_line"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_line_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_line_destructor(lv_obj_t * obj);
static lv_draw_res_t lv_line_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_line_signal(lv_obj_t * obj, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_line = {
    .constructor = lv_line_constructor,
    .destructor = lv_line_destructor,
    .signal_cb = lv_line_signal,
    .draw_cb = lv_line_draw,
    .instance_size = sizeof(lv_line_t),
    .base_class = &lv_obj
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a line objects
 * @param par pointer to an object, it will be the parent of the new line
 * @return pointer to the created line
 */
lv_obj_t * lv_line_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    return lv_obj_create_from_class(&lv_line, parent, copy);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set an array of points. The line object will connect these points.
 * @param line pointer to a line object
 * @param point_a an array of points. Only the address is saved,
 * so the array can NOT be a local variable which will be destroyed
 * @param point_num number of points in 'point_a'
 */
void lv_line_set_points(lv_obj_t * obj, const lv_point_t point_a[], uint16_t point_num)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_line_t * line = (lv_line_t *) obj;
    line->point_array    = point_a;
    line->point_num      = point_num;

    lv_obj_handle_self_size_chg(obj);

    lv_obj_invalidate(obj);
}

/**
 * Enable (or disable) the y coordinate inversion.
 * If enabled then y will be subtracted from the height of the object,
 * therefore the y=0 coordinate will be on the bottom.
 * @param line pointer to a line object
 * @param en true: enable the y inversion, false:disable the y inversion
 */
void lv_line_set_y_invert(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_line_t * line = (lv_line_t *) obj;
    if(line->y_inv == en) return;

    line->y_inv = en == false ? 0 : 1;

    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the y inversion attribute
 * @param line pointer to a line object
 * @return true: y inversion is enabled, false: disabled
 */
bool lv_line_get_y_invert(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_line_t * line = (lv_line_t *) obj;

    return line->y_inv == 0 ? false : true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_line_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_TRACE("line create started");

    lv_obj_construct_base(obj, parent, copy);
    /*Create a basic object*/
    lv_line_t * line = (lv_line_t *) obj;

    line->point_num   = 0;
    line->point_array = NULL;
    line->y_inv       = 0;


    /*Init the new line*/
    if(copy == NULL) {
        lv_obj_set_size(obj, LV_SIZE_AUTO, LV_SIZE_AUTO);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    }
    /*Copy an existing object*/
    else {
        lv_line_t * copy_line = (lv_line_t *) copy;
        lv_line_set_y_invert(obj, lv_line_get_y_invert(copy));
        lv_line_set_points(obj, copy_line->point_array, copy_line->point_num);

    }

    LV_LOG_INFO("line created");
}

static void lv_line_destructor(lv_obj_t * obj)
{

}
/**
 * Handle the drawing related tasks of the lines
 * @param line pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DRAW_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DRAW_DRAW: draw the object (always return 'true')
 *             LV_DRAW_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_draw_res_t`
 */
static lv_draw_res_t lv_line_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    /*A line never covers an area*/
    if(mode == LV_DRAW_MODE_COVER_CHECK)
        return LV_DRAW_RES_NOT_COVER;
    else if(mode == LV_DRAW_MODE_MAIN_DRAW) {
        lv_obj.draw_cb(obj, clip_area, mode);
        lv_line_t * line = (lv_line_t *) obj;

        if(line->point_num == 0 || line->point_array == NULL) return false;

        lv_area_t area;
        lv_obj_get_coords(obj, &area);
        lv_coord_t x_ofs = area.x1 - lv_obj_get_scroll_x(obj);
        lv_coord_t y_ofs = area.y1 - lv_obj_get_scroll_y(obj);
        lv_point_t p1;
        lv_point_t p2;
        lv_coord_t h = lv_obj_get_height(obj);
        uint16_t i;

        lv_draw_line_dsc_t line_dsc;
        lv_draw_line_dsc_init(&line_dsc);
        lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);

        /*Read all points and draw the lines*/
        for(i = 0; i < line->point_num - 1; i++) {

            p1.x = line->point_array[i].x + x_ofs;
            p2.x = line->point_array[i + 1].x + x_ofs;

            if(line->y_inv == 0) {
                p1.y = line->point_array[i].y + y_ofs;
                p2.y = line->point_array[i + 1].y + y_ofs;
            }
            else {
                p1.y = h - line->point_array[i].y + y_ofs;
                p2.y = h - line->point_array[i + 1].y + y_ofs;
            }
            lv_draw_line(&p1, &p2, clip_area, &line_dsc);
            line_dsc.round_start = 0;   /*Draw the rounding only on the end points after the first line*/
        }
    } else if (mode == LV_DRAW_MODE_POST_DRAW) {
        lv_obj.draw_cb(obj, clip_area, mode);
    }
    return LV_DRAW_RES_OK;
}

/**
 * Signal function of the line
 * @param line pointer to a line object
 * @param sign a signal type from lv_signal_t enum
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_line_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = lv_obj.signal_cb(obj, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_REFR_EXT_DRAW_SIZE) {
        /*The corner of the skew lines is out of the intended area*/
        lv_coord_t line_width = lv_obj_get_style_line_width(obj, LV_PART_MAIN);
        lv_coord_t * s = param;
        if(*s < line_width) *s = line_width;
    }
    else if(sign == LV_SIGNAL_GET_SELF_SIZE) {
        lv_line_t * line = (lv_line_t *) obj;

        lv_point_t * p = param;
        lv_coord_t w = 0;
        lv_coord_t h = 0;
        if(line->point_num > 0) {
            uint16_t i;
            for(i = 0; i < line->point_num; i++) {
                w = LV_MAX(line->point_array[i].x, w);
                h = LV_MAX(line->point_array[i].y, h);
            }

            lv_coord_t line_width = lv_obj_get_style_line_width(obj, LV_PART_MAIN);
            w += line_width;
            h += line_width;
            p->x = w;
            p->y = h;
        }
    }

    return res;
}
#endif
