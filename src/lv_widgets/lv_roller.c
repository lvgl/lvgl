/**
 * @file lv_roller.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_roller.h"
#if LV_USE_ROLLER != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_core/lv_group.h"
#include "../lv_themes/lv_theme.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_roller"

#if LV_USE_ANIMATION == 0
    #undef LV_ROLLER_DEF_ANIM_TIME
    #define LV_ROLLER_DEF_ANIM_TIME 0 /*No animation*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_roller_design(lv_obj_t * roller, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_design_res_t lv_roller_label_design(lv_obj_t * label, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_roller_scrl_signal(lv_obj_t * roller_scrl, lv_signal_t sign, void * param);
static lv_style_list_t * lv_roller_get_style(lv_obj_t * roller, uint8_t part);
static lv_res_t lv_roller_signal(lv_obj_t * roller, lv_signal_t sign, void * param);
static void refr_position(lv_obj_t * roller, lv_anim_enable_t animen);
static void refr_height(lv_obj_t * roller);
static void refr_width(lv_obj_t * roller);
static lv_res_t release_handler(lv_obj_t * roller);
static void inf_normalize(void * roller_scrl);
static lv_obj_t * get_label(const lv_obj_t * roller);
#if LV_USE_ANIMATION
    static void scroll_anim_ready_cb(lv_anim_t * a);
#endif
static void draw_bg(lv_obj_t * roller, const lv_area_t * clip_area);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_design_cb_t ancestor_label_design;
static lv_signal_cb_t ancestor_scrl_signal;

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
    lv_obj_t * roller = lv_page_create(par, copy);
    LV_ASSERT_MEM(roller);
    if(roller == NULL) return NULL;

    if(ancestor_scrl_signal == NULL) ancestor_scrl_signal = lv_obj_get_signal_cb(lv_page_get_scrollable(roller));
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(roller);

    /*Allocate the roller type specific extended data*/
    lv_roller_ext_t * ext = lv_obj_allocate_ext_attr(roller, sizeof(lv_roller_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(roller);
        return NULL;
    }

    ext->mode = LV_ROLLER_MODE_NORMAL;
    ext->option_cnt = 0;
    ext->sel_opt_id = 0;
    ext->sel_opt_id_ori = 0;
    ext->auto_fit = 1;
    lv_style_list_init(&ext->style_sel);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(roller, lv_roller_signal);
    lv_obj_set_design_cb(roller, lv_roller_design);

    /*Init the new roller roller*/
    if(copy == NULL) {

        lv_obj_t * label = lv_label_create(roller, NULL);
        lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
        if(ancestor_label_design == NULL) ancestor_label_design = lv_obj_get_design_cb(label);
        lv_obj_set_design_cb(label, lv_roller_label_design);
        lv_obj_t * scrl = lv_page_get_scrollable(roller);
        lv_obj_set_drag(scrl, true);
        lv_page_set_scrollable_fit2(roller, LV_FIT_PARENT, LV_FIT_NONE); /*Height is specified directly*/
        lv_roller_set_anim_time(roller, LV_ROLLER_DEF_ANIM_TIME);
        lv_roller_set_options(roller, "Option 1\nOption 2\nOption 3\nOption 4\nOption 5", LV_ROLLER_MODE_NORMAL);

        lv_obj_set_signal_cb(scrl, lv_roller_scrl_signal);

        lv_obj_clean_style_list(roller, LV_PAGE_PART_SCROLLABLE); /*Use a transparent scrollable*/
        lv_theme_apply(roller, LV_THEME_ROLLER);
        refr_height(roller);

        lv_roller_set_visible_row_count(roller, 3);
    }
    /*Copy an existing roller*/
    else {
        lv_label_create(roller, get_label(copy));

        lv_roller_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->mode = copy_ext->mode;
        ext->option_cnt = copy_ext->option_cnt;
        ext->sel_opt_id = copy_ext->sel_opt_id;
        ext->sel_opt_id_ori = copy_ext->sel_opt_id;
        ext->auto_fit = copy_ext->auto_fit;
        lv_obj_t * scrl = lv_page_get_scrollable(roller);
        lv_obj_set_signal_cb(scrl, lv_roller_scrl_signal);

        lv_style_list_copy(&ext->style_sel, &copy_ext->style_sel);
        lv_obj_refresh_style(roller, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    LV_LOG_INFO("roller created");

    return roller;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the options on a roller
 * @param roller pointer to roller object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 * @param mode `LV_ROLLER_MODE_NORMAL` or `LV_ROLLER_MODE_INFINITE`
 */
void lv_roller_set_options(lv_obj_t * roller, const char * options, lv_roller_mode_t mode)
{
    LV_ASSERT_OBJ(roller, LV_OBJX_NAME);
    LV_ASSERT_STR(options);


    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    lv_obj_t * label = get_label(roller);

    ext->sel_opt_id     = 0;
    ext->sel_opt_id_ori = 0;

    /*Count the '\n'-s to determine the number of options*/
    ext->option_cnt = 0;
    uint32_t cnt;
    for(cnt = 0; options[cnt] != '\0'; cnt++) {
        if(options[cnt] == '\n') ext->option_cnt++;
    }
    ext->option_cnt++; /*Last option has no `\n`*/

    if(mode == LV_ROLLER_MODE_NORMAL) {
        ext->mode = LV_ROLLER_MODE_NORMAL;
        lv_label_set_text(label, options);
    }
    else {
        ext->mode = LV_ROLLER_MODE_INFINITE;

        size_t opt_len = strlen(options) + 1; /*+1 to add '\n' after option lists*/
        char * opt_extra = _lv_mem_buf_get(opt_len * LV_ROLLER_INF_PAGES);
        uint8_t i;
        for(i = 0; i < LV_ROLLER_INF_PAGES; i++) {
            strcpy(&opt_extra[opt_len * i], options);
            opt_extra[opt_len * (i + 1) - 1] = '\n';
        }
        opt_extra[opt_len * LV_ROLLER_INF_PAGES - 1] = '\0';
        lv_label_set_text(label, opt_extra);
        _lv_mem_buf_release(opt_extra);

        ext->sel_opt_id     = ((LV_ROLLER_INF_PAGES / 2) + 0) * ext->option_cnt;

        ext->option_cnt = ext->option_cnt * LV_ROLLER_INF_PAGES;
    }

    ext->sel_opt_id_ori = ext->sel_opt_id;

    refr_height(roller);
    refr_width(roller);
}

/**
 * Set the align of the roller's options (left or center)
 * @param roller - pointer to a roller object
 * @param align - one of lv_label_align_t values (left, right, center)
 */
void lv_roller_set_align(lv_obj_t * roller, lv_label_align_t align)
{
    LV_ASSERT_OBJ(roller, LV_OBJX_NAME);

    lv_obj_t * label = get_label(roller);
    if(label == NULL) return; /*Probably the roller is being deleted if the label is NULL.*/

    lv_label_set_align(label, align);
    refr_width(roller); /*To set the correct label position*/
}

/**
 * Set the selected option
 * @param roller pointer to a roller object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 * @param anim_en LV_ANIM_ON: set with animation; LV_ANOM_OFF set immediately
 */
void lv_roller_set_selected(lv_obj_t * roller, uint16_t sel_opt, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(roller, LV_OBJX_NAME);

#if LV_USE_ANIMATION == 0
    anim = LV_ANIM_OFF;
#endif

    /* Set the value even if it's the same as the current value because
     * if moving to the next option with an animation which was just deleted in the PRESS signal
     * nothing will continue the animation. */

    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);

    /*In infinite mode interpret the new ID relative to the currently visible "page"*/
    if(ext->mode == LV_ROLLER_MODE_INFINITE) {
        int32_t sel_opt_signed = sel_opt;
        uint16_t page = ext->sel_opt_id / LV_ROLLER_INF_PAGES;

        /* `sel_opt` should be less than the number of options set by the user.
         * If it's more then probably it's a reference from not the first page
         * so normalize `sel_opt` */
        if(page != 0) {
            sel_opt_signed -= page * LV_ROLLER_INF_PAGES;
        }

        sel_opt = page * LV_ROLLER_INF_PAGES + sel_opt_signed;
    }

    ext->sel_opt_id     = sel_opt < ext->option_cnt ? sel_opt : ext->option_cnt - 1;
    ext->sel_opt_id_ori = ext->sel_opt_id;

    refr_position(roller, anim);
}

/**
 * Set the height to show the given number of rows (options)
 * @param roller pointer to a roller object
 * @param row_cnt number of desired visible rows
 */
void lv_roller_set_visible_row_count(lv_obj_t * roller, uint8_t row_cnt)
{
    LV_ASSERT_OBJ(roller, LV_OBJX_NAME);

    const lv_font_t * font = lv_obj_get_style_text_font(roller, LV_ROLLER_PART_BG);
    lv_style_int_t line_space = lv_obj_get_style_text_line_space(roller, LV_ROLLER_PART_BG);
    lv_obj_set_height(roller, (lv_font_get_line_height(font) + line_space) * row_cnt);

    refr_height(roller);
    refr_position(roller, LV_ANIM_OFF);
}

/**
 * Allow automatically setting the width of roller according to it's content.
 * @param roller pointer to a roller object
 * @param auto_fit true: enable auto fit
 */
void lv_roller_set_auto_fit(lv_obj_t * roller, bool auto_fit)
{
    LV_ASSERT_OBJ(roller, LV_OBJX_NAME);
    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    ext->auto_fit = auto_fit;
    refr_width(roller);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the id of the selected option
 * @param roller pointer to a roller object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t lv_roller_get_selected(const lv_obj_t * roller)
{
    LV_ASSERT_OBJ(roller, LV_OBJX_NAME);

    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    if(ext->mode == LV_ROLLER_MODE_INFINITE) {
        uint16_t real_id_cnt = ext->option_cnt / LV_ROLLER_INF_PAGES;
        return ext->sel_opt_id % real_id_cnt;
    }
    else {
        return ext->sel_opt_id;
    }
}


/**
 * Get the current selected option as a string
 * @param ddlist pointer to ddlist object
 * @param buf pointer to an array to store the string
 * @param buf_size size of `buf` in bytes. 0: to ignore it.
 */
void lv_roller_get_selected_str(const lv_obj_t * roller, char * buf, uint32_t buf_size)
{
    LV_ASSERT_OBJ(roller, LV_OBJX_NAME);

    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    lv_obj_t * label = get_label(roller);
    uint32_t i;
    uint16_t line        = 0;
    const char * opt_txt = lv_label_get_text(label);
    size_t txt_len     = strlen(opt_txt);

    for(i = 0; i < txt_len && line != ext->sel_opt_id; i++) {
        if(opt_txt[i] == '\n') line++;
    }

    uint32_t c;
    for(c = 0; i < txt_len && opt_txt[i] != '\n'; c++, i++) {
        if(buf_size && c >= buf_size - 1) {
            LV_LOG_WARN("lv_dropdown_get_selected_str: the buffer was too small")
            break;
        }
        buf[c] = opt_txt[i];
    }

    buf[c] = '\0';
}

/**
 * Get the total number of options
 * @param roller pointer to a roller object
 * @return the total number of options
 */
uint16_t lv_roller_get_option_cnt(const lv_obj_t * roller)
{
    LV_ASSERT_OBJ(roller, LV_OBJX_NAME);

    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    if(ext->mode == LV_ROLLER_MODE_INFINITE) {
        return ext->option_cnt / LV_ROLLER_INF_PAGES;
    }
    else {
        return ext->option_cnt;
    }
}

/**
 * Get the align attribute. Default alignment after _create is LV_LABEL_ALIGN_CENTER
 * @param roller pointer to a roller object
 * @return LV_LABEL_ALIGN_LEFT, LV_LABEL_ALIGN_RIGHT or LV_LABEL_ALIGN_CENTER
 */
lv_label_align_t lv_roller_get_align(const lv_obj_t * roller)
{
    LV_ASSERT_OBJ(roller, LV_OBJX_NAME);

    return lv_label_get_align(get_label(roller));
}

/**
 * Get whether the auto fit option is enabled or not.
 * @param roller pointer to a roller object
 * @return true: auto fit is enabled
 */
bool lv_roller_get_auto_fit(lv_obj_t * roller)
{
    LV_ASSERT_OBJ(roller, LV_OBJX_NAME);
    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    return ext->auto_fit ? true : false;
}

/**
 * Get the options of a roller
 * @param roller pointer to roller object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * lv_roller_get_options(const lv_obj_t * roller)
{
    LV_ASSERT_OBJ(roller, LV_OBJX_NAME);

    return lv_label_get_text(get_label(roller));
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the rollers
 * @param roller pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after all children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_roller_design(lv_obj_t * roller, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        return LV_DESIGN_RES_NOT_COVER;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        draw_bg(roller, clip_area);

        const lv_font_t * font = lv_obj_get_style_text_font(roller, LV_ROLLER_PART_BG);
        lv_style_int_t line_space = lv_obj_get_style_text_line_space(roller, LV_ROLLER_PART_BG);
        lv_coord_t font_h        = lv_font_get_line_height(font);
        lv_area_t rect_area;
        rect_area.y1 = roller->coords.y1 + lv_obj_get_height(roller) / 2 - font_h / 2 - line_space / 2;
        if((font_h & 0x1) && (line_space & 0x1)) rect_area.y1--; /*Compensate the two rounding error*/
        rect_area.y2 = rect_area.y1 + font_h + line_space - 1;
        lv_area_t roller_coords;
        lv_obj_get_coords(roller, &roller_coords);
        lv_obj_get_inner_coords(roller, &roller_coords);

        rect_area.x1 = roller_coords.x1;
        rect_area.x2 = roller_coords.x2;

        lv_draw_rect_dsc_t sel_dsc;
        lv_draw_rect_dsc_init(&sel_dsc);
        lv_obj_init_draw_rect_dsc(roller, LV_ROLLER_PART_SELECTED, &sel_dsc);
        lv_draw_rect(&rect_area, clip_area, &sel_dsc);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        lv_obj_init_draw_label_dsc(roller, LV_ROLLER_PART_SELECTED, &label_dsc);

        lv_coord_t bg_font_h        = lv_font_get_line_height(lv_obj_get_style_text_font(roller, LV_ROLLER_PART_BG));

        /*Redraw the text on the selected area*/
        lv_area_t rect_area;
        rect_area.y1 = roller->coords.y1 + lv_obj_get_height(roller) / 2 - bg_font_h / 2 - label_dsc.line_space / 2;
        if((bg_font_h & 0x1) && (label_dsc.line_space & 0x1)) rect_area.y1--; /*Compensate the two rounding error*/
        rect_area.y2 = rect_area.y1 + bg_font_h + label_dsc.line_space - 1;
        rect_area.x1 = roller->coords.x1;
        rect_area.x2 = roller->coords.x2;
        lv_area_t mask_sel;
        bool area_ok;
        area_ok = _lv_area_intersect(&mask_sel, clip_area, &rect_area);
        if(area_ok) {
            lv_obj_t * label = get_label(roller);
            lv_label_align_t label_align = lv_roller_get_align(roller);

            if(LV_LABEL_ALIGN_CENTER == label_align) {
                label_dsc.flag |= LV_TXT_FLAG_CENTER;
            }
            else if(LV_LABEL_ALIGN_RIGHT == label_align) {
                label_dsc.flag |= LV_TXT_FLAG_RIGHT;
            }

            /*Get the size of the "selected text"*/
            lv_point_t res_p;
            _lv_txt_get_size(&res_p, lv_label_get_text(label), label_dsc.font, label_dsc.letter_space, label_dsc.line_space,
                             lv_obj_get_width(roller), LV_TXT_FLAG_EXPAND);

            /*Move the selected label proportionally with the background label*/
            lv_coord_t roller_h = lv_obj_get_height(roller);
            int32_t label_y_prop = label->coords.y1 - (roller_h / 2 +
                                                       roller->coords.y1); /*label offset from the middle line of the roller*/
            label_y_prop = (label_y_prop << 14) / lv_obj_get_height(
                               label); /*Proportional position from the middle line (upscaled)*/

            /*Apply a correction with different line heights*/
            const lv_font_t * normal_label_font = lv_obj_get_style_text_font(roller, LV_ROLLER_PART_BG);
            lv_coord_t corr = (label_dsc.font->line_height - normal_label_font->line_height) / 2;

            /*Apply the proportional position to the selected text*/
            res_p.y -= corr;
            int32_t label_sel_y = roller_h / 2 + roller->coords.y1;
            label_sel_y += (label_y_prop * res_p.y) >> 14;
            label_sel_y -= corr;

            /*Draw the selected text*/
            lv_area_t label_sel_area;
            label_sel_area.x1 = label->coords.x1;
            label_sel_area.y1 = label_sel_y;
            label_sel_area.x2 = label->coords.x2;
            label_sel_area.y2 = label_sel_area.y1 + res_p.y;

            label_dsc.flag |= LV_TXT_FLAG_EXPAND;
            lv_draw_label(&label_sel_area, &mask_sel, &label_dsc, lv_label_get_text(label), NULL);
        }
    }

    return LV_DESIGN_RES_OK;
}


/**
 * Handle the drawing related tasks of the roller's label
 * @param roller pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after all children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_roller_label_design(lv_obj_t * label, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        return ancestor_label_design(label, clip_area, mode);
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        /* Split the drawing of the label into  an upper (above the selected area)
         * and a lower (below the selected area)*/
        lv_obj_t * roller = lv_obj_get_parent(lv_obj_get_parent(label));
        const lv_font_t * font = lv_obj_get_style_text_font(roller, LV_ROLLER_PART_BG);
        lv_style_int_t line_space = lv_obj_get_style_text_line_space(roller, LV_ROLLER_PART_BG);
        lv_coord_t font_h        = lv_font_get_line_height(font);

        lv_area_t rect_area;
        rect_area.y1 = roller->coords.y1 + lv_obj_get_height(roller) / 2 - font_h / 2 - line_space / 2;
        if((font_h & 0x1) && (line_space & 0x1)) rect_area.y1--; /*Compensate the two rounding error*/
        rect_area.y2 = rect_area.y1 + font_h + line_space - 1;
        lv_area_t roller_coords;
        lv_obj_get_coords(roller, &roller_coords);
        lv_obj_get_inner_coords(roller, &roller_coords);

        rect_area.x1 = roller_coords.x1;
        rect_area.x2 = roller_coords.x2;

        lv_area_t clip2;
        clip2.x1 = label->coords.x1;
        clip2.y1 = label->coords.y1;
        clip2.x2 = label->coords.x2;
        clip2.y2 = rect_area.y1;
        if(_lv_area_intersect(&clip2, clip_area, &clip2)) {
            ancestor_label_design(label, &clip2, mode);
        }

        clip2.x1 = label->coords.x1;
        clip2.y1 = rect_area.y2;
        clip2.x2 = label->coords.x2;
        clip2.y2 = label->coords.y2;
        if(_lv_area_intersect(&clip2, clip_area, &clip2)) {
            ancestor_label_design(label, &clip2, mode);
        }
    }

    return LV_DESIGN_RES_OK;
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
    lv_res_t res = LV_RES_OK;
    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_roller_get_style(roller, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(roller, sign, param);
    }

    /* Include the ancient signal function */
    if(sign != LV_SIGNAL_CONTROL) { /*Don't let the page to scroll on keys*/
        res = ancestor_signal(roller, sign, param);
        if(res != LV_RES_OK) return res;
    }

    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    LV_UNUSED(ext);

    if(sign == LV_SIGNAL_STYLE_CHG) {
        lv_obj_t * label = get_label(roller);
        /*Be sure the label's style is updated before processing the roller*/
        if(label) lv_signal_send(label, LV_SIGNAL_STYLE_CHG, NULL);
        refr_height(roller);
        refr_width(roller);
        refr_position(roller, false);
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {

        if(lv_obj_get_width(roller) != lv_area_get_width(param) ||
           lv_obj_get_height(roller) != lv_area_get_height(param)) {
#if LV_USE_ANIMATION
            lv_anim_del(lv_page_get_scrollable(roller), (lv_anim_exec_xcb_t)lv_obj_set_y);
#endif
            refr_position(roller, false);
            refr_width(roller);
        }
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        release_handler(roller);
    }
    else if(sign == LV_SIGNAL_FOCUS) {
#if LV_USE_GROUP
        lv_group_t * g             = lv_obj_get_group(roller);
        bool editing               = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

        /*Encoders need special handling*/
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            /*In navigate mode revert the original value*/
            if(!editing) {
                if(ext->sel_opt_id != ext->sel_opt_id_ori) {
                    ext->sel_opt_id = ext->sel_opt_id_ori;
                    refr_position(roller, true);
                }
            }
            /*Save the current state when entered to edit mode*/
            else {
                ext->sel_opt_id_ori = ext->sel_opt_id;
            }
        }
        else {
            ext->sel_opt_id_ori = ext->sel_opt_id; /*Save the current value. Used to revert this state if
                                                                    ENTER won't be pressed*/
        }
#endif
    }
    else if(sign == LV_SIGNAL_DEFOCUS) {
#if LV_USE_GROUP
        /*Revert the original state*/
        if(ext->sel_opt_id != ext->sel_opt_id_ori) {
            ext->sel_opt_id = ext->sel_opt_id_ori;
            refr_position(roller, true);
        }
#endif
    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        char c = *((char *)param);
        if(c == LV_KEY_RIGHT || c == LV_KEY_DOWN) {
            if(ext->sel_opt_id + 1 < ext->option_cnt) {
                uint16_t ori_id = ext->sel_opt_id_ori; /*lv_roller_set_selected will overwrite this*/
                lv_roller_set_selected(roller, ext->sel_opt_id + 1, true);
                ext->sel_opt_id_ori = ori_id;
            }
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_UP) {
            if(ext->sel_opt_id > 0) {
                uint16_t ori_id = ext->sel_opt_id_ori; /*lv_roller_set_selected will overwrite this*/

                lv_roller_set_selected(roller, ext->sel_opt_id - 1, true);
                ext->sel_opt_id_ori = ori_id;
            }
        }
#endif
    }
    else if(sign == LV_SIGNAL_CLEANUP) {
        lv_obj_clean_style_list(roller, LV_ROLLER_PART_SELECTED);
    }
    return res;
}

/**
 * Get the style descriptor of a part of the object
 * @param page pointer the object
 * @param part the part from `lv_roller_part_t`. (LV_ROLLER_PART_...)
 * @return pointer to the style descriptor of the specified part
 */
static lv_style_list_t * lv_roller_get_style(lv_obj_t * roller, uint8_t part)
{
    LV_ASSERT_OBJ(roller, LV_OBJX_NAME);

    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);
    lv_style_list_t * style_dsc_p;

    switch(part) {
        case LV_ROLLER_PART_BG:
            style_dsc_p = &roller->style_list;
            break;
        case LV_ROLLER_PART_SELECTED:
            style_dsc_p = &ext->style_sel;
            break;
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}

/**
 * Signal function of the scrollable part of the roller.
 * @param roller_scrl pointer to the scrollable part of roller (page)
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
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_obj_t * roller     = lv_obj_get_parent(roller_scrl);

    /*On delete the ddlist signal deletes the label so nothing left to do here*/
    lv_obj_t * label = get_label(roller);
    if(label == NULL) return LV_RES_INV;

    int32_t id            = -1;
    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);

    const lv_font_t * font = lv_obj_get_style_text_font(roller, LV_ROLLER_PART_BG);
    lv_style_int_t line_space = lv_obj_get_style_text_line_space(roller, LV_ROLLER_PART_BG);
    lv_coord_t font_h              = lv_font_get_line_height(font);

    if(sign == LV_SIGNAL_DRAG_END) {
        /*If dragged then align the list to have an element in the middle*/
        lv_coord_t label_y1   = label->coords.y1 - roller->coords.y1;
        lv_coord_t label_unit = font_h + line_space;
        lv_coord_t mid        = (roller->coords.y2 - roller->coords.y1) / 2;

        id = (mid - label_y1 + line_space / 2) / label_unit;

        if(id < 0) id = 0;
        if(id >= ext->option_cnt) id = ext->option_cnt - 1;

        ext->sel_opt_id     = id;
        ext->sel_opt_id_ori = id;
        res = lv_event_send(roller, LV_EVENT_VALUE_CHANGED, &id);
        if(res != LV_RES_OK) return res;
    }
    /*If picked an option by clicking then set it*/
    else if(sign == LV_SIGNAL_RELEASED) {
        release_handler(roller);
    }
    else if(sign == LV_SIGNAL_PRESSED) {
#if LV_USE_ANIMATION
        lv_anim_del(roller_scrl, (lv_anim_exec_xcb_t)lv_obj_set_y);
#endif
    }
    else if(sign == LV_SIGNAL_PARENT_SIZE_CHG) {
#if LV_USE_ANIMATION
        lv_anim_del(lv_page_get_scrollable(roller), (lv_anim_exec_xcb_t)lv_obj_set_y);
#endif
        refr_position(roller, false);
        refr_width(roller);

    }

    /*Position the scrollable according to the new selected option*/
    if(id != -1) {
        refr_position(roller, LV_ANIM_ON);
    }

    return res;
}

/**
 * Draw a rectangle which has gradient on its top and bottom
 * @param roller pointer to a roller object
 * @param clip_area pointer to the current mask (from the design function)
 */
static void draw_bg(lv_obj_t * roller, const lv_area_t * clip_area)
{
    lv_draw_rect_dsc_t bg_dsc;
    lv_draw_rect_dsc_init(&bg_dsc);
    lv_obj_init_draw_rect_dsc(roller, LV_ROLLER_PART_BG, &bg_dsc);

    /*With non-vertical gradient simply draw the background*/
    if(bg_dsc.bg_grad_dir == LV_GRAD_DIR_NONE) {
        lv_draw_rect(&roller->coords, clip_area, &bg_dsc);
        return;
    }

    /*With vertical gradient mirror it*/

    lv_area_t half_mask;
    lv_coord_t h = lv_obj_get_height(roller);
    bool union_ok;

    lv_area_copy(&half_mask, &roller->coords);
    half_mask.x1 -= roller->ext_draw_pad; /*Add ext size too (e.g. because of shadow draw) */
    half_mask.x2 += roller->ext_draw_pad;
    half_mask.y1 -= roller->ext_draw_pad;
    half_mask.y2 = roller->coords.y1 + h / 2;

    union_ok = _lv_area_intersect(&half_mask, &half_mask, clip_area);
    bg_dsc.bg_main_color_stop = bg_dsc.bg_main_color_stop / 2;
    bg_dsc.bg_grad_color_stop = 128 - (255 - bg_dsc.bg_grad_color_stop) / 2;
    if(union_ok) {
        lv_draw_rect(&roller->coords, &half_mask, &bg_dsc);
    }

    lv_area_copy(&half_mask, &roller->coords);
    half_mask.x1 -= roller->ext_draw_pad; /*Revert ext. size adding*/
    half_mask.x2 += roller->ext_draw_pad;
    half_mask.y1 = roller->coords.y1 + h / 2;
    half_mask.y2 += roller->ext_draw_pad;

    union_ok = _lv_area_intersect(&half_mask, &half_mask, clip_area);
    if(union_ok) {
        lv_color_t c = bg_dsc.bg_color;
        bg_dsc.bg_color = bg_dsc.bg_grad_color;
        bg_dsc.bg_grad_color = c;

        bg_dsc.bg_main_color_stop += 127;
        bg_dsc.bg_grad_color_stop += 127;
        lv_draw_rect(&roller->coords, &half_mask, &bg_dsc);
    }
}


/**
 * Refresh the position of the roller. It uses the id stored in: ext->ddlist.selected_option_id
 * @param roller pointer to a roller object
 * @param anim_en LV_ANIM_ON: refresh with animation; LV_ANOM_OFF: without animation
 */
static void refr_position(lv_obj_t * roller, lv_anim_enable_t anim_en)
{
#if LV_USE_ANIMATION == 0
    anim_en = LV_ANIM_OFF;
#endif

    lv_obj_t * roller_scrl         = lv_page_get_scrollable(roller);
    lv_roller_ext_t * ext          = lv_obj_get_ext_attr(roller);
    const lv_font_t * font = lv_obj_get_style_text_font(roller, LV_ROLLER_PART_BG);
    lv_style_int_t line_space = lv_obj_get_style_text_line_space(roller, LV_ROLLER_PART_BG);
    lv_coord_t font_h              = lv_font_get_line_height(font);
    lv_coord_t h                   = lv_obj_get_height(roller);
    uint16_t anim_time             = lv_roller_get_anim_time(roller);

    /* Normally the animation's `end_cb` sets correct position of the roller if infinite.
     * But without animations do it manually*/
    if(anim_en == LV_ANIM_OFF || anim_time == 0) {
        inf_normalize(roller_scrl);
    }

    lv_obj_t * label = get_label(roller);

    int32_t id = ext->sel_opt_id;
    lv_coord_t line_y1 = id * (font_h + line_space) + label->coords.y1 - roller_scrl->coords.y1;
    lv_coord_t new_y = -line_y1 + (h - font_h) / 2;

    if(anim_en == LV_ANIM_OFF || anim_time == 0) {
#if LV_USE_ANIMATION
        lv_anim_del(roller_scrl, (lv_anim_exec_xcb_t)lv_obj_set_y);
#endif
        lv_obj_set_y(roller_scrl, new_y);
    }
    else {
#if LV_USE_ANIMATION
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, roller_scrl);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)lv_obj_set_y);
        lv_anim_set_values(&a, lv_obj_get_y(roller_scrl), new_y);
        lv_anim_set_time(&a, anim_time);
        lv_anim_set_ready_cb(&a, scroll_anim_ready_cb);
        lv_anim_start(&a);
#endif
    }
}


static lv_res_t release_handler(lv_obj_t * roller)
{

    /*If there was dragging `DRAG_END` signal will refresh the position and update the selected option*/
    if(lv_indev_is_dragging(lv_indev_get_act())) return LV_RES_OK;

    lv_roller_ext_t * ext = lv_obj_get_ext_attr(roller);

    lv_indev_t * indev = lv_indev_get_act();
#if LV_USE_GROUP
    /*Leave edit mode once a new option is selected*/
    lv_indev_type_t indev_type = lv_indev_get_type(indev);
    if(indev_type == LV_INDEV_TYPE_ENCODER || indev_type == LV_INDEV_TYPE_KEYPAD) {
        ext->sel_opt_id_ori = ext->sel_opt_id;

        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            lv_group_t * g      = lv_obj_get_group(roller);
            if(lv_group_get_editing(g)) {
                lv_group_set_editing(g, false);
            }
        }
    }
#endif

    lv_obj_t * label = get_label(roller);
    if(label == NULL) return LV_RES_OK;

    if(lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER || lv_indev_get_type(indev) == LV_INDEV_TYPE_BUTTON) {
        /*Search the clicked option (For KEYPAD and ENCODER the new value should be already set)*/
        uint16_t new_opt  = 0;
        lv_point_t p;
        lv_indev_get_point(indev, &p);
        p.y -= label->coords.y1;
        p.x -= label->coords.x1;
        uint32_t letter_i;
        letter_i = lv_label_get_letter_on(label, &p);

        const char * txt  = lv_label_get_text(label);
        uint32_t i        = 0;
        uint32_t i_prev   = 0;

        uint32_t letter_cnt = 0;
        for(letter_cnt = 0; letter_cnt < letter_i; letter_cnt++) {
            uint32_t letter = _lv_txt_encoded_next(txt, &i);
            /*Count he lines to reach the clicked letter. But ignore the last '\n' because it
             * still belongs to the clicked line*/
            if(letter == '\n' && i_prev != letter_i) new_opt++;
            i_prev = i;
        }
        lv_roller_set_selected(roller, new_opt, LV_ANIM_ON);
    }

    uint32_t id  = ext->sel_opt_id; /*Just to use uint32_t in event data*/
    lv_res_t res = lv_event_send(roller, LV_EVENT_VALUE_CHANGED, &id);
    return res;
}

static void refr_width(lv_obj_t * roller)
{
    lv_obj_t * label = get_label(roller);
    if(label == NULL) return;

    switch(lv_label_get_align(label)) {
        case LV_LABEL_ALIGN_LEFT:
            lv_obj_align(label, NULL, LV_ALIGN_IN_LEFT_MID, 0, 0);
            break;
        case LV_LABEL_ALIGN_CENTER:
            lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
            break;
        case LV_LABEL_ALIGN_RIGHT:
            lv_obj_align(label, NULL, LV_ALIGN_IN_RIGHT_MID, 0, 0);
            break;
    }


    if(lv_roller_get_auto_fit(roller) == false) return;

    lv_coord_t label_w = lv_obj_get_width(label);

    lv_style_int_t left = lv_obj_get_style_pad_left(roller, LV_ROLLER_PART_BG);
    lv_style_int_t right = lv_obj_get_style_pad_right(roller, LV_ROLLER_PART_BG);

    const lv_font_t * base_font = lv_obj_get_style_text_font(roller, LV_ROLLER_PART_BG);
    const lv_font_t * sel_font = lv_obj_get_style_text_font(roller, LV_ROLLER_PART_SELECTED);

    /*The selected text might be larger to get its size*/
    if(base_font != sel_font) {
        lv_coord_t letter_sp = lv_obj_get_style_text_letter_space(roller, LV_ROLLER_PART_SELECTED);
        lv_coord_t line_sp = lv_obj_get_style_text_line_space(roller, LV_ROLLER_PART_SELECTED);
        lv_point_t p;
        _lv_txt_get_size(&p, lv_label_get_text(label), sel_font, letter_sp, line_sp, LV_COORD_MAX, LV_TXT_FLAG_NONE);
        if(label_w < p.x)label_w = p.x;
    }

    lv_obj_set_width(roller, label_w + left + right);
}

/**
 * Refresh the height of the roller and the scrollable
 * @param roller pointer to roller
 */
static void refr_height(lv_obj_t * roller)
{
    lv_obj_t * label = get_label(roller);
    if(label == NULL) return;

    lv_obj_set_height(lv_page_get_scrollable(roller), lv_obj_get_height(label) + lv_obj_get_height(roller));

#if LV_USE_ANIMATION
    lv_anim_del(lv_page_get_scrollable(roller), (lv_anim_exec_xcb_t)lv_obj_set_y);
#endif
    refr_position(roller, LV_ANIM_OFF);
}

/**
 * Set the middle page for the roller if infinite is enabled
 * @param scrl pointer to the roller's scrollable (lv_obj_t *)
 */
static void inf_normalize(void * scrl)
{
    lv_obj_t * roller_scrl = (lv_obj_t *)scrl;
    lv_obj_t * roller      = lv_obj_get_parent(roller_scrl);
    lv_roller_ext_t * ext  = lv_obj_get_ext_attr(roller);

    if(ext->mode == LV_ROLLER_MODE_INFINITE) {
        uint16_t real_id_cnt = ext->option_cnt / LV_ROLLER_INF_PAGES;
        ext->sel_opt_id = ext->sel_opt_id % real_id_cnt;
        ext->sel_opt_id += (LV_ROLLER_INF_PAGES / 2) * real_id_cnt; /*Select the middle page*/

        ext->sel_opt_id_ori = ext->sel_opt_id % real_id_cnt;
        ext->sel_opt_id_ori += (LV_ROLLER_INF_PAGES / 2) * real_id_cnt; /*Select the middle page*/

        /*Move to the new id*/
        const lv_font_t * font = lv_obj_get_style_text_font(roller, LV_ROLLER_PART_BG);
        lv_style_int_t line_space = lv_obj_get_style_text_line_space(roller, LV_ROLLER_PART_BG);
        lv_coord_t font_h              = lv_font_get_line_height(font);
        lv_coord_t h                   = lv_obj_get_height(roller);

        lv_obj_t * label = get_label(roller);

        lv_coord_t line_y1 = ext->sel_opt_id * (font_h + line_space) + label->coords.y1 - roller_scrl->coords.y1;
        lv_coord_t new_y = -line_y1 + (h - font_h) / 2;
        lv_obj_set_y(roller_scrl, new_y);
    }
}

static lv_obj_t * get_label(const lv_obj_t * roller)
{
    lv_obj_t * scrl = lv_page_get_scrollable(roller);
    if(scrl == NULL) return NULL;   /*The roller is being deleted, the scrollable already not exists*/
    return lv_obj_get_child(scrl, NULL);
}

#if LV_USE_ANIMATION
static void scroll_anim_ready_cb(lv_anim_t * a)
{
    inf_normalize(a->var);
}
#endif

#endif
