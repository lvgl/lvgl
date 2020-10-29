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
    lv_style_property_t prop;
    uint8_t part;
    union {
        lv_color_t _color;
        lv_style_int_t _int;
        lv_opa_t _opa;
        const void * _ptr;
    } start_value;
    union {
        lv_color_t _color;
        lv_style_int_t _int;
        lv_opa_t _opa;
        const void * _ptr;
    } end_value;
} lv_style_trans_t;


typedef struct {
    lv_draw_rect_dsc_t rect;
    lv_draw_label_dsc_t label;
    lv_draw_line_dsc_t line;
    lv_draw_img_dsc_t img;
    lv_style_int_t pad_top;
    lv_style_int_t pad_bottom;
    lv_style_int_t pad_right;
    lv_style_int_t pad_left;
    lv_style_int_t pad_inner;
    lv_style_int_t margin_top;
    lv_style_int_t margin_bottom;
    lv_style_int_t margin_left;
    lv_style_int_t margin_right;
    lv_style_int_t size;
    lv_style_int_t transform_width;
    lv_style_int_t transform_height;
    lv_style_int_t transform_angle;
    lv_style_int_t transform_zoom;
    lv_style_int_t scale_width;
    lv_style_int_t scale_border_width;
    lv_style_int_t scale_end_border_width;
    lv_style_int_t scale_end_line_width;
    lv_color_t scale_grad_color;
    lv_color_t scale_end_color;
    lv_opa_t opa_scale;
    uint32_t clip_corder :1;
    uint32_t border_post :1;
}style_snapshot_t;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void report_style_change_core(void * style, lv_obj_t * obj);
static void refresh_children_style(lv_obj_t * obj);
#if LV_USE_ANIMATION
static void trans_del(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, lv_style_trans_t * tr_limit);
static void trans_anim_cb(lv_style_trans_t * tr, lv_anim_value_t v);
static void trans_anim_start_cb(lv_anim_t * a);
static void trans_anim_ready_cb(lv_anim_t * a);
static void style_snapshot(lv_obj_t * obj, uint8_t part, style_snapshot_t * shot);
static _lv_style_state_cmp_t style_snapshot_compare(style_snapshot_t * shot1, style_snapshot_t * shot2);
#endif

#if LV_STYLE_CACHE_LEVEL >= 1
static bool style_prop_is_cacheable(lv_style_property_t prop);
static void update_style_cache(lv_obj_t * obj, uint8_t part, uint16_t prop);
static void update_style_cache_children(lv_obj_t * obj);
#endif
static void fade_anim_cb(lv_obj_t * obj, lv_anim_value_t v);
static void fade_in_anim_ready(lv_anim_t * a);

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
void lv_obj_add_style(lv_obj_t * obj, uint8_t part, lv_style_t * style)
{
    if(style == NULL) return;

    lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
    if(list == NULL) {
        LV_LOG_WARN("Can't find style with part: %d", part);
        return;
    }

    _lv_style_list_add_style(list, style);
#if LV_USE_ANIMATION
    trans_del(obj, part, 0xFF, NULL);
#endif
    _lv_obj_refresh_style(obj, part, LV_STYLE_PROP_ALL);
}

/**
 * Remove a style from the style list of an object.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 *              E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param style pointer to a style to remove
 */
void lv_obj_remove_style(lv_obj_t * obj, uint8_t part, lv_style_t * style)
{
    if(style == NULL) return;

    lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
    if(list == NULL) {
        LV_LOG_WARN("Can't find style with part: %d", part);
        return;
    }

    _lv_style_list_remove_style(list, style);
#if LV_USE_ANIMATION
    trans_del(obj, part, 0xFF, NULL);
#endif
    _lv_obj_refresh_style(obj, part, LV_STYLE_PROP_ALL);
}

/**
 * Reset a style to the default (empty) state.
 * Release all used memories and cancel pending related transitions.
 * Also notifies the object about the style change.
 * @param obj pointer to an object
 * @param part the part of the object which style list should be reseted.
 *             E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 */
void lv_obj_reset_style_list(lv_obj_t * obj, uint8_t part)
{
    _lv_obj_reset_style_list_no_refr(obj, part);

    _lv_obj_refresh_style(obj, part, LV_STYLE_PROP_ALL);
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
        lv_obj_t * i;
        _LV_LL_READ(&d->scr_ll, i) {
            report_style_change_core(style, i);
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
bool lv_obj_remove_style_local_prop(lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_style_t * style = _lv_obj_get_local_style(obj, part);
    if(style) return lv_style_remove_prop(style, prop);
    else return false;
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
 * Get the style list of part of an object
 * @param obj pointer to an object
 * @param part the part of the object.
 *             E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @return pointer to the style list
 */
lv_style_list_t * _lv_obj_get_style_list(const lv_obj_t * obj, uint8_t part)
{
    if(part == LV_OBJ_PART_MAIN) return &((lv_obj_t *)obj)->style_list;

    lv_get_style_info_t info;
    info.part = part;
    info.result = NULL;

    lv_res_t res;
    res = lv_signal_send((lv_obj_t *)obj, LV_SIGNAL_GET_STYLE, &info);

    if(res != LV_RES_OK) return NULL;

    return info.result;
}

/**
 * Enable/disable the use of style cache for an object
 * @param obj pointer to an object
 * @param dis true: disable; false: enable (re-enable)
 */
void _lv_obj_disable_style_caching(lv_obj_t * obj, bool dis)
{
#if LV_STYLE_CACHE_LEVEL >= 1
    uint8_t part;
    for(part = 0; part < _LV_OBJ_PART_REAL_FIRST; part++) {
        lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
        if(list == NULL) break;
        list->ignore_cache = dis;
    }
    for(part = _LV_OBJ_PART_REAL_FIRST; part < 0xFF; part++) {
        lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
        if(list == NULL) break;
        list->ignore_cache = dis;
    }
#else
    LV_UNUSED(obj);
    LV_UNUSED(dis);
#endif
}


#if LV_STYLE_CACHE_LEVEL > 0

/**
 * Mark the object and all of it's children's style lists as invalid.
 * The cache will be updated when a cached property asked nest time
 * @param obj pointer to an object
 */
void _lv_obj_invalidate_style_cache(lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    if(style_prop_is_cacheable(prop) == false) return;

    if(part != LV_OBJ_PART_ALL) {
        lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
        if(list == NULL) return;
        list->valid_cache = 0;
    }
    else {

        for(part = 0; part < _LV_OBJ_PART_REAL_FIRST; part++) {
            lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
            if(list == NULL) break;
            list->valid_cache = 0;
        }
        for(part = _LV_OBJ_PART_REAL_FIRST; part < 0xFF; part++) {
            lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
            if(list == NULL) break;
            list->valid_cache = 0;
        }
    }

    lv_obj_t * child = lv_obj_get_child(obj, NULL);
    while(child) {
        update_style_cache_children(child);
        child = lv_obj_get_child(obj, child);
    }
}
#endif /*LV_STYLE_CACHE_LEVEL >= 1*/

/**
 * Get a style property of a part of an object in the object's current state.
 * If there is a running transitions it is taken into account
 * @param obj pointer to an object
 * @param part the part of the object which style property should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop the property to get. E.g. `LV_STYLE_BORDER_WIDTH`.
 *  The state of the object will be added internally
 * @return the value of the property of the given part in the current state.
 * If the property is not found a default value will be returned.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_width()`
 * @note for performance reasons it's not checked if the property really has integer type
 */
lv_style_int_t _lv_obj_get_style_int(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    lv_style_property_t prop_ori = prop;

    lv_style_attr_t attr;
    attr = prop_ori >> 8;

    lv_style_int_t value_act;
    lv_res_t res = LV_RES_INV;
    const lv_obj_t * parent = obj;
    while(parent) {
        lv_style_list_t * list = _lv_obj_get_style_list(parent, part);
#if LV_STYLE_CACHE_LEVEL >= 1
        if(!list->ignore_cache && list->style_cnt > 0) {
            if(!list->valid_cache) update_style_cache((lv_obj_t *)parent, part, prop  & (~LV_STYLE_STATE_MASK));

            bool def = false;
            switch(prop  & (~LV_STYLE_STATE_MASK)) {
                case LV_STYLE_CLIP_CORNER:
                    if(list->clip_corner_off) def = true;
                    break;
                case LV_STYLE_TEXT_LETTER_SPACE:
                case LV_STYLE_TEXT_LINE_SPACE:
                    if(list->text_space_zero) def = true;
                    break;
                case LV_STYLE_TRANSFORM_ANGLE:
                case LV_STYLE_TRANSFORM_WIDTH:
                case LV_STYLE_TRANSFORM_HEIGHT:
                case LV_STYLE_TRANSFORM_ZOOM:
                    if(list->transform_all_zero) def = true;
                    break;
                case LV_STYLE_BORDER_WIDTH:
                    if(list->border_width_zero) def = true;
                    break;
                case LV_STYLE_BORDER_SIDE:
                    if(list->border_side_full) def = true;
                    break;
                case LV_STYLE_BORDER_POST:
                    if(list->border_post_off) def = true;
                    break;
                case LV_STYLE_OUTLINE_WIDTH:
                    if(list->outline_width_zero) def = true;
                    break;
                case LV_STYLE_RADIUS:
                    if(list->radius_zero) def = true;
                    break;
                case LV_STYLE_SHADOW_WIDTH:
                    if(list->shadow_width_zero) def = true;
                    break;
                case LV_STYLE_PAD_TOP:
                    if(list->pad_all_zero) def = true;
                    else if(list->pad_top) return list->pad_top;
                    break;
                case LV_STYLE_PAD_BOTTOM:
                    if(list->pad_all_zero) def = true;
                    break;
                case LV_STYLE_PAD_LEFT:
                    if(list->pad_all_zero) def = true;
                    else if(list->pad_left) return list->pad_left;
                    break;
                case LV_STYLE_PAD_RIGHT:
                    if(list->pad_all_zero) def = true;
                    break;
                case LV_STYLE_MARGIN_TOP:
                case LV_STYLE_MARGIN_BOTTOM:
                case LV_STYLE_MARGIN_LEFT:
                case LV_STYLE_MARGIN_RIGHT:
                    if(list->margin_all_zero) def = true;
                    break;
                case LV_STYLE_BG_BLEND_MODE:
                case LV_STYLE_BORDER_BLEND_MODE:
                case LV_STYLE_IMAGE_BLEND_MODE:
                case LV_STYLE_LINE_BLEND_MODE:
                case LV_STYLE_OUTLINE_BLEND_MODE:
                case LV_STYLE_PATTERN_BLEND_MODE:
                case LV_STYLE_SHADOW_BLEND_MODE:
                case LV_STYLE_TEXT_BLEND_MODE:
                case LV_STYLE_VALUE_BLEND_MODE:
                    if(list->blend_mode_all_normal) def = true;
                    break;
                case LV_STYLE_TEXT_DECOR:
                     if(list->text_decor_none) def = true;
                     break;
            }

            if(def) {
                break;
            }
        }
#endif

        lv_state_t state = lv_obj_get_state(parent);
        prop = (uint16_t)prop_ori + ((uint16_t)state << LV_STYLE_STATE_POS);

        res = _lv_style_list_get_int(list, prop, &value_act);
        if(res == LV_RES_OK) return value_act;

        if(LV_STYLE_ATTR_GET_INHERIT(attr) == 0) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_OBJ_PART_MAIN) {
            part = LV_OBJ_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        parent = lv_obj_get_parent(parent);
    }

    /*Handle unset values*/
    prop = prop & (~LV_STYLE_STATE_MASK);
    switch(prop) {
        case LV_STYLE_BORDER_SIDE:
            return LV_BORDER_SIDE_FULL;
        case LV_STYLE_SIZE:
            return LV_DPI / 20;
        case LV_STYLE_SCALE_WIDTH:
            return LV_DPI / 8;
        case LV_STYLE_BG_GRAD_STOP:
            return 255;
        case LV_STYLE_TRANSFORM_ZOOM:
            return LV_IMG_ZOOM_NONE;
        case LV_STYLE_SCROLLBAR_TICKNESS:
            return LV_DPX(10);
    }

    return 0;
}

/**
 * Get a style property of a part of an object in the object's current state.
 * If there is a running transitions it is taken into account
 * @param obj pointer to an object
 * @param part the part of the object which style property should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop the property to get. E.g. `LV_STYLE_BORDER_COLOR`.
 *  The state of the object will be added internally
 * @return the value of the property of the given part in the current state.
 * If the property is not found a default value will be returned.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_color()`
 * @note for performance reasons it's not checked if the property really has color type
 */
lv_color_t _lv_obj_get_style_color(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    lv_style_property_t prop_ori = prop;

    lv_style_attr_t attr;
    attr = prop_ori >> 8;

    lv_color_t value_act;
    lv_res_t res = LV_RES_INV;
    const lv_obj_t * parent = obj;
    while(parent) {
        lv_style_list_t * list = _lv_obj_get_style_list(parent, part);

        lv_state_t state = lv_obj_get_state(parent);
        prop = (uint16_t)prop_ori + ((uint16_t)state << LV_STYLE_STATE_POS);

        res = _lv_style_list_get_color(list, prop, &value_act);
        if(res == LV_RES_OK) return value_act;

        if(LV_STYLE_ATTR_GET_INHERIT(attr) == 0) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_OBJ_PART_MAIN) {
            part = LV_OBJ_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        parent = lv_obj_get_parent(parent);
    }

    /*Handle unset values*/
    prop = prop & (~LV_STYLE_STATE_MASK);
    switch(prop) {
        case LV_STYLE_BG_COLOR:
        case LV_STYLE_BG_GRAD_COLOR:
            return LV_COLOR_WHITE;
    }

    return LV_COLOR_BLACK;
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
lv_opa_t _lv_obj_get_style_opa(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    lv_style_property_t prop_ori = prop;

    lv_style_attr_t attr;
    attr = prop_ori >> 8;

    lv_opa_t value_act;
    lv_res_t res = LV_RES_INV;
    const lv_obj_t * parent = obj;
    while(parent) {
        lv_style_list_t * list = _lv_obj_get_style_list(parent, part);
#if LV_STYLE_CACHE_LEVEL >= 1
        if(!list->ignore_cache && list->style_cnt > 0) {
            if(!list->valid_cache) update_style_cache((lv_obj_t *)parent, part, prop  & (~LV_STYLE_STATE_MASK));
            bool def = false;
            switch(prop & (~LV_STYLE_STATE_MASK)) {
                case LV_STYLE_OPA_SCALE:
                    if(list->opa_scale_cover) def = true;
                    break;
                case LV_STYLE_BG_OPA:
                    if(list->bg_opa_cover) return LV_OPA_COVER;     /*Special case, not the default value is used*/
                    if(list->bg_opa_transp) def = true;
                    break;
                case LV_STYLE_IMAGE_RECOLOR_OPA:
                    if(list->img_recolor_opa_transp) def = true;
                    break;
            }

            if(def) {
                break;
            }
        }
#endif

        lv_state_t state = lv_obj_get_state(parent);
        prop = (uint16_t)prop_ori + ((uint16_t)state << LV_STYLE_STATE_POS);

        res = _lv_style_list_get_opa(list, prop, &value_act);
        if(res == LV_RES_OK) return value_act;

        if(LV_STYLE_ATTR_GET_INHERIT(attr) == 0) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_OBJ_PART_MAIN) {
            part = LV_OBJ_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        parent = lv_obj_get_parent(parent);
    }

    /*Handle unset values*/
    prop = prop & (~LV_STYLE_STATE_MASK);
    switch(prop) {
        case LV_STYLE_BG_OPA:
        case LV_STYLE_IMAGE_RECOLOR_OPA:
        case LV_STYLE_PATTERN_RECOLOR_OPA:
            return LV_OPA_TRANSP;
    }

    return LV_OPA_COVER;
}

/**
 * Get a style property of a part of an object in the object's current state.
 * If there is a running transitions it is taken into account
 * @param obj pointer to an object
 * @param part the part of the object which style property should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop the property to get. E.g. `LV_STYLE_TEXT_FONT`.
 *  The state of the object will be added internally
 * @return the value of the property of the given part in the current state.
 * If the property is not found a default value will be returned.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has pointer type
 */
const void * _lv_obj_get_style_ptr(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    lv_style_property_t prop_ori = prop;

    lv_style_attr_t attr;
    attr = prop_ori >> 8;

    const void * value_act;
    lv_res_t res = LV_RES_INV;
    const lv_obj_t * parent = obj;
    while(parent) {
        lv_style_list_t * list = _lv_obj_get_style_list(parent, part);
#if LV_STYLE_CACHE_LEVEL >= 1
        if(!list->ignore_cache && list->style_cnt > 0) {
            if(!list->valid_cache) update_style_cache((lv_obj_t *)parent, part, prop  & (~LV_STYLE_STATE_MASK));
            bool def = false;
            switch(prop  & (~LV_STYLE_STATE_MASK)) {
                case LV_STYLE_VALUE_STR:
                    if(list->value_txt_str) def = true;
                    break;
                case LV_STYLE_PATTERN_IMAGE:
                    if(list->pattern_img_null) def = true;
                    break;
                case LV_STYLE_TEXT_FONT:
                    if(list->text_font_normal) def = true;
                    break;
            }

            if(def) {
                break;
            }
        }
#endif
        lv_state_t state = lv_obj_get_state(parent);
        prop = (uint16_t)prop_ori + ((uint16_t)state << LV_STYLE_STATE_POS);

        res = _lv_style_list_get_ptr(list, prop, &value_act);
        if(res == LV_RES_OK)  return value_act;

        if(LV_STYLE_ATTR_GET_INHERIT(attr) == 0) break;

        /*If not found, check the `MAIN` style first*/
        if(part != LV_OBJ_PART_MAIN) {
            part = LV_OBJ_PART_MAIN;
            continue;
        }

        /*Check the parent too.*/
        parent = lv_obj_get_parent(parent);
    }

    /*Handle unset values*/
    prop = prop & (~LV_STYLE_STATE_MASK);
    switch(prop) {
        case LV_STYLE_TEXT_FONT:
        case LV_STYLE_VALUE_FONT:
            return lv_theme_get_font_normal();
#if LV_USE_ANIMATION
        case LV_STYLE_TRANSITION_PATH:
            return &lv_anim_path_def;
#endif
    }

    return NULL;
}

/**
 * Get the local style of a part of an object.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @return pointer to the local style if exists else `NULL`.
 */
lv_style_t * _lv_obj_get_local_style(lv_obj_t * obj, uint8_t part)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_style_list_t * style_list = _lv_obj_get_style_list(obj, part);
    return lv_style_list_get_local_style(style_list);
}


/**
 * Set a local style property of a part of an object in a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 *              E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 *             E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param the value to set
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_width()`
 * @note for performance reasons it's not checked if the property really has integer type
 */
void _lv_obj_set_style_local_int(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, lv_style_int_t value)
{
    lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
    _lv_style_list_set_local_int(list, prop, value);
#if LV_USE_ANIMATION
    trans_del(obj, part, prop, NULL);
#endif
    _lv_obj_refresh_style(obj, part, prop & (~LV_STYLE_STATE_MASK));
}

/**
 * Set a local style property of a part of an object in a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 *             E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 *             E.g. `LV_STYLE_BORDER_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param the value to set
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has color type
 */
void _lv_obj_set_style_local_color(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, lv_color_t color)
{
    lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
    _lv_style_list_set_local_color(list, prop, color);
#if LV_USE_ANIMATION
    trans_del(obj, part, prop, NULL);
#endif
    _lv_obj_refresh_style(obj, part, prop & (~LV_STYLE_STATE_MASK));
}

/**
 * Set a local style property of a part of an object in a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 *             E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 *             E.g. `LV_STYLE_BORDER_OPA | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param the value to set
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has opacity type
 */
void _lv_obj_set_style_local_opa(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, lv_opa_t opa)
{
    lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
    _lv_style_list_set_local_opa(list, prop, opa);
#if LV_USE_ANIMATION
    trans_del(obj, part, prop, NULL);
#endif
    _lv_obj_refresh_style(obj, part, prop & (~LV_STYLE_STATE_MASK));
}

/**
 * Set a local style property of a part of an object in a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 *             E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 *             E.g. `LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has pointer type
 */
void _lv_obj_set_style_local_ptr(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, const void * value)
{
    lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
    _lv_style_list_set_local_ptr(list, prop, value);
#if LV_USE_ANIMATION
    trans_del(obj, part, prop, NULL);
#endif
    _lv_obj_refresh_style(obj, part, prop & (~LV_STYLE_STATE_MASK));
}

/**
 * Reset a style to the default (empty) state.
 * Release all used memories and cancel pending related transitions.
 * Typically used in `LV_SIGNAL_CLEAN_UP.
 * @param obj pointer to an object
 * @param part the part of the object which style list should be reseted.
 *             E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 */
void _lv_obj_reset_style_list_no_refr(lv_obj_t * obj, uint8_t part)
{
    lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
    if(list == NULL) {
        LV_LOG_WARN("_lv_obj_reset_style_list_no_refr: can't find style with `part`");
        return;
    }

    _lv_style_list_reset(list);
#if LV_USE_ANIMATION
    trans_del(obj, part, 0xFF, NULL);
#endif
}


/**
 * Notify an object and its children about its style is modified
 * @param obj pointer to an object
 * @param part the part of the object which style property should be refreshed.
 * @param prop `LV_STYLE_PROP_ALL` or an `LV_STYLE_...` property. It is used to optimize what needs to be refreshed.
 */
void _lv_obj_refresh_style(lv_obj_t * obj, uint8_t part, lv_style_property_t prop)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
#if LV_STYLE_CACHE_LEVEL >= 1
    _lv_obj_invalidate_style_cache(obj, part, prop);
#endif

    /*If a real style refresh is required*/
    bool real_refr = false;
    switch(prop) {
        case LV_STYLE_PROP_ALL:
        case LV_STYLE_CLIP_CORNER:
        case LV_STYLE_SIZE:
        case LV_STYLE_TRANSFORM_WIDTH:
        case LV_STYLE_TRANSFORM_HEIGHT:
        case LV_STYLE_TRANSFORM_ANGLE:
        case LV_STYLE_TRANSFORM_ZOOM:
        case LV_STYLE_PAD_TOP:
        case LV_STYLE_PAD_BOTTOM:
        case LV_STYLE_PAD_LEFT:
        case LV_STYLE_PAD_RIGHT:
        case LV_STYLE_MARGIN_TOP:
        case LV_STYLE_MARGIN_BOTTOM:
        case LV_STYLE_MARGIN_LEFT:
        case LV_STYLE_MARGIN_RIGHT:
        case LV_STYLE_OUTLINE_WIDTH:
        case LV_STYLE_OUTLINE_PAD:
        case LV_STYLE_OUTLINE_OPA:
        case LV_STYLE_SHADOW_WIDTH:
        case LV_STYLE_SHADOW_OPA:
        case LV_STYLE_SHADOW_OFS_X:
        case LV_STYLE_SHADOW_OFS_Y:
        case LV_STYLE_SHADOW_SPREAD:
        case LV_STYLE_VALUE_LETTER_SPACE:
        case LV_STYLE_VALUE_LINE_SPACE:
        case LV_STYLE_VALUE_OFS_X:
        case LV_STYLE_VALUE_OFS_Y:
        case LV_STYLE_VALUE_ALIGN:
        case LV_STYLE_VALUE_STR:
        case LV_STYLE_VALUE_FONT:
        case LV_STYLE_VALUE_OPA:
        case LV_STYLE_TEXT_LETTER_SPACE:
        case LV_STYLE_TEXT_LINE_SPACE:
        case LV_STYLE_TEXT_FONT:
        case LV_STYLE_LINE_WIDTH:
            real_refr = true;
            break;
        default:
            real_refr = false;
    }

    if(real_refr) {
        lv_obj_invalidate(obj);
        obj->signal_cb(obj, LV_SIGNAL_STYLE_CHG, NULL);

        switch(prop) {
            case LV_STYLE_PROP_ALL:
            case LV_STYLE_MARGIN_TOP:
            case LV_STYLE_MARGIN_BOTTOM:
            case LV_STYLE_MARGIN_LEFT:
            case LV_STYLE_MARGIN_RIGHT:
                if(obj->parent) obj->parent->signal_cb(obj->parent, LV_SIGNAL_CHILD_CHG, obj);
                break;
        }

        lv_obj_invalidate(obj);

        /*Send style change signals*/
        if(prop == LV_STYLE_PROP_ALL || (prop & LV_STYLE_INHERIT_MASK)) refresh_children_style(obj);
    }
    else {
        lv_obj_invalidate(obj);
    }
}

/**
 * Remove all transitions from an object
 * @param obj pointer to an object
 */
void _lv_obj_remove_style_trans(lv_obj_t * obj)
{
    trans_del(obj, 0xFF, 0xFF, NULL);
}

#if LV_USE_ANIMATION

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
void _lv_obj_create_style_transition(lv_obj_t * obj, lv_style_property_t prop, uint8_t part, lv_state_t prev_state,
                                       lv_state_t new_state, uint32_t time, uint32_t delay, lv_anim_path_t * path)
{
    lv_style_trans_t * tr;
    lv_style_list_t * style_list = _lv_obj_get_style_list(obj, part);
    lv_style_t * style_trans = _lv_style_list_get_transition_style(style_list);

    /*Get the previous and current values*/
    if((prop & 0xF) < LV_STYLE_ID_COLOR) { /*Int*/
        style_list->skip_trans = 1;
        obj->state = prev_state;
        lv_style_int_t int1 = _lv_obj_get_style_int(obj, part, prop);
        obj->state = new_state;
        lv_style_int_t int2 =  _lv_obj_get_style_int(obj, part, prop);
        style_list->skip_trans = 0;

        if(int1 == int2)  return;
        obj->state = prev_state;
        int1 = _lv_obj_get_style_int(obj, part, prop);
        obj->state = new_state;
        _lv_style_set_int(style_trans, prop, int1);   /*Be sure `trans_style` has a valid value */

        if(prop == LV_STYLE_RADIUS) {
            if(int1 == LV_RADIUS_CIRCLE || int2 == LV_RADIUS_CIRCLE) {
                lv_coord_t whalf = lv_obj_get_width(obj) / 2;
                lv_coord_t hhalf = lv_obj_get_width(obj) / 2;
                if(int1 == LV_RADIUS_CIRCLE) int1 = LV_MATH_MIN(whalf + 1, hhalf + 1);
                if(int2 == LV_RADIUS_CIRCLE) int2 = LV_MATH_MIN(whalf + 1, hhalf + 1);
            }
        }

        tr = _lv_ll_ins_head(&LV_GC_ROOT(_lv_obj_style_trans_ll));
        LV_ASSERT_MEM(tr);
        if(tr == NULL) return;
        tr->start_value._int = int1;
        tr->end_value._int = int2;
    }
    else if((prop & 0xF) < LV_STYLE_ID_OPA) { /*Color*/
        style_list->skip_trans = 1;
        obj->state = prev_state;
        lv_color_t c1 = _lv_obj_get_style_color(obj, part, prop);
        obj->state = new_state;
        lv_color_t c2 =  _lv_obj_get_style_color(obj, part, prop);
        style_list->skip_trans = 0;

        if(c1.full == c2.full) return;
        obj->state = prev_state;
        c1 = _lv_obj_get_style_color(obj, part, prop);
        obj->state = new_state;
        _lv_style_set_color(style_trans, prop, c1);    /*Be sure `trans_style` has a valid value */

        tr = _lv_ll_ins_head(&LV_GC_ROOT(_lv_obj_style_trans_ll));
        LV_ASSERT_MEM(tr);
        if(tr == NULL) return;
        tr->start_value._color = c1;
        tr->end_value._color = c2;
    }
    else if((prop & 0xF) < LV_STYLE_ID_PTR) { /*Opa*/
        style_list->skip_trans = 1;
        obj->state = prev_state;
        lv_opa_t o1 = _lv_obj_get_style_opa(obj, part, prop);
        obj->state = new_state;
        lv_opa_t o2 =  _lv_obj_get_style_opa(obj, part, prop);
        style_list->skip_trans = 0;

        if(o1 == o2) return;

        obj->state = prev_state;
        o1 = _lv_obj_get_style_opa(obj, part, prop);
        obj->state = new_state;
        _lv_style_set_opa(style_trans, prop, o1);   /*Be sure `trans_style` has a valid value */

        tr = _lv_ll_ins_head(&LV_GC_ROOT(_lv_obj_style_trans_ll));
        LV_ASSERT_MEM(tr);
        if(tr == NULL) return;
        tr->start_value._opa = o1;
        tr->end_value._opa = o2;
    }
    else {      /*Ptr*/
        obj->state = prev_state;
        style_list->skip_trans = 1;
        const void * p1 = _lv_obj_get_style_ptr(obj, part, prop);
        obj->state = new_state;
        const void * p2 = _lv_obj_get_style_ptr(obj, part, prop);
        style_list->skip_trans = 0;

        if(memcmp(&p1, &p2, sizeof(const void *)) == 0)  return;
        obj->state = prev_state;
        p1 = _lv_obj_get_style_ptr(obj, part, prop);
        obj->state = new_state;
        _lv_style_set_ptr(style_trans, prop, p1);   /*Be sure `trans_style` has a valid value */

        tr = _lv_ll_ins_head(&LV_GC_ROOT(_lv_obj_style_trans_ll));
        LV_ASSERT_MEM(tr);
        if(tr == NULL) return;
        tr->start_value._ptr = p1;
        tr->end_value._ptr = p2;
    }

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
    _lv_style_state_cmp_t res = _LV_STYLE_STATE_CMP_SAME;
    uint8_t part;
    for(part = 0; part < _LV_OBJ_PART_REAL_FIRST; part++) {
        lv_style_list_t * style_list = _lv_obj_get_style_list(obj, part);
        if(style_list == NULL) break;   /*No more style lists*/

        style_snapshot_t shot1;
        style_snapshot_t shot2;

        obj->state = state1;
        style_snapshot(obj, part, &shot1);

        obj->state = state2;
        style_snapshot(obj, part, &shot2);

        _lv_style_state_cmp_t res_part = style_snapshot_compare(&shot1, &shot2);
        if(res_part == _LV_STYLE_STATE_CMP_DIFF) return _LV_STYLE_STATE_CMP_DIFF;
        if(res_part == _LV_STYLE_STATE_CMP_VISUAL_DIFF) res = _LV_STYLE_STATE_CMP_VISUAL_DIFF;
    }
    return res;
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
    uint8_t part;
    for(part = 0; part < _LV_OBJ_PART_SPACIAL_START; part++) {
        lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
        if(list == NULL) break;

        uint8_t ci;
        for(ci = 0; ci < list->style_cnt; ci++) {
            lv_style_t * class = lv_style_list_get_style(list, ci);
            if(class == style || style == NULL) {
                _lv_obj_refresh_style(obj, part, LV_STYLE_PROP_ALL);
                break;
            }
        }
    }

    lv_obj_t * child = lv_obj_get_child(obj, NULL);
    while(child) {
        report_style_change_core(style, child);
        child = lv_obj_get_child(obj, child);
    }

}

/**
 * Recursively refresh the style of the children. Go deeper until a not NULL style is found
 * because the NULL styles are inherited from the parent
 * @param obj pointer to an object
 */
static void refresh_children_style(lv_obj_t * obj)
{
    lv_obj_t * child = lv_obj_get_child(obj, NULL);
    while(child != NULL) {
        lv_obj_invalidate(child);
        child->signal_cb(child, LV_SIGNAL_STYLE_CHG, NULL);
        lv_obj_invalidate(child);

        refresh_children_style(child); /*Check children too*/
        child = lv_obj_get_child(obj, child);
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
static void trans_del(lv_obj_t * obj, uint8_t part, lv_style_property_t prop, lv_style_trans_t * tr_limit)
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
            lv_style_list_t * list = _lv_obj_get_style_list(tr->obj, tr->part);
            lv_style_t * style_trans = _lv_style_list_get_transition_style(list);
            lv_style_remove_prop(style_trans, tr->prop);

            lv_anim_del(tr, NULL);
            _lv_ll_remove(&LV_GC_ROOT(_lv_obj_style_trans_ll), tr);
            lv_mem_free(tr);
        }
        tr = tr_prev;
    }
}

static void trans_anim_cb(lv_style_trans_t * tr, lv_anim_value_t v)
{
    lv_style_list_t * list = _lv_obj_get_style_list(tr->obj, tr->part);
    lv_style_t * style = _lv_style_list_get_transition_style(list);

    if((tr->prop & 0xF) < LV_STYLE_ID_COLOR) { /*Value*/
        lv_style_int_t x;
        if(v == 0) x = tr->start_value._int;
        else if(v == 255) x = tr->end_value._int;
        else x = tr->start_value._int + ((int32_t)((int32_t)(tr->end_value._int - tr->start_value._int) * v) >> 8);
        _lv_style_set_int(style, tr->prop, x);
    }
    else if((tr->prop & 0xF) < LV_STYLE_ID_OPA) { /*Color*/
        lv_color_t x;
        if(v <= 0) x = tr->start_value._color;
        else if(v >= 255) x = tr->end_value._color;
        else x = lv_color_mix(tr->end_value._color, tr->start_value._color, v);
        _lv_style_set_color(style, tr->prop, x);
    }
    else if((tr->prop & 0xF) < LV_STYLE_ID_PTR) { /*Opa*/
        lv_opa_t x;
        if(v <= 0) x = tr->start_value._opa;
        else if(v >= 255) x = tr->end_value._opa;
        else x = tr->start_value._opa + (((tr->end_value._opa - tr->start_value._opa) * v) >> 8);
        _lv_style_set_opa(style, tr->prop, x);
    }
    else {
        const void * x;
        if(v < 128) x = tr->start_value._ptr;
        else x = tr->end_value._ptr;
        _lv_style_set_ptr(style, tr->prop, x);
    }
    _lv_obj_refresh_style(tr->obj, tr->part, tr->prop);

}

static void trans_anim_start_cb(lv_anim_t * a)
{
    lv_style_trans_t * tr = a->var;

    lv_style_property_t prop_tmp = tr->prop;

    /*Start the animation from the current value*/
    if((prop_tmp & 0xF) < LV_STYLE_ID_COLOR) { /*Int*/
        tr->start_value._int = _lv_obj_get_style_int(tr->obj, tr->part, prop_tmp);
    }
    else if((prop_tmp & 0xF) < LV_STYLE_ID_OPA) { /*Color*/
        tr->start_value._color = _lv_obj_get_style_color(tr->obj, tr->part, prop_tmp);
    }
    else if((prop_tmp & 0xF) < LV_STYLE_ID_PTR) { /*Opa*/
        tr->start_value._opa = _lv_obj_get_style_opa(tr->obj, tr->part, prop_tmp);
    }
    else {      /*Ptr*/
        tr->start_value._ptr = _lv_obj_get_style_ptr(tr->obj, tr->part, prop_tmp);
    }

    /*Init prop to an invalid values to be sure `trans_del` won't delete this added `tr`*/
    tr->prop = 0;
    /*Delete the relate transition if any*/
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
        }
    }

    if(!running) {
        lv_style_list_t * list = _lv_obj_get_style_list(tr->obj, tr->part);
        lv_style_t * style_trans = _lv_style_list_get_transition_style(list);
        lv_style_remove_prop(style_trans, tr->prop);
    }

    _lv_ll_remove(&LV_GC_ROOT(_lv_obj_style_trans_ll), tr);
    lv_mem_free(tr);
}

static void style_snapshot(lv_obj_t * obj, uint8_t part, style_snapshot_t * shot)
{
    _lv_obj_disable_style_caching(obj, true);
    _lv_memset_00(shot, sizeof(style_snapshot_t));
    lv_draw_rect_dsc_init(&shot->rect);
    lv_draw_label_dsc_init(&shot->label);
    lv_draw_img_dsc_init(&shot->img);
    lv_draw_line_dsc_init(&shot->line);

    lv_obj_init_draw_rect_dsc(obj, part, &shot->rect);
    lv_obj_init_draw_label_dsc(obj, part, &shot->label);
    lv_obj_init_draw_img_dsc(obj, part, &shot->img);
    lv_obj_init_draw_line_dsc(obj, part, &shot->line);


    shot->pad_top = lv_obj_get_style_pad_top(obj, part);
    shot->pad_bottom = lv_obj_get_style_pad_bottom(obj, part);
    shot->pad_right = lv_obj_get_style_pad_right(obj, part);
    shot->pad_left = lv_obj_get_style_pad_left(obj, part);
    shot->margin_top = lv_obj_get_style_margin_top(obj, part);
    shot->margin_bottom = lv_obj_get_style_margin_bottom(obj, part);
    shot->margin_left = lv_obj_get_style_margin_left(obj, part);
    shot->margin_right = lv_obj_get_style_margin_right(obj, part);
    shot->size = lv_obj_get_style_size(obj, part);
    shot->transform_width = lv_obj_get_style_transform_width(obj, part);
    shot->transform_height = lv_obj_get_style_transform_height(obj, part);
    shot->transform_angle = lv_obj_get_style_transform_angle(obj, part);
    shot->transform_zoom = lv_obj_get_style_transform_zoom(obj, part);
    shot->scale_width = lv_obj_get_style_scale_width(obj, part);
    shot->scale_border_width = lv_obj_get_style_scale_border_width(obj, part);
    shot->scale_end_border_width = lv_obj_get_style_scale_end_border_width(obj, part);
    shot->scale_end_line_width = lv_obj_get_style_scale_end_line_width(obj, part);
    shot->scale_grad_color = lv_obj_get_style_scale_grad_color(obj, part);
    shot->scale_end_color = lv_obj_get_style_scale_end_color(obj, part);
    shot->opa_scale = lv_obj_get_style_opa_scale(obj, part);
    shot->clip_corder = lv_obj_get_style_clip_corner(obj, part);
    shot->border_post  = lv_obj_get_style_border_post(obj, part);

    _lv_obj_disable_style_caching(obj, false);
}

static _lv_style_state_cmp_t style_snapshot_compare(style_snapshot_t * shot1, style_snapshot_t * shot2)
{
    if(memcmp(shot1, shot2, sizeof(style_snapshot_t)) == 0) return _LV_STYLE_STATE_CMP_SAME;


    if(shot1->pad_top != shot2->pad_top) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->pad_bottom != shot2->pad_bottom) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->pad_left != shot2->pad_right) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->pad_right != shot2->pad_right) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->pad_top != shot2->pad_top) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->margin_top != shot2->margin_top) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->margin_bottom != shot2->margin_bottom) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->margin_left != shot2->margin_left) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->margin_right != shot2->margin_right) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->margin_top != shot2->margin_top) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->transform_width != shot2->transform_width) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->transform_height != shot2->transform_height) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->transform_angle != shot2->transform_angle) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->transform_zoom != shot2->transform_zoom) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->rect.outline_width != shot2->rect.outline_width) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->rect.outline_pad != shot2->rect.outline_pad) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->rect.outline_opa != shot2->rect.outline_opa) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->rect.value_font != shot2->rect.value_font) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->rect.value_align != shot2->rect.value_align) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->rect.value_font != shot2->rect.value_font) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->rect.shadow_spread != shot2->rect.shadow_spread) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->rect.shadow_width != shot2->rect.shadow_width) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->rect.shadow_ofs_x != shot2->rect.shadow_ofs_x) return _LV_STYLE_STATE_CMP_DIFF;
    if(shot1->rect.shadow_ofs_y != shot2->rect.shadow_ofs_y) return _LV_STYLE_STATE_CMP_DIFF;

    /*If not returned earlier its just a visual difference, a simple redraw is enough*/
    return _LV_STYLE_STATE_CMP_VISUAL_DIFF;
}
#endif

#if LV_STYLE_CACHE_LEVEL >= 1

static bool style_prop_is_cacheable(lv_style_property_t prop)
{
    switch(prop) {
        case LV_STYLE_PROP_ALL:
        case LV_STYLE_CLIP_CORNER:
        case LV_STYLE_TEXT_LETTER_SPACE:
        case LV_STYLE_TEXT_LINE_SPACE:
        case LV_STYLE_TEXT_FONT:
        case LV_STYLE_TEXT_DECOR:
        case LV_STYLE_TRANSFORM_ANGLE:
        case LV_STYLE_TRANSFORM_WIDTH:
        case LV_STYLE_TRANSFORM_HEIGHT:
        case LV_STYLE_TRANSFORM_ZOOM:
        case LV_STYLE_BORDER_WIDTH:
        case LV_STYLE_OUTLINE_WIDTH:
        case LV_STYLE_RADIUS:
        case LV_STYLE_SHADOW_WIDTH:
        case LV_STYLE_OPA_SCALE:
        case LV_STYLE_BG_OPA:
        case LV_STYLE_BORDER_SIDE:
        case LV_STYLE_BORDER_POST:
        case LV_STYLE_IMAGE_RECOLOR_OPA:
        case LV_STYLE_VALUE_STR:
        case LV_STYLE_PATTERN_IMAGE:
        case LV_STYLE_PAD_TOP:
        case LV_STYLE_PAD_BOTTOM:
        case LV_STYLE_PAD_LEFT:
        case LV_STYLE_PAD_RIGHT:
        case LV_STYLE_MARGIN_TOP:
        case LV_STYLE_MARGIN_BOTTOM:
        case LV_STYLE_MARGIN_LEFT:
        case LV_STYLE_MARGIN_RIGHT:
        case LV_STYLE_BG_BLEND_MODE:
        case LV_STYLE_BORDER_BLEND_MODE:
        case LV_STYLE_IMAGE_BLEND_MODE:
        case LV_STYLE_LINE_BLEND_MODE:
        case LV_STYLE_OUTLINE_BLEND_MODE:
        case LV_STYLE_PATTERN_BLEND_MODE:
        case LV_STYLE_SHADOW_BLEND_MODE:
        case LV_STYLE_TEXT_BLEND_MODE:
        case LV_STYLE_VALUE_BLEND_MODE:
            return true;
            break;
        default:
            return false;
    }
}

/**
 * Update the cache of style list
 * @param obj pointer to an obejct
 * @param part the part of the object
 * @param prop the property which triggered the update
 */
static void update_style_cache(lv_obj_t * obj, uint8_t part, uint16_t prop)
{
    if(style_prop_is_cacheable(prop) == false) return;

    lv_style_list_t * list = _lv_obj_get_style_list(obj, part);

    bool ignore_cache_ori = list->ignore_cache;
    list->ignore_cache = 1;

#if LV_USE_OPA_SCALE
    list->opa_scale_cover    = lv_obj_get_style_opa_scale(obj, part) == LV_OPA_COVER ? 1 : 0;
#else
    list->opa_scale_cover    = 1;
#endif
    list->text_decor_none    = lv_obj_get_style_text_decor(obj, part) == LV_TEXT_DECOR_NONE ? 1 : 0;
    list->text_font_normal    = lv_obj_get_style_text_font(obj, part) == LV_THEME_DEFAULT_FONT_NORMAL ? 1 : 0;

    list->text_space_zero = 1;
    if(lv_obj_get_style_text_letter_space(obj, part) != 0 ||
       lv_obj_get_style_text_line_space(obj, part) != 0) {
        list->text_space_zero = 0;
    }


    lv_opa_t bg_opa = lv_obj_get_style_bg_opa(obj, part);
    list->bg_opa_transp    = bg_opa == LV_OPA_TRANSP ? 1 : 0;
    list->bg_opa_cover     = bg_opa == LV_OPA_COVER ? 1 : 0;

    list->border_width_zero = lv_obj_get_style_border_width(obj, part) == 0 ? 1 : 0;
    list->border_side_full = lv_obj_get_style_border_side(obj, part) == LV_BORDER_SIDE_FULL ? 1 : 0;
    list->border_post_off = lv_obj_get_style_border_post(obj, part) == 0 ? 1 : 0;
    list->clip_corner_off   = lv_obj_get_style_clip_corner(obj, part) == false ? 1 : 0;
    list->img_recolor_opa_transp    = lv_obj_get_style_image_recolor_opa(obj, part) == LV_OPA_TRANSP ? 1 : 0;
    list->outline_width_zero    = lv_obj_get_style_outline_width(obj, part) == 0 ? 1 : 0;
    list->pattern_img_null    = lv_obj_get_style_pattern_image(obj, part) == NULL ? 1 : 0;
    list->radius_zero    = lv_obj_get_style_radius(obj, part) == 0 ? 1 : 0;
    list->shadow_width_zero    = lv_obj_get_style_shadow_width(obj, part) == 0 ? 1 : 0;
    list->value_txt_str    = lv_obj_get_style_value_str(obj, part) == NULL ? 1 : 0;


    list->transform_all_zero  = 1;
    if(lv_obj_get_style_transform_angle(obj, part) != 0 ||
       lv_obj_get_style_transform_width(obj, part) != 0 ||
       lv_obj_get_style_transform_height(obj, part) != 0 ||
       lv_obj_get_style_transform_zoom(obj, part) != LV_IMG_ZOOM_NONE) {
        list->transform_all_zero  = 0;
    }

    list->pad_all_zero  = 1;
    lv_style_int_t pad_top = lv_obj_get_style_pad_top(obj, part);
    lv_style_int_t pad_left = lv_obj_get_style_pad_left(obj, part);
#if LV_STYLE_CACHE_LEVEL >= 2
    list->pad_top = 0;
    list->pad_left = 0;
    if(pad_top > 0 && pad_top <= _LV_STLYE_CAHCE_INT_MAX) list->pad_top = pad_top;
    if(pad_left > 0 && pad_left <= _LV_STLYE_CAHCE_INT_MAX) list->pad_left = pad_left;
#endif
    if(pad_top != 0 ||
       pad_left != 0 ||
       lv_obj_get_style_pad_bottom(obj, part) != 0 ||
       lv_obj_get_style_pad_right(obj, part) != 0) {
        list->pad_all_zero  = 0;
    }

    list->margin_all_zero  = 1;
    if(lv_obj_get_style_margin_top(obj, part) != 0 ||
       lv_obj_get_style_margin_bottom(obj, part) != 0 ||
       lv_obj_get_style_margin_left(obj, part) != 0 ||
       lv_obj_get_style_margin_right(obj, part) != 0) {
        list->margin_all_zero  = 0;
    }

    list->blend_mode_all_normal = 1;
#if LV_USE_BLEND_MODES
    if(lv_obj_get_style_bg_blend_mode(obj, part) != LV_BLEND_MODE_NORMAL ||
       lv_obj_get_style_border_blend_mode(obj, part) != LV_BLEND_MODE_NORMAL ||
       lv_obj_get_style_pattern_blend_mode(obj, part) != LV_BLEND_MODE_NORMAL ||
       lv_obj_get_style_outline_blend_mode(obj, part) != LV_BLEND_MODE_NORMAL ||
       lv_obj_get_style_value_blend_mode(obj, part) != LV_BLEND_MODE_NORMAL ||
       lv_obj_get_style_text_blend_mode(obj, part) != LV_BLEND_MODE_NORMAL ||
       lv_obj_get_style_line_blend_mode(obj, part) != LV_BLEND_MODE_NORMAL ||
       lv_obj_get_style_image_blend_mode(obj, part) != LV_BLEND_MODE_NORMAL ||
       lv_obj_get_style_shadow_blend_mode(obj, part) != LV_BLEND_MODE_NORMAL) {
        list->blend_mode_all_normal = 0;
    }
#endif
    list->ignore_cache = ignore_cache_ori;
    list->valid_cache = 1;
}

/**
 * Update the cache of style list
 * @param obj pointer to an object
 * @param part the part of the object
 */
static void update_style_cache_children(lv_obj_t * obj)
{
    uint8_t part;
    for(part = 0; part < _LV_OBJ_PART_SPACIAL_START; part++) {
        lv_style_list_t * list = _lv_obj_get_style_list(obj, part);
        if(list == NULL) break;

        bool ignore_cache_ori = list->ignore_cache;
        list->ignore_cache = 1;

        list->opa_scale_cover    = lv_obj_get_style_opa_scale(obj, part) == LV_OPA_COVER ? 1 : 0;
        list->text_decor_none    = lv_obj_get_style_text_decor(obj, part) == LV_TEXT_DECOR_NONE ? 1 : 0;
        list->text_font_normal    = lv_obj_get_style_text_font(obj, part) == lv_theme_get_font_normal() ? 1 : 0;
        list->img_recolor_opa_transp    = lv_obj_get_style_image_recolor_opa(obj, part) == LV_OPA_TRANSP ? 1 : 0;

        list->text_space_zero = 1;
        if(lv_obj_get_style_text_letter_space(obj, part) != 0 ||
           lv_obj_get_style_text_line_space(obj, part) != 0) {
            list->text_space_zero = 0;
        }

        list->ignore_cache = ignore_cache_ori;
    }

    lv_obj_t * child = lv_obj_get_child(obj, NULL);
    while(child) {
        update_style_cache_children(child);
        child = lv_obj_get_child(obj, child);
    }

}

#endif /*LV_STYLE_CACHE_LEVEL >= 1*/


static void fade_anim_cb(lv_obj_t * obj, lv_anim_value_t v)
{
    lv_obj_set_style_local_opa_scale(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, v);
}

static void fade_in_anim_ready(lv_anim_t * a)
{
    lv_style_remove_prop(_lv_obj_get_local_style(a->var, LV_OBJ_PART_MAIN), LV_STYLE_OPA_SCALE);
}
