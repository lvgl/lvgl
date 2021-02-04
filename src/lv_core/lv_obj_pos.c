/**
 * @file lv_obj_pos.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_obj

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool refr_size(lv_obj_t * obj, lv_coord_t w, lv_coord_t h);
static void calc_auto_size(lv_obj_t * obj, lv_coord_t * w_out, lv_coord_t * h_out);

void lv_obj_move_to(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, bool notify);
void lv_obj_move_children_by(lv_obj_t * obj, lv_coord_t x_diff, lv_coord_t y_diff);

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
 * Set the relative the position of an object (relative to the parent's top left corner)
 * @param obj: pointer to an object
 * @param x:   new distance from the left side of the parent plus the parent's left padding
 * @param y:   new distance from the top side of the parent  plus the parent's right padding
 */
void lv_obj_set_pos(lv_obj_t * obj, lv_coord_t x, lv_coord_t y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(lv_obj_is_layout_positioned(obj)) {
        LV_LOG_WARN("Can't set position because the position is set by a layout");
        return;
    }

    obj->x_set = x;
    obj->y_set = y;

    lv_obj_move_to(obj, x, y, true);
}

/**
 * Set the x coordinate of a object
 * @param obj: pointer to an object
 * @param x:   new distance from the left side from the parent plus the parent's left padding
 */
void lv_obj_set_x(lv_obj_t * obj, lv_coord_t x)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_set_pos(obj, x, obj->y_set);
}

/**
 * Set the y coordinate of a object
 * @param obj: pointer to an object
 * @param y:   new distance from the top of the parent  plus the parent's top padding
 */
void lv_obj_set_y(lv_obj_t * obj, lv_coord_t y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_set_pos(obj, obj->x_set, y);
}

/**
 * Set the size of an object.
 * @param obj: pointer to an object
 * @param w: new width in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 * @param h: new height in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 */
void lv_obj_set_size(lv_obj_t * obj, lv_coord_t w, lv_coord_t h)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    /*If the width or height is set by a layout do not modify them*/
    if(obj->w_set == LV_SIZE_LAYOUT && obj->h_set == LV_SIZE_LAYOUT) return;

    if(obj->w_set == LV_SIZE_LAYOUT) w = lv_obj_get_width(obj);
    if(obj->h_set == LV_SIZE_LAYOUT) h = lv_obj_get_height(obj);

    obj->w_set = w;
    obj->h_set = h;

    /*Calculate the required auto sizes*/
    bool x_auto = obj->w_set == LV_SIZE_AUTO ? true : false;
    bool y_auto = obj->h_set == LV_SIZE_AUTO ? true : false;

    /*Be sure the object is not scrolled when it has auto size*/
    if(x_auto) lv_obj_scroll_to_x(obj, 0, LV_ANIM_OFF);
    if(y_auto) lv_obj_scroll_to_y(obj, 0, LV_ANIM_OFF);

    if(x_auto && y_auto) calc_auto_size(obj, &w, &h);
    else if(x_auto) calc_auto_size(obj, &w, NULL);
    else if(y_auto) calc_auto_size(obj, NULL, &h);

    /*Calculate the required auto sizes*/
    bool pct_w = LV_COORD_IS_PCT(obj->w_set) ? true : false;
    bool pct_h = LV_COORD_IS_PCT(obj->h_set) ? true : false;

    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent) {
        lv_coord_t parent_w = lv_obj_get_width_fit(parent);
        lv_coord_t parent_h = lv_obj_get_height_fit(parent);
        if(pct_w) w = (LV_COORD_GET_PCT(obj->w_set) * parent_w) / 100;
        if(pct_h) h = (LV_COORD_GET_PCT(obj->h_set) * parent_h) / 100;
    }

    refr_size(obj, w, h);
}

/**
 * Set the width of an object
 * @param obj: pointer to an object
 * @param w:   new width in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 */
void lv_obj_set_width(lv_obj_t * obj, lv_coord_t w)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_set_size(obj, w, obj->h_set);
}

/**
 * Set the height of an object
 * @param obj: pointer to an object
 * @param h: new height in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 */
void lv_obj_set_height(lv_obj_t * obj, lv_coord_t h)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_set_size(obj, obj->w_set, h);
}

/**
 * Set the width reduced by the left and right padding.
 * @param obj: pointer to an object
 * @param w:   the width without paddings in pixels
 */
void lv_obj_set_content_width(lv_obj_t * obj, lv_coord_t w)
{
    lv_coord_t pleft = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t pright = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);

    lv_obj_set_width(obj, w + pleft + pright);
}

/**
 * Set the height reduced by the top and bottom padding.
 * @param obj: pointer to an object
 * @param h:   the height without paddings in pixels
 */
void lv_obj_set_content_height(lv_obj_t * obj, lv_coord_t h)
{
    lv_coord_t ptop = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t pbottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

    lv_obj_set_height(obj, h + ptop + pbottom);
}

/**
 * Set a layout for an object
 * @param obj:    pointer to an object
 * @param layout: pointer to a layout descriptor to set
 */
void lv_obj_set_layout(lv_obj_t * obj, const void * layout)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_allocate_spec_attr(obj);
    obj->spec_attr->layout_dsc = layout;

    lv_obj_update_layout(obj, NULL);
}

/**
 * Test whether the and object is positioned by a layout or not
 * @param obj: pointer to an object to test
 * @return true: positioned by a layout; false: not positioned by a layout
 */
bool lv_obj_is_layout_positioned(const lv_obj_t * obj)
{
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_LAYOUTABLE) == false) return false;

    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent == NULL) return false;
    if(parent->spec_attr && parent->spec_attr->layout_dsc) return true;
    else return false;
}

/**
 * Update the layout of an object.
 * @param cont: pointer to an object whose children needs to be updated
 * @param item: pointer to a child object that triggered the update. Set to `NULL` is not known.
 *              If not `NULL` the update process should make some optimization
 *              to update only the required parts of the layout
 */
void lv_obj_update_layout(lv_obj_t * cont, lv_obj_t * item)
{
    if(cont->spec_attr == NULL) return;
    if(cont->spec_attr->layout_dsc == NULL) return;
    if(cont->spec_attr->child_cnt == 0) return;

    const lv_layout_dsc_t * layout = cont->spec_attr->layout_dsc;
    if(layout->update_cb == NULL) return;
    layout->update_cb(cont, item);
}

/**
 * Align an object to an other object.
 * @param obj:   pointer to an object to align
 * @param base:  pointer to an other object (if NULL `obj`s parent is used). 'obj' will be aligned to it.
 * @param align: type of alignment (see 'lv_align_t' enum)
 * @param x_ofs: x coordinate offset after alignment
 * @param y_ofs: y coordinate offset after alignment
 */
void lv_obj_align(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(base == NULL) base = lv_obj_get_parent(obj);

    LV_ASSERT_OBJ(base, MY_CLASS);

    lv_coord_t x = 0;
    lv_coord_t y = 0;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    lv_coord_t pleft = lv_obj_get_style_pad_left(parent, LV_PART_MAIN);
    lv_coord_t ptop = lv_obj_get_style_pad_top(parent, LV_PART_MAIN);
    switch(align) {
    case LV_ALIGN_CENTER:
        x = lv_obj_get_width_fit(base) / 2 - lv_obj_get_width(obj) / 2;
        y = lv_obj_get_height_fit(base) / 2 - lv_obj_get_height(obj) / 2;
        break;
    case LV_ALIGN_IN_TOP_LEFT:
        x = 0;
        y = 0;
        break;
    case LV_ALIGN_IN_TOP_MID:
        x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2 - pleft;
        y = 0;
        break;

    case LV_ALIGN_IN_TOP_RIGHT:
        x = lv_obj_get_width_fit(base) - lv_obj_get_width(obj);
        y = 0;
        break;

    case LV_ALIGN_IN_BOTTOM_LEFT:
        x = 0;
        y = lv_obj_get_height_fit(base) - lv_obj_get_height(obj);
        break;
    case LV_ALIGN_IN_BOTTOM_MID:
        x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2 - pleft;
        y = lv_obj_get_height_fit(base) - lv_obj_get_height(obj);
        break;

    case LV_ALIGN_IN_BOTTOM_RIGHT:
        x = lv_obj_get_width_fit(base) - lv_obj_get_width(obj);
        y = lv_obj_get_height_fit(base) - lv_obj_get_height(obj);
        break;

    case LV_ALIGN_IN_LEFT_MID:
        x = 0;
        y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2 - pleft;
        break;

    case LV_ALIGN_IN_RIGHT_MID:
        x = lv_obj_get_width_fit(base) - lv_obj_get_width(obj);
        y = lv_obj_get_height_fit(base) / 2 - lv_obj_get_height(obj) / 2 - pleft;
        break;

    case LV_ALIGN_OUT_TOP_LEFT:
        x = -pleft;
        y = -lv_obj_get_height(obj) - ptop;
        break;

    case LV_ALIGN_OUT_TOP_MID:
        x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2 - pleft;
        y = -lv_obj_get_height(obj) - ptop;
        break;

    case LV_ALIGN_OUT_TOP_RIGHT:
        x = lv_obj_get_width(base) - lv_obj_get_width(obj) - pleft;
        y = -lv_obj_get_height(obj) - ptop;
        break;

    case LV_ALIGN_OUT_BOTTOM_LEFT:
        x = - pleft;
        y = lv_obj_get_height(base) - ptop;
        break;

    case LV_ALIGN_OUT_BOTTOM_MID:
        x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2 - pleft;
        y = lv_obj_get_height(base) - ptop;
        break;

    case LV_ALIGN_OUT_BOTTOM_RIGHT:
        x = lv_obj_get_width(base) - lv_obj_get_width(obj) - pleft;
        y = lv_obj_get_height(base) - ptop;
        break;

    case LV_ALIGN_OUT_LEFT_TOP:
        x = -lv_obj_get_width(obj) - pleft;
        y = - ptop;
        break;

    case LV_ALIGN_OUT_LEFT_MID:
        x = -lv_obj_get_width(obj) - pleft;
        y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2 - ptop;
        break;

    case LV_ALIGN_OUT_LEFT_BOTTOM:
        x = -lv_obj_get_width(obj) - pleft;
        y = lv_obj_get_height(base) - lv_obj_get_height(obj) - ptop;
        break;

    case LV_ALIGN_OUT_RIGHT_TOP:
        x = lv_obj_get_width(base) - pleft;
        y = - ptop;
        break;

    case LV_ALIGN_OUT_RIGHT_MID:
        x = lv_obj_get_width(base) - pleft;
        y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2 - ptop;
        break;

    case LV_ALIGN_OUT_RIGHT_BOTTOM:
        x = lv_obj_get_width(base) - pleft;
        y = lv_obj_get_height(base) - lv_obj_get_height(obj) - ptop;
        break;
    }

    x += x_ofs + base->coords.x1 - parent->coords.x1;
    y += y_ofs + base->coords.y1 - parent->coords.y1;

    lv_obj_set_pos(obj, x, y);
}


/**
 * Copy the coordinates of an object to an area
 * @param obj:        pointer to an object
 * @param coords: pointer to an area to store the coordinates
 */
void lv_obj_get_coords(const lv_obj_t * obj, lv_area_t * coords)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_area_copy(coords, &obj->coords);
}

/**
 * Get the x coordinate of object.
 * @param obj: pointer to an object
 * @return distance of `obj` from the left side of its parent plus the parent's left padding
 * @note Zero return value means the object is on the left padding of the parent, and not on the left edge.
 * @note Scrolling of the parent doesn't change the returned value.
 * @note The returned value is always the distance from the parent even if `obj` is positioned by a layout.
 */
lv_coord_t lv_obj_get_x(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_coord_t rel_x;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent) {
        rel_x  = obj->coords.x1 - parent->coords.x1;
        rel_x += lv_obj_get_scroll_x(parent);
        rel_x -= lv_obj_get_style_pad_left(parent, LV_PART_MAIN);
    }
    else {
        rel_x = obj->coords.x1;
    }
    return rel_x;
}

/**
 * Get the y coordinate of object.
 * @param obj: pointer to an object
 * @return distance of `obj` from the top side of its parent plus the parent's top padding
 * @note Zero return value means the object is on the top padding of the parent, and not on the top edge.
 * @note Scrolling of the parent doesn't change the returned value.
 * @note The returned value is always the distance from the parent even if `obj` is positioned by a layout.
 */
lv_coord_t lv_obj_get_y(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_coord_t rel_y;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent) {
        rel_y = obj->coords.y1 - parent->coords.y1;
        rel_y += lv_obj_get_scroll_y(parent);
        rel_y -= lv_obj_get_style_pad_top(parent, LV_PART_MAIN);
    }
    else {
        rel_y = obj->coords.y1;
    }
    return rel_y;
}

/**
 * Get the width of an object
 * @param obj pointer to an object
 * @return the width in pixels
 */
lv_coord_t lv_obj_get_width(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return lv_area_get_width(&obj->coords);
}

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height in pixels
 */
lv_coord_t lv_obj_get_height(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return lv_area_get_height(&obj->coords);
}

/**
 * Get that width reduced by the left and right padding.
 * @param obj: pointer to an object
 * @return the width which still fits into the container without causing overflow (making the object scrollable)
 */
lv_coord_t lv_obj_get_width_fit(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_coord_t left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);

    return lv_obj_get_width(obj) - left - right;
}

/**
 * Get that height reduced by the top an bottom padding.
 * @param obj: pointer to an object
 * @return the height which still fits into the container without causing overflow (making the object scrollable)
 */
lv_coord_t lv_obj_get_height_fit(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_coord_t top = lv_obj_get_style_pad_top((lv_obj_t *)obj, LV_PART_MAIN);
    lv_coord_t bottom =  lv_obj_get_style_pad_bottom((lv_obj_t *)obj, LV_PART_MAIN);

    return lv_obj_get_height(obj) - top - bottom;
}

/**
 * Get the width occupied by the "parts" of the widget. E.g. the width of all columns of a table.
 * @param obj: pointer to an objects
 * @return the width of the virtually drawn content
 * @note This size independent from the real size of the widget.
 *       It just tells how large the internal ("virtual") content is.
 */
lv_coord_t lv_obj_get_self_width(struct _lv_obj_t * obj)
{
    lv_point_t p = {0, LV_COORD_MIN};
    lv_signal_send((lv_obj_t * )obj, LV_SIGNAL_GET_SELF_SIZE, &p);
    return p.x;
}

/**
 * Get the height occupied by the "parts" of the widget. E.g. the height of all rows of a table.
 * @param obj: pointer to an objects
 * @return the width of the virtually drawn content
 * @note This size independent from the real size of the widget.
 *       It just tells how large the internal ("virtual") content is.
 */
lv_coord_t lv_obj_get_self_height(struct _lv_obj_t * obj)
{
    lv_point_t p = {LV_COORD_MIN, 0};
    lv_signal_send((lv_obj_t * )obj, LV_SIGNAL_GET_SELF_SIZE, &p);
    return p.y;
}

/**
 * Handle if the size of the internal ("virtual") content of an object has changed.
 * @param obj: pointer to an object
 * @return false: nothing happened; true: refresh happened
 */
bool lv_obj_handle_self_size_chg(struct _lv_obj_t * obj)
{
    if(obj->w_set != LV_SIZE_AUTO && obj->h_set == LV_SIZE_AUTO) return false;

    lv_obj_set_size(obj, obj->w_set, obj->h_set);
    return true;
}

/**
 * Move an object to a given x and y coordinate.
 * It's the core function to move objects. User should use `lv_obj_set_pos/x/y/..` etc.
 * @param obj:    pointer to an object to move
 * @param x:      the new x coordinate in pixels
 * @param y:      the new y coordinate in pixels
 * @param notify: true: send `LV_SIGNAL_CHILD_CHG` to the parent if `obj` moved; false: do not notify the parent
 */
void lv_obj_move_to(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, bool notify)
{
    /*Convert x and y to absolute coordinates*/
    lv_obj_t * parent = obj->parent;

    if(parent) {
        lv_coord_t pad_left = lv_obj_get_style_pad_left(parent, LV_PART_MAIN);
        lv_coord_t pad_top = lv_obj_get_style_pad_top(parent, LV_PART_MAIN);

        x += pad_left + parent->coords.x1 - lv_obj_get_scroll_x(parent);
        y += pad_top + parent->coords.y1 - lv_obj_get_scroll_y(parent);
    }

    /*Calculate and set the movement*/
    lv_point_t diff;
    diff.x = x - obj->coords.x1;
    diff.y = y - obj->coords.y1;

    /* Do nothing if the position is not changed */
    /* It is very important else recursive positioning can
     * occur without position change*/
    if(diff.x == 0 && diff.y == 0) return;

    /*Invalidate the original area*/
    lv_obj_invalidate(obj);

    /*Save the original coordinates*/
    lv_area_t ori;
    lv_obj_get_coords(obj, &ori);

    obj->coords.x1 += diff.x;
    obj->coords.y1 += diff.y;
    obj->coords.x2 += diff.x;
    obj->coords.y2 += diff.y;

    lv_obj_move_children_by(obj, diff.x, diff.y);

    /*Inform the object about its new coordinates*/
    lv_signal_send(obj, LV_SIGNAL_COORD_CHG, &ori);

    /*Send a signal to the parent too*/
    if(parent && notify) lv_signal_send(parent, LV_SIGNAL_CHILD_CHG, obj);

    /*Invalidate the new area*/
    lv_obj_invalidate(obj);
}


/**
 * Reposition the children of an object. (Called recursively)
 * It's a low level function and shouldn't be used by the user directly.
 * @param obj:    pointer to an object which children will be repositioned
 * @param x_diff: x coordinate shift
 * @param y_diff: y coordinate shift
 */
void lv_obj_move_children_by(lv_obj_t * obj, lv_coord_t x_diff, lv_coord_t y_diff)
{
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        child->coords.x1 += x_diff;
        child->coords.y1 += y_diff;
        child->coords.x2 += x_diff;
        child->coords.y2 += y_diff;

        lv_obj_move_children_by(child, x_diff, y_diff);
    }
}


void lv_obj_invalidate_area(const lv_obj_t * obj, const lv_area_t * area)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_area_t area_tmp;
    lv_area_copy(&area_tmp, area);
    bool visible = lv_obj_area_is_visible(obj, &area_tmp);

    if(visible) _lv_inv_area(lv_obj_get_disp(obj), &area_tmp);
}

void lv_obj_invalidate(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    /*Truncate the area to the object*/
    lv_area_t obj_coords;
    lv_coord_t ext_size = _lv_obj_get_ext_draw_size(obj);
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

    /*Invalidate the object only if it belongs to the current or previous'*/
    lv_obj_t * obj_scr = lv_obj_get_screen(obj);
    lv_disp_t * disp   = lv_obj_get_disp(obj_scr);
    if(obj_scr == lv_disp_get_scr_act(disp) ||
       obj_scr == lv_disp_get_scr_prev(disp) ||
       obj_scr == lv_disp_get_layer_top(disp) ||
       obj_scr == lv_disp_get_layer_sys(disp)) {

        /*Truncate the area to the object*/
        lv_area_t obj_coords;
        lv_coord_t ext_size = _lv_obj_get_ext_draw_size(obj);
        lv_area_copy(&obj_coords, &obj->coords);
        obj_coords.x1 -= ext_size;
        obj_coords.y1 -= ext_size;
        obj_coords.x2 += ext_size;
        obj_coords.y2 += ext_size;

        bool is_common;

        is_common = _lv_area_intersect(area, area, &obj_coords);
        if(is_common == false) return false;  /*The area is not on the object*/

        /*Truncate recursively to the parents*/
        lv_obj_t * par = lv_obj_get_parent(obj);
        while(par != NULL) {
            is_common = _lv_area_intersect(area, area, &par->coords);
            if(is_common == false) return false;       /*If no common parts with parent break;*/
            if(lv_obj_has_flag(par, LV_OBJ_FLAG_HIDDEN)) return false; /*If the parent is hidden then the child is hidden and won't be drawn*/

            par = lv_obj_get_parent(par);
        }
    }

    return true;
}

bool lv_obj_is_visible(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_area_t obj_coords;
    lv_coord_t ext_size = _lv_obj_get_ext_draw_size(obj);
    lv_area_copy(&obj_coords, &obj->coords);
    obj_coords.x1 -= ext_size;
    obj_coords.y1 -= ext_size;
    obj_coords.x2 += ext_size;
    obj_coords.y2 += ext_size;

    return lv_obj_area_is_visible(obj, &obj_coords);

}

void lv_obj_set_ext_click_area(lv_obj_t * obj, lv_coord_t size)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_allocate_spec_attr(obj);
    obj->spec_attr->ext_click_pad = size;
}

void lv_obj_get_click_area(const lv_obj_t * obj, lv_area_t * area)
{
    lv_area_copy(area, &obj->coords);
    if(obj->spec_attr) {
        area->x1 -= obj->spec_attr->ext_click_pad;
        area->x2 += obj->spec_attr->ext_click_pad;
        area->y1 -= obj->spec_attr->ext_click_pad;
        area->y2 += obj->spec_attr->ext_click_pad;
    }
}

bool lv_obj_hit_test(lv_obj_t * obj, const lv_point_t * point)
{
    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_ADV_HITTEST)) {
        lv_hit_test_info_t hit_info;
        hit_info.point = point;
        hit_info.result = true;
        lv_signal_send(obj, LV_SIGNAL_HIT_TEST, &hit_info);
        return hit_info.result;
    }
    else {
        lv_area_t a;
        lv_obj_get_click_area(obj, &a);
        return _lv_area_is_point_on(&a, point, 0);
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Set the size of an object.
 * It's the core function to set the size of objects but user should use `lv_obj_set_size/width/height/..` etc.
 * @param obj pointer to an object
 * @param w the new width in pixels
 * @param h the new height in pixels
 * @return true: the size was changed; false: `w` and `h` was equal to the current width and height so nothing happened.
 */
static bool refr_size(lv_obj_t * obj, lv_coord_t w, lv_coord_t h)
{
    /* If the size is managed by the layout don't let to overwrite it.*/
    if(obj->w_set == LV_SIZE_LAYOUT) w = lv_obj_get_width(obj);
    if(obj->h_set == LV_SIZE_LAYOUT) h = lv_obj_get_height(obj);

    /* Do nothing if the size is not changed */
    /* It is very important else recursive resizing can
     * occur without size change*/
    if(lv_obj_get_width(obj) == w && lv_obj_get_height(obj) == h) {
        return false;
    }
    /*Invalidate the original area*/
    lv_obj_invalidate(obj);

    /*Save the original coordinates*/
    lv_area_t ori;
    lv_obj_get_coords(obj, &ori);

    /* Set the length and height
     * Be sure the content is not scrolled in an invalid position on the new size*/
    obj->coords.y2 = obj->coords.y1 + h - 1;
    if(lv_obj_get_base_dir(obj) == LV_BIDI_DIR_RTL) {
        obj->coords.x1 = obj->coords.x2 - w + 1;
    }
    else {
        obj->coords.x2 = obj->coords.x1 + w - 1;
    }

    /*Send a signal to the object with its new coordinates*/
    lv_signal_send(obj, LV_SIGNAL_COORD_CHG, &ori);

    /*Send a signal to the parent too*/
    lv_obj_t * par = lv_obj_get_parent(obj);
    if(par != NULL) lv_signal_send(par, LV_SIGNAL_CHILD_CHG, obj);

    /*Invalidate the new area*/
    lv_obj_invalidate(obj);
    return true;
}


/**
 * Calculate the "auto size". It's `auto_size = max(children_size, self_size)`
 * @param obj pointer to an object
 * @param w_out store the width here. NULL to not calculate width
 * @param h_out store the height here. NULL to not calculate height
 */
static void calc_auto_size(lv_obj_t * obj, lv_coord_t * w_out, lv_coord_t * h_out)
{
    if(!w_out && !h_out) return;
    /*Get the bounding box of the children*/
    if(w_out) {
        lv_coord_t scroll_right = lv_obj_get_scroll_right(obj);
        lv_coord_t scroll_left = lv_obj_get_scroll_left(obj);
        *w_out = lv_obj_get_width(obj) + scroll_right + scroll_left;
    }

    if(h_out) {
        lv_coord_t scroll_bottom = lv_obj_get_scroll_bottom(obj);
        lv_coord_t scroll_top = lv_obj_get_scroll_top(obj);
        *h_out = lv_obj_get_height(obj) + scroll_bottom + scroll_top;
    }
}
