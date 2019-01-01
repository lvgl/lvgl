/**
 * @file lv_page.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_objx/lv_page.h"
#if USE_LV_PAGE != 0

#include "../lv_core/lv_group.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_core/lv_refr.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LV_PAGE_SB_MIN_SIZE             (LV_DPI / 8)
#define LV_PAGE_SCROLL_ANIM_TIME        200           /*[ms] Scroll anim time on `lv_page_scroll_up/down/left/rigth`*/
#define LV_PAGE_END_FLASH_SIZE          (LV_DPI / 4)
#define LV_PAGE_END_ANIM_TIME           300
#define LV_PAGE_END_ANIM_WAIT_TIME      300

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_page_sb_refresh(lv_obj_t * page);
static bool lv_page_design(lv_obj_t * page, const lv_area_t * mask, lv_design_mode_t mode);
static bool lv_scrl_design(lv_obj_t * scrl, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_page_signal(lv_obj_t * page, lv_signal_t sign, void * param);
static lv_res_t lv_page_scrollable_signal(lv_obj_t * scrl, lv_signal_t sign, void * param);
static void edge_flash_anim(void * page, int32_t v);
static void edge_flash_anim_end(void * page);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_func_t ancestor_design;
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a page objects
 * @param par pointer to an object, it will be the parent of the new page
 * @param copy pointer to a page object, if not NULL then the new object will be copied from it
 * @return pointer to the created page
 */
lv_obj_t * lv_page_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("page create started");

    /*Create the ancestor object*/
    lv_obj_t * new_page = lv_cont_create(par, copy);
    lv_mem_assert(new_page);
    if(new_page == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_page);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_page);

    /*Allocate the object type specific extended data*/
    lv_page_ext_t * ext = lv_obj_allocate_ext_attr(new_page, sizeof(lv_page_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->scrl = NULL;
    ext->pr_action = NULL;
    ext->rel_action = NULL;
    ext->sb.hor_draw = 0;
    ext->sb.ver_draw = 0;
    ext->sb.style = &lv_style_pretty;
    ext->sb.mode = LV_SB_MODE_AUTO;
    ext->edge_flash.enabled = 0;
    ext->edge_flash.bottom_ip = 0;
    ext->edge_flash.top_ip = 0;
    ext->edge_flash.left_ip = 0;
    ext->edge_flash.right_ip = 0;
    ext->edge_flash.state = 0;
    ext->edge_flash.style = &lv_style_plain_color;
    ext->arrow_scroll = 0;
    ext->scroll_prop = 0;
    ext->scroll_prop_ip = 0;

    /*Init the new page object*/
    if(copy == NULL) {
        ext->scrl = lv_cont_create(new_page, NULL);
        lv_obj_set_signal_func(ext->scrl, lv_page_scrollable_signal);
        lv_obj_set_design_func(ext->scrl, lv_scrl_design);
        lv_obj_set_drag(ext->scrl, true);
        lv_obj_set_drag_throw(ext->scrl, true);
        lv_obj_set_protect(ext->scrl, LV_PROTECT_PARENT | LV_PROTECT_PRESS_LOST);
        lv_cont_set_fit(ext->scrl, false, true);

        /* Add the signal function only if 'scrolling' is created
         * because everything has to be ready before any signal is received*/
        lv_obj_set_signal_func(new_page, lv_page_signal);
        lv_obj_set_design_func(new_page, lv_page_design);

        lv_page_set_sb_mode(new_page, ext->sb.mode);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            if(par == NULL) { /*Different styles if it is screen*/
                lv_page_set_style(new_page, LV_PAGE_STYLE_BG, th->bg);
                lv_page_set_style(new_page, LV_PAGE_STYLE_SCRL, &lv_style_transp);
            } else {
                lv_page_set_style(new_page, LV_PAGE_STYLE_BG, th->page.bg);
                lv_page_set_style(new_page, LV_PAGE_STYLE_SCRL, th->page.scrl);

            }
            lv_page_set_style(new_page, LV_PAGE_STYLE_SB, th->page.sb);
        } else {
            lv_page_set_style(new_page, LV_PAGE_STYLE_BG, &lv_style_pretty_color);
            lv_page_set_style(new_page, LV_PAGE_STYLE_SCRL, &lv_style_pretty);
            lv_page_set_style(new_page, LV_PAGE_STYLE_SB, &lv_style_pretty_color);
        }

    } else {
        lv_page_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->scrl = lv_cont_create(new_page, copy_ext->scrl);
        lv_obj_set_signal_func(ext->scrl, lv_page_scrollable_signal);

        lv_page_set_pr_action(new_page, copy_ext->pr_action);
        lv_page_set_rel_action(new_page, copy_ext->rel_action);
        lv_page_set_sb_mode(new_page, copy_ext->sb.mode);
        lv_page_set_arrow_scroll(new_page, copy_ext->arrow_scroll);


        lv_page_set_style(new_page, LV_PAGE_STYLE_BG, lv_page_get_style(copy, LV_PAGE_STYLE_BG));
        lv_page_set_style(new_page, LV_PAGE_STYLE_SCRL, lv_page_get_style(copy, LV_PAGE_STYLE_SCRL));
        lv_page_set_style(new_page, LV_PAGE_STYLE_SB, lv_page_get_style(copy, LV_PAGE_STYLE_SB));

        /* Add the signal function only if 'scrolling' is created
         * because everything has to be ready before any signal is received*/
        lv_obj_set_signal_func(new_page, lv_page_signal);
        lv_obj_set_design_func(new_page, lv_page_design);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_page);
    }

    lv_page_sb_refresh(new_page);

    LV_LOG_INFO("page created");

    return new_page;
}

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param obj pointer to an object
 */
void lv_page_clean(lv_obj_t * obj)
{
    lv_obj_t * scrl = lv_page_get_scrl(obj);
    lv_obj_clean(scrl);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a release action for the page
 * @param page pointer to a page object
 * @param rel_action a function to call when the page is release
 */
void lv_page_set_rel_action(lv_obj_t * page, lv_action_t rel_action)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    ext->rel_action = rel_action;
}

/**
 * Set a press action for the page
 * @param page pointer to a page object
 * @param pr_action a function to call when the page is pressed
 */
void lv_page_set_pr_action(lv_obj_t * page, lv_action_t pr_action)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    ext->pr_action = pr_action;
}

/**
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @param sb_mode the new mode from 'lv_page_sb.mode_t' enum
 */
void lv_page_set_sb_mode(lv_obj_t * page, lv_sb_mode_t sb_mode)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    if(ext->sb.mode == sb_mode) return;

    if(sb_mode == LV_SB_MODE_HIDE) ext->sb.mode |= LV_SB_MODE_HIDE;                 /*Set the hidden flag*/
    else if(sb_mode == LV_SB_MODE_UNHIDE) ext->sb.mode &= (~LV_SB_MODE_HIDE);       /*Clear the hidden flag*/
    else {
        if(ext->sb.mode & LV_SB_MODE_HIDE) sb_mode |= LV_SB_MODE_HIDE;
        ext->sb.mode = sb_mode;
    }

    ext->sb.hor_draw = 0;
    ext->sb.ver_draw = 0;

    lv_page_sb_refresh(page);
    lv_obj_invalidate(page);
}

/**
 * Enable/Disable scrolling with arrows if the page is in group (arrows: LV_GROUP_KEY_LEFT/RIGHT/UP/DOWN)
 * @param page pointer to a page object
 * @param en true: enable scrolling with arrows
 */
void lv_page_set_arrow_scroll(lv_obj_t * page, bool en)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    ext->arrow_scroll = en ? 1 : 0;
}

/**
 * Enable the scroll propagation feature. If enabled then the page will move its parent if there is no more space to scroll.
 * @param page pointer to a Page
 * @param en true or false to enable/disable scroll propagation
 */
void lv_page_set_scroll_propagation(lv_obj_t * page, bool en)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    ext->scroll_prop = en ? 1 : 0;
}

/**
 * Enable the edge flash effect. (Show an arc when the an edge is reached)
 * @param page pointer to a Page
 * @param en true or false to enable/disable end flash
 */
void lv_page_set_edge_flash(lv_obj_t * page, bool en)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    ext->edge_flash.enabled = en ? 1 : 0;
}

/**
 * Set a style of a page
 * @param page pointer to a page object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_page_set_style(lv_obj_t * page, lv_page_style_t type, lv_style_t * style)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);

    switch(type) {
        case LV_PAGE_STYLE_BG:
            lv_obj_set_style(page, style);
            break;
        case LV_PAGE_STYLE_SCRL:
            lv_obj_set_style(ext->scrl, style);
            break;
        case LV_PAGE_STYLE_SB:
            ext->sb.style = style;
            lv_area_set_height(&ext->sb.hor_area, ext->sb.style->body.padding.inner);
            lv_area_set_width(&ext->sb.ver_area, ext->sb.style->body.padding.inner);
            lv_page_sb_refresh(page);
            lv_obj_refresh_ext_size(page);
            lv_obj_invalidate(page);
            break;
        case LV_PAGE_STYLE_EDGE_FLASH:
            ext->edge_flash.style = style;
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the scrollable object of a page
 * @param page pointer to a page object
 * @return pointer to a container which is the scrollable part of the page
 */
lv_obj_t * lv_page_get_scrl(const lv_obj_t * page)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);

    return ext->scrl;
}

/**
 * Get the press action of the page
 * @param page pointer to a page object
 * @return a function to call when the page is pressed
 */
lv_action_t lv_page_get_pr_action(lv_obj_t * page)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    return ext->pr_action;
}

/**
 * Get the release action of the page
 * @param page pointer to a page object
 * @return a function to call when the page is released
 */
lv_action_t lv_page_get_rel_action(lv_obj_t * page)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    return ext->rel_action;
}

/**
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @return the mode from 'lv_page_sb.mode_t' enum
 */
lv_sb_mode_t lv_page_get_sb_mode(const lv_obj_t * page)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    return ext->sb.mode;
}

/**
 * Get the the scrolling with arrows (LV_GROUP_KEY_LEFT/RIGHT/UP/DOWN) is enabled or not
 * @param page pointer to a page object
 * @return true: scrolling with arrows is enabled
 */
bool lv_page_get_arrow_scroll(const lv_obj_t * page)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    return ext->arrow_scroll ? true : false;
}

/**
 * Get the scroll propagation property
 * @param page pointer to a Page
 * @return true or false
 */
bool lv_page_get_scroll_propagation(lv_obj_t * page)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    return ext->scroll_prop == 0 ? false : true;
}

/**
 * Get the edge flash effect property.
 * @param page pointer to a Page
 * return true or false
 */
bool lv_page_get_edge_flash(lv_obj_t * page)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    return ext->edge_flash.enabled == 0 ? false : true;
}

/**
 * Get that width which can be set to the children to still not cause overflow (show scrollbars)
 * @param page pointer to a page object
 * @return the width which still fits into the page
 */
lv_coord_t lv_page_get_fit_width(lv_obj_t * page)
{
    lv_style_t * bg_style = lv_page_get_style(page, LV_PAGE_STYLE_BG);
    lv_style_t * scrl_style = lv_page_get_style(page, LV_PAGE_STYLE_SCRL);

    return lv_obj_get_width(page) - 2 * (bg_style->body.padding.hor + scrl_style->body.padding.hor);
}

/**
 * Get that height which can be set to the children to still not cause overflow (show scrollbars)
 * @param page pointer to a page object
 * @return the height which still fits into the page
 */
lv_coord_t lv_page_get_fit_height(lv_obj_t * page)
{
    lv_style_t * bg_style = lv_page_get_style(page, LV_PAGE_STYLE_BG);
    lv_style_t * scrl_style = lv_page_get_style(page, LV_PAGE_STYLE_SCRL);

    return lv_obj_get_height(page) - 2 * (bg_style->body.padding.ver + scrl_style->body.padding.ver);
}

/**
 * Get a style of a page
 * @param page pointer to page object
 * @param type which style should be get
 * @return style pointer to a style
 *  */
lv_style_t * lv_page_get_style(const lv_obj_t * page, lv_page_style_t type)
{
    lv_style_t * style = NULL;
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);

    switch(type) {
        case LV_PAGE_STYLE_BG:
            style = lv_obj_get_style(page);
            break;
        case LV_PAGE_STYLE_SCRL:
            style = lv_obj_get_style(ext->scrl);
            break;
        case LV_PAGE_STYLE_SB:
            style = ext->sb.style;
            break;
        case LV_PAGE_STYLE_EDGE_FLASH:
            style = ext->edge_flash.style;
            break;
        default:
            style = NULL;
            break;
    }

    return style;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Glue the object to the page. After it the page can be moved (dragged) with this object too.
 * @param obj pointer to an object on a page
 * @param glue true: enable glue, false: disable glue
 */
void lv_page_glue_obj(lv_obj_t * obj, bool glue)
{
    lv_obj_set_drag_parent(obj, glue);
    lv_obj_set_drag(obj, glue);
}

/**
 * Focus on an object. It ensures that the object will be visible on the page.
 * @param page pointer to a page object
 * @param obj pointer to an object to focus (must be on the page)
 * @param anim_time scroll animation time in milliseconds (0: no animation)
 */
void lv_page_focus(lv_obj_t * page, const lv_obj_t * obj, uint16_t anim_time)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);

#if USE_LV_ANIMATION == 0
    anim_time = 0;
#else
    /* Be sure there is no position changing animation in progress
     * because it can overide the current changes*/
    lv_anim_del(page, (lv_anim_fp_t)lv_obj_set_y);
    lv_anim_del(page, (lv_anim_fp_t)lv_obj_set_pos);
    lv_anim_del(ext->scrl, (lv_anim_fp_t)lv_obj_set_y);
    lv_anim_del(ext->scrl, (lv_anim_fp_t)lv_obj_set_pos);
#endif

    lv_style_t * style = lv_page_get_style(page, LV_PAGE_STYLE_BG);
    lv_style_t * style_scrl = lv_page_get_style(page, LV_PAGE_STYLE_SCRL);

    lv_coord_t obj_y = obj->coords.y1 - ext->scrl->coords.y1;
    lv_coord_t obj_h = lv_obj_get_height(obj);
    lv_coord_t scrlable_y = lv_obj_get_y(ext->scrl);
    lv_coord_t page_h = lv_obj_get_height(page);

    lv_coord_t top_err = -(scrlable_y + obj_y);
    lv_coord_t bot_err = scrlable_y + obj_y + obj_h - page_h;

    /*If obj is higher then the page focus where the "error" is smaller*/

    /*Out of the page on the top*/
    if((obj_h <= page_h && top_err > 0) ||
            (obj_h > page_h && top_err < bot_err)) {
        /*Calculate a new position and let some space above*/
        scrlable_y = -(obj_y - style_scrl->body.padding.ver - style->body.padding.ver);
        scrlable_y += style_scrl->body.padding.ver;
    }
    /*Out of the page on the bottom*/
    else if((obj_h <= page_h && bot_err > 0) ||
            (obj_h > page_h && top_err >= bot_err)) {
        /*Calculate a new position and let some space below*/
        scrlable_y = -(obj_y + style_scrl->body.padding.ver + style->body.padding.ver);
        scrlable_y -= style_scrl->body.padding.ver;
        scrlable_y += page_h - obj_h;
    } else {
        /*Already in focus*/
        return;
    }

    if(anim_time == 0) {
        lv_obj_set_y(ext->scrl, scrlable_y);
#if USE_LV_ANIMATION
    } else {
        lv_anim_t a;
        a.act_time = 0;
        a.start = lv_obj_get_y(ext->scrl);
        a.end = scrlable_y;
        a.time = anim_time;
        a.end_cb = NULL;
        a.playback = 0;
        a.repeat = 0;
        a.var = ext->scrl;
        a.path = lv_anim_path_linear;
        a.fp = (lv_anim_fp_t) lv_obj_set_y;
        lv_anim_create(&a);
#endif
    }
}

/**
 * Scroll the page horizontally
 * @param page pointer to a page object
 * @param dist the distance to scroll (< 0: scroll right; > 0 scroll left)
 */
void lv_page_scroll_hor(lv_obj_t * page, lv_coord_t dist)
{
    lv_obj_t * scrl = lv_page_get_scrl(page);

#if USE_LV_ANIMATION
    lv_anim_t a;
    a.var = scrl;
    a.start = lv_obj_get_x(scrl);
    a.end = a.start + dist;
    a.fp = (lv_anim_fp_t)lv_obj_set_x;
    a.path = lv_anim_path_linear;
    a.end_cb = NULL;
    a.act_time = 0;
    a.time = LV_PAGE_SCROLL_ANIM_TIME;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 0;
    a.repeat_pause = 0;
    lv_anim_create(&a);
#else
    lv_obj_set_x(scrl, lv_obj_get_x(scrl) + dist);
#endif
}

/**
 * Scroll the page vertically
 * @param page pointer to a page object
 * @param dist the distance to scroll (< 0: scroll down; > 0 scroll up)
 */
void lv_page_scroll_ver(lv_obj_t * page, lv_coord_t dist)
{
    lv_obj_t * scrl = lv_page_get_scrl(page);

#if USE_LV_ANIMATION
    lv_anim_t a;
    a.var = scrl;
    a.start = lv_obj_get_y(scrl);
    a.end = a.start + dist;
    a.fp = (lv_anim_fp_t)lv_obj_set_y;
    a.path = lv_anim_path_linear;
    a.end_cb = NULL;
    a.act_time = 0;
    a.time = LV_PAGE_SCROLL_ANIM_TIME;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 0;
    a.repeat_pause = 0;
    lv_anim_create(&a);
#else
    lv_obj_set_y(scrl, lv_obj_get_x(scrl) + dist);
#endif
}

/**
 * Not intended to use directly by the user but by other object types internally.
 * Start an edge flash animation. Exactly one `ext->edge_flash.xxx_ip` should be set
 * @param page
 */
void lv_page_start_edge_flash(lv_obj_t * page)
{
#if USE_LV_ANIMATION
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    if(ext->edge_flash.enabled) {
        lv_anim_t a;
        a.var = page;
        a.start = 0;
        a.end = LV_PAGE_END_FLASH_SIZE;
        a.fp = (lv_anim_fp_t)edge_flash_anim;
        a.path = lv_anim_path_linear;
        a.end_cb = edge_flash_anim_end;
        a.act_time = 0;
        a.time = LV_PAGE_END_ANIM_TIME;
        a.playback = 1;
        a.playback_pause = LV_PAGE_END_ANIM_WAIT_TIME;
        a.repeat = 0;
        a.repeat_pause = 0;
        lv_anim_create(&a);
    }
#endif
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the pages
 * @param page pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_page_design(lv_obj_t * page, const lv_area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_design(page, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
        /*Draw without border*/
        lv_style_t * style = lv_page_get_style(page, LV_PAGE_STYLE_BG);
        lv_coord_t border_width_tmp =  style->body.border.width;
        style->body.border.width = 0;
        lv_draw_rect(&page->coords, mask, style, lv_obj_get_opa_scale(page));
        style->body.border.width = border_width_tmp;

    } else if(mode == LV_DESIGN_DRAW_POST) { /*Draw the scroll bars finally*/

        /*Draw only a border*/
        lv_style_t * style = lv_page_get_style(page, LV_PAGE_STYLE_BG);
        lv_coord_t shadow_width_tmp =  style->body.shadow.width;
        uint8_t empty_tmp =  style->body.empty;
        style->body.shadow.width = 0;
        style->body.empty = 1;
        lv_draw_rect(&page->coords, mask, style, lv_obj_get_opa_scale(page));
        style->body.shadow.width = shadow_width_tmp;
        style->body.empty = empty_tmp;

        lv_page_ext_t * ext = lv_obj_get_ext_attr(page);

        /*Draw the scrollbars*/
        lv_area_t sb_area;
        if(ext->sb.hor_draw && (ext->sb.mode & LV_SB_MODE_HIDE) == 0) {
            /*Convert the relative coordinates to absolute*/
            lv_area_copy(&sb_area, &ext->sb.hor_area);
            sb_area.x1 += page->coords.x1;
            sb_area.y1 += page->coords.y1;
            sb_area.x2 += page->coords.x1;
            sb_area.y2 += page->coords.y1;
            lv_draw_rect(&sb_area, mask, ext->sb.style, lv_obj_get_opa_scale(page));
        }

        if(ext->sb.ver_draw && (ext->sb.mode & LV_SB_MODE_HIDE) == 0) {
            /*Convert the relative coordinates to absolute*/
            lv_area_copy(&sb_area, &ext->sb.ver_area);
            sb_area.x1 += page->coords.x1;
            sb_area.y1 += page->coords.y1;
            sb_area.x2 += page->coords.x1;
            sb_area.y2 += page->coords.y1;
            lv_draw_rect(&sb_area, mask, ext->sb.style, lv_obj_get_opa_scale(page));
        }


        lv_coord_t page_w = lv_obj_get_width(page);
        lv_coord_t page_h = lv_obj_get_height(page);
        lv_area_t flash_area;

        if(ext->edge_flash.top_ip) {
            flash_area.x1 = page->coords.x1 - page_w;
            flash_area.x2 = page->coords.x2 + page_w;
            flash_area.y1 = page->coords.y1 - 3 * page_w + ext->edge_flash.state;
            flash_area.y2 = page->coords.y1 + ext->edge_flash.state;
        }
        else if(ext->edge_flash.bottom_ip) {
            flash_area.x1 = page->coords.x1 - page_w;
            flash_area.x2 = page->coords.x2 + page_w;
            flash_area.y1 = page->coords.y2 - ext->edge_flash.state;
            flash_area.y2 = page->coords.y2 + 3 * page_w - ext->edge_flash.state;
        }
        else if(ext->edge_flash.right_ip) {
            flash_area.x1 = page->coords.x2 - ext->edge_flash.state;
            flash_area.x2 = page->coords.x2 + 3 * page_h - ext->edge_flash.state;
            flash_area.y1 = page->coords.y1 - page_h;
            flash_area.y2 = page->coords.y2 + page_h;
        }
        else if(ext->edge_flash.left_ip) {
            flash_area.x1 = page->coords.x1 - 3 * page_h + ext->edge_flash.state;
            flash_area.x2 = page->coords.x1 + ext->edge_flash.state;
            flash_area.y1 = page->coords.y1 - page_h;
            flash_area.y2 = page->coords.y2 + page_h;
        }

        if(ext->edge_flash.left_ip || ext->edge_flash.right_ip || ext->edge_flash.top_ip || ext->edge_flash.bottom_ip) {
            lv_style_t flash_style;
            lv_style_copy(&flash_style, ext->edge_flash.style);
            flash_style.body.radius = LV_RADIUS_CIRCLE;
            uint32_t opa = (flash_style.body.opa * ext->edge_flash.state) / LV_PAGE_END_FLASH_SIZE;
            flash_style.body.opa = opa;
            lv_draw_rect(&flash_area, mask, &flash_style, lv_obj_get_opa_scale(page));
        }

    }

    return true;
}

/**
 * Handle the drawing related tasks of the scrollable object
 * @param scrl pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_scrl_design(lv_obj_t * scrl, const lv_area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_design(scrl, mask, mode);
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
#if USE_LV_GROUP
        /* If the page is focused in a group and
         * the background object is not visible (transparent or empty)
         * then "activate" the style of the scrollable*/
        lv_style_t * style_scrl_ori = lv_obj_get_style(scrl);
        lv_obj_t * page = lv_obj_get_parent(scrl);
        lv_style_t * style_page = lv_obj_get_style(page);
        lv_group_t * g = lv_obj_get_group(page);
        if((style_page->body.empty || style_page->body.opa == LV_OPA_TRANSP) && style_page->body.border.width == 0) { /*Is the background visible?*/
            if(lv_group_get_focused(g) == page) {
                lv_style_t * style_mod;
                style_mod = lv_group_mod_style(g, style_scrl_ori);
                scrl->style_p = style_mod;  /*Temporally change the style to the activated */
            }
        }
#endif
        ancestor_design(scrl, mask, mode);

#if USE_LV_GROUP
        scrl->style_p = style_scrl_ori;  /*Revert the style*/
#endif
    } else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design(scrl, mask, mode);
    }

    return true;
}

/**
 * Signal function of the page
 * @param page pointer to a page object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_page_signal(lv_obj_t * page, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(page, sign, param);
    if(res != LV_RES_OK) return res;

    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    lv_style_t * style = lv_obj_get_style(page);
    lv_obj_t * child;
    if(sign == LV_SIGNAL_CHILD_CHG) { /*Automatically move children to the scrollable object*/
        child = lv_obj_get_child(page, NULL);
        while(child != NULL) {
            if(lv_obj_is_protected(child, LV_PROTECT_PARENT) == false) {
                lv_obj_t * tmp = child;
                child = lv_obj_get_child(page, child); /*Get the next child before move this*/
                lv_obj_set_parent(tmp, ext->scrl);
            } else {
                child = lv_obj_get_child(page, child);
            }
        }
    } else if(sign == LV_SIGNAL_STYLE_CHG) {
        /*If no hor_fit enabled set the scrollable's width to the page's width*/
        if(lv_cont_get_hor_fit(ext->scrl) == false) {
            lv_obj_set_width(ext->scrl, lv_obj_get_width(page) - 2 * style->body.padding.hor);
        } else {
            ext->scrl->signal_func(ext->scrl, LV_SIGNAL_CORD_CHG, &ext->scrl->coords);
        }

        /*The scrollbars are important only if they are visible now*/
        if(ext->sb.hor_draw || ext->sb.ver_draw) lv_page_sb_refresh(page);

        /*Refresh the ext. size because the scrollbars might be positioned out of the page*/
        lv_obj_refresh_ext_size(page);
    } else if(sign == LV_SIGNAL_CORD_CHG) {
        /*Refresh the scrollbar and notify the scrl if the size is changed*/
        if(ext->scrl != NULL && (lv_obj_get_width(page) != lv_area_get_width(param) ||
                                 lv_obj_get_height(page) != lv_area_get_height(param))) {
            /*If no hor_fit enabled set the scrollable's width to the page's width*/
            if(lv_cont_get_hor_fit(ext->scrl) == false) {
                lv_obj_set_width(ext->scrl, lv_obj_get_width(page) - 2 * style->body.padding.hor);
            }

            ext->scrl->signal_func(ext->scrl, LV_SIGNAL_CORD_CHG, &ext->scrl->coords);

            /*The scrollbars are important only if they are visible now*/
            if(ext->sb.hor_draw || ext->sb.ver_draw) lv_page_sb_refresh(page);
        }
    } else if(sign == LV_SIGNAL_PRESSED) {
        if(ext->pr_action != NULL) {
            res = ext->pr_action(page);
        }
    } else if(sign == LV_SIGNAL_RELEASED) {
        if(lv_indev_is_dragging(lv_indev_get_act()) == false) {
            if(ext->rel_action != NULL) {
                res = ext->rel_action(page);
            }
        }
    } else if(sign == LV_SIGNAL_REFR_EXT_SIZE) {
        /*Ensure ext. size for the scrollbars if they are out of the page*/
        if(page->ext_size < (-ext->sb.style->body.padding.hor)) page->ext_size = -ext->sb.style->body.padding.hor;
        if(page->ext_size < (-ext->sb.style->body.padding.ver)) page->ext_size = -ext->sb.style->body.padding.ver;
    } else if(sign == LV_SIGNAL_CONTROLL) {
        uint32_t c = *((uint32_t *) param);

        if((c == LV_GROUP_KEY_DOWN) && ext->arrow_scroll) {
            lv_page_scroll_ver(page, - lv_obj_get_height(page) / 4);
        } else if((c == LV_GROUP_KEY_UP) && ext->arrow_scroll) {
            lv_page_scroll_ver(page, lv_obj_get_height(page) / 4);
        } else if((c == LV_GROUP_KEY_RIGHT) && ext->arrow_scroll) {
            /*If the page can be scrolled horizontally because it's not wide enough then scroll it vertically*/
            if(lv_page_get_scrl_width(page) < lv_obj_get_width(page)) lv_page_scroll_ver(page, - lv_obj_get_height(page) / 4);
            else lv_page_scroll_hor(page, - lv_obj_get_width(page) / 4);
        } else if((c == LV_GROUP_KEY_LEFT) && ext->arrow_scroll) {
            /*If the page can be scrolled horizontally because it's not wide enough then scroll it vertically*/
            if(lv_page_get_scrl_width(page) < lv_obj_get_width(page)) lv_page_scroll_ver(page, lv_obj_get_height(page) / 4);
            else lv_page_scroll_hor(page,  lv_obj_get_width(page) / 4);
        }
    } else if(sign == LV_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable = lv_page_get_arrow_scroll(page);
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_page";
    }

    return res;
}

/**
 * Signal function of the scrollable part of a page
 * @param scrl pointer to the scrollable object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_page_scrollable_signal(lv_obj_t * scrl, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(scrl, sign, param);
    if(res != LV_RES_OK) return res;

    lv_obj_t * page = lv_obj_get_parent(scrl);
    lv_style_t * page_style = lv_obj_get_style(page);
    lv_page_ext_t * page_ext = lv_obj_get_ext_attr(page);

    if(sign == LV_SIGNAL_CORD_CHG) {
        /*Limit the position of the scrollable object to be always visible
         * (Do not let its edge inner then its parent respective edge)*/
        lv_coord_t new_x = lv_obj_get_x(scrl);
        lv_coord_t new_y = lv_obj_get_y(scrl);
        bool refr_x = false;
        bool refr_y = false;
        lv_area_t page_coords;
        lv_area_t scrl_coords;
        lv_obj_get_coords(scrl, &scrl_coords);
        lv_obj_get_coords(page, &page_coords);

        lv_area_t * ori_coords = (lv_area_t *) param;
        lv_coord_t diff_x = scrl->coords.x1 - ori_coords->x1;
        lv_coord_t diff_y = scrl->coords.y1 - ori_coords->y1;
        lv_coord_t hpad = page_style->body.padding.hor;
        lv_coord_t vpad = page_style->body.padding.ver;
        lv_obj_t * page_parent = lv_obj_get_parent(page);

        lv_indev_t * indev = lv_indev_get_act();
        lv_point_t drag_vect;
        lv_indev_get_vect(indev, &drag_vect);


        /* Start the scroll propagation if there is drag vector on the indev, but the drag is not started yet
         * and the scrollable is in a corner. It will enable the scroll propagation only when a new scroll begins and not
         * when the scrollable is already being scrolled.*/
        if(page_ext->scroll_prop && page_ext->scroll_prop_ip == 0 && lv_indev_is_dragging(indev) == false) {
            if(((drag_vect.y > 0 && scrl_coords.y1 == page_coords.y1 + vpad) ||
                (drag_vect.y < 0 && scrl_coords.y2 == page_coords.y2 - vpad)) &&
               ((drag_vect.x > 0 && scrl_coords.x1 == page_coords.x1 + hpad) ||
                (drag_vect.x < 0 && scrl_coords.x2 == page_coords.x2 - hpad))) {

                if(lv_obj_get_parent(page_parent) != NULL) {    /*Do not propagate the scroll to a screen*/
                    page_ext->scroll_prop_ip = 1;
                }
            }
        }

        /*scrollable width smaller then page width? -> align to left*/
        if(lv_area_get_width(&scrl_coords) + 2 * hpad <= lv_area_get_width(&page_coords)) {
            if(scrl_coords.x1 != page_coords.x1 + hpad) {
                new_x = hpad;
                refr_x = true;
            }
        } else {
            /*If the scroll propagation is in progress revert the original coordinates (don't let the page scroll)*/
            if(page_ext->scroll_prop_ip) {
                if(drag_vect.x == diff_x) {   /*`scrl` is bouncing: drag pos. it somewhere and here it is reverted. Handle only the pos. because of drag*/
                    new_x = ori_coords->x1 - page_coords.x1;
                    refr_x = true;
                }
            }
            /*The edges of the scrollable can not be in the page (minus hpad) */
            else if(scrl_coords.x2  < page_coords.x2 - hpad) {
                new_x =  lv_area_get_width(&page_coords) - lv_area_get_width(&scrl_coords) - hpad;   /* Right align */
                refr_x = true;
                if(page_ext->edge_flash.enabled &&
                        page_ext->edge_flash.left_ip == 0 && page_ext->edge_flash.right_ip == 0 &&
                        page_ext->edge_flash.top_ip == 0 && page_ext->edge_flash.bottom_ip == 0) {
                    lv_page_start_edge_flash(page);
                    page_ext->edge_flash.right_ip = 1;
                }
            }
            else if(scrl_coords.x1 > page_coords.x1 + hpad) {
                new_x = hpad;  /*Left align*/
                refr_x = true;
                if(page_ext->edge_flash.enabled &&
                        page_ext->edge_flash.left_ip == 0 && page_ext->edge_flash.right_ip == 0 &&
                        page_ext->edge_flash.top_ip == 0 && page_ext->edge_flash.bottom_ip == 0) {
                    lv_page_start_edge_flash(page);
                    page_ext->edge_flash.left_ip = 1;
                }
            }
        }

        /*scrollable height smaller then page height? -> align to left*/
        if(lv_area_get_height(&scrl_coords) + 2 * vpad <= lv_area_get_height(&page_coords)) {
            if(scrl_coords.y1 != page_coords.y1 + vpad) {
                new_y = vpad;
                refr_y = true;
            }
        } else {
            /*If the scroll propagation is in progress revert the original coordinates (don't let the page scroll)*/
            if(page_ext->scroll_prop_ip) {
              if(drag_vect.y == diff_y) {   /*`scrl` is bouncing: drag pos. it somewhere and here it is reverted. Handle only the pos. because of drag*/
                    new_y = ori_coords->y1 - page_coords.y1;
                    refr_y = true;
                }
            }
            /*The edges of the scrollable can not be in the page (minus vpad) */
            else if(scrl_coords.y2 < page_coords.y2 - vpad) {
                new_y =  lv_area_get_height(&page_coords) - lv_area_get_height(&scrl_coords) - vpad;   /* Bottom align */
                refr_y = true;
                if(page_ext->edge_flash.enabled &&
                        page_ext->edge_flash.left_ip == 0 && page_ext->edge_flash.right_ip == 0 &&
                        page_ext->edge_flash.top_ip == 0 && page_ext->edge_flash.bottom_ip == 0) {
                    lv_page_start_edge_flash(page);
                    page_ext->edge_flash.bottom_ip = 1;
                }
            }
            else if(scrl_coords.y1  > page_coords.y1 + vpad) {
                new_y = vpad;  /*Top align*/
                refr_y = true;
                if(page_ext->edge_flash.enabled &&
                        page_ext->edge_flash.left_ip == 0 && page_ext->edge_flash.right_ip == 0 &&
                        page_ext->edge_flash.top_ip == 0 && page_ext->edge_flash.bottom_ip == 0) {
                    lv_page_start_edge_flash(page);
                    page_ext->edge_flash.top_ip = 1;
                }
            }
        }

        if(refr_x || refr_y) {
            lv_obj_set_pos(scrl, new_x, new_y);

            if(page_ext->scroll_prop_ip) {
                if(refr_y) lv_obj_set_y(page_parent, lv_obj_get_y(page_parent) + diff_y);
                if(refr_x) lv_obj_set_x(page_parent, lv_obj_get_x(page_parent) + diff_x);
            }
        }

        lv_page_sb_refresh(page);
    }
    else if(sign == LV_SIGNAL_DRAG_END) {

        /*Scroll propagation is finished on drag end*/
        page_ext->scroll_prop_ip = 0;

        /*Hide scrollbars if required*/
        if(page_ext->sb.mode == LV_SB_MODE_DRAG) {
            lv_area_t sb_area_tmp;
            if(page_ext->sb.hor_draw) {
                lv_area_copy(&sb_area_tmp, &page_ext->sb.hor_area);
                sb_area_tmp.x1 += page->coords.x1;
                sb_area_tmp.y1 += page->coords.y1;
                sb_area_tmp.x2 += page->coords.x1;
                sb_area_tmp.y2 += page->coords.y1;
                lv_inv_area(&sb_area_tmp);
                page_ext->sb.hor_draw = 0;
            }
            if(page_ext->sb.ver_draw)  {
                lv_area_copy(&sb_area_tmp, &page_ext->sb.ver_area);
                sb_area_tmp.x1 += page->coords.x1;
                sb_area_tmp.y1 += page->coords.y1;
                sb_area_tmp.x2 += page->coords.x1;
                sb_area_tmp.y2 += page->coords.y1;
                lv_inv_area(&sb_area_tmp);
                page_ext->sb.ver_draw = 0;
            }
        }
    } else if(sign == LV_SIGNAL_PRESSED) {
        if(page_ext->pr_action != NULL) {
            res = page_ext->pr_action(page);
        }
    } else if(sign == LV_SIGNAL_RELEASED) {
        if(lv_indev_is_dragging(lv_indev_get_act()) == false) {
            if(page_ext->rel_action != NULL) {
                res = page_ext->rel_action(page);
            }
        }
    }

    return res;
}


/**
 * Refresh the position and size of the scroll bars.
 * @param page pointer to a page object
 */
static void lv_page_sb_refresh(lv_obj_t * page)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    lv_style_t * style = lv_obj_get_style(page);
    lv_obj_t * scrl = ext->scrl;
    lv_coord_t size_tmp;
    lv_coord_t scrl_w = lv_obj_get_width(scrl);
    lv_coord_t scrl_h =  lv_obj_get_height(scrl);
    lv_coord_t hpad = style->body.padding.hor;
    lv_coord_t vpad = style->body.padding.ver;
    lv_coord_t obj_w = lv_obj_get_width(page);
    lv_coord_t obj_h = lv_obj_get_height(page);

    /*Always let 'scrollbar width' padding above, under, left and right to the scrollbars
     * else:
     * - horizontal and vertical scrollbars can overlap on the corners
     * - if the page has radius the scrollbar can be out of the radius  */
    lv_coord_t sb_hor_pad = LV_MATH_MAX(ext->sb.style->body.padding.inner, style->body.padding.hor);
    lv_coord_t sb_ver_pad = LV_MATH_MAX(ext->sb.style->body.padding.inner, style->body.padding.ver);

    if(ext->sb.mode == LV_SB_MODE_OFF) return;

    if(ext->sb.mode == LV_SB_MODE_ON) {
        ext->sb.hor_draw = 1;
        ext->sb.ver_draw = 1;
    }

    /*Invalidate the current (old) scrollbar areas*/
    lv_area_t sb_area_tmp;
    if(ext->sb.hor_draw != 0) {
        lv_area_copy(&sb_area_tmp, &ext->sb.hor_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        lv_inv_area(&sb_area_tmp);
    }
    if(ext->sb.ver_draw != 0)  {
        lv_area_copy(&sb_area_tmp, &ext->sb.ver_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        lv_inv_area(&sb_area_tmp);
    }


    if(ext->sb.mode == LV_SB_MODE_DRAG && lv_indev_is_dragging(lv_indev_get_act()) == false) {
        ext->sb.hor_draw = 0;
        ext->sb.ver_draw = 0;
        return;

    }

    /*Horizontal scrollbar*/
    if(scrl_w <= obj_w - 2 * hpad) {        /*Full sized scroll bar*/
        lv_area_set_width(&ext->sb.hor_area, obj_w - 2 * sb_hor_pad);
        lv_area_set_pos(&ext->sb.hor_area, sb_hor_pad, obj_h - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.ver);
        if(ext->sb.mode == LV_SB_MODE_AUTO || ext->sb.mode == LV_SB_MODE_DRAG)  ext->sb.hor_draw = 0;
    } else {
        size_tmp = (obj_w * (obj_w - (2 * sb_hor_pad))) / (scrl_w + 2 * hpad);
        if(size_tmp < LV_PAGE_SB_MIN_SIZE) size_tmp = LV_PAGE_SB_MIN_SIZE;
        lv_area_set_width(&ext->sb.hor_area,  size_tmp);

        lv_area_set_pos(&ext->sb.hor_area, sb_hor_pad +
                        (-(lv_obj_get_x(scrl) - hpad) * (obj_w - size_tmp -  2 * sb_hor_pad)) /
                        (scrl_w + 2 * hpad - obj_w),
                        obj_h - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.ver);

        if(ext->sb.mode == LV_SB_MODE_AUTO || ext->sb.mode == LV_SB_MODE_DRAG)  ext->sb.hor_draw = 1;
    }

    /*Vertical scrollbar*/
    if(scrl_h <= obj_h - 2 * vpad) {        /*Full sized scroll bar*/
        lv_area_set_height(&ext->sb.ver_area,  obj_h - 2 * sb_ver_pad);
        lv_area_set_pos(&ext->sb.ver_area, obj_w - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.hor, sb_ver_pad);
        if(ext->sb.mode == LV_SB_MODE_AUTO || ext->sb.mode == LV_SB_MODE_DRAG)  ext->sb.ver_draw = 0;
    } else {
        size_tmp = (obj_h * (obj_h - (2 * sb_ver_pad))) / (scrl_h + 2 * vpad);
        if(size_tmp < LV_PAGE_SB_MIN_SIZE) size_tmp = LV_PAGE_SB_MIN_SIZE;
        lv_area_set_height(&ext->sb.ver_area,  size_tmp);

        lv_area_set_pos(&ext->sb.ver_area,  obj_w - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.hor,
                        sb_ver_pad +
                        (-(lv_obj_get_y(scrl) - vpad) * (obj_h - size_tmp -  2 * sb_ver_pad)) /
                        (scrl_h + 2 * vpad - obj_h));

        if(ext->sb.mode == LV_SB_MODE_AUTO || ext->sb.mode == LV_SB_MODE_DRAG)  ext->sb.ver_draw = 1;
    }

    /*Invalidate the new scrollbar areas*/
    if(ext->sb.hor_draw != 0) {
        lv_area_copy(&sb_area_tmp, &ext->sb.hor_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        lv_inv_area(&sb_area_tmp);
    }
    if(ext->sb.ver_draw != 0)  {
        lv_area_copy(&sb_area_tmp, &ext->sb.ver_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        lv_inv_area(&sb_area_tmp);
    }
}

static void edge_flash_anim(void * page, int32_t v)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    ext->edge_flash.state = v;
    lv_obj_invalidate(page);
}

static void edge_flash_anim_end(void * page)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    ext->edge_flash.top_ip = 0;
    ext->edge_flash.bottom_ip = 0;
    ext->edge_flash.left_ip = 0;
    ext->edge_flash.right_ip = 0;
    lv_obj_invalidate(page);
}

#endif
