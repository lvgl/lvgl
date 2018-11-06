/**
 * @file lv_win.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_win.h"
#if USE_LV_WIN != 0

#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_win_signal(lv_obj_t * win, lv_signal_t sign, void * param);
static void lv_win_realign(lv_obj_t * win);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a window objects
 * @param par pointer to an object, it will be the parent of the new window
 * @param copy pointer to a window object, if not NULL then the new object will be copied from it
 * @return pointer to the created window
 */
lv_obj_t * lv_win_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("window create started");

    /*Create the ancestor object*/
    lv_obj_t * new_win = lv_obj_create(par, copy);
    lv_mem_assert(new_win);
    if(new_win == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_win);

    /*Allocate the object type specific extended data*/
    lv_win_ext_t * ext = lv_obj_allocate_ext_attr(new_win, sizeof(lv_win_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->page = NULL;
    ext->header = NULL;
    ext->title = NULL;
    ext->style_header = &lv_style_plain_color;
    ext->style_btn_rel = &lv_style_btn_rel;
    ext->style_btn_pr = &lv_style_btn_pr;
    ext->btn_size = (LV_DPI) / 2;

    /*Init the new window object*/
    if(copy == NULL) {
        lv_obj_set_size(new_win, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_pos(new_win, 0, 0);
        lv_obj_set_style(new_win, &lv_style_pretty);

        ext->page = lv_page_create(new_win, NULL);
        lv_obj_set_protect(ext->page, LV_PROTECT_PARENT);
        lv_page_set_sb_mode(ext->page, LV_SB_MODE_AUTO);
        lv_page_set_arrow_scroll(ext->page, true);

        /*Create a holder for the header*/
        ext->header = lv_obj_create(new_win, NULL);
        /*Move back the header because it is automatically moved to the scrollable */
        lv_obj_set_protect(ext->header, LV_PROTECT_PARENT);
        lv_obj_set_parent(ext->header, new_win);

        /*Create a title on the header*/
        ext->title = lv_label_create(ext->header, NULL);
        lv_label_set_text(ext->title, "My title");

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_win_set_style(new_win, LV_WIN_STYLE_BG, th->win.bg);
            lv_win_set_style(new_win, LV_WIN_STYLE_SB, th->win.sb);
            lv_win_set_style(new_win, LV_WIN_STYLE_HEADER, th->win.header);
            lv_win_set_style(new_win, LV_WIN_STYLE_CONTENT_BG, th->win.content.bg);
            lv_win_set_style(new_win, LV_WIN_STYLE_CONTENT_SCRL, th->win.content.scrl);
            lv_win_set_style(new_win, LV_WIN_STYLE_BTN_REL, th->win.btn.rel);
            lv_win_set_style(new_win, LV_WIN_STYLE_BTN_PR, th->win.btn.pr);
        } else {
            lv_win_set_style(new_win, LV_WIN_STYLE_BG, &lv_style_plain);
            lv_win_set_style(new_win, LV_WIN_STYLE_CONTENT_BG, &lv_style_plain);
            lv_win_set_style(new_win, LV_WIN_STYLE_CONTENT_SCRL, &lv_style_transp);
            lv_win_set_style(new_win, LV_WIN_STYLE_HEADER, &lv_style_plain_color);
        }

        lv_obj_set_signal_func(new_win, lv_win_signal);
        lv_obj_set_size(new_win, LV_HOR_RES, LV_VER_RES);
    }
    /*Copy an existing object*/
    else {
        lv_win_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        /*Create the objects*/
        ext->header = lv_obj_create(new_win, copy_ext->header);
        ext->title = lv_label_create(ext->header, copy_ext->title);
        ext->page = lv_page_create(new_win, copy_ext->page);
        ext->btn_size = copy_ext->btn_size;

        /*Copy the control buttons*/
        lv_obj_t * child;
        lv_obj_t * cbtn;
        child = lv_obj_get_child_back(copy_ext->header, NULL);
        child = lv_obj_get_child_back(copy_ext->header, child); /*Sip the title*/
        while(child != NULL) {
            cbtn = lv_btn_create(ext->header, child);
            lv_img_create(cbtn, lv_obj_get_child(child, NULL));
            child = lv_obj_get_child_back(copy_ext->header, child);
        }

        lv_obj_set_signal_func(new_win, lv_win_signal);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_win);
    }

    lv_win_realign(new_win);

    LV_LOG_INFO("window created");

    return new_win;
}

/**
 * Delete all children of the scrl object, without deleting scrl child.
 * @param obj pointer to an object
 */
void lv_win_clean(lv_obj_t * obj)
{
    lv_obj_t * scrl = lv_page_get_scrl(obj);
    lv_obj_clean(scrl);
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add control button to the header of the window
 * @param win pointer to a window object
 * @param img_src an image source ('lv_img_t' variable, path to file or a symbol)
 * @param rel_action a function pointer to call when the button is released
 * @return pointer to the created button object
 */
lv_obj_t * lv_win_add_btn(lv_obj_t * win, const void * img_src, lv_action_t rel_action)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);

    lv_obj_t * btn = lv_btn_create(ext->header, NULL);
    lv_btn_set_style(btn, LV_BTN_STYLE_REL, ext->style_btn_rel);
    lv_btn_set_style(btn, LV_BTN_STYLE_PR, ext->style_btn_pr);
    lv_obj_set_size(btn, ext->btn_size, ext->btn_size);
    lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, rel_action);

    lv_obj_t * img = lv_img_create(btn, NULL);
    lv_obj_set_click(img, false);
    lv_img_set_src(img, img_src);

    lv_win_realign(win);

    return btn;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * A release action which can be assigned to a window control button to close it
 * @param btn pointer to the released button
 * @return always LV_ACTION_RES_INV because the button is deleted with the window
 */
lv_res_t lv_win_close_action(lv_obj_t * btn)
{
    lv_obj_t * win = lv_win_get_from_btn(btn);

    lv_obj_del(win);

    return LV_RES_INV;
}

/**
 * Set the title of a window
 * @param win pointer to a window object
 * @param title string of the new title
 */
void lv_win_set_title(lv_obj_t * win, const char * title)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);

    lv_label_set_text(ext->title, title);
    lv_win_realign(win);
}

/**
 * Set the control button size of a window
 * @param win pointer to a window object
 * @param size control button size
 */
void lv_win_set_btn_size(lv_obj_t * win, lv_coord_t size)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    if(ext->btn_size == size) return;

    ext->btn_size = size;

    lv_win_realign(win);
}

/**
 * Set the layout of the window
 * @param win pointer to a window object
 * @param layout the layout from 'lv_layout_t'
 */
void lv_win_set_layout(lv_obj_t * win, lv_layout_t layout)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    lv_page_set_scrl_layout(ext->page, layout);
}

/**
 * Set the scroll bar mode of a window
 * @param win pointer to a window object
 * @param sb_mode the new scroll bar mode from  'lv_sb_mode_t'
 */
void lv_win_set_sb_mode(lv_obj_t * win, lv_sb_mode_t sb_mode)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    lv_page_set_sb_mode(ext->page, sb_mode);
}

/**
 * Set a style of a window
 * @param win pointer to a window object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_win_set_style(lv_obj_t * win, lv_win_style_t type, lv_style_t * style)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);

    switch(type) {
        case LV_WIN_STYLE_BG:
            lv_obj_set_style(win, style);
            lv_win_realign(win);
            break;
        case LV_WIN_STYLE_CONTENT_BG:
            lv_page_set_style(ext->page, LV_PAGE_STYLE_BG, style);
            break;
        case LV_WIN_STYLE_CONTENT_SCRL:
            lv_page_set_style(ext->page, LV_PAGE_STYLE_SCRL, style);
            break;
        case LV_WIN_STYLE_SB:
            lv_page_set_style(ext->page, LV_PAGE_STYLE_SB, style);
            break;
        case LV_WIN_STYLE_HEADER:
            lv_obj_set_style(ext->header, style);
            lv_win_realign(win);
            break;
        case LV_WIN_STYLE_BTN_REL:
            ext->style_btn_rel = style;
            break;
        case LV_WIN_STYLE_BTN_PR:
            ext->style_btn_pr = style;
            break;
    }

    /*Refresh the existing buttons*/
    if(type == LV_WIN_STYLE_BTN_REL || type == LV_WIN_STYLE_BTN_PR) {
        lv_obj_t * btn;
        btn = lv_obj_get_child_back(ext->header, NULL);
        btn = lv_obj_get_child_back(ext->header, btn);    /*Skip the title*/
        while(btn != NULL) {
            if(type == LV_WIN_STYLE_BTN_REL) lv_btn_set_style(btn, LV_BTN_STYLE_REL, style);
            else lv_btn_set_style(btn, LV_BTN_STYLE_PR, style);
            btn = lv_obj_get_child_back(ext->header, btn);
        }
    }
}


/*=====================
 * Getter functions
 *====================*/

/**
 * Get the title of a window
 * @param win pointer to a window object
 * @return title string of the window
 */
const char * lv_win_get_title(const lv_obj_t * win)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    return lv_label_get_text(ext->title);
}

/**
* Get the content holder object of window (`lv_page`) to allow additional customization
* @param win pointer to a window object
* @return the Page object where the window's content is
*/
lv_obj_t * lv_win_get_content(const lv_obj_t * win)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    return ext->page;
}

/**
 * Get the control button size of a window
 * @param win pointer to a window object
 * @return control button size
 */
lv_coord_t lv_win_get_btn_size(const lv_obj_t * win)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    return ext->btn_size;
}

/**
 * Get the pointer of a widow from one of  its control button.
 * It is useful in the action of the control buttons where only button is known.
 * @param ctrl_btn pointer to a control button of a window
 * @return pointer to the window of 'ctrl_btn'
 */
lv_obj_t * lv_win_get_from_btn(const lv_obj_t * ctrl_btn)
{
    lv_obj_t * header = lv_obj_get_parent(ctrl_btn);
    lv_obj_t * win = lv_obj_get_parent(header);

    return win;
}

/**
 * Get the layout of a window
 * @param win pointer to a window object
 * @return the layout of the window (from 'lv_layout_t')
 */
lv_layout_t lv_win_get_layout(lv_obj_t * win)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    return lv_page_get_scrl_layout(ext->page);
}

/**
 * Get the scroll bar mode of a window
 * @param win pointer to a window object
 * @return the scroll bar mode of the window (from 'lv_sb_mode_t')
 */
lv_sb_mode_t lv_win_get_sb_mode(lv_obj_t * win)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    return lv_page_get_sb_mode(ext->page);
}

/**
 * Get width of the content area (page scrollable) of the window
 * @param win pointer to a window object
 * @return the width of the content_bg area
 */
lv_coord_t lv_win_get_width(lv_obj_t * win)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    lv_obj_t * scrl = lv_page_get_scrl(ext->page);
    lv_style_t * style_scrl = lv_obj_get_style(scrl);

    return lv_obj_get_width(scrl) - 2 * style_scrl->body.padding.hor;
}

/**
 * Get a style of a window
 * @param win pointer to a button object
 * @param type which style window be get
 * @return style pointer to a style
 */
lv_style_t * lv_win_get_style(const lv_obj_t * win, lv_win_style_t type)
{
    lv_style_t * style = NULL;
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);

    switch(type) {
        case LV_WIN_STYLE_BG:
            style = lv_obj_get_style(win);
            break;
        case LV_WIN_STYLE_CONTENT_BG:
            style = lv_page_get_style(ext->page, LV_PAGE_STYLE_BG);
            break;
        case LV_WIN_STYLE_CONTENT_SCRL:
            style = lv_page_get_style(ext->page, LV_PAGE_STYLE_SCRL);
            break;
        case LV_WIN_STYLE_SB:
            style = lv_page_get_style(ext->page, LV_PAGE_STYLE_SB);
            break;
        case LV_WIN_STYLE_HEADER:
            style = lv_obj_get_style(ext->header);
            break;
        case LV_WIN_STYLE_BTN_REL:
            style = ext->style_btn_rel;
            break;
        case LV_WIN_STYLE_BTN_PR:
            style = ext->style_btn_pr;
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
 * Focus on an object. It ensures that the object will be visible in the window.
 * @param win pointer to a window object
 * @param obj pointer to an object to focus (must be in the window)
 * @param anim_time scroll animation time in milliseconds (0: no animation)
 */
void lv_win_focus(lv_obj_t * win, lv_obj_t * obj, uint16_t anim_time)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    lv_page_focus(ext->page, obj, anim_time);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the window
 * @param win pointer to a window object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_win_signal(lv_obj_t * win, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(win, sign, param);
    if(res != LV_RES_OK) return res;

    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    if(sign == LV_SIGNAL_CHILD_CHG) { /*Move children to the page*/
        lv_obj_t * page = ext->page;
        if(page != NULL) {
            lv_obj_t * child;
            child = lv_obj_get_child(win, NULL);
            while(child != NULL) {
                if(lv_obj_is_protected(child, LV_PROTECT_PARENT) == false) {
                    lv_obj_t * tmp = child;
                    child = lv_obj_get_child(win, child); /*Get the next child before move this*/
                    lv_obj_set_parent(tmp, page);
                } else {
                    child = lv_obj_get_child(win, child);
                }
            }
        }
    } else if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_win_realign(win);
    } else if(sign == LV_SIGNAL_CORD_CHG) {
        /*If the size is changed refresh the window*/
        if(lv_area_get_width(param) != lv_obj_get_width(win) ||
                lv_area_get_height(param) != lv_obj_get_height(win)) {
            lv_win_realign(win);
        }
    } else if(sign == LV_SIGNAL_CLEANUP) {
        ext->header = NULL;     /*These objects were children so they are already invalid*/
        ext->page = NULL;
        ext->title = NULL;
    } else if(sign == LV_SIGNAL_CONTROLL) {
        /*Forward all the control signals to the page*/
        ext->page->signal_func(ext->page, sign, param);
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_win";
    }


    return res;
}

/**
 * Realign the building elements of a window
 * @param win pointer to window objectker
 */
static void lv_win_realign(lv_obj_t * win)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);

    if(ext->page == NULL || ext->header == NULL || ext->title == NULL) return;

    lv_style_t * header_style = lv_win_get_style(win, LV_WIN_STYLE_HEADER);
    lv_obj_set_size(ext->header, lv_obj_get_width(win), ext->btn_size + 2 * header_style->body.padding.ver);

    bool first_btn = true;
    lv_obj_t * btn;
    lv_obj_t * btn_prev = NULL;
    /*Refresh the size of all control buttons*/
    btn = lv_obj_get_child_back(ext->header, NULL);
    btn = lv_obj_get_child_back(ext->header, btn);    /*Skip the title*/
    while(btn != NULL) {
        lv_obj_set_size(btn, ext->btn_size, ext->btn_size);
        if(first_btn) {
            lv_obj_align(btn, ext->header, LV_ALIGN_IN_RIGHT_MID, - header_style->body.padding.hor, 0);
            first_btn = false;
        } else {
            lv_obj_align(btn, btn_prev, LV_ALIGN_OUT_LEFT_MID, - header_style->body.padding.inner, 0);
        }
        btn_prev = btn;
        btn = lv_obj_get_child_back(ext->header, btn);
    }


    lv_obj_align(ext->title, NULL, LV_ALIGN_IN_LEFT_MID, ext->style_header->body.padding.hor, 0);

    lv_obj_set_pos(ext->header, 0, 0);

    lv_obj_set_size(ext->page, lv_obj_get_width(win), lv_obj_get_height(win) - lv_obj_get_height(ext->header));
    lv_obj_align(ext->page, ext->header, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
}

#endif

