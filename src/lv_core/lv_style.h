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

/*********************
 *      DEFINES
 *********************/
#define LV_RADIUS_CIRCLE (LV_COORD_MAX) /*A very big radius to always draw as circle*/

/**********************
 *      TYPEDEFS
 **********************/

/*Border types (Use 'OR'ed values)*/
enum {
    LV_BORDER_NONE     = 0x00,
    LV_BORDER_BOTTOM   = 0x01,
    LV_BORDER_TOP      = 0x02,
    LV_BORDER_LEFT     = 0x04,
    LV_BORDER_RIGHT    = 0x08,
    LV_BORDER_FULL     = 0x0F,
    LV_BORDER_INTERNAL = 0x10, /*FOR matrix-like objects (e.g. Button matrix)*/
};
typedef uint8_t lv_border_part_t;

/*Shadow types*/
enum {
    LV_SHADOW_BOTTOM = 0,
    LV_SHADOW_FULL,
};
typedef uint8_t lv_shadow_type_t;

typedef struct
{
    uint8_t glass : 1; /*1: Do not inherit this style*/

    struct
    {
        lv_color_t main_color;
        lv_color_t grad_color; /*`grad_color` will be removed in v6.0, use `aux_color` instead*/
        lv_coord_t radius;
        lv_opa_t opa;

        struct
        {
            lv_color_t color;
            lv_coord_t width;
            lv_border_part_t part;
            lv_opa_t opa;
        } border;

        struct
        {
            lv_color_t color;
            lv_coord_t width;
            lv_shadow_type_t type;
        } shadow;

        struct
        {
            lv_coord_t top;
            lv_coord_t bottom;
            lv_coord_t left;
            lv_coord_t right;
            lv_coord_t inner;
        } padding;
    } body;

    struct
    {
        lv_color_t color;
        lv_color_t sel_color;
        const lv_font_t * font;
        lv_coord_t letter_space;
        lv_coord_t line_space;
        lv_opa_t opa;
    } text;

    struct
    {
        lv_color_t color;
        lv_opa_t intense;
        lv_opa_t opa;
    } image;

    struct
    {
        lv_color_t color;
        lv_coord_t width;
        lv_opa_t opa;
        uint8_t rounded : 1; /*1: rounded line endings*/
    } line;
} lv_style_t;

#if LV_USE_ANIMATION
typedef struct
{
    lv_style_t style_start; /*Save not only pointers because can be same as 'style_anim' then it
                               will be modified too*/
    lv_style_t style_end;
    lv_style_t * style_anim;
    lv_anim_ready_cb_t ready_cb;
} lv_style_anim_dsc_t;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 *  Init the basic styles
 */
void lv_style_init(void);

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
static inline void lv_style_anim_set_time(lv_anim_t * a, uint16_t duration, uint16_t delay)
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
 * Set a user specific data for the animation
 * @param a pointer to an initialized `lv_anim_t` variable
 * @param user_data the user data
 */
static inline void lv_style_anim_set_user_data(lv_anim_t * a, lv_anim_user_data_t user_data)
{
    lv_anim_set_user_data(a, user_data);
}

/**
 * Get the user data
 * @param a pointer to an initialized `lv_anim_t` variable
 * @return the user data
 */
static inline lv_anim_user_data_t lv_style_anim_get_user_data(lv_anim_t * a)
{
    return lv_anim_get_user_data(a);
}

/**
 * Get pointer to the user data
 * @param a pointer to an initialized `lv_anim_t` variable
 * @return pointer to the user data
 */
static inline lv_anim_user_data_t * lv_style_anim_get_user_data_ptr(lv_anim_t * a)
{
    return lv_anim_get_user_data_ptr(a);
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
extern lv_style_t lv_style_scr;
extern lv_style_t lv_style_transp;
extern lv_style_t lv_style_transp_fit;
extern lv_style_t lv_style_transp_tight;
extern lv_style_t lv_style_plain;
extern lv_style_t lv_style_plain_color;
extern lv_style_t lv_style_pretty;
extern lv_style_t lv_style_pretty_color;
extern lv_style_t lv_style_btn_rel;
extern lv_style_t lv_style_btn_pr;
extern lv_style_t lv_style_btn_tgl_rel;
extern lv_style_t lv_style_btn_tgl_pr;
extern lv_style_t lv_style_btn_ina;

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_STYLE_H*/
