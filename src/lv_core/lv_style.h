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
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_font.h"
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
    const lv_style_t * style_start; /*Pointer to the starting style*/
    const lv_style_t * style_end;   /*Pointer to the destination style*/
    lv_style_t * style_anim;        /*Pointer to a style to animate*/
    lv_anim_ready_cb_t ready_cb;            /*Call it when the animation is ready (NULL if unused)*/
    int16_t time;                   /*Animation time in ms*/
    int16_t act_time;               /*Current time in animation. Set to negative to make delay.*/
    uint16_t playback_pause;        /*Wait before play back*/
    uint16_t repeat_pause;          /*Wait before repeat*/
#if LV_USE_USER_DATA_SINGLE
    lv_anim_user_data_t user_data;  /*Custom user data*/
#endif

#if LV_USE_USER_DATA_MULTI
    lv_anim_user_data_t ready_user_data;
#endif

    uint8_t playback : 1;           /*When the animation is ready play it back*/
    uint8_t repeat : 1;             /*Repeat the animation infinitely*/
} lv_style_anim_t;

/* Example initialization
lv_style_anim_t a;
a.style_anim = &style_to_anim;
a.style_start = &style_1;
a.style_end = &style_2;
a.act_time = 0;
a.time = 1000;
a.playback = 0;
a.playback_pause = 0;
a.repeat = 0;
a.repeat_pause = 0;
a.ready_cb = NULL;
a.user_data = NULL;
lv_style_anim_create(&a);
 */


static inline void lv_style_anim_init(lv_style_anim_t * a)
{
    memset(a, 0, sizeof(lv_style_anim_t));
}

static inline void lv_style_anim_set_styles(lv_style_anim_t * a, lv_style_t * to_anim, const lv_style_t * start, const lv_style_t * end)
{
    a->style_anim = to_anim;
    a->style_start = start;
    a->style_end = end;
}

static inline void lv_style_anim_set_time(lv_style_anim_t * a, uint16_t duration, uint16_t delay)
{
    a->time = duration;
    a->act_time = -delay;
}

static inline void lv_style_anim_set_ready_cb(lv_style_anim_t * a, lv_anim_ready_cb_t ready_cb)
{
    a->ready_cb = ready_cb;
}

static inline void lv_style_anim_set_playback(lv_style_anim_t * a, uint16_t wait_time)
{
    a->playback = 1;
    a->playback_pause = wait_time;
}

static inline void lv_style_anim_clear_playback(lv_style_anim_t * a)
{
    a->playback = 0;
}

static inline void lv_style_anim_set_repeat(lv_style_anim_t * a, uint16_t wait_time)
{
    a->repeat = 1;
    a->repeat_pause = wait_time;
}

static inline void lv_style_anim_clear_repeat(lv_style_anim_t * a)
{
    a->repeat = 0;
}

static inline void lv_style_anim_set_user_data(lv_style_anim_t * a, lv_anim_user_data_t user_data)
{
    memcpy(&a->user_data, &user_data, sizeof(user_data));
}

static inline lv_anim_user_data_t lv_style_anim_get_user_data(lv_style_anim_t * a)
{
    return a->user_data;
}

static inline lv_anim_user_data_t * lv_style_anim_get_user_data_ptr(lv_style_anim_t * a)
{
    return &a->user_data;
}


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
void lv_style_mix(const lv_style_t * start, const lv_style_t * end, lv_style_t * res,
                  uint16_t ratio);

#if LV_USE_ANIMATION

/**
 * Create an animation from a pre-configured 'lv_style_anim_t' variable
 * @param anim pointer to a pre-configured 'lv_style_anim_t' variable (will be copied)
 * @return pointer to a descriptor. Really this variable will be animated. (Can be used in
 * `lv_anim_del(dsc, NULL)`)
 */
void * lv_style_anim_create(lv_style_anim_t * anim);
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
