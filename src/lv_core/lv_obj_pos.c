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

    /*For consistency set the size to stretched if the objects is stretched on the grid*/
    if(gi) {
        if(_GRID_GET_CELL_FLAG(obj->x_set) == LV_GRID_STRETCH) obj->w_set = LV_SIZE_STRETCH;
        if(_GRID_GET_CELL_FLAG(obj->y_set) == LV_GRID_STRETCH) obj->h_set = LV_SIZE_STRETCH;
    }

    /*If not grid item but has grid position set the position to 0*/
    if(!gi) {
        if(_GRID_IS_CELL(x)) {
            obj->x_set = 0;
            x = 0;
        }
        if(_GRID_IS_CELL(y)) {
            obj->y_set = 0;
            y = 0;
        }
    }

    /*If the object is on a grid item let the grid to position it. */
    if(gi) {
        lv_area_t old_area;
        lv_area_copy(&old_area, &obj->coords);
        lv_grid_item_refr_pos(obj);

        lv_obj_t * cont = lv_obj_get_parent(obj);

        /*If the item was moved and grid is implicit in the changed direction refresh the whole grid.*/
        if((cont->grid->col_dsc == NULL && (old_area.x1 != obj->coords.x1 || old_area.x2 != obj->coords.x2)) ||
           (cont->grid->row_dsc == NULL && (old_area.y1 != obj->coords.y1 || old_area.y2 != obj->coords.y2)))
        {
            lv_grid_full_refr(cont);
        }
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

    lv_obj_set_pos(obj, x, lv_obj_get_y(obj));
}

/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent  plus the parent's top padding or a grid cell
 */
void lv_obj_set_y(lv_obj_t * obj, lv_coord_t y)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_set_pos(obj, lv_obj_get_x(obj), y);
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
    bool x_stretch = false;
    bool y_stretch = false;

    if(gi) {
        x_stretch = _GRID_GET_CELL_FLAG(obj->x_set) == LV_GRID_STRETCH ? true : false;
        y_stretch = _GRID_GET_CELL_FLAG(obj->y_set) == LV_GRID_STRETCH ? true : false;
        if(x_stretch) w = LV_SIZE_STRETCH;
        if(y_stretch) h = LV_SIZE_STRETCH;
    }

    obj->w_set = w;
    obj->h_set = h;

    /*If both stretched it was already managed by the grid*/
    if(x_stretch && y_stretch) return;

    if(x_stretch) w = lv_obj_get_width(obj);
    if(y_stretch) h = lv_obj_get_height(obj);

    /*Calculate the required auto sizes*/
    bool x_auto = obj->w_set == LV_SIZE_AUTO ? true : false;
    bool y_auto = obj->h_set == LV_SIZE_AUTO ? true : false;

    lv_coord_t auto_w;
    lv_coord_t auto_h;
    if(x_auto && y_auto) {
        _lv_obj_calc_auto_size(obj, &auto_w, &auto_h);
        w = auto_w;
        h = auto_h;
    }
    else if(x_auto) {
        _lv_obj_calc_auto_size(obj, &auto_w, NULL);
        w = auto_w;
    }
    else if(y_auto) {
        _lv_obj_calc_auto_size(obj, NULL, &auto_h);
        h = auto_h;
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

    lv_obj_set_size(obj, w, lv_obj_get_height(obj));
}

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height in pixels or `LV_SIZE_AUTO` to set the size to involve all children
 */
void lv_obj_set_height(lv_obj_t * obj, lv_coord_t h)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_obj_set_size(obj, lv_obj_get_width(obj), h);
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

    lv_point_t new_pos;
    _lv_area_align(&base->coords, &obj->coords, align, &new_pos);

    /*Bring together the coordination system of base and obj*/
    lv_obj_t * par        = lv_obj_get_parent(obj);
    lv_coord_t par_abs_x  = par->coords.x1;
    lv_coord_t par_abs_y  = par->coords.y1;
    new_pos.x += x_ofs;
    new_pos.y += y_ofs;
    new_pos.x -= par_abs_x;
    new_pos.y -= par_abs_y;

    lv_obj_set_pos(obj, new_pos.x, new_pos.y);
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
lv_coord_t lv_obj_get_height_margin(lv_obj_t * obj)
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
lv_coord_t lv_obj_get_width_margin(lv_obj_t * obj)
{
    lv_style_int_t mleft = lv_obj_get_style_margin_left(obj, LV_OBJ_PART_MAIN);
    lv_style_int_t mright = lv_obj_get_style_margin_right(obj, LV_OBJ_PART_MAIN);

    return lv_obj_get_width(obj) + mleft + mright;
}

/**
 * Check if a given screen-space point is on an object's coordinates.
 *
 * This method is intended to be used mainly by advanced hit testing algorithms to check
 * whether the point is even within the object (as an optimization).
 * @param obj object to check
 * @param point screen-space point
 */
bool lv_obj_is_point_on_coords(lv_obj_t * obj, const lv_point_t * point)
{
#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    lv_area_t ext_area;
    ext_area.x1 = obj->coords.x1 - obj->ext_click_pad_hor;
    ext_area.x2 = obj->coords.x2 + obj->ext_click_pad_hor;
    ext_area.y1 = obj->coords.y1 - obj->ext_click_pad_ver;
    ext_area.y2 = obj->coords.y2 + obj->ext_click_pad_ver;

    if(!_lv_area_is_point_on(&ext_area, point, 0)) {
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    lv_area_t ext_area;
    ext_area.x1 = obj->coords.x1 - obj->ext_click_pad.x1;
    ext_area.x2 = obj->coords.x2 + obj->ext_click_pad.x2;
    ext_area.y1 = obj->coords.y1 - obj->ext_click_pad.y1;
    ext_area.y2 = obj->coords.y2 + obj->ext_click_pad.y2;

    if(!_lv_area_is_point_on(&ext_area, point, 0)) {
#else
    if(!_lv_area_is_point_on(&obj->coords, point, 0)) {
#endif
        return false;
    }
    return true;
}

/**
 * Calculate the "auto size". It's `auto_size = max(gird_size, children_size)`
 * @param obj pointer to an object
 * @param w_out store the width here. NULL to not calculate width
 * @param h_out store the height here. NULL to not calculate height
 */
void _lv_obj_calc_auto_size(lv_obj_t * obj, lv_coord_t * w_out, lv_coord_t * h_out)
{
    if(!w_out && !h_out) return;

    /*If no other effect the auto-size of zero by default*/
    if(w_out) *w_out = 0;
    if(h_out) *h_out = 0;

    /*Get the grid size of the object has a defined grid*/
    lv_coord_t grid_w = 0;
    lv_coord_t grid_h = 0;
    if(obj->grid) {
        _lv_grid_calc_t calc;
        grid_calc(obj, &calc);
        grid_w = calc.col_dsc[calc.col_dsc_len - 1] + lv_obj_get_style_pad_top(obj, LV_OBJ_PART_MAIN) +  + lv_obj_get_style_pad_bottom(obj, LV_OBJ_PART_MAIN);
        grid_h = calc.row_dsc[calc.row_dsc_len - 1] + lv_obj_get_style_pad_left(obj, LV_OBJ_PART_MAIN) + lv_obj_get_style_pad_right(obj, LV_OBJ_PART_MAIN);;
        grid_calc_free(&calc);
    }

    /*Get the children's most right and bottom position*/
    lv_coord_t children_w = 0;
    lv_coord_t children_h = 0;
    if(w_out) {
        lv_obj_scroll_to_x(obj, 0, LV_ANIM_OFF);
        lv_coord_t scroll_right = lv_obj_get_scroll_right(obj);
        children_w = lv_obj_get_width(obj) + scroll_right;
    }

    if(h_out) {
        lv_obj_scroll_to_y(obj, 0, LV_ANIM_OFF);
        lv_coord_t scroll_bottom = lv_obj_get_scroll_bottom(obj);
        children_h = lv_obj_get_height(obj) + scroll_bottom;
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
        if(_GRID_IS_CELL(obj->x_set) || _GRID_IS_CELL(obj->x_set)) {
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
    if(_GRID_IS_CELL(obj->x_set) && _GRID_IS_CELL(obj->y_set)) return true;
    return false;
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

    /*Set the length and height*/
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

