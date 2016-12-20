/**
 * @file lv_mbox.c
 * 
 */


/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_MBOX != 0

#include "lv_mbox.h"
#include "../lv_misc/anim.h"
/*********************
 *      DEFINES
 *********************/
#define LV_MBOX_CLOSE_FADE_TIME	 750	/*ms*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if 0 /*Unused*/
static bool lv_mbox_design(lv_obj_t * mbox, const area_t * mask, lv_design_mode_t mode);
#endif
static void lv_temps_init(void);
static void lv_mbox_realign(lv_obj_t * mbox);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_mboxs_t lv_mboxs_def;	/*Default message box style*/

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
 * Create a message box objects
 * @param par pointer to an object, it will be the parent of the new message box
 * @param copy pointer to a message box object, if not NULL then the new object will be copied from it
 * @return pointer to the created message box
 */
lv_obj_t * lv_mbox_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor message box*/
	lv_obj_t * new_mbox = lv_rect_create(par, copy);
    dm_assert(new_mbox);
    
    /*Allocate the message box type specific extended data*/
    lv_mbox_ext_t * ext = lv_obj_alloc_ext(new_mbox, sizeof(lv_mbox_ext_t));
    dm_assert(ext);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_f(new_mbox, lv_mbox_signal);

    /* Let the design function of rect
     lv_obj_set_design_f(new_mbox, lv_mbox_design); */

    /*Init the new message box message box*/
    if(copy == NULL) {
    	lv_rect_set_layout(new_mbox, LV_RECT_LAYOUT_COL_L);
    	lv_rect_set_fit(new_mbox, true, true);

    	ext->title = lv_label_create(new_mbox, NULL);
    	lv_label_set_text(ext->title, "MESSAGE BOX");

    	ext->txt = lv_label_create(new_mbox, NULL);
    	lv_label_set_text(ext->txt, "Text of the message box");

    	ext->btnh = lv_rect_create(new_mbox, NULL);
    	lv_rect_set_fit(ext->btnh, false, true);
    	lv_rect_set_layout(ext->btnh, LV_RECT_LAYOUT_PRETTY);

    	lv_obj_set_style(new_mbox, lv_mboxs_get(LV_MBOXS_DEF, NULL));

    	lv_mbox_realign(new_mbox);
    }
    /*Copy an existing message box*/
    else {
    	lv_mbox_ext_t * copy_ext = lv_obj_get_ext(copy);
    	/*TODO*/
    }
    
    return new_mbox;
}

/**
 * Signal function of the message box
 * @param mbox pointer to a message box object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_mbox_signal(lv_obj_t * mbox, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_rect_signal(mbox, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);
    	lv_mboxs_t * style = lv_obj_get_style(mbox);
    	lv_obj_t * btn;

    	switch(sign) {
    		case LV_SIGNAL_CLEANUP:
    			/*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    			break;
    		case LV_SIGNAL_CORD_CHG:
    			/*If the size is changed refresh the message box*/
    			if(area_get_width(param) != lv_obj_get_width(mbox) ||
    			   area_get_height(param) != lv_obj_get_height(mbox)) {
    				lv_mbox_realign(mbox);
    			}
    			break;
    		case LV_SIGNAL_STYLE_CHG:
    			lv_obj_set_style(ext->title, &style->title);
    			lv_obj_set_style(ext->txt, &style->txt);
    			lv_obj_set_style(ext->btnh, &style->btnh);

    			/*Refresh all the buttons*/
    			btn = lv_obj_get_child(ext->btnh, NULL);
    			while(btn != NULL) {
    				/*Refresh the next button's style*/
    				lv_obj_set_style(btn, &style->btn);
    				lv_obj_set_size(btn, style->btn_w, style->btn_h);

    				/*Refresh the button label too*/
    				lv_obj_set_style(lv_obj_get_child(btn, NULL), &style->btn_label);
        			btn = lv_obj_get_child(ext->btnh, NULL);
    			}

    			/*Hide the title and/or buttons*/
    			lv_obj_set_hidden(ext->title, style->hide_title == 0 ? false : true);

    			if(style->hide_btns != 0 || lv_obj_get_child_num(ext->btnh) != 0) {
    				lv_obj_set_hidden(ext->btnh, true);
    			} else {
    				lv_obj_set_hidden(ext->btnh, false);
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
 * Add a button to the message box
 * @param mbox pointer to message box object
 * @param btn_txt the text of the button
 * @param rel_action a function which will be called when the button is relesed
 * @return pointer to the created button (lv_btn)
 */
lv_obj_t * lv_mbox_add_btn(lv_obj_t * mbox, const char * btn_txt, lv_action_t rel_action)
{
	lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);
	lv_mboxs_t * style = lv_obj_get_style(mbox);

	lv_obj_t * btn;
	btn = lv_btn_create(ext->btnh, NULL);
	lv_btn_set_rel_action(btn, rel_action);
	lv_obj_set_style(btn, &style->btn);
	lv_obj_set_size(btn, style->btn_w, style->btn_h);

	lv_obj_t * label;
	label = lv_label_create(btn, NULL);
	lv_obj_set_style(label, &style->btn_label);
	lv_label_set_text(label, btn_txt);

	/*With 1 button set center layout but from 3 set grid*/
	uint16_t child_num = lv_obj_get_child_num(ext->btnh);
	if(child_num == 1) {
		lv_rect_set_layout(ext->btnh, LV_RECT_LAYOUT_CENTER);
	} else if (child_num == 2) {
		lv_rect_set_layout(ext->btnh, LV_RECT_LAYOUT_PRETTY);
	}

	return btn;
}

/**
 * A release action which can be assigned to a message box button to close it
 * @param btn pointer to the released button
 * @param dispi pointer to the caller display input
 * @return always false because the button is deleted with the mesage box
 */
bool lv_mbox_close_action(lv_obj_t * btn, lv_dispi_t * dispi)
{
	lv_obj_t * mbox = lv_mbox_get_from_btn(btn);

	lv_obj_del(mbox);

	return false;
}

/**
 * Automatically delete the message box after a given time
 * @param mbox pointer to a message box object
 * @param tout a time (in milliseconds) to wait before delete the message box
 */
void lv_mbox_auto_close(lv_obj_t * mbox, uint16_t tout)
{

	lv_obj_anim(mbox, LV_ANIM_FADE | ANIM_OUT, LV_MBOX_CLOSE_FADE_TIME, tout, lv_obj_del);
}


/**
 * Set the title of the message box
 * @param mbox pointer to a message box
 * @param title a '\0' terminated character string which will be the message box title
 */
void lv_mbox_set_title(lv_obj_t * mbox, const char * title)
{
	lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);

	lv_label_set_text(ext->title, title);
}

/**
 * Set the text of the message box
 * @param mbox pointer to a message box
 * @param txt a '\0' terminated character string which will be the message box text
 */
void lv_mbox_set_txt(lv_obj_t * mbox, const char * txt)
{
	lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);

	lv_label_set_text(ext->txt, txt);
}


/*=====================
 * Getter functions
 *====================*/

/**
 * get the title of the message box
 * @param mbox pointer to a message box object
 * @return pointer to the title of the message box
 */
const char * lv_mbox_get_title(lv_obj_t * mbox)
{
	lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);

	return lv_label_get_text(ext->title);
}

/**
 * Get the text of the message box
 * @param mbox pointer to a message box object
 * @return pointer to the text of the message box
 */
const char * lv_mbox_get_txt(lv_obj_t * mbox)
{
	lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);

	return lv_label_get_text(ext->txt);
}

/**
 * Get the message box object from one of its button.
 * It is useful in the button release actions where only the button is known
 * @param btn pointer to a button of a message box
 * @return pointer to the button's message box
 */
lv_obj_t * lv_mbox_get_from_btn(lv_obj_t * btn)
{
	lv_obj_t * btnh = lv_obj_get_parent(btn);
	lv_obj_t * mbox = lv_obj_get_parent(btnh);

	return mbox;
}


/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_mboxs_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_mboxs_t style
 */
lv_mboxs_t * lv_mboxs_get(lv_mboxs_builtin_t style, lv_mboxs_t * copy)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_temps_init();
		style_inited = true;
	}

	lv_mboxs_t  *style_p;

	switch(style) {
		case LV_MBOXS_DEF:
		case LV_MBOXS_INFO:
		case LV_MBOXS_WARN:
		case LV_MBOXS_ERR:
		case LV_MBOXS_BUBBLE:
			style_p = &lv_mboxs_def;
			break;
		default:
			style_p = &lv_mboxs_def;
	}

	if(copy != NULL) {
		if(style_p != NULL) memcpy(copy, style_p, sizeof(lv_mboxs_t));
		else memcpy(copy, &lv_mboxs_def, sizeof(lv_mboxs_t));
	}

	return style_p;
}



/**********************
 *   STATIC FUNCTIONS
 **********************/

#if 0 /*Not used*/
/**
 * Handle the drawing related tasks of the message boxs
 * @param mbox pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_mbox_design(lv_obj_t * mbox, const area_t * mask, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
    	/*Return false if the object is not covers the mask_p area*/
    	return false;
    }


    /*Draw the object*/

    return true;
}
#endif

/**
 * Initialize the message box styles
 */
static void lv_temps_init(void)
{
	/*Default style*/
	lv_rects_get(LV_RECTS_DEF, &lv_mboxs_def.bg);
	lv_mboxs_def.bg.light = 10 * LV_DOWNSCALE;

	lv_btns_get(LV_BTNS_DEF, &lv_mboxs_def.btn);
	lv_mboxs_def.btn.flags[LV_BTN_STATE_PR].light_en = 0;
	lv_mboxs_def.btn.flags[LV_BTN_STATE_REL].light_en = 0;
	lv_labels_get(LV_LABELS_TITLE, &lv_mboxs_def.title);
	lv_labels_get(LV_LABELS_TXT, &lv_mboxs_def.txt);
	lv_labels_get(LV_LABELS_BTN, &lv_mboxs_def.btn_label);
	lv_rects_get(LV_RECTS_TRANSP, &lv_mboxs_def.btnh);
	lv_mboxs_def.btnh.hpad = 0;
	lv_mboxs_def.btnh.vpad = 0;

	lv_mboxs_def.btn_w = 80 * LV_DOWNSCALE;
	lv_mboxs_def.btn_h = 50 * LV_DOWNSCALE;
	lv_mboxs_def.hide_btns = 0;
	lv_mboxs_def.hide_title = 0;

	/*TODO add further styles*/
}

/**
 * Realign the elements of the message box
 * @param mbox pointer to message box object
 */
static void lv_mbox_realign(lv_obj_t * mbox)
{
	lv_mbox_ext_t * ext = lv_obj_get_ext(mbox);
	lv_mboxs_t * style = lv_obj_get_style(mbox);

	if(ext->btnh == NULL || ext->title == NULL || ext->txt == NULL) return;

	lv_obj_set_width(ext->btnh, lv_obj_get_width(mbox) - 2 * style->bg.hpad);

	lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
}


#endif
