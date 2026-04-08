/**
 * @file lv_style.h
 *
 */

#ifndef LV_STYLE_H
#define LV_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../font/lv_font.h"
#include "lv_color.h"
#include "lv_area.h"
#include "lv_anim.h"
#include "lv_text.h"
#include "lv_types.h"
#include "lv_assert.h"
#include "lv_bidi.h"
#include "lv_grad.h"
#include "../layouts/lv_layout.h"

/*********************
 *      DEFINES
 *********************/

#define LV_STYLE_SENTINEL_VALUE     0xAABBCCDD

/*
 * Flags for style behavior
 */
#define LV_STYLE_PROP_FLAG_NONE                     (0)       /**< No special behavior */
#define LV_STYLE_PROP_FLAG_INHERITABLE              (1 << 0)  /**< Inherited */
#define LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE          (1 << 1)  /**< Requires ext. draw size update when changed */
#define LV_STYLE_PROP_FLAG_LAYOUT_UPDATE            (1 << 2)  /**< Requires layout update when changed */
#define LV_STYLE_PROP_FLAG_PARENT_LAYOUT_UPDATE     (1 << 3)  /**< Requires layout update on parent when changed */
#define LV_STYLE_PROP_FLAG_LAYER_UPDATE             (1 << 4)  /**< Affects layer handling */
#define LV_STYLE_PROP_FLAG_TRANSFORM                (1 << 5)  /**< Affects the object's transformation */
#define LV_STYLE_PROP_FLAG_ALL                      (0x3F)    /**< Indicating all flags */

/*
 * Other constants
 */
#define LV_SCALE_NONE            256        /**< Value for not zooming the image */
LV_EXPORT_CONST_INT(LV_SCALE_NONE);

// *INDENT-OFF*
#if LV_USE_ASSERT_STYLE
#define LV_STYLE_CONST_INIT(var_name, prop_array)                       \
    const lv_style_t var_name = {                                       \
        .sentinel = LV_STYLE_SENTINEL_VALUE,                            \
        .values_and_props = (void*)prop_array,                          \
        .has_group = 0xFFFFFFFF,                                        \
        .prop_cnt = 255                                                 \
    }
#else
#define LV_STYLE_CONST_INIT(var_name, prop_array)                       \
    const lv_style_t var_name = {                                       \
        .values_and_props = (void*)prop_array,                          \
        .has_group = 0xFFFFFFFF,                                        \
        .prop_cnt = 255,                                                \
    }
#endif
// *INDENT-ON*

#define LV_STYLE_CONST_PROPS_END { .prop = LV_STYLE_PROP_INV, .value = { .num = 0 } }

#if LV_GRADIENT_MAX_STOPS < 2
#error LVGL needs at least 2 stops for gradients. Please increase the LV_GRADIENT_MAX_STOPS
#endif

#define LV_GRAD_LEFT    LV_PCT(0)
#define LV_GRAD_RIGHT   LV_PCT(100)
#define LV_GRAD_TOP     LV_PCT(0)
#define LV_GRAD_BOTTOM  LV_PCT(100)
#define LV_GRAD_CENTER  LV_PCT(50)

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Possible options for blending opaque drawings
 */
typedef enum {
    LV_BLEND_MODE_NORMAL,     /**< Simply mix according to the opacity value*/
    LV_BLEND_MODE_ADDITIVE,   /**< Add the respective color channels*/
    LV_BLEND_MODE_SUBTRACTIVE,/**< Subtract the foreground from the background*/
    LV_BLEND_MODE_MULTIPLY,   /**< Multiply the foreground and background*/
    LV_BLEND_MODE_DIFFERENCE, /**< Absolute difference between foreground and background*/
} lv_blend_mode_t;

/**
 * Some options to apply decorations on texts.
 * 'OR'ed values can be used.
 */
typedef enum {
    LV_TEXT_DECOR_NONE          = 0x00,
    LV_TEXT_DECOR_UNDERLINE     = 0x01,
    LV_TEXT_DECOR_STRIKETHROUGH = 0x02,
} lv_text_decor_t;

/**
 * Selects on which sides border should be drawn
 * 'OR'ed values can be used.
 */
typedef enum {
    LV_BORDER_SIDE_NONE     = 0x00,
    LV_BORDER_SIDE_BOTTOM   = 0x01,
    LV_BORDER_SIDE_TOP      = 0x02,
    LV_BORDER_SIDE_LEFT     = 0x04,
    LV_BORDER_SIDE_RIGHT    = 0x08,
    LV_BORDER_SIDE_FULL     = 0x0F,
    LV_BORDER_SIDE_INTERNAL = 0x10, /**< FOR matrix-like objects (e.g. Button matrix)*/
} lv_border_side_t;

typedef enum {
    LV_BLUR_QUALITY_AUTO = 0,   /**< Set the quality automatically */
    LV_BLUR_QUALITY_SPEED,      /**< Prefer speed over precision */
    LV_BLUR_QUALITY_PRECISION,  /**< Prefer precision over speed*/
} lv_blur_quality_t;

/** A image colorkey definition.
 *  The transparency within the color range of [low, high] will be set to LV_OPA_TRANSP If the "enable" flag is set to true.
 */
typedef struct {
    lv_color_t low;
    lv_color_t high;
} lv_image_colorkey_t;

/**
 * A common type to handle all the property types in the same way.
 */
typedef union {
    int32_t num;         /**< Number integer number (opacity, enums, booleans or "normal" numbers)*/
    const void * ptr;    /**< Constant pointers  (font, cone text, etc)*/
    lv_color_t color;    /**< Colors*/
} lv_style_value_t;

/**
 * Enumeration of all built in style properties
 *
 * Props are split into groups of 16. When adding a new prop to a group, ensure it does not overflow into the next one.
 */
enum _lv_style_id_t {
    LV_STYLE_PROP_INV               = 0,

    /*The properties are listed in a special order to make caching more effective.
     *Groups are used when LV_OBJ_STYLE_CACHE is enabled.
     *If a property from a groups is set, a bit will be set in
     *obj->style_main_prop_is_set and style_other_prop_is_set
     *to indicate that the property is set.
     *
     *Strategy 1: group related properties. E.g. if no "border" properties are set
     *            they will be skipped quickly.
     *
     *Strategy 2: group common property with rarely used properties. This way
     *            the common property is cached properly and it's rarely affected
     *            by other props. The other props are cached in an sub-optimal way,
     *            but they are used rarely.
     *
     *Strategy 3: group properties they are used rarely together so that caching doesn't
     *            interfere
     *
     *Each group can have 8 properties. (see STYLE_PROP_SHIFTED)*/

    /* Size related properties*/
    LV_STYLE_WIDTH = 1,
    LV_STYLE_HEIGHT,
    LV_STYLE_LENGTH,
    LV_STYLE_TRANSFORM_WIDTH,
    LV_STYLE_TRANSFORM_HEIGHT,

    LV_STYLE_MIN_WIDTH = 8,
    LV_STYLE_MAX_WIDTH,
    LV_STYLE_MIN_HEIGHT,
    LV_STYLE_MAX_HEIGHT,
    LV_STYLE_TRANSLATE_X,
    LV_STYLE_TRANSLATE_Y,
    LV_STYLE_RADIAL_OFFSET,

    /*Position related properties */
    LV_STYLE_X = 16,
    LV_STYLE_Y,
    LV_STYLE_ALIGN,

    /*Padding related properties */
    LV_STYLE_PAD_TOP = 24,
    LV_STYLE_PAD_BOTTOM,
    LV_STYLE_PAD_LEFT,
    LV_STYLE_PAD_RIGHT,
    LV_STYLE_PAD_RADIAL,
    LV_STYLE_PAD_ROW,
    LV_STYLE_PAD_COLUMN,

    /*Margin related properties*/
    LV_STYLE_MARGIN_TOP = 32,
    LV_STYLE_MARGIN_BOTTOM,
    LV_STYLE_MARGIN_LEFT,
    LV_STYLE_MARGIN_RIGHT,

    /*Bg. Gradient*/
    LV_STYLE_BG_GRAD = 40,
    LV_STYLE_BG_GRAD_DIR,
    LV_STYLE_BG_MAIN_OPA,
    LV_STYLE_BG_GRAD_OPA,
    LV_STYLE_BG_GRAD_COLOR,
    LV_STYLE_BG_MAIN_STOP,
    LV_STYLE_BG_GRAD_STOP,

    /*Bg image*/
    LV_STYLE_BG_IMAGE_SRC = 48,
    LV_STYLE_BG_IMAGE_OPA,
    LV_STYLE_BG_IMAGE_RECOLOR_OPA,
    LV_STYLE_BG_IMAGE_TILED,
    LV_STYLE_BG_IMAGE_RECOLOR,

    /*Group 3*/
    LV_STYLE_BORDER_WIDTH = 56,
    LV_STYLE_BORDER_COLOR,
    LV_STYLE_BORDER_OPA,
    LV_STYLE_BORDER_POST,
    LV_STYLE_BORDER_SIDE,

    /*Outline */
    LV_STYLE_OUTLINE_WIDTH = 64,
    LV_STYLE_OUTLINE_COLOR,
    LV_STYLE_OUTLINE_OPA,
    LV_STYLE_OUTLINE_PAD,

    /*Image, Shadow, Line, Arc, and Text are rarely used together.*/
    LV_STYLE_BG_OPA = 72,
    LV_STYLE_BG_COLOR,
    LV_STYLE_SHADOW_WIDTH,
    LV_STYLE_LINE_WIDTH,
    LV_STYLE_ARC_WIDTH,
    LV_STYLE_TEXT_FONT,
    LV_STYLE_IMAGE_RECOLOR_OPA,

    LV_STYLE_IMAGE_OPA = 80,
    LV_STYLE_SHADOW_OPA,
    LV_STYLE_LINE_OPA,
    LV_STYLE_ARC_OPA,
    LV_STYLE_TEXT_OPA,

    LV_STYLE_SHADOW_COLOR = 88,
    LV_STYLE_IMAGE_RECOLOR,
    LV_STYLE_LINE_COLOR,
    LV_STYLE_ARC_COLOR,
    LV_STYLE_TEXT_COLOR,

    LV_STYLE_ARC_IMAGE_SRC = 96,
    LV_STYLE_SHADOW_OFFSET_X,
    LV_STYLE_SHADOW_OFFSET_Y,
    LV_STYLE_SHADOW_SPREAD,
    LV_STYLE_LINE_DASH_WIDTH,
    LV_STYLE_TEXT_ALIGN,
    LV_STYLE_TEXT_LETTER_SPACE,
    LV_STYLE_TEXT_LINE_SPACE,

    LV_STYLE_LINE_DASH_GAP = 104,
    LV_STYLE_LINE_ROUNDED,
    LV_STYLE_IMAGE_COLORKEY,
    LV_STYLE_TEXT_OUTLINE_STROKE_WIDTH,
    LV_STYLE_TEXT_OUTLINE_STROKE_OPA,
    LV_STYLE_TEXT_OUTLINE_STROKE_COLOR,
    LV_STYLE_TEXT_DECOR,
    LV_STYLE_ARC_ROUNDED,

    /*Group unrelated props*/
    LV_STYLE_OPA = 112,
    LV_STYLE_OPA_LAYERED,
    LV_STYLE_COLOR_FILTER_DSC,
    LV_STYLE_COLOR_FILTER_OPA,
    LV_STYLE_ANIM,
    LV_STYLE_ANIM_DURATION,
    LV_STYLE_TRANSITION,

    /*Radius is requested a lot, group it with rarely requested ones*/
    LV_STYLE_RADIUS = 120,
    LV_STYLE_BITMAP_MASK_SRC,
    LV_STYLE_BLEND_MODE,
    LV_STYLE_ROTARY_SENSITIVITY,
    LV_STYLE_TRANSLATE_RADIAL,

    /*Requested a lot but rarely used*/
    LV_STYLE_CLIP_CORNER = 128,
    LV_STYLE_BASE_DIR,
    LV_STYLE_RECOLOR,
    LV_STYLE_RECOLOR_OPA,
    LV_STYLE_LAYOUT,

    /*Blur*/
    LV_STYLE_BLUR_RADIUS = 136,
    LV_STYLE_BLUR_BACKDROP,
    LV_STYLE_BLUR_QUALITY,

    /*Drop shadow*/
    LV_STYLE_DROP_SHADOW_RADIUS = 144,
    LV_STYLE_DROP_SHADOW_OFFSET_X,
    LV_STYLE_DROP_SHADOW_OFFSET_Y,
    LV_STYLE_DROP_SHADOW_COLOR,
    LV_STYLE_DROP_SHADOW_OPA,
    LV_STYLE_DROP_SHADOW_QUALITY,

    /*Scale and transform*/
    LV_STYLE_TRANSFORM_SCALE_X = 152,
    LV_STYLE_TRANSFORM_SCALE_Y,
    LV_STYLE_TRANSFORM_PIVOT_X,
    LV_STYLE_TRANSFORM_PIVOT_Y,
    LV_STYLE_TRANSFORM_ROTATION,
    LV_STYLE_TRANSFORM_SKEW_X,
    LV_STYLE_TRANSFORM_SKEW_Y,

    /*Flex and basic grid (rarely used together)*/
    LV_STYLE_FLEX_FLOW = 160,
    LV_STYLE_FLEX_MAIN_PLACE,
    LV_STYLE_FLEX_CROSS_PLACE,
    LV_STYLE_FLEX_TRACK_PLACE,
    LV_STYLE_FLEX_GROW,
    LV_STYLE_GRID_COLUMN_DSC_ARRAY,
    LV_STYLE_GRID_ROW_DSC_ARRAY,

    LV_STYLE_GRID_COLUMN_ALIGN = 168,
    LV_STYLE_GRID_ROW_ALIGN,
    LV_STYLE_GRID_CELL_COLUMN_POS,
    LV_STYLE_GRID_CELL_COLUMN_SPAN,
    LV_STYLE_GRID_CELL_X_ALIGN,
    LV_STYLE_GRID_CELL_ROW_POS,
    LV_STYLE_GRID_CELL_ROW_SPAN,
    LV_STYLE_GRID_CELL_Y_ALIGN,

    LV_STYLE_LAST_BUILT_IN_PROP,
    LV_STYLE_NUM_BUILT_IN_PROPS     = LV_STYLE_LAST_BUILT_IN_PROP + 1,

    LV_STYLE_PROP_ANY                = 0xFF,
    LV_STYLE_PROP_CONST             = 0xFF /* magic value for const styles */
};

typedef enum {
    LV_STYLE_RES_NOT_FOUND,
    LV_STYLE_RES_FOUND,
} lv_style_res_t;

/**
 * Descriptor for style transitions
 */
typedef struct {
    const lv_style_prop_t * props; /**< An array with the properties to animate.*/
    void * user_data;              /**< A custom user data that will be passed to the animation's user_data */
    lv_anim_path_cb_t path_xcb;    /**< A path for the animation.*/
    uint32_t time;                 /**< Duration of the transition in [ms]*/
    uint32_t delay;                /**< Delay before the transition in [ms]*/
} lv_style_transition_dsc_t;

/**
 * Descriptor of a constant style property.
 */
typedef struct {
    lv_style_prop_t prop;
    lv_style_value_t value;
} lv_style_const_prop_t;

/**
 * Descriptor of a style (a collection of properties and values).
 */
typedef struct {

#if LV_USE_ASSERT_STYLE
    uint32_t sentinel;
#endif

    void * values_and_props;

    uint32_t has_group;
    uint8_t prop_cnt;   /**< 255 means it's a constant style*/
} lv_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a style
 * @param style pointer to a style to initialize
 * @note Do not call `lv_style_init` on styles that already have some properties
 *       because this function won't free the used memory, just sets a default state for the style.
 *       In other words be sure to initialize styles only once!
 */
void lv_style_init(lv_style_t * style);

/**
 * Clear all properties from a style and free all allocated memories.
 * @param style pointer to a style
 */
void lv_style_reset(lv_style_t * style);

/**
 * Copy all properties of a style to an other.
 * It has the same affect callying the same `lv_set_style_...`
 * functions on both styles.
 * It means new memory will be allocated to store the properties in
 * the destination style.
 * After the copy the destination style is fully independent of the source
 * and source can removed without affecting the destination style.
 * @param dst   the destination to copy into (can not the a constant style)
 * @param src   the source style to copy from.
 */
void lv_style_copy(lv_style_t * dst, const lv_style_t * src);

/**
 * Copy all properties of a style to an other without resetting the dst style.
 * It has the same effect as calling the same `lv_set_style_...`
 * functions on both styles.
 * It means new memory will be allocated to store the properties in
 * the destination style.
 * After the copy the destination style is fully independent of the source
 * and source can removed without affecting the destination style.
 * @param dst   the destination to copy into (cannot be a constant style)
 * @param src   the source style to copy from.
 */
void lv_style_merge(lv_style_t * dst, const lv_style_t * src);


/**
 * Check if a style is constant
 * @param style     pointer to a style
 * @return          true: the style is constant
 */
static inline bool lv_style_is_const(const lv_style_t * style)
{
    if(style->prop_cnt == 255) return true;
    return false;
}


/**
 * Register a new style property for custom usage
 * @return a new property ID, or LV_STYLE_PROP_INV if there are no more available.
 *
 * Example:
 * @code
 * lv_style_prop_t MY_PROP;
 * static inline void lv_style_set_my_prop(lv_style_t * style, lv_color_t value) {
 * lv_style_value_t v = {.color = value}; lv_style_set_prop(style, MY_PROP, v); }
 *
 * ...
 * MY_PROP = lv_style_register_prop();
 * ...
 * lv_style_set_my_prop(&style1, lv_palette_main(LV_PALETTE_RED));
 * @endcode
 */
lv_style_prop_t lv_style_register_prop(uint8_t flag);

/**
 * Get the number of custom properties that have been registered thus far.
 */
lv_style_prop_t lv_style_get_num_custom_props(void);

/**
 * Remove a property from a style
 * @param style pointer to a style
 * @param prop  a style property ORed with a state.
 * @return true: the property was found and removed; false: the property wasn't found
 */
bool lv_style_remove_prop(lv_style_t * style, lv_style_prop_t prop);

/**
 * Set the value of property in a style.
 * This function shouldn't be used directly by the user.
 * Instead use `lv_style_set_<prop_name>()`. E.g. `lv_style_set_bg_color()`
 * @param style pointer to style
 * @param prop the ID of a property (e.g. `LV_STYLE_BG_COLOR`)
 * @param value `lv_style_value_t` variable in which a field is set according to the type of `prop`
 */
void lv_style_set_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t value);

/**
 * Get the value of a property
 * @param style pointer to a style
 * @param prop  the ID of a property
 * @param value pointer to a `lv_style_value_t` variable to store the value
 * @return LV_RESULT_INVALID: the property wasn't found in the style (`value` is unchanged)
 *         LV_RESULT_OK: the property was fond, and `value` is set accordingly
 * @note For performance reasons there are no sanity check on `style`
 */
lv_style_res_t lv_style_get_prop(const lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value);

/**
 * Initialize a transition descriptor.
 * @param tr        pointer to a transition descriptor to initialize
 * @param props     an array with the properties to transition. The last element must be zero.
 * @param path_cb   an animation path (ease) callback. If `NULL` liner path will be used.
 * @param time      duration of the transition in [ms]
 * @param delay     delay before the transition in [ms]
 * @param user_data any custom data that will be saved in the transition animation and will be available when `path_cb` is called
 *
 * Example:
 * @code
 * const static lv_style_prop_t trans_props[] = { LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR, 0 };
 * static lv_style_transition_dsc_t trans1;
 * lv_style_transition_dsc_init(&trans1, trans_props, NULL, 300, 0, NULL);
 * @endcode
 */
void lv_style_transition_dsc_init(lv_style_transition_dsc_t * tr, const lv_style_prop_t props[],
                                  lv_anim_path_cb_t path_cb, uint32_t time, uint32_t delay, void * user_data);

/**
 * Get the default value of a property
 * @param prop the ID of a property
 * @return the default value
 */
lv_style_value_t lv_style_prop_get_default(lv_style_prop_t prop);

/**
 * Get the value of a property
 * @param style pointer to a style
 * @param prop  the ID of a property
 * @param value pointer to a `lv_style_value_t` variable to store the value
 * @return LV_RESULT_INVALID: the property wasn't found in the style (`value` is unchanged)
 *         LV_RESULT_OK: the property was fond, and `value` is set accordingly
 * @note For performance reasons there are no sanity check on `style`
 * @note This function is the same as ::lv_style_get_prop but inlined. Use it only on performance critical places
 */
static inline lv_style_res_t lv_style_get_prop_inlined(const lv_style_t * style, lv_style_prop_t prop,
                                                       lv_style_value_t * value)
{
    if(lv_style_is_const(style)) {
        lv_style_const_prop_t * props = (lv_style_const_prop_t *)style->values_and_props;
        uint32_t i;
        for(i = 0; props[i].prop != LV_STYLE_PROP_INV; i++) {
            if(props[i].prop == prop) {
                *value = props[i].value;
                return LV_STYLE_RES_FOUND;
            }
        }
    }
    else {
        lv_style_prop_t * props = (lv_style_prop_t *)style->values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
        uint32_t i;
        for(i = 0; i < style->prop_cnt; i++) {
            if(props[i] == prop) {
                lv_style_value_t * values = (lv_style_value_t *)style->values_and_props;
                *value = values[i];
                return LV_STYLE_RES_FOUND;
            }
        }
    }
    return LV_STYLE_RES_NOT_FOUND;
}

/**
 * Checks if a style is empty (has no properties)
 * @param style pointer to a style
 * @return true if the style is empty
 */
bool lv_style_is_empty(const lv_style_t * style);

/**
 * Tell the group of a property. If the a property from a group is set in a style the (1 << group) bit of style->has_group is set.
 * It allows early skipping the style if the property is not exists in the style at all.
 * @param prop a style property
 * @return the group [0..30] 30 means all the custom properties with index > 120
 */
static inline uint32_t lv_style_get_prop_group(lv_style_prop_t prop)
{
    uint32_t group = prop >> 2;
    if(group > 30) group = 31;    /*The MSB marks all the custom properties*/
    return group;

}

/**
 * Get the flags of a built-in or custom property.
 *
 * @param prop a style property
 * @return the flags of the property
 */
uint8_t lv_style_prop_lookup_flags(lv_style_prop_t prop);

#include "lv_style_gen.h"

/**
 * Set `style`s width and height.
 * @param  style   pointer to style to be modified
 * @param  width   width in pixels
 * @param  height  height in pixels
 */
static inline void lv_style_set_size(lv_style_t * style, int32_t width, int32_t height)
{
    lv_style_set_width(style, width);
    lv_style_set_height(style, height);
}

/**
 * Set all 4 of `style`s padding values.
 * @param  style   pointer to style to be modified
 * @param  value   padding dimension in pixels
 */
static inline void lv_style_set_pad_all(lv_style_t * style, int32_t value)
{
    lv_style_set_pad_left(style, value);
    lv_style_set_pad_right(style, value);
    lv_style_set_pad_top(style, value);
    lv_style_set_pad_bottom(style, value);
}

/**
 * Set `style`s left and right padding values.
 * @param  style   pointer to style to be modified
 * @param  value   padding dimension in pixels
 */
static inline void lv_style_set_pad_hor(lv_style_t * style, int32_t value)
{
    lv_style_set_pad_left(style, value);
    lv_style_set_pad_right(style, value);
}

/**
 * Set `style`s top and bottom padding values.
 * @param  style   pointer to style to be modified
 * @param  value   padding dimension in pixels
 */
static inline void lv_style_set_pad_ver(lv_style_t * style, int32_t value)
{
    lv_style_set_pad_top(style, value);
    lv_style_set_pad_bottom(style, value);
}

/**
 * Set `style`s row and column padding gaps (applies only to Grid and Flex layouts).
 * @param  style   pointer to style to be modified
 * @param  value   gap dimension in pixels
 */
static inline void lv_style_set_pad_gap(lv_style_t * style, int32_t value)
{
    lv_style_set_pad_row(style, value);
    lv_style_set_pad_column(style, value);
}

/**
 * Set `style`s left and right margin values.
 * @param  style   pointer to style to be modified
 * @param  value   margin dimension in pixels
 */
static inline void lv_style_set_margin_hor(lv_style_t * style, int32_t value)
{
    lv_style_set_margin_left(style, value);
    lv_style_set_margin_right(style, value);
}

/**
 * Set `style`s top and bottom margin values.
 * @param  style   pointer to style to be modified
 * @param  value   margin dimension in pixels
 */
static inline void lv_style_set_margin_ver(lv_style_t * style, int32_t value)
{
    lv_style_set_margin_top(style, value);
    lv_style_set_margin_bottom(style, value);
}

/**
 * Set all 4 of `style`s margin values.
 * @param  style   pointer to style to be modified
 * @param  value   margin dimension in pixels
 */
static inline void lv_style_set_margin_all(lv_style_t * style, int32_t value)
{
    lv_style_set_margin_left(style, value);
    lv_style_set_margin_right(style, value);
    lv_style_set_margin_top(style, value);
    lv_style_set_margin_bottom(style, value);
}

/**
 * Set `style`s X and Y transform scale values.
 * @param  style   pointer to style to be modified
 * @param  value   scale factor.  Example values:
 *                     - 256 or LV_SCALE_NONE:  no zoom
 *                     - <256:  scale down
 *                     - >256:  scale up
 *                     - 128:  half size
 *                     - 512:  double size
 */
static inline void lv_style_set_transform_scale(lv_style_t * style, int32_t value)
{
    lv_style_set_transform_scale_x(style, value);
    lv_style_set_transform_scale_y(style, value);
}

/**
 * @brief Check if the style property has a specified behavioral flag.
 *
 * Do not pass multiple flags to this function as backwards-compatibility is not guaranteed
 * for that.
 *
 * @param prop Property ID
 * @param flag Flag
 * @return true if the flag is set for this property
 */
static inline bool lv_style_prop_has_flag(lv_style_prop_t prop, uint8_t flag)
{
    return lv_style_prop_lookup_flags(prop) & flag;
}

/*************************
 *    GLOBAL VARIABLES
 *************************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_style_prop_t lv_style_const_prop_id_inv;

/**********************
 *      MACROS
 **********************/

#if LV_USE_ASSERT_STYLE
#  define LV_ASSERT_STYLE(style_p)                                                                            \
    do {                                                                                                      \
        LV_ASSERT_MSG(style_p != NULL, "The style is NULL");                                                  \
        LV_ASSERT_MSG(style_p->sentinel == LV_STYLE_SENTINEL_VALUE, "Style is not initialized or corrupted"); \
    } while(0)
#else
#  define LV_ASSERT_STYLE(p) do{}while(0)
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_STYLE_H*/
