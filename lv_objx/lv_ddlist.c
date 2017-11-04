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
#include "../lv_obj/lv_indev.h"
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
static bool lv_ddlist_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param);
static lv_res_t lv_ddlist_rel_action(lv_obj_t * ddlist);
static void lv_ddlist_refr_size(lv_obj_t * ddlist, uint16_t anim_time);
static void lv_ddlist_pos_current_option(lv_obj_t * ddlist);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t  ancestor_signal;
static lv_signal_func_t  ancestor_scrl_signal;
static lv_design_func_t  ancestor_design;
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
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_ddlist);
    if(ancestor_scrl_signal == NULL) ancestor_scrl_signal = lv_obj_get_signal_func(lv_page_get_scrl(new_ddlist));
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_ddlist);
    
    /*Allocate the drop down list type specific extended data*/
    lv_ddlist_ext_t * ext = lv_obj_allocate_ext_attr(new_ddlist, sizeof(lv_ddlist_ext_t));
    dm_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->options_label = NULL;
    ext->callback = NULL;
    ext->opened = 0;
    ext->fix_height = 0;
    ext->selected_option_id = 0;
    ext->option_cnt = 0;
    ext->anim_time = LV_DDLIST_DEF_ANIM_TIME;
    ext->selected_style = &lv_style_plain_color;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_ddlist, lv_ddlist_signal);
    lv_obj_set_signal_func(lv_page_get_scrl(new_ddlist), lv_ddlist_scrl_signal);
    lv_obj_set_design_func(new_ddlist, lv_ddlist_design);

    /*Init the new drop down list drop down list*/
    if(copy == NULL) {
        lv_obj_t * scrl = lv_page_get_scrl(new_ddlist);
        lv_obj_set_drag(scrl, false);
        lv_page_set_scrl_fit(new_ddlist, true, true);

        ext->options_label = lv_label_create(new_ddlist, NULL);
        lv_cont_set_fit(new_ddlist, true, false);
        lv_page_set_release_action(new_ddlist, lv_ddlist_rel_action);
        lv_page_set_sb_mode(new_ddlist, LV_PAGE_SB_MODE_DRAG);
        lv_ddlist_set_style(new_ddlist, &lv_style_pretty, NULL, &lv_style_plain_color);
        lv_ddlist_set_options(new_ddlist, def_options);
    }
    /*Copy an existing drop down list*/
    else {
    	lv_ddlist_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->options_label = lv_label_create(new_ddlist, copy_ext->options_label);
        lv_label_set_text(ext->options_label, lv_label_get_text(copy_ext->options_label));
        ext->selected_option_id = copy_ext->selected_option_id;
        ext->fix_height = copy_ext->fix_height;
        ext->callback = copy_ext->callback;
        ext->option_cnt = copy_ext->option_cnt;
        ext->selected_style = copy_ext->selected_style;
        ext->anim_time = copy_ext->anim_time;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_ddlist);
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
    valid = ancestor_signal(ddlist, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        if(sign == LV_SIGNAL_STYLE_CHG) {
            lv_ddlist_refr_size(ddlist, 0);
            lv_obj_t *scrl = lv_page_get_scrl(ddlist);
            lv_obj_refresh_ext_size(scrl);  /*Because of the wider selected rectangle*/
        } else if(sign == LV_SIGNAL_FOCUS) {
            lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
            if(ext->opened == false) {
                ext->opened = true;
                lv_ddlist_refr_size(ddlist, true);
            }
        } else if(sign == LV_SIGNAL_DEFOCUS) {
            lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
            if(ext->opened != false) {
                ext->opened = false;
                lv_ddlist_refr_size(ddlist, true);
            }
        } else if(sign == LV_SIGNAL_CONTROLL) {
            lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
            char c = *((char*)param);
            if(c == LV_GROUP_KEY_RIGHT || c == LV_GROUP_KEY_DOWN) {
                if(ext->selected_option_id +1 < ext->option_cnt) {
                    ext->selected_option_id ++;
                    lv_obj_invalidate(ddlist);
                    if(ext->callback != NULL) {
                        ext->callback(ddlist);
                    }
                }
            } else if(c == LV_GROUP_KEY_LEFT || c == LV_GROUP_KEY_UP) {
                if(ext->selected_option_id > 0) {
                    ext->selected_option_id --;
                    lv_obj_invalidate(ddlist);
                    if(ext->callback != NULL) {
                        ext->callback(ddlist);
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
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    lv_label_set_text(ext->options_label, "");
    uint16_t i = 0;
    while(options[i][0] != '\0') {
        lv_label_ins_text(ext->options_label, LV_LABEL_POS_LAST, options[i]);
        if(options[i + 1][0] != '\0') lv_label_ins_text(ext->options_label, LV_LABEL_POS_LAST, "\n");
        i++;
    }

    ext->option_cnt = i;

    lv_ddlist_refr_size(ddlist, 0);
}

/**
 * Set the options in a drop down list from a string
 * @param ddlist pointer to drop down list object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
void lv_ddlist_set_options_str(lv_obj_t * ddlist, const char * options)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    /*Count the '\n'-s to determine the number of options*/
    ext->option_cnt = 0;
    uint16_t i;
    for(i = 0; options[i] != '\0'; i++) {
        if(options[i] == '\n') ext->option_cnt++;
    }
    ext->option_cnt++;     /*Last option in the at row*/

    lv_label_set_text(ext->options_label, options);
    lv_ddlist_refr_size(ddlist, 0);
}

/**
 * Set the selected option
 * @param ddlist pointer to drop down list object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 */
void lv_ddlist_set_selected(lv_obj_t * ddlist, uint16_t sel_opt)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    ext->selected_option_id = sel_opt < ext->option_cnt ? sel_opt : ext->option_cnt - 1;

    /*Move the list to show the current option*/
    if(ext->opened == 0) {
        lv_ddlist_pos_current_option(ddlist);
    } else {
        lv_obj_invalidate(ddlist);
    }
}

/**
 * Set a function to call when a new option is chosen
 * @param ddlist pointer to a drop down list
 * @param cb pointer to a call back function
 */
void lv_ddlist_set_action(lv_obj_t * ddlist, lv_action_t action)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    ext->callback = action;
}

/**
 * Set the fix height value.
 * If 0 then the opened ddlist will be auto. sized else the set height will be applied.
 * @param ddlist pointer to a drop down list
 * @param h the height when the list is opened (0: auto size)
 */
void lv_ddlist_set_fix_height(lv_obj_t * ddlist, cord_t h)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
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
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    ext->anim_time = anim_time;
}

/**
 * Open the drop down list with or without animation
 * @param ddlist pointer to drop down list object
 * @param anim true: use animation; false: not use animations
 */
void lv_ddlist_open(lv_obj_t * ddlist, bool anim)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    ext->opened = 1;
    lv_obj_set_drag(lv_page_get_scrl(ddlist), true);
    lv_ddlist_refr_size(ddlist, anim ? ext->anim_time : 0);
}

/**
 * Close (Collapse) the drop down list
 * @param ddlist pointer to drop down list object
 * @param anim true: use animation; false: not use animations
 */
void lv_ddlist_close(lv_obj_t * ddlist, bool anim)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    ext->opened = 0;
    lv_obj_set_drag(lv_page_get_scrl(ddlist), false);
    lv_ddlist_refr_size(ddlist, anim ? ext->anim_time : 0);
}

/**
 * Set the style of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param bg pointer to the new style of the background
 * @param sb pointer to the new style of the scrollbars (only visible with fix height)
 * @param sel pointer to the new style of the select rectangle
 */
void lv_ddlist_set_style(lv_obj_t * ddlist, lv_style_t *bg, lv_style_t *sb, lv_style_t *sel)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    ext->selected_style = sel;
    lv_obj_set_style(ext->options_label, bg);

    lv_page_set_style(ddlist, bg, &lv_style_transp_tight, sb);

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
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    return lv_label_get_text(ext->options_label);
}

/**
 * Get the selected option
 * @param ddlist pointer to drop down list object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t lv_ddlist_get_selected(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    return ext->selected_option_id;
}

/**
 * Get the current selected option as a string
 * @param ddlist pointer to ddlist object
 * @param buf pointer to an array to store the string
 */
void lv_ddlist_get_selected_str(lv_obj_t * ddlist, char * buf)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    uint16_t i;
    uint16_t line = 0;
    const char * opt_txt = lv_label_get_text(ext->options_label);
    uint16_t txt_len = strlen(opt_txt);
    
    
    for(i = 0; i < txt_len && line != ext->selected_option_id; i++) {
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
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    return ext->fix_height;
}

/**
 * Get the style of the rectangle on the selected option
 * @param ddlist pointer to a drop down list object
 * @return pointer the style of the select rectangle
 */
lv_style_t * lv_ddlist_get_style_select(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    if(ext->selected_style == NULL) return lv_obj_get_style(ddlist);

    return ext->selected_style;
}
/**
 * Get the open/close animation time.
 * @param ddlist pointer to a drop down list
 * @return open/close animation time [ms]
 */
uint16_t lv_ddlist_get_anim_time(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
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
    	return ancestor_design(ddlist, mask, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design(ddlist, mask, mode);

        /*If the list is opened draw a rectangle under the selected item*/
        lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
        if(ext->opened != 0) {
            lv_style_t *style = lv_ddlist_get_style_bg(ddlist);
            lv_obj_t *scrl = lv_page_get_scrl(ddlist);
            const font_t * font = style->text.font;
            cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
            area_t rect_area;
            rect_area.y1 = ext->options_label->coords.y1;
            rect_area.y1 += ext->selected_option_id * (font_h + style->text.line_space);
            rect_area.y1 -= style->text.line_space / 2;

            rect_area.y2 = rect_area.y1 + font_h + style->text.line_space;
            rect_area.x1 = scrl->coords.x1 - (style->body.padding.hor >> 1);    /*Draw a littlebit wider rectangle then the text*/
            rect_area.x2 = scrl->coords.x2 + (style->body.padding.hor >> 1);

            lv_draw_rect(&rect_area, mask, ext->selected_style);
        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design(ddlist, mask, mode);
    }

    return true;
}

/**
 * Signal function of the drop down list's scrollable part
 * @param scrl pointer to a drop down list's scrollable part
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
static bool lv_ddlist_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = ancestor_scrl_signal(scrl, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        if(sign == LV_SIGNAL_REFR_EXT_SIZE) {
            /* Because of the wider selected rectangle ext. size
             * In this way by dragging the scrollable part the wider rectangle area will be redrawn too*/
            lv_obj_t *ddlist = lv_obj_get_parent(scrl);
            lv_style_t *style = lv_ddlist_get_style_bg(ddlist);
            if(scrl->ext_size < (style->body.padding.hor >> 1)) scrl->ext_size = style->body.padding.hor >> 1;
        }
    }

    return valid;
}

/**
 * Called when a drop down list is released to open it or set new option
 * @param ddlist pointer to a drop down list object
 * @return LV_ACTION_RES_INV if the ddlist it deleted in the user callback else LV_ACTION_RES_OK
 */
static lv_res_t lv_ddlist_rel_action(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    if(ext->opened == 0) { /*Open the list*/
        ext->opened = 1;
        lv_obj_set_drag(lv_page_get_scrl(ddlist), true);
    } else {
        ext->opened = 0;
        lv_obj_set_drag(lv_page_get_scrl(ddlist), false);

        /*Search the clicked option*/
        lv_indev_t *indev = lv_indev_get_act();
        point_t p;
        lv_indev_get_point(indev, &p);
        p.x -= ext->options_label->coords.x1;
        p.y -= ext->options_label->coords.y1;
        uint16_t letter_i;
        letter_i = lv_label_get_letter_on(ext->options_label, &p);

        uint16_t new_opt = 0;
        const char * txt = lv_label_get_text(ext->options_label);
        uint16_t i;
        for(i = 0; i < letter_i; i++) {
            if(txt[i] == '\n') new_opt ++;
        }

        ext->selected_option_id = new_opt;

        if(ext->callback != NULL) {
            ext->callback(ddlist);
        }
    }
    lv_ddlist_refr_size(ddlist, ext->anim_time);

    return LV_RES_OK;

}

/**
 * Refresh the size of drop down list according to its status (open or closed)
 * @param ddlist pointer to a drop down list object
 * @param anim_time animations time for open/close [ms]
 */
static void lv_ddlist_refr_size(lv_obj_t * ddlist, uint16_t anim_time)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    lv_style_t * style = lv_obj_get_style(ddlist);
    cord_t new_height;
    if(ext->opened) { /*Open the list*/
        if(ext->fix_height == 0) new_height = lv_obj_get_height(lv_page_get_scrl(ddlist)) + 2 * style->body.padding.ver;
        else new_height = ext->fix_height;
    } else { /*Close the list*/
        const font_t * font = style->text.font;
        lv_style_t * label_style = lv_obj_get_style(ext->options_label);
        cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
        new_height = font_h + 2 * label_style->text.line_space;
    }
    if(anim_time == 0) {
        lv_obj_set_height(ddlist, new_height);
        lv_ddlist_pos_current_option(ddlist);
    } else {
        anim_t a;
        a.var = ddlist;
        a.start = lv_obj_get_height(ddlist);
        a.end = new_height;
        a.fp = (anim_fp_t)lv_obj_set_height;
        a.path = anim_get_path(ANIM_PATH_LIN);
        a.end_cb = (anim_cb_t)lv_ddlist_pos_current_option;
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
static void lv_ddlist_pos_current_option(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    lv_style_t * style = lv_obj_get_style(ddlist);
    const font_t * font = style->text.font;
    cord_t font_h = font_get_height(font) >> FONT_ANTIALIAS;
    lv_style_t * label_style = lv_obj_get_style(ext->options_label);
    lv_obj_t * scrl = lv_page_get_scrl(ddlist);

    cord_t h = lv_obj_get_height(ddlist);
    cord_t line_y1 = ext->selected_option_id * (font_h + label_style->text.line_space) + ext->options_label->coords.y1 - scrl->coords.y1;

    lv_obj_set_y(scrl, - line_y1 + (h - font_h) / 2);

}

#endif
