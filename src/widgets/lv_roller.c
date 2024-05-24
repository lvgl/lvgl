/**
 * @file lv_roller.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_roller.h"
#if LV_USE_ROLLER != 0

#include "../misc/lv_assert.h"
#include "../draw/lv_draw.h"
#include "../core/lv_group.h"
#include "../core/lv_indev.h"
#include "../core/lv_indev_scroll.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_roller_class
#define MY_CLASS_LABEL &lv_roller_label_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_roller_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_roller_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_roller_label_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_main(lv_event_t * e);
static void draw_label(lv_event_t * e);
static void get_sel_area(lv_obj_t * obj, lv_area_t * sel_area);
static void refr_position(lv_obj_t * obj, lv_anim_enable_t animen);
static lv_res_t release_handler(lv_obj_t * obj);
static void inf_normalize(lv_obj_t * obj_scrl);
static lv_obj_t * get_label(const lv_obj_t * obj);
static lv_coord_t get_selected_label_width(const lv_obj_t * obj);
static void scroll_anim_ready_cb(lv_anim_t * a);
static void set_y_anim(void * obj, int32_t v);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_roller_class = {
    .constructor_cb = lv_roller_constructor,
    .event_cb = lv_roller_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_DPI_DEF,
    .instance_size = sizeof(lv_roller_t),
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .base_class = &lv_obj_class
};

const lv_obj_class_t lv_roller_label_class  = {
    .event_cb = lv_roller_label_event,
    .instance_size = sizeof(lv_label_t),
    .base_class = &lv_label_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a roller object
 * @param parent pointer to an object, it will be the parent of the new roller
 * @return pointer to the created roller
 */
lv_obj_t * lv_roller_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
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
void lv_roller_set_options(lv_obj_t * obj, const char * options, lv_roller_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(options);

    lv_roller_t * roller = (lv_roller_t *)obj;
    lv_obj_t * label = get_label(obj);

    roller->sel_opt_id     = 0;
    roller->sel_opt_id_ori = 0;

    /*Count the '\n'-s to determine the number of options*/
    roller->option_cnt = 0;
    uint32_t cnt;
    for(cnt = 0; options[cnt] != '\0'; cnt++) {
        if(options[cnt] == '\n') roller->option_cnt++;
    }
    roller->option_cnt++; /*Last option has no `\n`*/

    if(mode == LV_ROLLER_MODE_NORMAL) {
        roller->mode = LV_ROLLER_MODE_NORMAL;
        lv_label_set_text(label, options);
    }
    else {
        roller->mode = LV_ROLLER_MODE_INFINITE;

        size_t opt_len = strlen(options) + 1; /*+1 to add '\n' after option lists*/
        char * opt_extra = lv_mem_buf_get(opt_len * LV_ROLLER_INF_PAGES);
        uint8_t i;
        for(i = 0; i < LV_ROLLER_INF_PAGES; i++) {
            strcpy(&opt_extra[opt_len * i], options);
            opt_extra[opt_len * (i + 1) - 1] = '\n';
        }
        opt_extra[opt_len * LV_ROLLER_INF_PAGES - 1] = '\0';
        lv_label_set_text(label, opt_extra);
        lv_mem_buf_release(opt_extra);

        roller->sel_opt_id     = ((LV_ROLLER_INF_PAGES / 2) + 0) * roller->option_cnt;

        roller->option_cnt = roller->option_cnt * LV_ROLLER_INF_PAGES;
        inf_normalize(obj);
    }

    roller->sel_opt_id_ori = roller->sel_opt_id;

    /*If the selected text has larger font the label needs some extra draw padding to draw it.*/
    lv_obj_refresh_ext_draw_size(label);

}

/**
 * Set the selected option
 * @param roller pointer to a roller object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 * @param anim_en LV_ANIM_ON: set with animation; LV_ANOM_OFF set immediately
 */
void lv_roller_set_selected(lv_obj_t * obj, uint16_t sel_opt, lv_anim_enable_t anim)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    /*Set the value even if it's the same as the current value because
     *if moving to the next option with an animation which was just deleted in the PRESS Call the ancestor's event handler
     *nothing will continue the animation.*/

    lv_roller_t * roller = (lv_roller_t *)obj;

    /*In infinite mode interpret the new ID relative to the currently visible "page"*/
    if(roller->mode == LV_ROLLER_MODE_INFINITE) {
        uint32_t real_option_cnt = roller->option_cnt / LV_ROLLER_INF_PAGES;
        uint16_t current_page = roller->sel_opt_id / real_option_cnt;
        /*Set by the user to e.g. 0, 1, 2, 3...
         *Upscale the value to the current page*/
        if(sel_opt < real_option_cnt) {
            uint16_t act_opt = roller->sel_opt_id - current_page * real_option_cnt;
            int32_t sel_opt_signed = sel_opt;
            /*Huge jump? Probably from last to first or first to last option.*/
            if(LV_ABS((int16_t)act_opt - sel_opt) > real_option_cnt / 2) {
                if(act_opt > sel_opt) sel_opt_signed += real_option_cnt;
                else sel_opt_signed -= real_option_cnt;
            }
            sel_opt = sel_opt_signed + real_option_cnt * current_page;
        }
    }

    roller->sel_opt_id     = sel_opt < roller->option_cnt ? sel_opt : roller->option_cnt - 1;
    roller->sel_opt_id_ori = roller->sel_opt_id;

    refr_position(obj, anim);
}

/**
 * Set the height to show the given number of rows (options)
 * @param roller pointer to a roller object
 * @param row_cnt number of desired visible rows
 */
void lv_roller_set_visible_row_count(lv_obj_t * obj, uint8_t row_cnt)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_obj_set_height(obj, (lv_font_get_line_height(font) + line_space) * row_cnt + 2 * border_width);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the id of the selected option
 * @param roller pointer to a roller object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t lv_roller_get_selected(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_roller_t * roller = (lv_roller_t *)obj;
    if(roller->mode == LV_ROLLER_MODE_INFINITE) {
        uint16_t real_id_cnt = roller->option_cnt / LV_ROLLER_INF_PAGES;
        return roller->sel_opt_id % real_id_cnt;
    }
    else {
        return roller->sel_opt_id;
    }
}

/**
 * Get the current selected option as a string
 * @param ddlist pointer to ddlist object
 * @param buf pointer to an array to store the string
 * @param buf_size size of `buf` in bytes. 0: to ignore it.
 */
void lv_roller_get_selected_str(const lv_obj_t * obj, char * buf, uint32_t buf_size)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_roller_t * roller = (lv_roller_t *)obj;
    lv_obj_t * label = get_label(obj);
    uint32_t i;
    uint16_t line        = 0;
    const char * opt_txt = lv_label_get_text(label);
    size_t txt_len     = strlen(opt_txt);

    for(i = 0; i < txt_len && line != roller->sel_opt_id; i++) {
        if(opt_txt[i] == '\n') line++;
    }

    uint32_t c;
    for(c = 0; i < txt_len && opt_txt[i] != '\n'; c++, i++) {
        if(buf_size && c >= buf_size - 1) {
            LV_LOG_WARN("lv_dropdown_get_selected_str: the buffer was too small");
            break;
        }
        buf[c] = opt_txt[i];
    }

    buf[c] = '\0';
}

/**
 * Get the options of a roller
 * @param roller pointer to roller object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * lv_roller_get_options(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return lv_label_get_text(get_label(obj));
}

/**
 * Get the total number of options
 * @param roller pointer to a roller object
 * @return the total number of options
 */
uint16_t lv_roller_get_option_cnt(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_roller_t * roller = (lv_roller_t *)obj;
    if(roller->mode == LV_ROLLER_MODE_INFINITE) {
        return roller->option_cnt / LV_ROLLER_INF_PAGES;
    }
    else {
        return roller->option_cnt;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_roller_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_roller_t * roller = (lv_roller_t *)obj;

    roller->mode = LV_ROLLER_MODE_NORMAL;
    roller->option_cnt = 0;
    roller->sel_opt_id = 0;
    roller->sel_opt_id_ori = 0;

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN_VER);

    LV_LOG_INFO("begin");
    lv_obj_t * label = lv_obj_class_create_obj(&lv_roller_label_class, obj);
    lv_obj_class_init_obj(label);
    lv_roller_set_options(obj, "Option 1\nOption 2\nOption 3\nOption 4\nOption 5", LV_ROLLER_MODE_NORMAL);

    LV_LOG_TRACE("finshed");
}

static void lv_roller_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_roller_t * roller = (lv_roller_t *)obj;

    if(code == LV_EVENT_GET_SELF_SIZE) {
        lv_point_t * p = lv_event_get_param(e);
        p->x = get_selected_label_width(obj);
    }
    else if(code == LV_EVENT_STYLE_CHANGED) {
        lv_obj_t * label = get_label(obj);
        /*Be sure the label's style is updated before processing the roller*/
        if(label) lv_event_send(label, LV_EVENT_STYLE_CHANGED, NULL);
        lv_obj_refresh_self_size(obj);
        refr_position(obj, LV_ANIM_OFF);
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        refr_position(obj, LV_ANIM_OFF);
    }
    else if(code == LV_EVENT_PRESSED) {
        roller->moved = 0;
        lv_anim_del(get_label(obj), set_y_anim);
    }
    else if(code == LV_EVENT_PRESSING) {
        lv_indev_t * indev = lv_indev_get_act();
        lv_point_t p;
        lv_indev_get_vect(indev, &p);
        if(p.y) {
            lv_obj_t * label = get_label(obj);
            lv_obj_set_y(label, lv_obj_get_y_aligned(label) + p.y);
            roller->moved = 1;
        }
    }
    else if(code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        release_handler(obj);
    }
    else if(code == LV_EVENT_FOCUSED) {
        lv_group_t * g             = lv_obj_get_group(obj);
        bool editing               = lv_group_get_editing(g);
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

        /*Encoders need special handling*/
        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            /*In navigate mode revert the original value*/
            if(!editing) {
                if(roller->sel_opt_id != roller->sel_opt_id_ori) {
                    roller->sel_opt_id = roller->sel_opt_id_ori;
                    refr_position(obj, LV_ANIM_ON);
                }
            }
            /*Save the current state when entered to edit mode*/
            else {
                roller->sel_opt_id_ori = roller->sel_opt_id;
            }
        }
        else {
            roller->sel_opt_id_ori = roller->sel_opt_id; /*Save the current value. Used to revert this state if
                                                                    ENTER won't be pressed*/
        }
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        /*Revert the original state*/
        if(roller->sel_opt_id != roller->sel_opt_id_ori) {
            roller->sel_opt_id = roller->sel_opt_id_ori;
            refr_position(obj, LV_ANIM_ON);
        }
    }
    else if(code == LV_EVENT_KEY) {
        char c = *((char *)lv_event_get_param(e));
        if(c == LV_KEY_RIGHT || c == LV_KEY_DOWN) {
            if(roller->sel_opt_id + 1 < roller->option_cnt) {
                uint16_t ori_id = roller->sel_opt_id_ori; /*lv_roller_set_selected will overwrite this*/
                lv_roller_set_selected(obj, roller->sel_opt_id + 1, LV_ANIM_ON);
                roller->sel_opt_id_ori = ori_id;
            }
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_UP) {
            if(roller->sel_opt_id > 0) {
                uint16_t ori_id = roller->sel_opt_id_ori; /*lv_roller_set_selected will overwrite this*/

                lv_roller_set_selected(obj, roller->sel_opt_id - 1, LV_ANIM_ON);
                roller->sel_opt_id_ori = ori_id;
            }
        }
    }
    else if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_obj_t * label = get_label(obj);
        lv_obj_refresh_ext_draw_size(label);
    }
    else if(code == LV_EVENT_DRAW_MAIN || code == LV_EVENT_DRAW_POST) {
        draw_main(e);
    }
}

static void lv_roller_label_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    lv_event_code_t code = lv_event_get_code(e);
    /*LV_EVENT_DRAW_MAIN will be called in the draw function*/
    if(code != LV_EVENT_DRAW_MAIN) {
        /* Call the ancestor's event handler */
        res = lv_obj_event_base(MY_CLASS_LABEL, e);
        if(res != LV_RES_OK) return;
    }

    lv_obj_t * label = lv_event_get_target(e);
    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        /*If the selected text has a larger font it needs some extra space to draw it*/
        lv_coord_t * s = lv_event_get_param(e);
        lv_obj_t * obj = lv_obj_get_parent(label);
        lv_coord_t sel_w = get_selected_label_width(obj);
        lv_coord_t label_w = lv_obj_get_width(label);
        *s = LV_MAX(*s, sel_w - label_w);
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        refr_position(lv_obj_get_parent(label), LV_ANIM_OFF);
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        draw_label(e);
    }
}

static void draw_main(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_DRAW_MAIN) {
        /*Draw the selected rectangle*/
        lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);
        lv_area_t sel_area;
        get_sel_area(obj, &sel_area);
        lv_draw_rect_dsc_t sel_dsc;
        lv_draw_rect_dsc_init(&sel_dsc);
        lv_obj_init_draw_rect_dsc(obj, LV_PART_SELECTED, &sel_dsc);
        lv_draw_rect(draw_ctx, &sel_dsc, &sel_area);
    }
    /*Post draw when the children are drawn*/
    else if(code == LV_EVENT_DRAW_POST) {
        lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        lv_obj_init_draw_label_dsc(obj, LV_PART_SELECTED, &label_dsc);

        /*Redraw the text on the selected area*/
        lv_area_t sel_area;
        get_sel_area(obj, &sel_area);
        lv_area_t mask_sel;
        bool area_ok;
        area_ok = _lv_area_intersect(&mask_sel, draw_ctx->clip_area, &sel_area);
        if(area_ok) {
            lv_obj_t * label = get_label(obj);
            if(lv_label_get_recolor(label)) label_dsc.flag |= LV_TEXT_FLAG_RECOLOR;

            /*Get the size of the "selected text"*/
            lv_point_t label_sel_size;
            lv_txt_get_size(&label_sel_size, lv_label_get_text(label), label_dsc.font, label_dsc.letter_space,
                            label_dsc.line_space, lv_obj_get_width(obj), LV_TEXT_FLAG_EXPAND);

            /*Move the selected label proportionally with the background label*/
            lv_coord_t roller_h = lv_obj_get_height(obj);
            const lv_font_t * normal_label_font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
            /*label offset from the middle line of the roller*/
            int32_t label_y_prop = (label->coords.y1 + normal_label_font->line_height / 2) - (roller_h / 2 + obj->coords.y1);

            /*Proportional position from the middle line.
             *Will be 0 for the first option, and 1 for the last option (upscaled by << 14)*/
            lv_coord_t remain_h = lv_obj_get_height(label) - normal_label_font->line_height;
            if(remain_h > 0) {
                label_y_prop = (label_y_prop << 14) / remain_h;
            }

            /*We don't want the selected label start and end exactly where the normal label is as
             *a larger font won't centered on selected area.*/
            int32_t corr = label_dsc.font->line_height;

            /*Apply the proportional position to the selected text*/
            int32_t label_sel_y = roller_h / 2 + obj->coords.y1;
            label_sel_y += ((label_sel_size.y - corr) * label_y_prop) >> 14;
            label_sel_y -= corr / 2;

            lv_coord_t bwidth = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
            lv_coord_t pleft = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
            lv_coord_t pright = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);

            /*Draw the selected text*/
            lv_area_t label_sel_area;
            label_sel_area.x1 = obj->coords.x1 + pleft + bwidth;
            label_sel_area.y1 = label_sel_y;
            label_sel_area.x2 = obj->coords.x2 - pright - bwidth;
            label_sel_area.y2 = label_sel_area.y1 + label_sel_size.y;

            label_dsc.flag |= LV_TEXT_FLAG_EXPAND;
            const lv_area_t * clip_area_ori = draw_ctx->clip_area;
            draw_ctx->clip_area = &mask_sel;
            lv_draw_label(draw_ctx, &label_dsc, &label_sel_area, lv_label_get_text(label), NULL);
            draw_ctx->clip_area = clip_area_ori;
        }
    }
}

static void draw_label(lv_event_t * e)
{
    /* Split the drawing of the label into  an upper (above the selected area)
     * and a lower (below the selected area)*/
    lv_obj_t * label_obj = lv_event_get_target(e);
    lv_obj_t * roller = lv_obj_get_parent(label_obj);
    lv_draw_label_dsc_t label_draw_dsc;
    lv_draw_label_dsc_init(&label_draw_dsc);
    lv_obj_init_draw_label_dsc(roller, LV_PART_MAIN, &label_draw_dsc);
    if(lv_label_get_recolor(label_obj)) label_draw_dsc.flag |= LV_TEXT_FLAG_RECOLOR;

    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);

    /*If the roller has shadow or outline it has some ext. draw size
     *therefore the label can overflow the roller's boundaries.
     *To solve this limit the clip area to the "plain" roller.*/
    const lv_area_t * clip_area_ori = draw_ctx->clip_area;
    lv_area_t roller_clip_area;
    if(!_lv_area_intersect(&roller_clip_area, draw_ctx->clip_area, &roller->coords)) return;
    draw_ctx->clip_area = &roller_clip_area;

    lv_area_t sel_area;
    get_sel_area(roller, &sel_area);

    lv_area_t clip2;
    clip2.x1 = label_obj->coords.x1;
    clip2.y1 = label_obj->coords.y1;
    clip2.x2 = label_obj->coords.x2;
    clip2.y2 = sel_area.y1;
    if(_lv_area_intersect(&clip2, draw_ctx->clip_area, &clip2)) {
        const lv_area_t * clip_area_ori2 = draw_ctx->clip_area;
        draw_ctx->clip_area = &clip2;
        lv_draw_label(draw_ctx, &label_draw_dsc, &label_obj->coords, lv_label_get_text(label_obj), NULL);
        draw_ctx->clip_area = clip_area_ori2;
    }

    clip2.x1 = label_obj->coords.x1;
    clip2.y1 = sel_area.y2;
    clip2.x2 = label_obj->coords.x2;
    clip2.y2 = label_obj->coords.y2;
    if(_lv_area_intersect(&clip2, draw_ctx->clip_area, &clip2)) {
        const lv_area_t * clip_area_ori2 = draw_ctx->clip_area;
        draw_ctx->clip_area = &clip2;
        lv_draw_label(draw_ctx, &label_draw_dsc, &label_obj->coords, lv_label_get_text(label_obj), NULL);
        draw_ctx->clip_area = clip_area_ori2;
    }

    draw_ctx->clip_area = clip_area_ori;
}

static void get_sel_area(lv_obj_t * obj, lv_area_t * sel_area)
{

    const lv_font_t * font_main = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    const lv_font_t * font_sel = lv_obj_get_style_text_font(obj, LV_PART_SELECTED);
    lv_coord_t font_main_h        = lv_font_get_line_height(font_main);
    lv_coord_t font_sel_h        = lv_font_get_line_height(font_sel);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    lv_coord_t d = (font_sel_h + font_main_h) / 2 + line_space;
    sel_area->y1 = obj->coords.y1 + lv_obj_get_height(obj) / 2 - d / 2;
    sel_area->y2 = sel_area->y1 + d;
    lv_area_t roller_coords;
    lv_obj_get_coords(obj, &roller_coords);

    sel_area->x1 = roller_coords.x1;
    sel_area->x2 = roller_coords.x2;

}

/**
 * Refresh the position of the roller. It uses the id stored in: roller->ddlist.selected_option_id
 * @param roller pointer to a roller object
 * @param anim_en LV_ANIM_ON: refresh with animation; LV_ANOM_OFF: without animation
 */
static void refr_position(lv_obj_t * obj, lv_anim_enable_t anim_en)
{
    lv_obj_t * label = get_label(obj);
    if(label == NULL) return;

    lv_text_align_t align = lv_obj_calculate_style_text_align(label, LV_PART_MAIN, lv_label_get_text(label));

    switch(align) {
        case LV_TEXT_ALIGN_CENTER:
            lv_obj_set_x(label, (lv_obj_get_content_width(obj) - lv_obj_get_width(label)) / 2);
            break;
        case LV_TEXT_ALIGN_RIGHT:
            lv_obj_set_x(label, lv_obj_get_content_width(obj) - lv_obj_get_width(label));
            break;
        case LV_TEXT_ALIGN_LEFT:
            lv_obj_set_x(label, 0);
            break;
    }

    lv_roller_t * roller = (lv_roller_t *)obj;
    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    lv_coord_t font_h              = lv_font_get_line_height(font);
    lv_coord_t h                   = lv_obj_get_content_height(obj);
    uint16_t anim_time             = lv_obj_get_style_anim_time(obj, LV_PART_MAIN);

    /*Normally the animation's `end_cb` sets correct position of the roller if infinite.
     *But without animations do it manually*/
    if(anim_en == LV_ANIM_OFF || anim_time == 0) {
        inf_normalize(obj);
    }

    int32_t id = roller->sel_opt_id;
    lv_coord_t sel_y1 = id * (font_h + line_space);
    lv_coord_t mid_y1 = h / 2 - font_h / 2;

    lv_coord_t new_y = mid_y1 - sel_y1;

    if(anim_en == LV_ANIM_OFF || anim_time == 0) {
        lv_anim_del(label, set_y_anim);
        lv_obj_set_y(label, new_y);
    }
    else {
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, label);
        lv_anim_set_exec_cb(&a, set_y_anim);
        lv_anim_set_values(&a, lv_obj_get_y(label), new_y);
        lv_anim_set_time(&a, anim_time);
        lv_anim_set_ready_cb(&a, scroll_anim_ready_cb);
        lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
        lv_anim_start(&a);
    }
}

static lv_res_t release_handler(lv_obj_t * obj)
{
    lv_obj_t * label = get_label(obj);
    if(label == NULL) return LV_RES_OK;

    lv_indev_t * indev = lv_indev_get_act();
    lv_roller_t * roller = (lv_roller_t *)obj;

    /*Leave edit mode once a new option is selected*/
    lv_indev_type_t indev_type = lv_indev_get_type(indev);
    if(indev_type == LV_INDEV_TYPE_ENCODER || indev_type == LV_INDEV_TYPE_KEYPAD) {
        roller->sel_opt_id_ori = roller->sel_opt_id;

        if(indev_type == LV_INDEV_TYPE_ENCODER) {
            lv_group_t * g      = lv_obj_get_group(obj);
            if(lv_group_get_editing(g)) {
                lv_group_set_editing(g, false);
            }
        }
    }

    if(lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER || lv_indev_get_type(indev) == LV_INDEV_TYPE_BUTTON) {
        /*Search the clicked option (For KEYPAD and ENCODER the new value should be already set)*/
        int16_t new_opt  = -1;
        if(roller->moved == 0) {
            new_opt = 0;
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
        }
        else {
            /*If dragged then align the list to have an element in the middle*/
            const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
            lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
            lv_coord_t font_h              = lv_font_get_line_height(font);

            lv_coord_t label_unit = font_h + line_space;
            lv_coord_t mid        = obj->coords.y1 + (obj->coords.y2 - obj->coords.y1) / 2;
            lv_coord_t label_y1 = label->coords.y1 + lv_indev_scroll_throw_predict(indev, LV_DIR_VER);
            int32_t id = (mid - label_y1) / label_unit;

            if(id < 0) id = 0;
            if(id >= roller->option_cnt) id = roller->option_cnt - 1;

            new_opt = id;
        }

        if(new_opt >= 0) {
            lv_roller_set_selected(obj, new_opt, LV_ANIM_ON);
        }
    }

    uint32_t id  = roller->sel_opt_id; /*Just to use uint32_t in event data*/
    lv_res_t res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, &id);
    return res;
}

/**
 * Set the middle page for the roller if infinite is enabled
 * @param roller pointer to a roller object
 */
static void inf_normalize(lv_obj_t * obj)
{
    lv_roller_t * roller = (lv_roller_t *)obj;

    if(roller->mode == LV_ROLLER_MODE_INFINITE) {
        uint16_t real_id_cnt = roller->option_cnt / LV_ROLLER_INF_PAGES;
        roller->sel_opt_id = roller->sel_opt_id % real_id_cnt;
        roller->sel_opt_id += (LV_ROLLER_INF_PAGES / 2) * real_id_cnt; /*Select the middle page*/

        roller->sel_opt_id_ori = roller->sel_opt_id % real_id_cnt;
        roller->sel_opt_id_ori += (LV_ROLLER_INF_PAGES / 2) * real_id_cnt; /*Select the middle page*/

        /*Move to the new id*/
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
        lv_coord_t font_h              = lv_font_get_line_height(font);
        lv_coord_t h                   = lv_obj_get_content_height(obj);

        lv_obj_t * label = get_label(obj);

        lv_coord_t sel_y1 = roller->sel_opt_id * (font_h + line_space);
        lv_coord_t mid_y1 = h / 2 - font_h / 2;
        lv_coord_t new_y = mid_y1 - sel_y1;
        lv_obj_set_y(label, new_y);
    }
}

static lv_obj_t * get_label(const lv_obj_t * obj)
{
    return lv_obj_get_child(obj, 0);
}

static lv_coord_t get_selected_label_width(const lv_obj_t * obj)
{
    lv_obj_t * label = get_label(obj);
    if(label == NULL) return 0;

    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_SELECTED);
    lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_SELECTED);
    const char * txt = lv_label_get_text(label);
    lv_point_t size;
    lv_txt_get_size(&size, txt, font, letter_space, 0, LV_COORD_MAX,  LV_TEXT_FLAG_NONE);
    return size.x;
}

static void scroll_anim_ready_cb(lv_anim_t * a)
{
    lv_obj_t * obj = lv_obj_get_parent(a->var); /*The label is animated*/
    inf_normalize(obj);
}

static void set_y_anim(void * obj, int32_t v)
{
    lv_obj_set_y(obj, v);
}

#endif
