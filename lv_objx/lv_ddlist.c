/**
 * @file lv_ddlist.c
 * 
 */


/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_DDLIST != 0

#include "lv_ddlist.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_obj/lv_group.h"
#include "../lv_obj/lv_dispi.h"
#include "misc/gfx/anim.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DDLIST_DEF_ANIM_TIME 200 /*ms*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_ddlist_design(lv_obj_t * ddlist, const area_t * mask, lv_design_mode_t mode);
static lv_action_res_t lv_ddlist_rel_action(lv_obj_t * ddlist, lv_dispi_t * dispi);
static void lv_ddlist_refr_size(lv_obj_t * ddlist, uint16_t anim_time);
static void lv_ddlist_pos_act_option(lv_obj_t * ddlist);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_f_t  ancestor_design_f;
static const char * def_options[] = {"Option 1", "Option 2", "Option 3", ""};
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
 * Create a drop down list objects
 * @param par pointer to an object, it will be the parent of the new drop down list
 * @param copy pointer to a drop down list object, if not NULL then the new object will be copied from it
 * @return pointer to the created drop down list
 */
lv_obj_t * lv_ddlist_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor drop down list*/
    lv_obj_t * new_ddlist = lv_page_create(par, copy);
    dm_assert(new_ddlist);
    
    /*Allocate the drop down list type specific extended data*/
    lv_ddlist_ext_t * ext = lv_obj_alloc_ext(new_ddlist, sizeof(lv_ddlist_ext_t));
    dm_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->opt_label = NULL;
    ext->cb = NULL;
    ext->opened = 0;
    ext->fix_height = 0;
    ext->sel_opt = 0;
    ext->num_opt = 0;
    ext->anim_time = LV_DDLIST_DEF_ANIM_TIME;
    ext->style_sel = lv_style_get(LV_STYLE_PLAIN_COLOR, NULL);

    /*The signal and design functions are not copied so set them here*/
    if(ancestor_design_f == NULL) ancestor_design_f = lv_obj_get_design_f(new_ddlist);

    lv_obj_set_signal_f(new_ddlist, lv_ddlist_signal);
    lv_obj_set_design_f(new_ddlist, lv_ddlist_design);

    /*Init the new drop down list drop down list*/
    if(copy == NULL) {
        lv_obj_t * scrl = lv_page_get_scrl(new_ddlist);
        lv_obj_set_drag(scrl, false);
        lv_obj_set_style(scrl, lv_style_get(LV_STYLE_TRANSP, NULL));
        lv_cont_set_fit(scrl, true, true);

        ext->opt_label = lv_label_create(new_ddlist, NULL);
        lv_cont_set_fit(new_ddlist, true, false);
        lv_page_set_rel_action(new_ddlist, lv_ddlist_rel_action);
        lv_page_set_sb_mode(new_ddlist, LV_PAGE_SB_MODE_DRAG);
        lv_obj_set_style(new_ddlist, lv_style_get(LV_STYLE_PRETTY, NULL));
        lv_ddlist_set_options(new_ddlist, def_options);
    }
    /*Copy an existing drop down list*/
    else {
    	lv_ddlist_ext_t * copy_ext = lv_obj_get_ext(copy);
        ext->opt_label = lv_label_create(new_ddlist, copy_ext->opt_label);
        lv_label_set_text(ext->opt_label, lv_label_get_text(copy_ext->opt_label));
        ext->sel_opt = copy_ext->sel_opt;
        ext->fix_height = copy_ext->fix_height;
        ext->cb = copy_ext->cb;
        ext->num_opt = copy_ext->num_opt;

        /*Refresh the style with new signal function*/
        lv_obj_refr_style(new_ddlist);
    }
    
    return new_ddlist;
}

/**
 * Signal function of the drop down list
 * @param ddlist pointer to a drop down list object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_ddlist_signal(lv_obj_t * ddlist, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_page_signal(ddlist, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	if(sign == LV_SIGNAL_STYLE_CHG) {
            lv_ddlist_refr_size(ddlist, 0);
    	} else if(sign == LV_SIGNAL_FOCUS) {
            lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
    	    if(ext->opened == false) {
    	        ext->opened = true;
    	        lv_ddlist_refr_size(ddlist, true);
    	    }
    	} else if(sign == LV_SIGNAL_DEFOCUS) {
            lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
            if(ext->opened != false) {
                ext->opened = false;
                lv_ddlist_refr_size(ddlist, true);
            }
        } else if(sign == LV_SIGNAL_CONTROLL) {
            lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
            char c = *((char*)param);
            if(c == LV_GROUP_KEY_RIGHT || c == LV_GROUP_KEY_DOWN) {
                if(ext->sel_opt +1 < ext->num_opt) {
                    ext->sel_opt ++;
                    lv_obj_inv(ddlist);
                    if(ext->cb != NULL) {
                        ext->cb(ddlist, NULL);
                    }
                }
            } else if(c == LV_GROUP_KEY_LEFT || c == LV_GROUP_KEY_UP) {
                if(ext->sel_opt > 0) {
                    ext->sel_opt --;
                    lv_obj_inv(ddlist);
                    if(ext->cb != NULL) {
                        ext->cb(ddlist, NULL);
                    }
                }
            } else if(c == LV_GROUP_KEY_ENTER || c == LV_GROUP_KEY_ESC) {
                if(ext->opened != false) ext->opened = false;
                if(ext->opened == false) ext->opened = true;

                lv_ddlist_refr_size(ddlist, true);
            }
        }
    }
    
    return valid;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the options in a drop down list from an array
 * @param ddlist pointer to drop down list object
 * @param options an array of strings with the text of the options.
 *                The lest element has to be "" (empty string)
 *                E.g. const char * opts[] = {"apple", "banana", "orange", ""};
 */
void lv_ddlist_set_options(lv_obj_t * ddlist, const char ** options)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);

    lv_label_set_text(ext->opt_label, "");
    uint16_t i = 0;
    while(options[i][0] != '\0') {
        lv_label_ins_text(ext->opt_label, LV_LABEL_POS_LAST, options[i]);
        if(options[i + 1][0] != '\0') lv_label_ins_text(ext->opt_label, LV_LABEL_POS_LAST, "\n");
        i++;
    }

    ext->num_opt = i;

    lv_ddlist_refr_size(ddlist, 0);
}

/**
 * Set the options in a drop down list from a string
 * @param ddlist pointer to drop down list object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
void lv_ddlist_set_options_str(lv_obj_t * ddlist, const char * options)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);

    /*Count the '\n'-s to determine the number of options*/
    ext->num_opt = 0;
    uint16_t i;
    for(i = 0; options[i] != '\0'; i++) {
        if(options[i] == '\n') ext->num_opt++;
    }
    ext->num_opt++;     /*Last option in the at row*/

    lv_label_set_text(ext->opt_label, options);
    lv_ddlist_refr_size(ddlist, 0);
}

/**
 * Set the selected option
 * @param ddlist pointer to drop down list object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 */
void lv_ddlist_set_selected(lv_obj_t * ddlist, uint16_t sel_opt)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);

    ext->sel_opt = sel_opt < ext->num_opt ? sel_opt : ext->num_opt - 1;

    /*Move the list to show the current option*/
    if(ext->opened == 0) {
        lv_ddlist_pos_act_option(ddlist);
    } else {
        lv_obj_inv(ddlist);
    }
}

/**
 * Set a function to call when a new option is chosen
 * @param ddlist pointer to a drop down list
 * @param cb pointer to a call back function
 */
void lv_ddlist_set_action(lv_obj_t * ddlist, lv_action_t cb)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
    ext->cb = cb;
}

/**
 * Set the fix height value.
 * If 0 then the opened ddlist will be auto. sized else the set height will be applied.
 * @param ddlist pointer to a drop down list
 * @param h the height when the list is opened (0: auto size)
 */
void lv_ddlist_set_fix_height(lv_obj_t * ddlist, cord_t h)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
    ext->fix_height = h;
    lv_ddlist_refr_size(ddlist, 0);
}

/**
 * Set the open/close animation time.
 * @param ddlist pointer to a drop down list
 * @param anim_time: open/close animation time [ms]
 */
void lv_ddlist_set_anim_time(lv_obj_t * ddlist, uint16_t anim_time)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
    ext->anim_time = anim_time;
}

/**
 * Set the style of the rectangle on the selected option
 * @param ddlist pointer to a drop down list object
 * @param style pointer the new style of the select rectangle
 */
void lv_ddlist_set_style_select(lv_obj_t * ddlist, lv_style_t * style)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
    ext->style_sel = style;

}

/**
 * Open or Collapse the drop down list
 * @param ddlist pointer to drop down list object
 * @param state true: open; false: collapse
 * @param anim true: use animations; false: not use animations
 */
void lv_ddlist_open(lv_obj_t * ddlist, bool state, bool anim)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
    ext->opened = state ? 1 : 0;
    lv_ddlist_refr_size(ddlist, anim ? ext->anim_time : 0);

}
/*=====================
 * Getter functions
 *====================*/

/**
 * Get the options of a drop down list
 * @param ddlist pointer to drop down list object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * lv_ddlist_get_options(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
    return lv_label_get_text(ext->opt_label);
}

/**
 * Get the selected option
 * @param ddlist pointer to drop down list object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t lv_ddlist_get_selected(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);

    return ext->sel_opt;
}

/**
 * Get the current selected option as a string
 * @param ddlist pointer to ddlist object
 * @param buf pointer to an array to store the string
 */
void lv_ddlist_get_selected_str(lv_obj_t * ddlist, char * buf)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);

    uint16_t i;
    uint16_t line = 0;
    const char * opt_txt = lv_label_get_text(ext->opt_label);
    uint16_t txt_len = strlen(opt_txt);
    
    
    for(i = 0; i < txt_len && line != ext->sel_opt; i++) {
        if(opt_txt[i] == '\n') line ++;
    }
    
    uint16_t c;
    for(c = 0; opt_txt[i] != '\n' && i < txt_len; c++, i++) buf[c] = opt_txt[i];
    
    buf[c] = '\0';
}

/**
 * Get the fix height value.
 * @param ddlist pointer to a drop down list object
 * @return the height if the ddlist is opened (0: auto size)
 */
cord_t lv_ddlist_get_fix_height(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
    return ext->fix_height;
}

/**
 * Get the style of the rectangle on the selected option
 * @param ddlist pointer to a drop down list object
 * @return pointer the style of the select rectangle
 */
lv_style_t * lv_ddlist_get_style_select(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
    if(ext->style_sel == NULL) return lv_obj_get_style(ddlist);

    return ext->style_sel;
}
/**
 * Get the open/close animation time.
 * @param ddlist pointer to a drop down list
 * @return open/close animation time [ms]
 */
uint16_t lv_ddlist_get_anim_time(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
    return ext->anim_time;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the drop down lists
 * @param ddlist pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_ddlist_design(lv_obj_t * ddlist, const area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	return ancestor_design_f(ddlist, mask, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design_f(ddlist, mask, mode);

        /*If the list is opened draw a rectangle below the selected item*/
        lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
        if(ext->opened != 0) {
            lv_style_t * style = lv_obj_get_style(ddlist);
            const font_t * font = style->font;
            cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
            area_t rect_area;
            rect_area.y1 = ext->opt_label->cords.y1;
            rect_area.y1 += ext->sel_opt * (font_h + style->line_space);
            rect_area.y1 -= style->line_space / 2;

            rect_area.y2 = rect_area.y1 + font_h + style->line_space;
            rect_area.x1 = ext->opt_label->cords.x1 - style->hpad;
            rect_area.x2 = rect_area.x1 + lv_obj_get_width(lv_page_get_scrl(ddlist));

            lv_draw_rect(&rect_area, mask, ext->style_sel);
        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design_f(ddlist, mask, mode);
    }

    return true;
}

/**
 * Called when a drop down list is released to open it or set new option
 * @param ddlist pointer to a drop down list object
 * @param dispi pointer to the called display input
 * @return LV_ACTION_RES_INV if the ddlist it deleted in the user callback else LV_ACTION_RES_OK
 */
static lv_action_res_t lv_ddlist_rel_action(lv_obj_t * ddlist, lv_dispi_t * dispi)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);

    if(ext->opened == 0) { /*Open the list*/
        ext->opened = 1;
        lv_obj_set_drag(lv_page_get_scrl(ddlist), true);
    } else {
        ext->opened = 0;
        lv_obj_set_drag(lv_page_get_scrl(ddlist), false);

        /*Search the clicked option*/
        point_t p;
        lv_dispi_get_point(dispi, &p);
        p.x -= ext->opt_label->cords.x1;
        p.y -= ext->opt_label->cords.y1;
        uint16_t letter_i;
        letter_i = lv_label_get_letter_on(ext->opt_label, &p);

        uint16_t new_opt = 0;
        const char * txt = lv_label_get_text(ext->opt_label);
        uint16_t i;
        for(i = 0; i < letter_i; i++) {
            if(txt[i] == '\n') new_opt ++;
        }

        ext->sel_opt = new_opt;

        if(ext->cb != NULL) {
            ext->cb(ddlist, dispi);
        }
    }
    lv_ddlist_refr_size(ddlist, ext->anim_time);

    return LV_ACTION_RES_OK;

}

/**
 * Refresh the size of drop down list according to its status (open or closed)
 * @param ddlist pointer to a drop down list object
 * @param anim_time animations time for open/close [ms]
 */
static void lv_ddlist_refr_size(lv_obj_t * ddlist, uint16_t anim_time)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
    lv_style_t * style = lv_obj_get_style(ddlist);
    cord_t new_height;
    if(ext->opened) { /*Open the list*/
        if(ext->fix_height == 0) new_height = lv_obj_get_height(lv_page_get_scrl(ddlist)) + 2 * style->vpad;
        else new_height = ext->fix_height;
    } else { /*Close the list*/
        const font_t * font = style->font;
        lv_style_t * label_style = lv_obj_get_style(ext->opt_label);
        cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
        new_height = font_h + 2 * label_style->line_space;
    }
    if(anim_time == 0) {
        lv_obj_set_height(ddlist, new_height);
        lv_ddlist_pos_act_option(ddlist);
    } else {
        anim_t a;
        a.var = ddlist;
        a.start = lv_obj_get_height(ddlist);
        a.end = new_height;
        a.fp = (anim_fp_t)lv_obj_set_height;
        a.path = anim_get_path(ANIM_PATH_LIN);
        a.end_cb = (anim_cb_t)lv_ddlist_pos_act_option;
        a.act_time = 0;
        a.time = ext->anim_time;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;

        anim_create(&a);
    }
}

/**
 * Set the position of list when it is closed to show the selected item
 * @param ddlist pointer to a drop down list
 */
static void lv_ddlist_pos_act_option(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext(ddlist);
    lv_style_t * style = lv_obj_get_style(ddlist);
    const font_t * font = style->font;
    cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
    lv_style_t * label_style = lv_obj_get_style(ext->opt_label);
    lv_obj_t * scrl = lv_page_get_scrl(ddlist);

    cord_t h = lv_obj_get_height(ddlist);
    cord_t line_y1 = ext->sel_opt * (font_h + label_style->line_space) + ext->opt_label->cords.y1 - scrl->cords.y1;

    lv_obj_set_y(scrl, - line_y1 + (h - font_h) / 2);

}

#endif
