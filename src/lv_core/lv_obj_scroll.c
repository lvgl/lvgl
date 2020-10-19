/**
 * @file lv_obj_scroll.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj_scroll.h"
#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_obj"
#define SCROLL_ANIM_TIME_MIN 100    /*ms*/
#define SCROLL_ANIM_TIME_MAX 300    /*ms*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void scroll_anim_x_cb(lv_obj_t * obj, lv_anim_value_t v);
static void scroll_anim_y_cb(lv_obj_t * obj, lv_anim_value_t v);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Set how the scrollbars should behave.
 * @param obj pointer to an object
 * @param mode: LV_SCROLL_MODE_ON/OFF/AUTO/ACTIVE
 */
void lv_obj_set_scroll_mode(lv_obj_t * obj, lv_scroll_mode_t mode)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(obj->scroll_mode == mode) return;
    obj->scroll_mode = mode;
    lv_obj_invalidate(obj);
}

/**
 * Get how the scrollbars should behave.
 * @param obj pointer to an object
 * @return mode: LV_SCROLL_MODE_ON/OFF/AUTO/ACTIVE
 */
lv_scroll_mode_t lv_obj_get_scroll_mode(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return obj->scroll_mode;
}

/**
 * Moves all children with horizontally or vertically.
 * It doesn't take into account any limits so any values are possible
 * @param obj pointer to an object whose children should be moved
 * @param x pixel to move horizontally
 * @param y pixels to move vertically
 */
void _lv_obj_scroll_by_raw(lv_obj_t * obj, lv_coord_t x, lv_coord_t y)
{
    obj->scroll.x += x;
    obj->scroll.y += y;

    _lv_obj_move_children_by(obj, x, y);
    lv_res_t res = lv_signal_send(obj, LV_SIGNAL_SCROLL, NULL);
    if(res != LV_RES_OK) return;
    lv_obj_invalidate(obj);
}
/**
 * Moves all children with horizontally or vertically.
 * Limits the scroll to the bounding box of the children.
 * @param obj pointer to an object whose children should be moved
 * @param x pixel to move horizontally
 * @param y pixels to move vertically
 */
void lv_obj_scroll_by(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en)
{
    if(x == 0 && y == 0) return;

    if(anim_en == LV_ANIM_ON) {
        lv_disp_t * d = lv_obj_get_disp(obj);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);

        lv_anim_path_t path;
        lv_anim_path_init(&path);
        lv_anim_path_set_cb(&path, lv_anim_path_ease_out);

        if(x) {
            uint32_t t = lv_anim_speed_to_time((lv_disp_get_hor_res(d) * 3) >> 2, 0, x);
            if(t < SCROLL_ANIM_TIME_MIN) t = SCROLL_ANIM_TIME_MIN;
            if(t > SCROLL_ANIM_TIME_MAX) t = SCROLL_ANIM_TIME_MAX;
            lv_anim_set_time(&a, t);
            lv_anim_set_values(&a, obj->scroll.x, obj->scroll.x + x);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) scroll_anim_x_cb);
            lv_anim_set_path(&a, &path);
            lv_anim_start(&a);
        }

        if(y) {
            uint32_t t = lv_anim_speed_to_time((lv_disp_get_ver_res(d) * 3) >> 2, 0, y);
            if(t < SCROLL_ANIM_TIME_MIN) t = SCROLL_ANIM_TIME_MIN;
            if(t > SCROLL_ANIM_TIME_MAX) t = SCROLL_ANIM_TIME_MAX;
            lv_anim_set_time(&a, t);
            lv_anim_set_values(&a, obj->scroll.y, obj->scroll.y + y);
            lv_anim_set_exec_cb(&a,  (lv_anim_exec_xcb_t) scroll_anim_y_cb);
            lv_anim_set_path(&a, &path);
            lv_anim_start(&a);
        }
    } else {
        _lv_obj_scroll_by_raw(obj, x, y);
    }
}

/**
 * Scroll the a given x coordinate to the left side of obj.
 * @param obj pointer to an object which should be scrolled
 * @param x the x coordinate to scroll to
 * @param y the y coordinate to scroll to
 */
void lv_obj_scroll_to(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en)
{
    lv_obj_scroll_to_x(obj, x, anim_en);
    lv_obj_scroll_to_y(obj, y, anim_en);
}

/**
 * Scroll the a given x coordinate to the left side of obj.
 * @param obj pointer to an object which should be scrolled
 * @param x the x coordinate to scroll to
 */
void lv_obj_scroll_to_x(lv_obj_t * obj, lv_coord_t x, lv_anim_enable_t anim_en)
{
    lv_obj_scroll_by(obj, -x - obj->scroll.x, 0, anim_en);
}

/**
 * Scroll the a given y coordinate to the top side of obj.
 * @param obj pointer to an object which should be scrolled
 * @param y the y coordinate to scroll to
 */
void lv_obj_scroll_to_y(lv_obj_t * obj, lv_coord_t y, lv_anim_enable_t anim_en)
{
    lv_obj_scroll_by(obj, 0,  -y - obj->scroll.y, anim_en);
}


/**
 * Return the height of the area above the parent.
 * That is the number of pixels the object can be scrolled down.
 * Normally positive but can be negative when scrolled inside.
 * @param obj
 * @return
 */
lv_coord_t lv_obj_get_scroll_top(const lv_obj_t * obj)
{
    return -obj->scroll.y;
}

/**
 * Return the height of the area below the parent.
 * That is the number of pixels the object can be scrolled up.
 * Normally positive but can be negative when scrolled inside.
 * @param obj
 * @return
 */
lv_coord_t lv_obj_get_scroll_bottom(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_coord_t child_res = LV_COORD_MIN;

    lv_obj_t * child = lv_obj_get_child(obj, NULL);
    if(child) {
        lv_coord_t y2 = LV_COORD_MIN;
        while(child) {
            y2 = LV_MATH_MAX(y2, child->coords.y2 + lv_obj_get_style_margin_bottom(child, LV_OBJ_PART_MAIN));
            child = lv_obj_get_child(obj, child);
        }

        child_res = y2;
    }


    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_OBJ_PART_MAIN);
    lv_coord_t pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_OBJ_PART_MAIN);

    child_res -= (obj->coords.y2 - pad_bottom);

    lv_coord_t self_h = _lv_obj_get_self_height(obj);
    self_h = self_h - (lv_obj_get_height(obj) - pad_top - pad_bottom);
    self_h += obj->scroll.y;
    return LV_MATH_MAX(child_res, self_h);


    return child_res - obj->coords.y2 + lv_obj_get_style_pad_bottom(obj, LV_OBJ_PART_MAIN);
}

/**
 * Return the weight of the area on the left the parent.
 * That is the number of pixels the object can be scrolled down.
 * Normally positive but can be negative when scrolled inside.
 * @param obj
 * @return
 */
lv_coord_t lv_obj_get_scroll_left(const lv_obj_t * obj)
{
    return -obj->scroll.x;
}

/**
 * Return the width of the area below the object.
 * That is the number of pixels the object can be scrolled left.
 * Normally positive but can be negative when scrolled inside.
 * @param obj
 * @return
 */
lv_coord_t lv_obj_get_scroll_right(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_coord_t child_res = LV_COORD_MIN;
    lv_obj_t * child = lv_obj_get_child(obj, NULL);
    if(child) {
        lv_coord_t x2 = LV_COORD_MIN;
        while(child) {
            x2 = LV_MATH_MAX(x2, child->coords.x2 + lv_obj_get_style_margin_right(child, LV_OBJ_PART_MAIN));
            child = lv_obj_get_child(obj, child);
        }
        child_res = x2;

    }

    lv_coord_t pad_right = lv_obj_get_style_pad_right(obj, LV_OBJ_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_OBJ_PART_MAIN);

    child_res -= (obj->coords.x2 - pad_right);

    lv_coord_t self_w = _lv_obj_get_self_width(obj);
    self_w = self_w - (lv_obj_get_width(obj) - pad_right - pad_left);
    self_w += obj->scroll.x;

    return LV_MATH_MAX(child_res, self_w);
}

/**
 * Get the X and Y coordinates where the scrolling would end for this object if a scrolling animation is in progress.
 * In no scrolling animation give the current `left` or `top` scroll position.
 * @param obj pointer to an object
 * @param end poinr to point to store the result
 */
void lv_obj_get_scroll_end(struct _lv_obj_t  * obj, lv_point_t * end)
{
    lv_anim_t * a;
    a = lv_anim_get(obj, (lv_anim_exec_xcb_t)scroll_anim_x_cb);
    end->x = a ? -a->end : lv_obj_get_scroll_left(obj);

    a = lv_anim_get(obj, (lv_anim_exec_xcb_t)scroll_anim_y_cb);
    end->y = a ? -a->end : lv_obj_get_scroll_top(obj);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void scroll_anim_x_cb(lv_obj_t * obj, lv_anim_value_t v)
{
    _lv_obj_scroll_by_raw(obj, v - obj->scroll.x, 0);
}

static void scroll_anim_y_cb(lv_obj_t * obj, lv_anim_value_t v)
{
    _lv_obj_scroll_by_raw(obj, 0, v - obj->scroll.y);
}
