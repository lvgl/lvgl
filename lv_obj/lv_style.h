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
#include "misc/gfx/color.h"
#include "misc/gfx/area.h"
#include "misc/gfx/font.h"
#include "misc/gfx/anim.h"

/*********************
 *      DEFINES
 *********************/
#define LV_RADIUS_CIRCLE      (CORD_MAX)    /*A very big radius to always draw as circle*/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_TEXT_ALIGN_LEFT = 0,
    LV_TEXT_ALIGN_MID,
}lv_text_align_t;

/*Shadow types*/
typedef enum
{
    LV_SHADOW_BOTTOM = 0,
    LV_SHADOW_FULL,
}lv_shadow_type_t;

typedef struct
{
    opa_t opa;
    uint8_t glass :1;   /*1: Do not inherit this style*/

    struct {
        color_t color_main;
        color_t color_gradient;
        cord_t radius;

        struct {
            color_t color;
            cord_t width;
            opa_t opa;
        }border;

        struct {
            color_t color;
            cord_t width;
            uint8_t type;
        }shadow;

        struct {
            cord_t ver;
            cord_t hor;
            cord_t inner;
        }padding;

        uint8_t empty :1;   /*Transparent background (border still drawn)*/
    }body;


    struct {
        color_t color;
        const font_t * font;
        cord_t space_letter;
        cord_t space_line;
        uint8_t align:2;
    }text;

    struct {
        color_t color;
        opa_t intense;
    }image;

    struct {
        color_t color;
        cord_t width;
    }line;
}lv_style_t;

typedef enum {
    LV_STYLE_SCREEN,
    LV_STYLE_TRANSPARENT,
    LV_STYLE_TRANSPARENT_TIGHT,
    LV_STYLE_PLAIN,
    LV_STYLE_PLAIN_COLOR,
    LV_STYLE_PRETTY,
    LV_STYLE_PRETTY_COLOR,
    LV_STYLE_BUTTON_OFF_RELEASED,
    LV_STYLE_BUTTON_OFF_PRESSED,
    LV_STYLE_BUTTON_ON_RELEASED,
    LV_STYLE_BUTTON_ON_PRESSED,
    LV_STYLE_BUTTON_INACTIVE,
}lv_style_name_t;


typedef struct {
    const lv_style_t * style_start; /*Pointer to the starting style*/
    const lv_style_t * style_end;   /*Pointer to the destination style*/
    lv_style_t * style_anim;        /*Pointer to a style to animate*/
    anim_cb_t end_cb;               /*Call it when the animation is ready*/
    int16_t time;                   /*Animation time in ms*/
    int16_t act_time;               /*Current time in animation. Set to negative to make delay.*/
    uint16_t playback_pause;        /*Wait before play back*/
    uint16_t repeat_pause;          /*Wait before repeat*/
    uint8_t playback :1;            /*When the animation is ready play it back*/
    uint8_t repeat :1;              /*Repeat the animation infinitely*/
}lv_style_anim_t;

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
a.end_cb = NULL;
lv_style_anim_create(&a);
 */

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 *  Init the basic styles
 */
void lv_style_init (void);

/**
 * Get style from its name
 * @param style_name an element of the 'lv_style_name_t' enum
 * @return pointer to the requested style (lv_style_def by default)
 */
lv_style_t * lv_style_get(lv_style_name_t style_name);


/**
 * Copy a style to an other
 * @param dest pointer to the destination style
 * @param src pointer to the source style
 */
void lv_style_copy(lv_style_t * dest, const lv_style_t * src);

/**
 * Create an animation from a pre-configured 'lv_style_anim_t' variable
 * @param anim pointer to a pre-configured 'lv_style_anim_t' variable (will be copied)
 */
void lv_style_anim_create(lv_style_anim_t * anim);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_STYLE_H*/
