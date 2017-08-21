/**
 * @file lv_tab.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_TAB != 0

#include "lv_tab.h"
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
static bool lv_tab_design(lv_obj_t * tab, const area_t * mask, lv_design_mode_t mode);

static bool lv_tab_page_signal(lv_obj_t * tab_page, lv_signal_t sign, void * param);
static bool lv_tab_scrl_signal(lv_obj_t * tab_scrl, lv_signal_t sign, void * param);

static void page_pressed_hadler(lv_obj_t * tab, lv_dispi_t * dispi);
static void page_pressing_hadler(lv_obj_t * tab, lv_obj_t * tab_page, lv_dispi_t * dispi);
static void page_press_lost_hadler(lv_obj_t * tab, lv_obj_t * tab_page, lv_dispi_t * dispi);
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
 * Create a tab objects
 * @param par pointer to an object, it will be the parent of the new tab
 * @param copy pointer to a tab object, if not NULL then the new object will be copied from it
 * @return pointer to the created tab
 */
lv_obj_t * lv_tab_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of tab*/
    lv_obj_t * new_tab = lv_obj_create(par, copy);
    dm_assert(new_tab);
    
    /*Allocate the tab type specific extended data*/
    lv_tab_ext_t * ext = lv_obj_alloc_ext(new_tab, sizeof(lv_tab_ext_t));
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
    lv_obj_set_signal_f(new_tab, lv_tab_signal);

    /*Init the new tab tab*/
    if(copy == NULL) {
        lv_obj_set_size(new_tab, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_style(new_tab, lv_style_get(LV_STYLE_PRETTY, NULL));

        ext->tabs = lv_btnm_create(new_tab, NULL);
        lv_btnm_set_map(ext->tabs, tab_def);
        lv_obj_set_height(ext->tabs, 3 * LV_DPI / 4);
        lv_btnm_set_action(ext->tabs, tab_btnm_action);
        lv_btnm_set_tgl(ext->tabs, true, 0);

        ext->indic = lv_obj_create(ext->tabs, NULL);
        lv_style_t * style_indic = lv_obj_get_style(ext->indic);
        lv_obj_set_size(ext->indic, LV_DPI, style_indic->line_width);

        ext->content = lv_cont_create(new_tab, NULL);
        lv_cont_set_fit(ext->content, true, false);
        lv_cont_set_layout(ext->content, LV_CONT_LAYOUT_ROW_T);
        lv_obj_set_height(ext->content, LV_VER_RES);
        lv_obj_set_style(ext->content, lv_style_get(LV_STYLE_TRANSP_TIGHT, NULL));
        lv_obj_align(ext->content, ext->tabs, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);


    }
    /*Copy an existing tab*/
    else {
    	lv_tab_ext_t * copy_ext = lv_obj_get_ext(copy);

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_tab);
    }
    
    return new_tab;
}

/**
 * Signal function of the tab
 * @param tab pointer to a tab object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_tab_signal(lv_obj_t * tab, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_obj_signal(tab, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	if(sign == LV_SIGNAL_CLEANUP) {
            /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    	} else if(sign == LV_SIGNAL_CORD_CHG) {
            lv_tab_ext_t * ext = lv_obj_get_ext(tab);
            if(ext->content != NULL &&
               (lv_obj_get_width(tab) != area_get_width(param) ||
                lv_obj_get_height(tab) != area_get_height(param))) {

                lv_obj_set_width(ext->tabs, lv_obj_get_width(tab));
                lv_obj_set_height(ext->content, lv_obj_get_height(tab) - lv_obj_get_height(ext->tabs));
                lv_obj_t * pages = lv_obj_get_child(ext->content, NULL);
                while(pages != NULL) {
                    lv_obj_set_size(pages, lv_obj_get_width(tab), lv_obj_get_height(tab) - lv_obj_get_height(ext->tabs));
                    pages = lv_obj_get_child(ext->content, pages);
                }

                if(ext->tab_num != 0) {
                    lv_style_t * style_tabs = lv_obj_get_style(ext->tabs);
                    cord_t indic_width = (lv_obj_get_width(tab) - style_tabs->opad * (ext->tab_num - 1) - 2 * style_tabs->hpad) / ext->tab_num;
                    lv_obj_set_width(ext->indic, indic_width);
                }
            }
    	}
    }
    
    return valid;
}

/*=====================
 * Setter functions
 *====================*/
void lv_tab_set_act(lv_obj_t * tab, uint16_t id)
{
    lv_tab_ext_t * ext = lv_obj_get_ext(tab);
    lv_style_t * style = lv_obj_get_style(ext->content);

    anim_t a;
    a.var = ext->content;
    a.start = lv_obj_get_x(ext->content);
    a.end = -(lv_obj_get_width(tab) * id + style->opad * id + style->hpad);
    a.fp = (anim_fp_t)lv_obj_set_x;
    a.path = anim_get_path(ANIM_PATH_LIN);
    a.end_cb = NULL;
    a.act_time = 0;
    a.time = 300;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 0;
    a.repeat_pause = 0;
    anim_create(&a);

    /*Move the indicator*/
    cord_t indic_width = lv_obj_get_width(ext->indic);
    lv_style_t * indic_style = lv_obj_get_style(ext->indic);
    a.var = ext->indic;
    a.start = lv_obj_get_x(ext->indic);
    a.end = indic_width * id + indic_style->opad * id + indic_style->hpad;
    a.fp = (anim_fp_t)lv_obj_set_x;
    a.path = anim_get_path(ANIM_PATH_LIN);
    a.end_cb = NULL;
    a.act_time = 0;
    a.time = 300;
    a.playback = 0;
    a.playback_pause = 0;
    a.repeat = 0;
    a.repeat_pause = 0;
    anim_create(&a);

    lv_btnm_set_tgl(ext->tabs, true, ext->tab_act);

}

/*=====================
 * Getter functions
 *====================*/

/*
 * New object specific "get" function comes here
 */
/*=====================
 * Other functions
 *====================*/

lv_obj_t * lv_tab_add(lv_obj_t * tab, const char * name)
{
    lv_tab_ext_t  * ext = lv_obj_get_ext(tab);

    lv_obj_t * h = lv_page_create(ext->content, NULL);
    lv_obj_set_size(h, lv_obj_get_width(tab), lv_obj_get_height(tab));
    lv_obj_set_style(h, lv_style_get(LV_STYLE_PRETTY_COLOR, NULL));
    lv_obj_set_signal_f(h, lv_tab_page_signal);
    lv_page_set_sb_mode(h, LV_PAGE_SB_MODE_AUTO);
    if(page_scrl_signal == NULL) page_scrl_signal = lv_obj_get_signal_f(lv_page_get_scrl(h));
    lv_obj_set_signal_f(lv_page_get_scrl(h), lv_tab_scrl_signal);


    lv_obj_t * l = lv_label_create(h, NULL);
    lv_label_set_text(l, name);

    ext->tab_num++;
    ext->tab_name_ptr = dm_realloc(ext->tab_name_ptr, sizeof(char *) * (ext->tab_num + 1));
    ext->tab_name_ptr[ext->tab_num - 1] = name;
    ext->tab_name_ptr[ext->tab_num] = "";

    lv_btnm_set_map(ext->tabs, ext->tab_name_ptr);

    lv_style_t * style_tabs = lv_obj_get_style(ext->tabs);
    cord_t indic_width = (lv_obj_get_width(tab) - style_tabs->opad * (ext->tab_num - 1) - 2 * style_tabs->hpad) / ext->tab_num;
    lv_obj_set_width(ext->indic, indic_width);
    lv_obj_align(ext->indic, NULL, LV_ALIGN_IN_BOTTOM_LEFT, style_tabs->hpad, - style_tabs->opad);

    return h;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


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


static bool lv_tab_page_signal(lv_obj_t * tab_page, lv_signal_t sign, void * param)
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
            page_pressed_hadler(tab, param);
        }
        else if(sign == LV_SIGNAL_PRESSING) {
            page_pressing_hadler(tab, tab_page, param);
        }
        else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
            page_press_lost_hadler(tab, tab_page, param);
        }

    }

    return valid;
}

static bool lv_tab_scrl_signal(lv_obj_t * tab_scrl, lv_signal_t sign, void * param)
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
            page_pressed_hadler(tab, param);
        }
        else if(sign == LV_SIGNAL_PRESSING) {
            page_pressing_hadler(tab, tab_page, param);
        }
        else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
            page_press_lost_hadler(tab, tab_page, param);
        }

    }

    return valid;
}
static void page_pressed_hadler(lv_obj_t * tab, lv_dispi_t * dispi)
{
    lv_tab_ext_t * ext = lv_obj_get_ext(tab);
    lv_dispi_get_point(dispi, &ext->point_last);
}

static void page_pressing_hadler(lv_obj_t * tab, lv_obj_t * tab_page, lv_dispi_t * dispi)
{
    lv_tab_ext_t * ext = lv_obj_get_ext(tab);
    point_t point_act;
    lv_dispi_get_point(dispi, &point_act);
    cord_t x_diff = point_act.x - ext->point_last.x;
    cord_t y_diff = point_act.y - ext->point_last.y;

    if(ext->draging == 0) {
        if(x_diff >= LV_DISPI_DRAG_LIMIT || x_diff<= -LV_DISPI_DRAG_LIMIT) {
            ext->drag_h = 1;
            ext->draging = 1;
            lv_obj_set_drag(lv_page_get_scrl(tab_page), false);
        } else if(y_diff >= LV_DISPI_DRAG_LIMIT || y_diff <= -LV_DISPI_DRAG_LIMIT) {
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
        cord_t p = (tab_page->cords.x1 * indic_width) / lv_obj_get_width(tab);
        lv_style_t * indic_style = lv_obj_get_style(ext->indic);
        lv_obj_set_x(ext->indic,  indic_width * ext->tab_act - p + indic_style -> opad - ext->tab_act + indic_style->hpad);
    }

}

static void page_press_lost_hadler(lv_obj_t * tab, lv_obj_t * tab_page, lv_dispi_t * dispi)
{
    lv_tab_ext_t * ext = lv_obj_get_ext(tab);
    ext->drag_h = 0;
    ext->draging = 0;

    lv_obj_set_drag(lv_page_get_scrl(tab_page), true);

    point_t point_act;
    lv_dispi_get_point(dispi, &point_act);
    cord_t x_diff = point_act.x - ext->point_last.x;
    cord_t x_predict = 0;

    while(x_diff != 0)   {
        x_predict += x_diff;
        x_diff = x_diff * (100 - LV_DISPI_DRAG_THROW) / 100;
    }

    printf("predict: %d\n", x_predict);

    cord_t page_x1 = tab_page->cords.x1 + x_predict;
    cord_t page_x2 = tab_page->cords.x2 + x_predict;

    if(page_x1 > (tab->cords.x2 - tab->cords.x1) / 2) {
        printf("Right\n");
        if(ext->tab_act != 0) ext->tab_act--;
    } else if(page_x2 < (tab->cords.x2 - tab->cords.x1) / 2) {
        printf("Left\n");
        if(ext->tab_act < ext->tab_num - 1) ext->tab_act++;
    }

    lv_tab_set_act(tab, ext->tab_act);

}

static lv_action_res_t tab_btnm_action(lv_obj_t * tab_btnm, uint16_t id)
{
    lv_obj_t * tab = lv_obj_get_parent(tab_btnm);
    lv_tab_ext_t * ext = lv_obj_get_ext(tab);
    ext->tab_act = id;
    lv_tab_set_act(tab, id);

    return LV_ACTION_RES_OK;
}

#endif
