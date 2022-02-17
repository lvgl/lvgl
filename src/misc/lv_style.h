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
 */
#define LV_STYLE_PROP_INHERIT               (1 << 0)  /*Inherited*/
#define LV_STYLE_PROP_EXT_DRAW              (1 << 1)  /*Requires ext. draw size update when changed*/
#define LV_STYLE_PROP_LAYOUT_REFR           (1 << 2)  /*Requires layout update when changed*/
#define LV_STYLE_PROP_PARENT_LAYOUT_REFR    (1 << 3)  /*Requires layout update on parent when changed*/

/**
 * Other constants
 */
#define LV_IMG_ZOOM_NONE            256        /*Value for not zooming the image*/
LV_EXPORT_CONST_INT(LV_IMG_ZOOM_NONE);

#if LV_USE_ASSERT_STYLE
#define LV_STYLE_CONST_INIT(var_name, prop_array) const lv_style_t var_name = { .sentinel = LV_STYLE_SENTINEL_VALUE, .v_p = { .const_props = prop_array }, .has_group = 0xFF, .is_const = 1 }
#else
#define LV_STYLE_CONST_INIT(var_name, prop_array) const lv_style_t var_name = { .v_p = { .const_props = prop_array }, .has_group = 0xFF, .is_const = 1 }
#endif

/** On simple system, don't waste resources on gradients */
#if !defined(LV_DRAW_COMPLEX) || !defined(LV_GRADIENT_MAX_STOPS)
#define LV_GRADIENT_MAX_STOPS 2
#endif


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
 */
typedef enum {
    LV_STYLE_PROP_INV               ,

    /*Group 0*/
    LV_STYLE_WIDTH                  ,
    LV_STYLE_MIN_WIDTH              ,
    LV_STYLE_MAX_WIDTH              ,
    LV_STYLE_HEIGHT                 ,
    LV_STYLE_MIN_HEIGHT             ,
    LV_STYLE_MAX_HEIGHT             ,
    LV_STYLE_X                      ,
    LV_STYLE_Y                      ,
    LV_STYLE_ALIGN                  ,
    LV_STYLE_TRANSFORM_WIDTH        ,
    LV_STYLE_TRANSFORM_HEIGHT       ,
    LV_STYLE_TRANSLATE_X            ,
    LV_STYLE_TRANSLATE_Y            ,
    LV_STYLE_TRANSFORM_ZOOM         ,
    LV_STYLE_TRANSFORM_ANGLE        ,

    /*Group 1*/
    LV_STYLE_PAD_TOP                ,
    LV_STYLE_PAD_BOTTOM             ,
    LV_STYLE_PAD_LEFT               ,
    LV_STYLE_PAD_RIGHT              ,
    LV_STYLE_PAD_ROW                ,
    LV_STYLE_PAD_COLUMN             ,

    /*Group 2*/
    LV_STYLE_BG_COLOR               ,
    LV_STYLE_BG_OPA                 ,
    LV_STYLE_BG_GRAD_COLOR          ,
    LV_STYLE_BG_GRAD_DIR            ,
    LV_STYLE_BG_MAIN_STOP           ,
    LV_STYLE_BG_GRAD_STOP           ,
    LV_STYLE_BG_GRAD                ,
    LV_STYLE_BG_DITHER_MODE         ,


    LV_STYLE_BG_IMG_SRC             ,
    LV_STYLE_BG_IMG_OPA             ,
    LV_STYLE_BG_IMG_RECOLOR         ,
    LV_STYLE_BG_IMG_RECOLOR_OPA     ,
    LV_STYLE_BG_IMG_TILED           ,

    /*Group 3*/
    LV_STYLE_BORDER_COLOR           ,
    LV_STYLE_BORDER_OPA             ,
    LV_STYLE_BORDER_WIDTH           ,
    LV_STYLE_BORDER_SIDE            ,
    LV_STYLE_BORDER_POST            ,

    LV_STYLE_OUTLINE_WIDTH          ,
    LV_STYLE_OUTLINE_COLOR          ,
    LV_STYLE_OUTLINE_OPA            ,
    LV_STYLE_OUTLINE_PAD            ,

    /*Group 4*/
    LV_STYLE_SHADOW_WIDTH           ,
    LV_STYLE_SHADOW_OFS_X           ,
    LV_STYLE_SHADOW_OFS_Y           ,
    LV_STYLE_SHADOW_SPREAD          ,
    LV_STYLE_SHADOW_COLOR           ,
    LV_STYLE_SHADOW_OPA             ,

    LV_STYLE_IMG_OPA                ,
    LV_STYLE_IMG_RECOLOR            ,
    LV_STYLE_IMG_RECOLOR_OPA        ,

    LV_STYLE_LINE_WIDTH             ,
    LV_STYLE_LINE_DASH_WIDTH        ,
    LV_STYLE_LINE_DASH_GAP          ,
    LV_STYLE_LINE_ROUNDED           ,
    LV_STYLE_LINE_COLOR             ,
    LV_STYLE_LINE_OPA               ,

    /*Group 5*/
    LV_STYLE_ARC_WIDTH              ,
    LV_STYLE_ARC_ROUNDED            ,
    LV_STYLE_ARC_COLOR              ,
    LV_STYLE_ARC_OPA                ,
    LV_STYLE_ARC_IMG_SRC            ,

    LV_STYLE_TEXT_COLOR             ,
    LV_STYLE_TEXT_OPA               ,
    LV_STYLE_TEXT_FONT              ,
    LV_STYLE_TEXT_LETTER_SPACE      ,
    LV_STYLE_TEXT_LINE_SPACE        ,
    LV_STYLE_TEXT_DECOR             ,
    LV_STYLE_TEXT_ALIGN             ,

    /*Group 6*/
    LV_STYLE_RADIUS                 ,
    LV_STYLE_CLIP_CORNER            ,
    LV_STYLE_OPA                    ,
    LV_STYLE_COLOR_FILTER_DSC       ,
    LV_STYLE_COLOR_FILTER_OPA       ,
    LV_STYLE_ANIM_TIME              ,
    LV_STYLE_ANIM_SPEED             ,
    LV_STYLE_TRANSITION             ,
    LV_STYLE_BLEND_MODE             ,
    LV_STYLE_LAYOUT                 ,
    LV_STYLE_BASE_DIR               ,

    _LV_STYLE_LAST_BUILT_IN_PROP     = LV_STYLE_BASE_DIR + 10,

    LV_STYLE_PROP_ANY                = 0xFFFF
} lv_style_prop_t;

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

    uint16_t prop1 : 15;
    uint16_t is_const : 1;
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
 * @return a new property ID.
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
 * @return LV_RES_INV: the property wasn't found in the style (`value` is unchanged)
 *         LV_RES_OK: the property was fond, and `value` is set accordingly
 * @note For performance reasons there are no sanity check on `style`
 */
lv_res_t lv_style_get_prop(const lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value);


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
static inline lv_res_t lv_style_get_prop_inlined(const lv_style_t * style, lv_style_prop_t prop,
                                                 lv_style_value_t * value)
{
    if(style->is_const) {
        const lv_style_const_prop_t * const_prop;
        for(const_prop = style->v_p.const_props; const_prop->prop != LV_STYLE_PROP_INV; const_prop++) {
            if(const_prop->prop == prop) {
                *value = const_prop->value;
                return LV_RES_OK;
            }
        }
        return LV_RES_INV;
    }

    if(style->prop_cnt == 0) return LV_RES_INV;

    if(style->prop_cnt > 1) {
        uint8_t * tmp = style->v_p.values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
        uint16_t * props = (uint16_t *)tmp;
        uint32_t i;
        for(i = 0; i < style->prop_cnt; i++) {
            if(props[i] == prop) {
                lv_style_value_t * values = (lv_style_value_t *)style->v_p.values_and_props;
                *value = values[i];
                return LV_RES_OK;
            }
        }
    }
    else if(style->prop1 == prop) {
        *value = style->v_p.value1;
        return LV_RES_OK;
    }
    return LV_RES_INV;
}

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

static inline uint8_t lv_style_lookup_flags(lv_style_prop_t prop)
{
    extern uint8_t lv_style_lookup_table[];
#warning do not merge this code until this magic number is removed
    if(prop < 256)
        return lv_style_lookup_table[prop];
    return 0;
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
