/**
 * @file lv_win.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_WIN != 0

#include "lv_win.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if 0 /*Not used*/
static bool lv_win_design(lv_obj_t * win, const area_t * mask, lv_design_mode_t mode);
#endif
static void lv_win_realign(lv_obj_t * win);

/**********************
 *  STATIC VARIABLES
 **********************/

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
 * Create a window objects
 * @param par pointer to an object, it will be the parent of the new window
 * @param copy pointer to a window object, if not NULL then the new object will be copied from it
 * @return pointer to the created window
 */
lv_obj_t * lv_win_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor object*/
    lv_obj_t * new_win = lv_obj_create(par, copy);
    dm_assert(new_win);
    
    /*Allocate the object type specific extended data*/
    lv_win_ext_t * ext = lv_obj_allocate_ext_attr(new_win, sizeof(lv_win_ext_t));
    dm_assert(ext);
    ext->page = NULL;
    ext->btnh = NULL;
    ext->header = NULL;
    ext->title = NULL;
    ext->style_header = lv_style_get(LV_STYLE_PLAIN_COLOR);
    ext->style_cbtn_rel = lv_style_get(LV_STYLE_BUTTON_ON_RELEASED);
    ext->style_cbtn_pr = lv_style_get(LV_STYLE_BUTTON_ON_PRESSED);
    ext->cbtn_size = ( LV_DPI) / 2;

    /*Init the new window object*/
    if(copy == NULL) {
        lv_obj_set_size(new_win, LV_HOR_RES, LV_VER_RES);
        lv_obj_set_pos(new_win, 0, 0);
        lv_obj_set_style(new_win, lv_style_get(LV_STYLE_PLAIN));

        ext->page = lv_page_create(new_win, NULL);
        lv_obj_set_protect(ext->page, LV_PROTECT_PARENT);
        lv_obj_set_style(ext->page, lv_style_get(LV_STYLE_PLAIN));
        lv_page_set_sb_mode(ext->page, LV_PAGE_SB_MODE_AUTO);

        lv_obj_t * scrl = lv_page_get_scrl(ext->page);
        lv_cont_set_fit(scrl, false, true);
        lv_obj_set_style(scrl, lv_style_get(LV_STYLE_TRANSPARENT));

    	/*Create a holder for the header*/
    	ext->header = lv_cont_create(new_win, NULL);
    	lv_cont_set_fit(ext->header, false, true);
    	/*Move back the header because it is automatically moved to the scrollable */
    	lv_obj_set_protect(ext->header, LV_PROTECT_PARENT);
    	lv_obj_set_parent(ext->header, new_win);
    	lv_obj_set_style(ext->header, lv_style_get(LV_STYLE_PLAIN_COLOR));

    	/*Create a title on the header*/
    	ext->title = lv_label_create(ext->header, NULL);
    	lv_label_set_text(ext->title,"My title");

    	/*Create a holder for the control buttons*/
    	ext->btnh = lv_cont_create(ext->header, NULL);
    	lv_cont_set_fit(ext->btnh, true, false);
        lv_obj_set_style(ext->btnh, lv_style_get(LV_STYLE_TRANSPARENT_TIGHT));
    	lv_cont_set_layout(ext->btnh, LV_CONT_LAYOUT_ROW_M);

        lv_obj_set_signal_func(new_win, lv_win_signal);
        lv_obj_set_size(new_win, LV_HOR_RES, LV_VER_RES);
    }
    /*Copy an existing object*/
    else {
    	lv_win_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    	/*Create the objects*/
    	ext->header = lv_cont_create(new_win, copy_ext->header);
    	ext->title = lv_label_create(ext->header, copy_ext->title);
    	ext->btnh = lv_cont_create(ext->header, copy_ext->btnh);

    	/*Copy the control buttons*/
    	lv_obj_t * child;
    	lv_obj_t * cbtn;
    	child = lv_obj_get_child(copy_ext->btnh, NULL);
    	while(child != NULL) {
    		cbtn = lv_btn_create(ext->btnh, child);
    		lv_img_create(cbtn, lv_obj_get_child(child, NULL));
    		child = lv_obj_get_child(copy_ext->btnh, child);
    	}

        lv_obj_set_signal_func(new_win, lv_win_signal);
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_win);
    }
    
    lv_win_realign(new_win);

    return new_win;
}

/**
 * Signal function of the window
 * @param win pointer to a window object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_win_signal(lv_obj_t * win, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_obj_signal(win, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {

        if(sign == LV_SIGNAL_CHILD_CHG) { /*Move children to the page*/
            lv_obj_t * page = lv_win_get_page(win);
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
        }
         else if(sign == LV_SIGNAL_STYLE_CHG) {
            /*Refresh the style of all control buttons*/
//    	    lv_win_ext_t * ext = lv_obj_get_ext(win);
//    	    lv_style_t * style = lv_obj_get_style(win);
//    	    lv_obj_t * child;
//    			child = lv_obj_get_child(ext->ctrl_holder, NULL);
//    			while(child != NULL) {
//    				lv_obj_set_style(child, &style->ctrl_btn);
//
//    				/*Refresh the image style too*/
//    				lv_obj_set_style(lv_obj_get_child(child, NULL), &style->ctrl_img);
//    				child = lv_obj_get_child(ext->ctrl_holder, child);
//    			}

            lv_win_realign(win);
    	}
        else if(sign == LV_SIGNAL_CORD_CHG) {
            /*If the size is changed refresh the window*/
            if(area_get_width(param) != lv_obj_get_width(win) ||
               area_get_height(param) != lv_obj_get_height(win)) {
                lv_win_realign(win);
            }
        }
    }
    
    return valid;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Add control button to the header of the window
 * @param win pointer to a window object
 * @param img_path path of an image on the control button
 * @param rel_action a function pointer to call when the button is released
 * @return pointer to the created button object
 */
lv_obj_t * lv_win_add_cbtn(lv_obj_t * win, const char * img_path, lv_action_t rel_action)
{
	lv_win_ext_t * ext = lv_obj_get_ext_attr(win);

	lv_obj_t * btn = lv_btn_create(ext->btnh, NULL);
    lv_btn_set_style(btn, LV_BTN_STATE_OFF_RELEASED, ext->style_cbtn_rel);
    lv_btn_set_style(btn, LV_BTN_STATE_OFF_PRESSED, ext->style_cbtn_pr);
	lv_obj_set_size(btn, ext->cbtn_size, ext->cbtn_size);
	lv_btn_set_action(btn, LV_BTN_ACTION_RELEASE, rel_action);

	lv_obj_t * img = lv_img_create(btn, NULL);
	lv_obj_set_click(img, false);
	lv_img_set_file(img, img_path);

	lv_win_realign(win);

	return btn;
}

/**
 * A release action which can be assigned to a window control button to close it
 * @param btn pointer to the released button
 * @return always LV_ACTION_RES_INV because the button is deleted with the window
 */
lv_action_res_t lv_win_close_action(lv_obj_t * btn)
{
	lv_obj_t * win = lv_win_get_from_cbtn(btn);

	lv_obj_del(win);

	return LV_ACTION_RES_INV;
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
 * @return control button size
 */
void lv_win_set_cbtn_size(lv_obj_t * win, cord_t size)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    ext->cbtn_size = size;

    lv_win_realign(win);
}

/**
 * Set the styles of the window  control buttons in a given state
 * @param win pointer to a window object
 * @param rel pointer to the style in released state
 * @param pr pointer to the style in pressed state
 */
void lv_win_set_styles_cbtn(lv_obj_t * win, lv_style_t *  rel, lv_style_t *  pr)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    ext->style_cbtn_rel = rel;
    ext->style_cbtn_pr = pr;
    lv_obj_t * cbtn;
    cbtn = lv_obj_get_child(ext->btnh, NULL);
    while(cbtn != NULL) {
        lv_btn_set_style(cbtn, LV_BTN_STATE_OFF_RELEASED, ext->style_cbtn_rel);
        lv_btn_set_style(cbtn, LV_BTN_STATE_OFF_PRESSED, ext->style_cbtn_pr);

        cbtn = lv_obj_get_child(ext->btnh, cbtn);
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
const char * lv_win_get_title(lv_obj_t * win)
{
	lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
	return lv_label_get_text(ext->title);
}

/**
 * Get the page of a window
 * @param win pointer to a window object
 * @return page pointer to the page object of the window
 */
lv_obj_t * lv_win_get_page(lv_obj_t * win)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    return ext->page;
}

/**
 * Get the s window header
 * @param win pointer to a window object
 * @return pointer to the window header object (lv_rect)
 */
lv_obj_t * lv_win_get_header(lv_obj_t * win)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    return ext->header;
}

/**
 * Get the control button size of a window
 * @param win pointer to a window object
 * @return control button size
 */
cord_t lv_win_get_cbtn_size(lv_obj_t * win)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    return ext->cbtn_size;
}

/**
 * Get width of the content area (page scrollable) of the window
 * @param win pointer to a window object
 * @return the width of the content area
 */
cord_t lv_win_get_width(lv_obj_t * win)
{
    lv_win_ext_t * ext = lv_obj_get_ext_attr(win);
    lv_obj_t * scrl = lv_page_get_scrl(ext->page);
    lv_style_t * style_scrl = lv_obj_get_style(scrl);

    return lv_obj_get_width(scrl) - 2 * style_scrl->body.padding.horizontal;
}

/**
 * Get the pointer of a widow from one of  its control button.
 * It is useful in the action of the control buttons where only button is known.
 * @param ctrl_btn pointer to a control button of a window
 * @return pointer to the window of 'ctrl_btn'
 */
lv_obj_t * lv_win_get_from_cbtn(lv_obj_t * ctrl_btn)
{
	lv_obj_t * ctrl_holder = lv_obj_get_parent(ctrl_btn);
	lv_obj_t * header = lv_obj_get_parent(ctrl_holder);
	lv_obj_t * win = lv_obj_get_parent(header);

	return win;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if 0 /*Not used*/
/**
 * Handle the drawing related tasks of the windows
 * @param win pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_win_design(lv_obj_t * win, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return false;
    } else if (mode == LV_DESIGN_DRAW_MAIN) {
        /*Draw the object*/

    } else if (mode == LV_DESIGN_DRAW_POST) {
        /*Draw after all children is drawn*/

    }



    return true;
}
#endif

/**
 * Realign the building elements of a window
 * @param win pointer to window objectker
 */
static void lv_win_realign(lv_obj_t * win)
{
	lv_win_ext_t * ext = lv_obj_get_ext_attr(win);

	if(ext->page == NULL || ext->btnh == NULL || ext->header == NULL || ext->title == NULL) return;

    lv_obj_t * cbtn;
	/*Refresh the size of all control buttons*/
	cbtn = lv_obj_get_child(ext->btnh, NULL);
	while(cbtn != NULL) {
		lv_obj_set_size(cbtn, ext->cbtn_size, ext->cbtn_size);
		cbtn = lv_obj_get_child(ext->btnh, cbtn);
	}

	lv_style_t * btnh_style = lv_obj_get_style(ext->btnh);
	lv_obj_set_height(ext->btnh, ext->cbtn_size + 2 * btnh_style->body.padding.vertical * 2);
	lv_obj_set_width(ext->header, lv_obj_get_width(win));

	/*Align the higher object first to make the correct header size first*/
	if(lv_obj_get_height(ext->title) > lv_obj_get_height(ext->btnh)) {
		lv_obj_align(ext->title, NULL, LV_ALIGN_IN_LEFT_MID, ext->style_header->body.padding.horizontal, 0);
		lv_obj_align(ext->btnh, NULL, LV_ALIGN_IN_RIGHT_MID, - ext->style_header->body.padding.horizontal, 0);
	} else {
		lv_obj_align(ext->btnh, NULL, LV_ALIGN_IN_RIGHT_MID, - ext->style_header->body.padding.horizontal, 0);
		lv_obj_align(ext->title, NULL, LV_ALIGN_IN_LEFT_MID, ext->style_header->body.padding.horizontal, 0);
	}

	lv_obj_set_pos_scale(ext->header, 0, 0);

    lv_obj_t * page = lv_win_get_page(win);
	lv_obj_set_size(page, lv_obj_get_width(win), lv_obj_get_height(win) - lv_obj_get_height(ext->header));
	lv_obj_align(page, ext->header, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

	lv_style_t * style_page = lv_obj_get_style(page);
    lv_obj_t * scrl = lv_page_get_scrl(page);

    lv_obj_set_width(scrl, lv_obj_get_width(page) - 2 * style_page->body.padding.horizontal);

}
#endif
