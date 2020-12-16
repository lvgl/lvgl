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


typedef struct {
    lv_draw_rect_dsc_t rect;
//    lv_draw_label_dsc_t label;
//    lv_draw_line_dsc_t line;
//    lv_draw_img_dsc_t img;
//    lv_style_int_t pad_top;
//    lv_style_int_t pad_bottom;
//    lv_style_int_t pad_right;
//    lv_style_int_t pad_left;
//    lv_style_int_t pad_inner;
//    lv_style_int_t margin_top;
//    lv_style_int_t margin_bottom;
//    lv_style_int_t margin_left;
//    lv_style_int_t margin_right;
//    lv_style_int_t size;
//    lv_style_int_t transform_width;
//    lv_style_int_t transform_height;
//    lv_style_int_t transform_angle;
//    lv_style_int_t transform_zoom;
//    lv_style_int_t scale_width;
//    lv_style_int_t scale_border_width;
//    lv_style_int_t scale_end_border_width;
//    lv_style_int_t scale_end_line_width;
//    lv_color_t scale_grad_color;
//    lv_color_t scale_end_color;
//    lv_opa_t opa_scale;
//    uint32_t clip_corder :1;
//    uint32_t border_post :1;
}style_snapshot_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void report_style_change_core(void * style, lv_obj_t * obj);
static void refresh_children_style(lv_obj_t * obj);
#if LV_USE_ANIMATION
static void trans_del(lv_obj_t * obj, uint8_t part, lv_style_prop_t prop, lv_style_trans_t * tr_limit);
static void trans_anim_cb(lv_style_trans_t * tr, lv_anim_value_t v);
static void trans_anim_start_cb(lv_anim_t * a);
static void trans_anim_ready_cb(lv_anim_t * a);
static void fade_anim_cb(lv_obj_t * obj, lv_anim_value_t v);
static void fade_in_anim_ready(lv_anim_t * a);
#endif
static void style_snapshot(lv_obj_t * obj, uint8_t part, style_snapshot_t * shot);
static _lv_style_state_cmp_t style_snapshot_compare(style_snapshot_t * shot1, style_snapshot_t * shot2);

#if LV_STYLE_CACHE_LEVEL >= 1
static bool style_prop_is_cacheable(lv_style_property_t prop);
static void update_style_cache(lv_obj_t * obj, uint8_t part, uint16_t prop);
static void update_style_cache_children(lv_obj_t * obj);
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

/**
 * Initialize the object related style manager module.
 * Called by LVGL in `lv_init()`
 */
void _lv_obj_style_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_obj_style_trans_ll), sizeof(lv_style_trans_t));
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

    obj->style_list.styles[i].style = style;
    obj->style_list.styles[i].part = part;
    obj->style_list.styles[i].state = state;

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
    for(j = i; j > obj->style_list.style_cnt - 2 ; j--) {
        obj->style_list.styles[j] = obj->style_list.styles[j + 1];
    }


    obj->style_list.style_cnt--;
    obj->style_list.styles = lv_mem_realloc(obj->style_list.styles, obj->style_list.style_cnt * sizeof(lv_style_t));


#if LV_USE_ANIMATION
    trans_del(obj, part, 0xFF, NULL);
#endif
    _lv_obj_refresh_style(obj, LV_STYLE_PROP_ALL);
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
#if LV_USE_ANIMATION
    trans_del(obj, 0xFF, 0xFF, NULL);
#endif

    lv_mem_free(obj->style_list.styles);
    obj->style_list.styles = NULL;
    obj->style_list.style_cnt = 0;

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
    lv_style_value_t value_final;
    bool found = false;
    bool inherit = prop & LV_STYLE_PROP_INHERIT ? true : false;
    const lv_obj_t * parent = obj;
    while(parent) {
        int32_t weight = -1;
        lv_state_t state = parent->state;
        lv_state_t state_inv = ~state;
        bool skip_trans = parent->style_list.skip_trans;
        uint32_t i;
        for(i = 0; i < parent->style_list.style_cnt; i++) {
            lv_obj_style_t * obj_style = &parent->style_list.styles[i];
            if(obj_style->is_trans == false) break;
            if(skip_trans) continue;
            if(obj_style->part != part) continue;
            found = lv_style_get_prop(obj_style->style, prop, &value_act);
            if(found) return value_act;
        }

        for(; i < parent->style_list.style_cnt; i++) {
            lv_obj_style_t * obj_style = &parent->style_list.styles[i];
            if(obj_style->part != part) continue;

            /* Be sure the style not specifies other state than the requested.
             * E.g. For HOVER+PRESS object state, HOVER style only is OK, but HOVER+FOCUS style is not*/
            if((obj_style->state & state_inv)) continue;

            /*Check only better candidates*/
            if(obj_style->state <= weight) continue;

            found = lv_style_get_prop(obj_style->style, prop, &value_act);

            if(found) {
                if(obj_style->state == state) return value_act;
                if(weight < obj_style->state) {
                    weight = obj_style->state;
                    value_final = value_act;
                }
            }
        }
        if(weight >= 0) return value_final;
        if(!inherit) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_PART_MAIN) {
            part = LV_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        parent = lv_obj_get_parent(parent);
    }

    return lv_style_prop_get_default(prop);
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

    lv_obj_invalidate(obj);
    if(prop == LV_STYLE_PROP_ALL || (prop & LV_STYLE_PROP_LAYOUT_REFR)) {
        lv_signal_send(obj, LV_SIGNAL_STYLE_CHG, NULL);
        lv_signal_send(obj->parent, LV_SIGNAL_CHILD_CHG, obj);
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
//    if(memcmp(&v1, &v2, sizeof(lv_style_value_t) == 0))  return;
    if(v1._ptr == v2._ptr && v1._int == v2._int && v1._color.full == v2._color.full)  return;
    obj->state = prev_state;
    v1 = lv_obj_get_style_prop(obj, part, prop);
    obj->state = new_state;
    lv_style_set_prop(style_trans->style, prop, v1);   /*Be sure `trans_style` has a valid value */

    if(prop == LV_STYLE_RADIUS) {
        if(v1._int == LV_RADIUS_CIRCLE || v2._int == LV_RADIUS_CIRCLE) {
            lv_coord_t whalf = lv_obj_get_width(obj) / 2;
            lv_coord_t hhalf = lv_obj_get_width(obj) / 2;
            if(v1._int == LV_RADIUS_CIRCLE) v1._int = LV_MATH_MIN(whalf + 1, hhalf + 1);
            if(v2._int == LV_RADIUS_CIRCLE) v2._int = LV_MATH_MIN(whalf + 1, hhalf + 1);
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
    return _LV_STYLE_STATE_CMP_DIFF;
//    _lv_style_state_cmp_t res = _LV_STYLE_STATE_CMP_SAME;
//    uint8_t part;
//    for(part = 0; part < _LV_OBJ_PART_MAX; part++) {
//        lv_style_list_t * style_list = _lv_obj_get_style_list(obj, part);
//        if(style_list == NULL) break;   /*No more style lists*/
//
//        style_snapshot_t shot1;
//        style_snapshot_t shot2;
//
//        obj->state = state1;
//        style_snapshot(obj, part, &shot1);
//
//        obj->state = state2;
//        style_snapshot(obj, part, &shot2);
//
//        _lv_style_state_cmp_t res_part = style_snapshot_compare(&shot1, &shot2);
//        if(res_part == _LV_STYLE_STATE_CMP_DIFF) return _LV_STYLE_STATE_CMP_DIFF;
//        if(res_part == _LV_STYLE_STATE_CMP_VISUAL_DIFF) res = _LV_STYLE_STATE_CMP_VISUAL_DIFF;
//    }
//    return res;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

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
static void trans_del(lv_obj_t * obj, uint8_t part, lv_style_prop_t prop, lv_style_trans_t * tr_limit)
{
    lv_style_trans_t * tr;
    lv_style_trans_t * tr_prev;
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
                }
            }

        }
        tr = tr_prev;
    }
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
            case LV_STYLE_TRANSITION_PROP_1:
            case LV_STYLE_TRANSITION_PROP_2:
            case LV_STYLE_TRANSITION_PROP_3:
            case LV_STYLE_TRANSITION_PROP_4:
            case LV_STYLE_TRANSITION_PROP_5:
            case LV_STYLE_TRANSITION_PROP_6:
            case LV_STYLE_BG_BLEND_MODE:
            case LV_STYLE_BORDER_BLEND_MODE:
            case LV_STYLE_OUTLINE_BLEND_MODE:
            case LV_STYLE_SHADOW_BLEND_MODE:
            case LV_STYLE_TEXT_BLEND_MODE:
            case LV_STYLE_LINE_BLEND_MODE:
                if(v < 128) value_final._int = tr->start_value._int;
                else value_final._int = tr->end_value._int;
                break;
            case LV_STYLE_TEXT_FONT:
                if(v < 128) value_final._ptr = tr->start_value._ptr;
                else value_final._ptr = tr->end_value._ptr;
                break;

            case LV_STYLE_BG_COLOR:
            case LV_STYLE_BORDER_COLOR:
            case LV_STYLE_TEXT_COLOR:
            case LV_STYLE_SHADOW_COLOR:
            case LV_STYLE_OUTLINE_COLOR:
            case LV_STYLE_IMG_RECOLOR:
                if(v <= 0) value_final._color = tr->start_value._color;
                else if(v >= 255) value_final._color = tr->end_value._color;
                else value_final._color = lv_color_mix(tr->end_value._color, tr->start_value._color, v);
                break;

            default:
                if(v == 0) value_final._int = tr->start_value._int;
                else if(v == 255) value_final._int = tr->end_value._int;
                else value_final._int = tr->start_value._int + ((int32_t)((int32_t)(tr->end_value._int - tr->start_value._int) * v) >> 8);

                break;
        }

        lv_style_set_prop(list->styles[i].style, tr->prop, value_final);
        _lv_obj_refresh_style(tr->obj, tr->prop);
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
}

static void trans_anim_ready_cb(lv_anim_t * a)
{
    lv_style_trans_t * tr = a->var;

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
        for(i = 0; i < tr->obj->style_list.style_cnt; i++) {
            if(tr->obj->style_list.styles[i].is_trans && tr->obj->style_list.styles[i].part == tr->part) {
                lv_style_remove_prop(tr->obj->style_list.styles[i].style, tr->prop);
            }
        }
    }

    _lv_ll_remove(&LV_GC_ROOT(_lv_obj_style_trans_ll), tr);
    lv_mem_free(tr);
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

static void style_snapshot(lv_obj_t * obj, uint8_t part, style_snapshot_t * shot)
{
//    _lv_obj_disable_style_caching(obj, true);
//    _lv_memset_00(shot, sizeof(style_snapshot_t));
//    lv_draw_rect_dsc_init(&shot->rect);
//    lv_draw_label_dsc_init(&shot->label);
//    lv_draw_img_dsc_init(&shot->img);
//    lv_draw_line_dsc_init(&shot->line);
//
//    lv_obj_init_draw_rect_dsc(obj, part, &shot->rect);
//    lv_obj_init_draw_label_dsc(obj, part, &shot->label);
//    lv_obj_init_draw_img_dsc(obj, part, &shot->img);
//    lv_obj_init_draw_line_dsc(obj, part, &shot->line);
//
//
//    shot->pad_top = lv_obj_get_style_pad_top(obj, part);
//    shot->pad_bottom = lv_obj_get_style_pad_bottom(obj, part);
//    shot->pad_right = lv_obj_get_style_pad_right(obj, part);
//    shot->pad_left = lv_obj_get_style_pad_left(obj, part);
//    shot->margin_top = lv_obj_get_style_margin_top(obj, part);
//    shot->margin_bottom = lv_obj_get_style_margin_bottom(obj, part);
//    shot->margin_left = lv_obj_get_style_margin_left(obj, part);
//    shot->margin_right = lv_obj_get_style_margin_right(obj, part);
//    shot->size = lv_obj_get_style_size(obj, part);
//    shot->transform_width = lv_obj_get_style_transform_width(obj, part);
//    shot->transform_height = lv_obj_get_style_transform_height(obj, part);
//    shot->transform_angle = lv_obj_get_style_transform_angle(obj, part);
//    shot->transform_zoom = lv_obj_get_style_transform_zoom(obj, part);
//    shot->scale_width = lv_obj_get_style_scale_width(obj, part);
//    shot->scale_border_width = lv_obj_get_style_scale_border_width(obj, part);
//    shot->scale_end_border_width = lv_obj_get_style_scale_end_border_width(obj, part);
//    shot->scale_end_line_width = lv_obj_get_style_scale_end_line_width(obj, part);
//    shot->scale_grad_color = lv_obj_get_style_scale_grad_color(obj, part);
//    shot->scale_end_color = lv_obj_get_style_scale_end_color(obj, part);
//    shot->opa_scale = lv_obj_get_style_opa_scale(obj, part);
//    shot->clip_corder = lv_obj_get_style_clip_corner(obj, part);
//    shot->border_post  = lv_obj_get_style_border_post(obj, part);
//
//    _lv_obj_disable_style_caching(obj, false);
}

static _lv_style_state_cmp_t style_snapshot_compare(style_snapshot_t * shot1, style_snapshot_t * shot2)
{
    return _LV_STYLE_STATE_CMP_DIFF;

//    if(memcmp(shot1, shot2, sizeof(style_snapshot_t)) == 0) return _LV_STYLE_STATE_CMP_SAME;
//
//
//    if(shot1->pad_top != shot2->pad_top) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->pad_bottom != shot2->pad_bottom) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->pad_left != shot2->pad_right) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->pad_right != shot2->pad_right) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->pad_top != shot2->pad_top) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->margin_top != shot2->margin_top) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->margin_bottom != shot2->margin_bottom) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->margin_left != shot2->margin_left) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->margin_right != shot2->margin_right) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->margin_top != shot2->margin_top) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->transform_width != shot2->transform_width) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->transform_height != shot2->transform_height) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->transform_angle != shot2->transform_angle) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->transform_zoom != shot2->transform_zoom) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->rect.outline_width != shot2->rect.outline_width) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->rect.outline_pad != shot2->rect.outline_pad) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->rect.outline_opa != shot2->rect.outline_opa) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->rect.value_font != shot2->rect.value_font) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->rect.value_align != shot2->rect.value_align) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->rect.value_font != shot2->rect.value_font) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->rect.shadow_spread != shot2->rect.shadow_spread) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->rect.shadow_width != shot2->rect.shadow_width) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->rect.shadow_ofs_x != shot2->rect.shadow_ofs_x) return _LV_STYLE_STATE_CMP_DIFF;
//    if(shot1->rect.shadow_ofs_y != shot2->rect.shadow_ofs_y) return _LV_STYLE_STATE_CMP_DIFF;
//
//    /*If not returned earlier its just a visual difference, a simple redraw is enough*/
//    return _LV_STYLE_STATE_CMP_VISUAL_DIFF;
}
