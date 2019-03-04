/**
 * @file lv_roller.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_roller.h"
#if USE_LV_ROLLER != 0

#include "../lv_draw/lv_draw.h"
#include "../lv_core/lv_group.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#if USE_LV_ANIMATION
#  ifndef LV_ROLLER_ANIM_TIME
#    define LV_ROLLER_ANIM_TIME     200         /*ms*/
#  endif
#else
#  undef  LV_ROLLER_ANIM_TIME
#  define LV_ROLLER_ANIM_TIME       0           /*No animation*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_roller_design(lv_obj_t * roller, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_roller_scrl_signal(lv_obj_t * roller_scrl, lv_signal_t sign, void * param);
static lv_res_t lv_roller_signal(lv_obj_t * roller, lv_signal_t sign, void * param);
static void refr_position(lv_obj_t * roller, bool anim_en);
static void draw_bg(lv_obj_t * roller, const lv_area_t * mask);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_signal_func_t ancestor_scrl_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a roller object
 * @param par pointer to an object, it will be the parent of the new roller
 * @param copy pointer to a roller object, if not NULL then the new object will be copied from it
 * @return pointer to the created roller
 */
lv_obj_t * lv_roller_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("roller create started");

    /*Create the ancestor of roller*/
    lv_obj_t * new_roller = lv_ddlist_create(par, copy);
    lv_mem_assert(new_roller);
    if(new_roller == NULL) return NULL;

    if(ancestor_scrl_signal == NULL) ancestor_scrl_signal = lv_obj_get_signal_func(lv_page_get_scrl(new_roller));
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_roller);

    /*Allocate the roller type specific extended data*/
    lv_roller_ext_t * ext = lv_obj_allocate_ext_attr(new_roller, sizeof(lv_roller_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;
    ext->ddlist.draw_arrow = 0;  /*Do not draw arrow by default*/

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_roller, lv_roller_signal);
    lv_obj_set_design_func(new_roller, lv_roller_design);

    /*Init the new roller roller*/
    if(copy == NULL) {
        lv_obj_t * scrl = lv_page_get_scrl(new_roller);
        lv_obj_set_drag(scrl, true);                        /*In ddlist is might be disabled*/
        lv_page_set_rel_action(new_roller, NULL);       /*Roller don't uses it (like ddlist)*/
        lv_page_set_scrl_fit(new_roller, true, false);      /*Height is specified directly*/
        lv_ddlist_open(new_roller, false);
        lv_ddlist_set_anim_time(new_roller, LV_ROLLER_ANIM_TIME);
        lv_roller_set_visible_row_count(new_roller, 3);
        lv_label_set_align(ext->ddlist.label, LV_LABEL_ALIGN_CENTER);

        lv_obj_set_signal_func(scrl, lv_roller_scrl_signal);

        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_roller_set_style(new_roller, LV_ROLLER_STYLE_BG, th->roller.bg);
            lv_roller_set_style(new_roller, LV_ROLLER_STYLE_SEL, th->roller.sel);
        } else {
            /*Let the ddlist's style*/
            lv_obj_refresh_style(new_roller);                /*To set scrollable size automatically*/
        }
    }
    /*Copy an existing roller*/
    else {
        lv_obj_t * scrl = lv_page_get_scrl(new_roller);
        lv_ddlist_open(new_roller, false);
        lv_obj_set_signal_func(scrl, lv_roller_scrl_signal);

        lv_obj_refresh_style(new_roller);        /*Refresh the style with new signal function*/
    }


    LV_LOG_INFO("roller created");


    return new_roller;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the align of the roller's options (left or center)
 * @param roller - pointer to a roller object
 * @param align - one of lv_label_align_t values (left, right, center)
 */
void lv_roller_set_align(lv_obj_t * roller, lv_label_align_t align)
{
    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    lv_mem_assert(ext);
    if(ext->ddlist.label == NULL) return;   /*Probably the roller is being deleted if the label is NULL.*/
    lv_label_set_align(ext->ddlist.label, align);
}

/**
 * Set the selected option
 * @param roller pointer to a roller object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 * @param anim_en true: set with animation; false set immediately
 */
void lv_roller_set_selected(lv_obj_t * roller, uint16_t sel_opt, bool anim_en)
{
#if USE_LV_ANIMATION == 0
    anim_en = false;
#endif

    if(lv_roller_get_selected(roller) == sel_opt) return;

    lv_ddlist_set_selected(roller, sel_opt);
    refr_position(roller, anim_en);
}

/**
 * Set the height to show the given number of rows (options)
 * @param roller pointer to a roller object
 * @param row_cnt number of desired visible rows
 */
void lv_roller_set_visible_row_count(lv_obj_t * roller, uint8_t row_cnt)
{
    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    lv_style_t * style_label = lv_obj_get_style(ext->ddlist.label);
    uint8_t n_line_space = (row_cnt > 1) ? row_cnt - 1 : 1;
    lv_ddlist_set_fix_height(roller, lv_font_get_height(style_label->text.font) * row_cnt + style_label->text.line_space * n_line_space);
}

/**
 * Set a style of a roller
 * @param roller pointer to a roller object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_roller_set_style(lv_obj_t * roller, lv_roller_style_t type, lv_style_t * style)
{
    switch(type) {
        case LV_ROLLER_STYLE_BG:
            lv_obj_set_style(roller, style);
            break;
        case LV_ROLLER_STYLE_SEL:
            lv_ddlist_set_style(roller, LV_DDLIST_STYLE_SEL, style);
            break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the align attribute. Default alignment after _create is LV_LABEL_ALIGN_CENTER
 * @param roller pointer to a roller object
 * @return LV_LABEL_ALIGN_LEFT, LV_LABEL_ALIGN_RIGHT or LV_LABEL_ALIGN_CENTER
 */
lv_label_align_t lv_roller_get_align(const lv_obj_t * roller)
{
    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    lv_mem_assert(ext);
    lv_mem_assert(ext->ddlist.label);
    return lv_label_get_align(ext->ddlist.label);
}

/**
 * Get the auto width set attribute
 * @param roller pointer to a roller object
 * @return true: auto size enabled; false: manual width settings enabled
 */
bool lv_roller_get_hor_fit(const lv_obj_t * roller)
{
    return lv_page_get_scrl_hor_fit(roller);
}

/**
 * Get a style of a roller
 * @param roller pointer to a roller object
 * @param type which style should be get
 * @return style pointer to a style
 *  */
lv_style_t * lv_roller_get_style(const lv_obj_t * roller, lv_roller_style_t type)
{
    switch(type) {
        case LV_ROLLER_STYLE_BG:
            return lv_obj_get_style(roller);
        case LV_ROLLER_STYLE_SEL:
            return lv_ddlist_get_style(roller, LV_DDLIST_STYLE_SEL);
        default:
            return NULL;
    }

    /*To avoid warning*/
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the rollers
 * @param roller pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_roller_design(lv_obj_t * roller, const lv_area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        draw_bg(roller, mask);

        lv_style_t * style = lv_roller_get_style(roller, LV_ROLLER_STYLE_BG);
        lv_opa_t opa_scale = lv_obj_get_opa_scale(roller);
        const lv_font_t * font = style->text.font;
        lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
        lv_coord_t font_h = lv_font_get_height(font);
        lv_area_t rect_area;
        rect_area.y1 = roller->coords.y1 + lv_obj_get_height(roller) / 2 - font_h / 2 - style->text.line_space / 2;
        if((font_h & 0x1) && (style->text.line_space & 0x1)) rect_area.y1 --;       /*Compensate the two rounding error*/
        rect_area.y2 = rect_area.y1 + font_h + style->text.line_space - 1;
        rect_area.x1 = roller->coords.x1;
        rect_area.x2 = roller->coords.x2;

        lv_draw_rect(&rect_area, mask, ext->ddlist.sel_style, opa_scale);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        lv_style_t * style = lv_roller_get_style(roller, LV_ROLLER_STYLE_BG);
        lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
        const lv_font_t * font = style->text.font;
        lv_coord_t font_h = lv_font_get_height(font);
        lv_opa_t opa_scale = lv_obj_get_opa_scale(roller);

        /*Redraw the text on the selected area with a different color*/
        lv_area_t rect_area;
        rect_area.y1 = roller->coords.y1 + lv_obj_get_height(roller) / 2 - font_h / 2 - style->text.line_space / 2;
        if((font_h & 0x1) && (style->text.line_space & 0x1)) rect_area.y1 --;       /*Compensate the two rounding error*/
        rect_area.y2 = rect_area.y1 + font_h + style->text.line_space - 1;
        rect_area.x1 = roller->coords.x1;
        rect_area.x2 = roller->coords.x2;
        lv_area_t mask_sel;
        bool area_ok;
        area_ok = lv_area_intersect(&mask_sel, mask, &rect_area);
        if(area_ok) {
            lv_style_t * sel_style = lv_roller_get_style(roller, LV_ROLLER_STYLE_SEL);
            lv_style_t new_style;
            lv_txt_flag_t txt_align = LV_TXT_FLAG_NONE;

            {
                lv_label_align_t label_align = lv_label_get_align(ext->ddlist.label);

                if(LV_LABEL_ALIGN_CENTER == label_align) {
                    txt_align |= LV_TXT_FLAG_CENTER;
                } else if(LV_LABEL_ALIGN_RIGHT == label_align) {
                    txt_align |= LV_TXT_FLAG_RIGHT;
                }
            }

            lv_style_copy(&new_style, style);
            new_style.text.color = sel_style->text.color;
            new_style.text.opa = sel_style->text.opa;
            lv_draw_label(&ext->ddlist.label->coords, &mask_sel, &new_style, opa_scale,
                          lv_label_get_text(ext->ddlist.label), txt_align, NULL);
        }
    }

    return true;
}

/**
 * Signal function of the roller
 * @param roller pointer to a roller object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_roller_signal(lv_obj_t * roller, lv_signal_t sign, void * param)
{
    lv_res_t res  = LV_RES_OK;

    /*Don't let the drop down list to handle the control signals. It works differently*/
    if(sign != LV_SIGNAL_CONTROLL && sign != LV_SIGNAL_FOCUS && sign != LV_SIGNAL_DEFOCUS) {
        /* Include the ancient signal function */
        res = ancestor_signal(roller, sign, param);
        if(res != LV_RES_OK) return res;
    }

    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    lv_align_t obj_align = LV_ALIGN_IN_LEFT_MID;
    if(ext->ddlist.label) {
        lv_label_align_t label_align = lv_label_get_align(ext->ddlist.label);
        if(LV_LABEL_ALIGN_CENTER == label_align) obj_align = LV_ALIGN_CENTER;
        else if(LV_LABEL_ALIGN_RIGHT == label_align) obj_align = LV_ALIGN_IN_RIGHT_MID;
    }

    if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_obj_set_height(lv_page_get_scrl(roller),
                          lv_obj_get_height(ext->ddlist.label) + lv_obj_get_height(roller));
        lv_obj_align(ext->ddlist.label, NULL, obj_align, 0, 0);
        lv_ddlist_set_selected(roller, ext->ddlist.sel_opt_id);
        refr_position(roller, false);
    } else if(sign == LV_SIGNAL_CORD_CHG) {

        if(lv_obj_get_width(roller) != lv_area_get_width(param) ||
                lv_obj_get_height(roller) != lv_area_get_height(param)) {

            lv_ddlist_set_fix_height(roller, lv_obj_get_height(roller));
            lv_obj_set_height(lv_page_get_scrl(roller),
                              lv_obj_get_height(ext->ddlist.label) + lv_obj_get_height(roller));

            lv_obj_align(ext->ddlist.label, NULL, obj_align, 0, 0);
            lv_ddlist_set_selected(roller, ext->ddlist.sel_opt_id);
            refr_position(roller, false);
        }
    } else if(sign == LV_SIGNAL_FOCUS) {
#if USE_LV_GROUP
        lv_group_t * g = lv_obj_get_group(roller);
        bool editing = lv_group_get_editing(g);
        lv_hal_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

        /*Encoders need special handling*/
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            /*In navigate mode revert the original value*/
            if(!editing) {
                if(ext->ddlist.sel_opt_id != ext->ddlist.sel_opt_id_ori) {
                    ext->ddlist.sel_opt_id = ext->ddlist.sel_opt_id_ori;
                    refr_position(roller, true);
                }
            }
            /*Save the current state when entered to edit mode*/
            else {
                ext->ddlist.sel_opt_id_ori = ext->ddlist.sel_opt_id;
            }
        } else {
            ext->ddlist.sel_opt_id_ori = ext->ddlist.sel_opt_id;      /*Save the current value. Used to revert this state if ENER wont't be pressed*/

        }
#endif
    } else if(sign == LV_SIGNAL_DEFOCUS) {
#if USE_LV_GROUP
        /*Revert the original state*/
        if(ext->ddlist.sel_opt_id != ext->ddlist.sel_opt_id_ori) {
            ext->ddlist.sel_opt_id = ext->ddlist.sel_opt_id_ori;
            refr_position(roller, true);
        }
#endif
    } else if(sign == LV_SIGNAL_CONTROLL) {
        char c = *((char *)param);
        if(c == LV_GROUP_KEY_RIGHT || c == LV_GROUP_KEY_DOWN) {
            if(ext->ddlist.sel_opt_id + 1 < ext->ddlist.option_cnt) {
                lv_roller_set_selected(roller, ext->ddlist.sel_opt_id + 1, true);
            }
        } else if(c == LV_GROUP_KEY_LEFT || c == LV_GROUP_KEY_UP) {
            if(ext->ddlist.sel_opt_id > 0) {
                lv_roller_set_selected(roller, ext->ddlist.sel_opt_id - 1, true);
            }
        } else if(c == LV_GROUP_KEY_ENTER) {
            ext->ddlist.sel_opt_id_ori = ext->ddlist.sel_opt_id;        /*Set the entered value as default*/
            if(ext->ddlist.action) ext->ddlist.action(roller);

#if USE_LV_GROUP
            lv_group_t * g = lv_obj_get_group(roller);
            bool editing = lv_group_get_editing(g);
            if(editing) lv_group_set_editing(g, false);     /*In edit mode go to navigate mode if an option is selected*/
#endif
        }
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_roller";
    }

    return res;
}

/**
 * Signal function of the scrollable part of the roller.
 * @param roller_scrl ointer to the scrollable part of roller (page)
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_roller_scrl_signal(lv_obj_t * roller_scrl, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_scrl_signal(roller_scrl, sign, param);
    if(res != LV_RES_OK) return res;

    lv_indev_t * indev = lv_indev_get_act();
    int32_t id = -1;
    lv_obj_t * roller = lv_obj_get_parent(roller_scrl);
    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);

    if(ext->ddlist.label == NULL) return LV_RES_INV;    /*On delete the ddlist signal deletes the label so nothing left to do here*/

    lv_style_t * style_label = lv_obj_get_style(ext->ddlist.label);
    const lv_font_t * font = style_label->text.font;
    lv_coord_t font_h = lv_font_get_height(font);

    if(sign == LV_SIGNAL_DRAG_END) {
        /*If dragged then align the list to there be an element in the middle*/
        lv_coord_t label_y1 = ext->ddlist.label->coords.y1 - roller->coords.y1;
        lv_coord_t label_unit = font_h + style_label->text.line_space;
        lv_coord_t mid = (roller->coords.y2 - roller->coords.y1) / 2;
        id = (mid - label_y1 + style_label->text.line_space / 2) / label_unit;
        if(id < 0) id = 0;
        if(id >= ext->ddlist.option_cnt) id = ext->ddlist.option_cnt - 1;
        ext->ddlist.sel_opt_id = id;
        if(ext->ddlist.action) ext->ddlist.action(roller);
    } else if(sign == LV_SIGNAL_RELEASED) {
        /*If picked an option by clicking then set it*/
        if(!lv_indev_is_dragging(indev)) {
            lv_point_t p;
            lv_indev_get_point(indev, &p);
            p.y = p.y - ext->ddlist.label->coords.y1;
            id = p.y / (font_h + style_label->text.line_space);
            if(id < 0) id = 0;
            if(id >= ext->ddlist.option_cnt) id = ext->ddlist.option_cnt - 1;
            ext->ddlist.sel_opt_id = id;
            if(ext->ddlist.action) ext->ddlist.action(roller);
        }
    }

    /*Position the scrollable according to the new selected option*/
    if(id != -1) {
        refr_position(roller, true);
    }

    return res;
}

/**
 * Draw a rectangle which has gradient on its top and bottom
 * @param roller pointer to a roller object
 * @param mask pointer to the current mask (from the design function)
 */
static void draw_bg(lv_obj_t * roller, const lv_area_t * mask)
{
    lv_style_t * style = lv_roller_get_style(roller, LV_ROLLER_STYLE_BG);
    lv_area_t half_mask;
    lv_area_t half_roller;
    lv_coord_t h = lv_obj_get_height(roller);
    bool union_ok;
    lv_area_copy(&half_roller, &roller->coords);

    half_roller.x1 -= roller->ext_size; /*Add ext size too (e.g. because of shadow draw) */
    half_roller.x2 += roller->ext_size;
    half_roller.y1 -= roller->ext_size;
    half_roller.y2 = roller->coords.y1 + h / 2;

    union_ok = lv_area_intersect(&half_mask, &half_roller, mask);

    half_roller.x1 += roller->ext_size; /*Revert ext. size adding*/
    half_roller.x2 -= roller->ext_size;
    half_roller.y1 += roller->ext_size;
    half_roller.y2 += style->body.radius;

    if(union_ok) {
        lv_draw_rect(&half_roller, &half_mask, style, lv_obj_get_opa_scale(roller));
    }

    half_roller.x1 -= roller->ext_size; /*Add ext size too (e.g. because of shadow draw) */
    half_roller.x2 += roller->ext_size;
    half_roller.y2 = roller->coords.y2 + roller->ext_size;
    half_roller.y1 = roller->coords.y1 + h / 2;
    if((h & 0x1) == 0) half_roller.y1++;    /*With even height the pixels in the middle would be drawn twice*/

    union_ok = lv_area_intersect(&half_mask, &half_roller, mask);

    half_roller.x1 += roller->ext_size; /*Revert ext. size adding*/
    half_roller.x2 -= roller->ext_size;
    half_roller.y2 -= roller->ext_size;
    half_roller.y1 -= style->body.radius;

    if(union_ok) {
        lv_color_t main_tmp = style->body.main_color;
        lv_color_t grad_tmp = style->body.grad_color;

        style->body.main_color = grad_tmp;
        style->body.grad_color = main_tmp;
        lv_draw_rect(&half_roller, &half_mask, style, lv_obj_get_opa_scale(roller));
        style->body.main_color = main_tmp;
        style->body.grad_color = grad_tmp;
    }
}

/**
 * Refresh the position of the roller. It uses the id stored in: ext->ddlist.selected_option_id
 * @param roller pointer to a roller object
 * @param anim_en true: refresh with animation; false: without animation
 */
static void refr_position(lv_obj_t * roller, bool anim_en)
{
#if USE_LV_ANIMATION == 0
    anim_en = false;
#endif
    lv_obj_t * roller_scrl = lv_page_get_scrl(roller);
    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    lv_style_t * style_label = lv_obj_get_style(ext->ddlist.label);
    const lv_font_t * font = style_label->text.font;
    lv_coord_t font_h = lv_font_get_height(font);
    lv_coord_t h = lv_obj_get_height(roller);
    int32_t id = ext->ddlist.sel_opt_id;
    lv_coord_t line_y1 = id * (font_h + style_label->text.line_space) + ext->ddlist.label->coords.y1 - roller_scrl->coords.y1;
    lv_coord_t new_y = - line_y1 + (h - font_h) / 2;

    if(ext->ddlist.anim_time == 0 || anim_en == false) {
        lv_obj_set_y(roller_scrl, new_y);
    } else {
#if USE_LV_ANIMATION
        lv_anim_t a;
        a.var = roller_scrl;
        a.start = lv_obj_get_y(roller_scrl);
        a.end = new_y;
        a.fp = (lv_anim_fp_t)lv_obj_set_y;
        a.path = lv_anim_path_linear;
        a.end_cb = NULL;
        a.act_time = 0;
        a.time = ext->ddlist.anim_time;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;
        lv_anim_create(&a);
#endif
    }
}

#endif
