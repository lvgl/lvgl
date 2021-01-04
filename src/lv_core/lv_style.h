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
#include "../lv_misc/lv_class.h"
#include "../lv_draw/lv_draw_blend.h"

/*********************
 *      DEFINES
 *********************/

#define LV_RADIUS_CIRCLE (0x7F) /**< A very big radius to always draw as circle*/
LV_EXPORT_CONST_INT(LV_RADIUS_CIRCLE);

#define LV_DEBUG_STYLE_SENTINEL_VALUE       0xAABBCCDD

#define LV_STYLE_PROP_INHERIT       (1 << 10)
#define LV_STYLE_PROP_EXT_DRAW      (1 << 11)
#define LV_STYLE_PROP_LAYOUT_REFR   (1 << 12)
#define LV_STYLE_PROP_FILTER        (1 << 13)

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
    int32_t num;
    const void * ptr;
    void (* func)(void);
    lv_color_t color;
}lv_style_value_t;

typedef enum {
    _LV_STYLE_PROP_INV = 0,
    LV_STYLE_RADIUS         = 1,
    LV_STYLE_CLIP_CORNER    = 2,
    LV_STYLE_TRANSFORM_WIDTH = 3  | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_TRANSFORM_HEIGHT = 4 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_TRANSFORM_ZOOM = 5   | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_TRANSFORM_ANGLE = 6  | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_OPA = 7 | LV_STYLE_PROP_INHERIT,
    LV_STYLE_COLOR_FILTER_CB = 8,
    LV_STYLE_COLOR_FILTER_OPA = 9,
    LV_STYLE_TRANSITION = 10,

    LV_STYLE_PAD_TOP        = 20 | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_PAD_BOTTOM     = 21 | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_PAD_LEFT       = 22 | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_PAD_RIGHT      = 23 | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_PAD_ROW        = 24 | LV_STYLE_PROP_LAYOUT_REFR,
    LV_STYLE_PAD_COLUMN     = 25 | LV_STYLE_PROP_LAYOUT_REFR,

    LV_STYLE_BG_COLOR        = 30,
    LV_STYLE_BG_COLOR_FILTERED = 30 | LV_STYLE_PROP_FILTER,
    LV_STYLE_BG_OPA         = 31,
    LV_STYLE_BG_GRAD_COLOR =  32,
    LV_STYLE_BG_GRAD_COLOR_FILTERED =  32 | LV_STYLE_PROP_FILTER,
    LV_STYLE_BG_GRAD_DIR =    33,
    LV_STYLE_BG_BLEND_MODE =  34,
    LV_STYLE_BG_MAIN_STOP =   35,
    LV_STYLE_BG_GRAD_STOP =   36,

    LV_STYLE_BORDER_COLOR   = 40,
    LV_STYLE_BORDER_COLOR_FILTERED   = 40 | LV_STYLE_PROP_FILTER,
    LV_STYLE_BORDER_OPA     = 41,
    LV_STYLE_BORDER_WIDTH   = 42,
    LV_STYLE_BORDER_SIDE    = 43,
    LV_STYLE_BORDER_POST    = 44,
    LV_STYLE_BORDER_BLEND_MODE = 45,

    LV_STYLE_TEXT_COLOR     = 50    | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_COLOR_FILTERED = 50    | LV_STYLE_PROP_INHERIT | LV_STYLE_PROP_FILTER,
    LV_STYLE_TEXT_OPA       = 51    | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_FONT      = 52    | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_LETTER_SPACE = 53 | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_LINE_SPACE = 54   | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_DECOR = 55        | LV_STYLE_PROP_INHERIT,
    LV_STYLE_TEXT_BLEND_MODE = 56   | LV_STYLE_PROP_INHERIT,

    LV_STYLE_IMG_OPA        = 60,
    LV_STYLE_IMG_BLEND_MODE = 61,
    LV_STYLE_IMG_RECOLOR = 62,
    LV_STYLE_IMG_RECOLOR_FILTERED = 62 | LV_STYLE_PROP_FILTER,
    LV_STYLE_IMG_RECOLOR_OPA = 63,

    LV_STYLE_OUTLINE_WIDTH = 71   | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_OUTLINE_COLOR = 72,
    LV_STYLE_OUTLINE_COLOR_FILTERED = 72 | LV_STYLE_PROP_FILTER,
    LV_STYLE_OUTLINE_OPA = 73     | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_OUTLINE_PAD = 74     | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_OUTLINE_BLEND_MODE = 75,

    LV_STYLE_SHADOW_WIDTH = 80 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_SHADOW_OFS_X = 81 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_SHADOW_OFS_Y = 82 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_SHADOW_SPREAD = 83 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_SHADOW_BLEND_MODE = 84,
    LV_STYLE_SHADOW_COLOR = 85,
    LV_STYLE_SHADOW_COLOR_FILTERED = 85 | LV_STYLE_PROP_FILTER,
    LV_STYLE_SHADOW_OPA = 86 | LV_STYLE_PROP_EXT_DRAW,

    LV_STYLE_LINE_WIDTH = 90 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_LINE_BLEND_MODE = 91,
    LV_STYLE_LINE_DASH_WIDTH = 92,
    LV_STYLE_LINE_DASH_GAP = 93,
    LV_STYLE_LINE_ROUNDED = 94,
    LV_STYLE_LINE_COLOR = 95,
    LV_STYLE_LINE_COLOR_FILTERED = 95 | LV_STYLE_PROP_FILTER,
    LV_STYLE_LINE_OPA = 96,

    LV_STYLE_CONTENT_SRC = 100   | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_CONTENT_ALIGN = 101 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_CONTENT_OFS_X = 102 | LV_STYLE_PROP_EXT_DRAW,
    LV_STYLE_CONTENT_OFS_Y = 103 | LV_STYLE_PROP_EXT_DRAW,

    _LV_STYLE_LAST_BUILT_IN_PROP = 128,

    LV_STYLE_PROP_ALL = 0xFFFF
}lv_style_prop_t;

struct _lv_style_transiton_t;

typedef struct {
    lv_color_filter_cb_t color_filter_cb;
    const struct _lv_style_transiton_t * transition;
    const char * content_src;
    const lv_font_t * text_font;

    lv_color_t bg_color;
    lv_color_t bg_grad_color;
    lv_color_t border_color;
    lv_color_t text_color;
    lv_color_t outline_color;
    lv_color_t shadow_color;
    lv_color_t img_recolor;
    lv_color_t line_color;

    lv_opa_t opa;
    lv_opa_t color_filter_opa;
    lv_opa_t bg_opa;
    lv_opa_t bg_main_stop;
    lv_opa_t bg_grad_stop;
    lv_opa_t border_opa;
    lv_opa_t outline_opa;
    lv_opa_t img_opa;
    lv_opa_t img_recolor_opa;
    lv_opa_t text_opa;
    lv_opa_t shadow_opa;
    lv_opa_t line_opa;

    int8_t transform_width;
    int8_t transform_height;
    int8_t pad_top;
    int8_t pad_bottom;
    int8_t pad_left;
    int8_t pad_right;
    int8_t pad_row;
    int8_t pad_column;
    int8_t border_width;
    uint8_t outline_width;
    uint8_t outline_pad;
    int8_t shadow_width;
    int8_t shadow_ofs_x;
    int8_t shadow_ofs_y;
    int8_t shadow_spread;
    int8_t text_letter_space;
    int8_t text_line_space;
    uint8_t line_width;
    uint8_t line_dash_width;
    uint8_t line_dash_gap;
    int8_t content_ofs_x;
    int8_t content_ofs_y;

    uint32_t radius:12;
    uint32_t content_align :5;
    uint32_t border_side :5;
    uint32_t bg_blend_mode   :2;
    uint32_t border_blend_mode  :2;
    uint32_t outline_blend_mode   :2;
    uint32_t shadow_blend_mode   :2;
    uint32_t text_decor :2;

    uint32_t transform_zoom  :14;
    uint32_t transform_angle :14;
    uint32_t text_blend_mode   :2;
    uint32_t line_blend_mode   :2;
    uint32_t img_blend_mode   :2;

    struct{
        uint32_t color_filter_cb:1;
        uint32_t transition:1;
        uint32_t content_src:1;
        uint32_t bg_color:1;
        uint32_t bg_grad_color:1;
        uint32_t border_color:1;
        uint32_t text_color:1;
        uint32_t outline_color:1;
        uint32_t shadow_color:1;
        uint32_t img_recolor:1;
        uint32_t line_color:1;
        uint32_t opa:1;
        uint32_t color_filter_opa:1;
        uint32_t bg_opa:1;
        uint32_t bg_main_stop:1;
        uint32_t bg_grad_stop:1;
        uint32_t border_opa:1;
        uint32_t outline_opa:1;
        uint32_t img_opa:1;
        uint32_t img_recolor_opa:1;
        uint32_t text_font:1;
        uint32_t text_opa:1;
        uint32_t shadow_opa:1;
        uint32_t line_opa:1;
        uint32_t radius:1;
        uint32_t transform_width:1;
        uint32_t transform_height:1;
        uint32_t pad_top:1;
        uint32_t pad_bottom:1;
        uint32_t pad_left:1;
        uint32_t pad_right:1;
        uint32_t pad_row:1;
        uint32_t pad_column:1;
        uint32_t border_width:1;
        uint32_t outline_width:1;
        uint32_t outline_pad:1;
        uint32_t shadow_width:1;
        uint32_t shadow_ofs_x:1;
        uint32_t shadow_ofs_y:1;
        uint32_t shadow_spread:1;
        uint32_t text_letter_space:1;
        uint32_t text_line_space:1;
        uint32_t line_width:1;
        uint32_t line_dash_width:1;
        uint32_t line_dash_gap:1;
        uint32_t content_ofs_x:1;
        uint32_t content_ofs_y:1;
        uint32_t transform_zoom:1;
        uint32_t transform_angle:1;
        uint32_t bg_blend_mode:1;
        uint32_t border_blend_mode:1;
        uint32_t outline_blend_mode:1;
        uint32_t shadow_blend_mode:1;
        uint32_t text_decor:1;
        uint32_t text_blend_mode:1;
        uint32_t img_blend_mode:1;
        uint32_t line_blend_mode:1;
        uint32_t content_align:1;
        uint32_t border_side:1;
    }has;
}lv_style_ext_t;


typedef struct _lv_style_transiton_t{
    const lv_style_prop_t * props;
    const lv_anim_path_t * path;
    uint32_t time;
    uint32_t delay;
}lv_style_transiton_t;

#if LV_USE_ASSERT_STYLE
#  define _LV_STYLE_SENTINEL uint32_t sentinel;
#else
#  define _LV_STYLE_SENTINEL
#endif

struct _lv_style_t;

typedef struct {
  bool (*remove_prop)(struct _lv_style_t * style, lv_style_prop_t prop);
  void (*set_prop)(struct _lv_style_t * style, lv_style_prop_t prop, lv_style_value_t value);
  bool (*get_prop)(const struct _lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value);
}lv_style_class_t;

typedef struct _lv_style_t{
    lv_style_class_t * class_p;
    lv_style_ext_t * ext;
    _LV_STYLE_SENTINEL
    /*32*/
    uint32_t radius :5;
    uint32_t transform_width :5;
    uint32_t transform_height :5;
    uint32_t opa :5;
    uint32_t color_filter_cb :4;
    uint32_t transition :4;
    uint32_t bg_color :4;

    /*32*/
    uint32_t color_filter_opa :5;
    uint32_t bg_opa :5;
    uint32_t border_opa:5;
    uint32_t img_opa:5;
    uint32_t bg_grad_color :4;
    uint32_t border_color:4;
    uint32_t border_width:4;

    /*32*/
    uint32_t pad_top:5;
    uint32_t pad_bottom:5;
    uint32_t pad_left:5;
    uint32_t pad_right:5;
    uint32_t text_color:4;
    uint32_t text_font :4;
    uint32_t line_color :4;

    /*32*/
    uint32_t text_opa:5;
    uint32_t line_width :5;
    uint32_t line_opa :5;
    uint32_t outline_width:5;
    uint32_t outline_pad:5;
    uint32_t outline_opa:5;
    uint32_t bg_grad_dir:2;

    /*25*/
    uint32_t shadow_width:5;
    uint32_t shadow_opa:4;
    uint32_t outline_color:4;
    uint32_t shadow_color:4;
    uint32_t line_rounded:1;
    uint32_t border_post:1;
    uint32_t clip_corner:1;
    uint32_t has_line_rounded:1;
    uint32_t has_clip_corner:1;
    uint32_t has_bg_grad_dir:1;
    uint32_t has_border_post:1;
    uint32_t dont_index:1;
} lv_style_t;

extern lv_style_class_t lv_style;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a style
 * @param style pointer to a style to initialize
 */
void lv_style_init(lv_style_t * style);

uint16_t lv_style_register_prop(bool inherit);

/**
 * Clear all properties from a style and all allocated memories.
 * @param style pointer to a style
 */
void lv_style_reset(lv_style_t * style);

void lv_style_set_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t value);

bool lv_style_get_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value);

void lv_style_transition_init(lv_style_transiton_t * tr, const lv_style_prop_t * props, const lv_anim_path_t * path, uint32_t time, uint32_t delay);

uint32_t lv_style_find_index_num(lv_style_value_t v);
uint32_t lv_style_find_index_color(lv_style_value_t v);

/**
 * Remove a property from a style
 * @param style pointer to a style
 * @param prop  a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @return true: the property was found and removed; false: the property wasn't found
 */
bool lv_style_remove_prop(lv_style_t * style, lv_style_prop_t prop);

static inline void lv_style_set_radius(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_RADIUS, v); }

static inline void lv_style_set_clip_corner(lv_style_t * style, bool value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_CLIP_CORNER, v); }

static inline void lv_style_set_transform_width(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_TRANSFORM_WIDTH, v); }

static inline void lv_style_set_transform_height(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_TRANSFORM_HEIGHT, v); }

static inline void lv_style_set_transform_zoom(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_TRANSFORM_ZOOM, v); }

static inline void lv_style_set_transform_angle(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_TRANSFORM_ANGLE, v); }

static inline void lv_style_set_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_OPA, v); }

static inline void lv_style_set_color_filter_cb(lv_style_t * style, lv_color_filter_cb_t value) {
  lv_style_value_t v = {.func = (void(*)(void))value}; lv_style_set_prop(style, LV_STYLE_COLOR_FILTER_CB, v); }

static inline void lv_style_set_color_filter_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_COLOR_FILTER_OPA, v); }

static inline void lv_style_set_pad_top(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_PAD_TOP, v); }

static inline void lv_style_set_pad_bottom(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_PAD_BOTTOM, v); }

static inline void lv_style_set_pad_left(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_PAD_LEFT, v); }

static inline void lv_style_set_pad_right(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_PAD_RIGHT, v); }

static inline void lv_style_set_pad_row(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_PAD_ROW, v); }

static inline void lv_style_set_pad_column(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_PAD_COLUMN, v); }

static inline void lv_style_set_bg_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_BG_COLOR, v); }

static inline void lv_style_set_bg_color_filtered(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_BG_COLOR_FILTERED, v); }

static inline void lv_style_set_bg_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_BG_OPA, v); }

static inline void lv_style_set_bg_grad_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_BG_GRAD_COLOR, v); }

static inline void lv_style_set_bg_grad_color_filtered(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_BG_GRAD_COLOR_FILTERED, v); }

static inline void lv_style_set_bg_grad_dir(lv_style_t * style, lv_grad_dir_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_BG_GRAD_DIR, v); }

static inline void lv_style_set_bg_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_BG_BLEND_MODE, v); }

static inline void lv_style_set_bg_main_stop(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_BG_MAIN_STOP, v); }

static inline void lv_style_set_bg_grad_stop(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_BG_GRAD_STOP, v); }

static inline void lv_style_set_border_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_BORDER_COLOR, v); }

static inline void lv_style_set_border_color_filtered(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_BORDER_COLOR_FILTERED, v); }

static inline void lv_style_set_border_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_BORDER_OPA, v); }

static inline void lv_style_set_border_width(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_BORDER_WIDTH, v); }

static inline void lv_style_set_border_side(lv_style_t * style, lv_border_side_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_BORDER_SIDE, v); }

static inline void lv_style_set_border_post(lv_style_t * style, bool value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_BORDER_POST, v); }

static inline void lv_style_set_border_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_BORDER_BLEND_MODE, v); }

static inline void lv_style_set_text_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_TEXT_COLOR, v); }

static inline void lv_style_set_text_color_filtered(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_TEXT_COLOR_FILTERED, v); }

static inline void lv_style_set_text_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_TEXT_OPA, v); }

static inline void lv_style_set_text_font(lv_style_t * style, const lv_font_t * value) {
  lv_style_value_t v = {.ptr = value}; lv_style_set_prop(style, LV_STYLE_TEXT_FONT, v); }

static inline void lv_style_set_text_letter_space(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_TEXT_LETTER_SPACE, v); }

static inline void lv_style_set_text_line_space(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_TEXT_LINE_SPACE, v); }

static inline void lv_style_set_text_decor(lv_style_t * style, lv_text_decor_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_TEXT_DECOR, v); }

static inline void lv_style_set_text_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_TEXT_BLEND_MODE, v); }

static inline void lv_style_set_img_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_IMG_OPA, v); }

static inline void lv_style_set_img_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_IMG_BLEND_MODE, v); }

static inline void lv_style_set_img_recolor(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_IMG_RECOLOR, v); }

static inline void lv_style_set_img_recolor_filtered(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_IMG_RECOLOR_FILTERED, v); }

static inline void lv_style_set_img_recolor_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_IMG_RECOLOR_OPA, v); }

static inline void lv_style_set_outline_width(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_OUTLINE_WIDTH, v); }

static inline void lv_style_set_outline_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_OUTLINE_COLOR, v); }

static inline void lv_style_set_outline_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_OUTLINE_OPA, v); }

static inline void lv_style_set_outline_pad(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_OUTLINE_PAD, v); }

static inline void lv_style_set_outline_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_OUTLINE_BLEND_MODE, v); }

static inline void lv_style_set_shadow_width(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_WIDTH, v); }

static inline void lv_style_set_shadow_ofs_x(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_OFS_X, v); }

static inline void lv_style_set_shadow_ofs_y(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_OFS_Y, v); }

static inline void lv_style_set_shadow_spread(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_SPREAD, v); }

static inline void lv_style_set_shadow_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_BLEND_MODE, v); }

static inline void lv_style_set_shadow_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_COLOR, v); }

static inline void lv_style_set_shadow_color_filtered(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_COLOR_FILTERED, v); }

static inline void lv_style_set_shadow_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_SHADOW_OPA, v); }

static inline void lv_style_set_line_width(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_LINE_WIDTH, v); }

static inline void lv_style_set_line_blend_mode(lv_style_t * style, lv_blend_mode_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_LINE_BLEND_MODE, v); }

static inline void lv_style_set_line_dash_width(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_LINE_DASH_WIDTH, v); }

static inline void lv_style_set_line_dash_gap(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_LINE_DASH_GAP, v); }

static inline void lv_style_set_line_rounded(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_LINE_ROUNDED, v); }

static inline void lv_style_set_line_color(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_LINE_COLOR, v); }

static inline void lv_style_set_line_color_filtered(lv_style_t * style, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_style_set_prop(style, LV_STYLE_LINE_COLOR_FILTERED, v); }

static inline void lv_style_set_line_opa(lv_style_t * style, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_LINE_OPA, v); }

static inline void lv_style_set_content_src(lv_style_t * style, const char * value) {
  lv_style_value_t v = {.ptr = value}; lv_style_set_prop(style, LV_STYLE_CONTENT_SRC, v); }

static inline void lv_style_set_content_align(lv_style_t * style, lv_align_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_CONTENT_ALIGN, v); }

static inline void lv_style_set_content_ofs_x(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_CONTENT_OFS_X, v); }

static inline void lv_style_set_content_ofs_y(lv_style_t * style, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_style_set_prop(style, LV_STYLE_CONTENT_OFS_Y, v); }

static inline void lv_style_set_transition(lv_style_t * style, const lv_style_transiton_t * value) {
  lv_style_value_t v = {.ptr = value}; lv_style_set_prop(style, LV_STYLE_TRANSITION, v); }


static inline void lv_style_set_pad_ver(lv_style_t * style, lv_coord_t value)
{
    lv_style_set_pad_top(style, value);
    lv_style_set_pad_bottom(style, value);
}

static inline void lv_style_set_pad_hor(lv_style_t * style, lv_coord_t value)
{
    lv_style_set_pad_left(style, value);
    lv_style_set_pad_right(style, value);
}

static inline void lv_style_set_pad_all(lv_style_t * style, lv_coord_t value)
{
    lv_style_set_pad_top(style, value);
    lv_style_set_pad_bottom(style, value);
    lv_style_set_pad_left(style, value);
    lv_style_set_pad_right(style, value);
}

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


uint32_t lv_style_find_index_num(lv_style_value_t v);
uint32_t lv_style_find_index_color(lv_style_value_t v);
uint32_t lv_style_find_index_ptr(lv_style_value_t v);
int32_t lv_style_get_indexed_num(uint32_t id);
lv_color_t lv_style_get_indexed_color(uint32_t id);
const void * lv_style_get_indexed_ptr(uint32_t id);

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

bool lv_style_is_empty(const lv_style_t * style);

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
