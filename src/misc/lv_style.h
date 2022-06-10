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
#include "lv_txt.h"
#include "lv_types.h"
#include "lv_assert.h"
#include "lv_bidi.h"

/*********************
 *      DEFINES
 *********************/

#define LV_STYLE_SENTINEL_VALUE     0xAABBCCDD

/**
 * Flags for style behavior
 *
 * The rest of the flags will have _FLAG added to their name in v9.
 */
#define LV_STYLE_PROP_FLAG_NONE             (0)
#define LV_STYLE_PROP_INHERIT               (1 << 0)  /*Inherited*/
#define LV_STYLE_PROP_EXT_DRAW              (1 << 1)  /*Requires ext. draw size update when changed*/
#define LV_STYLE_PROP_LAYOUT_REFR           (1 << 2)  /*Requires layout update when changed*/
#define LV_STYLE_PROP_PARENT_LAYOUT_REFR    (1 << 3)  /*Requires layout update on parent when changed*/
#define LV_STYLE_PROP_LAYER_REFR            (1 << 4)  /*Affects layer handling*/
#define LV_STYLE_PROP_ALL                   (0x1F)     /*Indicating all flags*/

/**
 * Other constants
 */
#define LV_IMG_ZOOM_NONE            256        /*Value for not zooming the image*/
LV_EXPORT_CONST_INT(LV_IMG_ZOOM_NONE);

#if LV_USE_ASSERT_STYLE
#define LV_STYLE_CONST_INIT(var_name, prop_array) const lv_style_t var_name = { .sentinel = LV_STYLE_SENTINEL_VALUE, .v_p = { .const_props = prop_array }, .has_group = 0xFF, .prop1 = LV_STYLE_PROP_ANY }
#else
#define LV_STYLE_CONST_INIT(var_name, prop_array) const lv_style_t var_name = { .v_p = { .const_props = prop_array }, .has_group = 0xFF, .prop1 = LV_STYLE_PROP_ANY }
#endif

/** On simple system, don't waste resources on gradients */
#if !defined(LV_DRAW_COMPLEX) || !defined(LV_GRADIENT_MAX_STOPS)
#define LV_GRADIENT_MAX_STOPS 2
#endif

#define LV_STYLE_PROP_META_INHERIT 0x8000
#define LV_STYLE_PROP_META_INITIAL 0x4000
#define LV_STYLE_PROP_META_MASK (LV_STYLE_PROP_META_INHERIT | LV_STYLE_PROP_META_INITIAL)

#define LV_STYLE_PROP_ID_MASK(prop) ((lv_style_prop_t)((prop) & ~LV_STYLE_PROP_META_MASK))

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Possible options how to blend opaque drawings
 */
enum {
    LV_BLEND_MODE_NORMAL,     /**< Simply mix according to the opacity value*/
    LV_BLEND_MODE_ADDITIVE,   /**< Add the respective color channels*/
    LV_BLEND_MODE_SUBTRACTIVE,/**< Subtract the foreground from the background*/
    LV_BLEND_MODE_MULTIPLY,   /**< Multiply the foreground and background*/
    LV_BLEND_MODE_REPLACE,    /**< Replace background with foreground in the area*/
};

typedef uint8_t lv_blend_mode_t;

/**
 * Some options to apply decorations on texts.
 * 'OR'ed values can be used.
 */
enum {
    LV_TEXT_DECOR_NONE          = 0x00,
    LV_TEXT_DECOR_UNDERLINE     = 0x01,
    LV_TEXT_DECOR_STRIKETHROUGH = 0x02,
};

typedef uint8_t lv_text_decor_t;

/**
 * Selects on which sides border should be drawn
 * 'OR'ed values can be used.
 */
enum {
    LV_BORDER_SIDE_NONE     = 0x00,
    LV_BORDER_SIDE_BOTTOM   = 0x01,
    LV_BORDER_SIDE_TOP      = 0x02,
    LV_BORDER_SIDE_LEFT     = 0x04,
    LV_BORDER_SIDE_RIGHT    = 0x08,
    LV_BORDER_SIDE_FULL     = 0x0F,
    LV_BORDER_SIDE_INTERNAL = 0x10, /**< FOR matrix-like objects (e.g. Button matrix)*/
};
typedef uint8_t lv_border_side_t;

/**
 * The direction of the gradient.
 */
enum {
    LV_GRAD_DIR_NONE, /**< No gradient (the `grad_color` property is ignored)*/
    LV_GRAD_DIR_VER,  /**< Vertical (top to bottom) gradient*/
    LV_GRAD_DIR_HOR,  /**< Horizontal (left to right) gradient*/
};

typedef uint8_t lv_grad_dir_t;

/**
 * The dithering algorithm for the gradient
 * Depends on LV_DITHER_GRADIENT
 */
enum {
    LV_DITHER_NONE,     /**< No dithering, colors are just quantized to the output resolution*/
    LV_DITHER_ORDERED,  /**< Ordered dithering. Faster to compute and use less memory but lower quality*/
    LV_DITHER_ERR_DIFF, /**< Error diffusion mode. Slower to compute and use more memory but give highest dither quality*/
};

typedef uint8_t lv_dither_mode_t;

/** A gradient stop definition.
 *  This matches a color and a position in a virtual 0-255 scale.
 */
typedef struct {
    lv_color_t color;   /**< The stop color */
    uint8_t    frac;    /**< The stop position in 1/255 unit */
} lv_gradient_stop_t;

/** A descriptor of a gradient. */
typedef struct {
    lv_gradient_stop_t   stops[LV_GRADIENT_MAX_STOPS]; /**< A gradient stop array */
    uint8_t              stops_count;                  /**< The number of used stops in the array */
    lv_grad_dir_t        dir : 3;                      /**< The gradient direction.
                                                        * Any of LV_GRAD_DIR_HOR, LV_GRAD_DIR_VER, LV_GRAD_DIR_NONE */
    lv_dither_mode_t     dither : 3;                   /**< Whether to dither the gradient or not.
                                                        * Any of LV_DITHER_NONE, LV_DITHER_ORDERED, LV_DITHER_ERR_DIFF */
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
typedef enum {
    LV_STYLE_PROP_INV               = 0,

    /*Group 0*/
    LV_STYLE_WIDTH                  = 1,
    LV_STYLE_MIN_WIDTH              = 2,
    LV_STYLE_MAX_WIDTH              = 3,
    LV_STYLE_HEIGHT                 = 4,
    LV_STYLE_MIN_HEIGHT             = 5,
    LV_STYLE_MAX_HEIGHT             = 6,
    LV_STYLE_X                      = 7,
    LV_STYLE_Y                      = 8,
    LV_STYLE_ALIGN                  = 9,
    LV_STYLE_LAYOUT                 = 10,
    LV_STYLE_RADIUS                 = 11,

    /*Group 1*/
    LV_STYLE_PAD_TOP                = 16,
    LV_STYLE_PAD_BOTTOM             = 17,
    LV_STYLE_PAD_LEFT               = 18,
    LV_STYLE_PAD_RIGHT              = 19,
    LV_STYLE_PAD_ROW                = 20,
    LV_STYLE_PAD_COLUMN             = 21,
    LV_STYLE_BASE_DIR               = 22,
    LV_STYLE_CLIP_CORNER            = 23,

    /*Group 2*/
    LV_STYLE_BG_COLOR               = 32,
    LV_STYLE_BG_OPA                 = 33,
    LV_STYLE_BG_GRAD_COLOR          = 34,
    LV_STYLE_BG_GRAD_DIR            = 35,
    LV_STYLE_BG_MAIN_STOP           = 36,
    LV_STYLE_BG_GRAD_STOP           = 37,
    LV_STYLE_BG_GRAD                = 38,
    LV_STYLE_BG_DITHER_MODE         = 39,
    LV_STYLE_BG_IMG_SRC             = 40,
    LV_STYLE_BG_IMG_OPA             = 41,
    LV_STYLE_BG_IMG_RECOLOR         = 42,
    LV_STYLE_BG_IMG_RECOLOR_OPA     = 43,
    LV_STYLE_BG_IMG_TILED           = 44,

    /*Group 3*/
    LV_STYLE_BORDER_COLOR           = 48,
    LV_STYLE_BORDER_OPA             = 49,
    LV_STYLE_BORDER_WIDTH           = 50,
    LV_STYLE_BORDER_SIDE            = 51,
    LV_STYLE_BORDER_POST            = 52,
    LV_STYLE_OUTLINE_WIDTH          = 53,
    LV_STYLE_OUTLINE_COLOR          = 54,
    LV_STYLE_OUTLINE_OPA            = 55,
    LV_STYLE_OUTLINE_PAD            = 56,

    /*Group 4*/
    LV_STYLE_SHADOW_WIDTH           = 64,
    LV_STYLE_SHADOW_OFS_X           = 65,
    LV_STYLE_SHADOW_OFS_Y           = 66,
    LV_STYLE_SHADOW_SPREAD          = 67,
    LV_STYLE_SHADOW_COLOR           = 68,
    LV_STYLE_SHADOW_OPA             = 69,
    LV_STYLE_IMG_OPA                = 70,
    LV_STYLE_IMG_RECOLOR            = 71,
    LV_STYLE_IMG_RECOLOR_OPA        = 72,
    LV_STYLE_LINE_WIDTH             = 73,
    LV_STYLE_LINE_DASH_WIDTH        = 74,
    LV_STYLE_LINE_DASH_GAP          = 75,
    LV_STYLE_LINE_ROUNDED           = 76,
    LV_STYLE_LINE_COLOR             = 77,
    LV_STYLE_LINE_OPA               = 78,

    /*Group 5*/
    LV_STYLE_ARC_WIDTH              = 80,
    LV_STYLE_ARC_ROUNDED            = 81,
    LV_STYLE_ARC_COLOR              = 82,
    LV_STYLE_ARC_OPA                = 83,
    LV_STYLE_ARC_IMG_SRC            = 84,
    LV_STYLE_TEXT_COLOR             = 85,
    LV_STYLE_TEXT_OPA               = 86,
    LV_STYLE_TEXT_FONT              = 87,
    LV_STYLE_TEXT_LETTER_SPACE      = 88,
    LV_STYLE_TEXT_LINE_SPACE        = 89,
    LV_STYLE_TEXT_DECOR             = 90,
    LV_STYLE_TEXT_ALIGN             = 91,

    /*Group 6*/
    LV_STYLE_OPA                    = 96,
    LV_STYLE_COLOR_FILTER_DSC       = 97,
    LV_STYLE_COLOR_FILTER_OPA       = 98,
    LV_STYLE_ANIM                   = 99,
    LV_STYLE_ANIM_TIME              = 100,
    LV_STYLE_ANIM_SPEED             = 101,
    LV_STYLE_TRANSITION             = 102,
    LV_STYLE_BLEND_MODE             = 103,
    LV_STYLE_TRANSFORM_WIDTH        = 104,
    LV_STYLE_TRANSFORM_HEIGHT       = 105,
    LV_STYLE_TRANSLATE_X            = 106,
    LV_STYLE_TRANSLATE_Y            = 107,
    LV_STYLE_TRANSFORM_ZOOM         = 108,
    LV_STYLE_TRANSFORM_ANGLE        = 109,
    LV_STYLE_TRANSFORM_PIVOT_X      = 110,
    LV_STYLE_TRANSFORM_PIVOT_Y      = 111,

    _LV_STYLE_LAST_BUILT_IN_PROP     = 111,
    _LV_STYLE_NUM_BUILT_IN_PROPS     = _LV_STYLE_LAST_BUILT_IN_PROP + 1,

    LV_STYLE_PROP_ANY                = 0xFFFF,
    _LV_STYLE_PROP_CONST             = 0xFFFF /* magic value for const styles */
} lv_style_prop_t;

enum {
    LV_STYLE_RES_NOT_FOUND,
    LV_STYLE_RES_FOUND,
    LV_STYLE_RES_INHERIT
};

typedef uint8_t lv_style_res_t;

/**
 * Descriptor for style transitions
 */
typedef struct {
    const lv_style_prop_t * props; /**< An array with the properties to animate.*/
#if LV_USE_USER_DATA
    void * user_data;              /**< A custom user data that will be passed to the animation's user_data */
#endif
    lv_anim_path_cb_t path_xcb;     /**< A path for the animation.*/
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

    /*If there is only one property store it directly.
     *For more properties allocate an array*/
    union {
        lv_style_value_t value1;
        uint8_t * values_and_props;
        const lv_style_const_prop_t * const_props;
    } v_p;

    uint16_t prop1;
    uint8_t has_group;
    uint8_t prop_cnt;
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
 * Set a special meta state for a property in a style.
 * This function shouldn't be used directly by the user.
 * @param style pointer to style
 * @param prop the ID of a property (e.g. `LV_STYLE_BG_COLOR`)
 * @param meta the meta value to attach to the property in the style
 */
void lv_style_set_prop_meta(lv_style_t * style, lv_style_prop_t prop, uint16_t meta);

/**
 * Get the value of a property
 * @param style pointer to a style
 * @param prop  the ID of a property
 * @param value pointer to a `lv_style_value_t` variable to store the value
 * @return LV_RES_INV: the property wasn't found in the style (`value` is unchanged)
 *         LV_RES_OK: the property was fond, and `value` is set accordingly
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
 * @return LV_RES_INV: the property wasn't found in the style (`value` is unchanged)
 *         LV_RES_OK: the property was fond, and `value` is set accordingly
 * @note For performance reasons there are no sanity check on `style`
 * @note This function is the same as ::lv_style_get_prop but inlined. Use it only on performance critical places
 */
static inline lv_style_res_t lv_style_get_prop_inlined(const lv_style_t * style, lv_style_prop_t prop,
                                                       lv_style_value_t * value)
{
    if(style->prop1 == LV_STYLE_PROP_ANY) {
        const lv_style_const_prop_t * const_prop;
        for(const_prop = style->v_p.const_props; const_prop->prop != LV_STYLE_PROP_INV; const_prop++) {
            lv_style_prop_t prop_id = LV_STYLE_PROP_ID_MASK(const_prop->prop);
            if(prop_id == prop) {
                if(const_prop->prop & LV_STYLE_PROP_META_INHERIT)
                    return LV_STYLE_RES_INHERIT;
                *value = (const_prop->prop & LV_STYLE_PROP_META_INITIAL) ? lv_style_prop_get_default(prop_id) : const_prop->value;
                return LV_STYLE_RES_FOUND;
            }
        }
        return LV_STYLE_RES_NOT_FOUND;
    }

    if(style->prop_cnt == 0) return LV_STYLE_RES_NOT_FOUND;

    if(style->prop_cnt > 1) {
        uint8_t * tmp = style->v_p.values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
        uint16_t * props = (uint16_t *)tmp;
        uint32_t i;
        for(i = 0; i < style->prop_cnt; i++) {
            lv_style_prop_t prop_id = LV_STYLE_PROP_ID_MASK(props[i]);
            if(prop_id == prop) {
                if(props[i] & LV_STYLE_PROP_META_INHERIT)
                    return LV_STYLE_RES_INHERIT;
                if(props[i] & LV_STYLE_PROP_META_INITIAL)
                    *value = lv_style_prop_get_default(prop_id);
                else {
                    lv_style_value_t * values = (lv_style_value_t *)style->v_p.values_and_props;
                    *value = values[i];
                }
                return LV_STYLE_RES_FOUND;
            }
        }
    }
    else if(LV_STYLE_PROP_ID_MASK(style->prop1) == prop) {
        if(style->prop1 & LV_STYLE_PROP_META_INHERIT)
            return LV_STYLE_RES_INHERIT;
        *value = (style->prop1 & LV_STYLE_PROP_META_INITIAL) ? lv_style_prop_get_default(LV_STYLE_PROP_ID_MASK(
                                                                                             style->prop1)) : style->v_p.value1;
        return LV_STYLE_RES_FOUND;
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
 * @return the group [0..7] 7 means all the custom properties with index > 112
 */
uint8_t _lv_style_get_prop_group(lv_style_prop_t prop);

/**
 * Get the flags of a built-in or custom property.
 *
 * @param prop a style property
 * @return the flags of the property
 */
uint8_t _lv_style_prop_lookup_flags(lv_style_prop_t prop);

#include "lv_style_gen.h"

static inline void lv_style_set_size(lv_style_t * style, lv_coord_t value)
{
    lv_style_set_width(style, value);
    lv_style_set_height(style, value);
}

static inline void lv_style_set_pad_all(lv_style_t * style, lv_coord_t value)
{
    lv_style_set_pad_left(style, value);
    lv_style_set_pad_right(style, value);
    lv_style_set_pad_top(style, value);
    lv_style_set_pad_bottom(style, value);
}

static inline void lv_style_set_pad_hor(lv_style_t * style, lv_coord_t value)
{
    lv_style_set_pad_left(style, value);
    lv_style_set_pad_right(style, value);
}

static inline void lv_style_set_pad_ver(lv_style_t * style, lv_coord_t value)
{
    lv_style_set_pad_top(style, value);
    lv_style_set_pad_bottom(style, value);
}

static inline void lv_style_set_pad_gap(lv_style_t * style, lv_coord_t value)
{
    lv_style_set_pad_row(style, value);
    lv_style_set_pad_column(style, value);
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
