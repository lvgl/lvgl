/**
 * @file lv_obj_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "../display/lv_display.h"
#include "../display/lv_display_private.h"
#include "../misc/lv_color.h"
#include "../stdlib/lv_string.h"
#include "../core/lv_global.h"
/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_obj_class)
#define style_refr LV_GLOBAL_DEFAULT()->style_refresh
#define style_trans_ll_p &(LV_GLOBAL_DEFAULT()->style_trans_ll)
#define _style_custom_prop_flag_lookup_table LV_GLOBAL_DEFAULT()->style_custom_prop_flag_lookup_table
#define STYLE_PROP_SHIFTED(prop) ((uint32_t)1 << ((prop) >> 3))

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t * obj;
    lv_style_prop_t prop;
    lv_style_selector_t selector;
    lv_style_value_t start_value;
    lv_style_value_t end_value;
} trans_t;

typedef enum {
    CACHE_ZERO = 0,
    CACHE_TRUE = 1,
    CACHE_UNSET = 2,
    CACHE_255 = 3,
    CACHE_NEED_CHECK = 4,
} cache_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_style_t * get_local_style(lv_obj_t * obj, lv_style_selector_t selector);
static _lv_obj_style_t * get_trans_style(lv_obj_t * obj, uint32_t part);
static lv_style_res_t get_prop_core(const lv_obj_t * obj, lv_style_selector_t selector, lv_style_prop_t prop,
                                    lv_style_value_t * v);
static void report_style_change_core(void * style, lv_obj_t * obj);
static void refresh_children_style(lv_obj_t * obj);
static bool trans_delete(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop, trans_t * tr_limit);
static void trans_anim_cb(void * _tr, int32_t v);
static void trans_anim_start_cb(lv_anim_t * a);
static void trans_anim_completed_cb(lv_anim_t * a);
static lv_layer_type_t calculate_layer_type(lv_obj_t * obj);
static void full_cache_refresh(lv_obj_t * obj, lv_part_t part);
static void fade_anim_cb(void * obj, int32_t v);
static void fade_in_anim_completed(lv_anim_t * a);
static bool style_has_flag(const lv_style_t * style, uint32_t flag);
static lv_style_res_t get_selector_style_prop(const lv_obj_t * obj, lv_style_selector_t selector, lv_style_prop_t prop,
                                              lv_style_value_t * value_act);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_obj_style_init(void)
{
    _lv_ll_init(style_trans_ll_p, sizeof(trans_t));
}

void _lv_obj_style_deinit(void)
{
    _lv_ll_clear(style_trans_ll_p);
    if(_style_custom_prop_flag_lookup_table != NULL) {
        lv_free(_style_custom_prop_flag_lookup_table);
        _style_custom_prop_flag_lookup_table = NULL;
    }
}

void lv_obj_add_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector)
{
    LV_ASSERT(obj->style_cnt < 63);

    trans_delete(obj, selector, LV_STYLE_PROP_ANY, NULL);

    lv_part_t part = lv_obj_style_get_selector_part(selector);

    if(style && part == LV_PART_MAIN && style_has_flag(style, LV_STYLE_PROP_FLAG_TRANSFORM)) {
        lv_obj_invalidate(obj);
    }

    /*Try removing the style first to be sure it won't be added twice*/
    lv_obj_remove_style(obj, style, selector);

    uint32_t i;
    /*Go after the transition and local styles*/
    for(i = 0; i < obj->style_cnt; i++) {
        if(obj->styles[i].is_trans) continue;
        if(obj->styles[i].is_local) continue;
        break;
    }

    /*Now `i` is at the first normal style. Insert the new style before this*/

    /*Allocate space for the new style and shift the rest of the style to the end*/
    obj->style_cnt++;
    LV_ASSERT(obj->style_cnt != 0);
    obj->styles = lv_realloc(obj->styles, obj->style_cnt * sizeof(_lv_obj_style_t));
    LV_ASSERT_MALLOC(obj->styles);

    uint32_t j;
    for(j = obj->style_cnt - 1; j > i ; j--) {
        obj->styles[j] = obj->styles[j - 1];
    }

    lv_memzero(&obj->styles[i], sizeof(_lv_obj_style_t));
    obj->styles[i].style = style;
    obj->styles[i].selector = selector;

#if LV_OBJ_STYLE_CACHE
    uint32_t * prop_is_set = part == LV_PART_MAIN ? &obj->style_main_prop_is_set : &obj->style_other_prop_is_set;
    if(lv_style_is_const(style)) {
        lv_style_const_prop_t * props = style->values_and_props;
        for(i = 0; props[i].prop_ptr; i++) {
            (*prop_is_set) |= STYLE_PROP_SHIFTED(*props[i].prop_ptr);
        }
    }
    else {
        lv_style_prop_t * props = (lv_style_prop_t *)style->values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
        for(i = 0; i < style->prop_cnt; i++) {
            (*prop_is_set) |= STYLE_PROP_SHIFTED(props[i]);
        }
    }
#endif

    lv_obj_refresh_style(obj, selector, LV_STYLE_PROP_ANY);
}

bool lv_obj_replace_style(lv_obj_t * obj, const lv_style_t * old_style, const lv_style_t * new_style,
                          lv_style_selector_t selector)
{
    lv_state_t state = lv_obj_style_get_selector_state(selector);
    lv_part_t part = lv_obj_style_get_selector_part(selector);

    /*All objects must exist*/
    if(!obj || !old_style || !new_style || (old_style == new_style)) {
        return false;
    }

    /*Similar to lv_obj_add_style, delete transition*/
    trans_delete(obj, selector, LV_STYLE_PROP_ANY, NULL);

    bool replaced = false;
    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        lv_state_t state_act = lv_obj_style_get_selector_state(obj->styles[i].selector);
        lv_part_t part_act = lv_obj_style_get_selector_part(obj->styles[i].selector);

        /*Skip local styles and transitions*/
        if(obj->styles[i].is_local || obj->styles[i].is_trans) {
            continue;
        }

        /*Skip non-matching styles*/
        if((state != LV_STATE_ANY && state_act != state) ||
           (part != LV_PART_ANY && part_act != part) ||
           (old_style != obj->styles[i].style)) {
            continue;
        }

        lv_memzero(&obj->styles[i], sizeof(_lv_obj_style_t));
        obj->styles[i].style = new_style;
        obj->styles[i].selector = selector;

        replaced = true;
        /*Don't break and continue replacing other occurrences*/
    }
    if(replaced) {
        full_cache_refresh(obj, part);
        lv_obj_refresh_style(obj, part, LV_STYLE_PROP_ANY);
    }
    return replaced;
}

void lv_obj_remove_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector)
{
    lv_state_t state = lv_obj_style_get_selector_state(selector);
    lv_part_t part = lv_obj_style_get_selector_part(selector);
    lv_style_prop_t prop = LV_STYLE_PROP_ANY;
    if(style && style->prop_cnt == 0) prop = LV_STYLE_PROP_INV;

    if(style && part == LV_PART_MAIN && style_has_flag(style, LV_STYLE_PROP_FLAG_TRANSFORM)) {
        lv_obj_invalidate(obj);
    }

    uint32_t i = 0;
    bool deleted = false;
    while(i <  obj->style_cnt) {
        lv_state_t state_act = lv_obj_style_get_selector_state(obj->styles[i].selector);
        lv_part_t part_act = lv_obj_style_get_selector_part(obj->styles[i].selector);
        if((state != LV_STATE_ANY && state_act != state) ||
           (part != LV_PART_ANY && part_act != part) ||
           (style != NULL && style != obj->styles[i].style)) {
            i++;
            continue;
        }

        if(obj->styles[i].is_trans) {
            trans_delete(obj, part, LV_STYLE_PROP_ANY, NULL);
        }

        if(obj->styles[i].is_local || obj->styles[i].is_trans) {
            if(obj->styles[i].style) lv_style_reset((lv_style_t *)obj->styles[i].style);
            lv_free((lv_style_t *)obj->styles[i].style);
            obj->styles[i].style = NULL;
        }

        /*Shift the styles after `i` by one*/
        uint32_t j;
        for(j = i; j < (uint32_t)obj->style_cnt - 1 ; j++) {
            obj->styles[j] = obj->styles[j + 1];
        }

        obj->style_cnt--;
        obj->styles = lv_realloc(obj->styles, obj->style_cnt * sizeof(_lv_obj_style_t));

        deleted = true;
        /*The style from the current `i` index is removed, so `i` points to the next style.
         *Therefore it doesn't needs to be incremented*/
    }

    if(deleted && prop != LV_STYLE_PROP_INV) {
        full_cache_refresh(obj, part);
        lv_obj_refresh_style(obj, part, prop);
    }
}

void lv_obj_remove_style_all(lv_obj_t * obj)
{
    lv_obj_remove_style(obj, NULL, LV_PART_ANY | LV_STATE_ANY);
}

void lv_obj_report_style_change(lv_style_t * style)
{
    if(!style_refr) return;
    lv_display_t * d = lv_display_get_next(NULL);

    while(d) {
        uint32_t i;
        for(i = 0; i < d->screen_cnt; i++) {
            report_style_change_core(style, d->screens[i]);
        }
        d = lv_display_get_next(d);
    }
}

void lv_obj_refresh_style(lv_obj_t * obj, lv_style_selector_t selector, lv_style_prop_t prop)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(!style_refr) return;

    lv_obj_invalidate(obj);

    lv_part_t part = lv_obj_style_get_selector_part(selector);

    bool is_layout_refr = lv_style_prop_has_flag(prop, LV_STYLE_PROP_FLAG_LAYOUT_UPDATE);
    bool is_ext_draw = lv_style_prop_has_flag(prop, LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE);
    bool is_inheritable = lv_style_prop_has_flag(prop, LV_STYLE_PROP_FLAG_INHERITABLE);
    bool is_layer_refr = lv_style_prop_has_flag(prop, LV_STYLE_PROP_FLAG_LAYER_UPDATE);

    if(is_layout_refr) {
        if(part == LV_PART_ANY ||
           part == LV_PART_MAIN ||
           lv_obj_get_style_height(obj, 0) == LV_SIZE_CONTENT ||
           lv_obj_get_style_width(obj, 0) == LV_SIZE_CONTENT) {
            lv_obj_send_event(obj, LV_EVENT_STYLE_CHANGED, NULL);
            lv_obj_mark_layout_as_dirty(obj);
        }
    }
    if((part == LV_PART_ANY || part == LV_PART_MAIN) && (prop == LV_STYLE_PROP_ANY || is_layout_refr)) {
        lv_obj_t * parent = lv_obj_get_parent(obj);
        if(parent) lv_obj_mark_layout_as_dirty(parent);
    }

    /*Cache the layer type*/
    if((part == LV_PART_ANY || part == LV_PART_MAIN) && is_layer_refr) {
        lv_layer_type_t layer_type = calculate_layer_type(obj);
        if(obj->spec_attr) obj->spec_attr->layer_type = layer_type;
        else if(layer_type != LV_LAYER_TYPE_NONE) {
            lv_obj_allocate_spec_attr(obj);
            obj->spec_attr->layer_type = layer_type;
        }
    }

    if(prop == LV_STYLE_PROP_ANY || is_ext_draw) {
        lv_obj_refresh_ext_draw_size(obj);
    }
    lv_obj_invalidate(obj);

    if(prop == LV_STYLE_PROP_ANY || (is_inheritable && (is_ext_draw || is_layout_refr))) {
        if(part != LV_PART_SCROLLBAR) {
            refresh_children_style(obj);
        }
    }
}

void lv_obj_enable_style_refresh(bool en)
{
    style_refr = en;
}

static inline lv_style_value_t lv_style_prop_get_default_inlined(lv_style_prop_t prop)
{
    const lv_color_t black = LV_COLOR_MAKE(0x00, 0x00, 0x00);
    const lv_color_t white = LV_COLOR_MAKE(0xff, 0xff, 0xff);
    switch(prop) {
        case LV_STYLE_TRANSFORM_SCALE_X:
        case LV_STYLE_TRANSFORM_SCALE_Y:
            return (lv_style_value_t) {
                .num = LV_SCALE_NONE
            };
        case LV_STYLE_BG_COLOR:
            return (lv_style_value_t) {
                .color = black
            };
        case LV_STYLE_BG_GRAD_COLOR:
        case LV_STYLE_BORDER_COLOR:
        case LV_STYLE_SHADOW_COLOR:
        case LV_STYLE_OUTLINE_COLOR:
        case LV_STYLE_ARC_COLOR:
        case LV_STYLE_LINE_COLOR:
        case LV_STYLE_TEXT_COLOR:
        case LV_STYLE_IMAGE_RECOLOR:
            return (lv_style_value_t) {
                .color = white
            };
        case LV_STYLE_OPA:
        case LV_STYLE_OPA_LAYERED:
        case LV_STYLE_BORDER_OPA:
        case LV_STYLE_TEXT_OPA:
        case LV_STYLE_IMAGE_OPA:
        case LV_STYLE_BG_GRAD_OPA:
        case LV_STYLE_BG_MAIN_OPA:
        case LV_STYLE_BG_IMAGE_OPA:
        case LV_STYLE_OUTLINE_OPA:
        case LV_STYLE_SHADOW_OPA:
        case LV_STYLE_LINE_OPA:
        case LV_STYLE_ARC_OPA:
            return (lv_style_value_t) {
                .num = LV_OPA_COVER
            };
        case LV_STYLE_BG_GRAD_STOP:
            return (lv_style_value_t) {
                .num = 255
            };
        case LV_STYLE_BORDER_SIDE:
            return (lv_style_value_t) {
                .num = LV_BORDER_SIDE_FULL
            };
        case LV_STYLE_TEXT_FONT:
            return (lv_style_value_t) {
                .ptr = LV_FONT_DEFAULT
            };
        case LV_STYLE_MAX_WIDTH:
        case LV_STYLE_MAX_HEIGHT:
            return (lv_style_value_t) {
                .num = LV_COORD_MAX
            };
        default:
            return (lv_style_value_t) {
                .ptr = 0
            };
    }
}

lv_style_value_t lv_obj_get_style_prop(const lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop)
{
    LV_ASSERT_NULL(obj)

    lv_style_selector_t selector = part | obj->state;
    lv_style_value_t value_act = { .ptr = NULL };
    lv_style_res_t found;

    found = get_selector_style_prop(obj, selector, prop, &value_act);
    if(found == LV_STYLE_RES_FOUND) return value_act;

    return lv_style_prop_get_default_inlined(prop);
}

bool lv_obj_has_style_prop(const lv_obj_t * obj, lv_style_selector_t selector, lv_style_prop_t prop)
{
    LV_ASSERT_NULL(obj)

    lv_style_value_t value_act = { .ptr = NULL };
    lv_style_res_t found;

    found = get_selector_style_prop(obj, selector, prop, &value_act);
    if(found == LV_STYLE_RES_FOUND) return true;

    return false;
}

void lv_obj_set_local_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_value_t value,
                                 lv_style_selector_t selector)
{
    lv_style_t * style = get_local_style(obj, selector);
    if(selector == LV_PART_MAIN && lv_style_prop_has_flag(prop, LV_STYLE_PROP_FLAG_TRANSFORM)) {
        lv_obj_invalidate(obj);
    }

    lv_style_set_prop(style, prop, value);

#if LV_OBJ_STYLE_CACHE
    uint32_t prop_shifted = STYLE_PROP_SHIFTED(prop);
    if(lv_obj_style_get_selector_part(selector) == LV_PART_MAIN) {
        obj->style_main_prop_is_set |= prop_shifted;
    }
    else {
        obj->style_other_prop_is_set |= prop_shifted;
    }
#endif

    lv_obj_refresh_style(obj, selector, prop);
}

lv_style_res_t lv_obj_get_local_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_value_t * value,
                                           lv_style_selector_t selector)
{
    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        if(obj->styles[i].is_local &&
           obj->styles[i].selector == selector) {
            return lv_style_get_prop(obj->styles[i].style, prop, value);
        }
    }

    return LV_STYLE_RES_NOT_FOUND;
}

bool lv_obj_remove_local_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_selector_t selector)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    uint32_t i;
    /*Find the style*/
    for(i = 0; i < obj->style_cnt; i++) {
        if(obj->styles[i].is_local &&
           obj->styles[i].selector == selector) {
            break;
        }
    }

    /*The style is not found*/
    if(i == obj->style_cnt) return false;

    lv_result_t res = lv_style_remove_prop((lv_style_t *)obj->styles[i].style, prop);
    if(res == LV_RESULT_OK) {
        full_cache_refresh(obj, lv_obj_style_get_selector_part(selector));
        lv_obj_refresh_style(obj, selector, prop);
    }

    return res;
}

void _lv_obj_style_create_transition(lv_obj_t * obj, lv_part_t part, lv_state_t prev_state, lv_state_t new_state,
                                     const _lv_obj_style_transition_dsc_t * tr_dsc)
{
    trans_t * tr;

    /*Get the previous and current values*/
    obj->skip_trans = 1;
    obj->state = prev_state;
    lv_style_value_t v1 = lv_obj_get_style_prop(obj, part, tr_dsc->prop);
    obj->state = new_state;
    lv_style_value_t v2 = lv_obj_get_style_prop(obj, part, tr_dsc->prop);
    obj->skip_trans = 0;

    if(v1.ptr == v2.ptr && v1.num == v2.num && lv_color_eq(v1.color, v2.color))  return;
    obj->state = prev_state;
    v1 = lv_obj_get_style_prop(obj, part, tr_dsc->prop);
    obj->state = new_state;

    _lv_obj_style_t * style_trans = get_trans_style(obj, part);
    lv_style_set_prop((lv_style_t *)style_trans->style, tr_dsc->prop, v1);  /*Be sure `trans_style` has a valid value*/

    if(tr_dsc->prop == LV_STYLE_RADIUS) {
        if(v1.num == LV_RADIUS_CIRCLE || v2.num == LV_RADIUS_CIRCLE) {
            int32_t whalf = lv_obj_get_width(obj) / 2;
            int32_t hhalf = lv_obj_get_height(obj) / 2;
            if(v1.num == LV_RADIUS_CIRCLE) v1.num = LV_MIN(whalf + 1, hhalf + 1);
            if(v2.num == LV_RADIUS_CIRCLE) v2.num = LV_MIN(whalf + 1, hhalf + 1);
        }
    }

    tr = _lv_ll_ins_head(style_trans_ll_p);
    LV_ASSERT_MALLOC(tr);
    if(tr == NULL) return;
    tr->start_value = v1;
    tr->end_value = v2;
    tr->obj = obj;
    tr->prop = tr_dsc->prop;
    tr->selector = part;

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, tr);
    lv_anim_set_exec_cb(&a, trans_anim_cb);
    lv_anim_set_start_cb(&a, trans_anim_start_cb);
    lv_anim_set_completed_cb(&a, trans_anim_completed_cb);
    lv_anim_set_values(&a, 0x00, 0xFF);
    lv_anim_set_duration(&a, tr_dsc->time);
    lv_anim_set_delay(&a, tr_dsc->delay);
    lv_anim_set_path_cb(&a, tr_dsc->path_cb);
    lv_anim_set_early_apply(&a, false);
    lv_anim_set_user_data(&a, tr_dsc->user_data);
    lv_anim_start(&a);
}

lv_style_value_t _lv_obj_style_apply_color_filter(const lv_obj_t * obj, uint32_t part, lv_style_value_t v)
{
    if(obj == NULL) return v;
    const lv_color_filter_dsc_t * f = lv_obj_get_style_color_filter_dsc(obj, part);
    if(f && f->filter_cb) {
        lv_opa_t f_opa = lv_obj_get_style_color_filter_opa(obj, part);
        if(f_opa != 0) v.color = f->filter_cb(f, v.color, f_opa);
    }
    return v;
}

_lv_style_state_cmp_t _lv_obj_style_state_compare(lv_obj_t * obj, lv_state_t state1, lv_state_t state2)
{
    _lv_style_state_cmp_t res = _LV_STYLE_STATE_CMP_SAME;

    /*Are there any new styles for the new state?*/
    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        if(obj->styles[i].is_trans) continue;

        lv_state_t state_act = lv_obj_style_get_selector_state(obj->styles[i].selector);
        /*The style is valid for a state but not the other*/
        bool valid1 = state_act & (~state1) ? false : true;
        bool valid2 = state_act & (~state2) ? false : true;
        if(valid1 != valid2) {
            const lv_style_t * style = obj->styles[i].style;
            lv_style_value_t v;
            /*If there is layout difference on the main part, return immediately. There is no more serious difference*/
            bool layout_diff = false;
            if(lv_style_get_prop(style, LV_STYLE_PAD_TOP, &v))layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_PAD_BOTTOM, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_PAD_LEFT, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_PAD_RIGHT, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_PAD_COLUMN, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_PAD_ROW, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_LAYOUT, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_TRANSLATE_X, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_TRANSLATE_Y, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_WIDTH, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_HEIGHT, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_MIN_WIDTH, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_MAX_WIDTH, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_MIN_HEIGHT, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_MAX_HEIGHT, &v)) layout_diff = true;
            else if(lv_style_get_prop(style, LV_STYLE_BORDER_WIDTH, &v)) layout_diff = true;

            if(layout_diff) {
                return _LV_STYLE_STATE_CMP_DIFF_LAYOUT;
            }

            /*Check for draw pad changes*/
            if(lv_style_get_prop(style, LV_STYLE_TRANSFORM_WIDTH, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_TRANSFORM_HEIGHT, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_TRANSFORM_ROTATION, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_TRANSFORM_SCALE_X, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_TRANSFORM_SCALE_Y, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_OUTLINE_OPA, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_OUTLINE_PAD, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_OUTLINE_WIDTH, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_SHADOW_WIDTH, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_SHADOW_OPA, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_SHADOW_OFFSET_X, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_SHADOW_OFFSET_Y, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_SHADOW_SPREAD, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_LINE_WIDTH, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(res == _LV_STYLE_STATE_CMP_SAME) res = _LV_STYLE_STATE_CMP_DIFF_REDRAW;
        }
    }

    return res;
}

void lv_obj_fade_in(lv_obj_t * obj, uint32_t time, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 0, LV_OPA_COVER);
    lv_anim_set_exec_cb(&a, fade_anim_cb);
    lv_anim_set_completed_cb(&a, fade_in_anim_completed);
    lv_anim_set_duration(&a, time);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
}

void lv_obj_fade_out(lv_obj_t * obj, uint32_t time, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, lv_obj_get_style_opa(obj, 0), LV_OPA_TRANSP);
    lv_anim_set_exec_cb(&a, fade_anim_cb);
    lv_anim_set_duration(&a, time);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
}

lv_text_align_t lv_obj_calculate_style_text_align(const lv_obj_t * obj, lv_part_t part, const char * txt)
{
    lv_text_align_t align = lv_obj_get_style_text_align(obj, part);
    lv_base_dir_t base_dir = lv_obj_get_style_base_dir(obj, part);
    lv_bidi_calculate_align(&align, &base_dir, txt);
    return align;
}

lv_opa_t lv_obj_get_style_opa_recursive(const lv_obj_t * obj, lv_part_t part)
{

    lv_opa_t opa_obj = lv_obj_get_style_opa(obj, part);
    if(opa_obj <= LV_OPA_MIN) return LV_OPA_TRANSP;

    lv_opa_t opa_final = LV_OPA_COVER;
    if(opa_obj < LV_OPA_MAX) {
        opa_final = LV_OPA_MIX2(opa_final, opa_obj);
    }

    if(part != LV_PART_MAIN) {
        part = LV_PART_MAIN;
    }
    else {
        obj = lv_obj_get_parent(obj);
    }

    while(obj) {
        opa_obj = lv_obj_get_style_opa(obj, part);
        if(opa_obj <= LV_OPA_MIN) return LV_OPA_TRANSP;
        if(opa_obj < LV_OPA_MAX) {
            opa_final = LV_OPA_MIX2(opa_final, opa_obj);
        }

        obj = lv_obj_get_parent(obj);
    }

    if(opa_final <= LV_OPA_MIN) return LV_OPA_TRANSP;
    if(opa_final >= LV_OPA_MAX) return LV_OPA_COVER;
    return opa_final;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Get the local style of an object for a given part and for a given state.
 * If the local style for the part-state pair doesn't exist allocate and return it.
 * @param obj pointer to an object
 * @param selector OR-ed value of parts and state for which the style should be get
 * @return pointer to the local style
 */
static lv_style_t * get_local_style(lv_obj_t * obj, lv_style_selector_t selector)
{
    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        if(obj->styles[i].is_local &&
           obj->styles[i].selector == selector) {
            return (lv_style_t *)obj->styles[i].style;
        }
    }

    obj->style_cnt++;
    LV_ASSERT(obj->style_cnt != 0);
    obj->styles = lv_realloc(obj->styles, obj->style_cnt * sizeof(_lv_obj_style_t));
    LV_ASSERT_MALLOC(obj->styles);

    for(i = obj->style_cnt - 1; i > 0 ; i--) {
        /*Copy only normal styles (not local and transition).
         *The new local style will be added as the last local style*/
        if(obj->styles[i - 1].is_local || obj->styles[i - 1].is_trans) break;
        obj->styles[i] = obj->styles[i - 1];
    }

    lv_memzero(&obj->styles[i], sizeof(_lv_obj_style_t));
    obj->styles[i].style = lv_malloc(sizeof(lv_style_t));
    lv_style_init((lv_style_t *)obj->styles[i].style);

    obj->styles[i].is_local = 1;
    obj->styles[i].selector = selector;
    return (lv_style_t *)obj->styles[i].style;
}

/**
 * Get the transition style of an object for a given part and for a given state.
 * If the transition style for the part-state pair doesn't exist allocate and return it.
 * @param obj   pointer to an object
 * @param selector OR-ed value of parts and state for which the style should be get
 * @return pointer to the transition style
 */
static _lv_obj_style_t * get_trans_style(lv_obj_t * obj,  lv_style_selector_t selector)
{
    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        if(obj->styles[i].is_trans && obj->styles[i].selector == selector) break;
    }

    /*Already have a transition style for it*/
    if(i != obj->style_cnt) return &obj->styles[i];

    obj->style_cnt++;
    LV_ASSERT(obj->style_cnt != 0);
    obj->styles = lv_realloc(obj->styles, obj->style_cnt * sizeof(_lv_obj_style_t));

    for(i = obj->style_cnt - 1; i > 0 ; i--) {
        obj->styles[i] = obj->styles[i - 1];
    }

    lv_memzero(&obj->styles[0], sizeof(_lv_obj_style_t));
    obj->styles[0].style = lv_malloc(sizeof(lv_style_t));
    lv_style_init((lv_style_t *)obj->styles[0].style);

    obj->styles[0].is_trans = 1;
    obj->styles[0].selector = selector;
    return &obj->styles[0];
}

static lv_style_res_t get_prop_core(const lv_obj_t * obj, lv_style_selector_t selector, lv_style_prop_t prop,
                                    lv_style_value_t * v)
{

    const uint32_t group = (uint32_t)1 << _lv_style_get_prop_group(prop);
    const lv_part_t part = lv_obj_style_get_selector_part(selector);
    const lv_state_t state = lv_obj_style_get_selector_state(selector);
    const lv_state_t state_inv = ~state;
    const bool skip_trans = obj->skip_trans;
    int32_t weight = -1;
    lv_style_res_t found;
    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        _lv_obj_style_t * obj_style = &obj->styles[i];
        if(obj_style->is_trans == false) break;
        if(skip_trans) continue;

        lv_part_t part_act = lv_obj_style_get_selector_part(obj->styles[i].selector);

        if(part_act != part) continue;
        if((obj_style->style->has_group & group) == 0) continue;
        found = lv_style_get_prop_inlined(obj_style->style, prop, v);
        if(found == LV_STYLE_RES_FOUND) {
            return LV_STYLE_RES_FOUND;
        }
    }

    for(; i < obj->style_cnt; i++) {
        if((obj->styles[i].style->has_group & group) == 0) continue;
        _lv_obj_style_t * obj_style = &obj->styles[i];
        lv_part_t part_act = lv_obj_style_get_selector_part(obj->styles[i].selector);
        if(part_act != part) continue;

        /*Be sure the style not specifies other state than the requested.
         *E.g. For HOVER+PRESS object state, HOVER style only is OK, but HOVER+FOCUS style is not*/
        lv_state_t state_act = lv_obj_style_get_selector_state(obj->styles[i].selector);
        if((state_act & state_inv)) continue;

        /*Check only better candidates*/
        if(state_act <= weight) continue;

        found = lv_style_get_prop_inlined(obj_style->style, prop, v);
        if(found == LV_STYLE_RES_FOUND) {
            if(state_act == state) {
                return LV_STYLE_RES_FOUND;
            }
            weight = state_act;
        }
    }

    if(weight >= 0) return LV_STYLE_RES_FOUND;
    else return LV_STYLE_RES_NOT_FOUND;
}

/**
 * Refresh the style of all children of an object. (Called recursively)
 * @param style refresh objects only with this
 * @param obj pointer to an object
 */
static void report_style_change_core(void * style, lv_obj_t * obj)
{
    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        if(style == NULL || obj->styles[i].style == style) {
            full_cache_refresh(obj, lv_obj_style_get_selector_part(obj->styles[i].selector));
            lv_obj_refresh_style(obj, LV_PART_ANY, LV_STYLE_PROP_ANY);
            break;
        }
    }

    uint32_t child_cnt = lv_obj_get_child_count(obj);
    for(i = 0; i < child_cnt; i++) {
        report_style_change_core(style, obj->spec_attr->children[i]);
    }
}

/**
 * Recursively refresh the style of the children. Go deeper until a not NULL style is found
 * because the NULL styles are inherited from the parent
 * @param obj pointer to an object
 */
static void refresh_children_style(lv_obj_t * obj)
{
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_count(obj);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = obj->spec_attr->children[i];
        lv_obj_invalidate(child);
        lv_obj_send_event(child, LV_EVENT_STYLE_CHANGED, NULL);
        lv_obj_invalidate(child);

        refresh_children_style(child); /*Check children too*/
    }
}

/**
 * Remove the transition from object's part's property.
 * - Remove the transition from `_lv_obj_style_trans_ll` and free it
 * - Delete pending transitions
 * @param obj pointer to an object which transition(s) should be removed
 * @param part a part of object or 0xFF to remove from all parts
 * @param prop a property or 0xFF to remove all properties
 * @param tr_limit delete transitions only "older" than this. `NULL` if not used
 */
static bool trans_delete(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop, trans_t * tr_limit)
{
    trans_t * tr;
    trans_t * tr_prev;
    bool removed = false;
    tr = _lv_ll_get_tail(style_trans_ll_p);
    while(tr != NULL) {
        if(tr == tr_limit) break;

        /*'tr' might be deleted, so get the next object while 'tr' is valid*/
        tr_prev = _lv_ll_get_prev(style_trans_ll_p, tr);

        if(tr->obj == obj && (part == tr->selector || part == LV_PART_ANY) && (prop == tr->prop || prop == LV_STYLE_PROP_ANY)) {
            /*Remove any transitioned properties from the trans. style
             *to allow changing it by normal styles*/
            uint32_t i;
            for(i = 0; i < obj->style_cnt; i++) {
                if(obj->styles[i].is_trans && (part == LV_PART_ANY || obj->styles[i].selector == part)) {
                    lv_style_remove_prop((lv_style_t *)obj->styles[i].style, tr->prop);
                }
            }

            /*Free the transition descriptor too*/
            lv_anim_delete(tr, NULL);
            _lv_ll_remove(style_trans_ll_p, tr);
            lv_free(tr);
            removed = true;

        }
        tr = tr_prev;
    }
    return removed;
}

static void trans_anim_cb(void * _tr, int32_t v)
{
    trans_t * tr = _tr;
    lv_obj_t * obj = tr->obj;

    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        if(obj->styles[i].is_trans == 0 || obj->styles[i].selector != tr->selector) continue;

        lv_style_value_t value_final = {0};
        switch(tr->prop) {

            case LV_STYLE_BORDER_SIDE:
            case LV_STYLE_BORDER_POST:
            case LV_STYLE_BLEND_MODE:
                if(v < 255) value_final.num = tr->start_value.num;
                else value_final.num = tr->end_value.num;
                break;
            case LV_STYLE_TRANSITION:
            case LV_STYLE_TEXT_FONT:
                if(v < 255) value_final.ptr = tr->start_value.ptr;
                else value_final.ptr = tr->end_value.ptr;
                break;
            case LV_STYLE_COLOR_FILTER_DSC:
                if(tr->start_value.ptr == NULL) value_final.ptr = tr->end_value.ptr;
                else if(tr->end_value.ptr == NULL) value_final.ptr = tr->start_value.ptr;
                else if(v < 128) value_final.ptr = tr->start_value.ptr;
                else value_final.ptr = tr->end_value.ptr;
                break;
            case LV_STYLE_BG_COLOR:
            case LV_STYLE_BG_GRAD_COLOR:
            case LV_STYLE_BORDER_COLOR:
            case LV_STYLE_TEXT_COLOR:
            case LV_STYLE_SHADOW_COLOR:
            case LV_STYLE_OUTLINE_COLOR:
            case LV_STYLE_IMAGE_RECOLOR:
                if(v <= 0) value_final.color = tr->start_value.color;
                else if(v >= 255) value_final.color = tr->end_value.color;
                else value_final.color = lv_color_mix(tr->end_value.color, tr->start_value.color, v);
                break;

            default:
                if(v == 0) value_final.num = tr->start_value.num;
                else if(v == 255) value_final.num = tr->end_value.num;
                else value_final.num = tr->start_value.num + ((int32_t)((int32_t)(tr->end_value.num - tr->start_value.num) * v) >> 8);
                break;
        }

        lv_style_value_t old_value = {0};
        bool refr = true;
        if(lv_style_get_prop(obj->styles[i].style, tr->prop, &old_value)) {
            if(value_final.ptr == old_value.ptr && lv_color_eq(value_final.color, old_value.color) &&
               value_final.num == old_value.num) {
                refr = false;
            }
        }
        lv_style_set_prop((lv_style_t *)obj->styles[i].style, tr->prop, value_final);
        if(refr) lv_obj_refresh_style(tr->obj, tr->selector, tr->prop);
        break;

    }

}

static void trans_anim_start_cb(lv_anim_t * a)
{
    trans_t * tr = a->var;

    lv_part_t part = lv_obj_style_get_selector_part(tr->selector);
    tr->start_value = lv_obj_get_style_prop(tr->obj, part, tr->prop);

    /*Init prop to an invalid values to be sure `trans_del` won't delete this added `tr`*/
    lv_style_prop_t prop_tmp = tr->prop;
    tr->prop = LV_STYLE_PROP_INV;

    /*Delete the related transitions if any*/
    trans_delete(tr->obj, part, prop_tmp, tr);

    tr->prop = prop_tmp;

    _lv_obj_style_t * style_trans = get_trans_style(tr->obj, tr->selector);
    lv_style_set_prop((lv_style_t *)style_trans->style, tr->prop,
                      tr->start_value);  /*Be sure `trans_style` has a valid value*/

}

static void trans_anim_completed_cb(lv_anim_t * a)
{
    trans_t * tr = a->var;
    lv_obj_t * obj = tr->obj;
    lv_style_prop_t prop = tr->prop;

    /*Remove the transitioned property from trans. style
     *if there no more transitions for this property
     *It allows changing it by normal styles*/
    bool running = false;
    trans_t * tr_i;
    _LV_LL_READ(style_trans_ll_p, tr_i) {
        if(tr_i != tr && tr_i->obj == tr->obj && tr_i->selector == tr->selector && tr_i->prop == tr->prop) {
            running = true;
            break;
        }
    }

    if(!running) {
        uint32_t i;
        for(i = 0; i < obj->style_cnt; i++) {
            if(obj->styles[i].is_trans && obj->styles[i].selector == tr->selector) {
                _lv_ll_remove(style_trans_ll_p, tr);
                lv_free(tr);

                _lv_obj_style_t * obj_style = &obj->styles[i];
                lv_style_remove_prop((lv_style_t *)obj_style->style, prop);

                if(lv_style_is_empty(obj->styles[i].style)) {
                    lv_obj_remove_style(obj, (lv_style_t *)obj_style->style, obj_style->selector);

                }
                break;
            }
        }
    }
}

static lv_layer_type_t calculate_layer_type(lv_obj_t * obj)
{
    if(lv_obj_get_style_transform_rotation(obj, 0) != 0) return LV_LAYER_TYPE_TRANSFORM;
    if(lv_obj_get_style_transform_scale_x(obj, 0) != 256) return LV_LAYER_TYPE_TRANSFORM;
    if(lv_obj_get_style_transform_scale_y(obj, 0) != 256) return LV_LAYER_TYPE_TRANSFORM;
    if(lv_obj_get_style_transform_skew_x(obj, 0) != 0) return LV_LAYER_TYPE_TRANSFORM;
    if(lv_obj_get_style_transform_skew_y(obj, 0) != 0) return LV_LAYER_TYPE_TRANSFORM;
    if(lv_obj_get_style_opa_layered(obj, 0) != LV_OPA_COVER) return LV_LAYER_TYPE_SIMPLE;
    if(lv_obj_get_style_bitmap_mask_src(obj, 0) != NULL) return LV_LAYER_TYPE_SIMPLE;
    if(lv_obj_get_style_blend_mode(obj, 0) != LV_BLEND_MODE_NORMAL) return LV_LAYER_TYPE_SIMPLE;
    return LV_LAYER_TYPE_NONE;
}

static void full_cache_refresh(lv_obj_t * obj, lv_part_t part)
{
#if LV_OBJ_STYLE_CACHE
    uint32_t i;
    if(part == LV_PART_MAIN || part == LV_PART_ANY) {
        obj->style_main_prop_is_set = 0;
        for(i = 0; i < obj->style_cnt; i++) {
            if(lv_obj_style_get_selector_part(obj->styles[i].selector) != LV_PART_MAIN) continue;
            lv_style_t * style = (lv_style_t *)obj->styles[i].style;
            uint32_t j;
            if(lv_style_is_const(style)) {
                lv_style_const_prop_t * props = style->values_and_props;
                for(j = 0; props[j].prop_ptr; j++) {
                    obj->style_main_prop_is_set |= STYLE_PROP_SHIFTED(*props[j].prop_ptr);
                }
            }
            else {
                lv_style_prop_t * props = (lv_style_prop_t *)style->values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
                for(j = 0; j < style->prop_cnt; j++) {
                    obj->style_main_prop_is_set |= STYLE_PROP_SHIFTED(props[j]);
                }
            }
        }
    }
    if(part != LV_PART_MAIN || part == LV_PART_ANY) {
        obj->style_other_prop_is_set = 0;
        for(i = 0; i < obj->style_cnt; i++) {
            if(lv_obj_style_get_selector_part(obj->styles[i].selector) == LV_PART_MAIN) continue;
            lv_style_t * style = (lv_style_t *)obj->styles[i].style;
            uint32_t j;
            if(lv_style_is_const(style)) {
                lv_style_const_prop_t * props = style->values_and_props;
                for(j = 0; props[j].prop_ptr; j++) {
                    obj->style_other_prop_is_set |= STYLE_PROP_SHIFTED(*props[j].prop_ptr);
                }
            }
            else {
                lv_style_prop_t * props = (lv_style_prop_t *)style->values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
                for(j = 0; j < style->prop_cnt; j++) {
                    obj->style_other_prop_is_set |= STYLE_PROP_SHIFTED(props[j]);
                }
            }
        }
    }
#else
    LV_UNUSED(obj);
    LV_UNUSED(part);
#endif
}

static void fade_anim_cb(void * obj, int32_t v)
{
    lv_obj_set_style_opa(obj, v, 0);
}

static void fade_in_anim_completed(lv_anim_t * a)
{
    lv_obj_remove_local_style_prop(a->var, LV_STYLE_OPA, 0);
}

static bool style_has_flag(const lv_style_t * style, uint32_t flag)
{
    if(lv_style_is_const(style)) {
        lv_style_const_prop_t * props = style->values_and_props;
        uint32_t i;
        for(i = 0; props[i].prop_ptr; i++) {
            if(lv_style_prop_has_flag(*props[i].prop_ptr, flag)) {
                return true;
            }
        }
    }
    else {
        lv_style_prop_t * props = (lv_style_prop_t *)style->values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
        uint32_t i;
        for(i = 0; i < style->prop_cnt; i++) {
            if(lv_style_prop_has_flag(props[i], flag)) {
                return true;
            }
        }
    }
    return false;
}

static lv_style_res_t get_selector_style_prop(const lv_obj_t * obj, lv_style_selector_t selector, lv_style_prop_t prop,
                                              lv_style_value_t * value_act)
{
    lv_style_res_t found;
    lv_part_t part = lv_obj_style_get_selector_part(selector);

    /*The happy path*/
#if LV_OBJ_STYLE_CACHE
    const uint32_t prop_shifted = STYLE_PROP_SHIFTED(prop);
    if((part == LV_PART_MAIN ? obj->style_main_prop_is_set : obj->style_other_prop_is_set) & prop_shifted)
#endif
    {
        found = get_prop_core(obj, selector, prop, value_act);
        if(found == LV_STYLE_RES_FOUND) return LV_STYLE_RES_FOUND;
    }

    extern const uint8_t _lv_style_builtin_prop_flag_lookup_table[];
    bool inheritable = false;
    if(prop < _LV_STYLE_NUM_BUILT_IN_PROPS) {
        inheritable = _lv_style_builtin_prop_flag_lookup_table[prop] & LV_STYLE_PROP_FLAG_INHERITABLE;
    }
    else {
        if(_style_custom_prop_flag_lookup_table != NULL) {
            inheritable = _style_custom_prop_flag_lookup_table[prop - _LV_STYLE_NUM_BUILT_IN_PROPS] &
                          LV_STYLE_PROP_FLAG_INHERITABLE;
        }
    }

    if(inheritable) {
        /*If not found, check the `MAIN` style first, if already on the MAIN part go to the parent*/
        if(part != LV_PART_MAIN) part = LV_PART_MAIN;
        else obj = obj->parent;

        while(obj) {
#if LV_OBJ_STYLE_CACHE
            if(obj->style_main_prop_is_set & prop_shifted)
#endif
            {
                selector = part | obj->state;
                found = get_prop_core(obj, selector, prop, value_act);
                if(found == LV_STYLE_RES_FOUND) return LV_STYLE_RES_FOUND;
            }
            /*Check the parent too.*/
            obj = obj->parent;
        }
    }
    else {
        /*Get the width and height from the class.
                * WIDTH and HEIGHT are not inherited so add them in the `else` to skip checking them for inherited properties */
        if(part == LV_PART_MAIN && (prop == LV_STYLE_WIDTH || prop == LV_STYLE_HEIGHT)) {
            const lv_obj_class_t * cls = obj->class_p;
            while(cls) {
                if(prop == LV_STYLE_WIDTH) {
                    if(cls->width_def != 0)  {
                        value_act->num = cls->width_def;
                        return LV_STYLE_RES_FOUND;
                    }
                }
                else {
                    if(cls->height_def != 0) {
                        value_act->num = cls->height_def;
                        return LV_STYLE_RES_FOUND;
                    }
                }
                cls = cls->base_class;
            }
        }
    }

    return LV_STYLE_RES_NOT_FOUND;
}
