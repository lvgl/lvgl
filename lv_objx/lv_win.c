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
static void lv_wins_init(void);
static void lv_win_realign(lv_obj_t * win);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_wins_t lv_wins_def;

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
 * @param copy pointer to a window object, if not NULL then
	lv_win_add_ctrl_btn(app->win, "U:/close", lv_app_win_close_action);
	lv_win_add_ctrl_btn(app->win, "U:/close", lv_app_win_close_action);
	lv_win_add_ctrl_btn(app->win, "U:/close", lv_app_win_close_action);the new object will be copied from it
 * @return pointer to the created window
 */
lv_obj_t * lv_win_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor object*/
    lv_obj_t * new_win = lv_obj_create(par, copy);
    dm_assert(new_win);
    
    /*Allocate the object type specific extended data*/
    lv_win_ext_t * ext = lv_obj_alloc_ext(new_win, sizeof(lv_win_ext_t));
    dm_assert(ext);
    ext->content = NULL;
    ext->ctrl_holder = NULL;
    ext->header = NULL;
    ext->title = NULL;

    lv_obj_set_signal_f(new_win, lv_win_signal);


    /*Init the new window object*/
    if(copy == NULL) {
    	/*Create a page for the content*/
		ext->content = lv_page_create(new_win, NULL);

    	/*Create a holder for the header*/
    	ext->header = lv_rect_create(new_win, NULL);
    	lv_rect_set_fit(ext->header, false, true);

    	/*Create a title on the header*/
    	ext->title = lv_label_create(ext->header, NULL);
    	lv_label_set_text(ext->title,"My title");

    	/*Create a holder for the control buttons*/
    	ext->ctrl_holder = lv_rect_create(ext->header, NULL);
    	lv_rect_set_fit(ext->ctrl_holder, true, false);
    	lv_rect_set_layout(ext->ctrl_holder, LV_RECT_LAYOUT_ROW_M);

    	lv_obj_set_style(new_win, lv_wins_get(LV_WINS_DEF, NULL));

        lv_obj_set_size(new_win, LV_HOR_RES, LV_VER_RES);
    }
    /*Copy an existing object*/
    else {
    	lv_win_ext_t * copy_ext = lv_obj_get_ext(copy);
    	/*Create the objects*/
    	ext->header = lv_rect_create(new_win, copy_ext->header);
    	ext->title = lv_label_create(ext->header, copy_ext->title);
    	ext->ctrl_holder = lv_rect_create(ext->header, copy_ext->ctrl_holder);
    	ext->content = lv_page_create(new_win, copy_ext->content);

    	/*Copy the control buttons*/
    	lv_obj_t * child;
    	lv_obj_t * cbtn;
    	child = lv_obj_get_child(copy_ext->ctrl_holder, NULL);
    	while(child != NULL) {
    		cbtn = lv_btn_create(ext->ctrl_holder, child);
    		lv_img_create(cbtn, lv_obj_get_child(child, NULL));
    		child = lv_obj_get_child(copy_ext->ctrl_holder, child);
    	}

        /*Set the style of 'copy' and isolate it if it is necessary*/
        if(lv_obj_get_style_iso(new_win) == false) {
            lv_obj_set_style(new_win, lv_obj_get_style(copy));
        } else {
            lv_obj_set_style(new_win, lv_obj_get_style(copy));
            lv_obj_iso_style(new_win, sizeof(lv_wins_t));
        }
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
    lv_win_ext_t * ext = lv_obj_get_ext(win);
    lv_wins_t * style = lv_obj_get_style(win);
    lv_obj_t * child;


    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			/*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    			break;
    		case LV_SIGNAL_STYLE_CHG:
    			lv_obj_set_style(ext->content, &style->content);
    			lv_obj_set_style(ext->ctrl_holder, &style->ctrl_holder);
    			lv_obj_set_style(ext->title, &style->title);
    			lv_obj_set_style(ext->header, &style->header);
				lv_obj_set_opa(ext->header, style->header_opa);

				if(style->header_opa == OPA_COVER || style->header_opa == OPA_TRANSP) {
					lv_obj_set_opa_protect(ext->header, false);
				} else {
					lv_obj_set_opa_protect(ext->header, true);
				}

    			/*Refresh the style of all control buttons*/
    			child = lv_obj_get_child(ext->ctrl_holder, NULL);
    			while(child != NULL) {
    				lv_obj_set_style(child, &style->ctrl_btn);
    				lv_obj_set_opa(child, style->ctrl_btn_opa);
    				if(style->ctrl_btn_opa == OPA_COVER || style->ctrl_btn_opa == OPA_TRANSP) {
    					lv_obj_set_opa_protect(child, false);
    				} else {
    					lv_obj_set_opa_protect(child, true);
    				}
    				/*Refresh the image style too*/
    				lv_obj_set_style(lv_obj_get_child(child, NULL), &style->ctrl_img);
    				child = lv_obj_get_child(ext->ctrl_holder, child);
    			}

    			lv_win_realign(win);

    			break;
    		case LV_SIGNAL_CHILD_CHG:
    			/*If a child added move it to the 'content' object*/
    			/*(A window can be only 2 children: the header and the content)*/
    			child = lv_obj_get_child(win, NULL);
    			if(ext->content != NULL &&  ext->header != NULL &&
    			   child != ext->content && child != ext->header &&
				   param != NULL) {
    				lv_obj_set_parent(param, ext->content);
    			}
    			break;
    		case LV_SIGNAL_CORD_CHG:
    			/*If the size is changed refresh the window*/
    			if(area_get_width(param) != lv_obj_get_width(win) ||
    			   area_get_height(param) != lv_obj_get_height(win)) {
    				lv_win_realign(win);
    			}
    			break;
    		default:
    			break;
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
lv_obj_t * lv_win_add_ctrl_btn(lv_obj_t * win, const char * img_path, lv_action_t rel_action)
{
	lv_win_ext_t * ext = lv_obj_get_ext(win);
	lv_wins_t * style = lv_obj_get_style(win);

	lv_obj_t * btn = lv_btn_create(ext->ctrl_holder, NULL);
	lv_obj_set_style(btn, &style->ctrl_btn);
	lv_obj_set_opa(btn, style->ctrl_btn_opa);
	lv_obj_set_size(btn, style->ctrl_btn_w, style->ctrl_btn_h);
	lv_btn_set_rel_action(btn, rel_action);

	if(style->ctrl_btn_opa == OPA_COVER || style->ctrl_btn_opa == OPA_TRANSP) {
		lv_obj_set_opa_protect(btn, false);
	} else {
		lv_obj_set_opa_protect(btn, true);
	}

	lv_obj_t * img = lv_img_create(btn, NULL);
	lv_obj_set_click(img, false);
	lv_obj_set_style(img, &style->ctrl_img);
	lv_img_set_file(img, img_path);

	lv_win_realign(win);

	return btn;
}

/**
 * A release action which can be assigned to a window control button to close it
 * @param btn pointer to the released button
 * @param dispi pointer to the caller display input
 * @return always false because the button is deleted with the window
 */
bool lv_win_close_action(lv_obj_t * btn, lv_dispi_t * dispi)
{
	lv_obj_t * win = lv_win_get_from_ctrl_btn(btn);

	lv_obj_del(win);

	return false;
}

/**
 * Set the title of a window
 * @param win pointer to a window object
 * @param title string of the new title
 */
void lv_win_set_title(lv_obj_t * win, const char * title)
{
	lv_win_ext_t * ext = lv_obj_get_ext(win);

	lv_label_set_text(ext->title, title);
	lv_win_realign(win);
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
	lv_win_ext_t * ext = lv_obj_get_ext(win);

	return lv_label_get_text(ext->title);
}

/**
 * Get the content object (lv_page type) of a window
 * @param win pointer to a window object
 * @return pointer to the content page object of a window
 */
lv_obj_t * lv_win_get_content(lv_obj_t * win)
{
    lv_win_ext_t * ext = lv_obj_get_ext(win);

    return ext->content;
}

/**
 * Get the pointer of a widow from one of  its control button.
 * It is useful in the action of the control buttons where only button is known.
 * @param ctrl_btn pointer to a control button of a window
 * @return pointer to the window of 'ctrl_btn'
 */
lv_obj_t * lv_win_get_from_ctrl_btn(lv_obj_t * ctrl_btn)
{
	lv_obj_t * ctrl_holder = lv_obj_get_parent(ctrl_btn);
	lv_obj_t * header = lv_obj_get_parent(ctrl_holder);
	lv_obj_t * win = lv_obj_get_parent(header);

	return win;
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_wins_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_wins_t style
 */
lv_wins_t * lv_wins_get(lv_wins_builtin_t style, lv_wins_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_wins_init();
		style_inited = true;
	}

	lv_wins_t  *style_p;

	switch(style) {
		case LV_WINS_DEF:
			style_p = &lv_wins_def;
			break;
		default:
			style_p = &lv_wins_def;
	}

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_wins_t));
		else memcpy(copy, &lv_wins_def, sizeof(lv_wins_t));
	}

	return style_p;
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
 * Initialize the window styles
 */
static void lv_wins_init(void)
{
	/*Transparent background. It will be always covered*/
	lv_objs_get(LV_OBJS_TRANSP, &lv_wins_def.bg);

	/*Style for the content*/
	lv_pages_get(LV_PAGES_DEF, &lv_wins_def.content);
	lv_wins_def.content.bg_rects.objs.color = COLOR_WHITE;
	lv_wins_def.content.bg_rects.gcolor = COLOR_WHITE;
	lv_wins_def.content.bg_rects.bwidth = 1 * LV_DOWNSCALE;
	lv_wins_def.content.bg_rects.bcolor = COLOR_GRAY;
	lv_wins_def.content.bg_rects.round = 0;
	lv_wins_def.content.bg_rects.hpad = 0;
	lv_wins_def.content.bg_rects.vpad = 0;
	lv_wins_def.header_on_content = 0;

	/*Styles for the header*/
	lv_rects_get(LV_RECTS_DEF, &lv_wins_def.header);
	lv_wins_def.header.hpad = 5 * LV_DOWNSCALE;
	lv_wins_def.header.vpad = 5 * LV_DOWNSCALE;
	lv_wins_def.header.objs.color = COLOR_MAKE(0x30, 0x40, 0x50);
	lv_wins_def.header.gcolor = COLOR_MAKE(0x30, 0x40, 0x50);
	lv_wins_def.header.bwidth = 0;
	lv_wins_def.header.round = 0;

	lv_rects_get(LV_RECTS_TRANSP, &lv_wins_def.ctrl_holder);
	lv_wins_def.ctrl_holder.hpad = 0;
	lv_wins_def.ctrl_holder.vpad = 0;
	lv_wins_def.ctrl_holder.opad = 10 * LV_DOWNSCALE;

	lv_btns_get(LV_BTNS_DEF, &lv_wins_def.ctrl_btn);
	lv_wins_def.ctrl_btn.bcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0xD0, 0xE0, 0xF0);
	lv_wins_def.ctrl_btn.mcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0x30, 0x40, 0x50);
	lv_wins_def.ctrl_btn.gcolor[LV_BTN_STATE_REL] = COLOR_MAKE(0x30, 0x40, 0x50);
	lv_wins_def.ctrl_btn.rects.bopa = 70;
	lv_wins_def.ctrl_btn.rects.bwidth = 2 * LV_DOWNSCALE;
	lv_wins_def.ctrl_btn.rects.round = LV_RECT_CIRCLE;

	lv_imgs_get(LV_IMGS_DEF, &lv_wins_def.ctrl_img);
	lv_wins_def.ctrl_img.recolor_opa = OPA_50;
	lv_wins_def.ctrl_img.objs.color = COLOR_WHITE;

	lv_labels_get(LV_LABELS_TITLE, &lv_wins_def.title);
	lv_wins_def.title.objs.color = COLOR_MAKE(0xD0, 0xE0, 0xF0);
	lv_wins_def.title.letter_space = 1 * LV_DOWNSCALE;
	lv_wins_def.title.line_space = 1 * LV_DOWNSCALE;

	lv_wins_def.ctrl_btn_w = 30 * LV_DOWNSCALE;
	lv_wins_def.ctrl_btn_h = 30 * LV_DOWNSCALE;

	lv_wins_def.header_opa = OPA_COVER;
	lv_wins_def.ctrl_btn_opa = OPA_COVER;
}

/**
 * Realign the building elements of a window
 * @param win pointer to window objectker
 */
static void lv_win_realign(lv_obj_t * win)
{
	lv_win_ext_t * ext = lv_obj_get_ext(win);
	lv_wins_t * style = lv_obj_get_style(win);

	if(ext->content == NULL || ext->ctrl_holder == NULL || ext->header == NULL || ext->title == NULL) return;

    lv_obj_t * cbtn;
	/*Refresh the style of all control buttons*/
	cbtn = lv_obj_get_child(ext->ctrl_holder, NULL);
	while(cbtn != NULL) {
		lv_obj_set_size(cbtn, style->ctrl_btn_w, style->ctrl_btn_h);
		cbtn = lv_obj_get_child(ext->ctrl_holder, cbtn);
	}

	lv_obj_set_height(ext->ctrl_holder, style->ctrl_btn_h + 2 * style->ctrl_holder.vpad * 2);
	lv_obj_set_width(ext->header, lv_obj_get_width(win));

	/*Align the higher object first to make the correct header size first*/
	if(lv_obj_get_height(ext->title) > lv_obj_get_height(ext->ctrl_holder)) {
		lv_obj_align(ext->title, NULL, LV_ALIGN_IN_LEFT_MID, style->header.hpad, 0);
		lv_obj_align(ext->ctrl_holder, NULL, LV_ALIGN_IN_RIGHT_MID, -style->header.hpad, 0);
	} else {
		lv_obj_align(ext->ctrl_holder, NULL, LV_ALIGN_IN_RIGHT_MID, -style->header.hpad, 0);
		lv_obj_align(ext->title, NULL, LV_ALIGN_IN_LEFT_MID, style->header.hpad, 0);
	}

	lv_obj_set_pos_us(ext->header, 0, 0);

	if(style->header_on_content == 0) {
        lv_obj_set_size(ext->content, lv_obj_get_width(win), lv_obj_get_height(win) - lv_obj_get_height(ext->header));
		lv_obj_align_us(ext->content, ext->header, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
	} else {
        lv_obj_set_size(ext->content, lv_obj_get_width(win), lv_obj_get_height(win));
		lv_obj_set_pos(ext->content, 0, 0);
	}

}
#endif
