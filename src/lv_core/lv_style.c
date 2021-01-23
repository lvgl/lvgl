/**
 * @file lv_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
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

void _lv_style_system_init(void)
{
}

/**
 * Initialize a style
 * @param style pointer to a style to initialize
 */
void lv_style_init(lv_style_t * style)
{
    lv_memset_00(style, sizeof(lv_style_t));
#if LV_USE_ASSERT_STYLE
    style->sentinel = LV_DEBUG_STYLE_SENTINEL_VALUE;
#endif

}

uint16_t lv_style_register_prop(bool inherit)
{
    static uint16_t act_id = (uint16_t)_LV_STYLE_LAST_BUILT_IN_PROP;
    act_id++;
    if(inherit) return act_id | LV_STYLE_PROP_INHERIT;
    else return act_id;
}

/**
 * Remove a property from a style
 * @param style pointer to a style
 * @param prop  a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @return true: the property was found and removed; false: the property wasn't found
 */
bool lv_style_remove_prop(lv_style_t * style, lv_style_prop_t prop)
{
    if(style == NULL) return false;
    LV_ASSERT_STYLE(style);

    if(!style->allocated) {
        if(style->prop1 == prop) {
            style->prop1 = _LV_STYLE_PROP_INV;
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

/**
 * Clear all properties from a style and all allocated memories.
 * @param style pointer to a style
 */
void lv_style_reset(lv_style_t * style)
{
    LV_ASSERT_STYLE(style);
//    lv_mem_free(style->ext);
    lv_style_init(style);
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

void lv_style_transition_init(lv_style_transition_t * tr, const lv_style_prop_t * props, const lv_anim_path_t * path, uint32_t time, uint32_t delay)
{
    lv_memset_00(tr, sizeof(lv_style_transition_t));
    tr->props = props;
    tr->path = path;
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
        case LV_STYLE_LINE_OPA:
        case LV_STYLE_OUTLINE_OPA:
        case LV_STYLE_SHADOW_OPA:
            value.num = LV_OPA_COVER;
            break;
        case LV_STYLE_BG_GRAD_STOP:
            value.num = 255;
            break;
        case LV_STYLE_BORDER_SIDE:
            value.num = LV_BORDER_SIDE_FULL;
            break;
        case LV_STYLE_TEXT_FONT:
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


/**
 * Check whether a style is valid (initialized correctly)
 * @param style pointer to a style
 * @return true: valid
 */
bool lv_debug_check_style(const lv_style_t * style)
{
    if(style == NULL) return true;  /*NULL style is still valid*/

#if LV_USE_ASSERT_STYLE
    if(style->sentinel != LV_DEBUG_STYLE_SENTINEL_VALUE) {
        LV_LOG_WARN("Invalid style (local variable or not initialized?)");
        return false;
    }
#endif

    return true;
}

/**
 * Check whether a style list is valid (initialized correctly)
 * @param style pointer to a style
 * @return true: valid
 */
bool lv_debug_check_style_list(const void * list)
{
    return true;
}

bool lv_style_is_empty(const lv_style_t * style)
{
    return style->prop_cnt == 0 ? true : false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
