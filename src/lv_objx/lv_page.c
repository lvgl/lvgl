/**
 * @file lv_page.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_objx/lv_page.h"
#if LV_USE_PAGE != 0

#include "../lv_core/lv_group.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_core/lv_refr.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LV_PAGE_SB_MIN_SIZE (LV_DPI / 8)

/*[ms] Scroll anim time on `lv_page_scroll_up/down/left/rigth`*/
#define LV_PAGE_SCROLL_ANIM_TIME 200

#define LV_PAGE_END_FLASH_SIZE (LV_DPI / 4)
#define LV_PAGE_END_ANIM_TIME 300
#define LV_PAGE_END_ANIM_WAIT_TIME 300

#if LV_USE_ANIMATION == 0
#undef LV_PAGE_DEF_ANIM_TIME
#define LV_PAGE_DEF_ANIM_TIME 0 /*No animation*/
#endif

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
static void scrl_def_event_cb(lv_obj_t * scrl, lv_event_t event);
#if LV_USE_ANIMATION
static void edge_flash_anim(void * page, lv_anim_value_t v);
static void edge_flash_anim_end(lv_anim_t * a);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_cb_t ancestor_design;
static lv_signal_cb_t ancestor_signal;

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

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_page);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(new_page);

    /*Allocate the object type specific extended data*/
    lv_page_ext_t * ext = lv_obj_allocate_ext_attr(new_page, sizeof(lv_page_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->scrl        = NULL;
    ext->sb.hor_draw = 0;
    ext->sb.ver_draw = 0;
    ext->sb.style    = &lv_style_pretty;
    ext->sb.mode     = LV_SB_MODE_AUTO;
#if LV_USE_ANIMATION
    ext->edge_flash.enabled   = 0;
    ext->edge_flash.bottom_ip = 0;
    ext->edge_flash.top_ip    = 0;
    ext->edge_flash.left_ip   = 0;
    ext->edge_flash.right_ip  = 0;
    ext->edge_flash.state     = 0;
    ext->edge_flash.style     = &lv_style_plain_color;
    ext->anim_time            = LV_PAGE_DEF_ANIM_TIME;
#endif
    ext->scroll_prop    = 0;
    ext->scroll_prop_ip = 0;

    /*Init the new page object*/
    if(copy == NULL) {
        ext->scrl = lv_cont_create(new_page, NULL);
        lv_obj_set_signal_cb(ext->scrl, lv_page_scrollable_signal);
        lv_obj_set_design_cb(ext->scrl, lv_scrl_design);
        lv_obj_set_drag(ext->scrl, true);
        lv_obj_set_drag_throw(ext->scrl, true);
        lv_obj_set_protect(ext->scrl, LV_PROTECT_PARENT | LV_PROTECT_PRESS_LOST);
        lv_cont_set_fit4(ext->scrl, LV_FIT_FILL, LV_FIT_FILL, LV_FIT_FILL, LV_FIT_FILL);
        lv_obj_set_event_cb(ext->scrl, scrl_def_event_cb); /*Propagate some event to the background
                                                              object by default for convenience */

        /* Add the signal function only if 'scrolling' is created
         * because everything has to be ready before any signal is received*/
        lv_obj_set_signal_cb(new_page, lv_page_signal);
        lv_obj_set_design_cb(new_page, lv_page_design);

        lv_page_set_sb_mode(new_page, ext->sb.mode);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            if(par == NULL) { /*Different styles if it is screen*/
                lv_page_set_style(new_page, LV_PAGE_STYLE_BG, th->style.bg);
                lv_page_set_style(new_page, LV_PAGE_STYLE_SCRL, &lv_style_transp);
            } else {
                lv_page_set_style(new_page, LV_PAGE_STYLE_BG, th->style.page.bg);
                lv_page_set_style(new_page, LV_PAGE_STYLE_SCRL, th->style.page.scrl);
            }
            lv_page_set_style(new_page, LV_PAGE_STYLE_SB, th->style.page.sb);
        } else {
            lv_page_set_style(new_page, LV_PAGE_STYLE_BG, &lv_style_pretty_color);
            lv_page_set_style(new_page, LV_PAGE_STYLE_SCRL, &lv_style_pretty);
            lv_page_set_style(new_page, LV_PAGE_STYLE_SB, &lv_style_pretty_color);
        }

    } else {
        lv_page_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->scrl                = lv_cont_create(new_page, copy_ext->scrl);
        lv_obj_set_signal_cb(ext->scrl, lv_page_scrollable_signal);

        lv_page_set_sb_mode(new_page, copy_ext->sb.mode);

        lv_page_set_style(new_page, LV_PAGE_STYLE_BG, lv_page_get_style(copy, LV_PAGE_STYLE_BG));
        lv_page_set_style(new_page, LV_PAGE_STYLE_SCRL, lv_page_get_style(copy, LV_PAGE_STYLE_SCRL));
        lv_page_set_style(new_page, LV_PAGE_STYLE_SB, lv_page_get_style(copy, LV_PAGE_STYLE_SB));

        /* Add the signal function only if 'scrolling' is created
         * because everything has to be ready before any signal is received*/
        lv_obj_set_signal_cb(new_page, lv_page_signal);
        lv_obj_set_design_cb(new_page, lv_page_design);

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
 * Set the scroll bar mode on a page
 * @param page pointer to a page object
 * @param sb_mode the new mode from 'lv_page_sb.mode_t' enum
 */
void lv_page_set_sb_mode(lv_obj_t * page, lv_sb_mode_t sb_mode)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    if(ext->sb.mode == sb_mode) return;

    if(sb_mode == LV_SB_MODE_HIDE)
        ext->sb.mode |= LV_SB_MODE_HIDE; /*Set the hidden flag*/
    else if(sb_mode == LV_SB_MODE_UNHIDE)
        ext->sb.mode &= (~LV_SB_MODE_HIDE); /*Clear the hidden flag*/
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
 * Set the animation time for the page
 * @param page pointer to a page object
 * @param anim_time animation time in milliseconds
 */
void lv_page_set_anim_time(lv_obj_t * page, uint16_t anim_time)
{
#if LV_USE_ANIMATION
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    ext->anim_time      = anim_time;
#else
    (void)page;      /*Unused*/
    (void)anim_time; /*Unused*/
#endif
}

/**
 * Enable the scroll propagation feature. If enabled then the page will move its parent if there is
 * no more space to scroll.
 * @param page pointer to a Page
 * @param en true or false to enable/disable scroll propagation
 */
void lv_page_set_scroll_propagation(lv_obj_t * page, bool en)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    ext->scroll_prop    = en ? 1 : 0;
}

/**
 * Enable the edge flash effect. (Show an arc when the an edge is reached)
 * @param page pointer to a Page
 * @param en true or false to enable/disable end flash
 */
void lv_page_set_edge_flash(lv_obj_t * page, bool en)
{
#if LV_USE_ANIMATION
    lv_page_ext_t * ext     = lv_obj_get_ext_attr(page);
    ext->edge_flash.enabled = en ? 1 : 0;
#else
    (void)page;
    (void)en;
#endif
}

/**
 * Set a style of a page
 * @param page pointer to a page object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_page_set_style(lv_obj_t * page, lv_page_style_t type, const lv_style_t * style)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);

    switch(type) {
        case LV_PAGE_STYLE_BG: lv_obj_set_style(page, style); break;
        case LV_PAGE_STYLE_SCRL: lv_obj_set_style(ext->scrl, style); break;
        case LV_PAGE_STYLE_SB:
            ext->sb.style = style;
            lv_area_set_height(&ext->sb.hor_area, ext->sb.style->body.padding.inner);
            lv_area_set_width(&ext->sb.ver_area, ext->sb.style->body.padding.inner);
            lv_page_sb_refresh(page);
            lv_obj_refresh_ext_draw_pad(page);
            lv_obj_invalidate(page);
            break;
#if LV_USE_ANIMATION
        case LV_PAGE_STYLE_EDGE_FLASH: ext->edge_flash.style = style; break;
#endif
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
 * Get the animation time
 * @param page pointer to a page object
 * @return the animation time in milliseconds
 */
uint16_t lv_page_get_anim_time(const lv_obj_t * page)
{
#if LV_USE_ANIMATION
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    return ext->anim_time;
#else
    (void)page; /*Unused*/
    return 0;
#endif
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
#if LV_USE_ANIMATION
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    return ext->edge_flash.enabled == 0 ? false : true;
#else
    (void)page;
    return false;
#endif
}

/**
 * Get that width which can be set to the children to still not cause overflow (show scrollbars)
 * @param page pointer to a page object
 * @return the width which still fits into the page
 */
lv_coord_t lv_page_get_fit_width(lv_obj_t * page)
{
    const lv_style_t * bg_style   = lv_page_get_style(page, LV_PAGE_STYLE_BG);
    const lv_style_t * scrl_style = lv_page_get_style(page, LV_PAGE_STYLE_SCRL);

    return lv_obj_get_width(page) - bg_style->body.padding.left - bg_style->body.padding.right -
           scrl_style->body.padding.left - scrl_style->body.padding.right;
}

/**
 * Get that height which can be set to the children to still not cause overflow (show scrollbars)
 * @param page pointer to a page object
 * @return the height which still fits into the page
 */
lv_coord_t lv_page_get_fit_height(lv_obj_t * page)
{
    const lv_style_t * bg_style   = lv_page_get_style(page, LV_PAGE_STYLE_BG);
    const lv_style_t * scrl_style = lv_page_get_style(page, LV_PAGE_STYLE_SCRL);

    return lv_obj_get_height(page) - bg_style->body.padding.top - bg_style->body.padding.bottom -
           scrl_style->body.padding.top - scrl_style->body.padding.bottom;
}

/**
 * Get a style of a page
 * @param page pointer to page object
 * @param type which style should be get
 * @return style pointer to a style
 *  */
const lv_style_t * lv_page_get_style(const lv_obj_t * page, lv_page_style_t type)
{
    const lv_style_t * style = NULL;
    lv_page_ext_t * ext      = lv_obj_get_ext_attr(page);

    switch(type) {
        case LV_PAGE_STYLE_BG: style = lv_obj_get_style(page); break;
        case LV_PAGE_STYLE_SCRL: style = lv_obj_get_style(ext->scrl); break;
        case LV_PAGE_STYLE_SB: style = ext->sb.style; break;
#if LV_USE_ANIMATION
        case LV_PAGE_STYLE_EDGE_FLASH: style = ext->edge_flash.style; break;
#endif
        default: style = NULL; break;
    }

    return style;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Find whether the page has been scrolled to a certain edge.
 * @param page Page object
 * @param edge Edge to check
 * @return true if the page is on the specified edge
 */
bool lv_page_on_edge(lv_obj_t * page, lv_page_edge_t edge)
{
    const lv_style_t * page_style = lv_obj_get_style(page);
    lv_obj_t * scrl               = lv_page_get_scrl(page);
    lv_area_t page_coords;
    lv_area_t scrl_coords;

    lv_obj_get_coords(scrl, &scrl_coords);
    lv_obj_get_coords(page, &page_coords);

    if((edge & LV_PAGE_EDGE_TOP) && scrl_coords.y1 == page_coords.y1 + page_style->body.padding.top) return true;
    if((edge & LV_PAGE_EDGE_BOTTOM) && scrl_coords.y2 == page_coords.y2 - page_style->body.padding.bottom) return true;
    if((edge & LV_PAGE_EDGE_LEFT) && scrl_coords.x1 == page_coords.x1 + page_style->body.padding.left) return true;
    if((edge & LV_PAGE_EDGE_RIGHT) && scrl_coords.x2 == page_coords.x2 - page_style->body.padding.right) return true;

    return false;
}

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
 * @param anim_en LV_ANIM_ON to focus with animation; LV_ANIM_OFF to focus without animation
 */
void lv_page_focus(lv_obj_t * page, const lv_obj_t * obj, lv_anim_enable_t anim_en)
{
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);

#if LV_USE_ANIMATION
    /* Be sure there is no position changing animation in progress
     * because it can overide the current changes*/
    lv_anim_del(page, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_del(page, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_del(ext->scrl, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_del(ext->scrl, (lv_anim_exec_xcb_t)lv_obj_set_y);
#endif

    const lv_style_t * style      = lv_page_get_style(page, LV_PAGE_STYLE_BG);
    const lv_style_t * style_scrl = lv_page_get_style(page, LV_PAGE_STYLE_SCRL);

    /*If obj is higher then the page focus where the "error" is smaller*/
    lv_coord_t obj_y      = obj->coords.y1 - ext->scrl->coords.y1;
    lv_coord_t obj_h      = lv_obj_get_height(obj);
    lv_coord_t scrlable_y = lv_obj_get_y(ext->scrl);
    lv_coord_t page_h     = lv_obj_get_height(page);

    lv_coord_t top_err = -(scrlable_y + obj_y);
    lv_coord_t bot_err = scrlable_y + obj_y + obj_h - page_h;

    /*Out of the page on the top*/
    if((obj_h <= page_h && top_err > 0) || (obj_h > page_h && top_err < bot_err)) {
        /*Calculate a new position and let some space above*/
        scrlable_y = -(obj_y - style_scrl->body.padding.top - style->body.padding.top);
        scrlable_y += style_scrl->body.padding.top;
    }
    /*Out of the page on the bottom*/
    else if((obj_h <= page_h && bot_err > 0) || (obj_h > page_h && top_err >= bot_err)) {
        /*Calculate a new position and let some space below*/
        scrlable_y = -(obj_y + style_scrl->body.padding.bottom + style->body.padding.bottom);
        scrlable_y -= style_scrl->body.padding.bottom;
        scrlable_y += page_h - obj_h;
    }

    /*If obj is wider then the page focus where the "error" is smaller*/
    lv_coord_t obj_x      = obj->coords.x1 - ext->scrl->coords.x1;
    lv_coord_t obj_w      = lv_obj_get_width(obj);
    lv_coord_t scrlable_x = lv_obj_get_x(ext->scrl);
    lv_coord_t page_w     = lv_obj_get_width(page);

    lv_coord_t left_err  = -(scrlable_x + obj_x);
    lv_coord_t right_err = scrlable_x + obj_x + obj_w - page_w;

    /*Out of the page on the left*/
    if((obj_w <= page_w && left_err > 0) || (obj_w > page_w && left_err < right_err)) {
        /*Calculate a new position and let some space above*/
        scrlable_x = -(obj_x - style_scrl->body.padding.left - style->body.padding.left);
        scrlable_x += style_scrl->body.padding.left;
    }
    /*Out of the page on the rigth*/
    else if((obj_w <= page_w && right_err > 0) || (obj_w > page_w && left_err >= right_err)) {
        /*Calculate a new position and let some space below*/
        scrlable_x = -(obj_x + style_scrl->body.padding.right + style->body.padding.right);
        scrlable_x -= style_scrl->body.padding.right;
        scrlable_x += page_w - obj_w;
    }

    if(anim_en == LV_ANIM_OFF || lv_page_get_anim_time(page) == 0) {
        lv_obj_set_y(ext->scrl, scrlable_y);
        lv_obj_set_x(ext->scrl, scrlable_x);
    } else {
#if LV_USE_ANIMATION
        lv_anim_t a;
        a.act_time = 0;
        a.start    = lv_obj_get_y(ext->scrl);
        a.end      = scrlable_y;
        a.time     = lv_page_get_anim_time(page);
        a.ready_cb = NULL;
        a.playback = 0;
        a.repeat   = 0;
        a.var      = ext->scrl;
        a.path_cb  = lv_anim_path_linear;
        a.exec_cb  = (lv_anim_exec_xcb_t)lv_obj_set_y;
        lv_anim_create(&a);

        a.start   = lv_obj_get_x(ext->scrl);
        a.end     = scrlable_x;
        a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_x;
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

#if LV_USE_ANIMATION
    lv_anim_t a;
    a.var            = scrl;
    a.start          = lv_obj_get_x(scrl);
    a.end            = a.start + dist;
    a.exec_cb        = (lv_anim_exec_xcb_t)lv_obj_set_x;
    a.path_cb        = lv_anim_path_linear;
    a.ready_cb       = NULL;
    a.act_time       = 0;
    a.time           = LV_PAGE_SCROLL_ANIM_TIME;
    a.playback       = 0;
    a.playback_pause = 0;
    a.repeat         = 0;
    a.repeat_pause   = 0;
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

#if LV_USE_ANIMATION
    lv_anim_t a;
    a.var            = scrl;
    a.start          = lv_obj_get_y(scrl);
    a.end            = a.start + dist;
    a.exec_cb        = (lv_anim_exec_xcb_t)lv_obj_set_y;
    a.path_cb        = lv_anim_path_linear;
    a.ready_cb       = NULL;
    a.act_time       = 0;
    a.time           = LV_PAGE_SCROLL_ANIM_TIME;
    a.playback       = 0;
    a.playback_pause = 0;
    a.repeat         = 0;
    a.repeat_pause   = 0;
    lv_anim_create(&a);
#else
    lv_obj_set_y(scrl, lv_obj_get_y(scrl) + dist);
#endif
}

/**
 * Not intended to use directly by the user but by other object types internally.
 * Start an edge flash animation. Exactly one `ext->edge_flash.xxx_ip` should be set
 * @param page
 */
void lv_page_start_edge_flash(lv_obj_t * page)
{
#if LV_USE_ANIMATION
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    if(ext->edge_flash.enabled) {
        lv_anim_t a;
        a.var            = page;
        a.start          = 0;
        a.end            = LV_PAGE_END_FLASH_SIZE;
        a.exec_cb        = (lv_anim_exec_xcb_t)edge_flash_anim;
        a.path_cb        = lv_anim_path_linear;
        a.ready_cb       = edge_flash_anim_end;
        a.act_time       = 0;
        a.time           = LV_PAGE_END_ANIM_TIME;
        a.playback       = 1;
        a.playback_pause = LV_PAGE_END_ANIM_WAIT_TIME;
        a.repeat         = 0;
        a.repeat_pause   = 0;
        lv_anim_create(&a);
    }
#else
    (void)page; /*Unused*/
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
    }
    /*Cache page bg style for temporary modification*/
    const lv_style_t * style = lv_page_get_style(page, LV_PAGE_STYLE_BG);
    lv_style_t style_tmp;
    lv_style_copy(&style_tmp, style);

    if(mode == LV_DESIGN_DRAW_MAIN) {
        /*Draw without border*/
        style_tmp.body.border.width = 0;
        lv_draw_rect(&page->coords, mask, &style_tmp, lv_obj_get_opa_scale(page));

    } else if(mode == LV_DESIGN_DRAW_POST) {
        /*Draw only a border*/
        style_tmp.body.shadow.width = 0;
        style_tmp.body.opa          = LV_OPA_TRANSP;
        lv_draw_rect(&page->coords, mask, &style_tmp, lv_obj_get_opa_scale(page));

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

#if LV_USE_ANIMATION
        {
            lv_coord_t page_w = lv_obj_get_width(page);
            lv_coord_t page_h = lv_obj_get_height(page);

            lv_area_t flash_area;

            if(ext->edge_flash.top_ip) {
                flash_area.x1 = page->coords.x1 - page_w;
                flash_area.x2 = page->coords.x2 + page_w;
                flash_area.y1 = page->coords.y1 - 3 * page_w + ext->edge_flash.state;
                flash_area.y2 = page->coords.y1 + ext->edge_flash.state;
            } else if(ext->edge_flash.bottom_ip) {
                flash_area.x1 = page->coords.x1 - page_w;
                flash_area.x2 = page->coords.x2 + page_w;
                flash_area.y1 = page->coords.y2 - ext->edge_flash.state;
                flash_area.y2 = page->coords.y2 + 3 * page_w - ext->edge_flash.state;
            } else if(ext->edge_flash.right_ip) {
                flash_area.x1 = page->coords.x2 - ext->edge_flash.state;
                flash_area.x2 = page->coords.x2 + 3 * page_h - ext->edge_flash.state;
                flash_area.y1 = page->coords.y1 - page_h;
                flash_area.y2 = page->coords.y2 + page_h;
            } else if(ext->edge_flash.left_ip) {
                flash_area.x1 = page->coords.x1 - 3 * page_h + ext->edge_flash.state;
                flash_area.x2 = page->coords.x1 + ext->edge_flash.state;
                flash_area.y1 = page->coords.y1 - page_h;
                flash_area.y2 = page->coords.y2 + page_h;
            }

            if(ext->edge_flash.left_ip || ext->edge_flash.right_ip || ext->edge_flash.top_ip ||
               ext->edge_flash.bottom_ip) {
                lv_style_t flash_style;
                lv_style_copy(&flash_style, ext->edge_flash.style);
                flash_style.body.radius = LV_RADIUS_CIRCLE;
                uint32_t opa            = (flash_style.body.opa * ext->edge_flash.state) / LV_PAGE_END_FLASH_SIZE;
                flash_style.body.opa    = opa;
                lv_draw_rect(&flash_area, mask, &flash_style, lv_obj_get_opa_scale(page));
            }
        }
#endif
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
#if LV_USE_GROUP
        /* If the page is focused in a group and
         * the background object is not visible (transparent)
         * then "activate" the style of the scrollable*/
        const lv_style_t * style_scrl_ori = lv_obj_get_style(scrl);
        lv_obj_t * page                   = lv_obj_get_parent(scrl);
        const lv_style_t * style_page     = lv_obj_get_style(page);
        lv_group_t * g                    = lv_obj_get_group(page);
        if((style_page->body.opa == LV_OPA_TRANSP) &&
           style_page->body.border.width == 0) { /*Is the background visible?*/
            if(lv_group_get_focused(g) == page) {
                lv_style_t * style_mod;
                style_mod = lv_group_mod_style(g, style_scrl_ori);
                /*If still not visible modify the style a littel bit*/
                if((style_mod->body.opa == LV_OPA_TRANSP) && style_mod->body.border.width == 0) {
                    style_mod->body.opa          = LV_OPA_50;
                    style_mod->body.border.width = 1;
                    style_mod                    = lv_group_mod_style(g, style_mod);
                }

                scrl->style_p = style_mod; /*Temporally change the style to the activated */
            }
        }
#endif
        ancestor_design(scrl, mask, mode);

#if LV_USE_GROUP
        scrl->style_p = style_scrl_ori; /*Revert the style*/
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
    lv_obj_t * child;
    if(sign == LV_SIGNAL_CHILD_CHG) { /*Automatically move children to the scrollable object*/
        const lv_style_t * style = lv_page_get_style(page, LV_PAGE_STYLE_SCRL);
        lv_fit_t fit_left        = lv_page_get_scrl_fit_left(page);
        lv_fit_t fit_top         = lv_page_get_scrl_fit_top(page);
        child                    = lv_obj_get_child(page, NULL);
        while(child != NULL) {
            if(lv_obj_is_protected(child, LV_PROTECT_PARENT) == false) {
                lv_obj_t * tmp = child;
                child          = lv_obj_get_child(page, child); /*Get the next child before move this*/

                /* Reposition the child to take padding into account (Only if it's on (0;0) now)
                 * It's required to keep new the object on the same coordinate if FIT is enabled.*/
                if((tmp->coords.x1 == page->coords.x1) && (fit_left == LV_FIT_TIGHT || fit_left == LV_FIT_FILL)) {
                    tmp->coords.x1 += style->body.padding.left;
                    tmp->coords.x2 += style->body.padding.left;
                }
                if((tmp->coords.y1 == page->coords.y1) && (fit_top == LV_FIT_TIGHT || fit_top == LV_FIT_FILL)) {
                    tmp->coords.y1 += style->body.padding.top;
                    tmp->coords.y2 += style->body.padding.top;
                }
                lv_obj_set_parent(tmp, ext->scrl);
            } else {
                child = lv_obj_get_child(page, child);
            }
        }
    } else if(sign == LV_SIGNAL_STYLE_CHG) {
        ext->scrl->signal_cb(ext->scrl, LV_SIGNAL_CORD_CHG, &ext->scrl->coords);

        /*The scrollbars are important only if they are visible now*/
        if(ext->sb.hor_draw || ext->sb.ver_draw) lv_page_sb_refresh(page);

        /*Refresh the ext. size because the scrollbars might be positioned out of the page*/
        lv_obj_refresh_ext_draw_pad(page);
    } else if(sign == LV_SIGNAL_CORD_CHG) {
        /*Refresh the scrollbar and notify the scrl if the size is changed*/
        if(ext->scrl != NULL && (lv_obj_get_width(page) != lv_area_get_width(param) ||
                                 lv_obj_get_height(page) != lv_area_get_height(param))) {
            /*If no hor_fit enabled set the scrollable's width to the page's width*/
            ext->scrl->signal_cb(ext->scrl, LV_SIGNAL_CORD_CHG, &ext->scrl->coords);

            /*The scrollbars are important only if they are visible now*/
            if(ext->sb.hor_draw || ext->sb.ver_draw) lv_page_sb_refresh(page);
        }
    } else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        /*Ensure ext. size for the scrollbars if they are out of the page*/
        if(page->ext_draw_pad < (-ext->sb.style->body.padding.right))
            page->ext_draw_pad = -ext->sb.style->body.padding.right;
        if(page->ext_draw_pad < (-ext->sb.style->body.padding.bottom))
            page->ext_draw_pad = -ext->sb.style->body.padding.bottom;
    } else if(sign == LV_SIGNAL_CONTROL) {
        uint32_t c = *((uint32_t *)param);

        if(c == LV_KEY_DOWN) {
            lv_page_scroll_ver(page, -lv_obj_get_height(page) / 4);
        } else if(c == LV_KEY_UP) {
            lv_page_scroll_ver(page, lv_obj_get_height(page) / 4);
        } else if(c == LV_KEY_RIGHT) {
            /*If the page can't be scrolled horizontally because it's not wide enough then scroll it
             * vertically*/
            if(lv_page_get_scrl_width(page) <= lv_obj_get_width(page))
                lv_page_scroll_ver(page, -lv_obj_get_height(page) / 4);
            else
                lv_page_scroll_hor(page, -lv_obj_get_width(page) / 4);
        } else if(c == LV_KEY_LEFT) {
            /*If the page can't be scrolled horizontally because it's not wide enough then scroll it
             * vertically*/
            if(lv_page_get_scrl_width(page) <= lv_obj_get_width(page))
                lv_page_scroll_ver(page, lv_obj_get_height(page) / 4);
            else
                lv_page_scroll_hor(page, lv_obj_get_width(page) / 4);
        }
    } else if(sign == LV_SIGNAL_GET_EDITABLE) {
        bool * editable = (bool *)param;
        *editable       = true;
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
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

    lv_obj_t * page               = lv_obj_get_parent(scrl);
    const lv_style_t * page_style = lv_obj_get_style(page);
    lv_page_ext_t * page_ext      = lv_obj_get_ext_attr(page);

    if(sign == LV_SIGNAL_CORD_CHG) {
        /*Limit the position of the scrollable object to be always visible
         * (Do not let its edge inner then its parent respective edge)*/
        lv_coord_t new_x = lv_obj_get_x(scrl);
        lv_coord_t new_y = lv_obj_get_y(scrl);
        bool refr_x      = false;
        bool refr_y      = false;
        lv_area_t page_coords;
        lv_area_t scrl_coords;
        lv_obj_get_coords(scrl, &scrl_coords);
        lv_obj_get_coords(page, &page_coords);

        lv_area_t * ori_coords = (lv_area_t *)param;
        lv_coord_t diff_x      = scrl->coords.x1 - ori_coords->x1;
        lv_coord_t diff_y      = scrl->coords.y1 - ori_coords->y1;
        lv_coord_t hpad        = page_style->body.padding.left + page_style->body.padding.right;
        lv_coord_t vpad        = page_style->body.padding.top + page_style->body.padding.bottom;
        lv_obj_t * page_parent = lv_obj_get_parent(page);

        lv_indev_t * indev = lv_indev_get_act();
        lv_point_t drag_vect;
        lv_indev_get_vect(indev, &drag_vect);

        /* Start the scroll propagation if there is drag vector on the indev, but the drag is not
         * started yet and the scrollable is in a corner. It will enable the scroll propagation only
         * when a new scroll begins and not when the scrollable is already being scrolled.*/
        if(page_ext->scroll_prop && page_ext->scroll_prop_ip == 0 && lv_indev_is_dragging(indev) == false) {
            if(((drag_vect.y > 0 && scrl_coords.y1 == page_coords.y1 + page_style->body.padding.top) ||
                (drag_vect.y < 0 && scrl_coords.y2 == page_coords.y2 - page_style->body.padding.bottom)) &&
               ((drag_vect.x > 0 && scrl_coords.x1 == page_coords.x1 + page_style->body.padding.left) ||
                (drag_vect.x < 0 && scrl_coords.x2 == page_coords.x2 - page_style->body.padding.right))) {

                if(lv_obj_get_parent(page_parent) != NULL) { /*Do not propagate the scroll to a screen*/
                    page_ext->scroll_prop_ip = 1;
                }
            }
        }

        /*scrollable width smaller then page width? -> align to left*/
        if(lv_area_get_width(&scrl_coords) + hpad <= lv_area_get_width(&page_coords)) {
            if(scrl_coords.x1 != page_coords.x1 + page_style->body.padding.left) {
                new_x  = page_style->body.padding.left;
                refr_x = true;
            }
        } else {
            /*If the scroll propagation is in progress revert the original coordinates (don't let
             * the page scroll)*/
            if(page_ext->scroll_prop_ip) {
                if(drag_vect.x == diff_x) { /*`scrl` is bouncing: drag pos. it somewhere and here it
                                               is reverted. Handle only the pos. because of drag*/
                    new_x  = ori_coords->x1 - page_coords.x1;
                    refr_x = true;
                }
            }
            /*The edges of the scrollable can not be in the page (minus hpad) */
            else if(scrl_coords.x2 < page_coords.x2 - page_style->body.padding.right) {
                new_x = lv_area_get_width(&page_coords) - lv_area_get_width(&scrl_coords) -
                        page_style->body.padding.right; /* Right align */
                refr_x = true;
#if LV_USE_ANIMATION
                if(page_ext->edge_flash.enabled && page_ext->edge_flash.left_ip == 0 &&
                   page_ext->edge_flash.right_ip == 0 && page_ext->edge_flash.top_ip == 0 &&
                   page_ext->edge_flash.bottom_ip == 0) {
                    lv_page_start_edge_flash(page);
                    page_ext->edge_flash.right_ip = 1;
                }
#endif
            } else if(scrl_coords.x1 > page_coords.x1 + page_style->body.padding.left) {
                new_x  = page_style->body.padding.left; /*Left align*/
                refr_x = true;
#if LV_USE_ANIMATION
                if(page_ext->edge_flash.enabled && page_ext->edge_flash.left_ip == 0 &&
                   page_ext->edge_flash.right_ip == 0 && page_ext->edge_flash.top_ip == 0 &&
                   page_ext->edge_flash.bottom_ip == 0) {
                    lv_page_start_edge_flash(page);
                    page_ext->edge_flash.left_ip = 1;
                }
#endif
            }
        }

        /*scrollable height smaller then page height? -> align to top*/
        if(lv_area_get_height(&scrl_coords) + vpad <= lv_area_get_height(&page_coords)) {
            if(scrl_coords.y1 != page_coords.y1 + page_style->body.padding.top) {
                new_y  = page_style->body.padding.top;
                refr_y = true;
            }
        } else {
            /*If the scroll propagation is in progress revert the original coordinates (don't let
             * the page scroll)*/
            if(page_ext->scroll_prop_ip) {
                if(drag_vect.y == diff_y) { /*`scrl` is bouncing: drag pos. it somewhere and here it
                                               is reverted. Handle only the pos. because of drag*/
                    new_y  = ori_coords->y1 - page_coords.y1;
                    refr_y = true;
                }
            }
            /*The edges of the scrollable can not be in the page (minus vpad) */
            else if(scrl_coords.y2 < page_coords.y2 - page_style->body.padding.bottom) {
                new_y = lv_area_get_height(&page_coords) - lv_area_get_height(&scrl_coords) -
                        page_style->body.padding.bottom; /* Bottom align */
                refr_y = true;
#if LV_USE_ANIMATION
                if(page_ext->edge_flash.enabled && page_ext->edge_flash.left_ip == 0 &&
                   page_ext->edge_flash.right_ip == 0 && page_ext->edge_flash.top_ip == 0 &&
                   page_ext->edge_flash.bottom_ip == 0) {
                    lv_page_start_edge_flash(page);
                    page_ext->edge_flash.bottom_ip = 1;
                }
#endif
            } else if(scrl_coords.y1 > page_coords.y1 + page_style->body.padding.top) {
                new_y  = page_style->body.padding.top; /*Top align*/
                refr_y = true;
#if LV_USE_ANIMATION
                if(page_ext->edge_flash.enabled && page_ext->edge_flash.left_ip == 0 &&
                   page_ext->edge_flash.right_ip == 0 && page_ext->edge_flash.top_ip == 0 &&
                   page_ext->edge_flash.bottom_ip == 0) {
                    lv_page_start_edge_flash(page);
                    page_ext->edge_flash.top_ip = 1;
                }
#endif
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
    } else if(sign == LV_SIGNAL_DRAG_END) {

        /*Scroll propagation is finished on drag end*/
        page_ext->scroll_prop_ip = 0;

        /*Hide scrollbars if required*/
        if(page_ext->sb.mode == LV_SB_MODE_DRAG) {
            lv_disp_t * disp = lv_obj_get_disp(page);
            lv_area_t sb_area_tmp;
            if(page_ext->sb.hor_draw) {
                lv_area_copy(&sb_area_tmp, &page_ext->sb.hor_area);
                sb_area_tmp.x1 += page->coords.x1;
                sb_area_tmp.y1 += page->coords.y1;
                sb_area_tmp.x2 += page->coords.x1;
                sb_area_tmp.y2 += page->coords.y1;
                lv_inv_area(disp, &sb_area_tmp);
                page_ext->sb.hor_draw = 0;
            }
            if(page_ext->sb.ver_draw) {
                lv_area_copy(&sb_area_tmp, &page_ext->sb.ver_area);
                sb_area_tmp.x1 += page->coords.x1;
                sb_area_tmp.y1 += page->coords.y1;
                sb_area_tmp.x2 += page->coords.x1;
                sb_area_tmp.y2 += page->coords.y1;
                lv_inv_area(disp, &sb_area_tmp);
                page_ext->sb.ver_draw = 0;
            }
        }
    }

    return res;
}

/**
 * Propagate the input device related event of the scrollable to the parent page background
 * It is used by default if the scrollable's event is not specified
 * @param scrl pointer to the page's scrollable object
 * @param event type of the event
 * @param data data of the event
 */
static void scrl_def_event_cb(lv_obj_t * scrl, lv_event_t event)
{
    lv_obj_t * page = lv_obj_get_parent(scrl);

    /*clang-format off*/
    if(event == LV_EVENT_PRESSED || event == LV_EVENT_PRESSING || event == LV_EVENT_PRESS_LOST ||
       event == LV_EVENT_RELEASED || event == LV_EVENT_SHORT_CLICKED || event == LV_EVENT_CLICKED ||
       event == LV_EVENT_LONG_PRESSED || event == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_event_send(page, event, lv_event_get_data());
    }
    /*clang-format on*/
}

/**
 * Refresh the position and size of the scroll bars.
 * @param page pointer to a page object
 */
static void lv_page_sb_refresh(lv_obj_t * page)
{
    lv_page_ext_t * ext      = lv_obj_get_ext_attr(page);
    const lv_style_t * style = lv_obj_get_style(page);
    lv_obj_t * scrl          = ext->scrl;
    lv_coord_t size_tmp;
    lv_coord_t scrl_w = lv_obj_get_width(scrl);
    lv_coord_t scrl_h = lv_obj_get_height(scrl);
    lv_coord_t obj_w  = lv_obj_get_width(page);
    lv_coord_t obj_h  = lv_obj_get_height(page);

    /*Always let 'scrollbar width' padding above, under, left and right to the scrollbars
     * else:
     * - horizontal and vertical scrollbars can overlap on the corners
     * - if the page has radius the scrollbar can be out of the radius  */
    lv_coord_t sb_hor_pad = LV_MATH_MAX(ext->sb.style->body.padding.inner, style->body.padding.right);
    lv_coord_t sb_ver_pad = LV_MATH_MAX(ext->sb.style->body.padding.inner, style->body.padding.bottom);

    if(ext->sb.mode == LV_SB_MODE_OFF) return;

    if(ext->sb.mode == LV_SB_MODE_ON) {
        ext->sb.hor_draw = 1;
        ext->sb.ver_draw = 1;
    }

    /*Invalidate the current (old) scrollbar areas*/
    lv_disp_t * disp = lv_obj_get_disp(page);
    lv_area_t sb_area_tmp;
    if(ext->sb.hor_draw != 0) {
        lv_area_copy(&sb_area_tmp, &ext->sb.hor_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        lv_inv_area(disp, &sb_area_tmp);
    }
    if(ext->sb.ver_draw != 0) {
        lv_area_copy(&sb_area_tmp, &ext->sb.ver_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        lv_inv_area(disp, &sb_area_tmp);
    }

    if(ext->sb.mode == LV_SB_MODE_DRAG && lv_indev_is_dragging(lv_indev_get_act()) == false) {
        ext->sb.hor_draw = 0;
        ext->sb.ver_draw = 0;
        return;
    }

    /*Full sized horizontal scrollbar*/
    if(scrl_w <= obj_w - style->body.padding.left - style->body.padding.right) {
        lv_area_set_width(&ext->sb.hor_area, obj_w - 2 * sb_hor_pad);
        lv_area_set_pos(&ext->sb.hor_area, sb_hor_pad,
                        obj_h - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.bottom);
        if(ext->sb.mode == LV_SB_MODE_AUTO || ext->sb.mode == LV_SB_MODE_DRAG) ext->sb.hor_draw = 0;
    }
    /*Smaller horizontal scrollbar*/
    else {
        size_tmp =
            (obj_w * (obj_w - (2 * sb_hor_pad))) / (scrl_w + style->body.padding.left + style->body.padding.right);
        if(size_tmp < LV_PAGE_SB_MIN_SIZE) size_tmp = LV_PAGE_SB_MIN_SIZE;
        lv_area_set_width(&ext->sb.hor_area, size_tmp);

        lv_area_set_pos(&ext->sb.hor_area,
                        sb_hor_pad +
                            (-(lv_obj_get_x(scrl) - style->body.padding.left) * (obj_w - size_tmp - 2 * sb_hor_pad)) /
                                (scrl_w + style->body.padding.left + style->body.padding.right - obj_w),
                        obj_h - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.bottom);

        if(ext->sb.mode == LV_SB_MODE_AUTO || ext->sb.mode == LV_SB_MODE_DRAG) ext->sb.hor_draw = 1;
    }

    /*Full sized vertical scroll bar*/
    if(scrl_h <= obj_h - style->body.padding.top - style->body.padding.bottom) {
        lv_area_set_height(&ext->sb.ver_area, obj_h - 2 * sb_ver_pad);
        lv_area_set_pos(&ext->sb.ver_area,
                        obj_w - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.right, sb_ver_pad);
        if(ext->sb.mode == LV_SB_MODE_AUTO || ext->sb.mode == LV_SB_MODE_DRAG) ext->sb.ver_draw = 0;
    }
    /*Smaller vertical scroll bar*/
    else {
        size_tmp =
            (obj_h * (obj_h - (2 * sb_ver_pad))) / (scrl_h + style->body.padding.top + style->body.padding.bottom);
        if(size_tmp < LV_PAGE_SB_MIN_SIZE) size_tmp = LV_PAGE_SB_MIN_SIZE;
        lv_area_set_height(&ext->sb.ver_area, size_tmp);

        lv_area_set_pos(&ext->sb.ver_area,
                        obj_w - ext->sb.style->body.padding.inner - ext->sb.style->body.padding.right,
                        sb_ver_pad + (-(lv_obj_get_y(scrl) - ext->sb.style->body.padding.bottom) *
                                      (obj_h - size_tmp - 2 * sb_ver_pad)) /
                                         (scrl_h + style->body.padding.top + style->body.padding.bottom - obj_h));

        if(ext->sb.mode == LV_SB_MODE_AUTO || ext->sb.mode == LV_SB_MODE_DRAG) ext->sb.ver_draw = 1;
    }

    /*Invalidate the new scrollbar areas*/
    if(ext->sb.hor_draw != 0) {
        lv_area_copy(&sb_area_tmp, &ext->sb.hor_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        lv_inv_area(disp, &sb_area_tmp);
    }
    if(ext->sb.ver_draw != 0) {
        lv_area_copy(&sb_area_tmp, &ext->sb.ver_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        lv_inv_area(disp, &sb_area_tmp);
    }
}

#if LV_USE_ANIMATION
static void edge_flash_anim(void * page, lv_anim_value_t v)
{
    lv_page_ext_t * ext   = lv_obj_get_ext_attr(page);
    ext->edge_flash.state = v;
    lv_obj_invalidate(page);
}

static void edge_flash_anim_end(lv_anim_t * a)
{
    lv_page_ext_t * ext       = lv_obj_get_ext_attr(a->var);
    ext->edge_flash.top_ip    = 0;
    ext->edge_flash.bottom_ip = 0;
    ext->edge_flash.left_ip   = 0;
    ext->edge_flash.right_ip  = 0;
    lv_obj_invalidate(a->var);
}
#endif

#endif
