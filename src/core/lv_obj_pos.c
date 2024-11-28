/**
 * @file lv_obj_pos.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_area_private.h"
#include "../layouts/lv_layout_private.h"
#include "lv_obj_event_private.h"
#include "lv_obj_draw_private.h"
#include "lv_obj_style_private.h"
#include "lv_obj_private.h"
#include "../display/lv_display.h"
#include "../display/lv_display_private.h"
#include "lv_refr_private.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_obj_class)
#define update_layout_mutex LV_GLOBAL_DEFAULT()->layout_update_mutex

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static int32_t calc_content_width(lv_obj_t * obj);
static int32_t calc_content_height(lv_obj_t * obj);
static void layout_update_core(lv_obj_t * obj);
static void transform_point_array(const lv_obj_t * obj, lv_point_t * p, size_t p_count, bool inv);
static bool is_transformed(const lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_obj_set_pos(lv_obj_t * obj, int32_t x, int32_t y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_set_x(obj, x);
    lv_obj_set_y(obj, y);
}

void lv_obj_set_x(lv_obj_t * obj, int32_t x)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_style_res_t res_x;
    lv_style_value_t v_x;

    res_x = lv_obj_get_local_style_prop(obj, LV_STYLE_X, &v_x, 0);

    if((res_x == LV_STYLE_RES_FOUND && v_x.num != x) || res_x == LV_STYLE_RES_NOT_FOUND) {
        lv_obj_set_style_x(obj, x, 0);
    }
}

void lv_obj_set_y(lv_obj_t * obj, int32_t y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_style_res_t res_y;
    lv_style_value_t v_y;

    res_y = lv_obj_get_local_style_prop(obj, LV_STYLE_Y, &v_y, 0);

    if((res_y == LV_STYLE_RES_FOUND && v_y.num != y) || res_y == LV_STYLE_RES_NOT_FOUND) {
        lv_obj_set_style_y(obj, y, 0);
    }
}

bool lv_obj_refr_size(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    /*If the width or height is set by a layout do not modify them*/
    if(obj->w_layout && obj->h_layout) return false;

    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent == NULL) return false;

    bool w_is_content = false;
    bool w_is_pct = false;

    int32_t w;
    if(obj->w_layout) {
        w = lv_obj_get_width(obj);
    }
    else {
        w = lv_obj_get_style_width(obj, LV_PART_MAIN);
        w_is_content = w == LV_SIZE_CONTENT;
        w_is_pct = LV_COORD_IS_PCT(w);
        int32_t parent_w = lv_obj_get_content_width(parent);

        if(w_is_content) {
            w = calc_content_width(obj);
        }
        else if(w_is_pct) {
            /*If parent has content size and the child has pct size
             *a circular dependency will occur. To solve it keep child size at zero */
            if(parent->w_layout == 0 && lv_obj_get_style_width(parent, 0) == LV_SIZE_CONTENT) {
                w = lv_obj_get_style_space_left(obj, 0) + lv_obj_get_style_space_right(obj, 0);
            }
            else {
                w = (LV_COORD_GET_PCT(w) * parent_w) / 100;
                w -= lv_obj_get_style_margin_left(obj, LV_PART_MAIN) + lv_obj_get_style_margin_right(obj, LV_PART_MAIN);
            }
        }

        int32_t minw = lv_obj_get_style_min_width(obj, LV_PART_MAIN);
        int32_t maxw = lv_obj_get_style_max_width(obj, LV_PART_MAIN);
        w = lv_clamp_width(w, minw, maxw, parent_w);
    }

    int32_t h;
    bool h_is_content = false;
    bool h_is_pct = false;
    if(obj->h_layout) {
        h = lv_obj_get_height(obj);
    }
    else {
        h = lv_obj_get_style_height(obj, LV_PART_MAIN);
        h_is_content = h == LV_SIZE_CONTENT;
        h_is_pct = LV_COORD_IS_PCT(h);
        int32_t parent_h = lv_obj_get_content_height(parent);

        if(h_is_content) {
            h = calc_content_height(obj);
        }
        else if(h_is_pct) {
            /*If parent has content size and the child has pct size
             *a circular dependency will occur. To solve it keep child size at zero */
            if(parent->h_layout == 0 && lv_obj_get_style_height(parent, 0) == LV_SIZE_CONTENT) {
                h = lv_obj_get_style_space_top(obj, 0) + lv_obj_get_style_space_bottom(obj, 0);
            }
            else {
                h = (LV_COORD_GET_PCT(h) * parent_h) / 100;
                h -= lv_obj_get_style_margin_top(obj, LV_PART_MAIN) + lv_obj_get_style_margin_bottom(obj, LV_PART_MAIN);
            }
        }

        int32_t minh = lv_obj_get_style_min_height(obj, LV_PART_MAIN);
        int32_t maxh = lv_obj_get_style_max_height(obj, LV_PART_MAIN);
        h = lv_clamp_height(h, minh, maxh, parent_h);
    }

    /*Do nothing if the size is not changed*/
    /*It is very important else recursive resizing can occur without size change*/
    if(lv_obj_get_width(obj) == w && lv_obj_get_height(obj) == h) return false;

    /*Invalidate the original area*/
    lv_obj_invalidate(obj);

    /*Save the original coordinates*/
    lv_area_t ori;
    lv_obj_get_coords(obj, &ori);

    /*Check if the object inside the parent or not*/
    lv_area_t parent_fit_area;
    lv_obj_get_content_coords(parent, &parent_fit_area);

    /*If the object is already out of the parent and its position is changes
     *surely the scrollbars also changes so invalidate them*/
    bool on1 = lv_area_is_in(&ori, &parent_fit_area, 0);
    if(!on1) lv_obj_scrollbar_invalidate(parent);

    /*Set the length and height
     *Be sure the content is not scrolled in an invalid position on the new size*/
    obj->coords.y2 = obj->coords.y1 + h - 1;
    if(lv_obj_get_style_base_dir(obj, LV_PART_MAIN) == LV_BASE_DIR_RTL) {
        obj->coords.x1 = obj->coords.x2 - w + 1;
    }
    else {
        obj->coords.x2 = obj->coords.x1 + w - 1;
    }

    /*Call the ancestor's event handler to the object with its new coordinates*/
    lv_obj_send_event(obj, LV_EVENT_SIZE_CHANGED, &ori);

    /*Call the ancestor's event handler to the parent too*/
    lv_obj_send_event(parent, LV_EVENT_CHILD_CHANGED, obj);

    /*Invalidate the new area*/
    lv_obj_invalidate(obj);

    obj->readjust_scroll_after_layout = 1;

    /*If the object was out of the parent invalidate the new scrollbar area too.
     *If it wasn't out of the parent but out now, also invalidate the scrollbars*/
    bool on2 = lv_area_is_in(&obj->coords, &parent_fit_area, 0);
    if(on1 || (!on1 && on2)) lv_obj_scrollbar_invalidate(parent);

    lv_obj_refresh_ext_draw_size(obj);

    return true;
}

void lv_obj_set_size(lv_obj_t * obj, int32_t w, int32_t h)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_set_width(obj, w);
    lv_obj_set_height(obj, h);
}

void lv_obj_set_width(lv_obj_t * obj, int32_t w)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_style_res_t res_w;
    lv_style_value_t v_w;

    res_w = lv_obj_get_local_style_prop(obj, LV_STYLE_WIDTH, &v_w, 0);

    if((res_w == LV_STYLE_RES_FOUND && v_w.num != w) || res_w == LV_STYLE_RES_NOT_FOUND) {
        lv_obj_set_style_width(obj, w, 0);
    }
}

void lv_obj_set_height(lv_obj_t * obj, int32_t h)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_style_res_t res_h;
    lv_style_value_t v_h;

    res_h = lv_obj_get_local_style_prop(obj, LV_STYLE_HEIGHT, &v_h, 0);

    if((res_h == LV_STYLE_RES_FOUND && v_h.num != h) || res_h == LV_STYLE_RES_NOT_FOUND) {
        lv_obj_set_style_height(obj, h, 0);
    }
}

void lv_obj_set_content_width(lv_obj_t * obj, int32_t w)
{
    int32_t left = lv_obj_get_style_space_left(obj, LV_PART_MAIN);
    int32_t right = lv_obj_get_style_space_right(obj, LV_PART_MAIN);
    lv_obj_set_width(obj, w + left + right);
}

void lv_obj_set_content_height(lv_obj_t * obj, int32_t h)
{
    int32_t top = lv_obj_get_style_space_top(obj, LV_PART_MAIN);
    int32_t bottom = lv_obj_get_style_space_bottom(obj, LV_PART_MAIN);
    lv_obj_set_height(obj, h + top + bottom);
}

void lv_obj_set_layout(lv_obj_t * obj, uint32_t layout)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_set_style_layout(obj, layout, 0);

    lv_obj_mark_layout_as_dirty(obj);
}

bool lv_obj_is_layout_positioned(const lv_obj_t * obj)
{
    if(lv_obj_has_flag_any(obj, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_IGNORE_LAYOUT | LV_OBJ_FLAG_FLOATING)) return false;

    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent == NULL) return false;

    uint32_t layout = lv_obj_get_style_layout(parent, LV_PART_MAIN);
    if(layout) return true;
    else return false;
}

void lv_obj_mark_layout_as_dirty(lv_obj_t * obj)
{
    obj->layout_inv = 1;

    /*Mark the screen as dirty too to mark that there is something to do on this screen*/
    lv_obj_t * scr = lv_obj_get_screen(obj);
    scr->scr_layout_inv = 1;

    /*Make the display refreshing*/
    lv_display_t * disp = lv_obj_get_display(scr);
    lv_display_send_event(disp, LV_EVENT_REFR_REQUEST, NULL);
}

void lv_obj_update_layout(const lv_obj_t * obj)
{
    if(update_layout_mutex) {
        LV_LOG_TRACE("Already running, returning");
        return;
    }
    LV_PROFILER_LAYOUT_BEGIN;
    update_layout_mutex = true;

    lv_obj_t * scr = lv_obj_get_screen(obj);
    /*Repeat until there are no more layout invalidations*/
    while(scr->scr_layout_inv) {
        LV_LOG_TRACE("Layout update begin");
        scr->scr_layout_inv = 0;
        layout_update_core(scr);
        LV_LOG_TRACE("Layout update end");
    }

    update_layout_mutex = false;
    LV_PROFILER_LAYOUT_END;
}

void lv_obj_set_align(lv_obj_t * obj, lv_align_t align)
{
    lv_obj_set_style_align(obj, align, 0);
}

void lv_obj_align(lv_obj_t * obj, lv_align_t align, int32_t x_ofs, int32_t y_ofs)
{
    lv_obj_set_style_align(obj, align, 0);
    lv_obj_set_pos(obj, x_ofs, y_ofs);
}

void lv_obj_align_to(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, int32_t x_ofs, int32_t y_ofs)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_update_layout(obj);
    if(base == NULL) base = lv_obj_get_parent(obj);

    LV_ASSERT_OBJ(base, MY_CLASS);

    int32_t x = 0;
    int32_t y = 0;

    lv_obj_t * parent = lv_obj_get_parent(obj);

    LV_ASSERT_OBJ(parent, MY_CLASS);

    int32_t pleft = lv_obj_get_style_space_left(parent, LV_PART_MAIN);
    int32_t ptop = lv_obj_get_style_space_top(parent, LV_PART_MAIN);

    int32_t bleft = lv_obj_get_style_space_left(base, LV_PART_MAIN);
    int32_t btop = lv_obj_get_style_space_top(base, LV_PART_MAIN);

    if(align == LV_ALIGN_DEFAULT) {
        if(lv_obj_get_style_base_dir(base, LV_PART_MAIN) == LV_BASE_DIR_RTL) align = LV_ALIGN_TOP_RIGHT;
        else align = LV_ALIGN_TOP_LEFT;
    }

    switch(align) {
        case LV_ALIGN_CENTER:
            x = lv_obj_get_content_width(base) / 2 - lv_obj_get_width(obj) / 2 + bleft;
            y = lv_obj_get_content_height(base) / 2 - lv_obj_get_height(obj) / 2 + btop;
            break;

        case LV_ALIGN_TOP_LEFT:
            x = bleft;
            y = btop;
            break;

        case LV_ALIGN_TOP_MID:
            x = lv_obj_get_content_width(base) / 2 - lv_obj_get_width(obj) / 2 + bleft;
            y = btop;
            break;

        case LV_ALIGN_TOP_RIGHT:
            x = lv_obj_get_content_width(base) - lv_obj_get_width(obj) + bleft;
            y = btop;
            break;

        case LV_ALIGN_BOTTOM_LEFT:
            x = bleft;
            y = lv_obj_get_content_height(base) - lv_obj_get_height(obj) + btop;
            break;
        case LV_ALIGN_BOTTOM_MID:
            x = lv_obj_get_content_width(base) / 2 - lv_obj_get_width(obj) / 2 + bleft;
            y = lv_obj_get_content_height(base) - lv_obj_get_height(obj) + btop;
            break;

        case LV_ALIGN_BOTTOM_RIGHT:
            x = lv_obj_get_content_width(base) - lv_obj_get_width(obj) + bleft;
            y = lv_obj_get_content_height(base) - lv_obj_get_height(obj) + btop;
            break;

        case LV_ALIGN_LEFT_MID:
            x = bleft;
            y = lv_obj_get_content_height(base) / 2 - lv_obj_get_height(obj) / 2 + btop;
            break;

        case LV_ALIGN_RIGHT_MID:
            x = lv_obj_get_content_width(base) - lv_obj_get_width(obj) + bleft;
            y = lv_obj_get_content_height(base) / 2 - lv_obj_get_height(obj) / 2 + btop;
            break;

        case LV_ALIGN_OUT_TOP_LEFT:
            x = 0;
            y = -lv_obj_get_height(obj);
            break;

        case LV_ALIGN_OUT_TOP_MID:
            x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2;
            y = -lv_obj_get_height(obj);
            break;

        case LV_ALIGN_OUT_TOP_RIGHT:
            x = lv_obj_get_width(base) - lv_obj_get_width(obj);
            y = -lv_obj_get_height(obj);
            break;

        case LV_ALIGN_OUT_BOTTOM_LEFT:
            x = 0;
            y = lv_obj_get_height(base);
            break;

        case LV_ALIGN_OUT_BOTTOM_MID:
            x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2;
            y = lv_obj_get_height(base);
            break;

        case LV_ALIGN_OUT_BOTTOM_RIGHT:
            x = lv_obj_get_width(base) - lv_obj_get_width(obj);
            y = lv_obj_get_height(base);
            break;

        case LV_ALIGN_OUT_LEFT_TOP:
            x = -lv_obj_get_width(obj);
            y = 0;
            break;

        case LV_ALIGN_OUT_LEFT_MID:
            x = -lv_obj_get_width(obj);
            y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2;
            break;

        case LV_ALIGN_OUT_LEFT_BOTTOM:
            x = -lv_obj_get_width(obj);
            y = lv_obj_get_height(base) - lv_obj_get_height(obj);
            break;

        case LV_ALIGN_OUT_RIGHT_TOP:
            x = lv_obj_get_width(base);
            y = 0;
            break;

        case LV_ALIGN_OUT_RIGHT_MID:
            x = lv_obj_get_width(base);
            y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2;
            break;

        case LV_ALIGN_OUT_RIGHT_BOTTOM:
            x = lv_obj_get_width(base);
            y = lv_obj_get_height(base) - lv_obj_get_height(obj);
            break;

        case LV_ALIGN_DEFAULT:
            break;
    }

    if(LV_COORD_IS_PCT(x_ofs)) x_ofs = (lv_obj_get_width(base) * LV_COORD_GET_PCT(x_ofs)) / 100;
    if(LV_COORD_IS_PCT(y_ofs)) y_ofs = (lv_obj_get_height(base) * LV_COORD_GET_PCT(y_ofs)) / 100;
    if(lv_obj_get_style_base_dir(parent, LV_PART_MAIN) == LV_BASE_DIR_RTL) {
        x += x_ofs + base->coords.x1 - parent->coords.x1 + lv_obj_get_scroll_right(parent) - pleft;
    }
    else {
        x += x_ofs + base->coords.x1 - parent->coords.x1 + lv_obj_get_scroll_left(parent) - pleft;
    }
    y += y_ofs + base->coords.y1 - parent->coords.y1 + lv_obj_get_scroll_top(parent) - ptop;
    lv_obj_set_style_align(obj, LV_ALIGN_TOP_LEFT, 0);
    lv_obj_set_pos(obj, x, y);

}

void lv_obj_get_coords(const lv_obj_t * obj, lv_area_t * coords)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_area_copy(coords, &obj->coords);
}

int32_t lv_obj_get_x(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    int32_t rel_x;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent) {
        rel_x  = obj->coords.x1 - parent->coords.x1;
        rel_x += lv_obj_get_scroll_x(parent);
        rel_x -= lv_obj_get_style_space_left(parent, LV_PART_MAIN);
    }
    else {
        rel_x = obj->coords.x1;
    }
    return rel_x;
}

int32_t lv_obj_get_x2(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return lv_obj_get_x(obj) + lv_obj_get_width(obj);
}

int32_t lv_obj_get_y(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    int32_t rel_y;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent) {
        rel_y = obj->coords.y1 - parent->coords.y1;
        rel_y += lv_obj_get_scroll_y(parent);
        rel_y -= lv_obj_get_style_space_top(parent, LV_PART_MAIN);
    }
    else {
        rel_y = obj->coords.y1;
    }
    return rel_y;
}

int32_t lv_obj_get_y2(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return lv_obj_get_y(obj) + lv_obj_get_height(obj);
}

int32_t lv_obj_get_x_aligned(const lv_obj_t * obj)
{
    return lv_obj_get_style_x(obj, LV_PART_MAIN);
}

int32_t lv_obj_get_y_aligned(const lv_obj_t * obj)
{
    return lv_obj_get_style_y(obj, LV_PART_MAIN);
}

int32_t lv_obj_get_width(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return lv_area_get_width(&obj->coords);
}

int32_t lv_obj_get_height(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return lv_area_get_height(&obj->coords);
}

int32_t lv_obj_get_content_width(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    int32_t left = lv_obj_get_style_space_left(obj, LV_PART_MAIN);
    int32_t right = lv_obj_get_style_space_right(obj, LV_PART_MAIN);

    return lv_obj_get_width(obj) - left - right;
}

int32_t lv_obj_get_content_height(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    int32_t top = lv_obj_get_style_space_top(obj, LV_PART_MAIN);
    int32_t bottom = lv_obj_get_style_space_bottom(obj, LV_PART_MAIN);

    return lv_obj_get_height(obj) - top - bottom;
}

void lv_obj_get_content_coords(const lv_obj_t * obj, lv_area_t * area)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_get_coords(obj, area);
    area->x1 += lv_obj_get_style_space_left(obj, LV_PART_MAIN);
    area->x2 -= lv_obj_get_style_space_right(obj, LV_PART_MAIN);
    area->y1 += lv_obj_get_style_space_top(obj, LV_PART_MAIN);
    area->y2 -= lv_obj_get_style_space_bottom(obj, LV_PART_MAIN);

}

int32_t lv_obj_get_self_width(const lv_obj_t * obj)
{
    lv_point_t p = {0, LV_COORD_MIN};
    lv_obj_send_event((lv_obj_t *)obj, LV_EVENT_GET_SELF_SIZE, &p);
    return p.x;
}

int32_t lv_obj_get_self_height(const lv_obj_t * obj)
{
    lv_point_t p = {LV_COORD_MIN, 0};
    lv_obj_send_event((lv_obj_t *)obj, LV_EVENT_GET_SELF_SIZE, &p);
    return p.y;
}

bool lv_obj_refresh_self_size(lv_obj_t * obj)
{
    int32_t w_set = lv_obj_get_style_width(obj, LV_PART_MAIN);
    int32_t h_set = lv_obj_get_style_height(obj, LV_PART_MAIN);
    if(w_set != LV_SIZE_CONTENT && h_set != LV_SIZE_CONTENT) return false;

    lv_obj_mark_layout_as_dirty(obj);
    return true;
}

void lv_obj_refr_pos(lv_obj_t * obj)
{
    if(lv_obj_is_layout_positioned(obj)) return;

    lv_obj_t * parent = lv_obj_get_parent(obj);
    int32_t x = lv_obj_get_style_x(obj, LV_PART_MAIN);
    int32_t y = lv_obj_get_style_y(obj, LV_PART_MAIN);

    if(parent == NULL) {
        lv_obj_move_to(obj, x, y);
        return;
    }

    /*Handle percentage value*/
    int32_t pw = lv_obj_get_content_width(parent);
    int32_t ph = lv_obj_get_content_height(parent);
    if(LV_COORD_IS_PCT(x)) {
        if(lv_obj_get_style_width(parent, 0) == LV_SIZE_CONTENT) x = 0; /*Avoid circular dependency*/
        else x = (pw * LV_COORD_GET_PCT(x)) / 100;
    }

    if(LV_COORD_IS_PCT(y)) {
        if(lv_obj_get_style_height(parent, 0) == LV_SIZE_CONTENT) y = 0; /*Avoid circular dependency*/
        y = (ph * LV_COORD_GET_PCT(y)) / 100;
    }

    /*Handle percentage value of translate*/
    int32_t tr_x = lv_obj_get_style_translate_x(obj, LV_PART_MAIN);
    int32_t tr_y = lv_obj_get_style_translate_y(obj, LV_PART_MAIN);
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    if(LV_COORD_IS_PCT(tr_x)) tr_x = (w * LV_COORD_GET_PCT(tr_x)) / 100;
    if(LV_COORD_IS_PCT(tr_y)) tr_y = (h * LV_COORD_GET_PCT(tr_y)) / 100;

    /*Use the translation*/
    x += tr_x;
    y += tr_y;

    lv_align_t align = lv_obj_get_style_align(obj, LV_PART_MAIN);

    if(align == LV_ALIGN_DEFAULT) {
        if(lv_obj_get_style_base_dir(parent, LV_PART_MAIN) == LV_BASE_DIR_RTL) align = LV_ALIGN_TOP_RIGHT;
        else align = LV_ALIGN_TOP_LEFT;
    }

    switch(align) {
        case LV_ALIGN_TOP_LEFT:
            break;
        case LV_ALIGN_TOP_MID:
            x += pw / 2 - w / 2;
            break;
        case LV_ALIGN_TOP_RIGHT:
            x += pw - w;
            break;
        case LV_ALIGN_LEFT_MID:
            y += ph / 2 - h / 2;
            break;
        case LV_ALIGN_BOTTOM_LEFT:
            y += ph - h;
            break;
        case LV_ALIGN_BOTTOM_MID:
            x += pw / 2 - w / 2;
            y += ph - h;
            break;
        case LV_ALIGN_BOTTOM_RIGHT:
            x += pw - w;
            y += ph - h;
            break;
        case LV_ALIGN_RIGHT_MID:
            x += pw - w;
            y += ph / 2 - h / 2;
            break;
        case LV_ALIGN_CENTER:
            x += pw / 2 - w / 2;
            y += ph / 2 - h / 2;
            break;
        default:
            break;
    }

    lv_obj_move_to(obj, x, y);
}

void lv_obj_move_to(lv_obj_t * obj, int32_t x, int32_t y)
{
    /*Convert x and y to absolute coordinates*/
    lv_obj_t * parent = obj->parent;

    if(parent) {
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_FLOATING)) {
            x += parent->coords.x1;
            y += parent->coords.y1;
        }
        else {
            x += parent->coords.x1 - lv_obj_get_scroll_x(parent);
            y += parent->coords.y1 - lv_obj_get_scroll_y(parent);
        }

        x += lv_obj_get_style_space_left(parent, LV_PART_MAIN);
        y += lv_obj_get_style_space_top(parent, LV_PART_MAIN);
    }

    /*Calculate and set the movement*/
    lv_point_t diff;
    diff.x = x - obj->coords.x1;
    diff.y = y - obj->coords.y1;

    /*Do nothing if the position is not changed*/
    /*It is very important else recursive positioning can
     *occur without position change*/
    if(diff.x == 0 && diff.y == 0) return;

    /*Invalidate the original area*/
    lv_obj_invalidate(obj);

    /*Save the original coordinates*/
    lv_area_t ori;
    lv_obj_get_coords(obj, &ori);

    /*Check if the object inside the parent or not*/
    lv_area_t parent_fit_area;
    bool on1 = false;
    if(parent) {
        lv_obj_get_content_coords(parent, &parent_fit_area);

        /*If the object is already out of the parent and its position is changes
         *surely the scrollbars also changes so invalidate them*/
        on1 = lv_area_is_in(&ori, &parent_fit_area, 0);
        if(!on1) lv_obj_scrollbar_invalidate(parent);
    }

    obj->coords.x1 += diff.x;
    obj->coords.y1 += diff.y;
    obj->coords.x2 += diff.x;
    obj->coords.y2 += diff.y;

    lv_obj_move_children_by(obj, diff.x, diff.y, false);

    /*Call the ancestor's event handler to the parent too*/
    if(parent) lv_obj_send_event(parent, LV_EVENT_CHILD_CHANGED, obj);

    /*Invalidate the new area*/
    lv_obj_invalidate(obj);

    /*If the object was out of the parent invalidate the new scrollbar area too.
     *If it wasn't out of the parent but out now, also invalidate the scrollbars*/
    if(parent) {
        bool on2 = lv_area_is_in(&obj->coords, &parent_fit_area, 0);
        if(on1 || (!on1 && on2)) lv_obj_scrollbar_invalidate(parent);
    }
}

void lv_obj_move_children_by(lv_obj_t * obj, int32_t x_diff, int32_t y_diff, bool ignore_floating)
{
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_count(obj);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = obj->spec_attr->children[i];
        if(ignore_floating && lv_obj_has_flag(child, LV_OBJ_FLAG_FLOATING)) continue;
        child->coords.x1 += x_diff;
        child->coords.y1 += y_diff;
        child->coords.x2 += x_diff;
        child->coords.y2 += y_diff;

        lv_obj_move_children_by(child, x_diff, y_diff, false);
    }
}

void lv_obj_transform_point(const lv_obj_t * obj, lv_point_t * p, lv_obj_point_transform_flag_t flags)
{
    lv_obj_transform_point_array(obj, p, 1, flags);
}

void lv_obj_transform_point_array(const lv_obj_t * obj, lv_point_t points[], size_t count,
                                  lv_obj_point_transform_flag_t flags)
{
    if(obj) {
        lv_layer_type_t layer_type = lv_obj_get_layer_type(obj);
        bool do_tranf = layer_type == LV_LAYER_TYPE_TRANSFORM;
        bool recursive = flags & LV_OBJ_POINT_TRANSFORM_FLAG_RECURSIVE;
        bool inverse = flags & LV_OBJ_POINT_TRANSFORM_FLAG_INVERSE;
        if(inverse) {
            if(recursive) lv_obj_transform_point_array(lv_obj_get_parent(obj), points, count, flags);
            if(do_tranf) transform_point_array(obj, points, count, inverse);
        }
        else {
            if(do_tranf) transform_point_array(obj, points, count, inverse);
            if(recursive) lv_obj_transform_point_array(lv_obj_get_parent(obj), points, count, flags);
        }
    }
}

void lv_obj_get_transformed_area(const lv_obj_t * obj, lv_area_t * area, lv_obj_point_transform_flag_t flags)
{
    lv_point_t p[4] = {
        {area->x1, area->y1},
        {area->x1, area->y2 + 1},
        {area->x2 + 1, area->y1},
        {area->x2 + 1, area->y2 + 1},
    };

    lv_obj_transform_point_array(obj, p, 4, flags);

    area->x1 = LV_MIN4(p[0].x, p[1].x, p[2].x, p[3].x);
    area->x2 = LV_MAX4(p[0].x, p[1].x, p[2].x, p[3].x);
    area->y1 = LV_MIN4(p[0].y, p[1].y, p[2].y, p[3].y);
    area->y2 = LV_MAX4(p[0].y, p[1].y, p[2].y, p[3].y);
}

void lv_obj_invalidate_area(const lv_obj_t * obj, const lv_area_t * area)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_display_t * disp   = lv_obj_get_display(obj);
    if(!lv_display_is_invalidation_enabled(disp)) return;

    lv_area_t area_tmp;
    lv_area_copy(&area_tmp, area);

    if(!lv_obj_area_is_visible(obj, &area_tmp)) return;
#if LV_DRAW_TRANSFORM_USE_MATRIX
    /**
     * When using the global matrix, the vertex coordinates of clip_area lose precision after transformation,
     * which can be solved by expanding the redrawing area.
     */
    lv_area_increase(&area_tmp, 5, 5);
#else
    if(obj->spec_attr && obj->spec_attr->layer_type == LV_LAYER_TYPE_TRANSFORM) {
        /*Make the area slightly larger to avoid rounding errors.
         *5 is an empirical value*/
        lv_area_increase(&area_tmp, 5, 5);
    }
#endif

    lv_inv_area(lv_obj_get_display(obj),  &area_tmp);
}

void lv_obj_invalidate(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    /*Truncate the area to the object*/
    lv_area_t obj_coords;
    int32_t ext_size = lv_obj_get_ext_draw_size(obj);
    lv_area_copy(&obj_coords, &obj->coords);
    obj_coords.x1 -= ext_size;
    obj_coords.y1 -= ext_size;
    obj_coords.x2 += ext_size;
    obj_coords.y2 += ext_size;

    lv_obj_invalidate_area(obj, &obj_coords);
}

bool lv_obj_area_is_visible(const lv_obj_t * obj, lv_area_t * area)
{
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_HIDDEN)) return false;

    /*Invalidate the object only if it belongs to the current or previous or one of the layers'*/
    lv_obj_t * obj_scr = lv_obj_get_screen(obj);
    lv_display_t * disp   = lv_obj_get_display(obj_scr);
    if(obj_scr != lv_display_get_screen_active(disp) &&
       obj_scr != lv_display_get_screen_prev(disp) &&
       obj_scr != lv_display_get_layer_bottom(disp) &&
       obj_scr != lv_display_get_layer_top(disp) &&
       obj_scr != lv_display_get_layer_sys(disp)) {
        return false;
    }

    /*Truncate the area to the object*/
    lv_area_t obj_coords;
    int32_t ext_size = lv_obj_get_ext_draw_size(obj);
    lv_area_copy(&obj_coords, &obj->coords);
    lv_area_increase(&obj_coords, ext_size, ext_size);

    /*The area is not on the object*/
    if(!lv_area_intersect(area, area, &obj_coords)) return false;

    if(!is_transformed(obj)) {
        *area = obj_coords;
    }
    else {
        lv_obj_get_transformed_area(obj, area, LV_OBJ_POINT_TRANSFORM_FLAG_RECURSIVE);
    }

    /*Truncate recursively to the parents*/
    lv_obj_t * parent = lv_obj_get_parent(obj);
    while(parent != NULL) {
        /*If the parent is hidden then the child is hidden and won't be drawn*/
        if(lv_obj_has_flag(parent, LV_OBJ_FLAG_HIDDEN)) return false;

        /*Truncate to the parent and if no common parts break*/
        lv_area_t parent_coords = parent->coords;
        if(lv_obj_has_flag(parent, LV_OBJ_FLAG_OVERFLOW_VISIBLE)) {
            int32_t parent_ext_size = lv_obj_get_ext_draw_size(parent);
            lv_area_increase(&parent_coords, parent_ext_size, parent_ext_size);
        }

        if(!is_transformed(parent)) {
            parent_coords = parent->coords;
        }
        else {
            lv_obj_get_transformed_area(parent, &parent_coords, LV_OBJ_POINT_TRANSFORM_FLAG_RECURSIVE);
        }
        if(!lv_area_intersect(area, area, &parent_coords)) return false;

        parent = lv_obj_get_parent(parent);
    }

    return true;
}

bool lv_obj_is_visible(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_area_t obj_coords;
    int32_t ext_size = lv_obj_get_ext_draw_size(obj);
    lv_area_copy(&obj_coords, &obj->coords);
    obj_coords.x1 -= ext_size;
    obj_coords.y1 -= ext_size;
    obj_coords.x2 += ext_size;
    obj_coords.y2 += ext_size;

    return lv_obj_area_is_visible(obj, &obj_coords);
}

void lv_obj_set_ext_click_area(lv_obj_t * obj, int32_t size)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_allocate_spec_attr(obj);
    obj->spec_attr->ext_click_pad = size;
}

void lv_obj_get_click_area(const lv_obj_t * obj, lv_area_t * area)
{
    lv_area_copy(area, &obj->coords);
    if(obj->spec_attr) {
        lv_area_increase(area, obj->spec_attr->ext_click_pad, obj->spec_attr->ext_click_pad);
    }
}

bool lv_obj_hit_test(lv_obj_t * obj, const lv_point_t * point)
{
    if(!lv_obj_has_flag(obj, LV_OBJ_FLAG_CLICKABLE)) return false;

    lv_area_t a;
    lv_obj_get_click_area(obj, &a);
    bool res = lv_area_is_point_on(&a, point, 0);
    if(res == false) return false;

    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_ADV_HITTEST)) {
        lv_hit_test_info_t hit_info;
        hit_info.point = point;
        hit_info.res = true;
        lv_obj_send_event(obj, LV_EVENT_HIT_TEST, &hit_info);
        return hit_info.res;
    }

    return res;
}

int32_t lv_clamp_width(int32_t width, int32_t min_width, int32_t max_width, int32_t ref_width)
{
    if(LV_COORD_IS_PCT(min_width)) min_width = (ref_width * LV_COORD_GET_PCT(min_width)) / 100;
    if(LV_COORD_IS_PCT(max_width)) max_width = (ref_width * LV_COORD_GET_PCT(max_width)) / 100;
    return LV_CLAMP(min_width, width, max_width);
}

int32_t lv_clamp_height(int32_t height, int32_t min_height, int32_t max_height, int32_t ref_height)
{
    if(LV_COORD_IS_PCT(min_height)) min_height = (ref_height * LV_COORD_GET_PCT(min_height)) / 100;
    if(LV_COORD_IS_PCT(max_height)) max_height = (ref_height * LV_COORD_GET_PCT(max_height)) / 100;
    return LV_CLAMP(min_height, height, max_height);
}

void lv_obj_center(lv_obj_t * obj)
{
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
}

void lv_obj_set_transform(lv_obj_t * obj, const lv_matrix_t * matrix)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(!matrix) {
        lv_obj_reset_transform(obj);
        return;
    }

    lv_obj_allocate_spec_attr(obj);
    if(!obj->spec_attr->matrix) {
        obj->spec_attr->matrix = lv_malloc(sizeof(lv_matrix_t));;
        LV_ASSERT_MALLOC(obj->spec_attr->matrix);
    }

    /* Invalidate the old area */
    lv_obj_invalidate(obj);

    /* Copy the matrix */
    *obj->spec_attr->matrix = *matrix;

    /* Matrix is set. Update the layer type */
    lv_obj_update_layer_type(obj);

    /* Invalidate the new area */
    lv_obj_invalidate(obj);
#else
    LV_UNUSED(obj);
    LV_UNUSED(matrix);
    LV_LOG_WARN("Transform matrix is not used because LV_DRAW_TRANSFORM_USE_MATRIX is disabled");
#endif
}

void lv_obj_reset_transform(lv_obj_t * obj)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(!obj->spec_attr) {
        return;
    }

    if(!obj->spec_attr->matrix) {
        return;
    }

    /* Invalidate the old area */
    lv_obj_invalidate(obj);

    /* Free the matrix */
    lv_free(obj->spec_attr->matrix);
    obj->spec_attr->matrix = NULL;

    /* Matrix is cleared. Update the layer type */
    lv_obj_update_layer_type(obj);

    /* Invalidate the new area */
    lv_obj_invalidate(obj);
#else
    LV_UNUSED(obj);
#endif
}

const lv_matrix_t * lv_obj_get_transform(const lv_obj_t * obj)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(obj->spec_attr) {
        return obj->spec_attr->matrix;
    }
#else
    LV_UNUSED(obj);
#endif
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool is_transformed(const lv_obj_t * obj)
{
    while(obj) {
        if(obj->spec_attr && obj->spec_attr->layer_type == LV_LAYER_TYPE_TRANSFORM) return true;
        obj = obj->parent;
    }
    return false;
}

static int32_t calc_content_width(lv_obj_t * obj)
{
    int32_t scroll_x_tmp = lv_obj_get_scroll_x(obj);
    if(obj->spec_attr) obj->spec_attr->scroll.x = 0;

    int32_t space_right = lv_obj_get_style_space_right(obj, LV_PART_MAIN);
    int32_t space_left = lv_obj_get_style_space_left(obj, LV_PART_MAIN);

    int32_t self_w;
    self_w = lv_obj_get_self_width(obj) + space_left + space_right;

    int32_t child_res = LV_COORD_MIN;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_count(obj);
    /*With RTL find the left most coordinate*/
    if(lv_obj_get_style_base_dir(obj, LV_PART_MAIN) == LV_BASE_DIR_RTL) {
        for(i = 0; i < child_cnt; i++) {
            int32_t child_res_tmp = LV_COORD_MIN;
            lv_obj_t * child = obj->spec_attr->children[i];
            if(lv_obj_has_flag_any(child,  LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) continue;

            if(!lv_obj_is_layout_positioned(child)) {
                lv_align_t align = lv_obj_get_style_align(child, 0);
                switch(align) {
                    case LV_ALIGN_DEFAULT:
                    case LV_ALIGN_TOP_RIGHT:
                    case LV_ALIGN_BOTTOM_RIGHT:
                    case LV_ALIGN_RIGHT_MID:
                        /*Normal right aligns. Other are ignored due to possible circular dependencies*/
                        child_res_tmp = obj->coords.x2 - child->coords.x1 + 1;
                        break;
                    default:
                        /* Consider other cases only if x=0 and use the width of the object.
                         * With x!=0 circular dependency could occur. */
                        if(lv_obj_get_style_x(child, 0) == 0) {
                            child_res_tmp = lv_area_get_width(&child->coords) + space_right;
                            child_res_tmp += lv_obj_get_style_margin_left(child, LV_PART_MAIN);
                        }
                        break;
                }
            }
            else {
                child_res_tmp = obj->coords.x2 - child->coords.x1 + 1;
            }
            child_res = LV_MAX(child_res, child_res_tmp + lv_obj_get_style_margin_left(child, LV_PART_MAIN));
        }
        if(child_res != LV_COORD_MIN) {
            child_res += space_left;
        }
    }
    /*Else find the right most coordinate*/
    else {
        for(i = 0; i < child_cnt; i++) {
            int32_t child_res_tmp = LV_COORD_MIN;
            lv_obj_t * child = obj->spec_attr->children[i];
            if(lv_obj_has_flag_any(child,  LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) continue;

            if(!lv_obj_is_layout_positioned(child)) {
                lv_align_t align = lv_obj_get_style_align(child, 0);
                switch(align) {
                    case LV_ALIGN_DEFAULT:
                    case LV_ALIGN_TOP_LEFT:
                    case LV_ALIGN_BOTTOM_LEFT:
                    case LV_ALIGN_LEFT_MID:
                        /*Normal left aligns.*/
                        child_res_tmp = child->coords.x2 - obj->coords.x1 + 1;
                        break;
                    default:
                        /* Consider other cases only if x=0 and use the width of the object.
                         * With x!=0 circular dependency could occur. */
                        if(lv_obj_get_style_x(child, 0) == 0) {
                            child_res_tmp = lv_area_get_width(&child->coords) + space_left;
                            child_res_tmp += lv_obj_get_style_margin_right(child, LV_PART_MAIN);
                        }
                        break;
                }
            }
            else {
                child_res_tmp = child->coords.x2 - obj->coords.x1 + 1;
            }

            child_res = LV_MAX(child_res, child_res_tmp + lv_obj_get_style_margin_right(child, LV_PART_MAIN));
        }

        if(child_res != LV_COORD_MIN) {
            child_res += space_right;
        }
    }

    if(obj->spec_attr) obj->spec_attr->scroll.x = -scroll_x_tmp;

    if(child_res == LV_COORD_MIN) return self_w;
    return LV_MAX(child_res, self_w);
}

static int32_t calc_content_height(lv_obj_t * obj)
{
    int32_t scroll_y_tmp = lv_obj_get_scroll_y(obj);
    if(obj->spec_attr) obj->spec_attr->scroll.y = 0;

    int32_t space_top = lv_obj_get_style_space_top(obj, LV_PART_MAIN);
    int32_t space_bottom = lv_obj_get_style_space_bottom(obj, LV_PART_MAIN);

    int32_t self_h;
    self_h = lv_obj_get_self_height(obj) + space_top + space_bottom;

    int32_t child_res = LV_COORD_MIN;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_count(obj);
    for(i = 0; i < child_cnt; i++) {
        int32_t child_res_tmp = LV_COORD_MIN;
        lv_obj_t * child = obj->spec_attr->children[i];
        if(lv_obj_has_flag_any(child,  LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) continue;

        if(!lv_obj_is_layout_positioned(child)) {
            lv_align_t align = lv_obj_get_style_align(child, 0);
            switch(align) {
                case LV_ALIGN_DEFAULT:
                case LV_ALIGN_TOP_RIGHT:
                case LV_ALIGN_TOP_MID:
                case LV_ALIGN_TOP_LEFT:
                    /*Normal top aligns. */
                    child_res_tmp = child->coords.y2 - obj->coords.y1 + 1;
                    break;
                default:
                    /* Consider other cases only if y=0 and use the height of the object.
                     * With y!=0 circular dependency could occur. */
                    if(lv_obj_get_style_y(child, 0) == 0) {
                        child_res_tmp = lv_area_get_height(&child->coords) + space_top;
                        child_res_tmp += lv_obj_get_style_margin_top(child, LV_PART_MAIN);
                    }
                    break;
            }
        }
        else {
            child_res_tmp = child->coords.y2 - obj->coords.y1 + 1;
        }

        child_res = LV_MAX(child_res, child_res_tmp + lv_obj_get_style_margin_bottom(child, LV_PART_MAIN));
    }

    if(obj->spec_attr) obj->spec_attr->scroll.y = -scroll_y_tmp;

    if(child_res == LV_COORD_MIN) return self_h;
    return LV_MAX(self_h, child_res + space_bottom);
}

static void layout_update_core(lv_obj_t * obj)
{
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_count(obj);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = obj->spec_attr->children[i];
        layout_update_core(child);
    }

    if(obj->layout_inv) {
        obj->layout_inv = 0;
        lv_obj_refr_size(obj);
        lv_obj_refr_pos(obj);

        if(child_cnt > 0) {
            lv_layout_apply(obj);
        }
    }

    if(obj->readjust_scroll_after_layout) {
        obj->readjust_scroll_after_layout = 0;
        lv_obj_readjust_scroll(obj, LV_ANIM_OFF);
    }
}

static void transform_point_array(const lv_obj_t * obj, lv_point_t * p, size_t p_count, bool inv)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    const lv_matrix_t * obj_matrix = lv_obj_get_transform(obj);
    if(obj_matrix) {
        lv_matrix_t m;
        lv_matrix_identity(&m);
        lv_matrix_translate(&m, obj->coords.x1, obj->coords.y1);
        lv_matrix_multiply(&m, obj_matrix);
        lv_matrix_translate(&m, -obj->coords.x1, -obj->coords.y1);

        if(inv) {
            lv_matrix_t inv_m;
            lv_matrix_inverse(&inv_m, &m);
            m = inv_m;
        }

        for(size_t i = 0; i < p_count; i++) {
            lv_point_precise_t p_precise = lv_point_to_precise(&p[i]);
            lv_point_precise_t res = lv_matrix_transform_precise_point(&m, &p_precise);
            p[i] = lv_point_from_precise(&res);
        }

        return;
    }
#endif /* LV_DRAW_TRANSFORM_USE_MATRIX */

    int32_t angle = lv_obj_get_style_transform_rotation(obj, 0);
    int32_t scale_x = lv_obj_get_style_transform_scale_x_safe(obj, 0);
    int32_t scale_y = lv_obj_get_style_transform_scale_y_safe(obj, 0);
    if(scale_x == 0) scale_x = 1;
    if(scale_y == 0) scale_y = 1;

    if(angle == 0 && scale_x == LV_SCALE_NONE && scale_y == LV_SCALE_NONE) return;

    lv_point_t pivot = {
        .x = lv_obj_get_style_transform_pivot_x(obj, 0),
        .y = lv_obj_get_style_transform_pivot_y(obj, 0)
    };

    if(LV_COORD_IS_PCT(pivot.x)) {
        pivot.x = (LV_COORD_GET_PCT(pivot.x) * lv_area_get_width(&obj->coords)) / 100;
    }
    if(LV_COORD_IS_PCT(pivot.y)) {
        pivot.y = (LV_COORD_GET_PCT(pivot.y) * lv_area_get_height(&obj->coords)) / 100;
    }

    pivot.x = obj->coords.x1 + pivot.x;
    pivot.y = obj->coords.y1 + pivot.y;

    if(inv) {
        angle = -angle;
        scale_x = (256 * 256 + scale_x - 1) / scale_x;
        scale_y = (256 * 256 + scale_y - 1) / scale_y;
    }

    lv_point_array_transform(p, p_count, angle, scale_x, scale_y, &pivot, !inv);
}
