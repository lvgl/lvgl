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
#include "../font/lv_font.h"
#include "lv_color.h"
#include "lv_area.h"
#include "lv_anim.h"
#include "lv_txt.h"
#include "lv_types.h"
#include "lv_assert.h"

/*********************
 *      DEFINES
 *********************/

#define LV_STYLE_SENTINEL_VALUE     0xAABBCCDD

/**
 * Flags for style properties
 */
#define LV_STYLE_PROP_INHERIT       (1 << 10)  /*Inherited*/
#define LV_STYLE_PROP_EXT_DRAW      (1 << 11)  /*Requires ext. draw size update when changed*/
#define LV_STYLE_PROP_LAYOUT_REFR   (1 << 12)  /*Requires layout update when changed*/
#define LV_STYLE_PROP_FILTER        (1 << 13)  /*Apply color filter*/

/**
 * Other constants
 */
#define LV_IMG_ZOOM_NONE            256        /*Value for not zooming the image*/
LV_EXPORT_CONST_INT(LV_IMG_ZOOM_NONE);

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
 * A common type to handle all the property types in the same way.
 */
typedef union {
    int32_t num;         /**< Number integer number (opacity, enums, booleans or "normal" numbers)*/
    const void * ptr;    /**< Constant pointers  (font, cone text, etc)*/
    lv_color_t color;    /**< Colors*/
}lv_style_value_t;

/**
 * Enumeration of all built in style properties
 */
typedef enum {
    LV_STYLE_PROP_INV                = 0,

    /*Group 0*/
    LV_STYLE_RADIUS                  = 1,
    LV_STYLE_CLIP_CORNER             = 2,
    LV_STYLE_TRANSFORM_WIDTH         = 3 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_TRANSFORM_HEIGHT        = 4 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_TRANSFORM_ZOOM          = 5 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_TRANSFORM_ANGLE         = 6 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_OPA                     = 7 | LV_STYLE_PROP_INHERIT,

    LV_STYLE_COLOR_FILTER_DSC        = 8,
    LV_STYLE_COLOR_FILTER_OPA        = 9,
    LV_STYLE_ANIM_TIME               = 10,
    LV_STYLE_TRANSITION              = 11,
    LV_STYLE_SIZE                    = 12,
    LV_STYLE_BLEND_MODE              = 13,

    /*Group 1*/
    LV_STYLE_PAD_TOP                 = 16 | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_PAD_BOTTOM              = 17 | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_PAD_LEFT                = 18 | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_PAD_RIGHT               = 19 | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_PAD_ROW                 = 20 | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_PAD_COLUMN              = 21 | LV_STYLE_PROP_LAYOUT_REFR,

    /*Group 2*/
    LV_STYLE_BG_COLOR                = 32,
    LV_STYLE_BG_COLOR_FILTERED       = 32 | LV_STYLE_PROP_FILTER,
    LV_STYLE_BG_OPA                  = 33,
    LV_STYLE_BG_GRAD_COLOR           = 34,
    LV_STYLE_BG_GRAD_COLOR_FILTERED  = 34 | LV_STYLE_PROP_FILTER,
    LV_STYLE_BG_GRAD_DIR             = 35,
    LV_STYLE_BG_MAIN_STOP            = 36,
    LV_STYLE_BG_GRAD_STOP            = 37,

    LV_STYLE_BG_IMG_SRC              = 38,
    LV_STYLE_BG_IMG_OPA              = 39,
    LV_STYLE_BG_IMG_RECOLOR          = 40,
    LV_STYLE_BG_IMG_RECOLOR_FILTERED = 40 | LV_STYLE_PROP_FILTER,
    LV_STYLE_BG_IMG_RECOLOR_OPA      = 41,
    LV_STYLE_BG_IMG_TILED            = 42,


    /*Group 3*/
    LV_STYLE_BORDER_COLOR            = 48,
    LV_STYLE_BORDER_COLOR_FILTERED   = 48 | LV_STYLE_PROP_FILTER,
    LV_STYLE_BORDER_OPA              = 49,
    LV_STYLE_BORDER_WIDTH            = 50,
    LV_STYLE_BORDER_SIDE             = 51,
    LV_STYLE_BORDER_POST             = 52,

    LV_STYLE_OUTLINE_WIDTH           = 58 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_OUTLINE_COLOR           = 59,
    LV_STYLE_OUTLINE_COLOR_FILTERED  = 59 | LV_STYLE_PROP_FILTER,
    LV_STYLE_OUTLINE_OPA             = 60 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_OUTLINE_PAD             = 61 | LV_STYLE_PROP_EXT_DRAW,

    /*Group 4*/
    LV_STYLE_SHADOW_WIDTH            = 64 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_SHADOW_OFS_X            = 65 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_SHADOW_OFS_Y            = 66 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_SHADOW_SPREAD           = 67 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_SHADOW_COLOR            = 68,
    LV_STYLE_SHADOW_COLOR_FILTERED   = 68 | LV_STYLE_PROP_FILTER,
    LV_STYLE_SHADOW_OPA              = 69 | LV_STYLE_PROP_EXT_DRAW,

    LV_STYLE_IMG_OPA                 = 70,
    LV_STYLE_IMG_RECOLOR             = 71,
    LV_STYLE_IMG_RECOLOR_FILTERED    = 71 | LV_STYLE_PROP_FILTER,
    LV_STYLE_IMG_RECOLOR_OPA         = 72,

    LV_STYLE_LINE_WIDTH              = 73 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_LINE_DASH_WIDTH         = 74,
    LV_STYLE_LINE_DASH_GAP           = 75,
    LV_STYLE_LINE_ROUNDED            = 76,
    LV_STYLE_LINE_COLOR              = 77,
    LV_STYLE_LINE_COLOR_FILTERED     = 77 | LV_STYLE_PROP_FILTER,
    LV_STYLE_LINE_OPA                = 78,

    /*Group 5*/
    LV_STYLE_ARC_WIDTH               = 80 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_ARC_ROUNDED             = 81,
    LV_STYLE_ARC_COLOR               = 82,
    LV_STYLE_ARC_COLOR_FILTERED      = 82 | LV_STYLE_PROP_FILTER,
    LV_STYLE_ARC_OPA                 = 83,
    LV_STYLE_ARC_IMG_SRC             = 84,

    LV_STYLE_TEXT_COLOR              = 87 | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_COLOR_FILTERED     = 87 | LV_STYLE_PROP_INHERIT | LV_STYLE_PROP_FILTER,
    LV_STYLE_TEXT_OPA                = 88 | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_FONT               = 89 | LV_STYLE_PROP_INHERIT | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_TEXT_LETTER_SPACE       = 90 | LV_STYLE_PROP_INHERIT | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_TEXT_LINE_SPACE         = 91 | LV_STYLE_PROP_INHERIT | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_TEXT_DECOR              = 92 | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_ALIGN              = 93 | LV_STYLE_PROP_INHERIT,

    /*Group 6*/
    LV_STYLE_CONTENT_TEXT            = 96  | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_CONTENT_ALIGN           = 97  | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_CONTENT_OFS_X           = 98  | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_CONTENT_OFS_Y           = 99  | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_CONTENT_FONT            = 100 | LV_STYLE_PROP_EXT_DRAW | LV_STYLE_PROP_INHERIT,
    LV_STYLE_CONTENT_COLOR           = 101 | LV_STYLE_PROP_INHERIT,
    LV_STYLE_CONTENT_COLOR_FILTERED  = 101 | LV_STYLE_PROP_INHERIT | LV_STYLE_PROP_FILTER,
    LV_STYLE_CONTENT_OPA             = 102 | LV_STYLE_PROP_EXT_DRAW | LV_STYLE_PROP_INHERIT,
    LV_STYLE_CONTENT_LETTER_SPACE    = 103 | LV_STYLE_PROP_EXT_DRAW | LV_STYLE_PROP_INHERIT,
    LV_STYLE_CONTENT_LINE_SPACE      = 104 | LV_STYLE_PROP_EXT_DRAW | LV_STYLE_PROP_INHERIT,
    LV_STYLE_CONTENT_DECOR           = 105 | LV_STYLE_PROP_EXT_DRAW | LV_STYLE_PROP_INHERIT,

    _LV_STYLE_LAST_BUILT_IN_PROP     = 111,

    LV_STYLE_PROP_ALL                = 0xFFFF
}lv_style_prop_t;

/**
 * Descriptor for style transitions
 */
typedef struct _lv_style_transiton_t{
    const lv_style_prop_t * props; /**< An array with the properties to animate.*/
    const lv_anim_path_t * path;   /**< A path for the animation.*/
    uint32_t time;                 /**< Duration of the transition in [ms]*/
    uint32_t delay;                /**< Delay before the transition in [ms]*/
}lv_style_transition_dsc_t;

#if LV_USE_ASSERT_STYLE
#  define _LV_STYLE_SENTINEL uint32_t sentinel;
#else
#  define _LV_STYLE_SENTINEL
#endif

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
 * @note Do not call `lv_style_init` on styles that are already have some properties
 *       because this function won't free the used memory just set a default state for the style.
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
 * lv_style_set_my_prop(&style1, lv_color_red());
 */
lv_style_prop_t lv_style_register_prop(void);

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
 * @param prop the ID of a property (e.g. `LV_STLYE_BG_COLOR`)
 * @param value `lv_style_value_t` variable in which a filed is set according to the type of `prop`
 */
void lv_style_set_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t value);

/**
 * Get the value of a property
 * @param style pointer to a style
 * @param prop  the ID of a property
 * @param value pointer to a `lv_style_value_t` variable to store the value
 * @return false: the property wsn't found in the style (`value` is unchanged)
 *         true: the property was fond, and `value` is set accordingly
 * @note For performance reasons there are no sanity check on `style`
 */
bool lv_style_get_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value);


/**
 * Get the value of a property
 * @param style pointer to a style
 * @param prop  the ID of a property
 * @param value pointer to a `lv_style_value_t` variable to store the value
 * @return false: the property wsn't found in the style (`value` is unchanged)
 *         true: the property was fond, and `value` is set accordingly
 * @note For performance reasons there are no sanity check on `style`
 * @note This function is the same as ::lv_style_get_prop but inlined. Use it only on performance critical places
 */
static inline bool lv_style_get_prop_inlined(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value)
{
    if(style->prop_cnt == 0) return false;

    if(style->prop_cnt > 1) {
        uint8_t * tmp = style->v_p.values_and_props + style->prop_cnt * sizeof(lv_style_value_t);
        uint16_t * props = (uint16_t *)tmp;
        uint32_t i;
        for(i = 0; i < style->prop_cnt; i++) {
            if(props[i] == prop) {
                lv_style_value_t * values = (lv_style_value_t *)style->v_p.values_and_props;
                *value = values[i];
                return true;
            }
        }
    } else if(style->prop1 == prop) {
        *value = style->v_p.value1;
        return true;
    }
    return false;
}

/**
 * Initialize a transition descriptor.
 * @param tr    pointer to a transition descriptor to initialize
 * @param props an array with the properties to transition. The last element must be zero.
 * @param path  and animation path. If `NULL` a default liner path will be used.
 * @param time  duration of the transition in [ms]
 * @param delay delay before the transition in [ms]
 * @example
 * const static lv_style_prop_t trans_props[] = { LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR, 0 };
 *  static lv_style_transition_dsc_t trans1;
 *  lv_style_transition_dsc_init(&trans1, trans_props, NULL, 300, 0);
 * @note For performance reasons there are no sanity check on `style`
 */
void lv_style_transition_dsc_init(lv_style_transition_dsc_t * tr, const lv_style_prop_t * props, const lv_anim_path_t * path, uint32_t time, uint32_t delay);

/**
 * Get the default value of a property
 * @param prop the ID of a property
 * @return the default value
 */
lv_style_value_t lv_style_prop_get_default(lv_style_prop_t prop);

/**
 * Checks if a style is empty (has no properties)
 * @param style pointer to a style
 * @return
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

static inline void lv_style_set_pad_all(lv_style_t * style, lv_coord_t value) {
    lv_style_set_pad_left(style, value);
    lv_style_set_pad_right(style, value);
    lv_style_set_pad_top(style, value);
    lv_style_set_pad_bottom(style, value);
}

static inline void lv_style_set_pad_hor(lv_style_t * style, lv_coord_t value) {
    lv_style_set_pad_left(style, value);
    lv_style_set_pad_right(style, value);
}

static inline void lv_style_set_pad_ver(lv_style_t * style, lv_coord_t value) {
    lv_style_set_pad_top(style, value);
    lv_style_set_pad_bottom(style, value);
}

static inline void lv_style_set_pad_gap(lv_style_t * style, lv_coord_t value) {
    lv_style_set_pad_row(style, value);
    lv_style_set_pad_column(style, value);
}


/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#if LV_USE_ASSERT_STYLE
#  define LV_ASSERT_STYLE(style_p)    LV_ASSERT_MSG(style_p != NULL, "The style is NULL");          \
                                      LV_ASSERT_MSG(style_p->sentinel == LV_STYLE_SENTINEL_VALUE, "Style is not initialized or corrupted");
#else
# define LV_ASSERT_STYLE(p)
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_STYLE_H*/
