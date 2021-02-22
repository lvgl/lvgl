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
#define MY_CLASS &lv_obj_class
#define SCROLL_ANIM_TIME_MIN 200    /*ms*/
#define SCROLL_ANIM_TIME_MAX 400    /*ms*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/
void lv_obj_move_children_by(lv_obj_t * obj, lv_coord_t x_diff, lv_coord_t y_diff);

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void scroll_anim_x_cb(lv_obj_t * obj, int32_t v);
static void scroll_anim_y_cb(lv_obj_t * obj, int32_t v);
static void scroll_anim_ready_cb(lv_anim_t * a);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*=====================
 * Setter functions
 *====================*/

void lv_obj_set_scrollbar_mode(lv_obj_t * obj, lv_scrollbar_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_allocate_spec_attr(obj);

    if(obj->spec_attr->scrollbar_mode == mode) return;
    obj->spec_attr->scrollbar_mode = mode;
    lv_obj_invalidate(obj);
}

void lv_obj_set_scroll_dir(struct _lv_obj_t * obj, lv_dir_t dir)
{
    lv_obj_allocate_spec_attr(obj);

    if(dir != obj->spec_attr->scroll_dir) {
        obj->spec_attr->scroll_dir = dir;
    }
}

void lv_obj_set_snap_align_x(struct _lv_obj_t * obj, lv_snap_align_t align)
{
    lv_obj_allocate_spec_attr(obj);
    obj->spec_attr->snap_align_x = align;
}

void lv_obj_set_snap_align_y(struct _lv_obj_t * obj, lv_snap_align_t align)
{
    lv_obj_allocate_spec_attr(obj);
    obj->spec_attr->snap_align_y = align;
}

/*=====================
 * Getter functions
 *====================*/

lv_scrollbar_mode_t lv_obj_get_scrollbar_mode(const struct _lv_obj_t * obj)
{
    if(obj->spec_attr) return obj->spec_attr->scrollbar_mode;
    else return LV_SCROLLBAR_MODE_AUTO;
}

lv_dir_t lv_obj_get_scroll_dir(const struct _lv_obj_t * obj)
{
    if(obj->spec_attr) return obj->spec_attr->scroll_dir;
    else return LV_DIR_ALL;
}

lv_snap_align_t lv_obj_get_snap_align_x(const struct _lv_obj_t * obj)
{
    if(obj->spec_attr) return obj->spec_attr->snap_align_x;
    else return LV_SCROLL_SNAP_ALIGN_NONE;
}

lv_snap_align_t lv_obj_get_snap_align_y(const struct _lv_obj_t * obj)
{
    if(obj->spec_attr) return obj->spec_attr->snap_align_y;
    else return LV_SCROLL_SNAP_ALIGN_NONE;
}

lv_coord_t lv_obj_get_scroll_x(const lv_obj_t * obj)
{
    if(obj->spec_attr == NULL) return 0;
    return -obj->spec_attr->scroll.x;
}

lv_coord_t lv_obj_get_scroll_y(const lv_obj_t * obj)
{
    if(obj->spec_attr == NULL) return 0;
    return -obj->spec_attr->scroll.y;
}

lv_coord_t lv_obj_get_scroll_top(lv_obj_t * obj)
{
    if(obj->spec_attr == NULL) return 0;
    return -obj->spec_attr->scroll.y;
}

lv_coord_t lv_obj_get_scroll_bottom(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_coord_t child_res = LV_COORD_MIN;
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        child_res = LV_MAX(child_res, child->coords.y2);
    }

    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

    child_res -= (obj->coords.y2 - pad_bottom);

    lv_coord_t self_h = lv_obj_get_self_height(obj);
    self_h = self_h - (lv_obj_get_height(obj) - pad_top - pad_bottom);
    self_h -= lv_obj_get_scroll_y(obj);
    return LV_MAX(child_res, self_h);
}

lv_coord_t lv_obj_get_scroll_left(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    /* Normally can't scroll the object out on the left.
     * So simply use the current scroll position as "left size"*/
    if(lv_obj_get_base_dir(obj) != LV_BIDI_DIR_RTL) {
        if(obj->spec_attr == NULL) return 0;
        return -obj->spec_attr->scroll.x;
    }

    /*With RTL base direction scrolling the left is normal so find the left most coordinate*/
    lv_coord_t pad_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);

    lv_coord_t child_res = 0;

    uint32_t i;
    lv_coord_t x1 = LV_COORD_MAX;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
       lv_obj_t * child = lv_obj_get_child(obj, i);
        x1 = LV_MIN(x1, child->coords.x1);

    }

    if(x1 != LV_COORD_MAX) {
        child_res = x1;
        child_res = (obj->coords.x1 + pad_left) - child_res;
    }

    lv_coord_t self_w = lv_obj_get_self_width(obj);
    self_w = self_w - (lv_obj_get_width(obj) - pad_right - pad_left);
    self_w += lv_obj_get_scroll_x(obj);

    return LV_MAX(child_res, self_w);
}

lv_coord_t lv_obj_get_scroll_right(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    /* With RTL base dir can't scroll to the object out on the right.
     * So simply use the current scroll position as "right size"*/
    if(lv_obj_get_base_dir(obj) == LV_BIDI_DIR_RTL) {
        if(obj->spec_attr == NULL) return 0;
        return obj->spec_attr->scroll.x;
    }

    /*With other base direction (LTR) scrolling to the right is normal so find the right most coordinate*/
    lv_coord_t child_res = LV_COORD_MIN;
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        child_res = LV_MAX(child_res, child->coords.x2);
    }

    lv_coord_t pad_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);

    child_res -= (obj->coords.x2 - pad_right);

    lv_coord_t self_w;
    self_w = lv_obj_get_self_width(obj);
    self_w = self_w - (lv_obj_get_width(obj) - pad_right - pad_left);
    self_w -= lv_obj_get_scroll_x(obj);
    return LV_MAX(child_res, self_w);
}

void lv_obj_get_scroll_end(struct _lv_obj_t  * obj, lv_point_t * end)
{
    lv_anim_t * a;
    a = lv_anim_get(obj, (lv_anim_exec_xcb_t)scroll_anim_x_cb);
    end->x = a ? -a->end : lv_obj_get_scroll_x(obj);

    a = lv_anim_get(obj, (lv_anim_exec_xcb_t)scroll_anim_y_cb);
    end->y = a ? -a->end : lv_obj_get_scroll_y(obj);
}

/*=====================
 * Other functions
 *====================*/

void _lv_obj_scroll_by_raw(lv_obj_t * obj, lv_coord_t x, lv_coord_t y)
{
    if(x == 0 && y == 0) return;

    lv_obj_allocate_spec_attr(obj);

    obj->spec_attr->scroll.x += x;
    obj->spec_attr->scroll.y += y;

    lv_obj_move_children_by(obj, x, y);
    lv_res_t res = lv_signal_send(obj, LV_SIGNAL_SCROLL, NULL);
    if(res != LV_RES_OK) return;
    lv_obj_invalidate(obj);
}

void lv_obj_scroll_by(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en)
{
    if(x == 0 && y == 0) return;
    if(anim_en == LV_ANIM_ON) {
        lv_disp_t * d = lv_obj_get_disp(obj);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_ready_cb(&a, scroll_anim_ready_cb);

        lv_anim_path_t path;
        lv_anim_path_init(&path);
        lv_anim_path_set_cb(&path, lv_anim_path_ease_out);

        if(x) {
            lv_res_t res;
            res = lv_signal_send(obj, LV_SIGNAL_SCROLL_BEGIN, NULL);
            if(res != LV_RES_OK) return;

            res = lv_event_send(obj, LV_EVENT_SCROLL_BEGIN, NULL);
            if(res != LV_RES_OK) return;

            uint32_t t = lv_anim_speed_to_time((lv_disp_get_hor_res(d) * 2) >> 2, 0, x);
            if(t < SCROLL_ANIM_TIME_MIN) t = SCROLL_ANIM_TIME_MIN;
            if(t > SCROLL_ANIM_TIME_MAX) t = SCROLL_ANIM_TIME_MAX;
            lv_anim_set_time(&a, t);
            lv_coord_t sx = lv_obj_get_scroll_x(obj);
            lv_anim_set_values(&a, -sx, -sx + x);
            lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) scroll_anim_x_cb);
            lv_anim_set_path(&a, &path);
            lv_anim_start(&a);
        }

        if(y) {
            lv_res_t res;
            res = lv_signal_send(obj, LV_SIGNAL_SCROLL_BEGIN, NULL);
            if(res != LV_RES_OK) return;

            res = lv_event_send(obj, LV_EVENT_SCROLL_BEGIN, NULL);
            if(res != LV_RES_OK) return;

            uint32_t t = lv_anim_speed_to_time((lv_disp_get_ver_res(d) * 2) >> 2, 0, y);
            if(t < SCROLL_ANIM_TIME_MIN) t = SCROLL_ANIM_TIME_MIN;
            if(t > SCROLL_ANIM_TIME_MAX) t = SCROLL_ANIM_TIME_MAX;
            lv_anim_set_time(&a, t);
            lv_coord_t sy = lv_obj_get_scroll_y(obj);
            lv_anim_set_values(&a, -sy, -sy + y);
            lv_anim_set_exec_cb(&a,  (lv_anim_exec_xcb_t) scroll_anim_y_cb);
            lv_anim_set_path(&a, &path);
            lv_anim_start(&a);
        }
    } else {
        /*Remove pending animations*/
        lv_anim_del(obj, (lv_anim_exec_xcb_t) scroll_anim_y_cb);
        lv_anim_del(obj, (lv_anim_exec_xcb_t) scroll_anim_x_cb);
        _lv_obj_scroll_by_raw(obj, x, y);
    }
}

void lv_obj_scroll_to(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en)
{
    lv_obj_scroll_to_x(obj, x, anim_en);
    lv_obj_scroll_to_y(obj, y, anim_en);
}

void lv_obj_scroll_to_x(lv_obj_t * obj, lv_coord_t x, lv_anim_enable_t anim_en)
{
    lv_anim_del(obj, (lv_anim_exec_xcb_t) scroll_anim_x_cb);

    /*Don't let scroll more then naturally possible by the size of the content*/
    if(x < 0) x = 0;
    lv_coord_t  scroll_max = lv_obj_get_scroll_left(obj) + lv_obj_get_scroll_right(obj);
    if(scroll_max < 0) scroll_max = 0;

    if(x > scroll_max) x = scroll_max;

    lv_coord_t scroll_x = lv_obj_get_scroll_x(obj);
    lv_coord_t diff = -x + scroll_x;

    lv_obj_scroll_by(obj, diff, 0, anim_en);
}

void lv_obj_scroll_to_y(lv_obj_t * obj, lv_coord_t y, lv_anim_enable_t anim_en)
{
    lv_anim_del(obj, (lv_anim_exec_xcb_t) scroll_anim_y_cb);

    /*Don't let scroll more then naturally possible by the size of the content*/
    if(y < 0) y = 0;
    lv_coord_t  scroll_max = lv_obj_get_scroll_top(obj) + lv_obj_get_scroll_bottom(obj);
    if(scroll_max < 0) scroll_max = 0;
    if(y > scroll_max) y = scroll_max;

    lv_coord_t scroll_y = lv_obj_get_scroll_y(obj);
    lv_coord_t diff = -y + scroll_y;

    lv_obj_scroll_by(obj, 0, diff, anim_en);
}

void lv_obj_scroll_to_view(lv_obj_t * obj, lv_anim_enable_t anim_en)
{
    lv_obj_t * parent = lv_obj_get_parent(obj);

    lv_coord_t pleft = lv_obj_get_style_pad_left(parent, LV_PART_MAIN);
    lv_coord_t pright = lv_obj_get_style_pad_right(parent, LV_PART_MAIN);
    lv_coord_t ptop = lv_obj_get_style_pad_top(parent, LV_PART_MAIN);
    lv_coord_t pbottom = lv_obj_get_style_pad_bottom(parent, LV_PART_MAIN);

    lv_coord_t left_diff = parent->coords.x1 + pleft - obj->coords.x1;
    lv_coord_t right_diff = -(parent->coords.x2 - pright - obj->coords.x2);
    lv_coord_t top_diff = parent->coords.y1 + ptop - obj->coords.y1;
    lv_coord_t bottom_diff = -(parent->coords.y2 - pbottom - obj->coords.y2);

    lv_coord_t y_scroll = 0;
    if((top_diff > 0 || bottom_diff > 0)) {
        if(LV_ABS(top_diff) < LV_ABS(bottom_diff)) y_scroll = top_diff;
        else y_scroll = -bottom_diff;
    }

    lv_coord_t x_scroll = 0;
    if((left_diff > 0 || right_diff > 0)) {
        if(LV_ABS(left_diff) < LV_ABS(right_diff)) x_scroll = left_diff;
        else x_scroll = -right_diff;
    }

    /* Remove any pending scroll animations.*/
    lv_anim_del(parent, (lv_anim_exec_xcb_t)scroll_anim_x_cb);
    lv_anim_del(parent, (lv_anim_exec_xcb_t)scroll_anim_y_cb);

    lv_obj_scroll_by(parent, x_scroll, y_scroll, anim_en);
}

void lv_obj_scroll_to_view_recursive(lv_obj_t * obj, lv_anim_enable_t anim_en)
{
    lv_obj_t * parent = lv_obj_get_parent(obj);
    while(parent) {
        lv_obj_scroll_to_view(obj, anim_en);
        obj = parent;
        parent = lv_obj_get_parent(parent);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void scroll_anim_x_cb(lv_obj_t * obj, int32_t v)
{
    _lv_obj_scroll_by_raw(obj, v + lv_obj_get_scroll_x(obj), 0);
}

static void scroll_anim_y_cb(lv_obj_t * obj, int32_t v)
{
    _lv_obj_scroll_by_raw(obj, 0, v + lv_obj_get_scroll_y(obj));
}

static void scroll_anim_ready_cb(lv_anim_t * a)
{
    lv_res_t res = lv_signal_send(a->var, LV_SIGNAL_SCROLL_END, NULL);
    if(res != LV_RES_OK) return;

    lv_event_send(a->var, LV_EVENT_SCROLL_END, NULL);
}
