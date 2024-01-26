/**
 * @file lv_obj.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "../indev/lv_indev.h"
#include "../indev/lv_indev_private.h"
#include "lv_refr.h"
#include "lv_group.h"
#include "../display/lv_display.h"
#include "../display/lv_display_private.h"
#include "../themes/lv_theme.h"
#include "../misc/lv_assert.h"
#include "../misc/lv_math.h"
#include "../misc/lv_log.h"
#include "../tick/lv_tick.h"
#include "../stdlib/lv_string.h"
#include <stdint.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_obj_class)
#define LV_OBJ_DEF_WIDTH    (LV_DPX(100))
#define LV_OBJ_DEF_HEIGHT   (LV_DPX(50))
#define STYLE_TRANSITION_MAX 32

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_obj_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_obj_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_obj_draw(lv_event_t * e);
static void lv_obj_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_scrollbar(lv_obj_t * obj, lv_layer_t * layer);
static lv_result_t scrollbar_init_draw_dsc(lv_obj_t * obj, lv_draw_rect_dsc_t * dsc);
static bool obj_valid_child(const lv_obj_t * parent, const lv_obj_t * obj_to_find);
static void update_obj_state(lv_obj_t * obj, lv_state_t new_state);
#if LV_USE_OBJ_PROPERTY
    static lv_result_t lv_obj_set_any(lv_obj_t *, lv_prop_id_t, const lv_property_t *);
    static lv_result_t lv_obj_get_any(const lv_obj_t *, lv_prop_id_t, lv_property_t *);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_OBJ_PROPERTY
static const lv_property_ops_t properties[] = {
    {
        .id = LV_PROPERTY_OBJ_PARENT,
        .setter = lv_obj_set_parent,
        .getter = lv_obj_get_parent,
    },
    {
        .id = LV_PROPERTY_ID_ANY,
        .setter = lv_obj_set_any,
        .getter = lv_obj_get_any,
    }
};
#endif

const lv_obj_class_t lv_obj_class = {
    .constructor_cb = lv_obj_constructor,
    .destructor_cb = lv_obj_destructor,
    .event_cb = lv_obj_event,
    .width_def = LV_DPI_DEF,
    .height_def = LV_DPI_DEF,
    .editable = LV_OBJ_CLASS_EDITABLE_FALSE,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_FALSE,
    .instance_size = (sizeof(lv_obj_t)),
    .base_class = NULL,
    .name = "obj",
#if LV_USE_OBJ_PROPERTY
    .prop_index_start = LV_PROPERTY_OBJ_START,
    .prop_index_end = LV_PROPERTY_OBJ_END,
    .properties = properties,
    .properties_count = sizeof(properties) / sizeof(properties[0]),
#endif
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_obj_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

/*-----------------
 * Attribute set
 *----------------*/

void lv_obj_add_flag(lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    bool was_on_layout = lv_obj_is_layout_positioned(obj);

    /* We must invalidate the area occupied by the object before we hide it as calls to invalidate hidden objects are ignored */
    if(f & LV_OBJ_FLAG_HIDDEN) lv_obj_invalidate(obj);

    obj->flags |= f;

    if(f & LV_OBJ_FLAG_HIDDEN) {
        if(lv_obj_has_state(obj, LV_STATE_FOCUSED)) {
            lv_group_t * group = lv_obj_get_group(obj);
            if(group != NULL) {
                lv_group_focus_next(group);
                lv_obj_t * next_obj = lv_group_get_focused(group);
                if(next_obj != NULL) {
                    lv_obj_invalidate(next_obj);
                }
            }
        }
    }

    if((was_on_layout != lv_obj_is_layout_positioned(obj)) || (f & (LV_OBJ_FLAG_LAYOUT_1 |  LV_OBJ_FLAG_LAYOUT_2))) {
        lv_obj_mark_layout_as_dirty(lv_obj_get_parent(obj));
        lv_obj_mark_layout_as_dirty(obj);
    }

    if(f & LV_OBJ_FLAG_SCROLLABLE) {
        lv_area_t hor_area, ver_area;
        lv_obj_get_scrollbar_area(obj, &hor_area, &ver_area);
        lv_obj_invalidate_area(obj, &hor_area);
        lv_obj_invalidate_area(obj, &ver_area);
    }
}

void lv_obj_remove_flag(lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    bool was_on_layout = lv_obj_is_layout_positioned(obj);
    if(f & LV_OBJ_FLAG_SCROLLABLE) {
        lv_area_t hor_area, ver_area;
        lv_obj_get_scrollbar_area(obj, &hor_area, &ver_area);
        lv_obj_invalidate_area(obj, &hor_area);
        lv_obj_invalidate_area(obj, &ver_area);
    }

    obj->flags &= (~f);

    if(f & LV_OBJ_FLAG_HIDDEN) {
        lv_obj_invalidate(obj);
        if(lv_obj_is_layout_positioned(obj)) {
            lv_obj_mark_layout_as_dirty(lv_obj_get_parent(obj));
            lv_obj_mark_layout_as_dirty(obj);
        }
    }

    if((was_on_layout != lv_obj_is_layout_positioned(obj)) || (f & (LV_OBJ_FLAG_LAYOUT_1 |  LV_OBJ_FLAG_LAYOUT_2))) {
        lv_obj_mark_layout_as_dirty(lv_obj_get_parent(obj));
    }

}

void lv_obj_update_flag(lv_obj_t * obj, lv_obj_flag_t f, bool v)
{
    if(v) lv_obj_add_flag(obj, f);
    else lv_obj_remove_flag(obj, f);
}

void lv_obj_add_state(lv_obj_t * obj, lv_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_state_t new_state = obj->state | state;
    if(obj->state != new_state) {

        if(new_state & LV_STATE_DISABLED) {
            lv_indev_reset(NULL, obj);
        }

        update_obj_state(obj, new_state);
    }
}

void lv_obj_remove_state(lv_obj_t * obj, lv_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_state_t new_state = obj->state & (~state);
    if(obj->state != new_state) {
        update_obj_state(obj, new_state);
    }
}

void lv_obj_set_state(lv_obj_t * obj, lv_state_t state, bool v)
{
    if(v) lv_obj_add_state(obj, state);
    else lv_obj_remove_state(obj, state);
}

/*=======================
 * Getter functions
 *======================*/

bool lv_obj_has_flag(const lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return (obj->flags & f)  == f;
}

bool lv_obj_has_flag_any(const lv_obj_t * obj, lv_obj_flag_t f)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return !!(obj->flags & f);
}

lv_state_t lv_obj_get_state(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return obj->state;
}

bool lv_obj_has_state(const lv_obj_t * obj, lv_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return !!(obj->state & state);
}

lv_group_t * lv_obj_get_group(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(obj->spec_attr) return obj->spec_attr->group_p;
    else return NULL;
}

/*-------------------
 * OTHER FUNCTIONS
 *------------------*/

void lv_obj_allocate_spec_attr(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(obj->spec_attr == NULL) {
        obj->spec_attr = lv_malloc_zeroed(sizeof(_lv_obj_spec_attr_t));
        LV_ASSERT_MALLOC(obj->spec_attr);
        if(obj->spec_attr == NULL) return;

        obj->spec_attr->scroll_dir = LV_DIR_ALL;
        obj->spec_attr->scrollbar_mode = LV_SCROLLBAR_MODE_AUTO;
    }
}

bool lv_obj_check_type(const lv_obj_t * obj, const lv_obj_class_t * class_p)
{
    if(obj == NULL) return false;
    return obj->class_p == class_p;
}

bool lv_obj_has_class(const lv_obj_t * obj, const lv_obj_class_t * class_p)
{
    const lv_obj_class_t * obj_class = obj->class_p;
    while(obj_class) {
        if(obj_class == class_p) return true;
        obj_class = obj_class->base_class;
    }

    return false;
}

const lv_obj_class_t * lv_obj_get_class(const lv_obj_t * obj)
{
    return obj->class_p;
}

bool lv_obj_is_valid(const lv_obj_t * obj)
{
    lv_display_t * disp = lv_display_get_next(NULL);
    while(disp) {
        uint32_t i;
        for(i = 0; i < disp->screen_cnt; i++) {
            if(disp->screens[i] == obj) return true;
            bool found = obj_valid_child(disp->screens[i], obj);
            if(found) return true;
        }

        disp = lv_display_get_next(disp);
    }

    return false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_obj_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_t * parent = obj->parent;
    if(parent) {
        int32_t sl = lv_obj_get_scroll_left(parent);
        int32_t st = lv_obj_get_scroll_top(parent);

        obj->coords.y1 = parent->coords.y1 + lv_obj_get_style_pad_top(parent, LV_PART_MAIN) - st;
        obj->coords.y2 = obj->coords.y1 - 1;
        obj->coords.x1  = parent->coords.x1 + lv_obj_get_style_pad_left(parent, LV_PART_MAIN) - sl;
        obj->coords.x2  = obj->coords.x1 - 1;
    }

    /*Set attributes*/
    obj->flags = LV_OBJ_FLAG_CLICKABLE;
    obj->flags |= LV_OBJ_FLAG_SNAPPABLE;
    if(parent) obj->flags |= LV_OBJ_FLAG_PRESS_LOCK;
    if(parent) obj->flags |= LV_OBJ_FLAG_SCROLL_CHAIN;
    obj->flags |= LV_OBJ_FLAG_CLICK_FOCUSABLE;
    obj->flags |= LV_OBJ_FLAG_SCROLLABLE;
    obj->flags |= LV_OBJ_FLAG_SCROLL_ELASTIC;
    obj->flags |= LV_OBJ_FLAG_SCROLL_MOMENTUM;
    obj->flags |= LV_OBJ_FLAG_SCROLL_WITH_ARROW;
    if(parent) obj->flags |= LV_OBJ_FLAG_GESTURE_BUBBLE;

#if LV_USE_OBJ_ID
    lv_obj_assign_id(class_p, obj);
#endif

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_obj_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    _lv_event_mark_deleted(obj);

    /*Remove all style*/
    lv_obj_enable_style_refresh(false); /*No need to refresh the style because the object will be deleted*/
    lv_obj_remove_style_all(obj);
    lv_obj_enable_style_refresh(true);

    /*Remove the animations from this object*/
    lv_anim_delete(obj, NULL);

    /*Delete from the group*/
    lv_group_t * group = lv_obj_get_group(obj);
    if(group) lv_group_remove_obj(obj);

    if(obj->spec_attr) {
        if(obj->spec_attr->children) {
            lv_free(obj->spec_attr->children);
            obj->spec_attr->children = NULL;
        }

        lv_event_remove_all(&obj->spec_attr->event_list);

        lv_free(obj->spec_attr);
        obj->spec_attr = NULL;
    }

#if LV_USE_OBJ_ID
    lv_obj_free_id(obj);
#endif
}

static void lv_obj_draw(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    if(code == LV_EVENT_COVER_CHECK) {
        lv_cover_check_info_t * info = lv_event_get_param(e);
        if(info->res == LV_COVER_RES_MASKED) return;
        if(lv_obj_get_style_clip_corner(obj, LV_PART_MAIN)) {
            info->res = LV_COVER_RES_MASKED;
            return;
        }

        /*Most trivial test. Is the mask fully IN the object? If no it surely doesn't cover it*/
        int32_t r = lv_obj_get_style_radius(obj, LV_PART_MAIN);
        int32_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
        int32_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        lv_area_increase(&coords, w, h);

        if(_lv_area_is_in(info->area, &coords, r) == false) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        if(lv_obj_get_style_bg_opa(obj, LV_PART_MAIN) < LV_OPA_MAX) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        if(lv_obj_get_style_opa(obj, LV_PART_MAIN) < LV_OPA_MAX) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        if(lv_obj_get_style_bg_grad_dir(obj, 0) != LV_GRAD_DIR_NONE) {
            if(lv_obj_get_style_bg_grad_opa(obj, 0) < LV_OPA_MAX) {
                info->res = LV_COVER_RES_NOT_COVER;
                return;
            }
            const lv_grad_dsc_t * grad_dsc = lv_obj_get_style_bg_grad(obj, 0);
            if(grad_dsc) {
                uint32_t i;
                for(i = 0; i < grad_dsc->stops_count; i++) {
                    if(grad_dsc->stops[i].opa < LV_OPA_MAX) {
                        info->res = LV_COVER_RES_NOT_COVER;
                        return;
                    }
                }
            }
        }
        info->res = LV_COVER_RES_COVER;

    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        lv_layer_t * layer = lv_event_get_layer(e);
        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);

        lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &draw_dsc);
        /*If the border is drawn later disable loading its properties*/
        if(lv_obj_get_style_border_post(obj, LV_PART_MAIN)) {
            draw_dsc.border_post = 1;
        }

        int32_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
        int32_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        lv_area_increase(&coords, w, h);

        lv_draw_rect(layer, &draw_dsc, &coords);
    }
    else if(code == LV_EVENT_DRAW_POST) {
        lv_layer_t * layer = lv_event_get_layer(e);
        draw_scrollbar(obj, layer);

        /*If the border is drawn later disable loading other properties*/
        if(lv_obj_get_style_border_post(obj, LV_PART_MAIN)) {
            lv_draw_rect_dsc_t draw_dsc;
            lv_draw_rect_dsc_init(&draw_dsc);
            draw_dsc.bg_opa = LV_OPA_TRANSP;
            draw_dsc.bg_image_opa = LV_OPA_TRANSP;
            draw_dsc.outline_opa = LV_OPA_TRANSP;
            draw_dsc.shadow_opa = LV_OPA_TRANSP;
            lv_obj_init_draw_rect_dsc(obj, LV_PART_MAIN, &draw_dsc);

            int32_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
            int32_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
            lv_area_t coords;
            lv_area_copy(&coords, &obj->coords);
            lv_area_increase(&coords, w, h);

            lv_draw_rect(layer, &draw_dsc, &coords);
        }
    }
}

static void draw_scrollbar(lv_obj_t * obj, lv_layer_t * layer)
{

    lv_area_t hor_area;
    lv_area_t ver_area;
    lv_obj_get_scrollbar_area(obj, &hor_area, &ver_area);

    if(lv_area_get_size(&hor_area) <= 0 && lv_area_get_size(&ver_area) <= 0) return;

    lv_draw_rect_dsc_t draw_dsc;
    lv_result_t sb_res = scrollbar_init_draw_dsc(obj, &draw_dsc);
    if(sb_res != LV_RESULT_OK) return;

    if(lv_area_get_size(&hor_area) > 0) {
        draw_dsc.base.id1 = 0;
        lv_draw_rect(layer, &draw_dsc, &hor_area);
    }
    if(lv_area_get_size(&ver_area) > 0) {
        draw_dsc.base.id1 = 1;
        lv_draw_rect(layer, &draw_dsc, &ver_area);
    }
}

/**
 * Initialize the draw descriptor for the scrollbar
 * @param obj pointer to an object
 * @param dsc the draw descriptor to initialize
 * @return LV_RESULT_OK: the scrollbar is visible; LV_RESULT_INVALID: the scrollbar is not visible
 */
static lv_result_t scrollbar_init_draw_dsc(lv_obj_t * obj, lv_draw_rect_dsc_t * dsc)
{
    lv_draw_rect_dsc_init(dsc);
    dsc->bg_opa = lv_obj_get_style_bg_opa(obj, LV_PART_SCROLLBAR);
    if(dsc->bg_opa > LV_OPA_MIN) {
        dsc->bg_color = lv_obj_get_style_bg_color(obj, LV_PART_SCROLLBAR);
    }

    dsc->border_opa = lv_obj_get_style_border_opa(obj, LV_PART_SCROLLBAR);
    if(dsc->border_opa > LV_OPA_MIN) {
        dsc->border_width = lv_obj_get_style_border_width(obj, LV_PART_SCROLLBAR);
        if(dsc->border_width > 0) {
            dsc->border_color = lv_obj_get_style_border_color(obj, LV_PART_SCROLLBAR);
        }
        else {
            dsc->border_opa = LV_OPA_TRANSP;
        }
    }

    dsc->shadow_opa = lv_obj_get_style_shadow_opa(obj, LV_PART_SCROLLBAR);
    if(dsc->shadow_opa > LV_OPA_MIN) {
        dsc->shadow_width = lv_obj_get_style_shadow_width(obj, LV_PART_SCROLLBAR);
        if(dsc->shadow_width > 0) {
            dsc->shadow_spread = lv_obj_get_style_shadow_spread(obj, LV_PART_SCROLLBAR);
            dsc->shadow_color = lv_obj_get_style_shadow_color(obj, LV_PART_SCROLLBAR);
        }
        else {
            dsc->shadow_opa = LV_OPA_TRANSP;
        }
    }

    lv_opa_t opa = lv_obj_get_style_opa_recursive(obj, LV_PART_SCROLLBAR);
    if(opa < LV_OPA_MAX) {
        lv_opa_t v = LV_OPA_MIX2(dsc->bg_opa, opa);
        dsc->bg_opa = v;
        dsc->border_opa = v;
        dsc->shadow_opa = v;
    }

    if(dsc->bg_opa != LV_OPA_TRANSP || dsc->border_opa != LV_OPA_TRANSP || dsc->shadow_opa != LV_OPA_TRANSP) {
        dsc->radius = lv_obj_get_style_radius(obj, LV_PART_SCROLLBAR);
        return LV_RESULT_OK;
    }
    else {
        return LV_RESULT_INVALID;
    }
}

static void lv_obj_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    if(code == LV_EVENT_PRESSED) {
        lv_obj_add_state(obj, LV_STATE_PRESSED);
    }
    else if(code == LV_EVENT_RELEASED) {
        lv_obj_remove_state(obj, LV_STATE_PRESSED);
        void * param = lv_event_get_param(e);
        /*Go the checked state if enabled*/
        if(lv_indev_get_scroll_obj(param) == NULL && lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE)) {
            if(!(lv_obj_get_state(obj) & LV_STATE_CHECKED)) lv_obj_add_state(obj, LV_STATE_CHECKED);
            else lv_obj_remove_state(obj, LV_STATE_CHECKED);

            lv_result_t res = lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RESULT_OK) return;
        }
    }
    else if(code == LV_EVENT_PRESS_LOST) {
        lv_obj_remove_state(obj, LV_STATE_PRESSED);
    }
    else if(code == LV_EVENT_STYLE_CHANGED) {
        uint32_t child_cnt = lv_obj_get_child_count(obj);
        for(uint32_t i = 0; i < child_cnt; i++) {
            lv_obj_t * child = obj->spec_attr->children[i];
            lv_obj_mark_layout_as_dirty(child);
        }
    }
    else if(code == LV_EVENT_KEY) {
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_CHECKABLE)) {
            uint32_t c = lv_event_get_key(e);
            if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
                lv_obj_add_state(obj, LV_STATE_CHECKED);
            }
            else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
                lv_obj_remove_state(obj, LV_STATE_CHECKED);
            }

            /*With Enter LV_EVENT_RELEASED will send VALUE_CHANGE event*/
            if(c != LV_KEY_ENTER) {
                lv_result_t res = lv_obj_send_event(obj, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RESULT_OK) return;
            }
        }
        else if(lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_WITH_ARROW) && !lv_obj_is_editable(obj)) {
            /*scroll by keypad or encoder*/
            lv_anim_enable_t anim_enable = LV_ANIM_OFF;
            int32_t sl = lv_obj_get_scroll_left(obj);
            int32_t sr = lv_obj_get_scroll_right(obj);
            uint32_t c = lv_event_get_key(e);
            if(c == LV_KEY_DOWN) {
                /*use scroll_to_x/y functions to enforce scroll limits*/
                lv_obj_scroll_to_y(obj, lv_obj_get_scroll_y(obj) + lv_obj_get_height(obj) / 4, anim_enable);
            }
            else if(c == LV_KEY_UP) {
                lv_obj_scroll_to_y(obj, lv_obj_get_scroll_y(obj) - lv_obj_get_height(obj) / 4, anim_enable);
            }
            else if(c == LV_KEY_RIGHT) {
                /*If the object can't be scrolled horizontally then scroll it vertically*/
                if(!((lv_obj_get_scroll_dir(obj) & LV_DIR_HOR) && (sl > 0 || sr > 0)))
                    lv_obj_scroll_to_y(obj, lv_obj_get_scroll_y(obj) + lv_obj_get_height(obj) / 4, anim_enable);
                else
                    lv_obj_scroll_to_x(obj, lv_obj_get_scroll_x(obj) + lv_obj_get_width(obj) / 4, anim_enable);
            }
            else if(c == LV_KEY_LEFT) {
                /*If the object can't be scrolled horizontally then scroll it vertically*/
                if(!((lv_obj_get_scroll_dir(obj) & LV_DIR_HOR) && (sl > 0 || sr > 0)))
                    lv_obj_scroll_to_y(obj, lv_obj_get_scroll_y(obj) - lv_obj_get_height(obj) / 4, anim_enable);
                else
                    lv_obj_scroll_to_x(obj, lv_obj_get_scroll_x(obj) - lv_obj_get_width(obj) / 4, anim_enable);
            }
        }
    }
    else if(code == LV_EVENT_FOCUSED) {
        if(lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS)) {
            lv_obj_scroll_to_view_recursive(obj, LV_ANIM_ON);
        }

        bool editing = false;
        editing = lv_group_get_editing(lv_obj_get_group(obj));
        lv_state_t state = LV_STATE_FOCUSED;

        /* Use the indev for then indev handler.
         * But if the obj was focused manually it returns NULL so try to
         * use the indev from the event*/
        lv_indev_t * indev = lv_indev_active();
        if(indev == NULL) indev = lv_event_get_indev(e);

        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev_type == LV_INDEV_TYPE_KEYPAD || indev_type == LV_INDEV_TYPE_ENCODER) state |= LV_STATE_FOCUS_KEY;
        if(editing) {
            state |= LV_STATE_EDITED;
            lv_obj_add_state(obj, state);
        }
        else {
            lv_obj_add_state(obj, state);
            lv_obj_remove_state(obj, LV_STATE_EDITED);
        }
    }
    else if(code == LV_EVENT_SCROLL_BEGIN) {
        lv_obj_add_state(obj, LV_STATE_SCROLLED);
    }
    else if(code == LV_EVENT_SCROLL_END) {
        lv_obj_remove_state(obj, LV_STATE_SCROLLED);
        if(lv_obj_get_scrollbar_mode(obj) == LV_SCROLLBAR_MODE_ACTIVE) {
            lv_area_t hor_area, ver_area;
            lv_obj_get_scrollbar_area(obj, &hor_area, &ver_area);
            lv_obj_invalidate_area(obj, &hor_area);
            lv_obj_invalidate_area(obj, &ver_area);
        }
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        lv_obj_remove_state(obj, LV_STATE_FOCUSED | LV_STATE_EDITED | LV_STATE_FOCUS_KEY);
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        int32_t align = lv_obj_get_style_align(obj, LV_PART_MAIN);
        uint16_t layout = lv_obj_get_style_layout(obj, LV_PART_MAIN);
        if(layout || align) {
            lv_obj_mark_layout_as_dirty(obj);
        }

        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_count(obj);
        for(i = 0; i < child_cnt; i++) {
            lv_obj_t * child = obj->spec_attr->children[i];
            lv_obj_mark_layout_as_dirty(child);
        }
    }
    else if(code == LV_EVENT_CHILD_CHANGED) {
        int32_t w = lv_obj_get_style_width(obj, LV_PART_MAIN);
        int32_t h = lv_obj_get_style_height(obj, LV_PART_MAIN);
        int32_t align = lv_obj_get_style_align(obj, LV_PART_MAIN);
        uint16_t layout = lv_obj_get_style_layout(obj, LV_PART_MAIN);
        if(layout || align || w == LV_SIZE_CONTENT || h == LV_SIZE_CONTENT) {
            lv_obj_mark_layout_as_dirty(obj);
        }
    }
    else if(code == LV_EVENT_CHILD_DELETED) {
        obj->readjust_scroll_after_layout = 1;
        lv_obj_mark_layout_as_dirty(obj);
    }
    else if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        int32_t d = lv_obj_calculate_ext_draw_size(obj, LV_PART_MAIN);
        lv_event_set_ext_draw_size(e, d);
    }
    else if(code == LV_EVENT_DRAW_MAIN || code == LV_EVENT_DRAW_POST || code == LV_EVENT_COVER_CHECK) {
        lv_obj_draw(e);
    }
    else if(code == LV_EVENT_INDEV_RESET) {
        lv_obj_remove_state(obj, LV_STATE_PRESSED);
        lv_obj_remove_state(obj, LV_STATE_SCROLLED);
    }
}

/**
 * Set the state (fully overwrite) of an object.
 * If specified in the styles, transition animations will be started from the previous state to the current.
 * @param obj       pointer to an object
 * @param state     the new state
 */
static void update_obj_state(lv_obj_t * obj, lv_state_t new_state)
{
    if(obj->state == new_state) return;

    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_state_t prev_state = obj->state;

    _lv_style_state_cmp_t cmp_res = _lv_obj_style_state_compare(obj, prev_state, new_state);
    /*If there is no difference in styles there is nothing else to do*/
    if(cmp_res == _LV_STYLE_STATE_CMP_SAME) {
        obj->state = new_state;
        return;
    }

    /*Invalidate the object in their current state*/
    lv_obj_invalidate(obj);

    obj->state = new_state;

    _lv_obj_style_transition_dsc_t * ts = lv_malloc_zeroed(sizeof(_lv_obj_style_transition_dsc_t) * STYLE_TRANSITION_MAX);
    uint32_t tsi = 0;
    uint32_t i;
    for(i = 0; i < obj->style_cnt && tsi < STYLE_TRANSITION_MAX; i++) {
        _lv_obj_style_t * obj_style = &obj->styles[i];
        lv_state_t state_act = lv_obj_style_get_selector_state(obj->styles[i].selector);
        lv_part_t part_act = lv_obj_style_get_selector_part(obj->styles[i].selector);
        if(state_act & (~new_state)) continue; /*Skip unrelated styles*/
        if(obj_style->is_trans) continue;

        lv_style_value_t v;
        if(lv_style_get_prop_inlined(obj_style->style, LV_STYLE_TRANSITION, &v) != LV_STYLE_RES_FOUND) continue;
        const lv_style_transition_dsc_t * tr = v.ptr;

        /*Add the props to the set if not added yet or added but with smaller weight*/
        uint32_t j;
        for(j = 0; tr->props[j] != 0 && tsi < STYLE_TRANSITION_MAX; j++) {
            uint32_t t;
            for(t = 0; t < tsi; t++) {
                lv_style_selector_t selector = ts[t].selector;
                lv_state_t state_ts = lv_obj_style_get_selector_state(selector);
                lv_part_t part_ts = lv_obj_style_get_selector_part(selector);
                if(ts[t].prop == tr->props[j] && part_ts == part_act && state_ts >= state_act) break;
            }

            /*If not found  add it*/
            if(t == tsi) {
                ts[tsi].time = tr->time;
                ts[tsi].delay = tr->delay;
                ts[tsi].path_cb = tr->path_xcb;
                ts[tsi].prop = tr->props[j];
                ts[tsi].user_data = tr->user_data;
                ts[tsi].selector = obj_style->selector;
                tsi++;
            }
        }
    }

    for(i = 0; i < tsi; i++) {
        lv_part_t part_act = lv_obj_style_get_selector_part(ts[i].selector);
        _lv_obj_style_create_transition(obj, part_act, prev_state, new_state, &ts[i]);
    }

    lv_free(ts);

    if(cmp_res == _LV_STYLE_STATE_CMP_DIFF_REDRAW) {
        /*Invalidation is not enough, e.g. layer type needs to be updated too*/
        lv_obj_refresh_style(obj, LV_PART_ANY, LV_STYLE_PROP_ANY);
    }
    else if(cmp_res == _LV_STYLE_STATE_CMP_DIFF_LAYOUT) {
        lv_obj_refresh_style(obj, LV_PART_ANY, LV_STYLE_PROP_ANY);
    }
    else if(cmp_res == _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD) {
        lv_obj_invalidate(obj);
        lv_obj_refresh_ext_draw_size(obj);
    }
}

static bool obj_valid_child(const lv_obj_t * parent, const lv_obj_t * obj_to_find)
{
    /*Check all children of `parent`*/
    uint32_t child_cnt = 0;
    if(parent->spec_attr) child_cnt = parent->spec_attr->child_cnt;
    uint32_t i;
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = parent->spec_attr->children[i];
        if(child == obj_to_find) {
            return true;
        }

        /*Check the children*/
        bool found = obj_valid_child(child, obj_to_find);
        if(found) {
            return true;
        }
    }
    return false;
}

#if LV_USE_OBJ_PROPERTY
static lv_result_t lv_obj_set_any(lv_obj_t * obj, lv_prop_id_t id, const lv_property_t * prop)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(id >= LV_PROPERTY_OBJ_FLAG_START && id <= LV_PROPERTY_OBJ_FLAG_END) {
        lv_obj_flag_t flag = 1L << (id - LV_PROPERTY_OBJ_FLAG_START);
        if(prop->num) lv_obj_add_flag(obj, flag);
        else lv_obj_remove_flag(obj, flag);
        return LV_RESULT_OK;
    }
    else if(id >= LV_PROPERTY_OBJ_STATE_START && id <= LV_PROPERTY_OBJ_STATE_END) {
        lv_state_t state = 1L << (id - LV_PROPERTY_OBJ_STATE_START);
        if(id == LV_PROPERTY_OBJ_STATE_ANY) {
            state = LV_STATE_ANY;
        }

        if(prop->num) lv_obj_add_state(obj, state);
        else lv_obj_remove_state(obj, state);
        return LV_RESULT_OK;
    }
    else {
        return LV_RESULT_INVALID;
    }
}

static lv_result_t lv_obj_get_any(const lv_obj_t * obj, lv_prop_id_t id, lv_property_t * prop)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(id >= LV_PROPERTY_OBJ_FLAG_START && id <= LV_PROPERTY_OBJ_FLAG_END) {
        lv_obj_flag_t flag = 1L << (id - LV_PROPERTY_OBJ_FLAG_START);
        prop->id = id;
        prop->num = obj->flags & flag;
        return LV_RESULT_OK;
    }
    else if(id >= LV_PROPERTY_OBJ_STATE_START && id <= LV_PROPERTY_OBJ_STATE_END) {
        prop->id = id;
        if(id == LV_PROPERTY_OBJ_STATE_ANY) {
            prop->num = obj->state;
        }
        else {
            lv_obj_flag_t flag = 1L << (id - LV_PROPERTY_OBJ_STATE_START);
            prop->num = obj->state & flag;
        }
        return LV_RESULT_OK;
    }
    else {
        return LV_RESULT_INVALID;
    }
}
#endif
