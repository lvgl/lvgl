/**
 * @file lv_obj_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj_private.h"
#include "../lvgl_public.h"
#include "../misc/lv_anim_private.h"
#include "../misc/lv_bidi_private.h"
#include "../misc/lv_style_private.h"
#include "lv_obj_style_private.h"
#include "lv_obj_class_private.h"
#include "../display/lv_display_private.h"
#include "../core/lv_global.h"
#include "lv_observer_private.h"
#include "lv_obj_style_internal.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_obj_class)
#define style_refr LV_GLOBAL_DEFAULT()->style_refresh
#define _style_custom_prop_flag_lookup_table LV_GLOBAL_DEFAULT()->style_custom_prop_flag_lookup_table

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const lv_style_t * style;
    lv_style_selector_t selector;
    int32_t value;
} bind_style_t;

typedef struct {
    lv_style_prop_t prop;
    lv_style_selector_t selector;
} bind_style_prop_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_style_t * get_local_style(lv_obj_t * obj, lv_style_selector_t selector);
static lv_style_res_t get_prop_core(const lv_obj_t * obj, lv_style_selector_t selector, lv_style_prop_t prop,
                                    lv_style_value_t * v);
static void report_style_change_core(void * style, lv_obj_t * obj);
static void refresh_children_style(lv_obj_t * obj);
static lv_layer_type_t calculate_layer_type(lv_obj_t * obj);
static bool calculate_has_blur(lv_obj_t * obj);
static void fade_anim_cb(void * obj, int32_t v);
static void fade_in_anim_completed(lv_anim_t * a);
static bool style_has_flag(const lv_style_t * style, uint32_t flag);
static lv_style_res_t get_selector_style_prop(const lv_obj_t * obj, lv_style_selector_t selector, lv_style_prop_t prop,
                                              lv_style_value_t * value_act);
static bool remove_style_core(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector, bool theme_only);
static lv_obj_style_t * find_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector);
static void cache_add_style_props(lv_obj_t * obj, lv_part_t part, const lv_style_t * style);
#if LV_USE_OBSERVER
    static void bind_style_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
    static void bind_style_prop_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_obj_style_init(void)
{
    lv_obj_style_transition_init();
}

void lv_obj_style_deinit(void)
{
    lv_obj_style_transition_deinit();
    if(_style_custom_prop_flag_lookup_table != NULL) {
        lv_free(_style_custom_prop_flag_lookup_table);
        _style_custom_prop_flag_lookup_table = NULL;
    }
}

void lv_obj_add_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector)
{
    LV_CHECK_ARG(obj != NULL, return);
    LV_CHECK_ARG(style != NULL, return);
    LV_CHECK_ARG_MSG(obj->style_cnt < 63, return,
                     "obj->style_cnt is restricted to 6 bits, so we can't store more than 63 styles");

    lv_part_t part = lv_obj_style_get_selector_part(selector);

    /*If the draw size changes invalidate the old area first*/
    if(part == LV_PART_MAIN && style_has_flag(style, LV_OBJ_STYLE_DRAW_AREA_FLAGS)) {
        lv_obj_invalidate(obj);
    }

    /*Try removing the style first to be sure it won't be added twice.
     *Don't start transitions when the style is just re-added.*/
    bool readded = remove_style_core(obj, style, selector, false);

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
    obj->styles = lv_realloc(obj->styles, obj->style_cnt * sizeof(lv_obj_style_t));
    LV_ASSERT_MALLOC(obj->styles);

    uint32_t j;
    for(j = obj->style_cnt - 1; j > i ; j--) {
        obj->styles[j] = obj->styles[j - 1];
    }

    lv_memzero(&obj->styles[i], sizeof(lv_obj_style_t));
    obj->styles[i].style = style;
    obj->styles[i].selector = selector;

    cache_add_style_props(obj, part, style);

    /*Read the start and end values of the transitions before the running transitions are removed*/
    lv_obj_style_trans_batch_t batch = {0};
    if(!readded && i < 64) {
        lv_obj_style_change_t c = {
            .mask = (uint64_t)1 << i,
            .style_before = style,
            .style_after = style,
            .disabled_before = true,
            .disabled_after = false,
        };
        lv_obj_style_transition_prepare_change(obj, part, &c, &batch);
    }

    bool trans_removed = lv_obj_style_remove_transitions(obj, part, LV_STYLE_PROP_ANY);
    if(trans_removed) {
        lv_obj_style_full_cache_refresh(obj, LV_PART_ANY);
    }

    lv_obj_style_transition_start(obj, &batch);

    lv_obj_refresh_style(obj, part, LV_STYLE_PROP_ANY);
}

bool lv_obj_replace_style(lv_obj_t * obj, const lv_style_t * old_style, const lv_style_t * new_style,
                          lv_style_selector_t selector)
{
    LV_CHECK_ARG(obj != NULL, return false);
    LV_CHECK_ARG(old_style != NULL, return false);
    LV_CHECK_ARG(new_style != NULL, return false);
    LV_CHECK_ARG(old_style != new_style, return false);

    lv_state_t state = lv_obj_style_get_selector_state(selector);
    lv_part_t part = lv_obj_style_get_selector_part(selector);

    /*If the draw size changes invalidate the old area first*/
    if((part == LV_PART_MAIN || part == LV_PART_ANY) &&
       (style_has_flag(old_style, LV_OBJ_STYLE_DRAW_AREA_FLAGS) || style_has_flag(new_style, LV_OBJ_STYLE_DRAW_AREA_FLAGS))) {
        lv_obj_invalidate(obj);
    }

    bool replaced = false;
    uint64_t replaced_mask = 0;
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

        lv_style_selector_t selector_act = obj->styles[i].selector;
        lv_memzero(&obj->styles[i], sizeof(lv_obj_style_t));
        obj->styles[i].style = new_style;
        obj->styles[i].selector = selector_act;

        replaced = true;
        if(i < 64) replaced_mask |= (uint64_t)1 << i;
        /*Don't break and continue replacing other occurrences*/
    }

    /*Read the start and end values of the transitions before the running transitions are removed*/
    lv_obj_style_trans_batch_t batch = {0};
    if(replaced_mask) {
        cache_add_style_props(obj, part, new_style);
        lv_obj_style_change_t c = {
            .mask = replaced_mask,
            .style_before = old_style,
            .style_after = new_style,
            .disabled_before = false,
            .disabled_after = false,
        };
        lv_obj_style_transition_prepare_change(obj, part, &c, &batch);
    }

    /*Similar to lv_obj_add_style, delete transition*/
    bool trans_removed = lv_obj_style_remove_transitions(obj, part, LV_STYLE_PROP_ANY);

    if(replaced || trans_removed) {
        lv_obj_style_full_cache_refresh(obj, part);
    }
    lv_obj_style_transition_start(obj, &batch);
    if(replaced || trans_removed) {
        lv_obj_refresh_style(obj, part, LV_STYLE_PROP_ANY);
    }
    return replaced;
}

void lv_obj_remove_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector)
{
    LV_CHECK_ARG(obj != NULL, return);

    lv_part_t part = lv_obj_style_get_selector_part(selector);
    if(style != NULL && lv_obj_style_transition_allowed(obj, part)) {
        lv_state_t state = lv_obj_style_get_selector_state(selector);
        lv_obj_style_change_t c = {
            .mask = 0,
            .style_before = style,
            .style_after = style,
            .disabled_before = false,
            .disabled_after = true,
        };

        uint32_t i;
        for(i = 0; i < obj->style_cnt && i < 64; i++) {
            const lv_obj_style_t * obj_style = &obj->styles[i];
            if(obj_style->style != style) continue;
            if(obj_style->is_disabled) continue;
            if(lv_obj_style_get_selector_part(obj_style->selector) != part) continue;
            if(state != LV_STATE_ANY && lv_obj_style_get_selector_state(obj_style->selector) != state) continue;
            c.mask |= (uint64_t)1 << i;
        }

        if(c.mask) {
            /*Show how the widget would look like without the style*/
            lv_obj_style_change_apply(obj, &c, true);

            lv_obj_style_trans_batch_t batch = {0};
            lv_obj_style_transition_prepare_change(obj, part, &c, &batch);

            /*Restore the styles so that `remove_style_core` can invalidate the old area*/
            lv_obj_style_change_apply(obj, &c, false);
            lv_obj_style_transition_start(obj, &batch);
        }
    }

    remove_style_core(obj, style, selector, false);
}

void lv_obj_remove_theme(lv_obj_t * obj, lv_style_selector_t selector)
{
    LV_CHECK_ARG(obj != NULL, return);

    remove_style_core(obj, NULL, selector, true);
}

void lv_obj_remove_style_all(lv_obj_t * obj)
{
    LV_CHECK_ARG(obj != NULL, return);

    remove_style_core(obj, NULL, LV_PART_ANY | LV_STATE_ANY, false);
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

void lv_obj_refresh_style(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return);

    if(!style_refr) return;

    LV_PROFILER_STYLE_BEGIN;

    lv_obj_invalidate(obj);

    bool is_layout_refr = lv_style_prop_has_flag(prop, LV_STYLE_PROP_FLAG_LAYOUT_UPDATE);
    bool is_ext_draw = lv_style_prop_has_flag(prop, LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE);
    bool is_inheritable = lv_style_prop_has_flag(prop, LV_STYLE_PROP_FLAG_INHERITABLE);
    bool is_layer_refr = lv_style_prop_has_flag(prop, LV_STYLE_PROP_FLAG_LAYER_UPDATE);

    if(is_layout_refr) {
        if(part == LV_PART_ANY ||
           part == LV_PART_MAIN ||
           lv_obj_get_style_height_internal(obj, LV_PART_MAIN) == LV_SIZE_CONTENT ||
           lv_obj_get_style_width_internal(obj, LV_PART_MAIN) == LV_SIZE_CONTENT) {
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
        lv_obj_update_layer_type(obj);
    }

    /*Cache whether the widget has blur or drop shadow*/
    if(prop == LV_STYLE_PROP_ANY || prop == LV_STYLE_BLUR_RADIUS || prop == LV_STYLE_DROP_SHADOW_OPA) {
        lv_obj_update_blur_status(obj);
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

    LV_PROFILER_STYLE_END;
}

void lv_obj_set_style_enabled(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector, bool en)
{
    LV_CHECK_ARG(obj != NULL, return);
    LV_CHECK_ARG(style != NULL, return);

    lv_obj_style_t * obj_style = find_style(obj, style, selector);
    if(obj_style == NULL) return;

    if(en != obj_style->is_disabled) return; /*Already in the right state*/

    lv_part_t part = lv_obj_style_get_selector_part(selector);

    /*If the draw size changes invalidate the old area first*/
    if((part == LV_PART_MAIN || part == LV_PART_ANY) && style_has_flag(style, LV_OBJ_STYLE_DRAW_AREA_FLAGS)) {
        lv_obj_invalidate(obj);
    }

    obj_style->is_disabled = !en;

    /*Make sure the cache has the style's properties for both the enabled and disabled case*/
    cache_add_style_props(obj, part, style);

    lv_obj_style_trans_batch_t batch = {0};
    uint32_t idx = (uint32_t)(obj_style - obj->styles);
    if(idx < 64) {
        lv_obj_style_change_t c = {
            .mask = (uint64_t)1 << idx,
            .style_before = style,
            .style_after = style,
            .disabled_before = en,
            .disabled_after = !en,
        };
        lv_obj_style_transition_prepare_change(obj, part, &c, &batch);
    }

    lv_obj_style_full_cache_refresh(obj, part);
    lv_obj_style_transition_start(obj, &batch);
    lv_obj_refresh_style(obj, part, LV_STYLE_PROP_ANY);
}

bool lv_obj_get_style_enabled(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector)
{
    LV_CHECK_ARG(obj != NULL, return false);
    LV_CHECK_ARG(style != NULL, return false);

    lv_obj_style_t * obj_style = find_style(obj, style, selector);
    if(obj_style == NULL) return false;

    return !obj_style->is_disabled;
}


void lv_obj_enable_style_refresh(bool en)
{
    style_refr = en;
}

lv_style_value_t lv_obj_get_style_prop(const lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop)
{
    LV_CHECK_ARG(obj != NULL, return lv_style_prop_get_default(prop));
    return lv_obj_get_style_prop_internal(obj, part, prop);
}

lv_style_value_t lv_obj_get_style_prop_internal(const lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop)
{
    LV_ASSERT(obj != NULL);
    lv_style_selector_t selector = part | obj->state;
    lv_style_value_t value_act = { .ptr = NULL };
    lv_style_res_t found;

    found = get_selector_style_prop(obj, selector, prop, &value_act);
    if(found == LV_STYLE_RES_FOUND) return value_act;

    return lv_style_prop_get_default(prop);
}
lv_style_value_t lv_obj_style_apply_color_filter_internal(const lv_obj_t * obj, lv_part_t part, lv_style_value_t v)
{
#if LV_USE_COLOR_FILTER
    LV_ASSERT(obj != NULL);

    const lv_color_filter_dsc_t * f = lv_obj_get_style_color_filter_dsc_internal(obj, part);
    if(f && f->filter_cb) {
        lv_opa_t f_opa = lv_obj_get_style_color_filter_opa_internal(obj, part);
        if(f_opa != 0) v.color = f->filter_cb(f, v.color, f_opa);
    }
#else
    LV_UNUSED(obj);
    LV_UNUSED(part);
    LV_UNUSED(v);
#endif
    return v;
}

bool lv_obj_has_style_prop(const lv_obj_t * obj, lv_style_selector_t selector, lv_style_prop_t prop)
{
    LV_CHECK_ARG(obj != NULL, return false);

    lv_style_value_t value_act = { .ptr = NULL };
    lv_style_res_t found;

    found = get_selector_style_prop(obj, selector, prop, &value_act);
    if(found == LV_STYLE_RES_FOUND) return true;

    return false;
}

void lv_obj_set_local_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_value_t value,
                                 lv_style_selector_t selector)
{
    LV_CHECK_ARG(obj != NULL, return);

    LV_PROFILER_STYLE_BEGIN;

    /*If the draw size changes invalidate the old area first*/
    if(lv_obj_style_get_selector_part(selector) == LV_PART_MAIN &&
       lv_style_prop_has_flag(prop, LV_OBJ_STYLE_DRAW_AREA_FLAGS)) {
        lv_obj_invalidate(obj);
    }

    lv_part_t part = lv_obj_style_get_selector_part(selector);

    /*Save the currently shown value if the change should be animated*/
    lv_obj_style_trans_batch_t batch = {0};
    lv_obj_style_transition_prepare_local(obj, part, prop, &batch);

    /*Stop running transitions with this property */
    bool trans_removed = lv_obj_style_remove_transitions(obj, part, prop);

    lv_style_t * style = get_local_style(obj, selector);

    lv_style_set_prop(style, prop, value);

    if(trans_removed) {
        lv_obj_style_full_cache_refresh(obj, LV_PART_ANY);
    }
#if LV_OBJ_STYLE_CACHE
    else {
        uint32_t prop_shifted = LV_OBJ_STYLE_PROP_SHIFTED(prop);
        if(part == LV_PART_MAIN) {
            obj->style_main_prop_is_set |= prop_shifted;
        }
        else {
            obj->style_other_prop_is_set |= prop_shifted;
        }
    }
#endif

    lv_obj_style_transition_start(obj, &batch);

    lv_obj_refresh_style(obj, part, prop);
    LV_PROFILER_STYLE_END;
}

lv_style_res_t lv_obj_get_local_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_value_t * value,
                                           lv_style_selector_t selector)
{
    LV_CHECK_ARG(obj != NULL, return LV_STYLE_RES_NOT_FOUND);
    LV_CHECK_ARG(value != NULL, return LV_STYLE_RES_NOT_FOUND);

    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        if(obj->styles[i].is_local &&
           obj->styles[i].selector == selector) {
            return lv_style_get_prop_internal(obj->styles[i].style, prop, value);
        }
    }

    return LV_STYLE_RES_NOT_FOUND;
}

bool lv_obj_remove_local_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_selector_t selector)
{
    LV_CHECK_OBJ(obj, MY_CLASS, return 0);

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

    lv_part_t part = lv_obj_style_get_selector_part(selector);

    /*Save the currently shown value if the change should be animated*/
    lv_obj_style_trans_batch_t batch = {0};
    lv_style_value_t v_tmp;
    if(lv_style_get_prop_internal(obj->styles[i].style, prop, &v_tmp) == LV_STYLE_RES_FOUND) {
        lv_obj_style_transition_prepare_local(obj, part, prop, &batch);
    }

    lv_result_t res = lv_style_remove_prop((lv_style_t *)obj->styles[i].style, prop);
    if(res == LV_RESULT_OK) {
        lv_obj_style_full_cache_refresh(obj, part);
        lv_obj_style_transition_start(obj, &batch);
        lv_obj_refresh_style(obj, part, prop);
    }
    else {
        lv_obj_style_transition_start(obj, &batch); /*Nothing changed, only frees the batch*/
    }

    return res;
}



lv_style_value_t lv_obj_style_apply_color_filter(const lv_obj_t * obj, lv_part_t part, lv_style_value_t v)
{
    LV_CHECK_ARG(obj != NULL, return v);
    return lv_obj_style_apply_color_filter_internal(obj, part, v);
}

lv_style_state_cmp_t lv_obj_style_state_compare(lv_obj_t * obj, lv_state_t state1, lv_state_t state2,
                                                lv_part_t * changed_part)
{
    LV_CHECK_ARG(obj != NULL, return LV_STYLE_STATE_CMP_SAME);

    lv_style_state_cmp_t res = LV_STYLE_STATE_CMP_SAME;

    /*Accumulate which part the differing styles belong to: the lone differing
     *part, or LV_PART_ANY once styles from more than one part differ. Reported
     *via changed_part so the caller can scope the refresh to a single part.
     *diff_part_found is needed as a separate flag because a style's part can
     *itself be LV_PART_ANY, so LV_PART_ANY can't double as "none seen yet".*/
    lv_part_t diff_part = LV_PART_ANY;
    bool diff_part_found = false;

    /*Are there any new styles for the new state?*/
    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        if(obj->styles[i].is_trans) continue;

        lv_state_t state_act = lv_obj_style_get_selector_state(obj->styles[i].selector);
        /*The style is valid for a state but not the other*/
        bool valid1 = state_act & (~state1) ? false : true;
        bool valid2 = state_act & (~state2) ? false : true;
        if(valid1 != valid2) {
            /*Record this differing style's part: keep it if it's the only one
             *seen so far, otherwise collapse to LV_PART_ANY (more than one part).*/
            lv_part_t part_act = lv_obj_style_get_selector_part(obj->styles[i].selector);
            if(!diff_part_found) {
                diff_part = part_act;
                diff_part_found = true;
            }
            else if(part_act != diff_part) {
                diff_part = LV_PART_ANY;
            }

            const lv_style_t * style = obj->styles[i].style;
            lv_style_value_t v;
            /*If there is layout difference on the main part, return immediately. There is no more serious difference*/
            bool layout_diff = false;
            if(lv_style_get_prop_internal(style, LV_STYLE_PAD_TOP, &v))layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_PAD_BOTTOM, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_PAD_LEFT, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_PAD_RIGHT, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_PAD_COLUMN, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_PAD_ROW, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_LAYOUT, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_TRANSLATE_X, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_TRANSLATE_Y, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_WIDTH, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_HEIGHT, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_MIN_WIDTH, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_MAX_WIDTH, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_MIN_HEIGHT, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_MAX_HEIGHT, &v)) layout_diff = true;
            else if(lv_style_get_prop_internal(style, LV_STYLE_BORDER_WIDTH, &v)) layout_diff = true;

            if(layout_diff) {
                /*Returning early, so the full set of changed parts is unknown:
                 *report LV_PART_ANY rather than a possibly-incomplete part.*/
                if(changed_part) *changed_part = LV_PART_ANY;
                return LV_STYLE_STATE_CMP_DIFF_LAYOUT;
            }

            /*Check for draw pad changes*/
            if(lv_style_get_prop_internal(style, LV_STYLE_TRANSFORM_WIDTH, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_TRANSFORM_HEIGHT, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_TRANSFORM_ROTATION, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_TRANSFORM_SCALE_X, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_TRANSFORM_SCALE_Y, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_OUTLINE_OPA, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_OUTLINE_PAD, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_OUTLINE_WIDTH, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_SHADOW_WIDTH, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_SHADOW_OPA, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_SHADOW_OFFSET_X, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_SHADOW_OFFSET_Y, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_SHADOW_SPREAD, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop_internal(style, LV_STYLE_LINE_WIDTH, &v)) res = LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(res == LV_STYLE_STATE_CMP_SAME) res = LV_STYLE_STATE_CMP_DIFF_REDRAW;
        }
    }

    if(changed_part) *changed_part = diff_part;
    return res;
}

void lv_obj_fade_in(lv_obj_t * obj, uint32_t time, uint32_t delay)
{
    LV_CHECK_ARG(obj != NULL, return);

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
    LV_CHECK_ARG(obj != NULL, return);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, lv_obj_get_style_opa_internal(obj, LV_PART_MAIN), LV_OPA_TRANSP);
    lv_anim_set_exec_cb(&a, fade_anim_cb);
    lv_anim_set_duration(&a, time);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
}

lv_text_align_t lv_obj_calculate_style_text_align(const lv_obj_t * obj, lv_part_t part, const char * txt)
{
    LV_CHECK_ARG(obj != NULL, return LV_TEXT_ALIGN_AUTO);
    LV_CHECK_ARG(txt != NULL, return LV_TEXT_ALIGN_AUTO);
    return lv_obj_calculate_style_text_align_internal(obj, part, txt);
}

lv_opa_t lv_obj_get_style_opa_recursive(const lv_obj_t * obj, lv_part_t part)
{
    LV_CHECK_ARG(obj != NULL, return LV_OPA_TRANSP);

    LV_PROFILER_STYLE_BEGIN;
    lv_opa_t opa_obj = lv_obj_get_style_opa_internal(obj, part);
    if(opa_obj <= LV_OPA_MIN) {
        LV_PROFILER_STYLE_END;
        return LV_OPA_TRANSP;
    }

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
        opa_obj = lv_obj_get_style_opa_internal(obj, part);
        if(opa_obj <= LV_OPA_MIN) {
            LV_PROFILER_STYLE_END;
            return LV_OPA_TRANSP;
        }
        if(opa_obj < LV_OPA_MAX) {
            opa_final = LV_OPA_MIX2(opa_final, opa_obj);
        }

        obj = lv_obj_get_parent(obj);
    }

    if(opa_final <= LV_OPA_MIN) {
        LV_PROFILER_STYLE_END;
        return LV_OPA_TRANSP;
    }

    if(opa_final >= LV_OPA_MAX) {
        LV_PROFILER_STYLE_END;
        return LV_OPA_COVER;
    }

    LV_PROFILER_STYLE_END;
    return opa_final;
}

void lv_obj_update_layer_type(lv_obj_t * obj)
{
    LV_CHECK_ARG(obj != NULL, return);

    lv_layer_type_t layer_type = calculate_layer_type(obj);
    if(obj->spec_attr) obj->spec_attr->layer_type = layer_type;
    else if(layer_type != LV_LAYER_TYPE_NONE) {
        if(!lv_obj_allocate_spec_attr(obj)) {
            return;
        }
        obj->spec_attr->layer_type = layer_type;
    }
}

void lv_obj_update_blur_status(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);

    /*Deletion releases the count itself (see obj_delete_core())*/
    if(obj->is_deleting) return;

    bool has_blur = calculate_has_blur(obj);
    if(has_blur == (bool)obj->has_blur) return;

    obj->has_blur = has_blur;
    if(has_blur) {
        LV_GLOBAL_DEFAULT()->blur_obj_cnt++;
    }
    else {
        LV_ASSERT(LV_GLOBAL_DEFAULT()->blur_obj_cnt > 0);
        LV_GLOBAL_DEFAULT()->blur_obj_cnt--;
    }
}

lv_color32_t lv_obj_style_apply_recolor(const lv_obj_t * obj, lv_part_t part, lv_color32_t color)
{
    LV_CHECK_ARG(obj != NULL, return (lv_color32_t) {
        0
    });

    lv_opa_t opa = lv_obj_get_style_recolor_opa_internal(obj, part);
    if(opa > LV_OPA_TRANSP) {
        lv_color_t recolor = lv_obj_get_style_recolor_internal(obj, part);
        color = lv_color_over32(color, lv_color_to_32(recolor, opa));
    }

    return color;
}

lv_color32_t lv_obj_get_style_recolor_recursive(const lv_obj_t * obj, lv_part_t part)
{
    LV_CHECK_ARG(obj != NULL, return (lv_color32_t) {
        0
    });

    lv_color32_t result;

    lv_color_t color = lv_obj_get_style_recolor_internal(obj, part);
    lv_opa_t opa = lv_obj_get_style_recolor_opa_internal(obj, part);

    result = lv_color_to_32(color, opa);

    if(part != LV_PART_MAIN) {
        part = LV_PART_MAIN;
    }
    else {
        obj = lv_obj_get_parent(obj);
    }

    while(obj) {
        result = lv_obj_style_apply_recolor(obj, part, result);
        obj = lv_obj_get_parent(obj);
    }

    return result;
}

void lv_obj_set_style_pad_all(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return);
    lv_obj_set_style_pad_left(obj, value, selector);
    lv_obj_set_style_pad_right(obj, value, selector);
    lv_obj_set_style_pad_top(obj, value, selector);
    lv_obj_set_style_pad_bottom(obj, value, selector);
}

void lv_obj_set_style_pad_hor(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return);
    lv_obj_set_style_pad_left(obj, value, selector);
    lv_obj_set_style_pad_right(obj, value, selector);
}

void lv_obj_set_style_pad_ver(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return);
    lv_obj_set_style_pad_top(obj, value, selector);
    lv_obj_set_style_pad_bottom(obj, value, selector);
}

void lv_obj_set_style_margin_all(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return);
    lv_obj_set_style_margin_left(obj, value, selector);
    lv_obj_set_style_margin_right(obj, value, selector);
    lv_obj_set_style_margin_top(obj, value, selector);
    lv_obj_set_style_margin_bottom(obj, value, selector);
}

void lv_obj_set_style_margin_hor(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return);
    lv_obj_set_style_margin_left(obj, value, selector);
    lv_obj_set_style_margin_right(obj, value, selector);
}

void lv_obj_set_style_margin_ver(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return);
    lv_obj_set_style_margin_top(obj, value, selector);
    lv_obj_set_style_margin_bottom(obj, value, selector);
}

void lv_obj_set_style_pad_gap(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return);
    lv_obj_set_style_pad_row(obj, value, selector);
    lv_obj_set_style_pad_column(obj, value, selector);
}

void lv_obj_set_style_size(lv_obj_t * obj, int32_t width, int32_t height, lv_style_selector_t selector)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return);
    lv_obj_set_style_width(obj, width, selector);
    lv_obj_set_style_height(obj, height, selector);
}

void lv_obj_set_style_transform_scale(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return);
    lv_obj_set_style_transform_scale_x(obj, value, selector);
    lv_obj_set_style_transform_scale_y(obj, value, selector);
}

int32_t lv_obj_get_style_space_left(const lv_obj_t * obj, lv_part_t part)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return 0)
    return lv_obj_get_style_space_left_internal(obj, part);
}

int32_t lv_obj_get_style_space_right(const lv_obj_t * obj, lv_part_t part)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return 0)
    return lv_obj_get_style_space_right_internal(obj, part);
}

int32_t lv_obj_get_style_space_top(const lv_obj_t * obj, lv_part_t part)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return 0)
    return lv_obj_get_style_space_top_internal(obj, part);
}

int32_t lv_obj_get_style_space_bottom(const lv_obj_t * obj, lv_part_t part)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return 0)
    return lv_obj_get_style_space_bottom_internal(obj, part);
}

int32_t lv_obj_get_style_transform_scale_x_safe(const lv_obj_t * obj, lv_part_t part)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return 0)
    return lv_obj_get_style_transform_scale_x_safe_internal(obj, part);
}

int32_t lv_obj_get_style_transform_scale_y_safe(const lv_obj_t * obj, lv_part_t part)
{
    LV_CHECK_OBJ(obj, &lv_obj_class, return 0)
    return lv_obj_get_style_transform_scale_y_safe_internal(obj, part);
}


#if LV_USE_OBSERVER

lv_observer_t * lv_obj_bind_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector,
                                  lv_subject_t * subject, int32_t ref_value)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(style != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    if(subject->type != LV_SUBJECT_TYPE_INT) {
        LV_LOG_WARN("Subject type must be `int` (was %d)", subject->type);
        return NULL;
    }

    lv_obj_add_style(obj, style, selector);

    bind_style_t * p = lv_malloc(sizeof(bind_style_t));
    LV_ASSERT_MALLOC(p);
    if(p == NULL) {
        LV_LOG_WARN("Out of memory");
        return NULL;
    }

    p->style = style;
    p->selector = selector;
    p->value = ref_value;

    lv_observer_t * observable = lv_subject_add_observer_obj(subject, bind_style_observer_cb, obj, p);
    observable->auto_free_user_data = 1;
    return observable;
}

lv_observer_t * lv_obj_bind_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_selector_t selector,
                                       lv_subject_t * subject)
{
    LV_CHECK_ARG(obj != NULL, return NULL);
    LV_CHECK_ARG(subject != NULL, return NULL);

    if(subject->type != LV_SUBJECT_TYPE_INT && subject->type != LV_SUBJECT_TYPE_COLOR &&
       subject->type != LV_SUBJECT_TYPE_POINTER) {
        LV_LOG_WARN("Subject type must be `int`, `color`, or `pointer` (was %"LV_PRIu32")", (uint32_t) subject->type);
        return NULL;
    }

    bind_style_prop_t * p = lv_malloc(sizeof(bind_style_prop_t));
    LV_ASSERT_MALLOC(p);
    if(p == NULL) {
        LV_LOG_WARN("Out of memory");
        return NULL;
    }

    p->prop = prop;
    p->selector = selector;

    lv_observer_t * observable = lv_subject_add_observer_obj(subject, bind_style_prop_observer_cb, obj, p);
    observable->auto_free_user_data = 1;
    return observable;
}


#endif /*LV_USE_OBSERVER*/


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

    /*Allocate the individual style first so obj->styles stays valid on failure*/
    lv_style_t * new_style = lv_malloc_zeroed(sizeof(lv_style_t));
    if(new_style == NULL) {
        LV_LOG_WARN("couldn't allocate local style");
        return NULL;
    }

    obj->style_cnt++;
    LV_ASSERT(obj->style_cnt != 0);
    lv_obj_style_t * new_styles = lv_realloc(obj->styles, obj->style_cnt * sizeof(lv_obj_style_t));
    if(new_styles == NULL) {
        obj->style_cnt--;
        lv_free(new_style);
        LV_LOG_WARN("couldn't allocate styles");
        return NULL;
    }
    obj->styles = new_styles;

    for(i = obj->style_cnt - 1; i > 0 ; i--) {
        /*Copy only normal styles (not local and transition).
         *The new local style will be added as the last local style*/
        if(obj->styles[i - 1].is_local || obj->styles[i - 1].is_trans) break;
        obj->styles[i] = obj->styles[i - 1];
    }

    lv_memzero(&obj->styles[i], sizeof(lv_obj_style_t));
    obj->styles[i].style = new_style;
    lv_style_init((lv_style_t *)obj->styles[i].style);

    obj->styles[i].is_local = 1;
    obj->styles[i].selector = selector;
    return (lv_style_t *)obj->styles[i].style;
}


static lv_style_res_t get_prop_core(const lv_obj_t * obj, lv_style_selector_t selector, lv_style_prop_t prop,
                                    lv_style_value_t * v)
{

    const uint32_t group = (uint32_t)1 << lv_style_get_prop_group(prop);
    const lv_part_t part = lv_obj_style_get_selector_part(selector);
    const bool skip_trans = (const bool) obj->skip_trans;
    int32_t weight = -1;
    lv_style_res_t found;
    uint32_t i;
    /*Handle the transition styles first. They are at the beginning of the style list
     *as the have the highest precedence. It's a different for loop as in case of transitions
     *the state doesn't need to be checked. */
    for(i = 0; i < obj->style_cnt; i++) {
        lv_obj_style_t * obj_style = &obj->styles[i];
        if(obj_style->is_trans == false) break;
        if(skip_trans) continue;

        lv_part_t part_act = lv_obj_style_get_selector_part(obj->styles[i].selector);

        if(part_act != part) continue;
        if((obj_style->style->has_group & group) == 0) continue;
        found = lv_style_get_prop_internal(obj_style->style, prop, v);
        if(found == LV_STYLE_RES_FOUND) {
            return LV_STYLE_RES_FOUND;
        }
    }

    /*Check the normal local and global styles that are assigned to given state*/
    const lv_state_t state = lv_obj_style_get_selector_state(selector);
    const lv_state_t state_inv = ~state;
    for(; i < obj->style_cnt; i++) {
        if((obj->styles[i].style->has_group & group) == 0) continue;
        if(obj->styles[i].is_disabled) continue;
        lv_obj_style_t * obj_style = &obj->styles[i];
        lv_part_t part_act = lv_obj_style_get_selector_part(obj->styles[i].selector);
        if(part_act != part) continue;

        /*Be sure the style not specifies other state than the requested.
         *E.g. For HOVER+PRESS object state, HOVER style only is OK, but HOVER+FOCUS style is not*/
        lv_state_t state_act = lv_obj_style_get_selector_state(obj->styles[i].selector);
        if((state_act & state_inv)) continue;

        /*Check only better candidates*/
        if((int32_t)state_act <= weight) continue;

        found = lv_style_get_prop_internal(obj_style->style, prop, v);
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
            lv_obj_style_full_cache_refresh(obj, lv_obj_style_get_selector_part(obj->styles[i].selector));
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





static lv_layer_type_t calculate_layer_type(lv_obj_t * obj)
{
#if LV_DRAW_TRANSFORM_USE_MATRIX
    if(lv_obj_get_transform(obj) != NULL) return LV_LAYER_TYPE_TRANSFORM;
#endif
    if(lv_obj_get_style_transform_rotation_internal(obj, LV_PART_MAIN) != 0) return LV_LAYER_TYPE_TRANSFORM;
    if(lv_obj_get_style_transform_scale_x_internal(obj, LV_PART_MAIN) != 256) return LV_LAYER_TYPE_TRANSFORM;
    if(lv_obj_get_style_transform_scale_y_internal(obj, LV_PART_MAIN) != 256) return LV_LAYER_TYPE_TRANSFORM;
    if(lv_obj_get_style_transform_skew_x_internal(obj, LV_PART_MAIN) != 0) return LV_LAYER_TYPE_TRANSFORM;
    if(lv_obj_get_style_transform_skew_y_internal(obj, LV_PART_MAIN) != 0) return LV_LAYER_TYPE_TRANSFORM;
    if(lv_obj_get_style_opa_layered_internal(obj, LV_PART_MAIN) != LV_OPA_COVER) return LV_LAYER_TYPE_SIMPLE;
    if(lv_obj_get_style_bitmap_mask_src_internal(obj, LV_PART_MAIN) != NULL) return LV_LAYER_TYPE_SIMPLE;
    if(lv_obj_get_style_blend_mode_internal(obj, LV_PART_MAIN) != LV_BLEND_MODE_NORMAL) return LV_LAYER_TYPE_SIMPLE;
    return LV_LAYER_TYPE_NONE;
}

/*True if any style attached to the widget enables blur or a drop shadow in the
 *widget's current state. The per-style has_group bitmask makes the common
 *no-blur case a cheap scan.*/
static bool calculate_has_blur(lv_obj_t * obj)
{
    const uint32_t group_blur = (uint32_t)1 << lv_style_get_prop_group(LV_STYLE_BLUR_RADIUS);
    const uint32_t group_dropshadow = (uint32_t)1 << lv_style_get_prop_group(LV_STYLE_DROP_SHADOW_OPA);
    const lv_state_t state = lv_obj_style_get_selector_state(lv_obj_get_state(obj));
    const lv_state_t state_inv = ~state;
    lv_style_value_t v;
    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        lv_obj_style_t * obj_style = &obj->styles[i];
        if(obj_style->is_disabled) continue;

        lv_state_t state_style = lv_obj_style_get_selector_state(obj->styles[i].selector);
        if((state_style & state_inv)) continue;

        if((obj_style->style->has_group & group_blur) &&
           lv_style_get_prop(obj_style->style, LV_STYLE_BLUR_RADIUS, &v)) {
            if(v.num > 0) return true;
        }
        if((obj_style->style->has_group & group_dropshadow) &&
           lv_style_get_prop(obj_style->style, LV_STYLE_DROP_SHADOW_OPA, &v)) {
            if(v.num > 0) return true;
        }
    }

    return false;
}

void lv_obj_style_full_cache_refresh(lv_obj_t * obj, lv_part_t part)
{
#if LV_OBJ_STYLE_CACHE
    uint32_t i;
    if(part == LV_PART_MAIN || part == LV_PART_ANY) {
        obj->style_main_prop_is_set = 0;
        for(i = 0; i < obj->style_cnt; i++) {
            if(lv_obj_style_get_selector_part(obj->styles[i].selector) != LV_PART_MAIN) continue;
            if(obj->styles[i].is_disabled) continue;
            lv_style_t * style = (lv_style_t *)obj->styles[i].style;
            uint32_t j;
            if(lv_style_is_const_internal(style)) {
                lv_style_const_prop_t * props = style->values_and_props;
                for(j = 0; props[j].prop != LV_STYLE_PROP_INV; j++) {
                    obj->style_main_prop_is_set |= LV_OBJ_STYLE_PROP_SHIFTED(props[j].prop);
                }
            }
            else {
                lv_style_prop_t * props = (lv_style_prop_t *)style->values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
                for(j = 0; j < style->prop_cnt; j++) {
                    obj->style_main_prop_is_set |= LV_OBJ_STYLE_PROP_SHIFTED(props[j]);
                }
            }
        }
    }
    if(part != LV_PART_MAIN || part == LV_PART_ANY) {
        obj->style_other_prop_is_set = 0;
        for(i = 0; i < obj->style_cnt; i++) {
            if(lv_obj_style_get_selector_part(obj->styles[i].selector) == LV_PART_MAIN) continue;
            if(obj->styles[i].is_disabled) continue;

            lv_style_t * style = (lv_style_t *)obj->styles[i].style;
            uint32_t j;
            if(lv_style_is_const_internal(style)) {
                lv_style_const_prop_t * props = style->values_and_props;
                for(j = 0; props[j].prop != LV_STYLE_PROP_INV; j++) {
                    obj->style_other_prop_is_set |= LV_OBJ_STYLE_PROP_SHIFTED(props[j].prop);
                }
            }
            else {
                lv_style_prop_t * props = (lv_style_prop_t *)style->values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
                for(j = 0; j < style->prop_cnt; j++) {
                    obj->style_other_prop_is_set |= LV_OBJ_STYLE_PROP_SHIFTED(props[j]);
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
    if(lv_style_is_const_internal(style)) {
        lv_style_const_prop_t * props = style->values_and_props;
        uint32_t i;
        for(i = 0; props[i].prop != LV_STYLE_PROP_INV; i++) {
            if(lv_style_prop_has_flag(props[i].prop, flag)) {
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
    const uint32_t prop_shifted = LV_OBJ_STYLE_PROP_SHIFTED(prop);
    if((part == LV_PART_MAIN ? obj->style_main_prop_is_set : obj->style_other_prop_is_set) & prop_shifted)
#endif
    {
        found = get_prop_core(obj, selector, prop, value_act);
        if(found == LV_STYLE_RES_FOUND) return LV_STYLE_RES_FOUND;
    }

    extern const uint8_t lv_style_builtin_prop_flag_lookup_table[];
    bool inheritable = false;
    if(prop < LV_STYLE_NUM_BUILT_IN_PROPS) {
        inheritable = lv_style_builtin_prop_flag_lookup_table[prop] & LV_STYLE_PROP_FLAG_INHERITABLE;
    }
    else {
        if(_style_custom_prop_flag_lookup_table != NULL) {
            inheritable = _style_custom_prop_flag_lookup_table[prop - LV_STYLE_NUM_BUILT_IN_PROPS] &
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

static bool remove_style_core(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector, bool theme_only)
{
    LV_ASSERT(obj != NULL);
    lv_state_t state = lv_obj_style_get_selector_state(selector);
    lv_part_t part = lv_obj_style_get_selector_part(selector);

    /*If the draw size changes invalidate the old area first.
     *`style == NULL` means "any style" (e.g. `lv_obj_remove_style_all()`) so it can't be checked for flags.*/
    if((part == LV_PART_MAIN || part == LV_PART_ANY) &&
       (style == NULL || style_has_flag(style, LV_OBJ_STYLE_DRAW_AREA_FLAGS))) {
        lv_obj_invalidate(obj);
    }

    uint32_t style_count = 0;
    bool deleted = false;

    for(uint32_t i = 0; i < obj->style_cnt; i++) {
        lv_state_t state_act = lv_obj_style_get_selector_state(obj->styles[i].selector);
        lv_part_t part_act = lv_obj_style_get_selector_part(obj->styles[i].selector);

        bool should_remove = true;
        if(theme_only && !obj->styles[i].is_theme) {
            should_remove = false;
        }
        else if((state != LV_STATE_ANY && state_act != state) ||
                (part != LV_PART_ANY && part_act != part) ||
                (style != NULL && style != obj->styles[i].style)) {
            should_remove = false;
        }

        if(!should_remove) {
            /*Keep this entry: copy it down to the current write position if needed*/
            if(style_count != i) {
                obj->styles[style_count] = obj->styles[i];
            }
            style_count++;
            continue;
        }

        /*This entry is being removed*/
        if(obj->styles[i].is_trans) {
            lv_obj_style_remove_transitions(obj, part, LV_STYLE_PROP_ANY);
        }
        if(obj->styles[i].is_local || obj->styles[i].is_trans) {
            if(obj->styles[i].style) lv_style_reset((lv_style_t *)obj->styles[i].style);
            lv_free((lv_style_t *)obj->styles[i].style);
            obj->styles[i].style = NULL;
        }
        deleted = true;
    }

    if(!deleted) {
        return false;
    }

    obj->style_cnt = style_count;
    if(style_count > 0) {
        obj->styles = lv_realloc(obj->styles, style_count * sizeof(lv_obj_style_t));
    }
    else {
        lv_free(obj->styles);
        obj->styles = NULL;
    }

    lv_obj_style_full_cache_refresh(obj, part);
    lv_obj_refresh_style(obj, part, LV_STYLE_PROP_ANY);

    return true;
}

static lv_obj_style_t * find_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector)
{
    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        if(obj->styles[i].style == style && obj->styles[i].selector == selector) {
            return &obj->styles[i];
        }
    }

    LV_LOG_WARN("%p style was not found on %p widget with %6" LV_PRIx32 " selector", (void *)style, (void *)obj, selector);
    return NULL;
}

/**
 * Mark the properties of a style as set in the style cache of an object.
 * It makes the cache a superset which is always safe.
 */
static void cache_add_style_props(lv_obj_t * obj, lv_part_t part, const lv_style_t * style)
{
#if LV_OBJ_STYLE_CACHE
    uint32_t * prop_is_set = part == LV_PART_MAIN ? &obj->style_main_prop_is_set : &obj->style_other_prop_is_set;
    uint32_t i;
    if(lv_style_is_const_internal(style)) {
        lv_style_const_prop_t * props = style->values_and_props;
        for(i = 0; props[i].prop != LV_STYLE_PROP_INV; i++) {
            (*prop_is_set) |= LV_OBJ_STYLE_PROP_SHIFTED(props[i].prop);
        }
    }
    else {
        lv_style_prop_t * props = (lv_style_prop_t *)style->values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
        for(i = 0; i < style->prop_cnt; i++) {
            (*prop_is_set) |= LV_OBJ_STYLE_PROP_SHIFTED(props[i]);
        }
    }
#else
    LV_UNUSED(obj);
    LV_UNUSED(part);
    LV_UNUSED(style);
#endif
}











#if LV_USE_OBSERVER

static void bind_style_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    bind_style_t * p = lv_observer_get_user_data(observer);

    int32_t v = lv_subject_get_int(subject);
    bool en = (v == p->value);
    lv_obj_set_style_enabled(observer->target, p->style, p->selector, en);
}

static void bind_style_prop_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    bind_style_prop_t * p = lv_observer_get_user_data(observer);

    lv_style_value_t style_v;
    if(subject->type == LV_SUBJECT_TYPE_INT) style_v.num = lv_subject_get_int(subject);
    else if(subject->type == LV_SUBJECT_TYPE_COLOR) style_v.color = lv_subject_get_color(subject);
    else if(subject->type == LV_SUBJECT_TYPE_POINTER) style_v.ptr = lv_subject_get_pointer(subject);
    else {
        LV_LOG_WARN("Not supported subject type (%"LV_PRIu32")", (uint32_t) subject->type);
        return;
    }

    lv_obj_set_local_style_prop(observer->target, p->prop, style_v, p->selector);
}


#endif /*LV_USE_OBSERVER*/
