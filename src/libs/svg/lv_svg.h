/**
 * @file lv_svg.h
 *
 */

#ifndef LV_SVG_H
#define LV_SVG_H

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_SVG

#include "../../misc/lv_array.h"
#include "../../misc/lv_tree.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
enum _lv_svg_tag_t {
    LV_SVG_TAG_INVALID = -1,
    LV_SVG_TAG_CONTENT,
    LV_SVG_TAG_SVG,
    LV_SVG_TAG_USE,
    LV_SVG_TAG_G,
    LV_SVG_TAG_PATH,
    LV_SVG_TAG_RECT,
    LV_SVG_TAG_CIRCLE,
    LV_SVG_TAG_ELLIPSE,
    LV_SVG_TAG_LINE,
    LV_SVG_TAG_POLYLINE,
    LV_SVG_TAG_POLYGON,
    LV_SVG_TAG_SOLID_COLOR,
    LV_SVG_TAG_LINEAR_GRADIENT,
    LV_SVG_TAG_RADIAL_GRADIENT,
    LV_SVG_TAG_STOP,
    LV_SVG_TAG_DEFS,
    LV_SVG_TAG_IMAGE,
#if LV_USE_SVG_ANIMATION
    LV_SVG_TAG_MPATH,
    LV_SVG_TAG_SET,
    LV_SVG_TAG_ANIMATE,
    LV_SVG_TAG_ANIMATE_COLOR,
    LV_SVG_TAG_ANIMATE_TRANSFORM,
    LV_SVG_TAG_ANIMATE_MOTION,
#endif
    LV_SVG_TAG_TEXT,
    LV_SVG_TAG_TSPAN,
    LV_SVG_TAG_TEXT_AREA,
};
typedef int8_t lv_svg_tag_t;

enum _lv_svg_attr_type_t {
    LV_SVG_ATTR_INVALID = 0,
    LV_SVG_ATTR_ID,
    LV_SVG_ATTR_XML_ID,
    LV_SVG_ATTR_VERSION,
    LV_SVG_ATTR_BASE_PROFILE,
    LV_SVG_ATTR_VIEWBOX,
    LV_SVG_ATTR_PRESERVE_ASPECT_RATIO,
    LV_SVG_ATTR_VIEWPORT_FILL,
    LV_SVG_ATTR_VIEWPORT_FILL_OPACITY,
    LV_SVG_ATTR_DISPLAY,
    LV_SVG_ATTR_VISIBILITY,
    LV_SVG_ATTR_X,
    LV_SVG_ATTR_Y,
    LV_SVG_ATTR_WIDTH,
    LV_SVG_ATTR_HEIGHT,
    LV_SVG_ATTR_RX,
    LV_SVG_ATTR_RY,
    LV_SVG_ATTR_CX,
    LV_SVG_ATTR_CY,
    LV_SVG_ATTR_R,
    LV_SVG_ATTR_X1,
    LV_SVG_ATTR_Y1,
    LV_SVG_ATTR_X2,
    LV_SVG_ATTR_Y2,
    LV_SVG_ATTR_POINTS,
    LV_SVG_ATTR_D,
    LV_SVG_ATTR_PATH_LENGTH,
    LV_SVG_ATTR_XLINK_HREF,
    LV_SVG_ATTR_FILL,
    LV_SVG_ATTR_FILL_RULE,
    LV_SVG_ATTR_FILL_OPACITY,
    LV_SVG_ATTR_STROKE,
    LV_SVG_ATTR_STROKE_WIDTH,
    LV_SVG_ATTR_STROKE_LINECAP,
    LV_SVG_ATTR_STROKE_LINEJOIN,
    LV_SVG_ATTR_STROKE_MITER_LIMIT,
    LV_SVG_ATTR_STROKE_DASH_ARRAY,
    LV_SVG_ATTR_STROKE_DASH_OFFSET,
    LV_SVG_ATTR_STROKE_OPACITY,
    LV_SVG_ATTR_OPACITY,
    LV_SVG_ATTR_SOLID_COLOR,
    LV_SVG_ATTR_SOLID_OPACITY,
    LV_SVG_ATTR_GRADIENT_UNITS,
    LV_SVG_ATTR_GRADIENT_STOP_OFFSET,
    LV_SVG_ATTR_GRADIENT_STOP_COLOR,
    LV_SVG_ATTR_GRADIENT_STOP_OPACITY,
    LV_SVG_ATTR_FONT_FAMILY,
    LV_SVG_ATTR_FONT_STYLE,
    LV_SVG_ATTR_FONT_VARIANT,
    LV_SVG_ATTR_FONT_WEIGHT,
    LV_SVG_ATTR_FONT_SIZE,
    LV_SVG_ATTR_TRANSFORM,
    LV_SVG_ATTR_TEXT_ANCHOR,
#if LV_USE_SVG_ANIMATION
    LV_SVG_ATTR_ATTRIBUTE_NAME,
    LV_SVG_ATTR_ATTRIBUTE_TYPE,
    LV_SVG_ATTR_BEGIN,
    LV_SVG_ATTR_END,
    LV_SVG_ATTR_DUR,
    LV_SVG_ATTR_MIN,
    LV_SVG_ATTR_MAX,
    LV_SVG_ATTR_RESTART,
    LV_SVG_ATTR_REPEAT_COUNT,
    LV_SVG_ATTR_REPEAT_DUR,
    LV_SVG_ATTR_CALC_MODE,
    LV_SVG_ATTR_VALUES,
    LV_SVG_ATTR_KEY_TIMES,
    LV_SVG_ATTR_KEY_SPLINES,
    LV_SVG_ATTR_KEY_POINTS,
    LV_SVG_ATTR_FROM,
    LV_SVG_ATTR_TO,
    LV_SVG_ATTR_BY,
    LV_SVG_ATTR_ADDITIVE,
    LV_SVG_ATTR_ACCUMULATE,
    LV_SVG_ATTR_PATH,
    LV_SVG_ATTR_ROTATE,
    LV_SVG_ATTR_TRANSFORM_TYPE,
#endif
};
typedef uint8_t lv_svg_attr_type_t;

enum _lv_svg_transform_type_t {
    LV_SVG_TRANSFORM_TYPE_MATRIX = 1,
    LV_SVG_TRANSFORM_TYPE_TRANSLATE,
    LV_SVG_TRANSFORM_TYPE_ROTATE,
    LV_SVG_TRANSFORM_TYPE_SCALE,
    LV_SVG_TRANSFORM_TYPE_SKEW_X,
    LV_SVG_TRANSFORM_TYPE_SKEW_Y,
};
typedef uint8_t lv_svg_transform_type_t;

#if LV_USE_SVG_ANIMATION
enum lv_svg_anim_action_t {
    LV_SVG_ANIM_REMOVE = 0,
    LV_SVG_ANIM_FREEZE,
};

enum _lv_svg_anim_restart_type_t {
    LV_SVG_ANIM_RESTART_ALWAYS = 0,
    LV_SVG_ANIM_RESTART_WHEN_NOT_ACTIVE,
    LV_SVG_ANIM_RESTART_NEVER,
};

enum _lv_svg_anim_calc_mode_t {
    LV_SVG_ANIM_CALC_MODE_LINEAR = 0,
    LV_SVG_ANIM_CALC_MODE_PACED,
    LV_SVG_ANIM_CALC_MODE_SPLINE,
    LV_SVG_ANIM_CALC_MODE_DISCRETE,
};

enum _lv_svg_anim_additive_type_t {
    LV_SVG_ANIM_ADDITIVE_REPLACE = 0,
    LV_SVG_ANIM_ADDITIVE_SUM,
};

enum _lv_svg_anim_accumulate_type_t {
    LV_SVG_ANIM_ACCUMULATE_NONE = 0,
    LV_SVG_ANIM_ACCUMULATE_SUM,
};
#endif

enum _lv_svg_aspect_ratio_t {
    LV_SVG_ASPECT_RATIO_NONE = 0,
    LV_SVG_ASPECT_RATIO_XMIN_YMIN = (1 << 1),
    LV_SVG_ASPECT_RATIO_XMID_YMIN = (2 << 1),
    LV_SVG_ASPECT_RATIO_XMAX_YMIN = (3 << 1),
    LV_SVG_ASPECT_RATIO_XMIN_YMID = (4 << 1),
    LV_SVG_ASPECT_RATIO_XMID_YMID = (5 << 1),
    LV_SVG_ASPECT_RATIO_XMAX_YMID = (6 << 1),
    LV_SVG_ASPECT_RATIO_XMIN_YMAX = (7 << 1),
    LV_SVG_ASPECT_RATIO_XMID_YMAX = (8 << 1),
    LV_SVG_ASPECT_RATIO_XMAX_YMAX = (9 << 1),
};
typedef uint32_t lv_svg_aspect_ratio_t;

enum _lv_svg_aspect_ratio_opt_t {
    LV_SVG_ASPECT_RATIO_OPT_MEET = 0,
    LV_SVG_ASPECT_RATIO_OPT_SLICE,
};

typedef struct {
    float x;
    float y;
} lv_svg_point_t;

typedef struct {
    float m[3][3];
} lv_svg_matrix_t;

typedef uint32_t lv_svg_color_t;

enum _lv_svg_fill_rule_t {
    LV_SVG_FILL_NONZERO = 0,
    LV_SVG_FILL_EVENODD,
};
typedef uint8_t lv_svg_fill_rule_t;

enum _lv_svg_line_cap_t {
    LV_SVG_LINE_CAP_BUTT = 0,
    LV_SVG_LINE_CAP_SQUARE,
    LV_SVG_LINE_CAP_ROUND,
};
typedef uint8_t lv_svg_line_cap_t;

enum _lv_svg_line_join_t {
    LV_SVG_LINE_JOIN_MITER = 0,
    LV_SVG_LINE_JOIN_BEVEL,
    LV_SVG_LINE_JOIN_ROUND,
};
typedef uint8_t lv_svg_line_join_t;

enum _lv_svg_gradient_units_t {
    LV_SVG_GRADIENT_UNITS_OBJECT = 0,
    LV_SVG_GRADIENT_UNITS_USER_SPACE,
};
typedef uint8_t lv_svg_gradient_units_t;

typedef union {
    int32_t ival;
    uint32_t uval;
    float fval;
    char * sval;
    void * val;
} lv_svg_attr_value_t;

/*
 * to simplify list buffer management, allocate enough memory for all data and length.
 * | size | data[0] | data[1] | data[2] | ... |
 */
typedef struct {
    uint32_t length;
    uint8_t data[1];
} lv_svg_attr_values_list_t;

/* https://www.w3.org/TR/SVGTiny12/svgudomidl.html */
enum _lv_svg_path_cmd_t {
    LV_SVG_PATH_CMD_MOVE_TO = 77,
    LV_SVG_PATH_CMD_LINE_TO = 76,
    LV_SVG_PATH_CMD_CURVE_TO = 67,
    LV_SVG_PATH_CMD_QUAD_TO = 81,
    LV_SVG_PATH_CMD_CLOSE = 90,
};

/*
 * to simplify list buffer management, allocate enough memory for all path data and cmd.
 * | cmd | data[0] | data[1] | data[2] | ... |
 */
typedef struct {
    uint32_t cmd;
    uint8_t data[1];
} lv_svg_attr_path_value_t;

enum _lv_svg_attr_value_type_t {
    LV_SVG_ATTR_VALUE_DATA = 0,
    LV_SVG_ATTR_VALUE_PTR,
};
typedef uint8_t lv_svg_attr_value_type_t;

enum _lv_svg_attr_value_class_t {
    LV_SVG_ATTR_VALUE_NONE = 0,
    LV_SVG_ATTR_VALUE_INITIAL,
    LV_SVG_ATTR_VALUE_INHERIT,
};
typedef uint8_t lv_svg_attr_value_class_t;

typedef struct {
    lv_svg_attr_type_t id;
    lv_svg_attr_value_type_t val_type;
    lv_svg_attr_value_class_t class_type;
    lv_svg_attr_value_t value;
} lv_svg_attr_t;

struct _lv_svg_render_obj;

typedef struct {
    lv_tree_node_t base;
    char * xml_id; /* xml_id or content */
    lv_svg_tag_t type;
    lv_array_t attrs;
    struct _lv_svg_render_obj * render_obj;
} lv_svg_node_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Loading SVG data and creating the DOM tree
 * @param svg_data pointer to the SVG data
 * @param data_len the SVG data length
 */
lv_svg_node_t * lv_svg_load_data(const char * svg_data, uint32_t data_len);

/**
 * @brief Create an SVG DOM node
 * @param parent pointer to the parent node
 * @return true: an new SVG DOM node, false: NULL
 */
lv_svg_node_t * lv_svg_node_create(lv_svg_node_t * parent);

/**
 * @brief Delete an SVG DOM subtree
 * @param node pointer to an SVG DOM subtree
 */
void lv_svg_node_delete(lv_svg_node_t * node);

/**********************
 *      MACROS
 **********************/
#define LV_SVG_NODE_CHILD(n, i) \
    ((lv_svg_node_t *)(LV_TREE_NODE((n))->children[i]))

#define LV_SVG_NODE(n) ((lv_svg_node_t*)(n))

#endif /*LV_USE_SVG*/

#endif /*LV_SVG_H*/
