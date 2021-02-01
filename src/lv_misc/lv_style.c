/**
 * @file lv_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_style.h"
#include "../lv_misc/lv_mem.h"
#include "../lv_misc/lv_anim.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_style_init(lv_style_t * style)
{
    lv_memset_00(style, sizeof(lv_style_t));
#if LV_USE_ASSERT_STYLE
    style->sentinel = LV_DEBUG_STYLE_SENTINEL_VALUE;
#endif

}

void lv_style_reset(lv_style_t * style)
{
    LV_ASSERT_STYLE(style);

    if(style->allocated) lv_mem_free(style->props_and_values);
    lv_style_init(style);
}

lv_style_prop_t lv_style_register_prop(void)
{
    static uint16_t act_id = (uint16_t)_LV_STYLE_LAST_BUILT_IN_PROP;
    act_id++;
    return act_id;
}

bool lv_style_remove_prop(lv_style_t * style, lv_style_prop_t prop)
{
    if(style == NULL) return false;
    LV_ASSERT_STYLE(style);

    if(!style->allocated) {
        if(style->prop1 == prop) {
            style->prop1 = LV_STYLE_PROP_INV;
            style->prop_cnt = 0;
            return true;
        }
        return false;
    }

    uint8_t * tmp = style->props_and_values + style->prop_cnt * sizeof(lv_style_value_t);
    uint16_t * props = (uint16_t *) tmp;
    uint32_t i;
    for(i = 0; i < style->prop_cnt; i++) {
        if(props[i] == prop) {
            lv_style_value_t * values = (lv_style_value_t *)style->props_and_values;
            style->prop_cnt--;
            size_t size = style->prop_cnt * (sizeof(lv_style_value_t) + sizeof(uint16_t));
            uint8_t * new_values_and_props = lv_mem_alloc(size);

            uint8_t * tmp = new_values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
            uint16_t * new_props = (uint16_t *) tmp;
            lv_style_value_t * new_values = (lv_style_value_t *)new_values_and_props;

            uint32_t j;
            for(j = 0; j < style->prop_cnt + 1; j++) { /* +1: because prop_cnt already reduced but all the old props. needs to be checked. */
                if(props[j] != prop) {
                    *new_values = values[j];
                    *new_props = props[j];

                    new_values++;
                    new_props++;
                }
            }

            lv_mem_free(style->props_and_values);
            style->props_and_values = new_values_and_props;
            return true;
        }
    }

    return false;
}

void lv_style_set_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t value)
{
    if(style->allocated) {
        uint8_t * tmp = style->props_and_values + style->prop_cnt * sizeof(lv_style_value_t);
        uint16_t * props = (uint16_t *) tmp;
        int32_t i;
        for(i = 0; i < style->prop_cnt; i++) {
            if(props[i] == prop) {
                lv_style_value_t * values = (lv_style_value_t *)style->props_and_values;
                values[i] = value;
                return;
            }
        }

        style->prop_cnt++;
        size_t size = style->prop_cnt * (sizeof(lv_style_value_t) + sizeof(uint16_t));
        style->props_and_values = lv_mem_realloc(style->props_and_values, size);
        tmp = style->props_and_values + (style->prop_cnt - 1) * sizeof(lv_style_value_t);
        props = (uint16_t *) tmp;
        /*Shift all props to make place for the value before them*/
        for(i = style->prop_cnt - 2; i >= 0; i--) {
            props[i + sizeof(lv_style_value_t) /sizeof(uint16_t)] = props[i];
        }

        /*Go to the new position wit the props*/
        tmp = style->props_and_values + (style->prop_cnt) * sizeof(lv_style_value_t);
        props = (uint16_t *) tmp;
        lv_style_value_t * values = (lv_style_value_t *)style->props_and_values;

        /*Set the new property and value*/
        props[style->prop_cnt - 1] = prop;
        values[style->prop_cnt - 1] = value;
    } else if(style->prop_cnt == 1) {
        if(style->prop1 == prop) {
            style->value1 = value;
            return;
        }
        style->prop_cnt++;
        size_t size = style->prop_cnt * (sizeof(lv_style_value_t) + sizeof(uint16_t));
        uint8_t * props_and_values = lv_mem_alloc(size);
        uint8_t * tmp = props_and_values + (style->prop_cnt) * sizeof(lv_style_value_t);
        uint16_t * props = (uint16_t *) tmp;
        lv_style_value_t * values = (lv_style_value_t *)props_and_values;
        props[0] = style->prop1;
        props[1] = prop;
        values[0] = style->value1;
        values[1] = value;

        style->props_and_values = props_and_values;

        style->allocated = 1;
    } else if (style->prop_cnt == 0) {
        style->prop_cnt++;
        style->prop1 = prop;
        style->value1 = value;
    }
}

bool lv_style_get_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value)
{
    if(style->allocated) {
        uint8_t * tmp = style->props_and_values + style->prop_cnt * sizeof(lv_style_value_t);
        uint16_t * props = (uint16_t *) tmp;
        uint32_t i;
        for(i = 0; i < style->prop_cnt; i++) {
            if(props[i] == prop) {
                lv_style_value_t * values = (lv_style_value_t *)style->props_and_values;
                *value = values[i];
                return true;
            }
        }
    } else {
        if(style->prop1 == prop) {
            *value = style->value1;
            return true;
        }
    }
    return false;
}

void lv_style_transition_dsc_init(lv_style_transition_dsc_t * tr, const lv_style_prop_t * props, const lv_anim_path_t * path, uint32_t time, uint32_t delay)
{
    lv_memset_00(tr, sizeof(lv_style_transition_dsc_t));
    tr->props = props;
    tr->path = path == NULL ? &lv_anim_path_def : path;
    tr->time = time;
    tr->delay = delay;
}

lv_style_value_t lv_style_prop_get_default(lv_style_prop_t prop)
{
    lv_style_value_t value;
    switch(prop) {
            break;
        case LV_STYLE_TRANSFORM_ZOOM:
            value.num = LV_IMG_ZOOM_NONE;
            break;
        case LV_STYLE_BG_COLOR:
            value.color = LV_COLOR_WHITE;
            break;
        case LV_STYLE_OPA:
        case LV_STYLE_BORDER_OPA:
        case LV_STYLE_TEXT_OPA:
        case LV_STYLE_IMG_OPA:
        case LV_STYLE_BG_IMG_OPA:
        case LV_STYLE_LINE_OPA:
        case LV_STYLE_OUTLINE_OPA:
        case LV_STYLE_SHADOW_OPA:
        case LV_STYLE_CONTENT_OPA:
            value.num = LV_OPA_COVER;
            break;
        case LV_STYLE_BG_GRAD_STOP:
            value.num = 255;
            break;
        case LV_STYLE_BORDER_SIDE:
            value.num = LV_BORDER_SIDE_FULL;
            break;
        case LV_STYLE_TEXT_FONT:
        case LV_STYLE_CONTENT_FONT:
            value.ptr = LV_THEME_DEFAULT_FONT_NORMAL;
            break;
        case LV_STYLE_SIZE:
            value.num = 10;
            break;
        default:
            value.ptr = NULL;
            value.num = 0;
            break;
    }

    return value;
}

bool lv_style_is_empty(const lv_style_t * style)
{
    LV_ASSERT_STYLE(style);

    return style->prop_cnt == 0 ? true : false;
}

bool lv_debug_check_style(const lv_style_t * style)
{
    if(style == NULL) return true;  /*NULL style is still valid*/

#if LV_USE_ASSERT_STYLE
    if(style->sentinel != LV_DEBUG_STYLE_SENTINEL_VALUE) {
        LV_LOG_WARN("Invalid style (was local variable or not initialized?)");
        return false;
    }
#endif

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
