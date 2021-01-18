/**
 * @file lv_obj_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "lv_disp.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_gc.h"

#if defined(LV_GC_INCLUDE)
    #include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_obj"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t * obj;
    lv_style_prop_t prop;
    uint8_t part;
    lv_style_value_t start_value;
    lv_style_value_t end_value;
} lv_style_trans_t;

typedef enum {
    CACHE_ZERO = 0,
    CACHE_TRUE = 1,
    CACHE_UNSET = 2,
    CACHE_255 = 3,
    CACHE_NEED_CHECK = 4,
}cache_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void report_style_change_core(void * style, lv_obj_t * obj);
static void refresh_children_style(lv_obj_t * obj);
#if LV_USE_ANIMATION
static bool trans_del(lv_obj_t * obj, uint8_t part, lv_style_prop_t prop, lv_style_trans_t * tr_limit);
static void trans_anim_cb(lv_style_trans_t * tr, lv_anim_value_t v);
static void trans_anim_start_cb(lv_anim_t * a);
static void trans_anim_ready_cb(lv_anim_t * a);
static void fade_anim_cb(lv_obj_t * obj, lv_anim_value_t v);
static void fade_in_anim_ready(lv_anim_t * a);
#endif
static cache_t read_cache(const lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop);
static void update_cache(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the object related style manager module.
 * Called by LVGL in `lv_init()`
 */
void _lv_obj_style_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_obj_style_trans_ll), sizeof(lv_style_trans_t));
}

void lv_obj_add_style_no_refresh(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_style_t * style)
{
    if(style == NULL) return;

#if LV_USE_ANIMATION
    trans_del(obj, part, 0xFF, NULL);
#endif

    uint32_t i;
    /*Go after the transition and local styles*/
    for(i = 0; i < obj->style_list.style_cnt; i++) {
        if(obj->style_list.styles[i].is_trans) continue;
        if(obj->style_list.styles[i].is_local) continue;
        break;
    }

    /*Now `i` is at the first normal style. Insert the new style before this */

    /*Allocate space for the new style and shift the rest of the style to the end*/
    obj->style_list.style_cnt++;
    obj->style_list.styles = lv_mem_realloc(obj->style_list.styles, obj->style_list.style_cnt * sizeof(lv_obj_style_t));

    uint32_t j;
    for(j = obj->style_list.style_cnt - 1; j > i ; j--) {
        obj->style_list.styles[j] = obj->style_list.styles[j - 1];
    }

    _lv_memset_00(&obj->style_list.styles[i], sizeof(lv_obj_style_t));
    obj->style_list.styles[i].style = style;
    obj->style_list.styles[i].part = part;
    obj->style_list.styles[i].state = state;

}

/**
 * Add a new style to the style list of an object.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 *             E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param style pointer to a style to add (Only its pointer will be saved)
 */
void lv_obj_add_style(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_style_t * style)
{
    lv_obj_add_style_no_refresh(obj, part, state, style);
    _lv_obj_refresh_style(obj, LV_STYLE_PROP_ALL);
}

/**
 * Remove a style from the style list of an object.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 *              E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param style pointer to a style to remove
 */
void lv_obj_remove_style(lv_obj_t * obj, uint32_t part, uint32_t state, lv_style_t * style)
{
    if(style == NULL) return;

    uint32_t i;
    /*Find the style*/
    for(i = 0; i < obj->style_list.style_cnt; i++) {
        if(obj->style_list.styles[i].state == state &&
           obj->style_list.styles[i].part == part) {
            break;
        }
    }

    /*The style is not found*/
    if(i == obj->style_list.style_cnt) return;

    /*Shift the styles after `i` by one*/
    uint32_t j;
    for(j = i; j < obj->style_list.style_cnt - 1 ; j++) {
        obj->style_list.styles[j] = obj->style_list.styles[j + 1];
    }


    obj->style_list.style_cnt--;
    obj->style_list.styles = lv_mem_realloc(obj->style_list.styles, obj->style_list.style_cnt * sizeof(lv_obj_style_t));


#if LV_USE_ANIMATION
    trans_del(obj, part, 0xFF, NULL);
#endif
    _lv_obj_refresh_style(obj, LV_STYLE_PROP_ALL);
}

void lv_obj_remove_all_styles_no_refresh(lv_obj_t * obj)
{
#if LV_USE_ANIMATION
    trans_del(obj, 0xFF, 0xFF, NULL);
#endif

    lv_mem_free(obj->style_list.styles);
    obj->style_list.styles = NULL;
    obj->style_list.style_cnt = 0;
}

/**
 * Reset a style to the default (empty) state.
 * Release all used memories and cancel pending related transitions.
 * Also notifies the object about the style change.
 * @param obj pointer to an object
 * @param part the part of the object which style list should be reseted.
 *             E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 */
void lv_obj_remove_all_styles(lv_obj_t * obj)
{
    lv_obj_remove_all_styles_no_refresh(obj);
    _lv_obj_refresh_style(obj, LV_STYLE_PROP_ALL);
}

/**
 * Notify all object if a style is modified
 * @param style pointer to a style. Only the objects with this style will be notified
 *               (NULL to notify all objects)
 */
void lv_obj_report_style_change(lv_style_t * style)
{
    lv_disp_t * d = lv_disp_get_next(NULL);

    while(d) {
        uint32_t i;
        for(i = 0; i < d->screen_cnt; i++) {
            report_style_change_core(style, d->screens[i]);
        }
        d = lv_disp_get_next(d);
    }
}

/**
 * Remove a local style property from a part of an object with a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be removed.
 *             E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 *             E.g. `LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @return true: the property was found and removed; false: the property was not found
 */
bool lv_obj_remove_style_prop(lv_obj_t * obj, uint32_t part, uint32_t state, lv_style_prop_t prop)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    uint32_t i;
    /*Find the style*/
    for(i = 0; i < obj->style_list.style_cnt; i++) {
        if(obj->style_list.styles[i].is_local &&
           obj->style_list.styles[i].state == state &&
           obj->style_list.styles[i].part == part) {
            break;
        }
    }

    /*The style is not found*/
    if(i == obj->style_list.style_cnt) return false;

    return lv_style_remove_prop(obj->style_list.styles[i].style, prop);
}


#if LV_USE_ANIMATION
/**
 * Fade in (from transparent to fully cover) an object and all its children using an `opa_scale` animation.
 * @param obj the object to fade in
 * @param time duration of the animation [ms]
 * @param delay wait before the animation starts [ms]
 */
void lv_obj_fade_in(lv_obj_t * obj, uint32_t time, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)fade_anim_cb);
    lv_anim_set_ready_cb(&a, fade_in_anim_ready);
    lv_anim_set_time(&a, time);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
}

/**
 * Fade out (from fully cover to transparent) an object and all its children using an `opa_scale` animation.
 * @param obj the object to fade in
 * @param time duration of the animation [ms]
 * @param delay wait before the animation starts [ms]
 */
void lv_obj_fade_out(lv_obj_t * obj, uint32_t time, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)fade_anim_cb);
    lv_anim_set_time(&a, time);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
}
#endif

static lv_style_value_t apply_color_filter(const lv_obj_t * obj, uint32_t part, lv_style_value_t v)
{
    if(obj == NULL) return v;
    lv_color_filter_cb_t f = lv_obj_get_style_color_filter_cb(obj, part);
    if(f) {
        lv_opa_t f_opa = lv_obj_get_style_color_filter_opa(obj, part);
        if(f_opa != 0) v.color = f(v.color, f_opa);
    }
    return v;
}
static bool get_prop_core(const lv_obj_t * obj, uint8_t part, lv_style_prop_t prop, lv_style_value_t * v)
{
    cache_t cache_res = read_cache(obj, part, prop);
    switch(cache_res) {
    case CACHE_ZERO:
        v->ptr = 0;
        return true;
    case CACHE_TRUE:
        v->num = 1;
        return true;
    case CACHE_255:
        v->num = 255;
        return true;
    case CACHE_UNSET:
        return false;
    case CACHE_NEED_CHECK:
        break;

    }

    int32_t weight = -1;
    lv_state_t state = obj->state;
    lv_state_t state_inv = ~state;
    lv_style_value_t value_tmp;
    bool skip_trans = obj->style_list.skip_trans;
    uint32_t i;
    bool found;
    for(i = 0; i < obj->style_list.style_cnt; i++) {
        lv_obj_style_t * obj_style = &obj->style_list.styles[i];
        if(obj_style->is_trans == false) break;
        if(skip_trans) continue;
        if(obj_style->part != part) continue;
        found = lv_style_get_prop(obj_style->style, prop, &value_tmp);
        if(found) {
            *v = value_tmp;
            return true;
        }
    }

    for(; i < obj->style_list.style_cnt; i++) {
        lv_obj_style_t * obj_style = &obj->style_list.styles[i];
        if(obj_style->part != part) continue;

        /* Be sure the style not specifies other state than the requested.
         * E.g. For HOVER+PRESS object state, HOVER style only is OK, but HOVER+FOCUS style is not*/
        if((obj_style->state & state_inv)) continue;

        /*Check only better candidates*/
        if(obj_style->state <= weight) continue;

        found = lv_style_get_prop(obj_style->style, prop, &value_tmp);

        if(found) {
            if(obj_style->state == state) {
                *v = value_tmp;
                return true;
            }
            if(weight < obj_style->state) {
                weight = obj_style->state;
                *v = value_tmp;
            }
        }
    }

    if(weight >= 0) {
        *v = value_tmp;
        return true;
    }
    else return false;
}

/**
 * Get a style property of a part of an object in the object's current state.
 * If there is a running transitions it is taken into account
 * @param obj pointer to an object
 * @param part the part of the object which style property should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop the property to get. E.g. `LV_STYLE_BORDER_OPA`.
 *  The state of the object will be added internally
 * @return the value of the property of the given part in the current state.
 * If the property is not found a default value will be returned.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has opacity type
 */
lv_style_value_t lv_obj_get_style_prop(const lv_obj_t * obj, uint8_t part, lv_style_prop_t prop)
{
    lv_style_value_t value_act;
    bool inherit = prop & LV_STYLE_PROP_INHERIT ? true : false;
    bool filter = prop & LV_STYLE_PROP_FILTER ? true : false;
    if(filter) {
        prop &= ~LV_STYLE_PROP_FILTER;
    }
    bool found = false;
    while(obj) {
        found = get_prop_core(obj, part, prop, &value_act);
        if(found) break;
        if(!inherit) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_PART_MAIN) {
            part = LV_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        obj = lv_obj_get_parent(obj);
    }

    if(!found) value_act = lv_style_prop_get_default(prop);
    if(filter) value_act = apply_color_filter(obj, part, value_act);
    return value_act;
}

/**
 * Notify an object and its children about its style is modified
 * @param obj pointer to an object
 * @param part the part of the object which style property should be refreshed.
 * @param prop `LV_STYLE_PROP_ALL` or an `LV_STYLE_...` property. It is used to optimize what needs to be refreshed.
 */
void _lv_obj_refresh_style(lv_obj_t * obj,lv_style_prop_t prop)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    update_cache(obj, LV_PART_MAIN, prop);

    lv_obj_invalidate(obj);
    if(prop == LV_STYLE_PROP_ALL || (prop & LV_STYLE_PROP_LAYOUT_REFR)) {
        lv_signal_send(obj, LV_SIGNAL_STYLE_CHG, NULL);
        lv_obj_invalidate(obj);
    } else if(prop & LV_STYLE_PROP_EXT_DRAW) {
        _lv_obj_refresh_ext_draw_pad(obj);
        lv_obj_invalidate(obj);
    }

    if(prop == LV_STYLE_PROP_ALL ||
      ((prop & LV_STYLE_PROP_INHERIT) && (prop & LV_STYLE_PROP_EXT_DRAW) && (prop & LV_STYLE_PROP_LAYOUT_REFR)))
    {
        refresh_children_style(obj);
    }
}

/**
 * Remove all transitions from an object
 * @param obj pointer to an object
 */
void _lv_obj_remove_style_trans(lv_obj_t * obj)
{
#if LV_USE_ANIMATION
    trans_del(obj, 0xFF, 0xFF, NULL);
#else
    LV_UNUSED(obj);
#endif
}

#if LV_USE_ANIMATION

lv_obj_style_t * _get_trans_style(lv_obj_t * obj, uint32_t part)
{
    uint32_t i;
    for(i = 0; i < obj->style_list.style_cnt; i++) {
        if(obj->style_list.styles[i].is_trans && obj->style_list.styles[i].part == part) break;
    }

    /*Already have a transition style for it*/
    if(i != obj->style_list.style_cnt) return &obj->style_list.styles[i];

    obj->style_list.style_cnt++;
    obj->style_list.styles = lv_mem_realloc(obj->style_list.styles, obj->style_list.style_cnt * sizeof(lv_obj_style_t));

    for(i = obj->style_list.style_cnt - 1; i > 0 ; i--) {
        obj->style_list.styles[i] = obj->style_list.styles[i - 1];
    }

    _lv_memset_00(&obj->style_list.styles[0], sizeof(lv_obj_style_t));
    obj->style_list.styles[0].style = lv_mem_alloc(sizeof(lv_style_t));
    lv_style_init(obj->style_list.styles[0].style);
    obj->style_list.styles[0].is_trans = 1;
    obj->style_list.styles[0].part = part;
    return &obj->style_list.styles[0];
}


lv_style_t * lv_obj_get_local_style(lv_obj_t * obj, uint32_t part, uint32_t state)
{
    uint32_t i;
    for(i = 0; i < obj->style_list.style_cnt; i++) {
        if(obj->style_list.styles[i].is_local &&
           obj->style_list.styles[i].part == part &&
           obj->style_list.styles[i].state == state)
        {
            return obj->style_list.styles[i].style;
        }
    }

    obj->style_list.style_cnt++;
    obj->style_list.styles = lv_mem_realloc(obj->style_list.styles, obj->style_list.style_cnt * sizeof(lv_obj_style_t));

    for(i = obj->style_list.style_cnt - 1; i > 0 ; i--) {
        /* Copy only normal styles (not local and transition).
         * The new local style will be added as the last local style*/
        if(obj->style_list.styles[i - 1].is_local || obj->style_list.styles[i - 1].is_trans) break;
        obj->style_list.styles[i] = obj->style_list.styles[i - 1];
    }

    _lv_memset_00(&obj->style_list.styles[i], sizeof(lv_obj_style_t));
    obj->style_list.styles[i].style = lv_mem_alloc(sizeof(lv_style_t));
    lv_style_init(obj->style_list.styles[i].style);
    obj->style_list.styles[i].is_local = 1;
    obj->style_list.styles[i].part = part;
    obj->style_list.styles[i].state = state;
    return obj->style_list.styles[i].style;
}


void lv_obj_set_style_prop(lv_obj_t * obj, uint32_t part, uint32_t state, lv_style_prop_t prop, lv_style_value_t value)
{
    lv_style_t * style = lv_obj_get_local_style(obj, part, state);
    lv_style_set_prop(style, prop, value);
    _lv_obj_refresh_style(obj, prop);

}

/**
 * Allocate and initialize a transition for a property of an object if the properties value is different in the new state.
 * It allocates `lv_style_trans_t` in `_lv_obj_style_trans_ll` and set only `start/end_values`. No animation will be created here.
 * @param obj and object to add the transition
 * @param prop the property to apply the transaction
 * @param part the part of the object to apply the transaction
 * @param prev_state the previous state of the objects
 * @param new_state the new state of the object
 * @param time duration of transition in [ms]
 * @param delay delay before starting the transition in [ms]
 * @param path the path of the transition
 * @return pointer to the allocated `the transaction` variable or `NULL` if no transition created
 */
void _lv_obj_create_style_transition(lv_obj_t * obj, lv_style_prop_t prop, uint8_t part, lv_state_t prev_state,
                                       lv_state_t new_state, uint32_t time, uint32_t delay, const lv_anim_path_t * path)
{
    lv_style_trans_t * tr;
    lv_obj_style_t * style_trans = _get_trans_style(obj, part);

    /*Get the previous and current values*/
    obj->style_list.skip_trans = 1;
    obj->state = prev_state;
    lv_style_value_t v1 = lv_obj_get_style_prop(obj, part, prop);
    obj->state = new_state;
    lv_style_value_t v2 = lv_obj_get_style_prop(obj, part, prop);
    obj->style_list.skip_trans = 0;

    if(v1.ptr == v2.ptr && v1.num == v2.num && v1.color.full == v2.color.full)  return;
    obj->state = prev_state;
    v1 = lv_obj_get_style_prop(obj, part, prop);
    obj->state = new_state;
    lv_style_set_prop(style_trans->style, prop, v1);   /*Be sure `trans_style` has a valid value */

    if(prop == LV_STYLE_RADIUS) {
        if(v1.num == LV_RADIUS_CIRCLE || v2.num == LV_RADIUS_CIRCLE) {
            lv_coord_t whalf = lv_obj_get_width(obj) / 2;
            lv_coord_t hhalf = lv_obj_get_width(obj) / 2;
            if(v1.num == LV_RADIUS_CIRCLE) v1.num = LV_MATH_MIN(whalf + 1, hhalf + 1);
            if(v2.num == LV_RADIUS_CIRCLE) v2.num = LV_MATH_MIN(whalf + 1, hhalf + 1);
        }
    }

    tr = _lv_ll_ins_head(&LV_GC_ROOT(_lv_obj_style_trans_ll));
    LV_ASSERT_MEM(tr);
    if(tr == NULL) return;
    tr->start_value = v1;
    tr->end_value = v2;

    if(tr) {
        tr->obj = obj;
        tr->prop = prop;
        tr->part = part;

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, tr);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)trans_anim_cb);
        lv_anim_set_start_cb(&a, trans_anim_start_cb);
        lv_anim_set_ready_cb(&a, trans_anim_ready_cb);
        lv_anim_set_values(&a, 0x00, 0xFF);
        lv_anim_set_time(&a, time);
        lv_anim_set_delay(&a, delay);
        lv_anim_set_path(&a, path);
        a.early_apply = 0;
        lv_anim_start(&a);
    }
}

#endif

/**
 * Compare the style properties of an object in 2 different states
 * @param obj pointer to an object
 * @param state1 a state
 * @param state2 an other state
 * @return an element of `_lv_style_state_cmp_t`
 */
_lv_style_state_cmp_t _lv_obj_style_state_compare(lv_obj_t * obj, lv_state_t state1, lv_state_t state2)
{
    lv_obj_style_list_t * list = &obj->style_list;
    _lv_style_state_cmp_t res = _LV_STYLE_STATE_CMP_SAME;

    /*Are there any new styles for the new state?*/
    uint32_t i;
    for(i = 0; i < list->style_cnt; i++) {
        if(list->styles[i].is_trans) continue;

        /*The style is valid for a stat but not the other*/
        bool valid1 = list->styles[i].state & (~state1) ? false : true;
        bool valid2 = list->styles[i].state & (~state2) ? false : true;
        if(valid1 != valid2) {
            lv_style_t * style = list->styles[i].style;
            lv_style_value_t v;
            /*If there is layout difference on the main part, return immediately. There is no more serious difference*/
            _lv_style_state_cmp_t res_tmp = res;
            if(lv_style_get_prop(style, LV_STYLE_PAD_TOP, &v)) res_tmp = _LV_STYLE_STATE_CMP_DIFF_LAYOUT;
            else if(lv_style_get_prop(style, LV_STYLE_PAD_BOTTOM, &v))  res_tmp = _LV_STYLE_STATE_CMP_DIFF_LAYOUT;
            else if(lv_style_get_prop(style, LV_STYLE_PAD_LEFT, &v))  res_tmp = _LV_STYLE_STATE_CMP_DIFF_LAYOUT;
            else if(lv_style_get_prop(style, LV_STYLE_PAD_RIGHT, &v))  res_tmp = _LV_STYLE_STATE_CMP_DIFF_LAYOUT;
            else if(lv_style_get_prop(style, LV_STYLE_PAD_COLUMN, &v))  res_tmp = _LV_STYLE_STATE_CMP_DIFF_LAYOUT;
            else if(lv_style_get_prop(style, LV_STYLE_PAD_ROW, &v))  res_tmp = _LV_STYLE_STATE_CMP_DIFF_LAYOUT;

            if(res_tmp == _LV_STYLE_STATE_CMP_DIFF_LAYOUT) {
                if(list->styles[i].part == LV_PART_MAIN) return _LV_STYLE_STATE_CMP_DIFF_LAYOUT;
                else {
                    res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
                    continue;
                }
            }

            /*Check for draw pad changes*/
            if(lv_style_get_prop(style, LV_STYLE_TRANSFORM_WIDTH, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_TRANSFORM_HEIGHT, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_TRANSFORM_ANGLE, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_TRANSFORM_ZOOM, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_OUTLINE_OPA, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_OUTLINE_PAD, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_SHADOW_WIDTH, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_SHADOW_OPA, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_SHADOW_OFS_X, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_SHADOW_OFS_Y, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_SHADOW_SPREAD, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_LINE_WIDTH, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_CONTENT_SRC, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_CONTENT_OFS_X, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_CONTENT_OFS_Y, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else if(lv_style_get_prop(style, LV_STYLE_CONTENT_ALIGN, &v)) res = _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD;
            else {
                if(res != _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD) res = _LV_STYLE_STATE_CMP_DIFF_REDRAW;
            }
        }
    }

    return res;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void update_cache(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop)
{
    lv_obj_style_list_t * list = &obj->style_list;
    if(part != LV_PART_MAIN) return;
    list->cache_state = LV_OBJ_STYLE_CACHE_STATE_INVALID; /*Set an invalid state to disable cache reading*/

    lv_style_value_t v;
    /*Unset or Set*/
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_OPA) {
        if(get_prop_core(obj, part, LV_STYLE_OPA, &v)) list->cache_opa_set = 1;
        else list->cache_opa_set = 0;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_TEXT_LETTER_SPACE || prop == LV_STYLE_TEXT_LINE_SPACE
       || prop == LV_STYLE_TEXT_DECOR || prop == LV_STYLE_TEXT_ALIGN)
    {
        lv_style_value_t va[4];
        if(get_prop_core(obj, part, LV_STYLE_TEXT_LETTER_SPACE, &va[0]) == false) va[0].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_TEXT_LINE_SPACE, &va[1]) == false) va[1].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_TEXT_DECOR, &va[2]) == false) va[2].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_TEXT_ALIGN, &va[3]) == false) va[3].num = 0;
        if(va[0].num || va[1].num || va[2].num || va[3].num) list->cache_text_extra_zero = 0;
        else list->cache_text_extra_zero = 1;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_TEXT_OPA) {
        if(get_prop_core(obj, part, LV_STYLE_TEXT_OPA, &v)) list->cache_text_opa_set = 1;
        else list->cache_text_opa_set = 0;
    }

    /*Indexed*/
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_RADIUS) {
        if(get_prop_core(obj, part, LV_STYLE_RADIUS, &v) == 0) v.num = 0;
        list->cache_radius_zero = v.num ? 0 : 1;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_PAD_LEFT || prop == LV_STYLE_PAD_RIGHT || prop == LV_STYLE_PAD_TOP || prop == LV_STYLE_PAD_BOTTOM) {
        lv_style_value_t va[4];
        if(get_prop_core(obj, part, LV_STYLE_PAD_LEFT, &va[0]) == 0) va[0].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_PAD_RIGHT, &va[1]) == 0) va[1].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_PAD_TOP, &va[2]) == 0) va[2].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_PAD_BOTTOM, &va[3]) == 0) va[3].num = 0;

        if(va[0].num || va[1].num || va[2].num || va[3].num) list->cache_pad_zero = 0;
        else list->cache_pad_zero = 1;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_BORDER_WIDTH) {
        if(get_prop_core(obj, part, LV_STYLE_BORDER_WIDTH, &v) == 0) v.num = 0;
        list->cache_border_width_zero = v.num ? 0 : 1;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_TEXT_COLOR_FILTERED) {
        if(get_prop_core(obj, part, LV_STYLE_TEXT_COLOR_FILTERED, &v)) {
            list->cache_text_color_set = 1;
        } else {
            list->cache_text_color_set = 0;
        }
    }

    /*Zero or Needs check*/
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_TRANSFORM_ANGLE || prop == LV_STYLE_TRANSFORM_ZOOM ||
            prop == LV_STYLE_TRANSFORM_HEIGHT || prop == LV_STYLE_TRANSFORM_WIDTH) {
        lv_style_value_t vt[4];
        if(get_prop_core(obj, part, LV_STYLE_TRANSFORM_ANGLE, &vt[0]) == false) vt[0].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_TRANSFORM_ZOOM, &vt[1]) == false) vt[1].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_TRANSFORM_WIDTH, &vt[2]) == false) vt[2].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_TRANSFORM_HEIGHT, &vt[3]) == false) vt[3].num = 0;

        if(vt[0].num == 0 && vt[1].num == 0 && vt[2].num == 0 && vt[3].num == 0) {
            list->cache_transform_zero = 1;
        } else {
            list->cache_transform_zero = 0;
        }
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_BG_BLEND_MODE || prop == LV_STYLE_TEXT_BLEND_MODE || prop == LV_STYLE_BORDER_BLEND_MODE) {
        lv_style_value_t va[5];
        if(get_prop_core(obj, part, LV_STYLE_BG_BLEND_MODE, &va[0]) == false) va[0].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_BORDER_BLEND_MODE, &va[1]) == false) va[1].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_SHADOW_BLEND_MODE, &va[1]) == false) va[2].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_OUTLINE_BLEND_MODE, &va[1]) == false) va[3].num = 0;
        if(get_prop_core(obj, part, LV_STYLE_TEXT_BLEND_MODE, &va[2]) == false) va[4].num = 0;

        if(va[0].num || va[1].num || va[2].num || va[3].num || va[4].num) list->cache_blend_mode_zero = 1;
        else list->cache_blend_mode_zero = 0;
    }

    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_BG_GRAD_DIR) {
        if(get_prop_core(obj, part, LV_STYLE_BG_GRAD_DIR, &v) == false) v.num = 0;
        if(v.num == 0) list->cache_bg_grad_dir_zero = 1;
        else list->cache_bg_grad_dir_zero = 0;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_OUTLINE_WIDTH) {
        if(get_prop_core(obj, part, LV_STYLE_OUTLINE_WIDTH, &v) == false) v.num = 0;
        if(v.num == 0) list->cache_outline_width_zero = 1;
        else list->cache_outline_width_zero = 0;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_SHADOW_WIDTH) {
        if(get_prop_core(obj, part, LV_STYLE_OUTLINE_WIDTH, &v) == false) v.num = 0;
        if(v.num == 0) list->cache_shadow_width_zero = 1;
        else list->cache_shadow_width_zero = 0;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_IMG_RECOLOR_OPA) {
        if(get_prop_core(obj, part, LV_STYLE_IMG_RECOLOR_OPA, &v) == false) v.num = 0;
        if(v.num == 0) list->cache_img_recolor_opa_zero = 1;
        else list->cache_img_recolor_opa_zero = 0;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_CONTENT_SRC) {
        if(get_prop_core(obj, part, LV_STYLE_CONTENT_SRC, &v) == false) v.ptr = NULL;
        if(v.ptr == NULL) list->cache_content_src_zero = 1;
        else list->cache_content_src_zero = 0;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_COLOR_FILTER_CB || prop == LV_STYLE_COLOR_FILTER_OPA) {
        lv_style_value_t vf[2];
        if(get_prop_core(obj, part, LV_STYLE_COLOR_FILTER_CB, &vf[0]) == false) vf[0].func = 0;
        if(get_prop_core(obj, part, LV_STYLE_COLOR_FILTER_OPA, &vf[1]) == false) vf[1].num = 0;
        if(vf[0].func == NULL || vf[1].num == 0) list->cache_filter_zero = 1;
        else list->cache_filter_zero = 0;
    }

    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_DRAWER) {
        if(get_prop_core(obj, part, LV_STYLE_DRAWER, &v) == false) v.ptr = NULL;
        if(v.ptr == NULL) list->cache_drawer_zero = 1;
        else list->cache_drawer_zero = 0;
    }

    /*1 or 0*/
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_BORDER_POST) {
        if(get_prop_core(obj, part, LV_STYLE_BORDER_POST, &v) == false) v.num = 0;
        if(v.num) list->cache_border_post_enable = 1;
        else list->cache_border_post_enable = 0;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_CLIP_CORNER) {
        if(get_prop_core(obj, part, LV_STYLE_CLIP_CORNER, &v) == false) v.num = 0;
        if(v.num) list->cache_clip_corner_enable = 1;
        else list->cache_clip_corner_enable = 0;
    }

    /*255 or Needs check*/
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_BG_OPA) {
        if(get_prop_core(obj, part, LV_STYLE_BG_OPA, &v) == false) v.num = LV_OPA_TRANSP;
        if(v.num == LV_OPA_COVER) list->cache_bg_opa_cover = 1;
        else list->cache_bg_opa_cover = 0;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_BORDER_OPA) {
        if(get_prop_core(obj, part, LV_STYLE_BORDER_OPA, &v) == false) v.num = LV_OPA_COVER;
        if(v.num == LV_OPA_COVER) list->cache_border_opa_cover = 1;
        else list->cache_border_opa_cover = 0;
    }
    if(prop == LV_STYLE_PROP_ALL || prop == LV_STYLE_IMG_OPA) {
        if(get_prop_core(obj, part, LV_STYLE_IMG_OPA, &v) == false) v.num = LV_OPA_COVER;
        if(v.num == LV_OPA_COVER) list->cache_img_opa_cover = 1;
        else list->cache_img_opa_cover = 0;
    }

    list->cache_state = obj->state;
}


static cache_t read_cache(const lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop)
{
    const lv_obj_style_list_t * list = &obj->style_list;
    if(part != LV_PART_MAIN) return CACHE_NEED_CHECK;
    if(obj->state != list->cache_state) return CACHE_NEED_CHECK;
    if(obj->style_list.skip_trans) return CACHE_NEED_CHECK;

    switch(prop) {

    case LV_STYLE_BG_BLEND_MODE:
    case LV_STYLE_BORDER_BLEND_MODE:
    case LV_STYLE_SHADOW_BLEND_MODE:
    case LV_STYLE_OUTLINE_BLEND_MODE:
        if(list->cache_blend_mode_zero ) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;
        break;

    case LV_STYLE_TEXT_BLEND_MODE:
        if(list->cache_blend_mode_zero ) return CACHE_UNSET;
        else return CACHE_NEED_CHECK;
        break;

    case LV_STYLE_OPA:
        if(list->cache_opa_set) return CACHE_NEED_CHECK;
        else return CACHE_UNSET;
        break;
    case LV_STYLE_TEXT_LETTER_SPACE:
    case LV_STYLE_TEXT_LINE_SPACE:
    case LV_STYLE_TEXT_DECOR:
    case LV_STYLE_TEXT_ALIGN:
        if(list->cache_text_extra_zero) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;
    case LV_STYLE_TEXT_OPA:
        if(list->cache_text_opa_set) return CACHE_NEED_CHECK;
        else return CACHE_UNSET;
        break;

    case LV_STYLE_RADIUS:
        if(list->cache_radius_zero) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;
    case LV_STYLE_PAD_LEFT:
    case LV_STYLE_PAD_RIGHT:
    case LV_STYLE_PAD_TOP:
    case LV_STYLE_PAD_BOTTOM:
        if(list->cache_pad_zero) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;
        break;
    case LV_STYLE_BORDER_WIDTH:
        if(list->cache_border_width_zero) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;

    case LV_STYLE_TRANSFORM_ANGLE:
    case LV_STYLE_TRANSFORM_ZOOM:
    case LV_STYLE_TRANSFORM_HEIGHT:
    case LV_STYLE_TRANSFORM_WIDTH:
        if(list->cache_transform_zero ) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;
        break;
    case LV_STYLE_BG_GRAD_DIR:
        if(list->cache_bg_grad_dir_zero ) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;
        break;
    case LV_STYLE_OUTLINE_WIDTH:
        if(list->cache_outline_width_zero ) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;
        break;
    case LV_STYLE_SHADOW_WIDTH:
        if(list->cache_shadow_width_zero ) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;
        break;
    case LV_STYLE_IMG_RECOLOR_OPA:
        if(list->cache_shadow_width_zero ) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;
        break;
    case LV_STYLE_CONTENT_SRC:
        if(list->cache_content_src_zero ) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;
        break;
    case LV_STYLE_COLOR_FILTER_CB:
    case LV_STYLE_COLOR_FILTER_OPA:
        if(list->cache_filter_zero ) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;
        break;

    case LV_STYLE_DRAWER:
        if(list->cache_drawer_zero) return CACHE_ZERO;
        else return CACHE_NEED_CHECK;
        break;

    /*1 or 0*/
    case LV_STYLE_BORDER_POST:
        if(list->cache_border_post_enable) return CACHE_TRUE;
        else return CACHE_ZERO;
        break;
    case LV_STYLE_CLIP_CORNER:
        if(list->cache_clip_corner_enable) return CACHE_TRUE;
        else return CACHE_ZERO;
        break;

    /*255 or Needs check*/
    case LV_STYLE_BG_OPA:
        if(list->cache_bg_opa_cover) return CACHE_255;
        else return CACHE_ZERO;
        break;
    case LV_STYLE_BORDER_OPA:
        if(list->cache_border_opa_cover) return CACHE_255;
        else return CACHE_ZERO;
        break;
    case LV_STYLE_IMG_OPA:
        if(list->cache_img_opa_cover) return CACHE_255;
        else return CACHE_ZERO;
        break;
    default:
        return CACHE_NEED_CHECK;
    }
}

/**
 * Refresh the style of all children of an object. (Called recursively)
 * @param style refresh objects only with this style_list.
 * @param obj pointer to an object
 */
static void report_style_change_core(void * style, lv_obj_t * obj)
{
    lv_obj_style_list_t * list = &obj->style_list;

    uint32_t i;
    for(i = 0; i < list->style_cnt; i++) {
        if(style == NULL || list->styles[i].style == style) {
            _lv_obj_refresh_style(obj, LV_STYLE_PROP_ALL);
            break;
        }
    }

    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        report_style_change_core(style, lv_obj_get_child(obj, i));
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
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        lv_obj_invalidate(child);
        lv_signal_send(child, LV_SIGNAL_STYLE_CHG, NULL);
        lv_obj_invalidate(child);

        refresh_children_style(child); /*Check children too*/
    }
}

#if LV_USE_ANIMATION
/**
 * Remove the transition from object's part's property.
 * - Remove the transition from `_lv_obj_style_trans_ll` and free it
 * - Delete pending transitions
 * @param obj pointer to an object which transition(s) should be removed
 * @param part a part of object or 0xFF to remove from all parts
 * @param prop a property or 0xFF to remove all properties
 * @param tr_limit delete transitions only "older" then this. `NULL` is not used
 */
static bool trans_del(lv_obj_t * obj, uint8_t part, lv_style_prop_t prop, lv_style_trans_t * tr_limit)
{
    lv_style_trans_t * tr;
    lv_style_trans_t * tr_prev;
    bool removed = false;
    tr = _lv_ll_get_tail(&LV_GC_ROOT(_lv_obj_style_trans_ll));
    while(tr != NULL) {
        if(tr == tr_limit) break;

        /*'tr' might be deleted, so get the next object while 'tr' is valid*/
        tr_prev = _lv_ll_get_prev(&LV_GC_ROOT(_lv_obj_style_trans_ll), tr);

        if(tr->obj == obj && (part == tr->part || part == 0xFF) && (prop == tr->prop || prop == 0xFF)) {
            /* Remove the transitioned property from trans. style
             * to allow changing it by normal styles*/
            uint32_t i;
            for(i = 0; i < obj->style_list.style_cnt; i++) {
                if(obj->style_list.styles[i].is_trans && (part == 0xff || obj->style_list.styles[i].part == part)) {
                    lv_style_remove_prop(obj->style_list.styles[i].style, tr->prop);
                    lv_anim_del(tr, NULL);
                    _lv_ll_remove(&LV_GC_ROOT(_lv_obj_style_trans_ll), tr);
                    lv_mem_free(tr);
                    removed = true;
                }
            }

        }
        tr = tr_prev;
    }
    return removed;
}

static void trans_anim_cb(lv_style_trans_t * tr, lv_anim_value_t v)
{
    lv_obj_style_list_t * list = &tr->obj->style_list;

    uint32_t i;
    for(i = 0; i < list->style_cnt; i++) {
        if(list->styles[i].is_trans == 0 || list->styles[i].part != tr->part) continue;

        lv_style_value_t value_final;
        switch (tr->prop) {

            case LV_STYLE_BORDER_SIDE:
            case LV_STYLE_BORDER_POST:
            case LV_STYLE_BG_BLEND_MODE:
            case LV_STYLE_BORDER_BLEND_MODE:
            case LV_STYLE_OUTLINE_BLEND_MODE:
            case LV_STYLE_SHADOW_BLEND_MODE:
            case LV_STYLE_TEXT_BLEND_MODE:
            case LV_STYLE_LINE_BLEND_MODE:
                if(v < 255) value_final.num = tr->start_value.num;
                else value_final.num = tr->end_value.num;
                break;
            case LV_STYLE_TRANSITION:
            case LV_STYLE_TEXT_FONT:
                if(v < 255) value_final.ptr = tr->start_value.ptr;
                else value_final.ptr = tr->end_value.ptr;
                break;
            case LV_STYLE_COLOR_FILTER_CB:
                if(tr->start_value.func == NULL) value_final.ptr = tr->end_value.func;
                else if(tr->end_value.func == NULL) value_final.ptr = tr->start_value.func;
                else if(v < 128) value_final.ptr = tr->start_value.ptr;
                else value_final.ptr = tr->end_value.ptr;
                break;
            case LV_STYLE_BG_COLOR:
            case LV_STYLE_BORDER_COLOR:
            case LV_STYLE_TEXT_COLOR:
            case LV_STYLE_SHADOW_COLOR:
            case LV_STYLE_OUTLINE_COLOR:
            case LV_STYLE_IMG_RECOLOR:
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

        lv_style_value_t old_value;
        bool refr = true;
        if(lv_style_get_prop(list->styles[i].style, tr->prop, &old_value)) {
            if(value_final.ptr == old_value.ptr && value_final.func == old_value.ptr && value_final.color.full == old_value.color.full && value_final.num == old_value.num) {
                refr = false;
            }
        }
        lv_style_set_prop(list->styles[i].style, tr->prop, value_final);
        if (refr) _lv_obj_refresh_style(tr->obj, tr->prop);
        break;

    }

}

static void trans_anim_start_cb(lv_anim_t * a)
{
    lv_style_trans_t * tr = a->var;

    tr->start_value = lv_obj_get_style_prop(tr->obj, tr->part, tr->prop);

    /*Init prop to an invalid values to be sure `trans_del` won't delete this added `tr`*/
    lv_style_prop_t prop_tmp = tr->prop;
    tr->prop = _LV_STYLE_PROP_INV;

    /*Delete the related transitions if any*/
    trans_del(tr->obj, tr->part, prop_tmp, tr);

    tr->prop = prop_tmp;

    lv_obj_style_t * style_trans = _get_trans_style(tr->obj, tr->part);
    lv_style_set_prop(style_trans->style, tr->prop, tr->start_value);   /*Be sure `trans_style` has a valid value */

}

static void trans_anim_ready_cb(lv_anim_t * a)
{
    lv_style_trans_t * tr = a->var;
    lv_obj_t * obj = tr->obj;
    lv_style_prop_t prop = tr->prop;

    /* Remove the transitioned property from trans. style
     * if there no more transitions for this property
     * It allows changing it by normal styles*/
    bool running = false;
    lv_style_trans_t * tr_i;
    _LV_LL_READ(&LV_GC_ROOT(_lv_obj_style_trans_ll), tr_i) {
        if(tr_i != tr && tr_i->obj == tr->obj && tr_i->part == tr->part && tr_i->prop == tr->prop) {
            running = true;
            break;
        }
    }

    if(!running) {
        uint32_t i;
        for(i = 0; i < obj->style_list.style_cnt; i++) {
            if(obj->style_list.styles[i].is_trans && obj->style_list.styles[i].part == tr->part) {
                _lv_ll_remove(&LV_GC_ROOT(_lv_obj_style_trans_ll), tr);
                lv_mem_free(tr);

                lv_obj_style_t * obj_style = &obj->style_list.styles[i];
                lv_style_remove_prop(obj_style->style, prop);

                if(lv_style_is_empty(obj->style_list.styles[i].style)) {
                    lv_style_t * style = obj_style->style;
                    lv_obj_remove_style(obj, obj_style->part, obj_style->state, obj_style->style);
                    lv_style_reset(style);
                    lv_mem_free(style);
                }
                break;
            }
        }
    }
}

static void fade_anim_cb(lv_obj_t * obj, lv_anim_value_t v)
{
//    lv_obj_set_style_local_opa_scale(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, v);
}

static void fade_in_anim_ready(lv_anim_t * a)
{
//    lv_style_remove_prop(_lv_obj_get_local_style(a->var, LV_OBJ_PART_MAIN), LV_STYLE_OPA_SCALE);
}

#endif

