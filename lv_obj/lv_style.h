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
    LV_TXT_ALIGN_LEFT = 0,
    LV_TXT_ALIGN_MID,
}lv_txt_align_t;


/*Shadow types*/
typedef enum
{
    LV_STYPE_BOTTOM = 0,
    LV_STYPE_FULL,
}lv_stype_t;

typedef struct
{
    /*Object level styles*/
    color_t ccolor;     /*Content color (e.g. text or image re-color )*/
    opa_t opa;          /*Opacity of the object*/
    uint8_t glass :1;   /*1: Do not inherit this style*/
    uint8_t empty :1;   /*Transparent background (border drawn)*/
    uint8_t stype :2;   /*Shadow type from 'lv_shadow_type_t'*/
    uint8_t txt_align:2;
    color_t mcolor;     /*Main color of background*/
    color_t gcolor;     /*Gradient color of background*/
    color_t bcolor;     /*Border color of background*/
    color_t scolor;     /*Shadow color of background*/
    cord_t radius;      /*Corner radius of background*/
    cord_t bwidth;      /*Width of the background border*/
    cord_t swidth;      /*Width of the background shadow effect*/
    cord_t vpad;        /*Vertical padding*/
    cord_t hpad;        /*Horizontal padding*/
    cord_t opad;        /*Object padding on the background*/
    opa_t bopa;         /*Opacity of background border*/
    const font_t * font;
    cord_t letter_space;
    cord_t line_space;
    opa_t img_recolor;
    cord_t line_width;
}lv_style_t;

typedef enum {
    LV_STYLE_SCR,
    LV_STYLE_TRANSP,
    LV_STYLE_TRANSP_TIGHT,
    LV_STYLE_PLAIN,
    LV_STYLE_PLAIN_COLOR,
    LV_STYLE_PRETTY,
    LV_STYLE_PRETTY_COLOR,
    LV_STYLE_BTN_REL,
    LV_STYLE_BTN_PR,
    LV_STYLE_BTN_TREL,
    LV_STYLE_BTN_TPR,
    LV_STYLE_BTN_INA,
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
lv_style_t * lv_style_get(lv_style_name_t style_name, lv_style_t * copy);


/**
 * Copy a style to an other
 * @param dest pointer to the destination style
 * @param src pointer to the source style
 */
void lv_style_cpy(lv_style_t * dest, const lv_style_t * src);

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
