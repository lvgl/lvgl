/**
 * @file lv_rect.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_LABEL != 0

#include "misc/others/color.h"
#include "lv_label.h"
#include "../lv_obj/lv_obj.h"
#include "../lv_misc/text.h"
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_label_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_labels_t lv_labels_def = {
  .font = LV_FONT_DEFAULT, .objs.color = COLOR_MAKE(0x10, 0x18, 0x20),
  .letter_space = 2 * LV_STYLE_MULT, .line_space =  2 * LV_STYLE_MULT,
  .mid =  1
};

static lv_labels_t lv_labels_btn = {
  .font = LV_FONT_DEFAULT, .objs.color = COLOR_MAKE(0xd0, 0xe0, 0xf0),
  .letter_space = 2 * LV_STYLE_MULT, .line_space =  2 * LV_STYLE_MULT,
  .mid =  1,
};

static lv_labels_t lv_labels_title = {
  .font = LV_FONT_DEFAULT, .objs.color = COLOR_MAKE(0x10, 0x20, 0x30),
  .letter_space = 4 * LV_STYLE_MULT, .line_space =  4 * LV_STYLE_MULT,
  .mid =  0,
};

static lv_labels_t lv_labels_txt = {
  .font = LV_FONT_DEFAULT, .objs.color = COLOR_MAKE(0x16, 0x23, 0x34),
  .letter_space = 1 * LV_STYLE_MULT, .line_space =  2 * LV_STYLE_MULT,
  .mid =  0,
};



/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a label objects
 * @param par_dp pointer to an object, it will be the parent of the new label
 * @param copy_dp pointer to a button object, if not NULL then the new object will be copied from it
 * @return pointer to the created button
 */
lv_obj_t* lv_label_create(lv_obj_t* par_dp, lv_obj_t * ori_dp)
{
    /*Create a basic object*/
    lv_obj_t* new_obj = lv_obj_create(par_dp, ori_dp);
    dm_assert(new_obj);
    
    /*Extend the basic object to a label object*/
    lv_obj_alloc_ext(new_obj, sizeof(lv_label_ext_t));
    
    lv_label_ext_t * label_p = lv_obj_get_ext(new_obj);
    label_p->txt_dp = NULL;

	lv_obj_set_design_f(new_obj, lv_label_design);
	lv_obj_set_signal_f(new_obj, lv_label_signal);

    /*Init the new label*/
    if(ori_dp == NULL) {
		lv_obj_set_opa(new_obj, OPA_COVER);
		lv_obj_set_click(new_obj, false);
		lv_obj_set_style(new_obj, &lv_labels_def);
		lv_label_set_fixw(new_obj, false);
		lv_label_set_text(new_obj, "Text");
    }
    /*Copy 'ori_dp' if not NULL*/
    else {
		lv_label_set_fixw(new_obj, lv_label_get_fixw(ori_dp));
		lv_label_set_text(new_obj, lv_label_get_text(ori_dp));
    }
    return new_obj;
}


/**
 * Signal function of the label
 * @param obj_dp pointer to a label object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_label_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_obj_signal(obj_dp, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        lv_label_ext_t * label_p = lv_obj_get_ext(obj_dp);
        /*No signal handling*/
    	switch(sign) {
            case LV_SIGNAL_CLEANUP:
                dm_free(label_p->txt_dp);
                label_p->txt_dp = NULL;
                break;
            case LV_SIGNAL_STYLE_CHG:
            	lv_label_set_text(obj_dp, lv_label_get_text(obj_dp));
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
 * Set a new text for a label
 * @param obj_dp pointer to a label object
 * @param text '\0' terminated character string
 */
void lv_label_set_text(lv_obj_t * obj_dp, const char * text)
{
	if(text == NULL) return;

    lv_obj_inv(obj_dp);
    
    lv_label_ext_t * ext_p = lv_obj_get_ext(obj_dp);
    uint32_t len = strlen(text) + 1;

    if(ext_p->txt_dp != NULL && text != ext_p->txt_dp) {
        dm_free(ext_p->txt_dp);
    }
    
    ext_p->txt_dp = dm_alloc(len);
    strcpy(ext_p->txt_dp, text);
    
    uint32_t line_start = 0;
    uint32_t new_line_start = 0;
    cord_t max_length = lv_obj_get_width(obj_dp);
    lv_labels_t * labels_p = lv_obj_get_style(obj_dp);
    const font_t * font_p = font_get(labels_p->font);
    uint8_t letter_height = font_get_height(font_p);
    cord_t new_height = 0;
    cord_t longest_line = 0;
    cord_t act_line_length;
    
    /*If the fix width is not enabled the set the max length to very big */
    if(ext_p->fixw == 0) {
        max_length = LV_CORD_MAX;
    }
    
    /*Calc. the height and longest line*/;
    while (text[line_start] != '\0')
    {
        new_line_start += txt_get_next_line(&text[line_start], font_p, labels_p->letter_space, max_length);
        new_height += letter_height;
        new_height += labels_p->line_space;
        
        /*If no fix width then calc. the longest line */
        if(ext_p->fixw == false) {
          act_line_length = txt_get_length(&text[line_start], new_line_start - line_start,
                                           font_p, labels_p->letter_space);
          if(act_line_length > longest_line) {
              longest_line = act_line_length;
          }
        }
        
        line_start = new_line_start;
    }
    
    /*Correction with the last line space*/
    new_height -= labels_p->line_space;
    
    lv_obj_set_height(obj_dp, new_height);

    /*Refresh the length if no fix width*/
    if(ext_p->fixw == 0) {
        lv_obj_set_width(obj_dp, longest_line);
    }
    
    lv_obj_t * parent_dp = lv_obj_get_parent(obj_dp);
    parent_dp->signal_f(parent_dp, LV_SIGNAL_CHILD_CHG, obj_dp);

    lv_obj_inv(obj_dp);
}

/**
 * Set the fix width attribute
 * @param obj_dp pointer to a label object
 * @param fixw true: enable fix width for the label
 */
void lv_label_set_fixw(lv_obj_t * obj_dp, bool fixw)
{
    lv_label_ext_t * ext_p = lv_obj_get_ext(obj_dp);
    ext_p->fixw = fixw == false ? 0 : 1;
}

/*=====================
 * Getter functions 
 *====================*/

/**
 * Get the text of a label
 * @param obj_dp pointer to a label object
 * @return the text of the label
 */
const char * lv_label_get_text(lv_obj_t* obj_dp)
{
    lv_label_ext_t * label_p = lv_obj_get_ext(obj_dp);
    
    return label_p->txt_dp;
}

/**
 * Get the fix width attribute of a label
 * @param obj_dp pointer to a label object
 * @return true: fix width is enabled
 */
bool lv_label_get_fixw(lv_obj_t * obj_dp)
{
    lv_label_ext_t * ext_p = lv_obj_get_ext(obj_dp);
    return ext_p->fixw == 0 ? false: true;
}

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_labels_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_labels_t style
 */
lv_labels_t * lv_labels_get(lv_labels_builtin_t style, lv_labels_t * copy_p)
{
	lv_labels_t * style_p;

	switch(style) {
		case LV_LABELS_DEF:
			style_p = &lv_labels_def;
			break;
		case LV_LABELS_BTN:
			style_p = &lv_labels_btn;
			break;
		case LV_LABELS_TXT:
			style_p = &lv_labels_txt;
			break;
		case LV_LABELS_TITLE:
			style_p = &lv_labels_title;
			break;
		default:
			style_p = &lv_labels_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_labels_t));
		else memcpy(copy_p, &lv_labels_def, sizeof(lv_labels_t));
	}

	return style_p;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the labels
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'
 */
static bool lv_label_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    /* A label never covers an area */
    if(mode == LV_DESIGN_COVER_CHK) return false;
    
    /*TEST: draw a background for the label*/
    /*lv_vfill(&obj_dp->cords, mask_p, COLOR_LIME, OPA_COVER);*/

    area_t cords;
    lv_obj_get_cords(obj_dp, &cords);
    opa_t opa= lv_obj_get_opa(obj_dp);
    const char * txt = lv_label_get_text(obj_dp);

    lv_draw_label(&cords, mask_p, lv_obj_get_style(obj_dp), opa, txt);

    return true;
}

#endif
