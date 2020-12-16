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

#define LV_DEBUG_STYLE_SENTINEL_VALUE       0xAABBCCDD

#define LV_STYLE_PROP_INHERIT       (1 << 8)

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

typedef union {
    int32_t _int;
    const void * _ptr;
    lv_color_t _color;
}lv_style_value_t;

typedef enum {
    _LV_STYLE_PROP_INV = 0,
    LV_STYLE_RADIUS         = 1,
    LV_STYLE_CLIP_CORNER    = 2,
    LV_STYLE_TRANSFORM_WIDTH = 3,
    LV_STYLE_TRANSFORM_HEIGHT = 4,
    LV_STYLE_TRANSFORM_ZOOM = 5,
    LV_STYLE_TRANSFORM_ANGLE = 6,
    LV_STYLE_OPA = 7 | LV_STYLE_PROP_INHERIT,

    LV_STYLE_PAD_TOP        = 10,
    LV_STYLE_PAD_BOTTOM     = 11,
    LV_STYLE_PAD_LEFT       = 12,
    LV_STYLE_PAD_RIGHT      = 13,
    LV_STYLE_MARGIN_TOP = 14,
    LV_STYLE_MARGIN_BOTTOM = 15,
    LV_STYLE_MARGIN_LEFT = 16,
    LV_STYLE_MARGIN_RIGHT = 17,

    LV_STYLE_BG_COLOR       = 20,
    LV_STYLE_BG_OPA         = 21,
    LV_STYLE_BG_GRAD_COLOR =  22,
    LV_STYLE_BG_GRAD_DIR =    23,
    LV_STYLE_BG_BLEND_MODE =  24,
    LV_STYLE_BG_MAIN_STOP =   25,
    LV_STYLE_BG_GRAD_STOP =   26,
    LV_STYLE_BG_IMG_SRC =     27,
    LV_STYLE_BG_IMG_MOSAIC =  28,

    LV_STYLE_BORDER_COLOR   = 31,
    LV_STYLE_BORDER_OPA     = 32,
    LV_STYLE_BORDER_WIDTH   = 33,
    LV_STYLE_BORDER_SIDE    = 34,
    LV_STYLE_BORDER_POST    = 35,
    LV_STYLE_BORDER_BLEND_MODE = 36,

    LV_STYLE_TEXT_COLOR     = 40 | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_OPA       = 41 | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_FONT      = 42 | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_LETTER_SPACE = 43 | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_LINE_SPACE = 44 | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_DECOR = 45 | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_BLEND_MODE = 46 | LV_STYLE_PROP_INHERIT,

    LV_STYLE_IMG_OPA        = 50,
    LV_STYLE_IMG_BLEND_MODE = 51,
    LV_STYLE_IMG_RECOLOR = 52,
    LV_STYLE_IMG_RECOLOR_OPA = 53,

    LV_STYLE_OUTLINE_WIDTH = 61,
    LV_STYLE_OUTLINE_COLOR = 62,
    LV_STYLE_OUTLINE_OPA = 63,
    LV_STYLE_OUTLINE_PAD = 64,
    LV_STYLE_OUTLINE_BLEND_MODE = 65,

    LV_STYLE_SHADOW_WIDTH = 70,
    LV_STYLE_SHADOW_OFS_X = 71,
    LV_STYLE_SHADOW_OFS_Y = 72,
    LV_STYLE_SHADOW_SPREAD = 73,
    LV_STYLE_SHADOW_BLEND_MODE = 74,
    LV_STYLE_SHADOW_COLOR = 75,
    LV_STYLE_SHADOW_OPA = 76,

    LV_STYLE_LINE_WIDTH = 80,
    LV_STYLE_LINE_BLEND_MODE = 81,
    LV_STYLE_LINE_DASH_WIDTH = 82,
    LV_STYLE_LINE_DASH_GAP = 83,
    LV_STYLE_LINE_ROUNDED = 84,
    LV_STYLE_LINE_COLOR = 85,
    LV_STYLE_LINE_OPA = 86,

    LV_STYLE_CONTENT_TEXT = 90,
    LV_STYLE_CONTENT_ALIGN = 91,
    LV_STYLE_CONTENT_OFS_X = 92,
    LV_STYLE_CONTENT_OFS_Y = 93,

    LV_STYLE_TRANSITION_TIME = 100,
    LV_STYLE_TRANSITION_DELAY = 101,
    LV_STYLE_TRANSITION_PATH = 102,
    LV_STYLE_TRANSITION_PROP_1 = 103,
    LV_STYLE_TRANSITION_PROP_2 = 104,
    LV_STYLE_TRANSITION_PROP_3 = 105,
    LV_STYLE_TRANSITION_PROP_4 = 106,
    LV_STYLE_TRANSITION_PROP_5 = 107,
    LV_STYLE_TRANSITION_PROP_6 = 108,
}lv_style_prop_t;

typedef struct {
    uint32_t clip_corner    :1;
    int8_t  transform_width :8;
    int8_t  transform_height:8;
    uint16_t  transform_zoom  :14;
    uint16_t  transform_angle  :14;
    lv_opa_t opa;

    int8_t margin_top;
    int8_t margin_bottom;
    int8_t margin_left;
    int8_t margin_right;

    lv_color_t bg_grad_color;
    lv_grad_dir_t bg_grad_dir:3;
    lv_blend_mode_t bg_blend_mode   :2;
    uint8_t bg_main_stop;
    uint8_t bg_grad_stop;
    const void * bg_img_src;
    uint32_t bg_img_mosaic :1;

    lv_blend_mode_t border_blend_mode   :2;

    uint8_t outline_width;
    lv_color_t outline_color;
    lv_opa_t outline_opa;
    int8_t outline_pad;
    lv_blend_mode_t outline_blend_mode   :2;

    uint8_t shadow_width;
    int8_t shadow_ofs_x;
    int8_t shadow_ofs_y;
    int8_t shadow_spread;
    lv_blend_mode_t shadow_blend_mode   :2;
    lv_color_t shadow_color;
    lv_opa_t shadow_opa;

    int8_t text_letter_space;
    int8_t text_line_space;
    lv_text_decor_t text_decor :2;
    lv_blend_mode_t text_blend_mode   :2;

    lv_blend_mode_t img_blend_mode   :2;
    lv_color_t img_recolor;
    lv_opa_t img_recolor_opa;

    uint8_t line_width;
    lv_blend_mode_t line_blend_mode   :2;
    uint8_t line_dash_width;
    uint8_t line_dash_gap;
    uint32_t line_rounded:1;
    lv_color_t line_color;
    lv_opa_t line_opa;

    uint32_t transition_time :16;
    uint32_t transition_delay :16;
    const lv_anim_path_cb_t * transition_path;
    uint16_t transition_prop_1;
    uint16_t transition_prop_2;
    uint16_t transition_prop_3;
    uint16_t transition_prop_4;
    uint16_t transition_prop_5;
    uint16_t transition_prop_6;

    const char * content_text;
    lv_align_t content_align :5;
    int8_t content_ofs_x;
    int8_t content_ofs_y;

    uint32_t has_clip_corner :1;
    uint32_t has_transform_width :1;
    uint32_t has_transform_height :1;
    uint32_t has_transform_zoom :1;
    uint32_t has_transform_angle :1;
    uint32_t has_opa :1;

    uint32_t has_margin_top :1;
    uint32_t has_margin_bottom :1;
    uint32_t has_margin_left :1;
    uint32_t has_margin_right :1;

    uint32_t has_bg_grad_color :1;
    uint32_t has_bg_grad_dir :1;
    uint32_t has_bg_blend_mode :1;
    uint32_t has_bg_main_stop :1;
    uint32_t has_bg_grad_stop :1;
    uint32_t has_bg_img_src :1;
    uint32_t has_bg_img_mosaic :1;

    uint32_t has_border_blend_mode :1;

    uint32_t has_outline_width :1;
    uint32_t has_outline_color :1;
    uint32_t has_outline_opa :1;
    uint32_t has_outline_pad :1;
    uint32_t has_outline_blend_mode :1;

    uint32_t has_shadow_width :1;
    uint32_t has_shadow_ofs_x :1;
    uint32_t has_shadow_ofs_y :1;
    uint32_t has_shadow_spread :1;
    uint32_t has_shadow_blend_mode :1;
    uint32_t has_shadow_color :1;
    uint32_t has_shadow_opa :1;

    uint32_t has_text_letter_space :1;
    uint32_t has_text_line_space :1;
    uint32_t has_text_decor :1;
    uint32_t has_text_blend_mode :1;

    uint32_t has_img_blend_mode :1;
    uint32_t has_img_recolor :1;
    uint32_t has_img_recolor_opa :1;

    uint32_t has_line_width :1;
    uint32_t has_line_blend_mode :1;
    uint32_t has_line_dash_width :1;
    uint32_t has_line_dash_gap :1;
    uint32_t has_line_rounded :1;
    uint32_t has_line_color :1;
    uint32_t has_line_opa :1;

    uint32_t has_content_text :1;
    uint32_t has_content_align :1;
    uint32_t has_content_ofs_x :1;
    uint32_t has_content_ofs_y :1;

    uint32_t has_transition_time :1;
    uint32_t has_transition_delay :1;
    uint32_t has_transition_path :1;
    uint32_t has_transition_prop_1 :1;
    uint32_t has_transition_prop_2 :1;
    uint32_t has_transition_prop_3 :1;
    uint32_t has_transition_prop_4 :1;
    uint32_t has_transition_prop_5 :1;
    uint32_t has_transition_prop_6 :1;
}lv_style_ext_t;


typedef struct {
#if LV_USE_ASSERT_STYLE
    uint32_t sentinel;
#endif
    const lv_font_t * text_font;
    lv_style_ext_t * ext;

    lv_opa_t bg_opa;
    lv_opa_t border_opa;
    lv_opa_t text_opa;
    lv_opa_t img_opa;

    int8_t pad_top :8;
    int8_t pad_bottom :8;
    int8_t pad_left :8;
    int8_t pad_right :8;

    lv_color_t bg_color;
    lv_color_t border_color;

    lv_color_t text_color;
    uint32_t radius :8;
    uint32_t border_width :8;

    uint32_t border_side:4;
    uint32_t border_post:1;

    uint16_t has_text_font :1;
    uint16_t has_bg_opa :1;
    uint16_t has_border_opa :1;
    uint16_t has_text_opa :1;
    uint16_t has_img_opa :1;
    uint16_t has_pad_top :1;
    uint16_t has_pad_bottom :1;
    uint16_t has_pad_left :1;
    uint16_t has_pad_right :1;
    uint16_t has_bg_color :1;
    uint16_t has_border_color :1;
    uint16_t has_text_color :1;
    uint16_t has_radius :1;
    uint16_t has_border_width :1;
    uint16_t has_border_side :1;
    uint16_t has_border_post :1;
} lv_style_t;

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
 * Clear all properties from a style and all allocated memories.
 * @param style pointer to a style
 */
void lv_style_reset(lv_style_t * style);

bool lv_style_remove_prop(lv_style_t * style, lv_style_prop_t prop);

void lv_style_set_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t value);

bool lv_style_get_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value);
/**
 * Remove a property from a style
 * @param style pointer to a style
 * @param prop  a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @return true: the property was found and removed; false: the property wasn't found
 */
bool lv_style_remove_prop(lv_style_t * style, lv_style_prop_t prop);

static inline void lv_style_set_radius(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_RADIUS, v); }

static inline void lv_style_set_clip_corner(lv_style_t * style, bool value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_CLIP_CORNER, v); }

static inline void lv_style_set_transform_width(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TRANSFORM_WIDTH, v); }

static inline void lv_style_set_transform_height(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TRANSFORM_HEIGHT, v); }

static inline void lv_style_set_transform_zoom(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TRANSFORM_ZOOM, v); }

static inline void lv_style_set_transform_angle(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TRANSFORM_ANGLE, v); }

static inline void lv_style_set_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_OPA, v); }

static inline void lv_style_set_pad_top(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_PAD_TOP, v); }

static inline void lv_style_set_pad_bottom(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_PAD_BOTTOM, v); }

static inline void lv_style_set_pad_left(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_PAD_LEFT, v); }

static inline void lv_style_set_pad_right(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_PAD_RIGHT, v); }

static inline void lv_style_set_margin_top(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_MARGIN_TOP, v); }

static inline void lv_style_set_margin_bottom(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_MARGIN_BOTTOM, v); }

static inline void lv_style_set_margin_left(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_MARGIN_LEFT, v); }

static inline void lv_style_set_margin_right(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_MARGIN_RIGHT, v); }

static inline void lv_style_set_bg_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {._color = value}; lv_style_set_prop(style, LV_STYLE_BG_COLOR, v); }

static inline void lv_style_set_bg_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_BG_OPA, v); }

static inline void lv_style_set_bg_grad_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {._color = value}; lv_style_set_prop(style, LV_STYLE_BG_GRAD_COLOR, v); }

static inline void lv_style_set_bg_grad_dir(lv_style_t * style, lv_grad_dir_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_BG_GRAD_DIR, v); }

static inline void lv_style_set_bg_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_BG_BLEND_MODE, v); }

static inline void lv_style_set_bg_main_stop(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_BG_MAIN_STOP, v); }

static inline void lv_style_set_bg_grad_stop(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_BG_GRAD_STOP, v); }

static inline void lv_style_set_border_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {._color = value}; lv_style_set_prop(style, LV_STYLE_BORDER_COLOR, v); }

static inline void lv_style_set_border_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_BORDER_OPA, v); }

static inline void lv_style_set_border_width(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_BORDER_WIDTH, v); }

static inline void lv_style_set_border_side(lv_style_t * style, lv_border_side_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_BORDER_SIDE, v); }

static inline void lv_style_set_border_post(lv_style_t * style, bool value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_BORDER_POST, v); }

static inline void lv_style_set_border_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_BORDER_BLEND_MODE, v); }

static inline void lv_style_set_text_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {._color = value}; lv_style_set_prop(style, LV_STYLE_TEXT_COLOR, v); }

static inline void lv_style_set_text_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TEXT_OPA, v); }

static inline void lv_style_set_text_font(lv_style_t * style, const lv_font_t * value) {
  lv_style_value_t v = {._ptr = value}; lv_style_set_prop(style, LV_STYLE_TEXT_FONT, v); }

static inline void lv_style_set_text_letter_space(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TEXT_LETTER_SPACE, v); }

static inline void lv_style_set_text_line_space(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TEXT_LINE_SPACE, v); }

static inline void lv_style_set_text_decor(lv_style_t * style, lv_text_decor_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TEXT_DECOR, v); }

static inline void lv_style_set_text_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TEXT_BLEND_MODE, v); }

static inline void lv_style_set_img_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_IMG_OPA, v); }

static inline void lv_style_set_img_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_IMG_BLEND_MODE, v); }

static inline void lv_style_set_img_recolor(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {._color = value}; lv_style_set_prop(style, LV_STYLE_IMG_RECOLOR, v); }

static inline void lv_style_set_img_recolor_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_IMG_RECOLOR_OPA, v); }

static inline void lv_style_set_outline_width(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_OUTLINE_WIDTH, v); }

static inline void lv_style_set_outline_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {._color = value}; lv_style_set_prop(style, LV_STYLE_OUTLINE_COLOR, v); }

static inline void lv_style_set_outline_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_OUTLINE_OPA, v); }

static inline void lv_style_set_outline_pad(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_OUTLINE_PAD, v); }

static inline void lv_style_set_outline_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_OUTLINE_BLEND_MODE, v); }

static inline void lv_style_set_shadow_width(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_WIDTH, v); }

static inline void lv_style_set_shadow_ofs_x(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_OFS_X, v); }

static inline void lv_style_set_shadow_ofs_y(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_OFS_Y, v); }

static inline void lv_style_set_shadow_spread(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_SPREAD, v); }

static inline void lv_style_set_shadow_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_BLEND_MODE, v); }

static inline void lv_style_set_shadow_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {._color = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_COLOR, v); }

static inline void lv_style_set_shadow_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_OPA, v); }

static inline void lv_style_set_line_width(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_LINE_WIDTH, v); }

static inline void lv_style_set_line_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_LINE_BLEND_MODE, v); }

static inline void lv_style_set_line_dash_width(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_LINE_DASH_WIDTH, v); }

static inline void lv_style_set_line_dash_gap(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_LINE_DASH_GAP, v); }

static inline void lv_style_set_line_rounded(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_LINE_ROUNDED, v); }

static inline void lv_style_set_line_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {._color = value}; lv_style_set_prop(style, LV_STYLE_LINE_COLOR, v); }

static inline void lv_style_set_line_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_LINE_OPA, v); }

static inline void lv_style_set_content_text(lv_style_t * style, const char * value) {
  lv_style_value_t v = {._ptr = value}; lv_style_set_prop(style, LV_STYLE_CONTENT_TEXT, v); }

static inline void lv_style_set_content_align(lv_style_t * style, lv_align_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_CONTENT_ALIGN, v); }

static inline void lv_style_set_content_ofs_x(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_CONTENT_OFS_X, v); }

static inline void lv_style_set_content_ofs_y(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_CONTENT_OFS_Y, v); }

static inline void lv_style_set_transition_time(lv_style_t * style, uint16_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TRANSITION_TIME, v); }

static inline void lv_style_set_transition_delay(lv_style_t * style, uint16_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TRANSITION_DELAY, v); }

static inline void lv_style_set_transition_path(lv_style_t * style, const lv_anim_path_t * value) {
  lv_style_value_t v = {._ptr = value}; lv_style_set_prop(style, LV_STYLE_TRANSITION_PATH, v); }

static inline void lv_style_set_transition_prop_1(lv_style_t * style, lv_style_prop_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TRANSITION_PROP_1, v); }

static inline void lv_style_set_transition_prop_2(lv_style_t * style, lv_style_prop_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TRANSITION_PROP_2, v); }

static inline void lv_style_set_transition_prop_3(lv_style_t * style, lv_style_prop_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TRANSITION_PROP_3, v); }

static inline void lv_style_set_transition_prop_4(lv_style_t * style, lv_style_prop_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TRANSITION_PROP_4, v); }

static inline void lv_style_set_transition_prop_5(lv_style_t * style, lv_style_prop_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TRANSITION_PROP_5, v); }

static inline void lv_style_set_transition_prop_6(lv_style_t * style, lv_style_prop_t value) {
  lv_style_value_t v = {._int = value}; lv_style_set_prop(style, LV_STYLE_TRANSITION_PROP_6, v); }




//static inline void lv_style_set_pad_ver(lv_style_t * style, lv_style_int_t value)
//{
//    lv_style_set_pad_top(style, state, value);
//    lv_style_set_pad_bottom(style, state, value);
//}

//
//static inline void lv_obj_set_style_margin_all(lv_obj_t * obj, uint8_t part, lv_state_t state,
//                                                     lv_style_int_t value)
//{
//    lv_obj_set_style_local_margin_top(obj, part, state, value);
//    lv_obj_set_style_local_margin_bottom(obj, part, state, value);
//    lv_obj_set_style_local_margin_left(obj, part, state, value);
//    lv_obj_set_style_local_margin_right(obj, part, state, value);
//}
//
//
//static inline void lv_style_set_margin_all(lv_style_t * style, lv_state_t state, lv_style_int_t value)
//{
//    lv_style_set_margin_top(style, state, value);
//    lv_style_set_margin_bottom(style, state, value);
//    lv_style_set_margin_left(style, state, value);
//    lv_style_set_margin_right(style, state, value);
//}
//
//
//static inline void lv_obj_set_style_local_margin_hor(lv_obj_t * obj, uint8_t part, lv_state_t state,
//                                                     lv_style_int_t value)
//{
//    lv_obj_set_style_local_margin_left(obj, part, state, value);
//    lv_obj_set_style_local_margin_right(obj, part, state, value);
//}
//
//
//static inline void lv_style_set_margin_hor(lv_style_t * style, lv_state_t state, lv_style_int_t value)
//{
//    lv_style_set_margin_left(style, state, value);
//    lv_style_set_margin_right(style, state, value);
//}
//
//
//static inline void lv_obj_set_style_local_margin_ver(lv_obj_t * obj, uint8_t part, lv_state_t state,
//                                                     lv_style_int_t value)
//{
//    lv_obj_set_style_local_margin_top(obj, part, state, value);
//    lv_obj_set_style_local_margin_bottom(obj, part, state, value);
//}
//
//
//static inline void lv_style_set_margin_ver(lv_style_t * style, lv_state_t state, lv_style_int_t value)
//{
//    lv_style_set_margin_top(style, state, value);
//    lv_style_set_margin_bottom(style, state, value);
//}



lv_style_value_t lv_style_prop_get_default(lv_style_prop_t prop);

bool lv_style_prop_is_inherited(lv_style_prop_t prop);

/**
 * Check whether a style is valid (initialized correctly)
 * @param style pointer to a style
 * @return true: valid
 */
bool lv_debug_check_style(const lv_style_t * style);

/**
 * Check whether a style list is valid (initialized correctly)
 * @param style pointer to a style
 * @return true: valid
 */
bool lv_debug_check_style_list(const void * list);

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
#define LV_STYLE_CREATE(name, copy_p) static lv_style_t name; lv_style_init(&name); lv_style_copy(&name, copy_p);



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
