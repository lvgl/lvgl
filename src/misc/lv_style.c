/**
 * @file lv_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_style.h"
#include "../core/lv_global.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_string.h"
#include "lv_assert.h"
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/
#define _lv_style_custom_prop_flag_lookup_table_size LV_GLOBAL_DEFAULT()->style_custom_table_size
#define _lv_style_custom_prop_flag_lookup_table LV_GLOBAL_DEFAULT()->style_custom_prop_flag_lookup_table
#define last_custom_prop_id LV_GLOBAL_DEFAULT()->style_last_custom_prop_id

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/

const lv_style_prop_t lv_style_const_prop_id_inv = LV_STYLE_PROP_INV;

const uint8_t _lv_style_builtin_prop_flag_lookup_table[_LV_STYLE_NUM_BUILT_IN_PROPS] = {
    [LV_STYLE_WIDTH] =                    LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_MIN_WIDTH] =                LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_MAX_WIDTH] =                LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_HEIGHT] =                   LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_MIN_HEIGHT] =               LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_MAX_HEIGHT] =               LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_LENGTH] =                   LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE,
    [LV_STYLE_X] =                        LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_Y] =                        LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_ALIGN] =                    LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_TRANSFORM_WIDTH] =           LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_TRANSFORM,
    [LV_STYLE_TRANSFORM_HEIGHT] =          LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_TRANSFORM,
    [LV_STYLE_TRANSLATE_X] =               LV_STYLE_PROP_FLAG_LAYOUT_UPDATE | LV_STYLE_PROP_FLAG_PARENT_LAYOUT_UPDATE,
    [LV_STYLE_TRANSLATE_Y] =               LV_STYLE_PROP_FLAG_LAYOUT_UPDATE | LV_STYLE_PROP_FLAG_PARENT_LAYOUT_UPDATE,
    [LV_STYLE_TRANSFORM_SCALE_X] =           LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYER_UPDATE | LV_STYLE_PROP_FLAG_TRANSFORM,
    [LV_STYLE_TRANSFORM_SCALE_Y] =           LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYER_UPDATE | LV_STYLE_PROP_FLAG_TRANSFORM,
    [LV_STYLE_TRANSFORM_SKEW_X] =          LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYER_UPDATE | LV_STYLE_PROP_FLAG_TRANSFORM,
    [LV_STYLE_TRANSFORM_SKEW_Y] =          LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYER_UPDATE | LV_STYLE_PROP_FLAG_TRANSFORM,
    [LV_STYLE_TRANSFORM_ROTATION] =        LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYER_UPDATE | LV_STYLE_PROP_FLAG_TRANSFORM,

    [LV_STYLE_PAD_TOP] =                   LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_PAD_BOTTOM] =                LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_PAD_LEFT] =                  LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_PAD_RIGHT] =                 LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_PAD_ROW] =                   LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_PAD_COLUMN] =                LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_MARGIN_TOP] =                LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_MARGIN_BOTTOM] =             LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_MARGIN_LEFT] =               LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_MARGIN_RIGHT] =              LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,

    [LV_STYLE_BG_COLOR] = 0,
    [LV_STYLE_BG_OPA] = 0,
    [LV_STYLE_BG_GRAD_COLOR] = 0,
    [LV_STYLE_BG_GRAD_DIR] = 0,
    [LV_STYLE_BG_MAIN_STOP] = 0,
    [LV_STYLE_BG_GRAD_STOP] = 0,
    [LV_STYLE_BG_MAIN_OPA] = 0,
    [LV_STYLE_BG_GRAD_OPA] = 0,
    [LV_STYLE_BG_GRAD] = 0,

    [LV_STYLE_BG_IMAGE_SRC] =                LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE,
    [LV_STYLE_BG_IMAGE_OPA] = 0,
    [LV_STYLE_BG_IMAGE_RECOLOR] = 0,
    [LV_STYLE_BG_IMAGE_RECOLOR_OPA] = 0,
    [LV_STYLE_BG_IMAGE_TILED] = 0,

    [LV_STYLE_BORDER_COLOR] = 0,
    [LV_STYLE_BORDER_OPA] = 0,
    [LV_STYLE_BORDER_WIDTH] =              LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_BORDER_SIDE] = 0,
    [LV_STYLE_BORDER_POST] = 0,

    [LV_STYLE_OUTLINE_WIDTH] =             LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE,
    [LV_STYLE_OUTLINE_COLOR] = 0,
    [LV_STYLE_OUTLINE_OPA] =               LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE,
    [LV_STYLE_OUTLINE_PAD] =               LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE,

    [LV_STYLE_SHADOW_WIDTH] =              LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE,
    [LV_STYLE_SHADOW_OFFSET_X] =           LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE,
    [LV_STYLE_SHADOW_OFFSET_Y] =           LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE,
    [LV_STYLE_SHADOW_SPREAD] =             LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE,
    [LV_STYLE_SHADOW_COLOR] = 0,
    [LV_STYLE_SHADOW_OPA] =                LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE,

    [LV_STYLE_IMAGE_OPA] = 0,
    [LV_STYLE_IMAGE_RECOLOR] = 0,
    [LV_STYLE_IMAGE_RECOLOR_OPA] = 0,

    [LV_STYLE_LINE_WIDTH] =                LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE,
    [LV_STYLE_LINE_DASH_WIDTH] = 0,
    [LV_STYLE_LINE_DASH_GAP] = 0,
    [LV_STYLE_LINE_ROUNDED] = 0,
    [LV_STYLE_LINE_COLOR] = 0,
    [LV_STYLE_LINE_OPA] = 0,

    [LV_STYLE_ARC_WIDTH] =                 LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE,
    [LV_STYLE_ARC_ROUNDED] = 0,
    [LV_STYLE_ARC_COLOR] = 0,
    [LV_STYLE_ARC_OPA] = 0,
    [LV_STYLE_ARC_IMAGE_SRC] = 0,

    [LV_STYLE_TEXT_COLOR] =                LV_STYLE_PROP_FLAG_INHERITABLE,
    [LV_STYLE_TEXT_OPA] =                  LV_STYLE_PROP_FLAG_INHERITABLE,
    [LV_STYLE_TEXT_FONT] =                 LV_STYLE_PROP_FLAG_INHERITABLE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_TEXT_LETTER_SPACE] =         LV_STYLE_PROP_FLAG_INHERITABLE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_TEXT_LINE_SPACE] =           LV_STYLE_PROP_FLAG_INHERITABLE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_TEXT_DECOR] =                LV_STYLE_PROP_FLAG_INHERITABLE,
    [LV_STYLE_TEXT_ALIGN] =                LV_STYLE_PROP_FLAG_INHERITABLE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,

    [LV_STYLE_RADIUS] = 0,
    [LV_STYLE_CLIP_CORNER] = 0,
    [LV_STYLE_OPA] = 0,
    [LV_STYLE_OPA_LAYERED] =               LV_STYLE_PROP_FLAG_LAYER_UPDATE,
    [LV_STYLE_COLOR_FILTER_DSC] =          LV_STYLE_PROP_FLAG_INHERITABLE,
    [LV_STYLE_COLOR_FILTER_OPA] =          LV_STYLE_PROP_FLAG_INHERITABLE,
    [LV_STYLE_ANIM_DURATION] = 0,
    [LV_STYLE_TRANSITION] = 0,
    [LV_STYLE_BLEND_MODE] =                LV_STYLE_PROP_FLAG_LAYER_UPDATE,
    [LV_STYLE_LAYOUT] =                    LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_BASE_DIR] =                  LV_STYLE_PROP_FLAG_INHERITABLE | LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_BITMAP_MASK_SRC] =           LV_STYLE_PROP_FLAG_LAYER_UPDATE,

#if LV_USE_FLEX
    [LV_STYLE_FLEX_FLOW] =                    LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_FLEX_MAIN_PLACE] =              LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_FLEX_CROSS_PLACE] =             LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_FLEX_TRACK_PLACE] =             LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_FLEX_GROW] =                    LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
#endif

#if LV_USE_GRID
    [LV_STYLE_GRID_COLUMN_DSC_ARRAY] =      LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_GRID_ROW_DSC_ARRAY] =         LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_GRID_COLUMN_ALIGN] =          LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_GRID_ROW_ALIGN] =             LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_GRID_CELL_ROW_SPAN] =         LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_GRID_CELL_ROW_POS] =          LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_GRID_CELL_COLUMN_SPAN] =      LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_GRID_CELL_COLUMN_POS] =       LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_GRID_CELL_X_ALIGN] =          LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
    [LV_STYLE_GRID_CELL_Y_ALIGN] =          LV_STYLE_PROP_FLAG_LAYOUT_UPDATE,
#endif

};

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
#if LV_USE_ASSERT_STYLE
    if(style->sentinel == LV_STYLE_SENTINEL_VALUE && style->prop_cnt > 1) {
        LV_LOG_WARN("Style might be already inited. (Potential memory leak)");
    }
#endif

    lv_memzero(style, sizeof(lv_style_t));
#if LV_USE_ASSERT_STYLE
    style->sentinel = LV_STYLE_SENTINEL_VALUE;
#endif
}

void lv_style_reset(lv_style_t * style)
{
    LV_ASSERT_STYLE(style);

    if(style->prop_cnt != 255) lv_free(style->values_and_props);
    lv_memzero(style, sizeof(lv_style_t));
#if LV_USE_ASSERT_STYLE
    style->sentinel = LV_STYLE_SENTINEL_VALUE;
#endif
}

lv_style_prop_t lv_style_register_prop(uint8_t flag)
{
    if(_lv_style_custom_prop_flag_lookup_table == NULL) {
        _lv_style_custom_prop_flag_lookup_table_size = 0;
        last_custom_prop_id = (uint16_t)_LV_STYLE_LAST_BUILT_IN_PROP;
    }

    //    if((last_custom_prop_id + 1) != 0) {
    //        LV_LOG_ERROR("No more custom property IDs available");
    //        return LV_STYLE_PROP_INV;
    //    }

    /*
     * Allocate the lookup table if it's not yet available.
     */
    size_t required_size = (last_custom_prop_id + 1 - _LV_STYLE_LAST_BUILT_IN_PROP);
    if(_lv_style_custom_prop_flag_lookup_table_size < required_size) {
        /* Round required_size up to the nearest 32-byte value */
        required_size = (required_size + 31) & ~31;
        LV_ASSERT_MSG(required_size > 0, "required size has become 0?");
        uint8_t * old_p = _lv_style_custom_prop_flag_lookup_table;
        uint8_t * new_p = lv_realloc(old_p, required_size * sizeof(uint8_t));
        if(new_p == NULL) {
            LV_LOG_ERROR("Unable to allocate space for custom property lookup table");
            return LV_STYLE_PROP_INV;
        }
        _lv_style_custom_prop_flag_lookup_table = new_p;
        _lv_style_custom_prop_flag_lookup_table_size = required_size;
    }
    last_custom_prop_id++;
    /* This should never happen - we should bail out above */
    LV_ASSERT_NULL(_lv_style_custom_prop_flag_lookup_table);
    _lv_style_custom_prop_flag_lookup_table[last_custom_prop_id - _LV_STYLE_NUM_BUILT_IN_PROPS] = flag;
    return last_custom_prop_id;
}

lv_style_prop_t lv_style_get_num_custom_props(void)
{
    return last_custom_prop_id - _LV_STYLE_LAST_BUILT_IN_PROP;
}

bool lv_style_remove_prop(lv_style_t * style, lv_style_prop_t prop)
{
    LV_ASSERT_STYLE(style);

    if(lv_style_is_const(style)) {
        LV_LOG_ERROR("Cannot remove prop from const style");
        return false;
    }

    if(style->prop_cnt == 0)  return false;

    uint8_t * tmp = (lv_style_prop_t *)style->values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
    uint8_t * old_props = (uint8_t *)tmp;
    uint32_t i;
    for(i = 0; i < style->prop_cnt; i++) {
        if(old_props[i] == prop) {
            lv_style_value_t * old_values = (lv_style_value_t *)style->values_and_props;

            size_t size = (style->prop_cnt - 1) * (sizeof(lv_style_value_t) + sizeof(lv_style_prop_t));
            uint8_t * new_values_and_props = lv_malloc(size);
            if(new_values_and_props == NULL) return false;
            style->values_and_props = new_values_and_props;
            style->prop_cnt--;

            tmp = new_values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
            uint8_t * new_props = (uint8_t *)tmp;
            lv_style_value_t * new_values = (lv_style_value_t *)new_values_and_props;

            uint32_t j;
            for(i = j = 0; j <= style->prop_cnt;
                j++) { /*<=: because prop_cnt already reduced but all the old props. needs to be checked.*/
                if(old_props[j] != prop) {
                    new_values[i] = old_values[j];
                    new_props[i++] = old_props[j];
                }
            }

            lv_free(old_values);
            return true;
        }
    }

    return false;
}

void lv_style_set_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t value)
{
    LV_ASSERT_STYLE(style);

    if(lv_style_is_const(style)) {
        LV_LOG_ERROR("Cannot set property of constant style");
        return;
    }

    LV_ASSERT(prop != LV_STYLE_PROP_INV);

    lv_style_prop_t * props;
    int32_t i;

    if(style->values_and_props) {
        props = (lv_style_prop_t *)style->values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
        for(i = style->prop_cnt - 1; i >= 0; i--) {
            if(props[i] == prop) {
                lv_style_value_t * values = (lv_style_value_t *)style->values_and_props;
                values[i] = value;
                return;
            }
        }
    }

    size_t size = (style->prop_cnt + 1) * (sizeof(lv_style_value_t) + sizeof(lv_style_prop_t));
    uint8_t * values_and_props = lv_realloc(style->values_and_props, size);
    if(values_and_props == NULL) return;
    style->values_and_props = values_and_props;

    props = values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
    /*Shift all props to make place for the value before them*/
    for(i = style->prop_cnt - 1; i >= 0; i--) {
        props[i + sizeof(lv_style_value_t) / sizeof(lv_style_prop_t)] = props[i];
    }
    style->prop_cnt++;

    /*Go to the new position with the props*/
    props = values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
    lv_style_value_t * values = (lv_style_value_t *)values_and_props;

    /*Set the new property and value*/
    props[style->prop_cnt - 1] = prop;
    values[style->prop_cnt - 1] = value;

    uint32_t group = _lv_style_get_prop_group(prop);
    style->has_group |= (uint32_t)1 << group;
}

lv_style_res_t lv_style_get_prop(const lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value)
{
    return lv_style_get_prop_inlined(style, prop, value);
}

void lv_style_transition_dsc_init(lv_style_transition_dsc_t * tr, const lv_style_prop_t props[],
                                  lv_anim_path_cb_t path_cb, uint32_t time, uint32_t delay, void * user_data)
{
    lv_memzero(tr, sizeof(lv_style_transition_dsc_t));
    tr->props = props;
    tr->path_xcb = path_cb == NULL ? lv_anim_path_linear : path_cb;
    tr->time = time;
    tr->delay = delay;
    tr->user_data = user_data;
}

lv_style_value_t lv_style_prop_get_default(lv_style_prop_t prop)
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
                .color = white
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
                .color = black
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
        case LV_STYLE_ROTARY_SENSITIVITY:
            return (lv_style_value_t) {
                .num = 256
            };
        default:
            return (lv_style_value_t) {
                .ptr = NULL
            };
    }
}

bool lv_style_is_empty(const lv_style_t * style)
{
    LV_ASSERT_STYLE(style);

    return style->prop_cnt == 0;
}

uint8_t _lv_style_prop_lookup_flags(lv_style_prop_t prop)
{
    if(prop == LV_STYLE_PROP_ANY) return LV_STYLE_PROP_FLAG_ALL; /*Any prop can have any flags*/
    if(prop == LV_STYLE_PROP_INV) return 0;

    if(prop < _LV_STYLE_NUM_BUILT_IN_PROPS)
        return _lv_style_builtin_prop_flag_lookup_table[prop];
    prop -= _LV_STYLE_NUM_BUILT_IN_PROPS;
    if(_lv_style_custom_prop_flag_lookup_table != NULL && prop < _lv_style_custom_prop_flag_lookup_table_size)
        return _lv_style_custom_prop_flag_lookup_table[prop];
    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
