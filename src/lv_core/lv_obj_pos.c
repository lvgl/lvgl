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
#define LV_OBJX_NAME "lv_obj"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool refr_size(lv_obj_t * obj, lv_coord_t w, lv_coord_t h);

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
 * Set relative the position of an object (relative to the parent)
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent plus the parent's left padding or a grid cell
 * @param y new distance from the top side of the parent  plus the parent's right padding or a grid cell
 * @note Zero value value means place the object is on the left padding of the parent, and not on the left edge.
 * @note A grid cell can be and explicit placement with cell position and span:
 *         `LV_GRID_CELL_START/END/CENTER/STRETCH(pos, span)`
 *       or "auto" to place the object on the grid in the creation order of other children
 *       `LV_GRID_AUTO_START/END/CENTER/STRETCH`
 * @note to use grid placement the parent needs have a defined grid with `lv_obj_set_grid`
 */
void lv_obj_set_pos(lv_obj_t * obj, lv_coord_t x, lv_coord_t y)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    obj->x_set = x;
    obj->y_set = y;

    bool gi = _lv_obj_is_grid_item(obj);
    bool fi = _lv_obj_is_flex_item(obj);

    /*For consistency set the size to stretched if the objects is stretched on the grid*/
    if(gi) {
        if(LV_GRID_GET_CELL_PLACE(obj->x_set) == LV_GRID_STRETCH) obj->w_set = LV_SIZE_STRETCH;
        if(LV_GRID_GET_CELL_PLACE(obj->y_set) == LV_GRID_STRETCH) obj->h_set = LV_SIZE_STRETCH;
    }

    /*If not grid or flex item but has grid or flex position set the position to 0*/
    if(!gi) {
        if(LV_COORD_IS_GRID(x)) x = 0;
        if(LV_COORD_IS_GRID(y)) y = 0;
    }

    if(!fi) {
        if(LV_COORD_IS_FLEX(x)) x = 0;
        if(LV_COORD_IS_FLEX(y)) y = 0;
    }

    /*If the object is on a grid item let the grid to position it. */
    if(gi) {
        lv_grid_item_refr_pos(obj);
    } else if(fi) {
        _lv_flex_refresh(lv_obj_get_parent(obj));
    } else {
        _lv_obj_move_to(obj, x, y, true);
    }
}

/**
 * Set the x coordinate of a object
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent plus the parent's left padding or a grid cell
 */
void lv_obj_set_x(lv_obj_t * obj, lv_coord_t x)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_set_pos(obj, x, obj->y_set);
}

/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent  plus the parent's top padding or a grid cell
 */
void lv_obj_set_y(lv_obj_t * obj, lv_coord_t y)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_set_pos(obj, obj->x_set, y);
}

/**
 * Set the size of an object.
 * @param obj pointer to an object
 * @param w new width in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 * @param h new height  in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 */
void lv_obj_set_size(lv_obj_t * obj, lv_coord_t w, lv_coord_t h)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    bool gi = _lv_obj_is_grid_item(obj);
    bool fi = _lv_obj_is_flex_item(obj);
    bool x_stretch = false;
    bool y_stretch = false;

    if(gi) {
        x_stretch = LV_GRID_GET_CELL_PLACE(obj->x_set) == LV_GRID_STRETCH ? true : false;
        y_stretch = LV_GRID_GET_CELL_PLACE(obj->y_set) == LV_GRID_STRETCH ? true : false;
        if(x_stretch) w = LV_SIZE_STRETCH;
        if(y_stretch) h = LV_SIZE_STRETCH;
    }

    obj->w_set = w;
    obj->h_set = h;

    /*If both stretched the size is managed by the grid*/
    if(x_stretch && y_stretch) return;

    if(x_stretch) w = lv_obj_get_width(obj);
    if(y_stretch) h = lv_obj_get_height(obj);

    /*Calculate the required auto sizes*/
    bool x_auto = obj->w_set == LV_SIZE_AUTO ? true : false;
    bool y_auto = obj->h_set == LV_SIZE_AUTO ? true : false;

    /*Be sure the object is not scrolled when it has auto size*/
    if(x_auto) lv_obj_scroll_to_x(obj, 0, LV_ANIM_OFF);
    if(y_auto) lv_obj_scroll_to_y(obj, 0, LV_ANIM_OFF);

    if(x_auto && y_auto) _lv_obj_calc_auto_size(obj, &w, &h);
    else if(x_auto) _lv_obj_calc_auto_size(obj, &w, NULL);
    else if(y_auto) _lv_obj_calc_auto_size(obj, NULL, &h);

    /*Calculate the required auto sizes*/
    bool pct_w = LV_COORD_IS_PCT(obj->w_set) ? true : false;
    bool pct_h = LV_COORD_IS_PCT(obj->h_set) ? true : false;

    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent) {
        lv_coord_t cont_w = lv_obj_get_width_fit(parent);
        lv_coord_t cont_h = lv_obj_get_height_fit(parent);
        if(pct_w) w = (LV_COORD_GET_PCT(obj->w_set) * cont_w) / 100;
        if(pct_h) h = (LV_COORD_GET_PCT(obj->h_set) * cont_h) / 100;
    }

    refr_size(obj, w, h);
}

/**
 * Set the width of an object
 * @param obj pointer to an object
 * @param w new width in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 */
void lv_obj_set_width(lv_obj_t * obj, lv_coord_t w)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_set_size(obj, w, obj->h_set);
}

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 */
void lv_obj_set_height(lv_obj_t * obj, lv_coord_t h)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_set_size(obj, obj->w_set, h);
}

/**
 * Set the width reduced by the left and right padding.
 * @param obj pointer to an object
 * @param w the width without paddings in pixels
 */
void lv_obj_set_content_width(lv_obj_t * obj, lv_coord_t w)
{
    lv_style_int_t pleft = lv_obj_get_style_pad_left(obj, LV_OBJ_PART_MAIN);
    lv_style_int_t pright = lv_obj_get_style_pad_right(obj, LV_OBJ_PART_MAIN);

    lv_obj_set_width(obj, w + pleft + pright);
}

/**
 * Set the height reduced by the top and bottom padding.
 * @param obj pointer to an object
 * @param h the height without paddings in pixels
 */
void lv_obj_set_content_height(lv_obj_t * obj, lv_coord_t h)
{
    lv_style_int_t ptop = lv_obj_get_style_pad_top(obj, LV_OBJ_PART_MAIN);
    lv_style_int_t pbottom = lv_obj_get_style_pad_bottom(obj, LV_OBJ_PART_MAIN);

    lv_obj_set_height(obj, h + ptop + pbottom);
}

/**
 * Set the width of an object by taking the left and right margin into account.
 * The object width will be `obj_w = w - margin_left - margin_right`
 * @param obj pointer to an object
 * @param w new height including margins in pixels
 */
void lv_obj_set_width_margin(lv_obj_t * obj, lv_coord_t w)
{
    lv_style_int_t mleft = lv_obj_get_style_margin_left(obj, LV_OBJ_PART_MAIN);
    lv_style_int_t mright = lv_obj_get_style_margin_right(obj, LV_OBJ_PART_MAIN);

    lv_obj_set_width(obj, w - mleft - mright);
}

/**
 * Set the height of an object by taking the top and bottom margin into account.
 * The object height will be `obj_h = h - margin_top - margin_bottom`
 * @param obj pointer to an object
 * @param h new height including margins in pixels
 */
void lv_obj_set_height_margin(lv_obj_t * obj, lv_coord_t h)
{
    lv_style_int_t mtop = lv_obj_get_style_margin_top(obj, LV_OBJ_PART_MAIN);
    lv_style_int_t mbottom = lv_obj_get_style_margin_bottom(obj, LV_OBJ_PART_MAIN);

    lv_obj_set_height(obj, h - mtop - mbottom);
}

/**
 * Align an object to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_ofs x coordinate offset after alignment
 * @param y_ofs y coordinate offset after alignment
 */
void lv_obj_align(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    if(base == NULL) base = lv_obj_get_parent(obj);

    LV_ASSERT_OBJ(base, LV_OBJX_NAME);

    lv_coord_t x;
    lv_coord_t y;
    switch(align) {
    case LV_ALIGN_CENTER:
        x = lv_obj_get_width_fit(base) / 2 - lv_obj_get_width_fit(obj) / 2;
        y = lv_obj_get_height_fit(base) / 2 - lv_obj_get_height_fit(obj) / 2;
        break;
    case LV_ALIGN_IN_TOP_LEFT:
        x = 0;
        y = 0;
        break;
    case LV_ALIGN_IN_TOP_MID:
        x = lv_obj_get_width_fit(base) / 2 - lv_obj_get_width_fit(obj) / 2;
        y = 0;
        break;

    case LV_ALIGN_IN_TOP_RIGHT:
        x = lv_obj_get_width(base) - lv_obj_get_width(obj);
        y = 0;
        break;

    case LV_ALIGN_IN_BOTTOM_LEFT:
        x = 0;
        y = lv_obj_get_height_fit(base) - lv_obj_get_height_fit(obj);
        break;
    case LV_ALIGN_IN_BOTTOM_MID:
        x = lv_obj_get_width_fit(base) / 2 - lv_obj_get_width_fit(obj) / 2;
        y = lv_obj_get_height_fit(base) - lv_obj_get_height_fit(obj);
        break;

    case LV_ALIGN_IN_BOTTOM_RIGHT:
        x = lv_obj_get_width_fit(base) - lv_obj_get_width_fit(obj);
        y = lv_obj_get_height_fit(base) - lv_obj_get_height_fit(obj);
        break;

    case LV_ALIGN_IN_LEFT_MID:
        x = 0;
        y = lv_obj_get_height_fit(base) / 2 - lv_obj_get_height_fit(obj) / 2;
        break;

    case LV_ALIGN_IN_RIGHT_MID:
        x = lv_obj_get_width_fit(base) - lv_obj_get_width_fit(obj);
        y = lv_obj_get_height_fit(base) / 2 - lv_obj_get_height_fit(obj) / 2;
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
        x = lv_obj_get_width_fit(base);
        y = lv_obj_get_height_fit(base) - lv_obj_get_height_fit(obj);
        break;
    }

    x += x_ofs + base->coords.x1;
    y += y_ofs + base->coords.y1;

    lv_obj_set_pos(obj, x, y);
}


/**
 * Copy the coordinates of an object to an area
 * @param obj pointer to an object
 * @param coords_out pointer to an area to store the coordinates
 */
void lv_obj_get_coords(const lv_obj_t * obj, lv_area_t * coords_out)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_area_copy(coords_out, &obj->coords);
}

/**
 * Reduce area retried by `lv_obj_get_coords()` the get graphically usable area of an object.
 * (Without the size of the border or other extra graphical elements)
 * @param coords_out store the result area here
 */
void lv_obj_get_inner_coords(const lv_obj_t * obj, lv_area_t * coords_out)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_border_side_t part = lv_obj_get_style_border_side(obj, LV_OBJ_PART_MAIN);
    lv_coord_t w = lv_obj_get_style_border_width(obj, LV_OBJ_PART_MAIN);

    if(part & LV_BORDER_SIDE_LEFT) coords_out->x1 += w;
    if(part & LV_BORDER_SIDE_RIGHT) coords_out->x2 -= w;
    if(part & LV_BORDER_SIDE_TOP) coords_out->y1 += w;
    if(part & LV_BORDER_SIDE_BOTTOM) coords_out->y2 -= w;
}

/**
 * Get the x coordinate of object.
 * @param obj pointer to an object
 * @return distance of 'obj' from the left side of its parent plus the parent's left padding
 * @note Zero return value means the object is on the left padding of the parent, and not on the left edge.
 * @note Scrolling of the parent doesn't change the returned value.
 * @note The returned value is always the distance from the parent even if the position is grid cell or other special value.
 */
lv_coord_t lv_obj_get_x(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_coord_t rel_x;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent) {
        rel_x   = obj->coords.x1 - parent->coords.x1;
        rel_x += lv_obj_get_scroll_left(parent);
        rel_x -= lv_obj_get_style_pad_left(parent, LV_OBJ_PART_MAIN);
    }
    else {
        rel_x = obj->coords.x1;
    }
    return rel_x;
}

/**
 * Get the y coordinate of object.
 * @param obj pointer to an object
 * @return distance of 'obj' from the top side of its parent plus the parent's top padding
 * @note Zero return value means the object is on the top padding of the parent, and not on the top edge.
 * @note Scrolling of the parent doesn't change the returned value.
 * @note The returned value is always the distance from the parent even if the position is grid cell or other special value.
 */
lv_coord_t lv_obj_get_y(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_coord_t rel_y;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    if(parent) {
        rel_y = obj->coords.y1 - parent->coords.y1;
        rel_y += lv_obj_get_scroll_top(parent);
        rel_y -= lv_obj_get_style_pad_top(parent, LV_OBJ_PART_MAIN);
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
 * @note The returned value is always the width in pixels even if the width is set to `LV_SIZE_AUTO` or other special value.
 */
lv_coord_t lv_obj_get_width(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return lv_area_get_width(&obj->coords);
}

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height in pixels
 * @note The returned value is always the width in pixels even if the width is set to `LV_SIZE_AUTO` or other special value.
 */
lv_coord_t lv_obj_get_height(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    return lv_area_get_height(&obj->coords);
}

/**
 * Get that width reduced by the left and right padding.
 * @param obj pointer to an object
 * @return the width which still fits into the container without causing overflow
 */
lv_coord_t lv_obj_get_width_fit(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_style_int_t left = lv_obj_get_style_pad_left(obj, LV_OBJ_PART_MAIN);
    lv_style_int_t right = lv_obj_get_style_pad_right(obj, LV_OBJ_PART_MAIN);

    return lv_obj_get_width(obj) - left - right;
}

/**
 * Get that height reduced by the top an bottom padding.
 * @param obj pointer to an object
 * @return the height which still fits into the container without causing overflow
 */
lv_coord_t lv_obj_get_height_fit(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_style_int_t top = lv_obj_get_style_pad_top((lv_obj_t *)obj, LV_OBJ_PART_MAIN);
    lv_style_int_t bottom =  lv_obj_get_style_pad_bottom((lv_obj_t *)obj, LV_OBJ_PART_MAIN);

    return lv_obj_get_height(obj) - top - bottom;
}

/**
 * Get the height of an object by taking the top and bottom margin into account.
 * The returned height will be `obj_h + margin_top + margin_bottom`
 * @param obj pointer to an object
 * @return the height including the margins
 */
lv_coord_t lv_obj_get_height_margin(const lv_obj_t * obj)
{
    lv_style_int_t mtop = lv_obj_get_style_margin_top(obj, LV_OBJ_PART_MAIN);
    lv_style_int_t mbottom = lv_obj_get_style_margin_bottom(obj, LV_OBJ_PART_MAIN);

    return lv_obj_get_height(obj) + mtop + mbottom;
}

/**
 * Get the width of an object by taking the left and right margin into account.
 * The returned width will be `obj_w + margin_left + margin_right`
 * @param obj pointer to an object
 * @return the height including the margins
 */
lv_coord_t lv_obj_get_width_margin(const lv_obj_t * obj)
{
    lv_style_int_t mleft = lv_obj_get_style_margin_left(obj, LV_OBJ_PART_MAIN);
    lv_style_int_t mright = lv_obj_get_style_margin_right(obj, LV_OBJ_PART_MAIN);

    return lv_obj_get_width(obj) + mleft + mright;
}

/**
 * Get the width of the virtual content of an object
 * @param obj pointer to an objects
 * @return the width of the virtually drawn content
 */
lv_coord_t _lv_obj_get_self_width(struct _lv_obj_t * obj)
{
    lv_point_t p = {0, LV_COORD_MIN};
    lv_signal_send((lv_obj_t * )obj, LV_SIGNAL_GET_SELF_SIZE, &p);
    return p.x;
}

/**
 * Get the height of the virtual content of an object
 * @param obj pointer to an objects
 * @return the width of the virtually drawn content
 */
lv_coord_t _lv_obj_get_self_height(struct _lv_obj_t * obj)
{
    lv_point_t p = {LV_COORD_MIN, 0};
    lv_signal_send((lv_obj_t * )obj, LV_SIGNAL_GET_SELF_SIZE, &p);
    return p.y;
}

/**
 * Handle if the size of the internal (virtual) content of an object has changed.
 * @param obj pointer to an object
 * @return false: nothing happened; true: refresh happened
 */
bool _lv_obj_handle_self_size_chg(struct _lv_obj_t * obj)
{
    if(obj->w_set != LV_SIZE_AUTO && obj->h_set == LV_SIZE_AUTO) return false;

    lv_obj_set_size(obj, obj->w_set, obj->h_set);
    return true;
}

/**
 * Calculate the "auto size". It's `auto_size = max(gird_size, children_size, self_size)`
 * @param obj pointer to an object
 * @param w_out store the width here. NULL to not calculate width
 * @param h_out store the height here. NULL to not calculate height
 */
void _lv_obj_calc_auto_size(lv_obj_t * obj, lv_coord_t * w_out, lv_coord_t * h_out)
{
    if(!w_out && !h_out) return;

//    printf("auto size\n");

    /*If no other effect the auto-size of zero by default*/
    if(w_out) *w_out = 0;
    if(h_out) *h_out = 0;

    /*Get the grid size of the object has a defined grid*/
    lv_coord_t grid_w = 0;
    lv_coord_t grid_h = 0;
    if(obj->grid) {
        _lv_grid_calc_t calc;
        _lv_grid_calc(obj, &calc);
        grid_w = calc.grid_w + lv_obj_get_style_pad_left(obj, LV_OBJ_PART_MAIN) + lv_obj_get_style_pad_right(obj, LV_OBJ_PART_MAIN);
        grid_h = calc.grid_h + lv_obj_get_style_pad_top(obj, LV_OBJ_PART_MAIN) +  lv_obj_get_style_pad_bottom(obj, LV_OBJ_PART_MAIN);
        _lv_grid_calc_free(&calc);
    }

    /*Get the children's most right and bottom position*/
    lv_coord_t children_w = 0;
    lv_coord_t children_h = 0;
    if(w_out) {
        lv_coord_t scroll_right = lv_obj_get_scroll_right(obj);
        lv_coord_t scroll_left = lv_obj_get_scroll_left(obj);
        children_w = lv_obj_get_width(obj) + scroll_right + scroll_left;
    }

    if(h_out) {
        lv_coord_t scroll_bottom = lv_obj_get_scroll_bottom(obj);
        lv_coord_t scroll_top = lv_obj_get_scroll_top(obj);
        children_h = lv_obj_get_height(obj) + scroll_bottom + scroll_top;
    }

    /*auto_size = max(gird_size, children_size)*/
    if(w_out) *w_out = LV_MATH_MAX(children_w, grid_w);
    if(h_out) *h_out = LV_MATH_MAX(children_h, grid_h);
}

/**
 * Move an object to a given x and y coordinate.
 * It's the core function to move objects but user should use `lv_obj_set_pos/x/y/..` etc.
 * @param obj pointer to an object to move
 * @param x the new x coordinate in pixels
 * @param y the new y coordinate in pixels
 * @param notify_parent true: send `LV_SIGNAL_CHILD_CHG` to the parent if `obj` moved; false: do not notify the parent
 */
void _lv_obj_move_to(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, bool notify_parent)
{
    /*Convert x and y to absolute coordinates*/
    lv_obj_t * parent = obj->parent;

    if(parent) {
        lv_coord_t pad_left = lv_obj_get_style_pad_left(parent, LV_OBJ_PART_MAIN);
        lv_coord_t pad_top = lv_obj_get_style_pad_top(parent, LV_OBJ_PART_MAIN);

        x += pad_left + parent->coords.x1 - lv_obj_get_scroll_left(parent);
        y += pad_top + parent->coords.y1 - lv_obj_get_scroll_top(parent);
    } else {
        /*If no parent then it's screen but screen can't be on a grid*/
        if(LV_COORD_IS_GRID(obj->x_set) || LV_COORD_IS_GRID(obj->x_set)) {
            obj->x_set = 0;
            obj->y_set = 0;
            x = 0;
            y = 0;
        }
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

    _lv_obj_move_children_by(obj, diff.x, diff.y);

    /*Inform the object about its new coordinates*/
    obj->signal_cb(obj, LV_SIGNAL_COORD_CHG, &ori);

    /*Send a signal to the parent too*/
    if(parent && notify_parent) parent->signal_cb(parent, LV_SIGNAL_CHILD_CHG, obj);

    /*Invalidate the new area*/
    lv_obj_invalidate(obj);
}


/**
 * Reposition the children of an object. (Called recursively)
 * @param obj pointer to an object which children will be repositioned
 * @param x_diff x coordinate shift
 * @param y_diff y coordinate shift
 */
void _lv_obj_move_children_by(lv_obj_t * obj, lv_coord_t x_diff, lv_coord_t y_diff)
{
    lv_obj_t * i;
    _LV_LL_READ(obj->child_ll, i) {
        i->coords.x1 += x_diff;
        i->coords.y1 += y_diff;
        i->coords.x2 += x_diff;
        i->coords.y2 += y_diff;

        _lv_obj_move_children_by(i, x_diff, y_diff);
    }
}

/**
 * Check if an object is valid grid item or not.
 * @param obj pointer to an object to check
 * @return true: grid item; false: not grid item
 */
bool _lv_obj_is_grid_item(lv_obj_t * obj)
{
    lv_obj_t * cont = lv_obj_get_parent(obj);
    if(cont == NULL) return false;
    if(cont->grid == NULL) return false;
    if(cont->grid->col_dsc == NULL) return false;
    if(cont->grid->row_dsc == NULL) return false;
    if(cont->grid->row_dsc_len == 0) return false;
    if(cont->grid->col_dsc_len == 0) return false;
    if(LV_COORD_IS_GRID(obj->x_set) == false || LV_COORD_IS_GRID(obj->y_set) == false) return false;
    return true;
}


/**
 * Check if an object is valid grid item or not.
 * @param obj pointer to an object to check
 * @return true: grid item; false: not grid item
 */
bool _lv_obj_is_flex_item(struct _lv_obj_t * obj)
{
    lv_obj_t * cont = lv_obj_get_parent(obj);
    if(cont == NULL) return false;
    if(cont->flex_dir == LV_FLEX_DIR_NONE) return false;
    if(LV_COORD_IS_FLEX(obj->x_set) == false || LV_COORD_IS_FLEX(obj->y_set) == false) return false;
    return true;
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

    /* Grow size is managed by the flexbox in `LV_SIGNAL_CHILD_CHG`
     * So the real current value now.
     * w or h has `LV_FLEX_GROW(x)` value which is a very large special value
     * so it should be avoided to use such a special value as width*/
    if(_LV_FLEX_GET_GROW(obj->w_set)) w = lv_obj_get_width(obj);
    if(_LV_FLEX_GET_GROW(obj->h_set)) h = lv_obj_get_height(obj);

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
    obj->signal_cb(obj, LV_SIGNAL_COORD_CHG, &ori);

    /*Send a signal to the parent too*/
    lv_obj_t * par = lv_obj_get_parent(obj);
    if(par != NULL) par->signal_cb(par, LV_SIGNAL_CHILD_CHG, obj);

    /*Invalidate the new area*/
    lv_obj_invalidate(obj);
    return true;
}

