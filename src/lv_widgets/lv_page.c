/**
 * @file lv_page.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_widgets/lv_page.h"
#if LV_USE_PAGE != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_core/lv_group.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_core/lv_refr.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_page"

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
static void scrlbar_refresh(lv_obj_t * page);
static void scrl_reposition(lv_obj_t * page);
static lv_design_res_t lv_page_design(lv_obj_t * page, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_page_signal(lv_obj_t * page, lv_signal_t sign, void * param);
static lv_style_list_t * lv_page_get_style(lv_obj_t * page, uint8_t part);
static lv_res_t lv_page_scrollable_signal(lv_obj_t * scrl, lv_signal_t sign, void * param);
static void scrl_def_event_cb(lv_obj_t * scrl, lv_event_t event);
static void refr_ext_draw_pad(lv_obj_t * page);
#if LV_USE_ANIMATION
    static void edge_flash_anim(void * page, lv_anim_value_t v);
    static void edge_flash_anim_end(lv_anim_t * a);
    static void get_edge_flash_area(lv_obj_t * page, lv_area_t * area, lv_coord_t state);
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
    lv_obj_t * page = lv_cont_create(par, copy);
    LV_ASSERT_MEM(page);
    if(page == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(page);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(page);

    /*Allocate the object type specific extended data*/
    lv_page_ext_t * ext = lv_obj_allocate_ext_attr(page, sizeof(lv_page_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(page);
        return NULL;
    }

    ext->scrl        = NULL;
    lv_style_list_init(&ext->scrlbar.style);
    ext->scrlbar.hor_draw = 0;
    ext->scrlbar.ver_draw = 0;
    ext->scrlbar.mode     = LV_SCROLLBAR_MODE_AUTO;
#if LV_USE_ANIMATION
    lv_style_list_init(&ext->edge_flash.style);
    ext->edge_flash.enabled   = 0;
    ext->edge_flash.bottom_ip = 0;
    ext->edge_flash.top_ip    = 0;
    ext->edge_flash.left_ip   = 0;
    ext->edge_flash.right_ip  = 0;
    ext->edge_flash.state     = 0;
    ext->anim_time            = LV_PAGE_DEF_ANIM_TIME;
#endif
    ext->scroll_prop    = 0;
    ext->scroll_prop_obj = NULL;

    /*Init the new page object*/
    if(copy == NULL) {
        ext->scrl = lv_cont_create(page, NULL);
        lv_obj_set_focus_parent(ext->scrl, true);
        lv_obj_set_drag(ext->scrl, true);
        lv_obj_set_drag_throw(ext->scrl, true);
        lv_obj_add_protect(ext->scrl, LV_PROTECT_PARENT | LV_PROTECT_PRESS_LOST);
        lv_cont_set_fit(ext->scrl, LV_FIT_MAX);
        lv_obj_set_event_cb(ext->scrl, scrl_def_event_cb); /*Propagate some event to the background
                                                              object by default for convenience */
        lv_obj_set_signal_cb(ext->scrl, lv_page_scrollable_signal);

        /* Add the signal function only if 'scrolling' is created
        +         * because everything has to be ready before any signal is received*/
        lv_obj_set_signal_cb(page, lv_page_signal);
        lv_obj_set_design_cb(page, lv_page_design);

        lv_page_set_scrollbar_mode(page, ext->scrlbar.mode);

        lv_theme_apply(page, LV_THEME_PAGE);

    }
    else {
        lv_page_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->scrl                = lv_cont_create(page, copy_ext->scrl);
        lv_obj_set_signal_cb(ext->scrl, lv_page_scrollable_signal);

        lv_style_list_copy(&ext->scrlbar.style, &copy_ext->scrlbar.style);
#if LV_USE_ANIMATION
        lv_style_list_copy(&ext->edge_flash.style, &copy_ext->edge_flash.style);
#endif

        /* Add the signal function only if 'scrolling' is created
         * because everything has to be ready before any signal is received*/
        lv_obj_set_signal_cb(page, lv_page_signal);
        lv_obj_set_design_cb(page, lv_page_design);

        lv_page_set_scrollbar_mode(page, copy_ext->scrlbar.mode);
    }


    scrlbar_refresh(page);

    LV_LOG_INFO("page created");

    return page;
}

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param page pointer to an object
 */
void lv_page_clean(lv_obj_t * page)
{
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

    lv_obj_t * scrl = lv_page_get_scrollable(page);
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
void lv_page_set_scrollbar_mode(lv_obj_t * page, lv_scrollbar_mode_t sb_mode)
{
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    if(ext->scrlbar.mode == sb_mode) return;

    if(sb_mode == LV_SCROLLBAR_MODE_HIDE)
        ext->scrlbar.mode |= LV_SCROLLBAR_MODE_HIDE; /*Set the hidden flag*/
    else if(sb_mode == LV_SCROLLBAR_MODE_UNHIDE)
        ext->scrlbar.mode &= (~LV_SCROLLBAR_MODE_HIDE); /*Clear the hidden flag*/
    else {
        if(ext->scrlbar.mode & LV_SCROLLBAR_MODE_HIDE) sb_mode |= LV_SCROLLBAR_MODE_HIDE;
        ext->scrlbar.mode = sb_mode;
    }

    ext->scrlbar.hor_draw = 0;
    ext->scrlbar.ver_draw = 0;

    scrlbar_refresh(page);
    lv_obj_invalidate(page);
}

/**
 * Set the animation time for the page
 * @param page pointer to a page object
 * @param anim_time animation time in milliseconds
 */
void lv_page_set_anim_time(lv_obj_t * page, uint16_t anim_time)
{
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

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
 * The page needs to have a page-like parent (e.g. `lv_page`, `lv_tabview` tab, `lv_win` content area etc)
 * If enabled drag direction will be changed `LV_DRAG_DIR_ONE` automatically to allow scrolling only in one direction at one time.
 * @param page pointer to a Page
 * @param en true or false to enable/disable scroll propagation
 */
void lv_page_set_scroll_propagation(lv_obj_t * page, bool en)
{
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    if(en) lv_obj_set_drag_dir(ext->scrl, LV_DRAG_DIR_ONE);
    else lv_obj_set_drag_dir(ext->scrl, LV_DRAG_DIR_BOTH);

    ext->scroll_prop    = en ? 1 : 0;
}

/**
 * Enable the edge flash effect. (Show an arc when the an edge is reached)
 * @param page pointer to a Page
 * @param en true or false to enable/disable end flash
 */
void lv_page_set_edge_flash(lv_obj_t * page, bool en)
{
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

#if LV_USE_ANIMATION
    lv_page_ext_t * ext     = lv_obj_get_ext_attr(page);
    ext->edge_flash.enabled = en ? 1 : 0;
#else
    (void)page;
    (void)en;
#endif
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the scrollable object of a page
 * @param page pointer to a page object
 * @return pointer to a container which is the scrollable part of the page
 */
lv_obj_t * lv_page_get_scrollable(const lv_obj_t * page)
{
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

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
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

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
lv_scrollbar_mode_t lv_page_get_scrollbar_mode(const lv_obj_t * page)
{
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    return ext->scrlbar.mode;
}

/**
 * Get the scroll propagation property
 * @param page pointer to a Page
 * @return true or false
 */
bool lv_page_get_scroll_propagation(lv_obj_t * page)
{
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    return ext->scroll_prop ? true : false;
}

/**
 * Get the edge flash effect property.
 * @param page pointer to a Page
 * return true or false
 */
bool lv_page_get_edge_flash(lv_obj_t * page)
{
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

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
lv_coord_t lv_page_get_width_fit(lv_obj_t * page)
{
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    lv_style_int_t bg_left = lv_obj_get_style_pad_left(page, LV_PAGE_PART_BG);
    lv_style_int_t bg_right = lv_obj_get_style_pad_right(page, LV_PAGE_PART_BG);
    lv_style_int_t scrl_left = lv_obj_get_style_pad_left(ext->scrl, LV_CONT_PART_MAIN);
    lv_style_int_t scrl_right = lv_obj_get_style_pad_right(ext->scrl, LV_CONT_PART_MAIN);

    return lv_obj_get_width(page) - bg_left - bg_right - scrl_left - scrl_right;
}

/**
 * Get that height which can be set to the children to still not cause overflow (show scrollbars)
 * @param page pointer to a page object
 * @return the height which still fits into the page
 */
lv_coord_t lv_page_get_height_fit(lv_obj_t * page)
{
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);

    lv_style_int_t bg_top = lv_obj_get_style_pad_top(page, LV_PAGE_PART_BG);
    lv_style_int_t bg_bottom = lv_obj_get_style_pad_bottom(page, LV_PAGE_PART_BG);
    lv_style_int_t scrl_top = lv_obj_get_style_pad_top(ext->scrl, LV_CONT_PART_MAIN);
    lv_style_int_t scrl_bottom = lv_obj_get_style_pad_bottom(ext->scrl, LV_CONT_PART_MAIN);

    return lv_obj_get_height(page) - bg_top - bg_bottom - scrl_top - scrl_bottom;
}

/**
 * Divide the width of the object and get the width of a given number of columns.
 * Take into account the paddings of the background and scrollable too.
 * @param page pointer to an object
 * @param div indicates how many columns are assumed.
 * If 1 the width will be set the the parent's width
 * If 2 only half parent width - inner padding of the parent
 * If 3 only third parent width - 2 * inner padding of the parent
 * @param span how many columns are combined
 * @return the width according to the given parameters
 */
lv_coord_t lv_page_get_width_grid(lv_obj_t * page, uint8_t div, uint8_t span)
{

    lv_coord_t obj_w = lv_page_get_width_fit(page);
    lv_style_int_t pinner = lv_obj_get_style_pad_inner(page, LV_PAGE_PART_SCROLLABLE);

    lv_coord_t r = (obj_w - (div - 1) * pinner) / div;

    r = r * span + (span - 1) * pinner;
    return r;
}

/**
 * Divide the height of the object and get the height of a given number of rows.
 * Take into account the paddings of the background and scrollable too.
 * @param obj pointer to an object
 * @param div indicates how many rows are assumed.
 * If 1 the height will be set the the parent's height
 * If 2 only half parent height - inner padding of the parent
 * If 3 only third parent height - 2 * inner padding of the parent
 * @param span how many rows are combined
 * @return the height according to the given parameters
 */
lv_coord_t lv_page_get_height_grid(lv_obj_t * page, uint8_t div, uint8_t span)
{
    lv_coord_t obj_h = lv_page_get_height_fit(page);
    lv_style_int_t pinner = lv_obj_get_style_pad_inner(page, LV_PAGE_PART_SCROLLABLE);

    lv_coord_t r = (obj_h - (div - 1) * pinner) / div;

    r = r * span + (span - 1) * pinner;
    return r;
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
    lv_obj_t * scrl               = lv_page_get_scrollable(page);
    lv_area_t page_coords;
    lv_area_t scrl_coords;

    lv_obj_get_coords(scrl, &scrl_coords);
    lv_obj_get_coords(page, &page_coords);

    lv_style_int_t left = lv_obj_get_style_pad_left(page, LV_PAGE_PART_BG);
    lv_style_int_t right = lv_obj_get_style_pad_right(page, LV_PAGE_PART_BG);
    lv_style_int_t top = lv_obj_get_style_pad_top(page, LV_PAGE_PART_BG);
    lv_style_int_t bottom = lv_obj_get_style_pad_bottom(page, LV_PAGE_PART_BG);

    if((edge & LV_PAGE_EDGE_TOP) && scrl_coords.y1 == page_coords.y1 + top) return true;
    if((edge & LV_PAGE_EDGE_BOTTOM) && scrl_coords.y2 == page_coords.y2 - bottom) return true;
    if((edge & LV_PAGE_EDGE_LEFT) && scrl_coords.x1 == page_coords.x1 + left) return true;
    if((edge & LV_PAGE_EDGE_RIGHT) && scrl_coords.x2 == page_coords.x2 - right) return true;

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
     * because it can override the current changes*/
    lv_anim_del(page, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_del(page, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_del(ext->scrl, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_del(ext->scrl, (lv_anim_exec_xcb_t)lv_obj_set_y);
#endif

    /*if using focus mode, change target to parent*/
    obj = lv_obj_get_focused_obj(obj);


    /*If obj is higher then the page focus where the "error" is smaller*/
    lv_coord_t obj_y      = obj->coords.y1 - ext->scrl->coords.y1;
    lv_coord_t obj_h      = lv_obj_get_height(obj);
    lv_coord_t scrlable_y = lv_obj_get_y(ext->scrl);
    lv_coord_t page_h     = lv_obj_get_height(page);

    lv_style_int_t bg_top = lv_obj_get_style_pad_top(page, LV_PAGE_PART_BG);
    lv_style_int_t bg_bottom = lv_obj_get_style_pad_bottom(page, LV_PAGE_PART_BG);
    lv_style_int_t scrl_top = lv_obj_get_style_pad_top(ext->scrl, LV_CONT_PART_MAIN);
    lv_style_int_t scrl_bottom = lv_obj_get_style_pad_bottom(ext->scrl, LV_CONT_PART_MAIN);

    lv_coord_t top_err = -((scrlable_y + obj_y) - bg_top);
    lv_coord_t bot_err = scrlable_y + obj_y + obj_h - (page_h - bg_bottom);

    /*Out of the page on the top*/
    if((obj_h <= page_h && top_err > 0) || (obj_h > page_h && top_err < bot_err)) {
        /*Calculate a new position and let some space above*/
        scrlable_y = -(obj_y - scrl_top - bg_top);
        scrlable_y += scrl_top;
    }
    /*Out of the page on the bottom*/
    else if((obj_h <= page_h && bot_err > 0) || (obj_h > page_h && top_err >= bot_err)) {
        /*Calculate a new position and let some space below*/
        scrlable_y = -(obj_y + scrl_bottom + bg_bottom);
        scrlable_y -= scrl_bottom;
        scrlable_y += page_h - obj_h;
    }


    /*If obj is wider then the page focus where the "error" is smaller*/
    lv_coord_t obj_x      = obj->coords.x1 - ext->scrl->coords.x1;
    lv_coord_t obj_w      = lv_obj_get_width(obj);
    lv_coord_t scrlable_x = lv_obj_get_x(ext->scrl);
    lv_coord_t page_w     = lv_obj_get_width(page);

    lv_style_int_t bg_left = lv_obj_get_style_pad_left(page, LV_PAGE_PART_BG);
    lv_style_int_t bg_right = lv_obj_get_style_pad_right(page, LV_PAGE_PART_BG);
    lv_style_int_t scrl_left = lv_obj_get_style_pad_top(ext->scrl, LV_CONT_PART_MAIN);
    lv_style_int_t scrl_right = lv_obj_get_style_pad_bottom(ext->scrl, LV_CONT_PART_MAIN);

    lv_coord_t left_err  = -((scrlable_x + obj_x) - bg_left);
    lv_coord_t right_err = scrlable_x + obj_x + obj_w - (page_w - bg_right);

    /*Out of the page on the left*/
    if((obj_w <= page_w && left_err > 0) || (obj_w > page_w && left_err < right_err)) {
        /*Calculate a new position and let some space on the side*/
        scrlable_x = -(obj_x - scrl_left - bg_left);
        scrlable_x += scrl_left;
    }
    /*Out of the page on the rigth*/
    else if((obj_w <= page_w && right_err > 0) || (obj_w > page_w && left_err >= right_err)) {
        /*Calculate a new position and let some space on the side*/
        scrlable_x = -(obj_x + scrl_right + bg_right);
        scrlable_x -= scrl_right;
        scrlable_x += page_w - obj_w;
    }

    if(anim_en == LV_ANIM_OFF || lv_page_get_anim_time(page) == 0) {
        lv_obj_set_y(ext->scrl, scrlable_y);
        lv_obj_set_x(ext->scrl, scrlable_x);
    }
    else {
#if LV_USE_ANIMATION
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, ext->scrl);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_values(&a, lv_obj_get_y(ext->scrl), scrlable_y);
        lv_anim_set_time(&a, lv_page_get_anim_time(page));
        lv_anim_start(&a);

        lv_anim_set_values(&a, lv_obj_get_x(ext->scrl), scrlable_x);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
        lv_anim_start(&a);
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
    lv_obj_t * scrl = lv_page_get_scrollable(page);

#if LV_USE_ANIMATION
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, scrl);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&a, lv_obj_get_x(scrl), lv_obj_get_x(scrl) + dist);
    lv_anim_set_time(&a, lv_page_get_anim_time(page));
    lv_anim_start(&a);
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
    lv_obj_t * scrl = lv_page_get_scrollable(page);

#if LV_USE_ANIMATION
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, scrl);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_values(&a, lv_obj_get_y(scrl), lv_obj_get_y(scrl) + dist);
    lv_anim_set_time(&a, lv_page_get_anim_time(page));
    lv_anim_start(&a);
#else
    lv_obj_set_y(scrl, lv_obj_get_y(scrl) + dist);
#endif
}

/**
 * Not intended to use directly by the user but by other object types internally.
 * Start an edge flash animation.
 * @param page
 * @param edge the edge to flash. Can be `LV_PAGE_EDGE_LEFT/RIGHT/TOP/BOTTOM`
 */
void lv_page_start_edge_flash(lv_obj_t * page, lv_page_edge_t edge)
{
#if LV_USE_ANIMATION
    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    if(ext->edge_flash.enabled == 0) return;

    if(ext->edge_flash.left_ip ||
       ext->edge_flash.right_ip ||
       ext->edge_flash.top_ip ||
       ext->edge_flash.bottom_ip) {
        return;
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, page);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)edge_flash_anim);
    lv_anim_set_values(&a, 0, LV_PAGE_END_FLASH_SIZE);
    lv_anim_set_time(&a, lv_page_get_anim_time(page));
    lv_anim_set_playback_time(&a, lv_page_get_anim_time(page));
    lv_anim_set_playback_delay(&a, LV_PAGE_END_ANIM_WAIT_TIME);
    lv_anim_set_ready_cb(&a, edge_flash_anim_end);
    lv_anim_start(&a);

    switch(edge) {
        case LV_PAGE_EDGE_BOTTOM:
            ext->edge_flash.bottom_ip = 1;
            break;
        case LV_PAGE_EDGE_TOP:
            ext->edge_flash.top_ip = 1;
            break;
        case LV_PAGE_EDGE_LEFT:
            ext->edge_flash.left_ip = 1;
            break;
        case LV_PAGE_EDGE_RIGHT:
            ext->edge_flash.right_ip = 1;
            break;
    }

#else
    LV_UNUSED(page);
    LV_UNUSED(edge);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the pages
 * @param page pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_page_design(lv_obj_t * page, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_design(page, clip_area, mode);
    }

    if(mode == LV_DESIGN_DRAW_MAIN) {
        return ancestor_design(page, clip_area, mode);
    }
    else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design(page, clip_area, mode);
        lv_page_ext_t * ext = lv_obj_get_ext_attr(page);

        lv_area_t sb_hor_area;
        lv_area_t sb_ver_area;
        /*Convert the relative coordinates to absolute*/
        lv_area_copy(&sb_hor_area, &ext->scrlbar.hor_area);
        sb_hor_area.x1 += page->coords.x1;
        sb_hor_area.y1 += page->coords.y1;
        sb_hor_area.x2 += page->coords.x1;
        sb_hor_area.y2 += page->coords.y1;

        /*Convert the relative coordinates to absolute*/
        lv_area_copy(&sb_ver_area, &ext->scrlbar.ver_area);
        sb_ver_area.x1 += page->coords.x1;
        sb_ver_area.y1 += page->coords.y1;
        sb_ver_area.x2 += page->coords.x1;
        sb_ver_area.y2 += page->coords.y1;

        if((ext->scrlbar.hor_draw && _lv_area_is_on(&sb_hor_area, clip_area)) ||
           (ext->scrlbar.ver_draw && _lv_area_is_on(&sb_ver_area, clip_area))) {
            /*Draw the scrollbars*/
            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            lv_obj_init_draw_rect_dsc(page, LV_PAGE_PART_SCROLLBAR, &rect_dsc);
            if(ext->scrlbar.hor_draw && (ext->scrlbar.mode & LV_SCROLLBAR_MODE_HIDE) == 0) {
                lv_draw_rect(&sb_hor_area, clip_area, &rect_dsc);
            }

            if(ext->scrlbar.ver_draw && (ext->scrlbar.mode & LV_SCROLLBAR_MODE_HIDE) == 0) {
                lv_draw_rect(&sb_ver_area, clip_area, &rect_dsc);
            }
        }

#if LV_USE_ANIMATION
        {


            if(ext->edge_flash.left_ip || ext->edge_flash.right_ip || ext->edge_flash.top_ip ||
               ext->edge_flash.bottom_ip) {
                lv_area_t flash_area;
                get_edge_flash_area(page, &flash_area, ext->edge_flash.state);

                lv_draw_rect_dsc_t edge_draw_dsc;
                lv_draw_rect_dsc_init(&edge_draw_dsc);
                lv_obj_init_draw_rect_dsc(page, LV_PAGE_PART_EDGE_FLASH, &edge_draw_dsc);
                edge_draw_dsc.radius  = LV_RADIUS_CIRCLE;
                uint32_t opa            = (edge_draw_dsc.bg_opa * ext->edge_flash.state) / LV_PAGE_END_FLASH_SIZE;
                edge_draw_dsc.bg_opa    = opa;
                lv_draw_rect(&flash_area, clip_area, &edge_draw_dsc);
            }
        }
#endif
    }

    return LV_DESIGN_RES_OK;
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
    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_page_get_style(page, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(page, sign, param);
    }
    else if(sign == LV_SIGNAL_GET_STATE_DSC) {
        lv_get_state_info_t * info = param;
        if(info->part == LV_PAGE_PART_SCROLLABLE) info->result = lv_obj_get_state(lv_page_get_scrollable(page),
                                                                                      LV_CONT_PART_MAIN);
        else info->result = lv_obj_get_state(page, info->part);
        return LV_RES_OK;
    }

    /* Include the ancient signal function */
    res = ancestor_signal(page, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    if(sign == LV_SIGNAL_CLEANUP) {
        /*Check whether the object being deleted is propagating scroll to the parent */
        if(ext->scroll_prop) {
            lv_obj_t * parent_page = lv_obj_get_parent(lv_obj_get_parent(page));
            lv_page_ext_t * parent_ext = lv_obj_get_ext_attr(parent_page);
            if(parent_ext->scroll_prop_obj == page) {
                parent_ext->scroll_prop_obj = NULL;
            }
        }

        lv_obj_clean_style_list(page, LV_PAGE_PART_SCROLLBAR);
#if LV_USE_ANIMATION
        lv_obj_clean_style_list(page, LV_PAGE_PART_EDGE_FLASH);
#endif
    }
    /*Automatically move children to the scrollable object*/
    else if(sign == LV_SIGNAL_CHILD_CHG) {
        lv_obj_t * child;
        if(ext->scrl == NULL) return LV_RES_OK;

        lv_fit_t fit_left        = lv_page_get_scrl_fit_left(page);
        lv_fit_t fit_right        = lv_page_get_scrl_fit_right(page);
        lv_fit_t fit_top         = lv_page_get_scrl_fit_top(page);
        lv_bidi_dir_t base_dir = lv_obj_get_base_dir(page);

        lv_style_int_t scrl_left = lv_obj_get_style_pad_left(ext->scrl, LV_CONT_PART_MAIN);
        lv_style_int_t scrl_right = lv_obj_get_style_pad_right(ext->scrl, LV_CONT_PART_MAIN);
        lv_style_int_t scrl_top = lv_obj_get_style_pad_top(ext->scrl, LV_CONT_PART_MAIN);

        child = lv_obj_get_child(page, NULL);
        while(child != NULL) {
            if(lv_obj_is_protected(child, LV_PROTECT_PARENT) == false) {
                lv_obj_t * tmp = child;
                child          = lv_obj_get_child(page, child); /*Get the next child before move this*/

                /* Reposition the child to take padding into account
                 * It's required to keep new the object on the same coordinate if FIT is enabled.*/
                if((tmp->coords.x1 == page->coords.x1)  &&
                   (fit_left == LV_FIT_TIGHT || fit_left == LV_FIT_MAX) &&
                   base_dir != LV_BIDI_DIR_RTL) {
                    tmp->coords.x1 += scrl_left;
                    tmp->coords.x2 += scrl_left;
                }
                else if((tmp->coords.x2 == page->coords.x2) &&
                        (fit_right == LV_FIT_TIGHT || fit_right == LV_FIT_MAX)
                        && base_dir == LV_BIDI_DIR_RTL) {
                    tmp->coords.x1 -= scrl_right;
                    tmp->coords.x2 -= scrl_right;
                }
                if((tmp->coords.y1 == page->coords.y1) && (fit_top == LV_FIT_TIGHT || fit_top == LV_FIT_MAX)) {
                    tmp->coords.y1 += scrl_top;
                    tmp->coords.y2 += scrl_top;
                }
                lv_obj_set_parent(tmp, ext->scrl);
            }
            else {
                child = lv_obj_get_child(page, child);
            }
        }
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_style_int_t sb_width = lv_obj_get_style_size(page, LV_PAGE_PART_SCROLLBAR);
        lv_area_set_height(&ext->scrlbar.hor_area, sb_width);
        lv_area_set_width(&ext->scrlbar.ver_area, sb_width);

        /*The scrollbars are important only if they are visible now*/
        if(ext->scrlbar.hor_draw || ext->scrlbar.ver_draw) scrlbar_refresh(page);

        /*Refresh the ext. size because the scrollbars might be positioned out of the page*/
        refr_ext_draw_pad(page);
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {
        /*Refresh the scrollbar and notify the scrl if the size is changed*/
        if(ext->scrl != NULL && (lv_obj_get_width(page) != lv_area_get_width(param) ||
                                 lv_obj_get_height(page) != lv_area_get_height(param))) {
            ext->scrl->signal_cb(ext->scrl, LV_SIGNAL_COORD_CHG, &ext->scrl->coords);

            /*The scrollbars are important only if they are visible now*/
            if(ext->scrlbar.hor_draw || ext->scrlbar.ver_draw) scrlbar_refresh(page);
        }
    }
    else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        refr_ext_draw_pad(page);
    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        uint32_t c = *((uint32_t *)param);

        if(c == LV_KEY_DOWN) {
            lv_page_scroll_ver(page, -lv_obj_get_height(page) / 4);
        }
        else if(c == LV_KEY_UP) {
            lv_page_scroll_ver(page, lv_obj_get_height(page) / 4);
        }
        else if(c == LV_KEY_RIGHT) {
            /*If the page can't be scrolled horizontally because it's not wide enough then scroll it
             * vertically*/
            if(lv_page_get_scrl_width(page) <= lv_obj_get_width(page))
                lv_page_scroll_ver(page, -lv_obj_get_height(page) / 4);
            else
                lv_page_scroll_hor(page, -lv_obj_get_width(page) / 4);
        }
        else if(c == LV_KEY_LEFT) {
            /*If the page can't be scrolled horizontally because it's not wide enough then scroll it
             * vertically*/
            if(lv_page_get_scrl_width(page) <= lv_obj_get_width(page))
                lv_page_scroll_ver(page, lv_obj_get_height(page) / 4);
            else
                lv_page_scroll_hor(page, lv_obj_get_width(page) / 4);
        }
#endif
    }
    else if(sign == LV_SIGNAL_GET_EDITABLE) {
#if LV_USE_GROUP
        bool * editable = (bool *)param;
        *editable       = true;
#endif
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

    if(sign == LV_SIGNAL_GET_STYLE) {
        return ancestor_signal(scrl, sign, param);
    }

    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(scrl, sign, param);

    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, "");

    lv_obj_t * page               = lv_obj_get_parent(scrl);
    lv_page_ext_t * page_ext      = lv_obj_get_ext_attr(page);

    if(sign == LV_SIGNAL_COORD_CHG) {
        lv_obj_t * page_parent = lv_obj_get_parent(page);

        /*Handle scroll propagation*/
        lv_indev_t * indev = lv_indev_get_act();
        if(page_ext->scroll_prop  && indev) {
            lv_point_t * drag_sum = &indev->proc.types.pointer.drag_sum;
            lv_page_ext_t * parent_ext = lv_obj_get_ext_attr(lv_obj_get_parent(page_parent));
            if(parent_ext->scroll_prop_obj == NULL) {
                /*If the dragging just started or scroll is already propagated to this object
                 *  enable the scroll propagation if the conditions are met*/
                if((lv_indev_is_dragging(indev) == false || page_ext->scroll_prop_obj) && (drag_sum->y || drag_sum->x)) {
                    /*Propagate vertically?*/
                    if((drag_sum->y > 0 && lv_page_on_edge(page, LV_PAGE_EDGE_TOP)) ||
                       (drag_sum->y < 0 && lv_page_on_edge(page, LV_PAGE_EDGE_BOTTOM))) {
                        lv_obj_set_drag_parent(page, true);
                        lv_obj_set_drag_parent(scrl, true);
                        parent_ext->scroll_prop_obj = page;
                    }
                    /*Propagate horizontally?*/
                    if((drag_sum->x > 0 && lv_page_on_edge(page, LV_PAGE_EDGE_LEFT)) ||
                       (drag_sum->x < 0 && lv_page_on_edge(page, LV_PAGE_EDGE_RIGHT))) {
                        lv_obj_set_drag_parent(page, true);
                        lv_obj_set_drag_parent(scrl, true);
                        parent_ext->scroll_prop_obj = page;
                    }
                }
            }
        }

        scrl_reposition(page);

        lv_page_ext_t * ext      = lv_obj_get_ext_attr(page);

        /*The scrollbars are important only if they are visible now or the scrollable's size has changed*/
        if((ext->scrlbar.hor_draw || ext->scrlbar.ver_draw) ||
           (lv_obj_get_width(scrl) != lv_area_get_width(param) || lv_obj_get_height(scrl) != lv_area_get_height(param))) {
            scrlbar_refresh(page);
        }
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        scrl_reposition(page);
        scrlbar_refresh(page);
    }
    else if(sign == LV_SIGNAL_DRAG_BEGIN) {
        if(page_ext->scrlbar.mode == LV_SCROLLBAR_MODE_DRAG) {
            scrlbar_refresh(page);
        }
    }
    else if(sign == LV_SIGNAL_DRAG_END) {
        /*Scroll propagation is finished on drag end*/
        if(page_ext->scroll_prop_obj) {
            lv_obj_t * scroller_page = page_ext->scroll_prop_obj;
            lv_page_ext_t * scroller_page_ext      = lv_obj_get_ext_attr(scroller_page);
            page_ext->scroll_prop_obj = NULL;
            lv_obj_set_drag_parent(scroller_page, false);
            lv_obj_set_drag_parent(lv_page_get_scrollable(scroller_page), false);

            /*Hide scrollbars if required*/
            if(scroller_page_ext->scrlbar.mode == LV_SCROLLBAR_MODE_DRAG) {
                lv_area_t sb_area_tmp;
                if(scroller_page_ext->scrlbar.hor_draw) {
                    lv_area_copy(&sb_area_tmp, &scroller_page_ext->scrlbar.hor_area);
                    sb_area_tmp.x1 += scroller_page->coords.x1;
                    sb_area_tmp.y1 += scroller_page->coords.y1;
                    sb_area_tmp.x2 += scroller_page->coords.x1;
                    sb_area_tmp.y2 += scroller_page->coords.y1;
                    lv_obj_invalidate_area(scroller_page, &sb_area_tmp);
                    scroller_page_ext->scrlbar.hor_draw = 0;
                }
                if(scroller_page_ext->scrlbar.ver_draw) {
                    lv_area_copy(&sb_area_tmp, &scroller_page_ext->scrlbar.ver_area);
                    sb_area_tmp.x1 += scroller_page->coords.x1;
                    sb_area_tmp.y1 += scroller_page->coords.y1;
                    sb_area_tmp.x2 += scroller_page->coords.x1;
                    sb_area_tmp.y2 += scroller_page->coords.y1;
                    lv_obj_invalidate_area(scroller_page, &sb_area_tmp);
                    scroller_page_ext->scrlbar.ver_draw = 0;
                }
            }

            /*The scrolling can be chained so stop all of them*/
            lv_page_ext_t * scroller_ext = lv_obj_get_ext_attr(scroller_page);
            while(scroller_ext->scroll_prop_obj) {
                scroller_page = scroller_ext->scroll_prop_obj;
                scroller_ext->scroll_prop_obj = NULL;
                lv_obj_set_drag_parent(scroller_page, false);
                lv_obj_set_drag_parent(lv_page_get_scrollable(scroller_page), false);

                /*Hide scrollbars if required*/
                if(scroller_page_ext->scrlbar.mode == LV_SCROLLBAR_MODE_DRAG) {
                    scroller_page_ext->scrlbar.hor_draw = 0;
                    scroller_page_ext->scrlbar.ver_draw = 0;
                    lv_obj_invalidate(scroller_page);
                }
                scroller_ext = lv_obj_get_ext_attr(scroller_page);
            }
        }

        /*Hide scrollbars if required*/
        if(page_ext->scrlbar.mode == LV_SCROLLBAR_MODE_DRAG) {
            lv_area_t sb_area_tmp;
            if(page_ext->scrlbar.hor_draw) {
                lv_area_copy(&sb_area_tmp, &page_ext->scrlbar.hor_area);
                sb_area_tmp.x1 += page->coords.x1;
                sb_area_tmp.y1 += page->coords.y1;
                sb_area_tmp.x2 += page->coords.x1;
                sb_area_tmp.y2 += page->coords.y1;
                lv_obj_invalidate_area(page, &sb_area_tmp);
                page_ext->scrlbar.hor_draw = 0;
            }
            if(page_ext->scrlbar.ver_draw) {
                lv_area_copy(&sb_area_tmp, &page_ext->scrlbar.ver_area);
                sb_area_tmp.x1 += page->coords.x1;
                sb_area_tmp.y1 += page->coords.y1;
                sb_area_tmp.x2 += page->coords.x1;
                sb_area_tmp.y2 += page->coords.y1;
                lv_obj_invalidate_area(page, &sb_area_tmp);
                page_ext->scrlbar.ver_draw = 0;
            }
        }
    }
    else if(sign == LV_SIGNAL_CLEANUP) {
        page_ext->scrl = NULL;

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
       event == LV_EVENT_LONG_PRESSED || event == LV_EVENT_LONG_PRESSED_REPEAT ||
       event == LV_EVENT_DRAG_BEGIN || event == LV_EVENT_DRAG_END || event == LV_EVENT_DRAG_THROW_BEGIN) {
        lv_event_send(page, event, lv_event_get_data());
    }
    /*clang-format on*/
}

/**
 * Get the style descriptor of a part of the object
 * @param page pointer the object
 * @param part the part of the page. (LV_PAGE_PART_...)
 * @return pointer to the style descriptor of the specified part
 */
static lv_style_list_t * lv_page_get_style(lv_obj_t * page, uint8_t part)
{
    LV_ASSERT_OBJ(page, LV_OBJX_NAME);

    lv_page_ext_t * ext = lv_obj_get_ext_attr(page);
    lv_style_list_t * style_dsc_p;

    switch(part) {
        case LV_PAGE_PART_BG:
            style_dsc_p = &page->style_list;
            break;
        case LV_PAGE_PART_SCROLLABLE:
            style_dsc_p = lv_obj_get_style_list(ext->scrl, LV_CONT_PART_MAIN);
            break;
        case LV_PAGE_PART_SCROLLBAR:
            style_dsc_p = &ext->scrlbar.style;
            break;
#if LV_USE_ANIMATION
        case LV_PAGE_PART_EDGE_FLASH:
            style_dsc_p = &ext->edge_flash.style;
            break;
#endif
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}

static void scrl_reposition(lv_obj_t * page)
{
    /*Limit the position of the scrollable object to be always visible
     * (Do not let its edge inner then its parent respective edge)*/
    lv_obj_t * scrl = lv_page_get_scrollable(page);
    lv_coord_t new_x = lv_obj_get_x(scrl);
    lv_coord_t new_y = lv_obj_get_y(scrl);
    bool refr_x      = false;
    bool refr_y      = false;
    lv_area_t page_coords;
    lv_area_t scrl_coords;
    lv_obj_get_coords(scrl, &scrl_coords);
    lv_obj_get_coords(page, &page_coords);

    lv_style_int_t left = lv_obj_get_style_pad_left(page, LV_PAGE_PART_BG);
    lv_style_int_t right = lv_obj_get_style_pad_right(page, LV_PAGE_PART_BG);
    lv_style_int_t top = lv_obj_get_style_pad_top(page, LV_PAGE_PART_BG);
    lv_style_int_t bottom = lv_obj_get_style_pad_bottom(page, LV_PAGE_PART_BG);

    /*scrollable width smaller then page width? -> align to left*/
    if(lv_area_get_width(&scrl_coords) + left + right <= lv_area_get_width(&page_coords)) {
        if(scrl_coords.x1 != page_coords.x1 + left) {
            new_x  = left;
            refr_x = true;
        }
    }
    else {
        /*The edges of the scrollable can not be in the page (minus hpad) */
        if(scrl_coords.x2 < page_coords.x2 - right) {
            new_x = lv_area_get_width(&page_coords) - lv_area_get_width(&scrl_coords) - right; /* Right align */
            refr_x = true;
            lv_page_start_edge_flash(page, LV_PAGE_EDGE_RIGHT);
        }
        else if(scrl_coords.x1 > page_coords.x1 + left) {
            new_x  = left; /*Left align*/
            refr_x = true;
            lv_page_start_edge_flash(page, LV_PAGE_EDGE_LEFT);
        }
    }

    /*scrollable height smaller then page height? -> align to top*/
    if(lv_area_get_height(&scrl_coords) + top + bottom <= lv_area_get_height(&page_coords)) {
        if(scrl_coords.y1 != page_coords.y1 + top) {
            new_y  = top;
            refr_y = true;
        }
    }
    else {
        /*The edges of the scrollable can not be in the page (minus vpad) */
        if(scrl_coords.y2 < page_coords.y2 - bottom) {
            new_y = lv_area_get_height(&page_coords) - lv_area_get_height(&scrl_coords) - bottom; /* Bottom align */
            refr_y = true;
            lv_page_start_edge_flash(page, LV_PAGE_EDGE_BOTTOM);
        }
        else if(scrl_coords.y1 > page_coords.y1 + top) {
            new_y  = top; /*Top align*/
            refr_y = true;
            lv_page_start_edge_flash(page, LV_PAGE_EDGE_TOP);
        }
    }

    if(refr_x || refr_y) {
        lv_obj_set_pos(scrl, new_x, new_y);
    }
}

/**
 * Refresh the position and size of the scroll bars.
 * @param page pointer to a page object
 */
static void scrlbar_refresh(lv_obj_t * page)
{
    lv_page_ext_t * ext      = lv_obj_get_ext_attr(page);
    lv_obj_t * scrl          = ext->scrl;
    lv_coord_t size_tmp;
    lv_coord_t scrl_w = lv_obj_get_width(scrl);
    lv_coord_t scrl_h = lv_obj_get_height(scrl);
    lv_coord_t obj_w  = lv_obj_get_width(page);
    lv_coord_t obj_h  = lv_obj_get_height(page);

    lv_style_int_t sb_width = lv_obj_get_style_size(page, LV_PAGE_PART_SCROLLBAR);
    lv_style_int_t sb_right = lv_obj_get_style_pad_right(page, LV_PAGE_PART_SCROLLBAR);
    lv_style_int_t sb_bottom = lv_obj_get_style_pad_bottom(page, LV_PAGE_PART_SCROLLBAR);

    lv_style_int_t bg_left = lv_obj_get_style_pad_left(page, LV_PAGE_PART_BG);
    lv_style_int_t bg_right = lv_obj_get_style_pad_right(page, LV_PAGE_PART_BG);
    lv_style_int_t bg_top = lv_obj_get_style_pad_top(page, LV_PAGE_PART_BG);
    lv_style_int_t bg_bottom = lv_obj_get_style_pad_bottom(page, LV_PAGE_PART_BG);

    /*Always let 'scrollbar width' padding above, under, left and right to the scrollbars
     * else:
     * - horizontal and vertical scrollbars can overlap on the corners
     * - if the page has radius the scrollbar can be out of the radius  */
    lv_coord_t sb_hor_pad = LV_MATH_MAX(sb_width, sb_right);
    lv_coord_t sb_ver_pad = LV_MATH_MAX(sb_width, sb_bottom);

    if(ext->scrlbar.mode == LV_SCROLLBAR_MODE_OFF) return;

    if(ext->scrlbar.mode == LV_SCROLLBAR_MODE_ON) {
        ext->scrlbar.hor_draw = 1;
        ext->scrlbar.ver_draw = 1;
    }

    /*Invalidate the current (old) scrollbar areas*/
    lv_area_t sb_area_tmp;
    if(ext->scrlbar.hor_draw != 0) {
        lv_area_copy(&sb_area_tmp, &ext->scrlbar.hor_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        lv_obj_invalidate_area(page, &sb_area_tmp);
    }
    if(ext->scrlbar.ver_draw != 0) {
        lv_area_copy(&sb_area_tmp, &ext->scrlbar.ver_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        lv_obj_invalidate_area(page, &sb_area_tmp);
    }

    /*Full sized horizontal scrollbar*/
    if(scrl_w <= obj_w - bg_left - bg_right) {
        lv_area_set_width(&ext->scrlbar.hor_area, obj_w - 2 * sb_hor_pad);
        _lv_area_set_pos(&ext->scrlbar.hor_area, sb_hor_pad,
                         obj_h - sb_width - sb_bottom);
        if(ext->scrlbar.mode == LV_SCROLLBAR_MODE_AUTO ||
           ext->scrlbar.mode == LV_SCROLLBAR_MODE_DRAG) ext->scrlbar.hor_draw = 0;
    }
    /*Smaller horizontal scrollbar*/
    else {
        size_tmp =
            (obj_w * (obj_w - (2 * sb_hor_pad))) / (scrl_w + bg_left + bg_right);
        if(size_tmp < LV_PAGE_SB_MIN_SIZE) size_tmp = LV_PAGE_SB_MIN_SIZE;
        lv_area_set_width(&ext->scrlbar.hor_area, size_tmp);

        _lv_area_set_pos(&ext->scrlbar.hor_area,
                         sb_hor_pad +
                         (-(lv_obj_get_x(scrl) - bg_left) * (obj_w - size_tmp - 2 * sb_hor_pad)) /
                         (scrl_w + bg_left + bg_right - obj_w),
                         obj_h - sb_width - sb_bottom);

        if(ext->scrlbar.mode == LV_SCROLLBAR_MODE_AUTO) ext->scrlbar.hor_draw = 1;
        else if(ext->scrlbar.mode == LV_SCROLLBAR_MODE_DRAG) {
            lv_indev_t * indev = lv_indev_get_next(NULL);
            while(indev) {
                if(indev->driver.type == LV_INDEV_TYPE_POINTER && (indev->proc.types.pointer.drag_dir & LV_DRAG_DIR_HOR)) {
                    lv_obj_t * drag_obj = indev->proc.types.pointer.act_obj;
                    while(drag_obj && drag_obj->drag_parent) drag_obj = lv_obj_get_parent(drag_obj);

                    if(drag_obj && drag_obj == scrl) {
                        ext->scrlbar.hor_draw = 1;
                        break;
                    }
                }
                indev = lv_indev_get_next(indev);
            }
        }
    }

    /*Full sized vertical scroll bar*/
    if(scrl_h <= obj_h - bg_top - bg_bottom) {
        lv_area_set_height(&ext->scrlbar.ver_area, obj_h - 2 * sb_ver_pad);
        _lv_area_set_pos(&ext->scrlbar.ver_area,
                         obj_w - sb_width - sb_right, sb_ver_pad);
        if(ext->scrlbar.mode == LV_SCROLLBAR_MODE_AUTO ||
           ext->scrlbar.mode == LV_SCROLLBAR_MODE_DRAG) ext->scrlbar.ver_draw = 0;
    }
    /*Smaller vertical scroll bar*/
    else {
        size_tmp =
            (obj_h * (obj_h - (2 * sb_ver_pad))) / (scrl_h + bg_top + bg_bottom);
        if(size_tmp < LV_PAGE_SB_MIN_SIZE) size_tmp = LV_PAGE_SB_MIN_SIZE;
        lv_area_set_height(&ext->scrlbar.ver_area, size_tmp);

        _lv_area_set_pos(&ext->scrlbar.ver_area,
                         obj_w - sb_width - sb_right,
                         sb_ver_pad + (-(lv_obj_get_y(scrl) - bg_left) *
                                       (obj_h - size_tmp - 2 * sb_ver_pad)) /
                         (scrl_h + bg_top + bg_bottom - obj_h));

        if(ext->scrlbar.mode == LV_SCROLLBAR_MODE_AUTO) ext->scrlbar.ver_draw = 1;
        else if(ext->scrlbar.mode == LV_SCROLLBAR_MODE_DRAG) {
            lv_indev_t * indev = lv_indev_get_next(NULL);
            while(indev) {
                if(indev->driver.type == LV_INDEV_TYPE_POINTER && (indev->proc.types.pointer.drag_dir & LV_DRAG_DIR_VER)) {
                    lv_obj_t * drag_obj = indev->proc.types.pointer.act_obj;
                    while(drag_obj && drag_obj->drag_parent) drag_obj = lv_obj_get_parent(drag_obj);

                    if(drag_obj && drag_obj == scrl) {
                        ext->scrlbar.ver_draw = 1;
                        break;
                    }
                }
                indev = lv_indev_get_next(indev);
            }
        }
    }

    /*Invalidate the new scrollbar areas*/
    if(ext->scrlbar.hor_draw != 0) {
        lv_area_copy(&sb_area_tmp, &ext->scrlbar.hor_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        lv_obj_invalidate_area(page, &sb_area_tmp);
    }
    if(ext->scrlbar.ver_draw != 0) {
        lv_area_copy(&sb_area_tmp, &ext->scrlbar.ver_area);
        sb_area_tmp.x1 += page->coords.x1;
        sb_area_tmp.y1 += page->coords.y1;
        sb_area_tmp.x2 += page->coords.x1;
        sb_area_tmp.y2 += page->coords.y1;
        lv_obj_invalidate_area(page, &sb_area_tmp);
    }
}

static void refr_ext_draw_pad(lv_obj_t * page)
{
    lv_style_int_t sb_bottom = lv_obj_get_style_pad_bottom(page, LV_PAGE_PART_SCROLLBAR);
    lv_style_int_t sb_right = lv_obj_get_style_pad_right(page, LV_PAGE_PART_SCROLLBAR);

    /*Ensure ext. size for the scrollbars if they are out of the page*/
    if(page->ext_draw_pad < (-sb_right)) page->ext_draw_pad = -sb_right;
    if(page->ext_draw_pad < (-sb_bottom)) page->ext_draw_pad = -sb_bottom;
}

#if LV_USE_ANIMATION
static void edge_flash_anim(void * page, lv_anim_value_t v)
{
    lv_page_ext_t * ext   = lv_obj_get_ext_attr(page);
    ext->edge_flash.state = v;

    lv_area_t flash_area;
    get_edge_flash_area(page, &flash_area, LV_PAGE_END_FLASH_SIZE);
    lv_obj_invalidate_area(page, &flash_area);
}

static void edge_flash_anim_end(lv_anim_t * a)
{
    lv_area_t flash_area;
    get_edge_flash_area(a->var, &flash_area, LV_PAGE_END_FLASH_SIZE);
    lv_obj_invalidate_area(a->var, &flash_area);

    lv_page_ext_t * ext       = lv_obj_get_ext_attr(a->var);
    ext->edge_flash.top_ip    = 0;
    ext->edge_flash.bottom_ip = 0;
    ext->edge_flash.left_ip   = 0;
    ext->edge_flash.right_ip  = 0;

}

static void get_edge_flash_area(lv_obj_t * page, lv_area_t * flash_area, lv_coord_t state)
{
    lv_coord_t page_w = lv_obj_get_width(page);
    lv_coord_t page_h = lv_obj_get_height(page);
    lv_page_ext_t * ext   = lv_obj_get_ext_attr(page);

    if(ext->edge_flash.top_ip) {
        flash_area->x1 = page->coords.x1 - page_w;
        flash_area->x2 = page->coords.x2 + page_w;
        flash_area->y1 = page->coords.y1 - 3 * page_w + state;
        flash_area->y2 = page->coords.y1 + state;
    }
    else if(ext->edge_flash.bottom_ip) {
        flash_area->x1 = page->coords.x1 - page_w;
        flash_area->x2 = page->coords.x2 + page_w;
        flash_area->y1 = page->coords.y2 - state;
        flash_area->y2 = page->coords.y2 + 3 * page_w - state;
    }
    else if(ext->edge_flash.right_ip) {
        flash_area->x1 = page->coords.x2 - state;
        flash_area->x2 = page->coords.x2 + 3 * page_h - state;
        flash_area->y1 = page->coords.y1 - page_h;
        flash_area->y2 = page->coords.y2 + page_h;
    }
    else if(ext->edge_flash.left_ip) {
        flash_area->x1 = page->coords.x1 - 3 * page_h + state;
        flash_area->x2 = page->coords.x1 + state;
        flash_area->y1 = page->coords.y1 - page_h;
        flash_area->y2 = page->coords.y2 + page_h;
    }
    else {
        lv_area_set(flash_area, 0, 0, -1, -1);
    }
}

#endif

#endif
