/**
 * @file lv_ddlist.c
 * 
 */


/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_DDLIST != 0

#include "lv_ddlist.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_core/lv_group.h"
#include "../lv_core/lv_indev.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_fonts/lv_symbol_def.h"
#include "../lv_misc/lv_anim.h"

/*********************
 *      DEFINES
 *********************/
#if USE_LV_ANIMATION
#  ifndef LV_DDLIST_ANIM_TIME
#    define LV_DDLIST_ANIM_TIME     200     	/*ms*/
#  endif
#else
#  undef  LV_DDLIST_ANIM_TIME
#  define LV_DDLIST_ANIM_TIME     0     		/*No animation*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_ddlist_design(lv_obj_t * ddlist, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_ddlist_signal(lv_obj_t * ddlist, lv_signal_t sign, void * param);
static lv_res_t lv_ddlist_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param);
static lv_res_t lv_ddlist_release_action(lv_obj_t * ddlist);
static void lv_ddlist_refr_size(lv_obj_t * ddlist, bool anim_en);
static void lv_ddlist_pos_current_option(lv_obj_t * ddlist);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t  ancestor_signal;
static lv_signal_func_t  ancestor_scrl_signal;
static lv_design_func_t  ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

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
    lv_mem_assert(new_ddlist);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_ddlist);
    if(ancestor_scrl_signal == NULL) ancestor_scrl_signal = lv_obj_get_signal_func(lv_page_get_scrl(new_ddlist));
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_ddlist);
    
    /*Allocate the drop down list type specific extended data*/
    lv_ddlist_ext_t * ext = lv_obj_allocate_ext_attr(new_ddlist, sizeof(lv_ddlist_ext_t));
    lv_mem_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->label = NULL;
    ext->action = NULL;
    ext->opened = 0;
    ext->fix_height = 0;
    ext->sel_opt_id = 0;
    ext->sel_opt_id_ori = 0;
    ext->option_cnt = 0;
    ext->anim_time = LV_DDLIST_ANIM_TIME;
    ext->sel_style = &lv_style_plain_color;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_ddlist, lv_ddlist_signal);
    lv_obj_set_signal_func(lv_page_get_scrl(new_ddlist), lv_ddlist_scrl_signal);
    lv_obj_set_design_func(new_ddlist, lv_ddlist_design);

    /*Init the new drop down list drop down list*/
    if(copy == NULL) {
        lv_obj_t * scrl = lv_page_get_scrl(new_ddlist);
        lv_obj_set_drag(scrl, false);
        lv_page_set_scrl_fit(new_ddlist, true, true);

        ext->label = lv_label_create(new_ddlist, NULL);
        lv_cont_set_fit(new_ddlist, true, false);
        lv_page_set_rel_action(new_ddlist, lv_ddlist_release_action);
        lv_page_set_sb_mode(new_ddlist, LV_SB_MODE_DRAG);
        lv_page_set_style(new_ddlist, LV_PAGE_STYLE_SCRL, &lv_style_transp_tight);

        lv_ddlist_set_options(new_ddlist, "Option 1\nOption 2\nOption 3");

        /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            lv_ddlist_set_style(new_ddlist, LV_DDLIST_STYLE_BG, th->ddlist.bg);
            lv_ddlist_set_style(new_ddlist, LV_DDLIST_STYLE_SEL,th->ddlist.sel);
            lv_ddlist_set_style(new_ddlist, LV_DDLIST_STYLE_SB, th->ddlist.sb);
        } else {
            lv_ddlist_set_style(new_ddlist, LV_DDLIST_STYLE_BG, &lv_style_pretty);
            lv_ddlist_set_style(new_ddlist, LV_DDLIST_STYLE_SEL, &lv_style_plain_color);
            lv_ddlist_set_style(new_ddlist, LV_DDLIST_STYLE_SB, &lv_style_pretty_color);
        }
    }
    /*Copy an existing drop down list*/
    else {
    	lv_ddlist_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->label = lv_label_create(new_ddlist, copy_ext->label);
        lv_label_set_text(ext->label, lv_label_get_text(copy_ext->label));
        ext->sel_opt_id = copy_ext->sel_opt_id;
        ext->fix_height = copy_ext->fix_height;
        ext->action = copy_ext->action;
        ext->option_cnt = copy_ext->option_cnt;
        ext->sel_style = copy_ext->sel_style;
        ext->anim_time = copy_ext->anim_time;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_ddlist);
    }
    
    return new_ddlist;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the options in a drop down list from a string
 * @param ddlist pointer to drop down list object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
void lv_ddlist_set_options(lv_obj_t * ddlist, const char * options)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    /*Count the '\n'-s to determine the number of options*/
    ext->option_cnt = 0;
    uint16_t i;
    for(i = 0; options[i] != '\0'; i++) {
        if(options[i] == '\n') ext->option_cnt++;
    }
    ext->option_cnt++;     /*Last option in the at row*/

    lv_label_set_text(ext->label, options);
    lv_ddlist_refr_size(ddlist, false);
}

/**
 * Set the selected option
 * @param ddlist pointer to drop down list object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 */
void lv_ddlist_set_selected(lv_obj_t * ddlist, uint16_t sel_opt)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    ext->sel_opt_id = sel_opt < ext->option_cnt ? sel_opt : ext->option_cnt - 1;

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
 * @param action pointer to a call back function
 */
void lv_ddlist_set_action(lv_obj_t * ddlist, lv_action_t action)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    ext->action = action;
}

/**
 * Set the fix height for the drop down list
 * If 0 then the opened ddlist will be auto. sized else the set height will be applied.
 * @param ddlist pointer to a drop down list
 * @param h the height when the list is opened (0: auto size)
 */
void lv_ddlist_set_fix_height(lv_obj_t * ddlist, lv_coord_t h)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    ext->fix_height = h;

    lv_ddlist_refr_size(ddlist, false);
}

/**
 * Enable or disable the horizontal fit to the content
 * @param ddlist pointer to a drop down list
 * @param fit en true: enable auto fit; false: disable auto fit
 */
void lv_ddlist_set_hor_fit(lv_obj_t * ddlist, bool fit_en)
{
    lv_cont_set_fit(ddlist, fit_en, lv_cont_get_ver_fit(ddlist));
    lv_page_set_scrl_fit(ddlist, fit_en, lv_page_get_scrl_fit_ver(ddlist));


    lv_ddlist_refr_size(ddlist, false);
}

/**
 * Set the open/close animation time.
 * @param ddlist pointer to a drop down list
 * @param anim_time: open/close animation time [ms]
 */
void lv_ddlist_set_anim_time(lv_obj_t * ddlist, uint16_t anim_time)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
#if USE_LV_ANIMATION == 0
    anim_time = 0;
#endif

    ext->anim_time = anim_time;
}

/**
 * Set a style of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_ddlist_set_style(lv_obj_t *ddlist, lv_ddlist_style_t type, lv_style_t *style)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    switch (type) {
        case LV_DDLIST_STYLE_BG:
            lv_page_set_style(ddlist, LV_PAGE_STYLE_BG, style);
            break;
        case LV_DDLIST_STYLE_SB:
            lv_page_set_style(ddlist, LV_PAGE_STYLE_SB, style);
            break;
        case LV_DDLIST_STYLE_SEL:
            ext->sel_style = style;
            lv_obj_t *scrl = lv_page_get_scrl(ddlist);
            lv_obj_refresh_ext_size(scrl);  /*Because of the wider selected rectangle*/
            break;
    }
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
    return lv_label_get_text(ext->label);
}

/**
 * Get the selected option
 * @param ddlist pointer to drop down list object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t lv_ddlist_get_selected(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    return ext->sel_opt_id;
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
    const char * opt_txt = lv_label_get_text(ext->label);
    uint16_t txt_len = strlen(opt_txt);
    
    
    for(i = 0; i < txt_len && line != ext->sel_opt_id; i++) {
        if(opt_txt[i] == '\n') line ++;
    }
    
    uint16_t c;
    for(c = 0; opt_txt[i] != '\n' && i < txt_len; c++, i++) buf[c] = opt_txt[i];
    
    buf[c] = '\0';
}

/**
 * Get the "option selected" callback function
 * @param ddlist pointer to a drop down list
 * @return  pointer to the call back function
 */
lv_action_t lv_ddlist_get_action(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    return ext->action;
}

/**
 * Get the fix height value.
 * @param ddlist pointer to a drop down list object
 * @return the height if the ddlist is opened (0: auto size)
 */
lv_coord_t lv_ddlist_get_fix_height(lv_obj_t * ddlist)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    return ext->fix_height;
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


/**
 * Get a style of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_ddlist_get_style(lv_obj_t *ddlist, lv_ddlist_style_t type)
{
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    switch (type) {
        case LV_DDLIST_STYLE_BG:        return lv_page_get_style(ddlist, LV_PAGE_STYLE_BG);
        case LV_DDLIST_STYLE_SB:        return lv_page_get_style(ddlist, LV_PAGE_STYLE_SB);
        case LV_DDLIST_STYLE_SEL:  return ext->sel_style;
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
}
/*=====================
 * Other functions
 *====================*/

/**
 * Open the drop down list with or without animation
 * @param ddlist pointer to drop down list object
 * @param anim_en true: use animation; false: not use animations
 */
void lv_ddlist_open(lv_obj_t * ddlist, bool anim_en)
{
#if USE_LV_ANIMATION == 0
    anim_en = false;
#endif
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    ext->opened = 1;
    lv_obj_set_drag(lv_page_get_scrl(ddlist), true);
    lv_ddlist_refr_size(ddlist, anim_en);
}

/**
 * Close (Collapse) the drop down list
 * @param ddlist pointer to drop down list object
 * @param anim_en true: use animation; false: not use animations
 */
void lv_ddlist_close(lv_obj_t * ddlist, bool anim_en)
{
#if USE_LV_ANIMATION == 0
    anim_en = false;
#endif
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    ext->opened = 0;
    lv_obj_set_drag(lv_page_get_scrl(ddlist), false);
    lv_ddlist_refr_size(ddlist, anim_en);
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
static bool lv_ddlist_design(lv_obj_t * ddlist, const lv_area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	return ancestor_design(ddlist, mask, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design(ddlist, mask, mode);

        lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

        /*If the list is opened draw a rectangle under the selected item*/
        if(ext->opened != 0) {
            lv_style_t *style = lv_ddlist_get_style(ddlist, LV_DDLIST_STYLE_BG);
            const lv_font_t * font = style->text.font;
            lv_coord_t font_h = lv_font_get_height(font);

            /*Draw the selected*/
            lv_area_t rect_area;
            rect_area.y1 = ext->label->coords.y1;
            rect_area.y1 += ext->sel_opt_id * (font_h + style->text.line_space);
            rect_area.y1 -= style->text.line_space / 2;

            rect_area.y2 = rect_area.y1 + font_h + style->text.line_space;
            rect_area.x1 = ddlist->coords.x1;
            rect_area.x2 = ddlist->coords.x2;

            lv_draw_rect(&rect_area, mask, ext->sel_style);
        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design(ddlist, mask, mode);

        /*Redraw the text on the selected area with a different color*/
        lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

        /*Redraw only in opened state*/
        if(ext->opened == 0) return true;

        lv_style_t *style = lv_ddlist_get_style(ddlist, LV_DDLIST_STYLE_BG);
        const lv_font_t * font = style->text.font;
        lv_coord_t font_h = lv_font_get_height(font);

        lv_area_t area_sel;
        area_sel.y1 = ext->label->coords.y1;
        area_sel.y1 += ext->sel_opt_id * (font_h + style->text.line_space);
        area_sel.y1 -= style->text.line_space / 2;

        area_sel.y2 = area_sel.y1 + font_h + style->text.line_space;
        area_sel.x1 = ddlist->coords.x1;
        area_sel.x2 = ddlist->coords.x2;
        lv_area_t mask_sel;
        bool area_ok;
        area_ok = lv_area_union(&mask_sel, mask, &area_sel);
        if(area_ok) {
            lv_style_t *sel_style = lv_ddlist_get_style(ddlist, LV_DDLIST_STYLE_SEL);
            lv_style_t new_style;
            lv_style_copy(&new_style, style);
            new_style.text.color = sel_style->text.color;
            new_style.text.opa = sel_style->text.opa;
            lv_draw_label(&ext->label->coords, &mask_sel, &new_style,
                          lv_label_get_text(ext->label), LV_TXT_FLAG_NONE, NULL);
        }
    }

    return true;
}

/**
 * Signal function of the drop down list
 * @param ddlist pointer to a drop down list object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_ddlist_signal(lv_obj_t * ddlist, lv_signal_t sign, void * param)
{
    lv_res_t res;
    /* Include the ancient signal function */
    res = ancestor_signal(ddlist, sign, param);
    if(res != LV_RES_OK) return res;

    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);

    if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_ddlist_refr_size(ddlist, 0);
    }
    else if(sign == LV_SIGNAL_CLEANUP) {
        ext->label = NULL;
    }
    else if(sign == LV_SIGNAL_FOCUS) {
        if(ext->opened == false) {
            ext->opened = true;
            lv_ddlist_refr_size(ddlist, true);
            ext->sel_opt_id_ori = ext->sel_opt_id;
        }
    }
    else if(sign == LV_SIGNAL_DEFOCUS) {
        if(ext->opened != false) {
            ext->opened = false;
            ext->sel_opt_id = ext->sel_opt_id_ori;
            lv_ddlist_refr_size(ddlist, true);
        }
    }
    else if(sign == LV_SIGNAL_CONTROLL) {
        char c = *((char*)param);
        if(c == LV_GROUP_KEY_RIGHT || c == LV_GROUP_KEY_DOWN) {
            if(!ext->opened) {
                ext->opened = 1;
                lv_ddlist_refr_size(ddlist, true);
            }

            if(ext->sel_opt_id + 1 < ext->option_cnt) {
                ext->sel_opt_id ++;
                lv_ddlist_pos_current_option(ddlist);
                lv_obj_invalidate(ddlist);
            }
        } else if(c == LV_GROUP_KEY_LEFT || c == LV_GROUP_KEY_UP) {
            if(!ext->opened) {
                ext->opened = 1;
                lv_ddlist_refr_size(ddlist, true);
            }
            if(ext->sel_opt_id > 0) {
                ext->sel_opt_id --;
                lv_ddlist_pos_current_option(ddlist);
                lv_obj_invalidate(ddlist);
            }
        } else if(c == LV_GROUP_KEY_ENTER  || c == LV_GROUP_KEY_ENTER_LONG) {
            if(ext->opened) {
                ext->sel_opt_id_ori = ext->sel_opt_id;
                ext->opened = 0;
                if(ext->action) ext->action(ddlist);
            }
            else {
                ext->opened = 1;
            }

            lv_ddlist_refr_size(ddlist, true);
        }
        else if(c == LV_GROUP_KEY_ESC) {
            if(ext->opened) {
                ext->opened = 0;
                lv_ddlist_refr_size(ddlist, true);
            }
        }
    }
    else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_ddlist";
    }

    return res;
}

/**
 * Signal function of the drop down list's scrollable part
 * @param scrl pointer to a drop down list's scrollable part
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_ddlist_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_scrl_signal(scrl, sign, param);
    if(res != LV_RES_OK) return res;

    lv_obj_t *ddlist = lv_obj_get_parent(scrl);

    if(sign == LV_SIGNAL_REFR_EXT_SIZE) {
        /* Because of the wider selected rectangle ext. size
         * In this way by dragging the scrollable part the wider rectangle area can be redrawn too*/
        lv_style_t *style = lv_ddlist_get_style(ddlist, LV_DDLIST_STYLE_BG);
        if(scrl->ext_size < style->body.padding.hor) scrl->ext_size = style->body.padding.hor;
    }
    else if(sign == LV_SIGNAL_CLEANUP) {
        lv_ddlist_ext_t *ext = lv_obj_get_ext_attr(ddlist);
        ext->label = NULL;      /*The label is already deleted*/
    }

    return res;
}

/**
 * Called when a drop down list is released to open it or set new option
 * @param ddlist pointer to a drop down list object
 * @return LV_ACTION_RES_INV if the ddlist it deleted in the user callback else LV_ACTION_RES_OK
 */
static lv_res_t lv_ddlist_release_action(lv_obj_t * ddlist)
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
        lv_point_t p;
        lv_indev_get_point(indev, &p);
        p.x -= ext->label->coords.x1;
        p.y -= ext->label->coords.y1;
        uint16_t letter_i;
        letter_i = lv_label_get_letter_on(ext->label, &p);

        uint16_t new_opt = 0;
        const char * txt = lv_label_get_text(ext->label);
        uint16_t i;
        for(i = 0; i < letter_i; i++) {
            if(txt[i] == '\n') new_opt ++;
        }

        ext->sel_opt_id = new_opt;

        if(ext->action != NULL) {
            ext->action(ddlist);
        }
    }
    lv_ddlist_refr_size(ddlist, true);

    return LV_RES_OK;

}

/**
 * Refresh the size of drop down list according to its status (open or closed)
 * @param ddlist pointer to a drop down list object
 * @param anim_en Change the size (open/close) with or without animation (true/false)
 */
static void lv_ddlist_refr_size(lv_obj_t * ddlist, bool anim_en)
{
#if USE_LV_ANIMATION == 0
    anim_en = false;
#endif
    lv_ddlist_ext_t * ext = lv_obj_get_ext_attr(ddlist);
    lv_style_t * style = lv_obj_get_style(ddlist);
    lv_coord_t new_height;
    if(ext->opened) { /*Open the list*/
        if(ext->fix_height == 0) new_height = lv_obj_get_height(lv_page_get_scrl(ddlist)) + 2 * style->body.padding.ver;
        else new_height = ext->fix_height;
    } else { /*Close the list*/
        const lv_font_t * font = style->text.font;
        lv_style_t * label_style = lv_obj_get_style(ext->label);
        lv_coord_t font_h = lv_font_get_height(font);
        new_height = font_h + 2 * label_style->text.line_space;
    }

    if(anim_en == 0) {
        lv_obj_set_height(ddlist, new_height);
        lv_ddlist_pos_current_option(ddlist);
    } else {
#if USE_LV_ANIMATION
        lv_anim_t a;
        a.var = ddlist;
        a.start = lv_obj_get_height(ddlist);
        a.end = new_height;
        a.fp = (lv_anim_fp_t)lv_obj_set_height;
        a.path = lv_anim_path_linear;
        a.end_cb = (lv_anim_cb_t)lv_ddlist_pos_current_option;
        a.act_time = 0;
        a.time = ext->anim_time;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;

        lv_anim_create(&a);
#endif
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
    const lv_font_t * font = style->text.font;
    lv_coord_t font_h = lv_font_get_height(font);
    lv_style_t * label_style = lv_obj_get_style(ext->label);
    lv_obj_t * scrl = lv_page_get_scrl(ddlist);

    lv_coord_t h = lv_obj_get_height(ddlist);
    lv_coord_t line_y1 = ext->sel_opt_id * (font_h + label_style->text.line_space) + ext->label->coords.y1 - scrl->coords.y1;

    lv_obj_set_y(scrl, - line_y1 + (h - font_h) / 2);

}

#endif
