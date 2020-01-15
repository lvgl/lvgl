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
#include "../lv_draw/lv_draw_blend.h"

/*********************
 *      DEFINES
 *********************/
#define LV_RADIUS_CIRCLE (LV_COORD_MAX) /**< A very big radius to always draw as circle*/

LV_EXPORT_CONST_INT(LV_RADIUS_CIRCLE);

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
};
typedef uint8_t lv_border_side_t;

enum {
    LV_GRAD_DIR_NONE,
    LV_GRAD_DIR_VER,
    LV_GRAD_DIR_HOR,
};

typedef uint8_t lv_grad_dir_t;

#define LV_STYLE_PROP_INIT(name, group, id, attr)  name = (((group << 4) + id) | ((attr) << 8))

#define LV_STYLE_ID_MASK 0x00FF

#define LV_STYLE_ATTR_NONE          0
#define LV_STYLE_ATTR_INHERIT       (1 << 7)

typedef union {
    struct {
        uint8_t state       :7; /* To which state the property refers to*/
        uint8_t inherit     :1; /*1: The property can be inherited*/
    }bits;
    uint8_t full;
}lv_style_attr_t;


#define LV_STYLE_ID_VALUE 0x0   /*max 6 pcs*/
#define LV_STYLE_ID_COLOR 0x6   /*max 4 pcs*/
#define LV_STYLE_ID_OPA   0xA   /*max 4 pcs*/
#define LV_STYLE_ID_PTR   0xE   /*max 2 pcs*/

enum {
    LV_STYLE_PROP_INIT(LV_STYLE_RADIUS,             0x0, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_CLIP_CORNER,        0x0, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_TRANSITION_TIME,    0x0, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_OPA_SCALE,          0x0, LV_STYLE_ID_OPA + 0,   LV_STYLE_ATTR_INHERIT),

    LV_STYLE_PROP_INIT(LV_STYLE_PAD_TOP,            0x1, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PAD_BOTTOM,         0x1, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PAD_LEFT,           0x1, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PAD_RIGHT,          0x1, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PAD_INNER,          0x1, LV_STYLE_ID_VALUE + 4, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_BG_BLEND_MODE,      0x2, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BG_MAIN_STOP,      0x2, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BG_GRAD_STOP,       0x2, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BG_GRAD_DIR,        0x2, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BG_COLOR,           0x2, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BG_GRAD_COLOR,      0x2, LV_STYLE_ID_COLOR + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BG_OPA,             0x2, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_BORDER_WIDTH,       0x3, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BORDER_SIDE,        0x3, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BORDER_BLEND_MODE,  0x3, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BORDER_COLOR,       0x3, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_BORDER_OPA,         0x3, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_WIDTH,       0x4, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_OFFSET_X,    0x4, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_OFFSET_Y,    0x4, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_SPREAD,      0x4, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_BLEND_MODE,  0x4, LV_STYLE_ID_VALUE + 4, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_COLOR,       0x4, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SHADOW_OPA,         0x4, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_PATTERN_REPEATE,       0x5, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PATTERN_BLEND_MODE,    0x5, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PATTERN_RECOLOR,       0x5, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PATTERN_OPA,           0x5, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PATTERN_RECOLOR_OPA,   0x5, LV_STYLE_ID_OPA   + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_PATTERN_IMAGE,         0x5, LV_STYLE_ID_PTR   + 0, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_LETTER_SPACE,       0x6, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_LINE_SPACE,         0x6, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_BLEND_MODE,    0x6, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_COLOR,         0x6, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_SEL_COLOR,     0x6, LV_STYLE_ID_COLOR + 1, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_TEXT_OPA,           0x6, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_FONT,               0x6, LV_STYLE_ID_PTR   + 0, LV_STYLE_ATTR_INHERIT),

    LV_STYLE_PROP_INIT(LV_STYLE_LINE_WIDTH,         0x7, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_LINE_BLEND_MODE,    0x7, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_LINE_DASH_WIDTH,   0x7, LV_STYLE_ID_VALUE + 2, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_LINE_DASH_GAP,      0x7, LV_STYLE_ID_VALUE + 3, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_LINE_COLOR,         0x7, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_LINE_OPA,           0x7, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_NONE),

    LV_STYLE_PROP_INIT(LV_STYLE_IMAGE_BLEND_MODE,   0x8, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_IMAGE_RECOLOR,      0x8, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_IMAGE_OPA,          0x8, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_IMAGE_RECOLOR_OPA,  0x8, LV_STYLE_ID_OPA   + 1, LV_STYLE_ATTR_INHERIT),

    LV_STYLE_PROP_INIT(LV_STYLE_OVERLAY_COLOR,      0x9, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_INHERIT),
    LV_STYLE_PROP_INIT(LV_STYLE_OVERLAY_OPA,        0x9, LV_STYLE_ID_OPA   + 0, LV_STYLE_ATTR_INHERIT),

    LV_STYLE_PROP_INIT(LV_STYLE_SIZE,                0xA, LV_STYLE_ID_VALUE + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SCALE_WIDTH,         0xA, LV_STYLE_ID_VALUE + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SCALE_COLOR,         0xA, LV_STYLE_ID_COLOR + 0, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SCALE_GRAD_COLOR,    0xA, LV_STYLE_ID_COLOR + 1, LV_STYLE_ATTR_NONE),
    LV_STYLE_PROP_INIT(LV_STYLE_SCALE_END_COLOR,     0xA, LV_STYLE_ID_COLOR + 2, LV_STYLE_ATTR_NONE),
};


typedef uint16_t lv_style_property_t;


#define LV_STYLE_STATE_POS       8
#define LV_STYLE_STATE_MASK      0x7F00

enum {
    LV_STYLE_STATE_CHECKED  =         (1 << (0 + LV_STYLE_STATE_POS)),
    LV_STYLE_STATE_FOCUS  =           (1 << (1 + LV_STYLE_STATE_POS)),
    LV_STYLE_STATE_EDIT  =            (1 << (2 + LV_STYLE_STATE_POS)),
    LV_STYLE_STATE_HOVER  =           (1 << (3 + LV_STYLE_STATE_POS)),
    LV_STYLE_STATE_PRESSED  =         (1 << (4 + LV_STYLE_STATE_POS)),
    LV_STYLE_STATE_DISABLED =         (1 << (6 + LV_STYLE_STATE_POS)),
    LV_STYLE_STATE_ALL =              (0x7F << LV_STYLE_STATE_POS)
};

typedef uint16_t lv_style_state_t;

typedef struct {
    uint8_t * map;
    uint16_t size       :9;
}lv_style_t;

typedef int16_t lv_style_int_t;


typedef struct {
    lv_style_t  local;
    lv_style_t ** classes;
    uint8_t class_cnt;
}lv_style_dsc_t;



#if LV_USE_ANIMATION
/** Data structure for style animations. */
typedef struct
{
    lv_style_t style_start; /* Save not only pointers because can be same as 'style_anim' and
                               then it would be modified too*/
    lv_style_t style_end;
    lv_style_t * style_anim;
    lv_style_property_t * prop_list;
    lv_anim_ready_cb_t ready_cb;
} lv_style_anim_dsc_t;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 *  Init. the built-in styles
 */
void lv_style_built_in_init(void);

void lv_style_init(lv_style_t * style);

void lv_style_dsc_init(lv_style_dsc_t * style_dsc);

void lv_style_dsc_add_class(lv_style_dsc_t * style_dsc, lv_style_t * style);

void lv_style_dsc_remove_class(lv_style_dsc_t * style_dsc, lv_style_t * class);

void lv_style_dsc_reset(lv_style_dsc_t * style_dsc);

static inline lv_style_t * lv_style_dsc_get_class(lv_style_dsc_t * style_dsc, uint8_t id)
{
    if(style_dsc->class_cnt == 0) return NULL;
    if(style_dsc->class_cnt == 1) return (lv_style_t*) style_dsc->classes;

    return style_dsc->classes[id];
}

void lv_style_reset(lv_style_t * style);

/**
 * Copy a style to an other
 * @param dest pointer to the destination style
 * @param src pointer to the source style
 */
void lv_style_copy(lv_style_t * dest, const lv_style_t * src);

/**
 * Mix two styles according to a given ratio
 * @param start start style
 * @param end end style
 * @param res store the result style here
 * @param ratio the ratio of mix [0..256]; 0: `start` style; 256: `end` style
 */
void lv_style_mix(const lv_style_t * start, const lv_style_t * end, lv_style_t * res, uint16_t ratio);

void lv_style_set_int(lv_style_t * style, lv_style_property_t prop, lv_style_int_t value);
void lv_style_set_color(lv_style_t * style, lv_style_property_t prop, lv_color_t color);
void lv_style_set_opa(lv_style_t * style, lv_style_property_t prop, lv_opa_t opa);
void lv_style_set_ptr(lv_style_t * style, lv_style_property_t prop, void * p);


int16_t lv_style_get_int(const lv_style_t * style, lv_style_property_t prop, lv_style_int_t * res);
int16_t lv_style_get_opa(const lv_style_t * style, lv_style_property_t prop, lv_opa_t * res);
int16_t lv_style_get_color(const lv_style_t * style, lv_style_property_t prop, lv_color_t * res);
int16_t lv_style_get_ptr(const lv_style_t * style, lv_style_property_t prop, void ** res);

lv_res_t lv_style_dsc_get_int(lv_style_dsc_t * dsc, lv_style_property_t prop, lv_style_int_t * value);
lv_res_t lv_style_dsc_get_color(lv_style_dsc_t * dsc, lv_style_property_t prop, lv_color_t * value);
lv_res_t lv_style_dsc_get_opa(lv_style_dsc_t * dsc, lv_style_property_t prop, lv_opa_t * value);
lv_res_t lv_style_dsc_get_ptr(lv_style_dsc_t * dsc, lv_style_property_t prop, void ** value);

lv_res_t lv_style_cache_update(lv_style_dsc_t * dsc);

#if LV_USE_ANIMATION

/**
 * Initialize an animation variable.
 * E.g.:
 * lv_anim_t a;
 * lv_style_anim__init(&a);
 * lv_style_anim_set_...(&a);
 * lv_style_anim_create(&a);
 * @param a pointer to an `lv_anim_t` variable to initialize
 */
void lv_style_anim_init(lv_anim_t * a);

/**
 *
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param to_anim pointer to the style to animate
 * @param start pointer to a style to animate from (start value)
 * @param end pointer to a style to animate to (end value)
 */
void lv_style_anim_set_styles(lv_anim_t * a, lv_style_t * to_anim, const lv_style_t * start, const lv_style_t * end);

/**
 * Set the duration and delay of an animation
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param duration duration of the animation in milliseconds
 * @param delay delay before the animation in milliseconds
 */
static inline void lv_style_anim_set_time(lv_anim_t * a, uint16_t duration, int16_t delay)
{
    lv_anim_set_time(a, duration, delay);
}

/**
 * Set a function call when the animation is ready
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param ready_cb a function call when the animation is ready
 */
static inline void lv_style_anim_set_ready_cb(lv_anim_t * a, lv_anim_ready_cb_t ready_cb)
{
    lv_style_anim_dsc_t * dsc = (lv_style_anim_dsc_t *)a->var;
    dsc->ready_cb             = ready_cb;
}

/**
 * Make the animation to play back to when the forward direction is ready
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param wait_time time in milliseconds to wait before starting the back direction
 */
static inline void lv_style_anim_set_playback(lv_anim_t * a, uint16_t wait_time)
{
    lv_anim_set_playback(a, wait_time);
}

/**
 * Disable playback. (Disabled after `lv_anim_init()`)
 * @param a pointer to an initialized `lv_anim_t` variable
 */
static inline void lv_style_anim_clear_playback(lv_anim_t * a)
{
    lv_anim_clear_playback(a);
}

/**
 * Make the animation to start again when ready.
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param wait_time time in milliseconds to wait before starting the animation again
 */
static inline void lv_style_anim_set_repeat(lv_anim_t * a, uint16_t wait_time)
{
    lv_anim_set_repeat(a, wait_time);
}

/**
 * Disable repeat. (Disabled after `lv_anim_init()`)
 * @param a pointer to an initialized `lv_anim_t` variable
 */
static inline void lv_style_anim_clear_repeat(lv_anim_t * a)
{
    lv_anim_clear_repeat(a);
}

/**
 * Create an animation
 * @param a an initialized 'anim_t' variable. Not required after call.
 */
static inline void lv_style_anim_create(lv_anim_t * a)
{
    lv_anim_create(a);
}

#endif

/*************************
 *    GLOBAL VARIABLES
 *************************/
//extern lv_style_t lv_style_transp_tight;

/**********************
 *      MACROS
 **********************/

/**
 * Create and initialize a `static` style
 * Example:
 *     LV_STYLE_CREATE(my_style, &lv_style_plain);
 *   is equivalent to
 *     static lv_style_t my_style;
 *     lv_style_init(&my_style);
 */
#define LV_STYLE_CREATE(name, copy_p) static lv_style_t name; lv_style_init(&name);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_STYLE_H*/
