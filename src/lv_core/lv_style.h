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
#include "../lv_font/lv_font.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_types.h"
#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw_blend.h"

/*********************
 *      DEFINES
 *********************/

#define LV_RADIUS_CIRCLE (0x7FFF) /**< A very big radius to always draw as circle*/
LV_EXPORT_CONST_INT(LV_RADIUS_CIRCLE);

#define LV_DEBUG_STYLE_SENTINEL_VALUE      0x2288AAEE
#define LV_DEBUG_STYLE_LIST_SENTINEL_VALUE 0x9977CCBB

#define LV_STYLE_PROP_INIT(name, group, id, attr)  name = (((group << 4) + id) | ((attr) << 8))

#define LV_STYLE_ID_MASK 0x00FF

#define LV_STYLE_ATTR_NONE          0
#define LV_STYLE_ATTR_INHERIT       (1 << 7)

#define _LV_STYLE_CLOSING_PROP     0xFF

#define LV_STYLE_TRANS_NUM_MAX      6

#define LV_STYLE_PROP_ALL 0xFF

/**********************
 *      TYPEDEFS
 **********************/

/*Border types (Use 'OR'ed values)*/
enum {
    LV_BORDER_SIDE_NONE     = 0x00,
    LV_BORDER_SIDE_BOTTOM   = 0x01,
    LV_BORDER_SIDE_TOP      = 0x02,
    LV_BORDER_SIDE_LEFT     = 0x04,
    LV_BORDER_SIDE_RIGHT    = 0x08,
    LV_BORDER_SIDE_FULL     = 0x0F,
    LV_BORDER_SIDE_INTERNAL = 0x10, /**< FOR matrix-like objects (e.g. Button matrix)*/
    _LV_BORDER_SIDE_LAST
};
typedef uint8_t lv_border_side_t;

enum {
    LV_GRAD_DIR_NONE,
    LV_GRAD_DIR_VER,
    LV_GRAD_DIR_HOR,
    _LV_GRAD_DIR_LAST
};

typedef uint8_t lv_grad_dir_t;

/*Text decorations (Use 'OR'ed values)*/
enum {
    LV_TEXT_DECOR_NONE          = 0x00,
    LV_TEXT_DECOR_UNDERLINE     = 0x01,
    LV_TEXT_DECOR_STRIKETHROUGH = 0x02,
    _LV_TEXT_DECOR_LAST
};

typedef uint8_t lv_text_decor_t;

typedef uint8_t lv_style_attr_t;

#define LV_STYLE_ATTR_GET_INHERIT(f) ((f)&0x80)
#define LV_STYLE_ATTR_GET_STATE(f) ((f)&0x7F)

#define LV_STYLE_ID_VALUE 0x0   /*max 9 pcs*/
#define LV_STYLE_ID_COLOR 0x9   /*max 3 pcs*/
#define LV_STYLE_ID_OPA   0xC   /*max 2 pcs*/
#define LV_STYLE_ID_PTR   0xE   /*max 2 pcs*/

enum {
    /*Skip 0th property*/
    LV_STYLE_PROP_INIT(LV_STYLE_RADIUS,             0x0, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_CLIP_CORNER,        0x0, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SIZE,               0x0, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSFORM_WIDTH,    0x0, LV_STYLE_ID_VALUE + 4, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSFORM_HEIGHT,   0x0, LV_STYLE_ID_VALUE + 5, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSFORM_ANGLE,    0x0, LV_STYLE_ID_VALUE + 6, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSFORM_ZOOM,     0x0, LV_STYLE_ID_VALUE + 7, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_OPA_SCALE,          0x0, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_INHERIT),

    LV_STYLE_PROP_INIT(LV_STYLE_PAD_TOP,            0x1, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PAD_BOTTOM,         0x1, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PAD_LEFT,           0x1, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PAD_RIGHT,          0x1, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PAD_INNER,          0x1, LV_STYLE_ID_VALUE + 4, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_MARGIN_TOP,         0x1, LV_STYLE_ID_VALUE + 5, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_MARGIN_BOTTOM,      0x1, LV_STYLE_ID_VALUE + 6, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_MARGIN_LEFT,        0x1, LV_STYLE_ID_VALUE + 7, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_MARGIN_RIGHT,       0x1, LV_STYLE_ID_VALUE + 8, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_BG_BLEND_MODE,      0x2, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BG_MAIN_STOP,       0x2, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BG_GRAD_STOP,       0x2, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BG_GRAD_DIR,        0x2, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BG_COLOR,           0x2, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BG_GRAD_COLOR,      0x2, LV_STYLE_ID_COLOR + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BG_OPA,             0x2, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_BORDER_WIDTH,       0x3, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BORDER_SIDE,        0x3, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BORDER_BLEND_MODE,  0x3, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BORDER_POST,        0x3, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BORDER_COLOR,       0x3, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BORDER_OPA,         0x3, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_OUTLINE_WIDTH,      0x4, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_OUTLINE_PAD,        0x4, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_OUTLINE_BLEND_MODE, 0x4, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_OUTLINE_COLOR,      0x4, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_OUTLINE_OPA,        0x4, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_WIDTH,       0x5, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_OFS_X,       0x5, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_OFS_Y,       0x5, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_SPREAD,      0x5, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_BLEND_MODE,  0x5, LV_STYLE_ID_VALUE + 4, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_COLOR,       0x5, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_OPA,         0x5, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_PATTERN_BLEND_MODE, 0x6, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PATTERN_REPEAT,     0x6, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PATTERN_RECOLOR,    0x6, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PATTERN_OPA,        0x6, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PATTERN_RECOLOR_OPA, 0x6, LV_STYLE_ID_OPA   + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PATTERN_IMAGE,      0x6, LV_STYLE_ID_PTR   + 0, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_VALUE_LETTER_SPACE, 0x7, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_VALUE_LINE_SPACE,   0x7, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_VALUE_BLEND_MODE,   0x7, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_VALUE_OFS_X,        0x7, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_VALUE_OFS_Y,        0x7, LV_STYLE_ID_VALUE + 4, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_VALUE_ALIGN,        0x7, LV_STYLE_ID_VALUE + 5, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_VALUE_COLOR,        0x7, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_VALUE_OPA,          0x7, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_VALUE_FONT,         0x7, LV_STYLE_ID_PTR   + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_VALUE_STR,          0x7, LV_STYLE_ID_PTR   + 1, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_LETTER_SPACE,  0x8, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_LINE_SPACE,    0x8, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_DECOR,         0x8, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_BLEND_MODE,    0x8, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_COLOR,         0x8, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_SEL_COLOR,     0x8, LV_STYLE_ID_COLOR + 1, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_SEL_BG_COLOR,  0x8, LV_STYLE_ID_COLOR + 2, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_OPA,           0x8, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_FONT,          0x8, LV_STYLE_ID_PTR   + 0, LV_STYLE_ATTR_INHERIT),

    LV_STYLE_PROP_INIT(LV_STYLE_LINE_WIDTH,         0x9, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_LINE_BLEND_MODE,    0x9, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_LINE_DASH_WIDTH,    0x9, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_LINE_DASH_GAP,      0x9, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_LINE_ROUNDED,       0x9, LV_STYLE_ID_VALUE + 4, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_LINE_COLOR,         0x9, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_LINE_OPA,           0x9, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_IMAGE_BLEND_MODE,   0xA, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_IMAGE_RECOLOR,      0xA, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_IMAGE_OPA,          0xA, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_IMAGE_RECOLOR_OPA,  0xA, LV_STYLE_ID_OPA   + 1, LV_STYLE_ATTR_INHERIT),

    LV_STYLE_PROP_INIT(LV_STYLE_TRANSITION_TIME,    0xB, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSITION_DELAY,   0xB, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSITION_PROP_1,  0xB, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSITION_PROP_2,  0xB, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSITION_PROP_3,  0xB, LV_STYLE_ID_VALUE + 4, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSITION_PROP_4,  0xB, LV_STYLE_ID_VALUE + 5, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSITION_PROP_5,  0xB, LV_STYLE_ID_VALUE + 6, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSITION_PROP_6,  0xB, LV_STYLE_ID_VALUE + 7, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSITION_PATH,    0xB, LV_STYLE_ID_PTR   + 0, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_SCALE_WIDTH,            0xC, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SCALE_BORDER_WIDTH,     0xC, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SCALE_END_BORDER_WIDTH, 0xC, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SCALE_END_LINE_WIDTH,   0xC, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SCALE_GRAD_COLOR,       0xC, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SCALE_END_COLOR,        0xC, LV_STYLE_ID_COLOR + 1, LV_STYLE_ATTR_NONE),
};

typedef uint16_t lv_style_property_t;

#define LV_STYLE_STATE_POS       8
#define LV_STYLE_STATE_MASK      0x7F00
#define LV_STYLE_INHERIT_MASK    0x8000

typedef uint16_t lv_style_state_t;

typedef struct {
    uint8_t * map;
#if LV_USE_ASSERT_STYLE
    uint32_t sentinel;
#endif
} lv_style_t;

typedef int16_t lv_style_int_t;

typedef struct {
    lv_style_t ** style_list;
#if LV_USE_ASSERT_STYLE
    uint32_t sentinel;
#endif
    uint32_t style_cnt     : 6;
    uint32_t has_local     : 1;
    uint32_t has_trans     : 1;
    uint32_t skip_trans    : 1;      /*1: Temporally skip the transition style if any*/
    uint32_t ignore_trans  : 1;      /*1: Mark that this style list shouldn't receive transitions at all*/
    uint32_t valid_cache   : 1;      /*1: The cache is valid and can be used*/
    uint32_t ignore_cache  : 1;      /*1: Ignore cache while getting value of properties*/

    uint32_t radius_zero : 1;
    uint32_t opa_scale_cover : 1;
    uint32_t clip_corner_off : 1;
    uint32_t transform_all_zero : 1;
    uint32_t pad_all_zero : 1;
    uint32_t margin_all_zero : 1;
    uint32_t blend_mode_all_normal : 1;
    uint32_t bg_opa_transp : 1;
    uint32_t bg_opa_cover : 1;

    uint32_t border_width_zero : 1;
    uint32_t border_side_full : 1;
    uint32_t border_post_off : 1;

    uint32_t outline_width_zero : 1;
    uint32_t pattern_img_null : 1;
    uint32_t shadow_width_zero : 1;
    uint32_t value_txt_str : 1;
    uint32_t img_recolor_opa_transp : 1;

    uint32_t text_space_zero : 1;
    uint32_t text_decor_none : 1;
    uint32_t text_font_normal : 1;
} lv_style_list_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a style
 * @param style pointer to a style to initialize
 */
void lv_style_init(lv_style_t * style);

/**
 * Copy a style with all its properties
 * @param style_dest pointer to the destination style. (Should be initialized with `lv_style_init()`)
 * @param style_src pointer to the source (to copy )style
 */
void lv_style_copy(lv_style_t * style_dest, const lv_style_t * style_src);

/**
 * Initialize a style list
 * @param list a style list to initialize
 */
void lv_style_list_init(lv_style_list_t * list);

/**
 * Copy a style list with all its styles and local style properties
 * @param list_dest pointer to the destination style list. (should be initialized with `lv_style_list_init()`)
 * @param list_src pointer to the source (to copy) style list.
 */
void lv_style_list_copy(lv_style_list_t * list_dest, const lv_style_list_t * list_src);

/**
 * Add a style to a style list.
 * Only the style pointer will be saved so the shouldn't be a local variable.
 * (It should be static, global or dynamically allocated)
 * @param list pointer to a style list
 * @param style pointer to a style to add
 */
void _lv_style_list_add_style(lv_style_list_t * list, lv_style_t * style);

/**
 * Remove a style from a style list
 * @param style_list pointer to a style list
 * @param style pointer to a style to remove
 */
void _lv_style_list_remove_style(lv_style_list_t * list, lv_style_t * style);

/**
 * Remove all styles added from style list, clear the local style, transition style and free all allocated memories.
 * Leave `ignore_trans` flag as it is.
 * @param list pointer to a style list.
 */
void _lv_style_list_reset(lv_style_list_t * style_list);

static inline lv_style_t * lv_style_list_get_style(lv_style_list_t * list, uint8_t id)
{
    if(list->has_trans && list->skip_trans) id++;
    if(list->style_cnt == 0 || id >= list->style_cnt) return NULL;
    return list->style_list[id];
}

/**
 * Clear all properties from a style and all allocated memories.
 * @param style pointer to a style
 */
void lv_style_reset(lv_style_t * style);

/**
 * Get the size of the properties in a style in bytes
 * @param style pointer to a style
 * @return size of the properties in bytes
 */
uint16_t _lv_style_get_mem_size(const lv_style_t * style);

/**
 * Copy a style to an other
 * @param dest pointer to the destination style
 * @param src pointer to the source style
 */
void lv_style_copy(lv_style_t * dest, const lv_style_t * src);

/**
 * Remove a property from a style
 * @param style pointer to a style
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @return true: the property was found and removed; false: the property wasn't found
 */
bool lv_style_remove_prop(lv_style_t * style, lv_style_property_t prop);

/**
 * Set an integer typed property in a style.
 * @param style pointer to a style where the property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note shouldn't be used directly. Use the specific property set functions instead.
 *       For example: `lv_style_set_border_width()`
 * @note for performance reasons it's not checked if the property really has integer type
 */
void _lv_style_set_int(lv_style_t * style, lv_style_property_t prop, lv_style_int_t value);

/**
 * Set a color typed property in a style.
 * @param style pointer to a style where the property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note shouldn't be used directly. Use the specific property set functions instead.
 *       For example: `lv_style_set_border_color()`
 * @note for performance reasons it's not checked if the property really has color type
 */
void _lv_style_set_color(lv_style_t * style, lv_style_property_t prop, lv_color_t color);

/**
 * Set an opacity typed property in a style.
 * @param style pointer to a style where the property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_OPA | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note shouldn't be used directly. Use the specific property set functions instead.
 *       For example: `lv_style_set_border_opa()`
 * @note for performance reasons it's not checked if the property really has opacity type
 */
void _lv_style_set_opa(lv_style_t * style, lv_style_property_t prop, lv_opa_t opa);

/**
 * Set a pointer typed property in a style.
 * @param style pointer to a style where the property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_TEXT_POINTER | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note shouldn't be used directly. Use the specific property set functions instead.
 *       For example: `lv_style_set_border_width()`
 * @note for performance reasons it's not checked if the property really has pointer type
 */
void _lv_style_set_ptr(lv_style_t * style, lv_style_property_t prop, const void * p);

/**
 * Get an integer typed property from a style.
 * @param style pointer to a style from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result value
 * @return -1: the property wasn't found in the style.
 *         The matching state bits of the desired state (in `prop`) and the best matching property's state
 *         Higher value means match in higher precedence state.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_style_get_border_width()`
 * @note for performance reasons it's not checked if the property really has integer type
 */
int16_t _lv_style_get_int(const lv_style_t * style, lv_style_property_t prop, lv_style_int_t * res);

/**
 * Get a color typed property from a style.
 * @param style pointer to a style from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result value
 * @return -1: the property wasn't found in the style.
 *         The matching state bits of the desired state (in `prop`) and the best matching property's state
 *         Higher value means match in higher precedence state.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_style_get_border_color()`
 * @note for performance reasons it's not checked if the property really has color type
 */
int16_t _lv_style_get_color(const lv_style_t * style, lv_style_property_t prop, lv_color_t * res);

/**
 * Get an opacity typed property from a style.
 * @param style pointer to a style from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_OPA | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result value
 * @return -1: the property wasn't found in the style.
 *         The matching state bits of the desired state (in `prop`) and the best matching property's state
 *         Higher value means match in higher precedence state.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has opacity type
 */
int16_t _lv_style_get_opa(const lv_style_t * style, lv_style_property_t prop, lv_opa_t * res);

/**
 * Get a pointer typed property from a style.
 * @param style pointer to a style from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result value
 * @return -1: the property wasn't found in the style.
 *         The matching state bits of the desired state (in `prop`) and the best matching property's state
 *         Higher value means match in higher precedence state.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_style_get_text_font()`
 * @note for performance reasons it's not checked if the property really has pointer type
 */
int16_t _lv_style_get_ptr(const lv_style_t * style, lv_style_property_t prop, const void ** res);

/**
 * Get the local style of a style list
 * @param list pointer to a style list where the local property should be set
 * @return pointer to the local style if exists else `NULL`.
 */
lv_style_t * lv_style_list_get_local_style(lv_style_list_t * list);

/**
 * Get the transition style of a style list
 * @param list pointer to a style list where the transition property should be set
 * @return pointer to the transition style if exists else `NULL`.
 */
lv_style_t * _lv_style_list_get_transition_style(lv_style_list_t * list);

/**
 * Allocate the transition style in a style list. If already exists simply return it.
 * @param list pointer to a style list
 * @return the transition style of a style list
 */
lv_style_t * _lv_style_list_add_trans_style(lv_style_list_t * list);

/**
 * Set a local integer typed property in a style list.
 * @param list pointer to a style list where the local property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note for performance reasons it's not checked if the property really has integer type
 */
void _lv_style_list_set_local_int(lv_style_list_t * list, lv_style_property_t prop, lv_style_int_t value);

/**
 * Set a local color typed property in a style list.
 * @param list pointer to a style list where the local property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note for performance reasons it's not checked if the property really has color type
 */
void _lv_style_list_set_local_color(lv_style_list_t * list, lv_style_property_t prop, lv_color_t value);

/**
 * Set a local opacity typed property in a style list.
 * @param list pointer to a style list where the local property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_OPA | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note for performance reasons it's not checked if the property really has opacity type
 */
void _lv_style_list_set_local_opa(lv_style_list_t * list, lv_style_property_t prop, lv_opa_t value);

/**
 * Set a local pointer typed property in a style list.
 * @param list pointer to a style list where the local property should be set
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param value the value to set
 * @note for performance reasons it's not checked if the property really has pointer type
 */
void _lv_style_list_set_local_ptr(lv_style_list_t * list, lv_style_property_t prop, const void * value);

/**
 * Get an integer typed property from a style list.
 * It will return the property which match best with given state.
 * @param list pointer to a style list from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result
 * @return LV_RES_OK: there was a matching property in the list
 *         LV_RES_INV: there was NO matching property in the list
 * @note for performance reasons it's not checked if the property really has integer type
 */
lv_res_t _lv_style_list_get_int(lv_style_list_t * list, lv_style_property_t prop, lv_style_int_t * res);

/**
 * Get a color typed property from a style list.
 * It will return the property which match best with given state.
 * @param list pointer to a style list from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result
 * @return LV_RES_OK: there was a matching property in the list
 *         LV_RES_INV: there was NO matching property in the list
 * @note for performance reasons it's not checked if the property really has color type
 */
lv_res_t _lv_style_list_get_color(lv_style_list_t * list, lv_style_property_t prop, lv_color_t * res);

/**
 * Get an opacity typed property from a style list.
 * It will return the property which match best with given state.
 * @param list pointer to a style list from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_OPA | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result
 * @return LV_RES_OK: there was a matching property in the list
 *         LV_RES_INV: there was NO matching property in the list
 * @note for performance reasons it's not checked if the property really has opacity type
 */
lv_res_t _lv_style_list_get_opa(lv_style_list_t * list, lv_style_property_t prop, lv_opa_t * res);

/**
 * Get a pointer typed property from a style list.
 * It will return the property which match best with given state.
 * @param list pointer to a style list from where the property should be get
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param res pointer to a buffer to store the result
 * @return LV_RES_OK: there was a matching property in the list
 *         LV_RES_INV: there was NO matching property in the list
 * @note for performance reasons it's not checked if the property really has pointer type
 */
lv_res_t _lv_style_list_get_ptr(lv_style_list_t * list, lv_style_property_t prop, const void ** res);

/**
 * Check whether a style is valid (initialized correctly)
 * @param style pointer to a style
 * @return true: valid
 */
bool lv_debug_check_style(const lv_style_t * style);

/**
 * Check whether a style list is valid (initialized correctly)
 * @param list pointer to a style list
 * @return true: valid
 */
bool lv_debug_check_style_list(const lv_style_list_t * list);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

/**
 * Create and initialize a `static` style
 * Example:
 *     LV_STYLE_CREATE(my_style, &style_to_copy);
 *   is equivalent to
 *     static lv_style_t my_style;
 *     lv_style_init(&my_style);
 *     lv_style_copy(&my_style, &style_to_copy);
 */
#define LV_STYLE_CREATE(name, copy_p) static lv_style_t name; lv_style_init(&name); lv_style_copy(&name, copy_p)

#if LV_USE_DEBUG

# ifndef LV_DEBUG_IS_STYLE
#  define LV_DEBUG_IS_STYLE(style_p) (lv_debug_check_style(style_p))
# endif

# ifndef LV_DEBUG_IS_STYLE_LIST
#  define LV_DEBUG_IS_STYLE_LIST(list_p) (lv_debug_check_style_list(list_p))
# endif

# if LV_USE_ASSERT_STYLE
#  ifndef LV_ASSERT_STYLE
#   define LV_ASSERT_STYLE(style_p) LV_DEBUG_ASSERT(LV_DEBUG_IS_STYLE(style_p), "Invalid style", style_p);
#  endif
#  ifndef LV_ASSERT_STYLE_LIST
#   define LV_ASSERT_STYLE_LIST(list_p) LV_DEBUG_ASSERT(LV_DEBUG_IS_STYLE_LIST(list_p), "Invalid style list", list_p);
#  endif
# else
#   define LV_ASSERT_STYLE(style_p)
#   define LV_ASSERT_STYLE_LIST(list_p)
# endif

#else
# define LV_ASSERT_STYLE(p)
# define LV_ASSERT_STYLE_LIST(p)
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_STYLE_H*/
