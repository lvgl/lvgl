/**
 * @file lv_cont.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_cont.h"
#if LV_USE_CONT != 0

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../lv_core/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_draw/lv_draw_basic.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_bidi.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_cont"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_cont_signal(lv_obj_t * cont, lv_signal_t sign, void * param);
static void lv_cont_refr_layout(lv_obj_t * cont);
static void lv_cont_layout_col(lv_obj_t * cont);
static void lv_cont_layout_row(lv_obj_t * cont);
static void lv_cont_layout_center(lv_obj_t * cont);
static void lv_cont_layout_pretty(lv_obj_t * cont);
static void lv_cont_layout_grid(lv_obj_t * cont);
static void lv_cont_refr_autofit(lv_obj_t * cont);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a container objects
 * @param par pointer to an object, it will be the parent of the new container
 * @param copy pointer to a container object, if not NULL then the new object will be copied from it
 * @return pointer to the created container
 */
lv_obj_t * lv_cont_create(lv_obj_t * par, const lv_obj_t * copy)
{

    LV_LOG_TRACE("container create started");

    /*Create a basic object*/
    lv_obj_t * new_cont = lv_obj_create(par, copy);
    LV_ASSERT_MEM(new_cont);
    if(new_cont == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_cont);

    lv_obj_allocate_ext_attr(new_cont, sizeof(lv_cont_ext_t));
    lv_cont_ext_t * ext = lv_obj_get_ext_attr(new_cont);
    if(ext == NULL) return NULL;

    LV_ASSERT_MEM(ext);
    ext->fit_left   = LV_FIT_NONE;
    ext->fit_right  = LV_FIT_NONE;
    ext->fit_top    = LV_FIT_NONE;
    ext->fit_bottom = LV_FIT_NONE;
    ext->layout     = LV_LAYOUT_OFF;

    lv_obj_set_signal_cb(new_cont, lv_cont_signal);

    /*Init the new container*/
    if(copy == NULL) {
        /*Set the default styles if it's not screen*/
        if(par != NULL) {
            lv_theme_t * th = lv_theme_get_current();
            if(th) {
                lv_cont_set_style(new_cont, LV_CONT_STYLE_MAIN, th->style.cont);
            } else {
                lv_cont_set_style(new_cont, LV_CONT_STYLE_MAIN, &lv_style_pretty);
            }
        }
    }
    /*Copy an existing object*/
    else {
        lv_cont_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->fit_left            = copy_ext->fit_left;
        ext->fit_right           = copy_ext->fit_right;
        ext->fit_top             = copy_ext->fit_top;
        ext->fit_bottom          = copy_ext->fit_bottom;
        ext->layout              = copy_ext->layout;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_cont);
    }

    LV_LOG_INFO("container created");

    return new_cont;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a layout on a container
 * @param cont pointer to a container object
 * @param layout a layout from 'lv_cont_layout_t'
 */
void lv_cont_set_layout(lv_obj_t * cont, lv_layout_t layout)
{
    LV_ASSERT_OBJ(cont, LV_OBJX_NAME);

    lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);
    if(ext->layout == layout) return;

    ext->layout = layout;

    /*Send a signal to refresh the layout*/
    cont->signal_cb(cont, LV_SIGNAL_CHILD_CHG, NULL);
}

/**
 * Set the fit policy in all 4 directions separately.
 * It tell how to change the container's size automatically.
 * @param cont pointer to a container object
 * @param left left fit policy from `lv_fit_t`
 * @param right right fit policy from `lv_fit_t`
 * @param top bottom fit policy from `lv_fit_t`
 * @param bottom bottom fit policy from `lv_fit_t`
 */
void lv_cont_set_fit4(lv_obj_t * cont, lv_fit_t left, lv_fit_t right, lv_fit_t top, lv_fit_t bottom)
{
    LV_ASSERT_OBJ(cont, LV_OBJX_NAME);

    lv_obj_invalidate(cont);
    lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);
    if(ext->fit_left == left && ext->fit_right == right && ext->fit_top == top && ext->fit_bottom == bottom) {
        return;
    }

    ext->fit_left   = left;
    ext->fit_right  = right;
    ext->fit_top    = top;
    ext->fit_bottom = bottom;

    /*Send a signal to refresh the layout*/
    cont->signal_cb(cont, LV_SIGNAL_CHILD_CHG, NULL);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the layout of a container
 * @param cont pointer to container object
 * @return the layout from 'lv_cont_layout_t'
 */
lv_layout_t lv_cont_get_layout(const lv_obj_t * cont)
{
    LV_ASSERT_OBJ(cont, LV_OBJX_NAME);

    lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);
    return ext->layout;
}

/**
 * Get left fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `lv_fit_t`
 */
lv_fit_t lv_cont_get_fit_left(const lv_obj_t * cont)
{
    LV_ASSERT_OBJ(cont, LV_OBJX_NAME);

    lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);
    return ext->fit_left;
}

/**
 * Get right fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `lv_fit_t`
 */
lv_fit_t lv_cont_get_fit_right(const lv_obj_t * cont)
{
    LV_ASSERT_OBJ(cont, LV_OBJX_NAME);

    lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);
    return ext->fit_right;
}

/**
 * Get top fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `lv_fit_t`
 */
lv_fit_t lv_cont_get_fit_top(const lv_obj_t * cont)
{
    LV_ASSERT_OBJ(cont, LV_OBJX_NAME);

    lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);
    return ext->fit_top;
}

/**
 * Get bottom fit mode of a container
 * @param cont pointer to a container object
 * @return an element of `lv_fit_t`
 */
lv_fit_t lv_cont_get_fit_bottom(const lv_obj_t * cont)
{
    LV_ASSERT_OBJ(cont, LV_OBJX_NAME);

    lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);
    return ext->fit_bottom;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the container
 * @param cont pointer to a container object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_cont_signal(lv_obj_t * cont, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(cont, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_STYLE_CHG) { /*Recalculate the padding if the style changed*/
        lv_cont_refr_layout(cont);
        lv_cont_refr_autofit(cont);
    } else if(sign == LV_SIGNAL_CHILD_CHG) {
        lv_cont_refr_layout(cont);
        lv_cont_refr_autofit(cont);
    } else if(sign == LV_SIGNAL_CORD_CHG) {
        if(lv_obj_get_width(cont) != lv_area_get_width(param) || lv_obj_get_height(cont) != lv_area_get_height(param)) {
            lv_cont_refr_layout(cont);
            lv_cont_refr_autofit(cont);
        }
    } else if(sign == LV_SIGNAL_PARENT_SIZE_CHG) {
        /*FLOOD and FILL fit needs to be refreshed if the parent size has changed*/
        lv_cont_refr_autofit(cont);

    }

    return res;
}

/**
 * Refresh the layout of a container
 * @param cont pointer to an object which layout should be refreshed
 */
static void lv_cont_refr_layout(lv_obj_t * cont)
{
    lv_layout_t type = lv_cont_get_layout(cont);

    /*'cont' has to be at least 1 child*/
    if(lv_obj_get_child(cont, NULL) == NULL) return;

    if(type == LV_LAYOUT_OFF) return;

    if(type == LV_LAYOUT_CENTER) {
        lv_cont_layout_center(cont);
    } else if(type == LV_LAYOUT_COL_L || type == LV_LAYOUT_COL_M || type == LV_LAYOUT_COL_R) {
        lv_cont_layout_col(cont);
    } else if(type == LV_LAYOUT_ROW_T || type == LV_LAYOUT_ROW_M || type == LV_LAYOUT_ROW_B) {
        lv_cont_layout_row(cont);
    } else if(type == LV_LAYOUT_PRETTY) {
        lv_cont_layout_pretty(cont);
    } else if(type == LV_LAYOUT_GRID) {
        lv_cont_layout_grid(cont);
    }
}

/**
 * Handle column type layouts
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_col(lv_obj_t * cont)
{
    lv_layout_t type = lv_cont_get_layout(cont);
    lv_obj_t * child;

    /*Adjust margin and get the alignment type*/
    lv_align_t align;
    const lv_style_t * style = lv_obj_get_style(cont);
    lv_coord_t hpad_corr;

    switch(type) {
        case LV_LAYOUT_COL_L:
            hpad_corr = style->body.padding.left;
            align     = LV_ALIGN_IN_TOP_LEFT;
            break;
        case LV_LAYOUT_COL_M:
            hpad_corr = 0;
            align     = LV_ALIGN_IN_TOP_MID;
            break;
        case LV_LAYOUT_COL_R:
            hpad_corr = -style->body.padding.right;
            align     = LV_ALIGN_IN_TOP_RIGHT;
            break;
        default:
            hpad_corr = 0;
            align     = LV_ALIGN_IN_TOP_LEFT;
            break;
    }

    /* Disable child change action because the children will be moved a lot
     * an unnecessary child change signals could be sent*/
    lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);
    /* Align the children */
    lv_coord_t last_cord = style->body.padding.top;
    LV_LL_READ_BACK(cont->child_ll, child)
    {
        if(lv_obj_get_hidden(child) != false || lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

        lv_obj_align(child, cont, align, hpad_corr, last_cord);
        last_cord += lv_obj_get_height(child) + style->body.padding.inner;
    }

    lv_obj_clear_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle row type layouts
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_row(lv_obj_t * cont)
{
    lv_layout_t type = lv_cont_get_layout(cont);
    lv_obj_t * child;

    /*Adjust margin and get the alignment type*/
    lv_align_t align;
    const lv_style_t * style = lv_obj_get_style(cont);
    lv_coord_t vpad_corr;
    lv_bidi_dir_t base_dir = lv_obj_get_base_dir(cont);
    switch(type) {
        case LV_LAYOUT_ROW_T:
            vpad_corr = style->body.padding.top;
            align     = base_dir == LV_BIDI_DIR_RTL ? LV_ALIGN_IN_TOP_RIGHT : LV_ALIGN_IN_TOP_LEFT;
            break;
        case LV_LAYOUT_ROW_M:
            vpad_corr = 0;
            align     = base_dir == LV_BIDI_DIR_RTL ? LV_ALIGN_IN_RIGHT_MID: LV_ALIGN_IN_LEFT_MID;
            break;
        case LV_LAYOUT_ROW_B:
            vpad_corr = -style->body.padding.bottom;
            align     = base_dir == LV_BIDI_DIR_RTL ? LV_ALIGN_IN_BOTTOM_RIGHT: LV_ALIGN_IN_BOTTOM_LEFT;
            break;
        default:
            vpad_corr = 0;
            align     = base_dir == LV_BIDI_DIR_RTL ? LV_ALIGN_IN_TOP_RIGHT : LV_ALIGN_IN_TOP_LEFT;
            break;
    }

    /* Disable child change action because the children will be moved a lot
     * an unnecessary child change signals could be sent*/
    lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);

    /* Align the children */
    lv_coord_t last_cord;
    if(base_dir == LV_BIDI_DIR_RTL) last_cord = style->body.padding.right;
    else last_cord = style->body.padding.left;

    LV_LL_READ_BACK(cont->child_ll, child)
    {
        if(lv_obj_get_hidden(child) != false || lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

//        last_cord -= lv_obj_get_width(child);

        if(base_dir == LV_BIDI_DIR_RTL) lv_obj_align(child, cont, align, -last_cord, vpad_corr);
        else lv_obj_align(child, cont, align, last_cord, vpad_corr);

        last_cord += lv_obj_get_width(child) + style->body.padding.inner;
    }

    lv_obj_clear_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle the center layout
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_center(lv_obj_t * cont)
{
    lv_obj_t * child;
    const lv_style_t * style = lv_obj_get_style(cont);
    uint32_t obj_num         = 0;
    lv_coord_t h_tot         = 0;

    LV_LL_READ(cont->child_ll, child)
    {
        if(lv_obj_get_hidden(child) != false || lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;
        h_tot += lv_obj_get_height(child) + style->body.padding.inner;
        obj_num++;
    }

    if(obj_num == 0) return;

    h_tot -= style->body.padding.inner;

    /* Disable child change action because the children will be moved a lot
     * an unnecessary child change signals could be sent*/
    lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);

    /* Align the children */
    lv_coord_t last_cord = -(h_tot / 2);
    LV_LL_READ_BACK(cont->child_ll, child)
    {
        if(lv_obj_get_hidden(child) != false || lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

        lv_obj_align(child, cont, LV_ALIGN_CENTER, 0, last_cord + lv_obj_get_height(child) / 2);
        last_cord += lv_obj_get_height(child) + style->body.padding.inner;
    }

    lv_obj_clear_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle the pretty layout. Put as many object as possible in row
 * then begin a new row
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_pretty(lv_obj_t * cont)
{
    lv_obj_t * child_rs;  /* Row starter child */
    lv_obj_t * child_rc;  /* Row closer child */
    lv_obj_t * child_tmp; /* Temporary child */
    const lv_style_t * style = lv_obj_get_style(cont);
    lv_coord_t w_obj         = lv_obj_get_width(cont);
    lv_coord_t act_y         = style->body.padding.top;
    /* Disable child change action because the children will be moved a lot
     * an unnecessary child change signals could be sent*/

    child_rs = lv_ll_get_tail(&cont->child_ll); /*Set the row starter child*/
    if(child_rs == NULL) return;                /*Return if no child*/

    lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);

    child_rc = child_rs; /*Initially the the row starter and closer is the same*/
    while(child_rs != NULL) {
        lv_coord_t h_row = 0;
        lv_coord_t w_row =
            style->body.padding.left + style->body.padding.right; /*The width is at least the left+right hpad*/
        uint32_t obj_num = 0;

        /*Find the row closer object and collect some data*/
        do {
            if(lv_obj_get_hidden(child_rc) == false && lv_obj_is_protected(child_rc, LV_PROTECT_POS) == false) {
                /*If this object is already not fit then break*/
                if(w_row + lv_obj_get_width(child_rc) > w_obj) {
                    /*Step back one child because the last already not fit, so the previous is the
                     * closer*/
                    if(child_rc != NULL && obj_num != 0) {
                        child_rc = lv_ll_get_next(&cont->child_ll, child_rc);
                    }
                    break;
                }
                w_row += lv_obj_get_width(child_rc) + style->body.padding.inner; /*Add the object width + opad*/
                h_row = LV_MATH_MAX(h_row, lv_obj_get_height(child_rc));         /*Search the highest object*/
                obj_num++;
                if(lv_obj_is_protected(child_rc, LV_PROTECT_FOLLOW))
                    break; /*If can not be followed by an other object then break here*/
            }
            child_rc = lv_ll_get_prev(&cont->child_ll, child_rc); /*Load the next object*/
            if(obj_num == 0)
                child_rs = child_rc; /*If the first object was hidden (or too long) then set the
                                        next as first */
        } while(child_rc != NULL);

        /*If the object is too long  then align it to the middle*/
        if(obj_num == 0) {
            if(child_rc != NULL) {
                lv_obj_align(child_rc, cont, LV_ALIGN_IN_TOP_MID, 0, act_y);
                h_row = lv_obj_get_height(child_rc); /*Not set previously because of the early break*/
            }
        }
        /*If there is only one object in the row then align it to the middle*/
        else if(obj_num == 1) {
            lv_obj_align(child_rs, cont, LV_ALIGN_IN_TOP_MID, 0, act_y);
        }
        /*If there are two object in the row then align them proportionally*/
        else if(obj_num == 2) {
            lv_obj_t * obj1 = child_rs;
            lv_obj_t * obj2 = lv_ll_get_prev(&cont->child_ll, child_rs);
            w_row           = lv_obj_get_width(obj1) + lv_obj_get_width(obj2);
            lv_coord_t pad  = (w_obj - w_row) / 3;
            lv_obj_align(obj1, cont, LV_ALIGN_IN_TOP_LEFT, pad, act_y + (h_row - lv_obj_get_height(obj1)) / 2);
            lv_obj_align(obj2, cont, LV_ALIGN_IN_TOP_RIGHT, -pad, act_y + (h_row - lv_obj_get_height(obj2)) / 2);
        }
        /* Align the children (from child_rs to child_rc)*/
        else {
            w_row -= style->body.padding.inner * obj_num;
            lv_coord_t new_opad = (w_obj - w_row) / (obj_num - 1);
            lv_coord_t act_x    = style->body.padding.left; /*x init*/
            child_tmp           = child_rs;
            while(child_tmp != NULL) {
                if(lv_obj_get_hidden(child_tmp) == false && lv_obj_is_protected(child_tmp, LV_PROTECT_POS) == false) {
                    lv_obj_align(child_tmp, cont, LV_ALIGN_IN_TOP_LEFT, act_x,
                                 act_y + (h_row - lv_obj_get_height(child_tmp)) / 2);
                    act_x += lv_obj_get_width(child_tmp) + new_opad;
                }
                if(child_tmp == child_rc) break;
                child_tmp = lv_ll_get_prev(&cont->child_ll, child_tmp);
            }
        }

        if(child_rc == NULL) break;
        act_y += style->body.padding.inner + h_row;           /*y increment*/
        child_rs = lv_ll_get_prev(&cont->child_ll, child_rc); /*Go to the next object*/
        child_rc = child_rs;
    }
    lv_obj_clear_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle the grid layout. Align same-sized objects in a grid
 * @param cont pointer to an object which layout should be handled
 */
static void lv_cont_layout_grid(lv_obj_t * cont)
{
    lv_obj_t * child;
    const lv_style_t * style = lv_obj_get_style(cont);
    lv_coord_t w_tot         = lv_obj_get_width(cont);
    lv_coord_t w_obj         = lv_obj_get_width(lv_obj_get_child(cont, NULL));
    lv_coord_t w_fit         =  lv_obj_get_width_fit(cont);
    lv_coord_t h_obj         = lv_obj_get_height(lv_obj_get_child(cont, NULL));
    uint16_t obj_row         = (w_fit) / (w_obj + style->body.padding.inner); /*Obj. num. in a row*/
    lv_coord_t x_ofs;
    if(obj_row > 1) {
        x_ofs = w_obj + (w_fit - (obj_row * w_obj)) / (obj_row - 1);
    } else {
        x_ofs = w_tot / 2 - w_obj / 2;
    }
    lv_coord_t y_ofs = h_obj + style->body.padding.inner;

    /* Disable child change action because the children will be moved a lot
     * an unnecessary child change signals could be sent*/
    lv_obj_set_protect(cont, LV_PROTECT_CHILD_CHG);

    /* Align the children */
    lv_coord_t act_x = style->body.padding.left;
    lv_coord_t act_y = style->body.padding.top;
    uint16_t obj_cnt = 0;
    LV_LL_READ_BACK(cont->child_ll, child)
    {
        if(lv_obj_get_hidden(child) != false || lv_obj_is_protected(child, LV_PROTECT_POS) != false) continue;

        if(obj_row > 1) {
            lv_obj_set_pos(child, act_x, act_y);
            act_x += x_ofs;
        } else {
            lv_obj_set_pos(child, x_ofs, act_y);
        }
        obj_cnt++;

        if(obj_cnt >= obj_row) {
            obj_cnt = 0;
            act_x   = style->body.padding.left;
            act_y += y_ofs;
        }
    }

    lv_obj_clear_protect(cont, LV_PROTECT_CHILD_CHG);
}

/**
 * Handle auto fit. Set the size of the object to involve all children.
 * @param cont pointer to an object which size will be modified
 */
static void lv_cont_refr_autofit(lv_obj_t * cont)
{
    lv_cont_ext_t * ext = lv_obj_get_ext_attr(cont);

    if(ext->fit_left == LV_FIT_NONE && ext->fit_right == LV_FIT_NONE && ext->fit_top == LV_FIT_NONE &&
       ext->fit_bottom == LV_FIT_NONE) {
        return;
    }

    lv_area_t tight_area;
    lv_area_t ori;
    const lv_style_t * style = lv_obj_get_style(cont);
    lv_obj_t * child_i;

    lv_obj_t * par               = lv_obj_get_parent(cont);
    const lv_style_t * par_style = lv_obj_get_style(par);
    lv_area_t flood_area;
    lv_area_copy(&flood_area, &par->coords);
    flood_area.x1 += par_style->body.padding.left;
    flood_area.x2 -= par_style->body.padding.right;
    flood_area.y1 += par_style->body.padding.top;
    flood_area.y2 -= par_style->body.padding.bottom;

    /*Search the side coordinates of the children*/
    lv_obj_get_coords(cont, &ori);
    lv_obj_get_coords(cont, &tight_area);

    bool has_children = lv_ll_is_empty(&cont->child_ll) ? false : true;

    if(has_children) {
        tight_area.x1 = LV_COORD_MAX;
        tight_area.y1 = LV_COORD_MAX;
        tight_area.x2 = LV_COORD_MIN;
        tight_area.y2 = LV_COORD_MIN;

        LV_LL_READ(cont->child_ll, child_i)
        {
            if(lv_obj_get_hidden(child_i) != false) continue;
            tight_area.x1 = LV_MATH_MIN(tight_area.x1, child_i->coords.x1);
            tight_area.y1 = LV_MATH_MIN(tight_area.y1, child_i->coords.y1);
            tight_area.x2 = LV_MATH_MAX(tight_area.x2, child_i->coords.x2);
            tight_area.y2 = LV_MATH_MAX(tight_area.y2, child_i->coords.y2);
        }

        tight_area.x1 -= style->body.padding.left;
        tight_area.x2 += style->body.padding.right;
        tight_area.y1 -= style->body.padding.top;
        tight_area.y2 += style->body.padding.bottom;
    }

    lv_area_t new_area;
    lv_area_copy(&new_area, &ori);

    switch(ext->fit_left) {
        case LV_FIT_TIGHT: new_area.x1 = tight_area.x1; break;
        case LV_FIT_FLOOD: new_area.x1 = flood_area.x1; break;
        case LV_FIT_FILL: new_area.x1 = has_children ? LV_MATH_MIN(tight_area.x1, flood_area.x1) : flood_area.x1; break;
        default: break;
    }

    switch(ext->fit_right) {
        case LV_FIT_TIGHT: new_area.x2 = tight_area.x2; break;
        case LV_FIT_FLOOD: new_area.x2 = flood_area.x2; break;
        case LV_FIT_FILL: new_area.x2 = has_children ? LV_MATH_MAX(tight_area.x2, flood_area.x2) : flood_area.x2; break;
        default: break;
    }

    switch(ext->fit_top) {
        case LV_FIT_TIGHT: new_area.y1 = tight_area.y1; break;
        case LV_FIT_FLOOD: new_area.y1 = flood_area.y1; break;
        case LV_FIT_FILL: new_area.y1 = has_children ? LV_MATH_MIN(tight_area.y1, flood_area.y1) : flood_area.y1; break;
        default: break;
    }

    switch(ext->fit_bottom) {
        case LV_FIT_TIGHT: new_area.y2 = tight_area.y2; break;
        case LV_FIT_FLOOD: new_area.y2 = flood_area.y2; break;
        case LV_FIT_FILL: new_area.y2 = has_children ? LV_MATH_MAX(tight_area.y2, flood_area.y2) : flood_area.y2; break;
        default: break;
    }

    /*Do nothing if the coordinates are not changed*/
    if(cont->coords.x1 != new_area.x1 || cont->coords.y1 != new_area.y1 || cont->coords.x2 != new_area.x2 ||
       cont->coords.y2 != new_area.y2) {

        lv_obj_invalidate(cont);
        lv_area_copy(&cont->coords, &new_area);
        lv_obj_invalidate(cont);

        /*Notify the object about its new coordinates*/
        cont->signal_cb(cont, LV_SIGNAL_CORD_CHG, &ori);

        /*Inform the parent about the new coordinates*/
        par->signal_cb(par, LV_SIGNAL_CHILD_CHG, cont);

        if(lv_obj_get_auto_realign(cont)) {
            lv_obj_realign(cont);
        }

        /*Tell the children the parent's size has changed*/
        LV_LL_READ(cont->child_ll, child_i)
        {
            child_i->signal_cb(child_i, LV_SIGNAL_PARENT_SIZE_CHG, NULL);
        }
    }
}

#endif
