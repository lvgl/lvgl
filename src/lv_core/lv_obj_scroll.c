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
lv_coord_t lv_obj_get_scroll_bottom(const lv_obj_t * obj)
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
lv_coord_t lv_obj_get_scroll_right(const lv_obj_t * obj)
{
    static uint32_t cnt = 0;
    printf("scrl right: %d\n", cnt);
    cnt++;
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

    return child_res - obj->coords.x2 + lv_obj_get_style_pad_right(obj, LV_OBJ_PART_MAIN);
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
