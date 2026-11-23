/**
 * @file lv_obj_style_transition.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj_private.h"
#include "../lvgl_public.h"
#include "../misc/lv_anim_private.h"
#include "../misc/lv_style_private.h"
#include "lv_obj_style_private.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define style_trans_ll_p &(LV_GLOBAL_DEFAULT()->style_trans_ll)

/*Max number of transitions started at once by a state or style change*/
#define STYLE_TRANSITION_MAX 32

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

/*A transition to start after a state or style change*/
struct _lv_obj_style_pending_trans_t {
    lv_obj_style_transition_dsc_t dsc;
    lv_style_value_t start_value;   /*The value currently shown*/
    lv_style_value_t end_value;     /*The value after the change*/
    bool read_end;                  /*Read `end_value` when starting the transition*/
};

typedef struct _lv_obj_style_pending_trans_t lv_obj_style_pending_trans_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_style_t * get_trans_style(lv_obj_t * obj,  lv_style_selector_t selector);
static bool remove_trans_styles(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop, trans_t * tr_limit);
static void trans_anim_cb(void * _tr, int32_t v);
static void trans_anim_start_cb(lv_anim_t * a);
static void trans_anim_completed_cb(lv_anim_t * a);
static uint32_t collect_transitions(lv_obj_t * obj, lv_state_t state, lv_part_t part, lv_style_prop_t prop,
                                    uint8_t type, const lv_style_t * filter1, const lv_style_t * filter2,
                                    lv_obj_style_pending_trans_t ** ts_out);
static lv_style_value_t get_prop_skip_trans(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop);
static bool style_value_eq(lv_style_value_t v1, lv_style_value_t v2);
static void start_transition_core(lv_obj_t * obj, lv_part_t part, lv_style_value_t v1, lv_style_value_t v2,
                                  const lv_obj_style_transition_dsc_t * tr_dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_obj_style_create_transition(lv_obj_t * obj, lv_part_t part, lv_state_t prev_state, lv_state_t new_state,
                                    const lv_obj_style_transition_dsc_t * tr_dsc)
{
    LV_CHECK_ARG(obj != NULL, return);
    LV_CHECK_ARG(tr_dsc != NULL, return);

    /*Get the previous and current values*/
    obj->skip_trans = 1;
    obj->state = prev_state;
    lv_style_value_t v1 = lv_obj_get_style_prop(obj, part, tr_dsc->prop);
    obj->state = new_state;
    lv_style_value_t v2 = lv_obj_get_style_prop(obj, part, tr_dsc->prop);
    obj->skip_trans = 0;

    if(style_value_eq(v1, v2)) return;
    obj->state = prev_state;
    v1 = lv_obj_get_style_prop(obj, part, tr_dsc->prop);
    obj->state = new_state;

    start_transition_core(obj, part, v1, v2, tr_dsc);
    lv_obj_refresh_style(obj, part, tr_dsc->prop);
}

void lv_obj_style_start_state_transitions(lv_obj_t * obj, lv_state_t prev_state, lv_state_t new_state)
{
    LV_CHECK_ARG(obj != NULL, return);

    lv_obj_style_pending_trans_t * ts = NULL;
    uint32_t ts_cnt = collect_transitions(obj, new_state, LV_PART_ANY, LV_STYLE_PROP_ANY,
                                          LV_STYLE_TRANSITION_TYPE_STATE, NULL, NULL, &ts);

    uint32_t i;
    for(i = 0; i < ts_cnt; i++) {
        lv_part_t part_act = lv_obj_style_get_selector_part(ts[i].dsc.selector);
        lv_obj_style_create_transition(obj, part_act, prev_state, new_state, &ts[i].dsc);
    }

    lv_free(ts);
}

void lv_obj_style_transition_init(void)
{
    lv_ll_init(style_trans_ll_p, sizeof(trans_t));
}

void lv_obj_style_transition_deinit(void)
{
    lv_ll_clear(style_trans_ll_p);
}

bool lv_obj_style_remove_transitions(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop)
{
    return remove_trans_styles(obj, part, prop, NULL);
}

bool lv_obj_style_transition_allowed(const lv_obj_t * obj, lv_part_t part)
{
    if(!obj->rendered) return false;
    if(part == LV_PART_ANY) return false;

#if LV_OBJ_STYLE_CACHE
    /*Quick check: no transition property is set on this part*/
    uint32_t prop_is_set = part == LV_PART_MAIN ? obj->style_main_prop_is_set : obj->style_other_prop_is_set;
    if((prop_is_set & LV_OBJ_STYLE_PROP_SHIFTED(LV_STYLE_TRANSITION)) == 0) return false;
#endif

    return true;
}

void lv_obj_style_change_apply(lv_obj_t * obj, const lv_obj_style_change_t * c, bool after)
{
    uint32_t i;
    for(i = 0; i < obj->style_cnt && i < 64; i++) {
        if((c->mask & ((uint64_t)1 << i)) == 0) continue;
        obj->styles[i].style = after ? c->style_after : c->style_before;
        obj->styles[i].is_disabled = after ? c->disabled_after : c->disabled_before;
    }
}

void lv_obj_style_transition_prepare_change(lv_obj_t * obj, lv_part_t part, const lv_obj_style_change_t * c,
                                            lv_obj_style_trans_batch_t * batch)
{
    batch->items = NULL;
    batch->cnt = 0;
    if(!lv_obj_style_transition_allowed(obj, part)) return;

    const lv_style_t * filter2 = c->style_after != c->style_before ? c->style_after : NULL;
    lv_obj_style_pending_trans_t * ts;
    uint32_t ts_cnt = collect_transitions(obj, obj->state, part, LV_STYLE_PROP_ANY,
                                          LV_STYLE_TRANSITION_TYPE_STYLE, c->style_before, filter2, &ts);
    if(ts_cnt == 0) return;

    uint32_t i;
    /*The start value is the currently shown value, including the running transitions*/
    lv_obj_style_change_apply(obj, c, false);
    for(i = 0; i < ts_cnt; i++) {
        ts[i].start_value = lv_obj_get_style_prop(obj, part, ts[i].dsc.prop);
    }

    lv_obj_style_change_apply(obj, c, true);
    for(i = 0; i < ts_cnt; i++) {
        ts[i].end_value = get_prop_skip_trans(obj, part, ts[i].dsc.prop);
    }

    batch->items = ts;
    batch->cnt = ts_cnt;
}

void lv_obj_style_transition_prepare_local(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop,
                                           lv_obj_style_trans_batch_t * batch)
{
    batch->items = NULL;
    batch->cnt = 0;
    if(prop == LV_STYLE_TRANSITION) return;
    if(!lv_obj_style_transition_allowed(obj, part)) return;

    lv_obj_style_pending_trans_t * ts;
    uint32_t ts_cnt = collect_transitions(obj, obj->state, part, prop, LV_STYLE_TRANSITION_TYPE_LOCAL,
                                          NULL, NULL, &ts);
    if(ts_cnt == 0) return;

    ts[0].start_value = lv_obj_get_style_prop(obj, part, prop);
    ts[0].read_end = true;

    batch->items = ts;
    batch->cnt = ts_cnt;
}

void lv_obj_style_transition_start(lv_obj_t * obj, lv_obj_style_trans_batch_t * batch)
{
    lv_obj_style_pending_trans_t * ts = batch->items;
    uint32_t i;
    for(i = 0; i < batch->cnt; i++) {
        lv_part_t part = lv_obj_style_get_selector_part(ts[i].dsc.selector);
        if(ts[i].read_end) ts[i].end_value = get_prop_skip_trans(obj, part, ts[i].dsc.prop);
        if(style_value_eq(ts[i].start_value, ts[i].end_value)) continue;

        /*Replace the running transition of this property (if any)*/
        remove_trans_styles(obj, part, ts[i].dsc.prop, NULL);
        start_transition_core(obj, part, ts[i].start_value, ts[i].end_value, &ts[i].dsc);
    }

    lv_free(ts);
    batch->items = NULL;
    batch->cnt = 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Get the transition style of an object for a given part and for a given state.
 * If the transition style for the part-state pair doesn't exist allocate and return it.
 * @param obj   pointer to an object
 * @param selector OR-ed value of parts and state for which the style should be get
 * @return pointer to the transition style
 */
static lv_obj_style_t * get_trans_style(lv_obj_t * obj,  lv_style_selector_t selector)
{
    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        if(obj->styles[i].is_trans && obj->styles[i].selector == selector) break;
    }

    /*Already have a transition style for it*/
    if(i != obj->style_cnt) return &obj->styles[i];

    /*Allocate the individual style first so obj->styles stays valid on failure*/
    lv_style_t * new_style = lv_malloc(sizeof(lv_style_t));
    if(new_style == NULL) {
        LV_LOG_WARN("couldn't allocate transition style");
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
        obj->styles[i] = obj->styles[i - 1];
    }

    lv_memzero(&obj->styles[0], sizeof(lv_obj_style_t));
    obj->styles[0].style = new_style;
    lv_style_init((lv_style_t *)obj->styles[0].style);

    obj->styles[0].is_trans = 1;
    obj->styles[0].selector = selector;
    return &obj->styles[0];
}

/**
 * Remove the transition from object's part's property.
 * - Remove the transition from `lv_obj_style_trans_ll` and free it
 * - Delete pending transitions
 * @param obj pointer to an object which transition(s) should be removed
 * @param part a part of object or 0xFF to remove from all parts
 * @param prop a property or 0xFF to remove all properties
 * @param tr_limit delete transitions only "older" than this. `NULL` if not used
 * @return true: at least 1 transition style was removed.
 * @note it doesn't update the cache, `full_cache_refresh` needs to be called manually
 */
static bool remove_trans_styles(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop, trans_t * tr_limit)
{
    trans_t * tr;
    trans_t * tr_prev;
    bool removed = false;
    tr = lv_ll_get_tail(style_trans_ll_p);
    while(tr != NULL) {
        if(tr == tr_limit) break;

        /*'tr' might be deleted, so get the next object while 'tr' is valid*/
        tr_prev = lv_ll_get_prev(style_trans_ll_p, tr);

        if(tr->obj == obj && (part == tr->selector || part == LV_PART_ANY) && (prop == tr->prop || prop == LV_STYLE_PROP_ANY)) {
            /*Dropping the transitioned value changes the drawn area right away,
             *so invalidate the old area first*/
            if(lv_obj_style_get_selector_part(tr->selector) == LV_PART_MAIN &&
               lv_style_prop_has_flag(tr->prop, LV_OBJ_STYLE_DRAW_AREA_FLAGS)) {
                lv_obj_invalidate(obj);
            }

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
            lv_ll_remove(style_trans_ll_p, tr);
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
    /*Find the transition style of of the descriptor*/
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
            case LV_STYLE_BG_IMAGE_SRC:
                if(v < 255) value_final.ptr = tr->start_value.ptr;
                else value_final.ptr = tr->end_value.ptr;
                break;
            case LV_STYLE_COLOR_FILTER_DSC:
                if(tr->start_value.ptr == NULL) value_final.ptr = tr->end_value.ptr;
                else if(tr->end_value.ptr == NULL) value_final.ptr = tr->start_value.ptr;
                else if(v < 128) value_final.ptr = tr->start_value.ptr;
                else value_final.ptr = tr->end_value.ptr;
                break;
            /*Colors*/
            case LV_STYLE_RECOLOR:
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
            /*Opacities*/
            case LV_STYLE_OPA:
            case LV_STYLE_OPA_LAYERED:
            case LV_STYLE_RECOLOR_OPA:
            case LV_STYLE_COLOR_FILTER_OPA:
            case LV_STYLE_BG_OPA:
            case LV_STYLE_BG_GRAD_OPA:
            case LV_STYLE_BG_MAIN_OPA:
            case LV_STYLE_BG_IMAGE_OPA:
            case LV_STYLE_BG_IMAGE_RECOLOR_OPA:
            case LV_STYLE_BORDER_OPA:
            case LV_STYLE_OUTLINE_OPA:
            case LV_STYLE_SHADOW_OPA:
            case LV_STYLE_LINE_OPA:
            case LV_STYLE_ARC_OPA:
            case LV_STYLE_TEXT_OPA:
            case LV_STYLE_IMAGE_OPA:
            case LV_STYLE_IMAGE_RECOLOR_OPA:
            case LV_STYLE_DROP_SHADOW_OPA:
            case LV_STYLE_TEXT_OUTLINE_STROKE_OPA:
                if(v <= 0) value_final.num = tr->start_value.num;
                else if(v >= 255) value_final.num = tr->end_value.num;
                else value_final.num =  LV_CLAMP(0, tr->start_value.num + ((tr->end_value.num - tr->start_value.num) * v >> 8), 255);
                break;

            default:
                if(v == 0) value_final.num = tr->start_value.num;
                else if(v == 255) value_final.num = tr->end_value.num;
                else value_final.num = tr->start_value.num + ((int32_t)((int32_t)(tr->end_value.num - tr->start_value.num) * v) >> 8);
                break;
        }

        /*Don't do anything is the prop didn't changed*/
        lv_style_value_t old_value = {0};
        if(lv_style_get_prop_internal(obj->styles[i].style, tr->prop, &old_value) == LV_STYLE_RES_FOUND) {
            if(value_final.ptr == old_value.ptr &&
               lv_color_eq(value_final.color, old_value.color) &&
               value_final.num == old_value.num) {
                break;
            }
        }

        /*If the draw size changes invalidate the old area first*/
        lv_part_t part = lv_obj_style_get_selector_part(tr->selector);
        if(part == LV_PART_MAIN && lv_style_prop_has_flag(tr->prop, LV_OBJ_STYLE_DRAW_AREA_FLAGS)) {
            lv_obj_invalidate(tr->obj);
        }
        lv_style_set_prop((lv_style_t *)obj->styles[i].style, tr->prop, value_final);

#if LV_OBJ_STYLE_CACHE
        uint32_t prop_shifted = LV_OBJ_STYLE_PROP_SHIFTED(tr->prop);
        if(part == LV_PART_MAIN) {
            obj->style_main_prop_is_set |= prop_shifted;
        }
        else {
            obj->style_other_prop_is_set |= prop_shifted;
        }
#endif

        lv_obj_refresh_style(tr->obj, tr->selector, tr->prop);
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
    bool trans_removed = remove_trans_styles(tr->obj, part, prop_tmp, tr);

    tr->prop = prop_tmp;

    lv_obj_style_t * style_trans = get_trans_style(tr->obj, tr->selector);
    /*Be sure `trans_style` has a valid value*/
    lv_style_set_prop((lv_style_t *)style_trans->style, tr->prop, tr->start_value);
#if LV_OBJ_STYLE_CACHE
    if(trans_removed) {
        lv_obj_style_full_cache_refresh(tr->obj, part);
    }
    else {
        uint32_t prop_shifted = LV_OBJ_STYLE_PROP_SHIFTED(tr->prop);
        if(part == LV_PART_MAIN) {
            tr->obj->style_main_prop_is_set |= prop_shifted;
        }
        else {
            tr->obj->style_other_prop_is_set |= prop_shifted;
        }
    }
#else
    LV_UNUSED(trans_removed);
#endif

    lv_obj_refresh_style(tr->obj, tr->selector, tr->prop);

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
    LV_LL_READ(style_trans_ll_p, tr_i) {
        if(tr_i != tr && tr_i->obj == tr->obj && tr_i->selector == tr->selector && tr_i->prop == tr->prop) {
            running = true;
            break;
        }
    }

    if(!running) {
        uint32_t i;
        for(i = 0; i < obj->style_cnt; i++) {
            if(obj->styles[i].is_trans && obj->styles[i].selector == tr->selector) {
                lv_ll_remove(style_trans_ll_p, tr);
                lv_free(tr);

                lv_obj_style_t * obj_style = &obj->styles[i];
                lv_style_remove_prop((lv_style_t *)obj_style->style, prop);

                if(lv_style_is_empty(obj->styles[i].style)) {
                    lv_obj_remove_style(obj, (lv_style_t *)obj_style->style, obj_style->selector);

                }
                break;
            }
        }
    }
}

/**
 * Collect the transition descriptors which apply in a given state.
 * In case of conflict the descriptor of the higher precedence style is used.
 * @param obj       pointer to a widget
 * @param state     consider the styles of this state
 * @param part      consider the styles of this part or `LV_PART_ANY` for all parts
 * @param prop      collect only this property or `LV_STYLE_PROP_ANY` for all
 * @param type      collect only the descriptors having this type
 * @param filter1   if not `NULL` collect only the properties set in this style (or in `filter2`)
 * @param filter2   another style to filter by or `NULL`
 * @param ts_out    the collected transitions are stored here. Allocated only if needed,
 *                  free it with `lv_free()` (or pass it to `lv_obj_style_transition_start()`)
 * @return          number of collected transitions
 */
static uint32_t collect_transitions(lv_obj_t * obj, lv_state_t state, lv_part_t part, lv_style_prop_t prop,
                                    uint8_t type, const lv_style_t * filter1, const lv_style_t * filter2,
                                    lv_obj_style_pending_trans_t ** ts_out)
{
    *ts_out = NULL;

    lv_obj_style_pending_trans_t * ts = NULL;
    uint32_t tsi = 0;
    uint32_t i;
    for(i = 0; i < obj->style_cnt; i++) {
        const lv_obj_style_t * obj_style = &obj->styles[i];
        if(obj_style->is_trans || obj_style->is_disabled) continue;

        lv_part_t part_act = lv_obj_style_get_selector_part(obj_style->selector);
        if(part != LV_PART_ANY && part_act != part) continue;

        lv_state_t state_act = lv_obj_style_get_selector_state(obj_style->selector);
        if(state_act & (~state)) continue; /*Skip unrelated styles*/

        lv_style_value_t v;
        if(lv_style_get_prop_internal(obj_style->style, LV_STYLE_TRANSITION, &v) != LV_STYLE_RES_FOUND) continue;
        const lv_style_transition_dsc_t * tr = v.ptr;
        if(tr == NULL || tr->props == NULL) continue;

        uint8_t tr_type = tr->type == LV_STYLE_TRANSITION_TYPE_DEFAULT ?
                          LV_STYLE_TRANSITION_TYPE_STATE : tr->type;
        if((tr_type & type) == 0) continue;

        uint32_t j;
        for(j = 0; tr->props[j] != 0; j++) {
            lv_style_prop_t prop_act = tr->props[j];
            if(prop != LV_STYLE_PROP_ANY && prop_act != prop) continue;

            if(filter1 || filter2) {
                lv_style_value_t v_tmp;
                bool has = (filter1 && lv_style_get_prop_internal(filter1, prop_act, &v_tmp) == LV_STYLE_RES_FOUND) ||
                           (filter2 && lv_style_get_prop_internal(filter2, prop_act, &v_tmp) == LV_STYLE_RES_FOUND);
                if(!has) continue;
            }

            /*Already added for this part?*/
            uint32_t t;
            for(t = 0; t < tsi; t++) {
                if(ts[t].dsc.prop == prop_act &&
                   lv_obj_style_get_selector_part(ts[t].dsc.selector) == part_act) break;
            }

            if(t < tsi) {
                /*Keep the descriptor of the more specific state*/
                if(lv_obj_style_get_selector_state(ts[t].dsc.selector) >= state_act) continue;
            }
            else {
                if(tsi >= STYLE_TRANSITION_MAX) continue;
                if(ts == NULL) {
                    ts = lv_malloc(sizeof(lv_obj_style_pending_trans_t) * STYLE_TRANSITION_MAX);
                    LV_ASSERT_MALLOC(ts);
                    if(ts == NULL) {
                        LV_LOG_WARN("No memory to compute style transitions");
                        return 0;
                    }
                }
                tsi++;
            }

            lv_memzero(&ts[t], sizeof(lv_obj_style_pending_trans_t));
            ts[t].dsc.time = tr->time;
            ts[t].dsc.delay = tr->delay;
            ts[t].dsc.path_cb = tr->path_xcb;
            ts[t].dsc.prop = prop_act;
            ts[t].dsc.user_data = tr->user_data;
            ts[t].dsc.selector = obj_style->selector;
        }
    }

    if(tsi == 0) {
        lv_free(ts);
        ts = NULL;
    }

    *ts_out = ts;
    return tsi;
}

/**
 * Get a style property ignoring the running transitions, i.e. the final value.
 */
static lv_style_value_t get_prop_skip_trans(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop)
{
    uint32_t skip_trans_prev = obj->skip_trans;
    obj->skip_trans = 1;
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, prop);
    obj->skip_trans = skip_trans_prev;
    return v;
}

static bool style_value_eq(lv_style_value_t v1, lv_style_value_t v2)
{
    return v1.ptr == v2.ptr && v1.num == v2.num && lv_color_eq(v1.color, v2.color);
}

/**
 * Start a transition animation from `v1` to `v2`.
 * The transition style is set to `v1` right away. It doesn't refresh the style of the widget.
 */
static void start_transition_core(lv_obj_t * obj, lv_part_t part, lv_style_value_t v1, lv_style_value_t v2,
                                  const lv_obj_style_transition_dsc_t * tr_dsc)
{
    /*The radius of the transition style is set to the original value (e.g. `LV_RADIUS_CIRCLE`)*/
    const lv_style_value_t v_start = v1;

    /*Allocate the descriptor first so that nothing is changed if it fails*/
    trans_t * tr = lv_ll_ins_head(style_trans_ll_p);
    LV_ASSERT_MALLOC(tr);
    if(tr == NULL) return;

    lv_obj_style_t * style_trans = get_trans_style(obj, part);
    if(style_trans == NULL) {
        lv_ll_remove(style_trans_ll_p, tr);
        lv_free(tr);
        return;
    }

    if(tr_dsc->prop == LV_STYLE_RADIUS) {
        if(v1.num == LV_RADIUS_CIRCLE || v2.num == LV_RADIUS_CIRCLE) {
            int32_t whalf = lv_obj_get_width(obj) / 2;
            int32_t hhalf = lv_obj_get_height(obj) / 2;
            if(v1.num == LV_RADIUS_CIRCLE) v1.num = LV_MIN(whalf + 1, hhalf + 1);
            if(v2.num == LV_RADIUS_CIRCLE) v2.num = LV_MIN(whalf + 1, hhalf + 1);
        }
    }

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
    if(lv_anim_start(&a) == NULL) {
        lv_ll_remove(style_trans_ll_p, tr);
        lv_free(tr);
        return;
    }

    /*Set the start value only if the transition really started, else the property would be stuck at it*/
    lv_style_set_prop((lv_style_t *)style_trans->style, tr_dsc->prop, v_start);

#if LV_OBJ_STYLE_CACHE
    if(part == LV_PART_MAIN) obj->style_main_prop_is_set |= LV_OBJ_STYLE_PROP_SHIFTED(tr_dsc->prop);
    else obj->style_other_prop_is_set |= LV_OBJ_STYLE_PROP_SHIFTED(tr_dsc->prop);
#endif
}
