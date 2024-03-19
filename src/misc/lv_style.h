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
#include <stdbool.h>
#include <stdint.h>
#include "../font/lv_font.h"
#include "lv_color.h"
#include "lv_area.h"
#include "lv_anim.h"
#include "lv_text.h"
#include "lv_types.h"
#include "lv_assert.h"
#include "lv_bidi.h"
#include "../layouts/lv_layout.h"

/*********************
 *      DEFINES
 *********************/

#define LV_STYLE_SENTINEL_VALUE     0xAABBCCDD

/**
 * Flags for style behavior
 *
 * The rest of the flags will have _FLAG added to their name in v9.
 */
#define LV_STYLE_PROP_FLAG_NONE                     (0)
#define LV_STYLE_PROP_FLAG_INHERITABLE              (1 << 0)  /*Inherited*/
#define LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE          (1 << 1)  /*Requires ext. draw size update when changed*/
#define LV_STYLE_PROP_FLAG_LAYOUT_UPDATE            (1 << 2)  /*Requires layout update when changed*/
#define LV_STYLE_PROP_FLAG_PARENT_LAYOUT_UPDATE     (1 << 3)  /*Requires layout update on parent when changed*/
#define LV_STYLE_PROP_FLAG_LAYER_UPDATE             (1 << 4)  /*Affects layer handling*/
#define LV_STYLE_PROP_FLAG_TRANSFORM                (1 << 5)  /*Affects the object's transformation*/
#define LV_STYLE_PROP_FLAG_ALL                      (0x3F)    /*Indicating all flags*/

/**
 * Other constants
 */
#define LV_SCALE_NONE            256        /*Value for not zooming the image*/
LV_EXPORT_CONST_INT(LV_SCALE_NONE);

// *INDENT-OFF*
#if LV_USE_ASSERT_STYLE
#define LV_STYLE_CONST_INIT(var_name, prop_array)                       \
    const lv_style_t var_name = {                                       \
        .sentinel = LV_STYLE_SENTINEL_VALUE,                            \
        .values_and_props = (void*)prop_array,                                      \
        .has_group = 0xFFFFFFFF,                                        \
        .prop_cnt = 255                                               \
    }
#else
#define LV_STYLE_CONST_INIT(var_name, prop_array)                       \
    const lv_style_t var_name = {                                       \
        .values_and_props = prop_array,                                      \
        .has_group = 0xFFFFFFFF,                                        \
        .prop_cnt = 255,                                               \
    }
#endif
// *INDENT-ON*

#define LV_STYLE_CONST_PROPS_END { .prop_ptr = NULL, .value = { .num = 0 } }

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Possible options how to blend opaque drawings
 */
enum _lv_blend_mode_t {
    LV_BLEND_MODE_NORMAL,     /**< Simply mix according to the opacity value*/
    LV_BLEND_MODE_ADDITIVE,   /**< Add the respective color channels*/
    LV_BLEND_MODE_SUBTRACTIVE,/**< Subtract the foreground from the background*/
    LV_BLEND_MODE_MULTIPLY,   /**< Multiply the foreground and background*/
};

#ifdef DOXYGEN
typedef _lv_blend_mode_t lv_blend_mode_t;
#else
typedef uint8_t lv_blend_mode_t;
#endif /*DOXYGEN*/

/**
 * Some options to apply decorations on texts.
 * 'OR'ed values can be used.
 */
enum _lv_text_decor_t {
    LV_TEXT_DECOR_NONE          = 0x00,
    LV_TEXT_DECOR_UNDERLINE     = 0x01,
    LV_TEXT_DECOR_STRIKETHROUGH = 0x02,
};

#ifdef DOXYGEN
typedef _lv_text_decor_t lv_text_decor_t;
#else
typedef uint8_t lv_text_decor_t;
#endif /*DOXYGEN*/

/**
 * Selects on which sides border should be drawn
 * 'OR'ed values can be used.
 */
enum _lv_border_side_t {
    LV_BORDER_SIDE_NONE     = 0x00,
    LV_BORDER_SIDE_BOTTOM   = 0x01,
    LV_BORDER_SIDE_TOP      = 0x02,
    LV_BORDER_SIDE_LEFT     = 0x04,
    LV_BORDER_SIDE_RIGHT    = 0x08,
    LV_BORDER_SIDE_FULL     = 0x0F,
    LV_BORDER_SIDE_INTERNAL = 0x10, /**< FOR matrix-like objects (e.g. Button matrix)*/
};

#ifdef DOXYGEN
typedef _lv_border_side_t lv_border_side_t;
#else
typedef uint8_t lv_border_side_t;
#endif /*DOXYGEN*/

/**
 * The direction of the gradient.
 */
enum _lv_grad_dir_t {
    LV_GRAD_DIR_NONE, /**< No gradient (the `grad_color` property is ignored)*/
    LV_GRAD_DIR_VER,  /**< Vertical (top to bottom) gradient*/
    LV_GRAD_DIR_HOR,  /**< Horizontal (left to right) gradient*/
};

#ifdef DOXYGEN
typedef _lv_grad_dir_t lv_grad_dir_t;
#else
typedef uint8_t lv_grad_dir_t;
#endif /*DOXYGEN*/

/** A gradient stop definition.
 *  This matches a color and a position in a virtual 0-255 scale.
 */
typedef struct {
    lv_color_t color;   /**< The stop color */
    lv_opa_t   opa;     /**< The opacity of the color*/
    uint8_t    frac;    /**< The stop position in 1/255 unit */
} lv_gradient_stop_t;

/** A descriptor of a gradient. */
typedef struct {
    lv_gradient_stop_t   stops[LV_GRADIENT_MAX_STOPS]; /**< A gradient stop array */
    uint8_t              stops_count;                  /**< The number of used stops in the array */
    lv_grad_dir_t        dir : 3;                      /**< The gradient direction.
                                                        * Any of LV_GRAD_DIR_HOR, LV_GRAD_DIR_VER, LV_GRAD_DIR_NONE */
} lv_grad_dsc_t;

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
enum _lv_style_prop_t {
    LV_STYLE_PROP_INV               = 0,

    /*Group 0*/
    LV_STYLE_WIDTH                  = 1,
    LV_STYLE_HEIGHT                 = 2,
    LV_STYLE_LENGTH                 = 3,

    LV_STYLE_MIN_WIDTH              = 4,
    LV_STYLE_MAX_WIDTH              = 5,
    LV_STYLE_MIN_HEIGHT             = 6,
    LV_STYLE_MAX_HEIGHT             = 7,

    LV_STYLE_X                      = 8,
    LV_STYLE_Y                      = 9,
    LV_STYLE_ALIGN                  = 10,

    LV_STYLE_RADIUS                 = 12,

    /*Group 1*/
    LV_STYLE_PAD_TOP                = 16,
    LV_STYLE_PAD_BOTTOM             = 17,
    LV_STYLE_PAD_LEFT               = 18,
    LV_STYLE_PAD_RIGHT              = 19,

    LV_STYLE_PAD_ROW                = 20,
    LV_STYLE_PAD_COLUMN             = 21,
    LV_STYLE_LAYOUT                 = 22,

    LV_STYLE_MARGIN_TOP             = 24,
    LV_STYLE_MARGIN_BOTTOM          = 25,
    LV_STYLE_MARGIN_LEFT            = 26,
    LV_STYLE_MARGIN_RIGHT           = 27,

    /*Group 2*/
    LV_STYLE_BG_COLOR               = 28,
    LV_STYLE_BG_OPA                 = 29,

    LV_STYLE_BG_GRAD_DIR            = 32,
    LV_STYLE_BG_MAIN_STOP           = 33,
    LV_STYLE_BG_GRAD_STOP           = 34,
    LV_STYLE_BG_GRAD_COLOR          = 35,

    LV_STYLE_BG_MAIN_OPA            = 36,
    LV_STYLE_BG_GRAD_OPA            = 37,
    LV_STYLE_BG_GRAD                = 38,
    LV_STYLE_BASE_DIR               = 39,

    LV_STYLE_BG_IMAGE_SRC             = 40,
    LV_STYLE_BG_IMAGE_OPA             = 41,
    LV_STYLE_BG_IMAGE_RECOLOR         = 42,
    LV_STYLE_BG_IMAGE_RECOLOR_OPA     = 43,

    LV_STYLE_BG_IMAGE_TILED           = 44,
    LV_STYLE_CLIP_CORNER            = 45,

    /*Group 3*/
    LV_STYLE_BORDER_WIDTH           = 48,
    LV_STYLE_BORDER_COLOR           = 49,
    LV_STYLE_BORDER_OPA             = 50,

    LV_STYLE_BORDER_SIDE            = 52,
    LV_STYLE_BORDER_POST            = 53,

    LV_STYLE_OUTLINE_WIDTH          = 56,
    LV_STYLE_OUTLINE_COLOR          = 57,
    LV_STYLE_OUTLINE_OPA            = 58,
    LV_STYLE_OUTLINE_PAD            = 59,

    /*Group 4*/
    LV_STYLE_SHADOW_WIDTH           = 60,
    LV_STYLE_SHADOW_COLOR           = 61,
    LV_STYLE_SHADOW_OPA             = 62,

    LV_STYLE_SHADOW_OFFSET_X        = 64,
    LV_STYLE_SHADOW_OFFSET_Y        = 65,
    LV_STYLE_SHADOW_SPREAD          = 66,

    LV_STYLE_IMAGE_OPA                = 68,
    LV_STYLE_IMAGE_RECOLOR            = 69,
    LV_STYLE_IMAGE_RECOLOR_OPA        = 70,

    LV_STYLE_LINE_WIDTH             = 72,
    LV_STYLE_LINE_DASH_WIDTH        = 73,
    LV_STYLE_LINE_DASH_GAP          = 74,
    LV_STYLE_LINE_ROUNDED           = 75,
    LV_STYLE_LINE_COLOR             = 76,
    LV_STYLE_LINE_OPA               = 77,

    /*Group 5*/
    LV_STYLE_ARC_WIDTH              = 80,
    LV_STYLE_ARC_ROUNDED            = 81,
    LV_STYLE_ARC_COLOR              = 82,
    LV_STYLE_ARC_OPA                = 83,
    LV_STYLE_ARC_IMAGE_SRC            = 84,

    LV_STYLE_TEXT_COLOR             = 88,
    LV_STYLE_TEXT_OPA               = 89,
    LV_STYLE_TEXT_FONT              = 90,

    LV_STYLE_TEXT_LETTER_SPACE      = 91,
    LV_STYLE_TEXT_LINE_SPACE        = 92,
    LV_STYLE_TEXT_DECOR             = 93,
    LV_STYLE_TEXT_ALIGN             = 94,

    LV_STYLE_OPA                    = 95,
    LV_STYLE_OPA_LAYERED            = 96,
    LV_STYLE_COLOR_FILTER_DSC       = 97,
    LV_STYLE_COLOR_FILTER_OPA       = 98,
    LV_STYLE_ANIM                   = 99,
    LV_STYLE_ANIM_DURATION          = 100,
    LV_STYLE_TRANSITION             = 102,
    LV_STYLE_BLEND_MODE             = 103,
    LV_STYLE_TRANSFORM_WIDTH        = 104,
    LV_STYLE_TRANSFORM_HEIGHT       = 105,
    LV_STYLE_TRANSLATE_X            = 106,
    LV_STYLE_TRANSLATE_Y            = 107,
    LV_STYLE_TRANSFORM_SCALE_X      = 108,
    LV_STYLE_TRANSFORM_SCALE_Y      = 109,
    LV_STYLE_TRANSFORM_ROTATION     = 110,
    LV_STYLE_TRANSFORM_PIVOT_X      = 111,
    LV_STYLE_TRANSFORM_PIVOT_Y      = 112,
    LV_STYLE_TRANSFORM_SKEW_X       = 113,
    LV_STYLE_TRANSFORM_SKEW_Y       = 114,
    LV_STYLE_BITMAP_MASK_SRC        = 115,
    LV_STYLE_ROTARY_SENSITIVITY     = 116,

#if LV_USE_FLEX
    LV_STYLE_FLEX_FLOW              = 125,
    LV_STYLE_FLEX_MAIN_PLACE        = 126,
    LV_STYLE_FLEX_CROSS_PLACE       = 127,
    LV_STYLE_FLEX_TRACK_PLACE       = 128,
    LV_STYLE_FLEX_GROW              = 129,
#endif

#if LV_USE_GRID
    LV_STYLE_GRID_COLUMN_ALIGN      = 130,
    LV_STYLE_GRID_ROW_ALIGN         = 131,
    LV_STYLE_GRID_ROW_DSC_ARRAY     = 132,
    LV_STYLE_GRID_COLUMN_DSC_ARRAY  = 133,
    LV_STYLE_GRID_CELL_COLUMN_POS   = 134,
    LV_STYLE_GRID_CELL_COLUMN_SPAN  = 135,
    LV_STYLE_GRID_CELL_X_ALIGN      = 136,
    LV_STYLE_GRID_CELL_ROW_POS      = 137,
    LV_STYLE_GRID_CELL_ROW_SPAN     = 138,
    LV_STYLE_GRID_CELL_Y_ALIGN      = 139,
#endif

    _LV_STYLE_LAST_BUILT_IN_PROP     = 140,

    _LV_STYLE_NUM_BUILT_IN_PROPS     = _LV_STYLE_LAST_BUILT_IN_PROP + 1,

    LV_STYLE_PROP_ANY                = 0xFF,
    _LV_STYLE_PROP_CONST             = 0xFF /* magic value for const styles */
};

#ifdef DOXYGEN
typedef _lv_style_prop_t lv_style_prop_t;
#else
typedef uint8_t lv_style_prop_t;
#endif /*DOXYGEN*/

enum _lv_style_res_t {
    LV_STYLE_RES_NOT_FOUND,
    LV_STYLE_RES_FOUND,
};

#ifdef DOXYGEN
typedef _lv_style_res_t lv_style_res_t;
#else
typedef uint8_t lv_style_res_t;
#endif /*DOXYGEN*/

/**
 * Descriptor for style transitions
 */
typedef struct {
    const lv_style_prop_t * props; /**< An array with the properties to animate.*/
    void * user_data;              /**< A custom user data that will be passed to the animation's user_data */
    lv_anim_path_cb_t path_xcb;     /**< A path for the animation.*/
    uint32_t time;                 /**< Duration of the transition in [ms]*/
    uint32_t delay;                /**< Delay before the transition in [ms]*/
} lv_style_transition_dsc_t;

/**
 * Descriptor of a constant style property.
 */
typedef struct {
    const lv_style_prop_t * prop_ptr;
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
 * @example
 * lv_style_prop_t MY_PROP;
 * static inline void lv_style_set_my_prop(lv_style_t * style, lv_color_t value) {
 * lv_style_value_t v = {.color = value}; lv_style_set_prop(style, MY_PROP, v); }
 *
 * ...
 * MY_PROP = lv_style_register_prop();
 * ...
 * lv_style_set_my_prop(&style1, lv_palette_main(LV_PALETTE_RED));
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
 * @example
 * const static lv_style_prop_t trans_props[] = { LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR, 0 };
 *  static lv_style_transition_dsc_t trans1;
 *  lv_style_transition_dsc_init(&trans1, trans_props, NULL, 300, 0, NULL);
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
        for(i = 0; props[i].prop_ptr; i++) {
            if(*props[i].prop_ptr == prop) {
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
static inline uint32_t _lv_style_get_prop_group(lv_style_prop_t prop)
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
uint8_t _lv_style_prop_lookup_flags(lv_style_prop_t prop);

#include "lv_style_gen.h"

static inline void lv_style_set_size(lv_style_t * style, int32_t width, int32_t height)
{
    lv_style_set_width(style, width);
    lv_style_set_height(style, height);
}

static inline void lv_style_set_pad_all(lv_style_t * style, int32_t value)
{
    lv_style_set_pad_left(style, value);
    lv_style_set_pad_right(style, value);
    lv_style_set_pad_top(style, value);
    lv_style_set_pad_bottom(style, value);
}

static inline void lv_style_set_pad_hor(lv_style_t * style, int32_t value)
{
    lv_style_set_pad_left(style, value);
    lv_style_set_pad_right(style, value);
}

static inline void lv_style_set_pad_ver(lv_style_t * style, int32_t value)
{
    lv_style_set_pad_top(style, value);
    lv_style_set_pad_bottom(style, value);
}

static inline void lv_style_set_pad_gap(lv_style_t * style, int32_t value)
{
    lv_style_set_pad_row(style, value);
    lv_style_set_pad_column(style, value);
}

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
    return _lv_style_prop_lookup_flags(prop) & flag;
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
