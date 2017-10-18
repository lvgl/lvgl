/**
 * @file lv_tab.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_TABVIEW != 0

#include <lvgl/lv_objx/lv_tabview.h>
#include "lv_btnm.h"
#include "misc/gfx/anim.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if 0 /*Unused*/
static bool lv_tab_design(lv_obj_t * tab, const area_t * mask, lv_design_mode_t mode);
#endif

static bool tabpage_signal(lv_obj_t * tab_page, lv_signal_t sign, void * param);
static bool tabscrl_signal(lv_obj_t * tab_scrl, lv_signal_t sign, void * param);

static void tabpage_pressed_hadler(lv_obj_t * tabview, lv_obj_t * tabpage);
static void tabpage_pressing_hadler(lv_obj_t * tabview, lv_obj_t * tabpage);
static void tabpage_press_lost_hadler(lv_obj_t * tabview, lv_obj_t * tabpage);
static lv_action_res_t tab_btnm_action(lv_obj_t * tab_btnm, uint16_t id);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_f_t page_scrl_signal;
static const char * tab_def[] = {""};
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*----------------- 
 * Create function
 *-----------------*/

/**
 * Create a Tab view object
 * @param par pointer to an object, it will be the parent of the new tab
 * @param copy pointer to a tab object, if not NULL then the new object will be copied from it
 * @return pointer to the created tab
 */
lv_obj_t * lv_tabview_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of tab*/
    lv_obj_t * new_tabview = lv_obj_create(par, copy);
    dm_assert(new_tabview);
    
    /*Allocate the tab type specific extended data*/
    lv_tabview_ext_t * ext = lv_obj_alloc_ext(new_tabview, sizeof(lv_tabview_ext_t));
    dm_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->drag_h = 0;
    ext->draging = 0;
    ext->tab_act = 0;
    ext->point_last.x = 0;
    ext->point_last.y = 0;
    ext->content = NULL;
    ext->indic = NULL;
    ext->tabs = NULL;
    ext->tab_name_ptr = dm_alloc(sizeof(char*));
    ext->tab_name_ptr[0] = "";

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_f(new_tabview, lv_tabview_signal);

    /*Init the new tab tab*/
    if(copy == NULL) {
        lv_obj_set_size(new_tabview, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_style(new_tabview, lv_style_get(LV_STYLE_PRETTY, NULL));

        ext->tabs = lv_btnm_create(new_tabview, NULL);
        lv_btnm_set_map(ext->tabs, tab_def);
        lv_obj_set_height(ext->tabs, 3 * LV_DPI / 4);
        lv_btnm_set_action(ext->tabs, tab_btnm_action);
        lv_btnm_set_tgl(ext->tabs, true, 0);

        ext->indic = lv_obj_create(ext->tabs, NULL);
        lv_style_t * style_indic = lv_obj_get_style(ext->indic);
        lv_obj_set_size(ext->indic, LV_DPI, style_indic->line.width);
        lv_obj_align(ext->indic, ext->tabs, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
        lv_obj_set_click(ext->indic, false);

        ext->content = lv_cont_create(new_tabview, NULL);
        lv_cont_set_fit(ext->content, true, false);
        lv_cont_set_layout(ext->content, LV_CONT_LAYOUT_ROW_T);
        lv_obj_set_height(ext->content, LV_VER_RES);
        lv_obj_set_style(ext->content, lv_style_get(LV_STYLE_TRANSP_TIGHT, NULL));
        lv_obj_align(ext->content, ext->tabs, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    }
    /*Copy an existing tab*/
    else {
    	lv_tabview_ext_t * copy_ext = lv_obj_get_ext(copy);
        ext->point_last.x = 0;
        ext->point_last.y = 0;
        ext->tabs = lv_btnm_create(new_tabview, copy_ext->tabs);
        ext->indic = lv_obj_create(ext->tabs, copy_ext->indic);
        ext->content = lv_cont_create(new_tabview, copy_ext->content);

        ext->tab_name_ptr = dm_alloc(sizeof(char*));
        ext->tab_name_ptr[0] = "";
        lv_btnm_set_map(ext->tabs, ext->tab_name_ptr);

        uint16_t i;
        for (i = 0; i < copy_ext->tab_cnt; i++) {
            lv_tabview_add_tab(new_tabview, copy_ext->tab_name_ptr[i]);
        }


        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_tabview);
    }
    
    return new_tabview;
}

/**
 * Signal function of the Tab view
 * @param tabview pointer to a Tab view object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_tabview_signal(lv_obj_t * tabview, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_obj_signal(tabview, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        lv_tabview_ext_t * ext = lv_obj_get_ext(tabview);
    	if(sign == LV_SIGNAL_CLEANUP) {
            dm_free(ext->tab_name_ptr);
            ext->tab_name_ptr = NULL;
    	} else if(sign == LV_SIGNAL_CORD_CHG) {
    	    if(ext->content != NULL &&
    	      (lv_obj_get_width(tabview) != area_get_width(param) ||
    	       lv_obj_get_height(tabview) != area_get_height(param)))
    	    {
    	        lv_tabview_realign(tabview);
    	    }
    	}
    }
    
    return valid;
}

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add a new tab with the given name
 * @param tabview pointer to Tab view object where to ass the new tab
 * @param name the text on the tab button
 * @return pointer to page object (lv_page) which is the containter of the contet
 */
lv_obj_t * lv_tabview_add_tab(lv_obj_t * tabview, const char * name)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext(tabview);

    /*Create the container page*/
    lv_obj_t * h = lv_page_create(ext->content, NULL);
    lv_obj_set_size(h, lv_obj_get_width(tabview), lv_obj_get_height(tabview));
    lv_obj_set_style(h, lv_style_get(LV_STYLE_PRETTY_COLOR, NULL));
    lv_obj_set_signal_f(h, tabpage_signal);
    lv_page_set_sb_mode(h, LV_PAGE_SB_MODE_AUTO);
    if(page_scrl_signal == NULL) page_scrl_signal = lv_obj_get_signal_f(lv_page_get_scrl(h));
    lv_obj_set_signal_f(lv_page_get_scrl(h), tabscrl_signal);

    /*Extend the button matrix map with the new name*/
    ext->tab_cnt++;
    ext->tab_name_ptr = dm_realloc(ext->tab_name_ptr, sizeof(char *) * (ext->tab_cnt + 1));
    ext->tab_name_ptr[ext->tab_cnt - 1] = name;
    ext->tab_name_ptr[ext->tab_cnt] = "";

    lv_btnm_set_map(ext->tabs, ext->tab_name_ptr);

    /*Modify the indicator size*/
    lv_style_t * style_tabs = lv_obj_get_style(ext->tabs);
    cord_t indic_width = (lv_obj_get_width(tabview) - style_tabs->body.pad_obj * (ext->tab_cnt - 1) - 2 * style_tabs->body.pad_hor) / ext->tab_cnt;
    lv_obj_set_width(ext->indic, indic_width);
    lv_obj_set_x(ext->indic, indic_width * ext->tab_act + style_tabs->body.pad_obj * ext->tab_act + style_tabs->body.pad_hor);

    /*Set the first tab as active*/
    if(ext->tab_cnt == 1) {
        ext->tab_act = 0;
        lv_tabview_set_act(tabview, 0, false);
    }

    return h;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new tab
 * @param tabview pointer to Tab view object
 * @param id index of a tab to load
 * @param anim_en true: set with sliding animation; false: set immediately
 */
void lv_tabview_set_act(lv_obj_t * tabview, uint16_t id, bool anim_en)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext(tabview);
    lv_style_t * style = lv_obj_get_style(ext->content);

    ext->tab_act = id >= ext->tab_cnt  ? ext->tab_cnt - 1 : id;

#if LV_TABVIEW_ANIM_TIME == 0
    anim_en = false;
#endif

    cord_t cont_x = -(lv_obj_get_width(tabview) * id + style->body.pad_obj * id + style->body.pad_hor);
    if(anim_en == false) {
        lv_obj_set_x(ext->content, cont_x);
    } else {
        anim_t a;
        a.var = ext->content;
        a.start = lv_obj_get_x(ext->content);
        a.end = cont_x;
        a.fp = (anim_fp_t)lv_obj_set_x;
        a.path = anim_get_path(ANIM_PATH_LIN);
        a.end_cb = NULL;
        a.act_time = 0;
        a.time = LV_TABVIEW_ANIM_TIME;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;
        anim_create(&a);
    }

    /*Move the indicator*/
    cord_t indic_width = lv_obj_get_width(ext->indic);
    lv_style_t * tabs_style = lv_obj_get_style(ext->tabs);
    cord_t indic_x = indic_width * id + tabs_style->body.pad_obj * id + tabs_style->body.pad_hor;

    if(anim_en == false) {
        lv_obj_set_x(ext->indic, indic_x);
    } else {
        anim_t a;
        a.var = ext->indic;
        a.start = lv_obj_get_x(ext->indic);
        a.end = indic_x;
        a.fp = (anim_fp_t)lv_obj_set_x;
        a.path = anim_get_path(ANIM_PATH_LIN);
        a.end_cb = NULL;
        a.act_time = 0;
        a.time = LV_TABVIEW_ANIM_TIME;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;
        anim_create(&a);
    }

    lv_btnm_set_tgl(ext->tabs, true, ext->tab_act);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the index of the currently active tab
 * @param tabview pointer to Tab view object
 * @return the active tab index
 */
uint16_t lv_tabview_get_tab_act(lv_obj_t * tabview)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext(tabview);
    return ext->tab_act;
}

/**
 * Get the number of tabs
 * @param tabview pointer to Tab view object
 * @return tab count
 */
uint16_t lv_tabview_get_tab_cnt(lv_obj_t * tabview)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext(tabview);
    return ext->tab_cnt;
}

/**
 * Get the page (content area) of a tab
 * @param tabview pointer to Tab view object
 * @param id index of the tab (>= 0)
 * @return pointer to page (lv_page) object
 */
lv_obj_t * lv_tabview_get_tab_page(lv_obj_t * tabview, uint16_t id)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext(tabview);
    uint16_t i = 0;
    lv_obj_t * page = lv_obj_get_child(ext->content, NULL);

    while(page != NULL && i != id) {
        i++;
        page = lv_obj_get_child(ext->content, page);
    }

    if(i == id) return page;

    return NULL;
}

/**
 * Get the tab button matrix (lv_btnm) of a Tab view
 * @param tabview pointer to Tab view object
 * @return pointer to button matrix (lv_btnm) object which is the tab buttons
 */
lv_obj_t * lv_tabview_get_tabs(lv_obj_t * tabview)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext(tabview);
    return ext->tabs;
}

/**
 * Get the indicator rectangle (lv_obj) of a Tab view
 * @param tabview pointer to Tab view object
 * @return pointer to Base object (lv_obj) which is the indicator rectangle on the tab buttons
 */
lv_obj_t * lv_tabview_get_indic(lv_obj_t * tabview)
{
    lv_tabview_ext_t  * ext = lv_obj_get_ext(tabview);
    return ext->indic;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Realign and resize the elements of Tab view
 * @param tabview pointer to a Tab view object
 */
void lv_tabview_realign(lv_obj_t * tabview)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext(tabview);

    lv_obj_set_width(ext->tabs, lv_obj_get_width(tabview));
    lv_obj_set_height(ext->content, lv_obj_get_height(tabview) - lv_obj_get_height(ext->tabs));
    lv_obj_align(ext->content, ext->tabs, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    lv_obj_t * pages = lv_obj_get_child(ext->content, NULL);
    while(pages != NULL) {
        lv_obj_set_size(pages, lv_obj_get_width(tabview), lv_obj_get_height(tabview) - lv_obj_get_height(ext->tabs));
        pages = lv_obj_get_child(ext->content, pages);
    }

    if(ext->tab_cnt != 0) {
        lv_style_t * style_tabs = lv_obj_get_style(ext->tabs);
        cord_t indic_width = (lv_obj_get_width(tabview) - style_tabs->body.pad_obj * (ext->tab_cnt - 1) -
                2 * style_tabs->body.pad_hor) / ext->tab_cnt;
        lv_obj_set_width(ext->indic, indic_width);
    }

    lv_obj_align(ext->indic, ext->tabs, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

    lv_tabview_set_act(tabview, ext->tab_act, false);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if 0 /*Unused*/
/**
 * Handle the drawing related tasks of the tabs
 * @param tab pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_tab_design(lv_obj_t * tab, const area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}
#endif

/**
 * Signal function of a tab's page
 * @param tab pointer to a tab page object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
static bool tabpage_signal(lv_obj_t * tab_page, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_page_signal(tab_page, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        lv_obj_t * cont = lv_obj_get_parent(tab_page);
        lv_obj_t * tab = lv_obj_get_parent(cont);
        if(sign == LV_SIGNAL_PRESSED) {
            tabpage_pressed_hadler(tab, tab_page);
        }
        else if(sign == LV_SIGNAL_PRESSING) {
            tabpage_pressing_hadler(tab, tab_page);
        }
        else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
            tabpage_press_lost_hadler(tab, tab_page);
        }

    }

    return valid;
}
/**
 * Signal function of the tab page's scrollable object
 * @param tab_scrl pointer to a tab page's scrollable object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
static bool tabscrl_signal(lv_obj_t * tab_scrl, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = page_scrl_signal(tab_scrl, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        lv_obj_t * tab_page = lv_obj_get_parent(tab_scrl);
        lv_obj_t * cont = lv_obj_get_parent(tab_page);
        lv_obj_t * tab = lv_obj_get_parent(cont);
        if(sign == LV_SIGNAL_PRESSED) {
            tabpage_pressed_hadler(tab, tab_page);
        }
        else if(sign == LV_SIGNAL_PRESSING) {
            tabpage_pressing_hadler(tab, tab_page);
        }
        else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
            tabpage_press_lost_hadler(tab, tab_page);
        }

    }

    return valid;
}

/**
 * Called when a tab's page or scrollable object is pressed
 * @param tabview pointer to the tab view object
 * @param tabpage pointer to the page of a tab
 */
static void tabpage_pressed_hadler(lv_obj_t * tabview, lv_obj_t * tabpage)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext(tabview);
    lv_indev_t * indev = lv_indev_get_act();
    lv_indev_get_point(indev, &ext->point_last);
}

/**
 * Called when a tab's page or scrollable object is being pressed
 * @param tabview pointer to the tab view object
 * @param tabpage pointer to the page of a tab
 */
static void tabpage_pressing_hadler(lv_obj_t * tabview, lv_obj_t * tabpage)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext(tabview);
    lv_indev_t * indev = lv_indev_get_act();
    point_t point_act;
    lv_indev_get_point(indev, &point_act);
    cord_t x_diff = point_act.x - ext->point_last.x;
    cord_t y_diff = point_act.y - ext->point_last.y;

    if(ext->draging == 0) {
        if(x_diff >= LV_INDEV_DRAG_LIMIT || x_diff<= -LV_INDEV_DRAG_LIMIT) {
            ext->drag_h = 1;
            ext->draging = 1;
            lv_obj_set_drag(lv_page_get_scrl(tabpage), false);
        } else if(y_diff >= LV_INDEV_DRAG_LIMIT || y_diff <= -LV_INDEV_DRAG_LIMIT) {
            ext->drag_h = 0;
            ext->draging = 1;
        }
    }
    if(ext->drag_h != 0) {
        lv_obj_set_x(ext->content, lv_obj_get_x(ext->content) + point_act.x - ext->point_last.x);
        ext->point_last.x = point_act.x;
        ext->point_last.y = point_act.y;

        /*Move the indicator*/
        cord_t indic_width = lv_obj_get_width(ext->indic);
        lv_style_t * tabs_style = lv_obj_get_style(ext->tabs);
        lv_style_t * indic_style = lv_obj_get_style(ext->indic);
        cord_t p = ((tabpage->cords.x1 - tabview->cords.x1) * (indic_width + tabs_style->body.pad_obj)) / lv_obj_get_width(tabview);

        lv_obj_set_x(ext->indic, indic_width * ext->tab_act + tabs_style->body.pad_obj * ext->tab_act + indic_style->body.pad_hor - p);
    }
}

/**
 * Called when a tab's page or scrollable object is released or the press id lost
 * @param tabview pointer to the tab view object
 * @param tabpage pointer to the page of a tab
 */
static void tabpage_press_lost_hadler(lv_obj_t * tabview, lv_obj_t * tabpage)
{
    lv_tabview_ext_t * ext = lv_obj_get_ext(tabview);
    ext->drag_h = 0;
    ext->draging = 0;

    lv_obj_set_drag(lv_page_get_scrl(tabpage), true);

    lv_indev_t * indev = lv_indev_get_act();
    point_t point_act;
    lv_indev_get_point(indev, &point_act);
    cord_t x_diff = point_act.x - ext->point_last.x;
    cord_t x_predict = 0;

    while(x_diff != 0)   {
        x_predict += x_diff;
        x_diff = x_diff * (100 - LV_INDEV_DRAG_THROW) / 100;
    }


    cord_t page_x1 = tabpage->cords.x1 + x_predict;
    cord_t page_x2 = tabpage->cords.x2 + x_predict;

    if(page_x1 > (tabview->cords.x2 - tabview->cords.x1) / 2) {
        if(ext->tab_act != 0) ext->tab_act--;
    } else if(page_x2 < (tabview->cords.x2 - tabview->cords.x1) / 2) {
        if(ext->tab_act < ext->tab_cnt - 1) ext->tab_act++;
    }

    lv_tabview_set_act(tabview, ext->tab_act, true);
}

/**
 * Called when a tab button is released
 * @param tab_btnm pointer to the tab's button matrix object
 * @param id the id of the tab (>= 0)
 * @return LV_ACTION_RES_OK because the button matrix in not deleted in the function
 */
static lv_action_res_t tab_btnm_action(lv_obj_t * tab_btnm, uint16_t id)
{
    lv_obj_t * tab = lv_obj_get_parent(tab_btnm);
    lv_tabview_ext_t * ext = lv_obj_get_ext(tab);
    ext->tab_act = id;
    lv_tabview_set_act(tab, id, true);

    return LV_ACTION_RES_OK;
}

#endif
