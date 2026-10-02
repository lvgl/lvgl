/**
 * @file lv_obj_pos.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_area_private.h"
#include "../lvgl_public.h"
#include "../layouts/lv_layout_private.h"
#include "lv_obj_event_private.h"
#include "lv_obj_draw_private.h"
#include "lv_obj_style_private.h"
#include "lv_obj_private.h"
#include "../display/lv_display_private.h"
#include "lv_refr_private.h"
#include "../core/lv_global.h"
#include "lv_obj_class_private.h"
#include "lv_obj_style_internal.h"
#include "../misc/lv_style_private.h"

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
static int32_t calc_content_width(lv_obj_t * obj, const lv_point_t * self_size);
static int32_t calc_content_height(lv_obj_t * obj, const lv_point_t * self_size);
static void update_children_coordinates(lv_obj_t * obj);
static void update_coordinates(lv_obj_t * obj);
static void transform_point_array(const lv_obj_t * obj, lv_point_t * p, size_t p_count, bool inv);
static bool is_transformed(const lv_obj_t * obj);
static lv_result_t invalidate_area_core(const lv_obj_t * obj, lv_area_t * area_tmp);
static lv_result_t obj_invalidate_area_internal(const lv_obj_t * obj, const lv_area_t * area);
static void obj_move_to(lv_obj_t * obj, int32_t x, int32_t y);
static bool style_width_is_content(lv_obj_t * obj);
static bool style_height_is_content(lv_obj_t * obj);

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
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_obj_set_x(obj, x);
    lv_obj_set_y(obj, y);
}

void lv_obj_set_x(lv_obj_t * obj, int32_t x)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_style_res_t res_x;
    lv_style_value_t v_x;

    res_x = lv_obj_get_local_style_prop(obj, LV_STYLE_X, &v_x, 0);

    if((res_x == LV_STYLE_RES_FOUND && v_x.num != x) || res_x == LV_STYLE_RES_NOT_FOUND) {
        lv_obj_set_style_x(obj, x, 0);
    }
}

void lv_obj_set_y(lv_obj_t * obj, int32_t y)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_style_res_t res_y;
    lv_style_value_t v_y;

    res_y = lv_obj_get_local_style_prop(obj, LV_STYLE_Y, &v_y, 0);

    if((res_y == LV_STYLE_RES_FOUND && v_y.num != y) || res_y == LV_STYLE_RES_NOT_FOUND) {
        lv_obj_set_style_y(obj, y, 0);
    }
}

int32_t lv_obj_calc_dynamic_width(lv_obj_t * obj, lv_style_prop_t prop)
{
    LV_ASSERT(prop == LV_STYLE_WIDTH || prop == LV_STYLE_MIN_WIDTH || prop == LV_STYLE_MAX_WIDTH);

    int32_t width = lv_obj_get_style_prop(obj, 0, prop).num;
    if(prop != LV_STYLE_WIDTH && width == LV_SIZE_CONTENT) {
        LV_LOG_WARN("LV_SIZE_CONTENT is not supported as min or max width, ignoring it");
        return prop == LV_STYLE_MIN_WIDTH ? 0 : LV_COORD_MAX;
    }

    if(LV_COORD_IS_PCT(width)) {
        lv_obj_t * parent = lv_obj_get_parent(obj);

        /*A screen has no parent to take a percentage of*/
        if(parent == NULL) return prop == LV_STYLE_MAX_WIDTH ? LV_COORD_MAX : 0;

        /*A min or max width is a limit on a width that is being worked out right now, so
         *there is no later point to resolve it at. A percentage of a parent that is still
         *being measured is meaningless, so drop the limit instead.*/
        if(prop != LV_STYLE_WIDTH && parent->w_content_pending) {
            return prop == LV_STYLE_MIN_WIDTH ? 0 : LV_COORD_MAX;
        }

        int32_t parent_width = lv_obj_get_content_width(parent);
        width = (LV_COORD_GET_PCT(width) * parent_width) / 100;
        width -= lv_obj_get_style_margin_left(obj, LV_PART_MAIN) + lv_obj_get_style_margin_right(obj, LV_PART_MAIN);
    }
    return width;
}

int32_t lv_obj_calc_dynamic_height(lv_obj_t * obj, lv_style_prop_t prop)
{
    LV_ASSERT(prop == LV_STYLE_HEIGHT || prop == LV_STYLE_MIN_HEIGHT || prop == LV_STYLE_MAX_HEIGHT);

    int32_t height = lv_obj_get_style_prop(obj, 0, prop).num;
    if(prop != LV_STYLE_HEIGHT && height == LV_SIZE_CONTENT) {
        LV_LOG_WARN("LV_SIZE_CONTENT is not supported as min or max height, ignoring it");
        return prop == LV_STYLE_MIN_HEIGHT ? 0 : LV_COORD_MAX;
    }

    if(LV_COORD_IS_PCT(height)) {
        lv_obj_t * parent = lv_obj_get_parent(obj);

        /*See the same cases in lv_obj_calc_dynamic_width()*/
        if(parent == NULL) return prop == LV_STYLE_MAX_HEIGHT ? LV_COORD_MAX : 0;
        if(prop != LV_STYLE_HEIGHT && parent->h_content_pending) {
            return prop == LV_STYLE_MIN_HEIGHT ? 0 : LV_COORD_MAX;
        }

        int32_t parent_height = lv_obj_get_content_height(parent);
        height = (LV_COORD_GET_PCT(height) * parent_height) / 100;
        height -= lv_obj_get_style_margin_top(obj, LV_PART_MAIN) + lv_obj_get_style_margin_bottom(obj, LV_PART_MAIN);
    }
    return height;

}

static void update_coordinates_init(lv_obj_t * obj)
{
    obj->w_layout_controlled = 0;
    obj->h_layout_controlled = 0;
    obj->child_coords_changed = 0;
    obj->size_changed = 0;

    /*If there is no parent it can't control the layout either*/
    lv_obj_t * parent = obj->parent;
    if(parent == NULL) return;

    /*If there is no layout on the parent the child size not layout controlled for sure*/
    lv_layout_t layout = lv_obj_get_style_layout(parent, 0);
    if(layout == LV_LAYOUT_NONE) return;

    /*If hidden, floating, etc, it's not affected by layout*/
    if(!lv_obj_is_layout_positioned(obj)) return;

#if LV_USE_FLEX
    if(layout == LV_LAYOUT_FLEX) {
        int32_t grow = lv_obj_get_style_flex_grow(obj, 0);
        if(grow > 0) {
            lv_flex_flow_t flow = lv_obj_get_style_flex_flow(parent, 0);
            if(flow & LV_FLEX_FLOW_COLUMN) obj->h_layout_controlled = 1;
            else obj->w_layout_controlled = 1;
        }
    }
#endif

#if LV_USE_GRID
    if(layout == LV_LAYOUT_GRID) {
        if(lv_obj_get_style_grid_cell_x_align(obj, 0) == LV_GRID_ALIGN_STRETCH) obj->w_layout_controlled = 1;
        if(lv_obj_get_style_grid_cell_y_align(obj, 0) == LV_GRID_ALIGN_STRETCH) obj->h_layout_controlled = 1;
    }
#endif
}

/**
 * Set the sizes that don't need a layout, and record whether this widget's own size is
 * `LV_SIZE_CONTENT` so that its children know it is not measured yet.
 * @param obj   pointer to a widget
 * @return      true if the size of `obj` changed
 *
 * While the parent is still being measured its size means nothing, so a percentage size is
 * taken as 0 here and resolved again once the parent's content size is known. The min and
 * max sizes still apply, so a percentage with a min size keeps that min size.
 */
static bool update_fixed_and_pct_size(lv_obj_t * obj)
{
    lv_obj_t * parent = lv_obj_get_parent(obj);

    /*Don't update layout and content width now*/
    int32_t width = lv_obj_get_style_width(obj, 0);
    int32_t height = lv_obj_get_style_height(obj, 0);
    obj->w_content_pending = obj->w_layout_controlled == 0 && width == LV_SIZE_CONTENT;
    obj->h_content_pending = obj->h_layout_controlled == 0 && height == LV_SIZE_CONTENT;

    /*Don't set the size of the screen as they always cover the whole display*/
    if(parent == NULL) return false;

    bool changed = false;

    if(obj->w_layout_controlled == 0 && width != LV_SIZE_CONTENT) {
        /*Just use the width, handle percentage values and clamp*/
        if(parent->w_content_pending && LV_COORD_IS_PCT(width)) width = 0;
        else width = lv_obj_calc_dynamic_width(obj, LV_STYLE_WIDTH);
        int32_t min_width = lv_obj_calc_dynamic_width(obj, LV_STYLE_MIN_WIDTH);
        int32_t max_width = lv_obj_calc_dynamic_width(obj, LV_STYLE_MAX_WIDTH);
        width = LV_CLAMP(min_width, width, max_width);
        if(lv_area_get_width(&obj->coords) != width) {
            lv_obj_invalidate(obj);
            lv_area_set_width(&obj->coords, width);
            obj->size_changed = 1;
            parent->child_coords_changed = 1;
            changed = true;
        }
    }

    if(obj->h_layout_controlled == 0 && height != LV_SIZE_CONTENT) {
        /*Just use the height, handle percentage values and clamp*/
        if(parent->h_content_pending && LV_COORD_IS_PCT(height)) height = 0;
        else height = lv_obj_calc_dynamic_height(obj, LV_STYLE_HEIGHT);
        int32_t min_height = lv_obj_calc_dynamic_height(obj, LV_STYLE_MIN_HEIGHT);
        int32_t max_height = lv_obj_calc_dynamic_height(obj, LV_STYLE_MAX_HEIGHT);
        height = LV_CLAMP(min_height, height, max_height);
        if(lv_area_get_height(&obj->coords) != height) {
            lv_obj_invalidate(obj);
            lv_area_set_height(&obj->coords, height);
            obj->size_changed = 1;
            parent->child_coords_changed = 1;
            changed = true;
        }
    }

    return changed;
}


void lv_obj_set_size(lv_obj_t * obj, int32_t w, int32_t h)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_obj_set_width(obj, w);
    lv_obj_set_height(obj, h);
}

void lv_obj_set_width(lv_obj_t * obj, int32_t w)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    lv_style_res_t res_w;
    lv_style_value_t v_w;

    res_w = lv_obj_get_local_style_prop(obj, LV_STYLE_WIDTH, &v_w, 0);

    if((res_w == LV_STYLE_RES_FOUND && v_w.num != w) || res_w == LV_STYLE_RES_NOT_FOUND) {
        lv_obj_set_style_width(obj, w, 0);
    }
}

void lv_obj_set_height(lv_obj_t * obj, int32_t h)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    lv_style_res_t res_h;
    lv_style_value_t v_h;

    res_h = lv_obj_get_local_style_prop(obj, LV_STYLE_HEIGHT, &v_h, 0);

    if((res_h == LV_STYLE_RES_FOUND && v_h.num != h) || res_h == LV_STYLE_RES_NOT_FOUND) {
        lv_obj_set_style_height(obj, h, 0);
    }
}

void lv_obj_set_content_width(lv_obj_t * obj, int32_t w)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    int32_t left = lv_obj_get_style_space_left_internal(obj, LV_PART_MAIN);
    int32_t right = lv_obj_get_style_space_right_internal(obj, LV_PART_MAIN);
    lv_obj_set_width(obj, w + left + right);
}

void lv_obj_set_content_height(lv_obj_t * obj, int32_t h)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    int32_t top = lv_obj_get_style_space_top_internal(obj, LV_PART_MAIN);
    int32_t bottom = lv_obj_get_style_space_bottom_internal(obj, LV_PART_MAIN);
    lv_obj_set_height(obj, h + top + bottom);
}

void lv_obj_set_layout(lv_obj_t * obj, uint32_t layout)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_obj_set_style_layout(obj, layout, 0);

    lv_obj_mark_layout_as_dirty(obj);
}

bool lv_obj_is_layout_positioned(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return false);

    if(lv_obj_is_hidden(obj) || lv_obj_is_ignore_layout(obj) || lv_obj_is_floating(obj)) return false;

    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent == NULL) return false;

    uint32_t layout = lv_obj_get_style_layout_internal(parent, LV_PART_MAIN);
    if(layout) return true;
    else return false;
}

void lv_obj_mark_layout_as_dirty(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    /*Mark the screen as dirty too to mark that there is something to do on this screen*/
    lv_obj_t * scr = lv_obj_get_screen(obj);
    if(scr->update_children_coords == 0) {
        lv_display_t * disp = lv_obj_get_display(scr);
        lv_display_send_event(disp, LV_EVENT_REFR_REQUEST, NULL);
    }
    obj->coords_invalid = 1;

    lv_obj_t * parent = obj;
    while(parent) {
        parent->update_children_coords = 1;
        parent = lv_obj_get_parent(parent);
    }
}

void lv_obj_update_layout(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    if(update_layout_mutex) {
        LV_LOG_ERROR("Layout update is already running");
        LV_ASSERT(0);
        return;
    }

    LV_PROFILER_LAYOUT_BEGIN;
    update_layout_mutex = true;

    lv_obj_t * scr = lv_obj_get_screen(obj);
    if(scr->update_children_coords) {
        LV_LOG_TRACE("Layout update begin");
        update_coordinates(scr);

        LV_LOG_TRACE("Layout update end");
    }

    update_layout_mutex = false;
    LV_PROFILER_LAYOUT_END;
}

void lv_obj_set_align(lv_obj_t * obj, lv_align_t align)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_obj_set_style_align(obj, align, 0);
}

void lv_obj_align(lv_obj_t * obj, lv_align_t align, int32_t x_ofs, int32_t y_ofs)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    lv_obj_set_style_align(obj, align, 0);
    lv_obj_set_pos(obj, x_ofs, y_ofs);
}

void lv_obj_align_to(lv_obj_t * obj, const  lv_obj_t * base, lv_align_t align, int32_t x_ofs, int32_t y_ofs)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_obj_update_layout(base);
    lv_obj_update_layout(obj);
    if(base == NULL) base = lv_obj_get_parent(obj);

    if(base == NULL) {
        LV_LOG_WARN("lv_obj_align_to: base is NULL");
        return;
    }

    int32_t x = 0;
    int32_t y = 0;

    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent == NULL) {
        LV_LOG_WARN("lv_obj_align_to: parent is NULL");
        return;
    }

    int32_t pleft = lv_obj_get_style_space_left_internal(parent, LV_PART_MAIN);
    int32_t ptop = lv_obj_get_style_space_top_internal(parent, LV_PART_MAIN);

    int32_t bleft = lv_obj_get_style_space_left_internal(base, LV_PART_MAIN);
    int32_t btop = lv_obj_get_style_space_top_internal(base, LV_PART_MAIN);

    if(align == LV_ALIGN_DEFAULT) {
        if(lv_obj_get_style_base_dir_internal(base, LV_PART_MAIN) == LV_BASE_DIR_RTL) align = LV_ALIGN_TOP_RIGHT;
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
    if(lv_obj_get_style_base_dir_internal(parent, LV_PART_MAIN) == LV_BASE_DIR_RTL) {
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
    LV_CHECK_ARG(coords != NULL, return);
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    *coords = obj->coords;
}

int32_t lv_obj_get_x(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    int32_t rel_x;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent) {
        rel_x  = obj->coords.x1 - parent->coords.x1;
        rel_x += lv_obj_get_scroll_x(parent);
        rel_x -= lv_obj_get_style_space_left_internal(parent, LV_PART_MAIN);
    }
    else {
        rel_x = obj->coords.x1;
    }
    return rel_x;
}

int32_t lv_obj_get_x2(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    return lv_obj_get_x(obj) + lv_obj_get_width(obj);
}

int32_t lv_obj_get_y(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    int32_t rel_y;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent) {
        rel_y = obj->coords.y1 - parent->coords.y1;
        rel_y += lv_obj_get_scroll_y(parent);
        rel_y -= lv_obj_get_style_space_top_internal(parent, LV_PART_MAIN);
    }
    else {
        rel_y = obj->coords.y1;
    }
    return rel_y;
}

int32_t lv_obj_get_y2(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    return lv_obj_get_y(obj) + lv_obj_get_height(obj);
}

int32_t lv_obj_get_x_aligned(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    return lv_obj_get_style_x_internal(obj, LV_PART_MAIN);
}

int32_t lv_obj_get_y_aligned(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    return lv_obj_get_style_y_internal(obj, LV_PART_MAIN);
}

int32_t lv_obj_get_width(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    return lv_area_get_width(&obj->coords);
}

int32_t lv_obj_get_height(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    return lv_area_get_height(&obj->coords);
}

int32_t lv_obj_get_content_width(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    int32_t left = lv_obj_get_style_space_left_internal(obj, LV_PART_MAIN);
    int32_t right = lv_obj_get_style_space_right_internal(obj, LV_PART_MAIN);

    return lv_obj_get_width(obj) - left - right;
}

int32_t lv_obj_get_content_height(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    int32_t top = lv_obj_get_style_space_top_internal(obj, LV_PART_MAIN);
    int32_t bottom = lv_obj_get_style_space_bottom_internal(obj, LV_PART_MAIN);

    return lv_obj_get_height(obj) - top - bottom;
}

void lv_obj_get_content_coords(const lv_obj_t * obj, lv_area_t * area)
{
    LV_CHECK_ARG(area != NULL, return);
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_obj_get_coords(obj, area);
    area->x1 += lv_obj_get_style_space_left_internal(obj, LV_PART_MAIN);
    area->x2 -= lv_obj_get_style_space_right_internal(obj, LV_PART_MAIN);
    area->y1 += lv_obj_get_style_space_top_internal(obj, LV_PART_MAIN);
    area->y2 -= lv_obj_get_style_space_bottom_internal(obj, LV_PART_MAIN);

}


int32_t lv_obj_get_self_width(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    lv_point_t p = {0, LV_COORD_MIN};
    lv_obj_send_event((lv_obj_t *)obj, LV_EVENT_GET_SELF_SIZE, &p);
    return p.x;
}

int32_t lv_obj_get_self_height(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

    lv_point_t p = {LV_COORD_MIN, 0};
    lv_obj_send_event((lv_obj_t *)obj, LV_EVENT_GET_SELF_SIZE, &p);
    return p.y;
}


static bool style_width_is_content(lv_obj_t * obj)
{
    /*A min or max width of LV_SIZE_CONTENT is not supported and is ignored, so it doesn't
     *make the width content sized either*/
    return lv_obj_get_style_width_internal(obj, LV_PART_MAIN) == LV_SIZE_CONTENT;
}

static bool style_height_is_content(lv_obj_t * obj)
{
    /*See style_width_is_content()*/
    return lv_obj_get_style_height_internal(obj, LV_PART_MAIN) == LV_SIZE_CONTENT;
}

bool lv_obj_refresh_self_size(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return false);

    if(!style_width_is_content(obj) && !style_height_is_content(obj)) {
        return false;
    }

    lv_obj_mark_layout_as_dirty(obj);
    return true;
}

/**
 * Resolve the `LV_SIZE_CONTENT` sizes of a widget from its children.
 * `update_fixed_and_pct_size()` is assumed to have set `w_content` and `h_content`.
 * @param obj   pointer to a widget
 */
static void update_content_size(lv_obj_t * obj)
{
    lv_obj_t * parent = lv_obj_get_parent(obj);
    /*Don't set the size of the screen as they always cover the whole display*/
    if(parent == NULL) return;

    /*If the LV_SIZE_CONTENT is set but the size controlled by a layout it doesn't matter*/
    bool w_content = obj->w_content_pending;
    bool h_content = obj->h_content_pending;

    /*Neither width nor height has content size. Return early to avoid getting the self size*/
    if(!w_content && !h_content) return;

    lv_point_t p = {0};
    lv_obj_send_event(obj, LV_EVENT_GET_SELF_SIZE, &p);

    /*If the width or height is set by a layout do not modify them*/
    if(w_content) {
        int32_t width = calc_content_width(obj, &p);
        int32_t minw = lv_obj_calc_dynamic_width(obj, LV_STYLE_MIN_WIDTH);
        int32_t maxw = lv_obj_calc_dynamic_width(obj, LV_STYLE_MAX_WIDTH);

        /*Use min as the upper limit if the coordinates are swapped*/
        if(minw > maxw) maxw = minw;

        width = LV_CLAMP(minw, width, maxw);

        if(lv_area_get_width(&obj->coords) != width) {
            lv_obj_invalidate(obj);
            lv_area_set_width(&obj->coords, width);

            obj->size_changed = 1;
            parent->child_coords_changed = 1;
        }
    }

    if(h_content) {
        int32_t height = calc_content_height(obj, &p);
        int32_t minh = lv_obj_calc_dynamic_height(obj, LV_STYLE_MIN_HEIGHT);
        int32_t maxh = lv_obj_calc_dynamic_height(obj, LV_STYLE_MAX_HEIGHT);

        /*Use min as the upper limit if the coordinates are swapped*/
        if(minh > maxh) maxh = minh;

        height = LV_CLAMP(minh, height, maxh);

        if(lv_area_get_height(&obj->coords) != height) {
            lv_obj_invalidate(obj);
            lv_area_set_height(&obj->coords, height);

            obj->size_changed = 1;
            parent->child_coords_changed = 1;
        }
    }
}

static void update_align(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    if(lv_obj_is_layout_positioned(obj)) return;

    lv_obj_t * parent = lv_obj_get_parent(obj);
    int32_t x = lv_obj_get_style_x_internal(obj, LV_PART_MAIN);
    int32_t y = lv_obj_get_style_y_internal(obj, LV_PART_MAIN);

    if(parent == NULL) {
        obj_move_to(obj, x, y);
        return;
    }

    /*Handle percentage value*/
    int32_t pw = lv_obj_get_content_width(parent);
    int32_t ph = lv_obj_get_content_height(parent);
    /*The parent's content size is already worked out when the children are aligned, so a
     *percentage can be resolved here. `calc_content_width/height()` counted this child as if
     *`x` and `y` were 0, so moving it now can't change the parent's size, it only lets the
     *child overflow. `*_content_pending` is a safety net: it is still set only if this runs
     *while the parent is being measured, and then there is nothing to take a percentage of.*/
    if(LV_COORD_IS_PCT(x)) {
        x = parent->w_content_pending ? 0 : (pw * LV_COORD_GET_PCT(x)) / 100;
    }

    if(LV_COORD_IS_PCT(y)) {
        y = parent->h_content_pending ? 0 : (ph * LV_COORD_GET_PCT(y)) / 100;
    }

    /*Handle percentage value of translate*/
    int32_t tr_x = lv_obj_get_style_translate_x_internal(obj, LV_PART_MAIN);
    int32_t tr_y = lv_obj_get_style_translate_y_internal(obj, LV_PART_MAIN);
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    if(LV_COORD_IS_PCT(tr_x)) tr_x = (w * LV_COORD_GET_PCT(tr_x)) / 100;
    if(LV_COORD_IS_PCT(tr_y)) tr_y = (h * LV_COORD_GET_PCT(tr_y)) / 100;

    /*Use the translation*/
    x += tr_x;
    y += tr_y;

    lv_align_t align = lv_obj_get_style_align_internal(obj, LV_PART_MAIN);

    if(align == LV_ALIGN_DEFAULT) {
        align = LV_ALIGN_TOP_LEFT;
    }

    bool rtl = lv_obj_get_style_base_dir_internal(parent, LV_PART_MAIN) == LV_BASE_DIR_RTL;

    if(rtl) {
        switch(align) {
            case LV_ALIGN_TOP_LEFT:
                align = LV_ALIGN_TOP_RIGHT;
                break;
            case LV_ALIGN_TOP_RIGHT:
                align = LV_ALIGN_TOP_LEFT;
                break;
            case LV_ALIGN_LEFT_MID:
                align = LV_ALIGN_RIGHT_MID;
                break;
            case LV_ALIGN_RIGHT_MID:
                align = LV_ALIGN_LEFT_MID;
                break;
            case LV_ALIGN_BOTTOM_LEFT:
                align = LV_ALIGN_BOTTOM_RIGHT;
                break;
            case LV_ALIGN_BOTTOM_RIGHT:
                align = LV_ALIGN_BOTTOM_LEFT;
                break;
            default:
                break;
        }
    }

    switch(align) {
        case LV_ALIGN_TOP_LEFT:
            break;
        case LV_ALIGN_TOP_MID:
            x = rtl ? pw / 2 - w / 2 - x : pw / 2 - w / 2 + x;
            break;
        case LV_ALIGN_TOP_RIGHT:
            x = rtl ? pw - w - x : pw - w + x;
            break;
        case LV_ALIGN_LEFT_MID:
            y += ph / 2 - h / 2;
            break;
        case LV_ALIGN_BOTTOM_LEFT:
            y += ph - h;
            break;
        case LV_ALIGN_BOTTOM_MID:
            x = rtl ? pw / 2 - w / 2 - x : pw / 2 - w / 2 + x;
            y += ph - h;
            break;
        case LV_ALIGN_BOTTOM_RIGHT:
            x = rtl ? pw - w - x : pw - w + x;
            y += ph - h;
            break;
        case LV_ALIGN_RIGHT_MID:
            x = rtl ? pw - w - x : pw - w + x;
            y += ph / 2 - h / 2;
            break;
        case LV_ALIGN_CENTER:
            x = rtl ? pw / 2 - w / 2 - x : pw / 2 - w / 2 + x;
            y += ph / 2 - h / 2;
            break;
        default:
            break;
    }

    obj_move_to(obj, x, y);
}

void lv_obj_transform_point(const lv_obj_t * obj, lv_point_t * p, lv_obj_point_transform_flag_t flags)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    LV_CHECK_ARG(p != NULL, return);

    lv_obj_transform_point_array(obj, p, 1, flags);
}

void lv_obj_transform_point_array(const lv_obj_t * obj, lv_point_t points[], size_t count,
                                  lv_obj_point_transform_flag_t flags)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);
    LV_CHECK_ARG(points != NULL || count == 0, return);

    lv_layer_type_t layer_type = lv_obj_get_layer_type(obj);
    bool do_tranf = layer_type == LV_LAYER_TYPE_TRANSFORM;
    bool recursive = flags & LV_OBJ_POINT_TRANSFORM_FLAG_RECURSIVE;
    bool inverse = flags & LV_OBJ_POINT_TRANSFORM_FLAG_INVERSE;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(inverse) {
        if(recursive && parent) lv_obj_transform_point_array(parent, points, count, flags);
        if(do_tranf) transform_point_array(obj, points, count, inverse);
    }
    else {
        if(do_tranf) transform_point_array(obj, points, count, inverse);
        if(recursive && parent) lv_obj_transform_point_array(parent, points, count, flags);
    }
}

void lv_obj_get_transformed_area(const lv_obj_t * obj, lv_area_t * area, lv_obj_point_transform_flag_t flags)
{
    LV_CHECK_ARG(area != NULL, return);
    LV_CHECK_OBJ(obj, MY_CLASS, return);

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

/*
 * Deferred blur invalidation expansion. A blur object samples the pixels
 * behind it, so when anything behind it changes the blur object must be
 * redrawn too. Once per frame, after all invalidations are collected, walk
 * the tree and add the full extent of any blur object overlapping an
 * invalidated area. The walk repeats until a pass adds no new areas, which
 * catches transitive cases (a blur object overlapping another blur object
 * that overlaps an invalidated area).
 *
 * Each widget's blur status is cached in obj->has_blur and counted globally in
 * blur_obj_cnt (see lv_obj_update_blur_status()), so the walk is skipped
 * entirely while no widget has blur and costs one bit test per widget when it
 * does run.
 */

static lv_obj_tree_walk_res_t blur_expand_walk_cb(lv_obj_t * obj, void * user_data)
{
    if(!obj->has_blur) return LV_OBJ_TREE_WALK_NEXT;

    /*invalidate_area_core() expects untransformed coordinates and applies the
     *transform itself*/
    lv_area_t obj_coords;
    int32_t ext_size = lv_obj_get_ext_draw_size(obj);
    obj_coords = obj->coords;
    lv_area_increase(&obj_coords, ext_size, ext_size);

    /*The invalidated areas are in screen coordinates so check the overlap
     *against the transformed area*/
    lv_area_t scr_coords;
    scr_coords = obj_coords;
    if(is_transformed(obj)) {
        lv_obj_get_transformed_area(obj, &scr_coords, LV_OBJ_POINT_TRANSFORM_FLAG_RECURSIVE);
    }

    lv_display_t * disp = user_data;
    uint32_t i;
    for(i = 0; i < disp->inv_p; i++) {
        /*The join pass runs after this and its state is reset each frame*/
        LV_ASSERT(!disp->inv_area_joined[i]);
        if(lv_area_is_on(&disp->inv_areas[i], &scr_coords)) {
            invalidate_area_core(obj, &obj_coords);

            /*No need to check the children as the widget is already invalidated
             *which will redraw the children too*/
            return LV_OBJ_TREE_WALK_SKIP_CHILDREN;
        }
    }

    return LV_OBJ_TREE_WALK_NEXT;
}

void lv_obj_invalidate_expand_blur(lv_display_t * disp)
{
    if(disp->inv_p == 0) return;

    /*There is nothing to expand if no widget has blur or drop shadow*/
    if(LV_GLOBAL_DEFAULT()->blur_obj_cnt == 0) return;

    uint32_t prev_inv_p;
    do {
        prev_inv_p = disp->inv_p;

        lv_obj_tree_walk(disp->act_scr, blur_expand_walk_cb, disp);
        if(disp->prev_scr) lv_obj_tree_walk(disp->prev_scr, blur_expand_walk_cb, disp);
        lv_obj_tree_walk(disp->sys_layer, blur_expand_walk_cb, disp);
        lv_obj_tree_walk(disp->top_layer, blur_expand_walk_cb, disp);
        lv_obj_tree_walk(disp->bottom_layer, blur_expand_walk_cb, disp);

        /*Repeat while new areas keep being added. An overflow in lv_inv_area()
         *can instead shrink inv_p (it collapses to a single whole-screen area)
         *and the loop exits, which is correct because the whole screen covers
         *every blur object.*/
    } while(disp->inv_p > prev_inv_p);
}

lv_result_t lv_obj_invalidate_area(const lv_obj_t * obj, const lv_area_t * area)
{
    LV_CHECK_ARG(area != NULL, return LV_RESULT_INVALID);
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);

    lv_display_t * disp   = lv_obj_get_display(obj);
    if(!lv_display_is_invalidation_enabled(disp)) return LV_RESULT_INVALID;

    /*If there are blurred or drop-shadow parts the whole widget needs to be invalidated
     *as these can't be calculated partially. */
    if(obj->has_blur) return lv_obj_invalidate((lv_obj_t *)obj);
    else return obj_invalidate_area_internal(obj, area);
}


lv_result_t lv_obj_invalidate(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return LV_RESULT_INVALID);

    /*Already invalidated*/
    if(obj->redraw_requested) return LV_RESULT_OK;

    lv_display_t * disp = lv_obj_get_display(obj);
    if(!lv_display_is_invalidation_enabled(disp)) return LV_RESULT_INVALID;

    obj->redraw_requested = 1;

    /*Truncate the area to the object*/
    lv_area_t obj_coords;
    int32_t ext_size = lv_obj_get_ext_draw_size(obj);
    obj_coords = obj->coords;
    obj_coords.x1 -= ext_size;
    obj_coords.y1 -= ext_size;
    obj_coords.x2 += ext_size;
    obj_coords.y2 += ext_size;

    lv_result_t res = obj_invalidate_area_internal(obj, &obj_coords);

    return res;
}

bool lv_obj_area_is_visible(const lv_obj_t * obj, lv_area_t * area)
{
    LV_CHECK_ARG(area != NULL, return false);
    LV_CHECK_OBJ(obj, MY_CLASS, return false);

    if(lv_obj_is_hidden(obj)) return false;

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
    obj_coords = obj->coords;
    lv_area_increase(&obj_coords, ext_size, ext_size);

    /*The area is not on the object*/
    if(!lv_area_intersect(area, area, &obj_coords)) return false;

    if(is_transformed(obj)) {
        lv_obj_get_transformed_area(obj, area, LV_OBJ_POINT_TRANSFORM_FLAG_RECURSIVE);
    }

    /*Truncate recursively to the parents*/
    lv_obj_t * parent = lv_obj_get_parent(obj);
    while(parent != NULL) {
        /*If the parent is hidden then the child is hidden and won't be drawn*/
        if(lv_obj_is_hidden(parent)) return false;

        /*Truncate to the parent and if no common parts break*/
        lv_area_t parent_coords = parent->coords;
        if(lv_obj_is_overflow_visible(parent)) {
            int32_t parent_ext_size = lv_obj_get_ext_draw_size(parent);
            lv_area_increase(&parent_coords, parent_ext_size, parent_ext_size);
        }

        if(is_transformed(parent)) {
            lv_obj_get_transformed_area(parent, &parent_coords, LV_OBJ_POINT_TRANSFORM_FLAG_RECURSIVE);
        }
        if(!lv_area_intersect(area, area, &parent_coords)) return false;

        parent = lv_obj_get_parent(parent);
    }

    return true;
}

bool lv_obj_is_visible(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return false);

    lv_area_t obj_coords;
    int32_t ext_size = lv_obj_get_ext_draw_size(obj);
    obj_coords = obj->coords;
    obj_coords.x1 -= ext_size;
    obj_coords.y1 -= ext_size;
    obj_coords.x2 += ext_size;
    obj_coords.y2 += ext_size;

    return lv_obj_area_is_visible(obj, &obj_coords);
}

void lv_obj_set_ext_click_area(lv_obj_t * obj, int32_t size)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    if(!lv_obj_allocate_spec_attr(obj)) {
        return;
    }
    obj->spec_attr->ext_click_pad = size;
}

void lv_obj_get_click_area(const lv_obj_t * obj, lv_area_t * area)
{
    LV_CHECK_ARG(area != NULL, return);
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    *area = obj->coords;
    if(obj->spec_attr) {
        lv_area_increase(area, obj->spec_attr->ext_click_pad, obj->spec_attr->ext_click_pad);
    }
}

bool lv_obj_hit_test(lv_obj_t * obj, const lv_point_t * point)
{
    LV_CHECK_ARG(point != NULL, return false);
    LV_CHECK_OBJ(obj, MY_CLASS, return false);

    if(!lv_obj_is_clickable(obj)) return false;

    lv_area_t a;
    lv_obj_get_click_area(obj, &a);
    bool res = lv_area_is_point_on(&a, point, 0);
    if(res == false) return false;

    if(lv_obj_is_adv_hittest(obj)) {
        lv_hit_test_info_t hit_info;
        hit_info.point = point;
        hit_info.res = true;
        lv_obj_send_event(obj, LV_EVENT_HIT_TEST, &hit_info);
        return hit_info.res;
    }

    return res;
}

void lv_obj_center(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
}

void lv_obj_set_transform(lv_obj_t * obj, const lv_matrix_t * matrix)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

#if LV_DRAW_TRANSFORM_USE_MATRIX
    if(!matrix) {
        lv_obj_reset_transform(obj);
        return;
    }

    if(!lv_obj_allocate_spec_attr(obj)) {
        return;
    }

    if(!obj->spec_attr->matrix) {
        obj->spec_attr->matrix = lv_malloc(sizeof(lv_matrix_t));
        LV_ASSERT_MALLOC(obj->spec_attr->matrix);
        if(obj->spec_attr->matrix == NULL) return;
    }

    /* Invalidate the old area */
    lv_obj_invalidate(obj);

    /* Copy the matrix */
    *obj->spec_attr->matrix = *matrix;

    /* Matrix is set. Update the layer type */
    lv_obj_update_layer_type(obj);

#else
    LV_UNUSED(obj);
    LV_UNUSED(matrix);
    LV_LOG_WARN("Transform matrix is not used because LV_DRAW_TRANSFORM_USE_MATRIX is disabled");
#endif
}

void lv_obj_reset_transform(lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

#if LV_DRAW_TRANSFORM_USE_MATRIX
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
#else
    LV_UNUSED(obj);
#endif
}

const lv_matrix_t * lv_obj_get_transform(const lv_obj_t * obj)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return NULL);

#if LV_DRAW_TRANSFORM_USE_MATRIX
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

static lv_result_t obj_invalidate_area_internal(const lv_obj_t * obj, const lv_area_t * area)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_NULL(area);

    lv_area_t area_tmp;
    area_tmp = *area;

    return invalidate_area_core(obj, &area_tmp);
}

static bool is_transformed(const lv_obj_t * obj)
{
    while(obj) {
        if(obj->spec_attr && obj->spec_attr->layer_type == LV_LAYER_TYPE_TRANSFORM) return true;
        obj = obj->parent;
    }
    return false;
}

static int32_t calc_content_width(lv_obj_t * obj, const lv_point_t * self_size)
{
    /*Assumptions:
     * - Child sizes are already set
     * - Layout positions are applied
     * - Only normal x/y/align based alignment is not set yet*/

    int32_t scroll_x_tmp = lv_obj_get_scroll_x(obj);
    if(obj->spec_attr) obj->spec_attr->scroll.x = 0;

    int32_t space_right = lv_obj_get_style_space_right_internal(obj, LV_PART_MAIN);
    int32_t space_left = lv_obj_get_style_space_left_internal(obj, LV_PART_MAIN);

    int32_t self_w = self_size ? self_size->x : lv_obj_get_self_width(obj);
    self_w += space_left + space_right;

    int32_t child_res = LV_COORD_MIN;

    {
        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_count(obj);
        /*With RTL find the left most coordinate*/
        if(lv_obj_get_style_base_dir_internal(obj, LV_PART_MAIN) == LV_BASE_DIR_RTL) {
            for(i = 0; i < child_cnt; i++) {
                int32_t child_res_tmp = LV_COORD_MIN;
                lv_obj_t * child = obj->spec_attr->children[i];
                if(child->hidden || child->floating)
                    continue;

                int32_t margins = lv_obj_get_style_margin_left_internal(child, LV_PART_MAIN)
                                  + lv_obj_get_style_margin_right_internal(child, LV_PART_MAIN);

                if(!lv_obj_is_layout_positioned(child)) {
                    lv_align_t align = lv_obj_get_style_align_internal(child, LV_PART_MAIN);
                    int32_t x = lv_obj_get_style_x_internal(child, LV_PART_MAIN);

                    /*A percentage x is resolved against this width, which is what is being
                     *worked out here, so it counts as 0*/
                    if(LV_COORD_IS_PCT(x)) x = 0;

                    switch(align) {
                        case LV_ALIGN_DEFAULT:
                        case LV_ALIGN_TOP_RIGHT:
                        case LV_ALIGN_BOTTOM_RIGHT:
                        case LV_ALIGN_RIGHT_MID:
                            /*Normal right aligns. Other are ignored due to possible circular dependencies*/
                            child_res_tmp = x + lv_area_get_width(&child->coords) + margins + space_right;
                            break;
                        default:
                            /* Consider other cases only if x=0 and use the width of the object.
                             * With x!=0 circular dependency could occur. */
                            if(x == 0) {
                                child_res_tmp = lv_area_get_width(&child->coords) + margins + space_right;
                            }
                            break;
                    }
                }
                else {
                    child_res_tmp = obj->coords.x2 - child->coords.x1 + 1;
                }
                child_res = LV_MAX(child_res, child_res_tmp);
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
                if(child->hidden || child->floating) continue;

                int32_t margins = lv_obj_get_style_margin_left_internal(child, LV_PART_MAIN)
                                  + lv_obj_get_style_margin_right_internal(child, LV_PART_MAIN);

                if(!lv_obj_is_layout_positioned(child)) {
                    lv_align_t align = lv_obj_get_style_align_internal(child, LV_PART_MAIN);
                    int32_t x = lv_obj_get_style_x_internal(child, LV_PART_MAIN);

                    /*See the RTL branch above*/
                    if(LV_COORD_IS_PCT(x)) x = 0;

                    switch(align) {
                        case LV_ALIGN_DEFAULT:
                        case LV_ALIGN_TOP_LEFT:
                        case LV_ALIGN_BOTTOM_LEFT:
                        case LV_ALIGN_LEFT_MID:
                            /*Normal left aligns.*/
                            child_res_tmp = x + lv_area_get_width(&child->coords) + margins + space_left;
                            break;
                        default:
                            /* Consider other cases only if x=0 and use the width of the object.
                             * With x!=0 circular dependency could occur. */
                            if(x == 0) {
                                child_res_tmp = lv_area_get_width(&child->coords) + margins + space_left;
                            }
                            break;
                    }
                }
                else {
                    child_res_tmp = child->coords.x2 - obj->coords.x1 + 1;
                }

                child_res = LV_MAX(child_res, child_res_tmp);
            }

            if(child_res != LV_COORD_MIN) {
                child_res += space_right;
            }
        }
    }

    if(obj->spec_attr) {
        obj->spec_attr->scroll.x = -scroll_x_tmp;
    }

    if(child_res == LV_COORD_MIN) {
        return self_w;
    }

    return LV_MAX(child_res, self_w);
}

static int32_t calc_content_height(lv_obj_t * obj, const lv_point_t * self_size)
{
    /*Assumptions:
     * - Child sizes are already set
     * - Layout positions are applied
     * - Only normal x/y/align based alignment is not set yet*/

    int32_t scroll_y_tmp = lv_obj_get_scroll_y(obj);
    if(obj->spec_attr) obj->spec_attr->scroll.y = 0;

    int32_t space_top = lv_obj_get_style_space_top_internal(obj, LV_PART_MAIN);
    int32_t space_bottom = lv_obj_get_style_space_bottom_internal(obj, LV_PART_MAIN);

    int32_t self_h = self_size ? self_size->y : lv_obj_get_self_height(obj);
    self_h += space_top + space_bottom;

    int32_t child_res = LV_COORD_MIN;

    {
        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_count(obj);
        for(i = 0; i < child_cnt; i++) {
            int32_t child_res_tmp = LV_COORD_MIN;
            lv_obj_t * child = obj->spec_attr->children[i];
            if(child->hidden || child->floating) continue;

            int32_t margins = lv_obj_get_style_margin_top_internal(child, LV_PART_MAIN)
                              + lv_obj_get_style_margin_bottom_internal(child, LV_PART_MAIN);

            if(!lv_obj_is_layout_positioned(child)) {
                lv_align_t align = lv_obj_get_style_align_internal(child, LV_PART_MAIN);
                int32_t y = lv_obj_get_style_y_internal(child, LV_PART_MAIN);

                /*A percentage y is resolved against this height, which is what is being
                 *worked out here, so it counts as 0*/
                if(LV_COORD_IS_PCT(y)) y = 0;

                switch(align) {
                    case LV_ALIGN_DEFAULT:
                    case LV_ALIGN_TOP_RIGHT:
                    case LV_ALIGN_TOP_MID:
                    case LV_ALIGN_TOP_LEFT:
                        /*Normal top aligns. */
                        child_res_tmp = y + lv_area_get_height(&child->coords) + margins + space_top;
                        break;
                    default:
                        /* Consider other cases only if y=0 and use the height of the object.
                         * With y!=0 circular dependency could occur. */
                        if(y == 0) {
                            child_res_tmp = lv_area_get_height(&child->coords) + margins + space_top;
                        }
                        break;
                }
            }
            else {
                child_res_tmp = child->coords.y2 - obj->coords.y1 + 1;
            }

            child_res = LV_MAX(child_res, child_res_tmp);
        }

        if(child_res != LV_COORD_MIN) {
            child_res += space_bottom;
        }
    }

    if(obj->spec_attr) {
        obj->spec_attr->scroll.y = -scroll_y_tmp;
    }

    if(child_res == LV_COORD_MIN) {
        return self_h;
    }

    return LV_MAX(self_h, child_res);
}

/**
 * Update the coordinates of a widget and of every widget below it. This is the entry point
 * of a layout pass, called on the screen.
 * @param obj   pointer to a widget
 */
static void update_coordinates(lv_obj_t * obj)
{
    update_coordinates_init(obj);
    update_fixed_and_pct_size(obj);

    /*It clears `coords_invalid` and `update_children_coords` on the way out*/
    update_children_coordinates(obj);

    update_align(obj);
}

/**
 * Update the size and position of the children relative to `obj`.
 * It also updates layouts and size of `obj` if it was `LV_SIZE_CONTENT` in
 * any directions. However, it doesn't update the size of `obj` if it is
 * set by a layout (e.g. flex_grow or grid cell stretch)
 * It is assumed that `update_coordinates_init(obj)` and `update_fixed_and_pct_size(obj)`
 * are called before this function. The percentage pass at the end calls it with only
 * `update_fixed_and_pct_size(child)`, on purpose: `update_coordinates_init()` would clear
 * the `size_changed` flag that tells the recursion there is work to do.
 * @param obj   pointer to a widget whose children's coordinates
 *              needs to be updated
 */
static void update_children_coordinates(lv_obj_t * obj)
{
    lv_obj_t * parent = lv_obj_get_parent(obj);
    /*The widget and its children are ok, nothing to do here*/
    if(!obj->update_children_coords &&
       !obj->child_coords_changed &&
       !obj->coords_invalid &&
       !obj->size_changed &&
       (parent && !parent->size_changed)) {
        return;
    }

    uint32_t child_cnt = lv_obj_get_child_count(obj);
    /*Step 1: Calculate what we can without layouts.*/
    for(uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * child = obj->spec_attr->children[i];
        update_coordinates_init(child);

        /*Set only fixed and percentage width and/or height now as
         *fixed doesn't depend on anything and percentage depends
         *only on the parent that is already calculated*/
        update_fixed_and_pct_size(child);

        /*Update all the children's size and position where the size
         *doesn't depend on a layout. If any of the sizes depend on the layout
         *the layout needs to set the size first to update the children*/
        if(child->w_layout_controlled == 0 && child->h_layout_controlled == 0) {
            update_children_coordinates(child);
        }
    }

    /*Step 2: Handle the simple case of layout sizing when either width or height is layout controlled
     *E.g. flex_grow*/

    /*In the first iteration of size calculation set sizes depending only on the siblings in the same direction.
     *Almost every size is set here. See the next iteration for more info. */
    if(child_cnt > 0 && (obj->child_coords_changed || obj->coords_invalid)) {
        lv_layout_update_children_sizes(obj, 0);
    }

    for(uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * child = obj->spec_attr->children[i];

        /*The layout set the sizes where it could, now it's time to calculate missing content sizes*/
        if((child->w_layout_controlled && !child->h_layout_controlled) ||
           (!child->w_layout_controlled && child->h_layout_controlled)) {
            update_children_coordinates(child);
        }
    }

    /*Step 3: Complex layout case when both width and height are layout controlled*/

    /*The second iteration of layout sizes are needed only in some special cases.
     *E.g. when a grid row has CONTENT height it needs to know the content size of
     *the children first to set the STRETCHed items' height accordingly.
     *Only grid needs 2 iterations*/
    if(child_cnt > 0 && (obj->child_coords_changed || obj->coords_invalid)) {
        lv_layout_update_children_sizes(obj, 1);
    }

    for(uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * child = obj->spec_attr->children[i];

        /*The layout sets all the sizes, now it's time to calculate missing content sizes*/
        /*Set the children size and position after this the CONTENT size can be set.*/
        if(child->w_layout_controlled && child->h_layout_controlled) {
            update_children_coordinates(child);
        }
    }

    /*Step 4: Finalizing*/

    /*All children sizes are set, now set their positions*/
    if(child_cnt > 0 && (obj->child_coords_changed || obj->coords_invalid)) {
        lv_layout_update_children_positions(obj);
    }

    /*All children are positioned too, set the content size of the widget (not the children)
     *Also consider parent->size_changed as min/max_width/height can be % which depends on the parent*/
    if(obj->child_coords_changed || obj->coords_invalid || obj->size_changed || (parent && parent->size_changed)) {
        update_content_size(obj);
    }

    /*The children sized in percent were measured as 0 so far. The size of this widget known now, so
     *resolve the children percentage sizes. This can't change the content size again: a percentage child never
     *counted towards it, so a child that is now too large simply overflows. This break the circular dependency.*/
    bool was_content_pending = obj->w_content_pending || obj->h_content_pending;

    /*The size is settled now, whether or not it changed. It tells `update_fixed_and_pct_size`
     *and `update_align` to treat this widget as a non content sized one and resolve the
     *children's percentages against it.*/
    obj->w_content_pending = 0;
    obj->h_content_pending = 0;

    /*Only the children of a widget that actually changed size have to be resolved again*/
    if(was_content_pending && obj->size_changed) {
        for(uint32_t i = 0; i < child_cnt; i++) {
            lv_obj_t * child = obj->spec_attr->children[i];
            if(update_fixed_and_pct_size(child)) {
                update_children_coordinates(child);
            }
        }
    }

    /*Content size is known, align the children*/
    for(uint32_t i = 0; i < child_cnt; i++) {
        lv_obj_t * child = obj->spec_attr->children[i];

        /*Size is set, set the position if defined by align, x, y (not layout)*/
        update_align(child);
    }

    /*If the widget was scrolled to the end and due to a layout change
     *the content got smaller make sure that the widget is scrolled inside
     *and jump back to end if needed.
     *TODO child_coords_changed might be used instead */
    if(obj->readjust_scroll_after_layout) {
        obj->readjust_scroll_after_layout = 0;
        lv_obj_readjust_scroll(obj, LV_ANIM_OFF);
    }

    if(obj->size_changed) {
        lv_obj_send_event(obj, LV_EVENT_SIZE_CHANGED, NULL);
    }

    /*All set on this widget*/
    obj->coords_invalid = 0;
    obj->child_coords_changed = 0;
    obj->update_children_coords = 0;
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

    int32_t angle = lv_obj_get_style_transform_rotation_internal(obj, LV_PART_MAIN);
    int32_t scale_x = lv_obj_get_style_transform_scale_x_safe_internal(obj, LV_PART_MAIN);
    int32_t scale_y = lv_obj_get_style_transform_scale_y_safe_internal(obj, LV_PART_MAIN);
    if(scale_x == 0) scale_x = 1;
    if(scale_y == 0) scale_y = 1;

    if(angle == 0 && scale_x == LV_SCALE_NONE && scale_y == LV_SCALE_NONE) return;

    lv_point_t pivot = {
        .x = lv_obj_get_style_transform_pivot_x_internal(obj, LV_PART_MAIN),
        .y = lv_obj_get_style_transform_pivot_y_internal(obj, LV_PART_MAIN)
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

static lv_result_t invalidate_area_core(const lv_obj_t * obj, lv_area_t * area_tmp)
{
    if(!lv_obj_area_is_visible(obj, area_tmp)) return LV_RESULT_INVALID;
#if LV_DRAW_TRANSFORM_USE_MATRIX
    /**
     * When using the global matrix, the vertex coordinates of clip_area lose precision after transformation,
     * which can be solved by expanding the redrawing area.
     */
    lv_area_increase(area_tmp, 5, 5);
#else
    if(obj->spec_attr && obj->spec_attr->layer_type == LV_LAYER_TYPE_TRANSFORM) {
        /*Make the area slightly larger to avoid rounding errors.
         *5 is an empirical value*/
        lv_area_increase(area_tmp, 5, 5);
    }
#endif

    lv_result_t res = lv_inv_area(lv_obj_get_display(obj), area_tmp);
    return res;
}

void lv_obj_move_children_by(lv_obj_t * obj, int32_t x_diff, int32_t y_diff, bool ignore_floating)
{
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_count(obj);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = obj->spec_attr->children[i];
        if(ignore_floating && lv_obj_is_floating(child)) continue;
        child->coords.x1 += x_diff;
        child->coords.y1 += y_diff;
        child->coords.x2 += x_diff;
        child->coords.y2 += y_diff;

        lv_obj_move_children_by(child, x_diff, y_diff, false);
    }
}

void obj_move_to(lv_obj_t * obj, int32_t x, int32_t y)
{
    /*Convert x and y to absolute coordinates*/
    lv_obj_t * parent = obj->parent;

    if(parent) {
        if(lv_obj_is_floating(obj)) {
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

    /*Check if the object is inside the parent or not*/
    lv_area_t parent_fit_area;
    bool on1 = false;
    if(parent) {
        lv_obj_get_content_coords(parent, &parent_fit_area);
        on1 = lv_area_is_in(&obj->coords, &parent_fit_area, 0);
    }

    obj->coords.x1 += diff.x;
    obj->coords.y1 += diff.y;
    obj->coords.x2 += diff.x;
    obj->coords.y2 += diff.y;

    lv_obj_move_children_by(obj, diff.x, diff.y, false);

    /*Invalidate if the object wasn't inside the parent before the move
     *or isn't inside it now. If it stayed inside, the scrollbars cannot change.*/
    if(parent) {
        bool on2 = lv_area_is_in(&obj->coords, &parent_fit_area, 0);
        if(!on1 || !on2) lv_obj_scrollbar_invalidate(parent);
    }
}
