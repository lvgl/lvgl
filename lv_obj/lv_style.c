/**
 * @file lv_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "lv_style.h"
#include "lv_obj.h"
#include "misc/mem/dyn_mem.h"

/*********************
 *      DEFINES
 *********************/
#define LV_STYLE_ANIM_RES       256    /*Animation max in 1024 steps*/
#define LV_STYLE_ANIM_SHIFT     8      /*log2(LV_STYLE_ANIM_RES)*/

#define VAL_PROP(v1, v2, r)   v1 + (((v2-v1) * r) >> LV_STYLE_ANIM_SHIFT)
#define STYLE_ATTR_ANIM(attr, r)   if(start->attr != end->attr) act->attr = VAL_PROP(start->attr, end->attr, r)

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_style_t style_start;   /*Save not only pointers because if same as 'style_anim' then it will be modified too*/
    lv_style_t style_end;
    lv_style_t * style_anim;
}lv_style_anim_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_style_aimator(lv_style_anim_dsc_t * dsc, int32_t val);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_style_t lv_style_scr;
lv_style_t lv_style_transp;
lv_style_t lv_style_transp_fit;
lv_style_t lv_style_transp_tight;
lv_style_t lv_style_plain;
lv_style_t lv_style_plain_color;
lv_style_t lv_style_pretty;
lv_style_t lv_style_pretty_color;
lv_style_t lv_style_btn_released;
lv_style_t lv_style_btn_pressed;
lv_style_t lv_style_btn_tgl_released;
lv_style_t lv_style_btn_tgl_pressed;
lv_style_t lv_style_btn_inactive;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 *  Init the basic styles
 */
void lv_style_init (void)
{
    /* Not White/Black/Gray colors are created by HSV model with
     * HUE = 210*/

    /*Screen style*/
    lv_style_scr.glass = 0;
    lv_style_scr.body.opa = OPA_COVER;
    lv_style_scr.body.main_color = COLOR_WHITE;
    lv_style_scr.body.gradient_color = COLOR_WHITE;
    lv_style_scr.body.radius = 0;
    lv_style_scr.body.padding.ver = LV_DPI / 12;
    lv_style_scr.body.padding.hor = LV_DPI / 12;
    lv_style_scr.body.padding.inner = LV_DPI / 12;

    lv_style_scr.body.border.color = COLOR_BLACK;
    lv_style_scr.body.border.opa = OPA_COVER;
    lv_style_scr.body.border.width = 0;
    lv_style_scr.body.border.part = LV_BORDER_FULL;

    lv_style_scr.body.shadow.color = COLOR_GRAY;
    lv_style_scr.body.shadow.type = LV_SHADOW_FULL;
    lv_style_scr.body.shadow.width = 0;

    lv_style_scr.text.opa = OPA_COVER;
    lv_style_scr.text.color = COLOR_MAKE(0x30, 0x30, 0x30);
    lv_style_scr.text.font = FONT_DEFAULT;
    lv_style_scr.text.letter_space = 1 << LV_ANTIALIAS;
    lv_style_scr.text.line_space = 2 << LV_ANTIALIAS;

    lv_style_scr.image.opa = OPA_COVER;
    lv_style_scr.image.color = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_scr.image.intense = OPA_TRANSP;

    lv_style_scr.line.opa = OPA_COVER;
    lv_style_scr.line.color = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_scr.line.width = 1 << LV_ANTIALIAS;

    /*Plain style (by default near the same as the screen style)*/
    memcpy(&lv_style_plain, &lv_style_scr, sizeof(lv_style_t));

    /*Plain color style*/
    memcpy(&lv_style_plain_color, &lv_style_plain, sizeof(lv_style_t));
    lv_style_plain_color.text.color = COLOR_MAKE(0xf0, 0xf0, 0xf0);
    lv_style_plain_color.image.color = COLOR_MAKE(0xf0, 0xf0, 0xf0);
    lv_style_plain_color.line.color = COLOR_MAKE(0xf0, 0xf0, 0xf0);
    lv_style_plain_color.body.main_color = COLOR_MAKE(0x55, 0x96, 0xd8);
    lv_style_plain_color.body.gradient_color = lv_style_plain_color.body.main_color;

    /*Pretty style */
    memcpy(&lv_style_pretty, &lv_style_plain, sizeof(lv_style_t));
    lv_style_pretty.text.color = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_pretty.image.color = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_pretty.line.color = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_pretty.body.main_color = COLOR_WHITE;
    lv_style_pretty.body.gradient_color = COLOR_SILVER;
    lv_style_pretty.body.radius = LV_DPI / 15;
    lv_style_pretty.body.border.color = COLOR_MAKE(0x40, 0x40, 0x40);
    lv_style_pretty.body.border.width = LV_DPI / 50 >= 1 ? LV_DPI / 50  : 1;
    lv_style_pretty.body.border.opa = OPA_30;

    /*Pretty color style*/
    memcpy(&lv_style_pretty_color, &lv_style_pretty, sizeof(lv_style_t));
    lv_style_pretty_color.text.color = COLOR_MAKE(0xc0, 0xc0, 0xc0);
    lv_style_pretty_color.image.color = COLOR_MAKE(0xe0, 0xe0, 0xe0);
    lv_style_pretty_color.line.color = COLOR_MAKE(0xc0, 0xc0, 0xc0);
    lv_style_pretty_color.body.main_color = COLOR_MAKE(0x6b, 0x9a, 0xc7);
    lv_style_pretty_color.body.gradient_color = COLOR_MAKE(0x2b, 0x59, 0x8b);
    lv_style_pretty_color.body.border.color = COLOR_MAKE(0x15, 0x2c, 0x42);

    /*Transparent style*/
    memcpy(&lv_style_transp, &lv_style_plain, sizeof(lv_style_t));
    lv_style_transp.body.empty = 1;
    lv_style_transp.glass = 1;
    lv_style_transp.body.border.width = 0;

    /*Transparent tight style*/
    memcpy(&lv_style_transp_fit, &lv_style_transp, sizeof(lv_style_t));
    lv_style_transp_fit.body.padding.hor = 0;
    lv_style_transp_fit.body.padding.ver = 0;

    /*Transparent fitting size*/
    memcpy(&lv_style_transp_tight, &lv_style_transp_fit, sizeof(lv_style_t));
    lv_style_transp_tight.body.padding.inner = 0;

    /*Button released style*/
    memcpy(&lv_style_btn_released, &lv_style_plain, sizeof(lv_style_t));
    lv_style_btn_released.body.main_color = COLOR_MAKE(0x76, 0xa2, 0xd0);
    lv_style_btn_released.body.gradient_color = COLOR_MAKE(0x19, 0x3a, 0x5d);
    lv_style_btn_released.body.radius = LV_DPI / 15;
    lv_style_btn_released.body.padding.hor = LV_DPI / 4;
    lv_style_btn_released.body.padding.ver = LV_DPI / 6;
    lv_style_btn_released.body.padding.inner = LV_DPI / 10;
    lv_style_btn_released.body.border.color = COLOR_MAKE(0x0b, 0x19, 0x28);
    lv_style_btn_released.body.border.width = LV_DPI / 50 >= 1 ? LV_DPI / 50  : 1;
    lv_style_btn_released.body.border.opa = OPA_70;
    lv_style_btn_released.text.color = COLOR_MAKE(0xff, 0xff, 0xff);
    lv_style_btn_released.body.shadow.color = COLOR_GRAY;
    lv_style_btn_released.body.shadow.width = 0;

    /*Button pressed style*/
    memcpy(&lv_style_btn_pressed, &lv_style_btn_released, sizeof(lv_style_t));
    lv_style_btn_pressed.body.main_color = COLOR_MAKE(0x33, 0x62, 0x94);
    lv_style_btn_pressed.body.gradient_color = COLOR_MAKE(0x10, 0x26, 0x3c);
    lv_style_btn_pressed.text.color = COLOR_MAKE(0xa4, 0xb5, 0xc6);
    lv_style_btn_pressed.image.color = COLOR_MAKE(0xa4, 0xb5, 0xc6);
    lv_style_btn_pressed.line.color = COLOR_MAKE(0xa4, 0xb5, 0xc6);

    /*Button toggle released style*/
    memcpy(&lv_style_btn_tgl_released, &lv_style_btn_released, sizeof(lv_style_t));
    lv_style_btn_tgl_released.body.main_color = COLOR_MAKE(0x0a, 0x11, 0x22);
    lv_style_btn_tgl_released.body.gradient_color = COLOR_MAKE(0x37, 0x62, 0x90);
    lv_style_btn_tgl_released.body.border.color = COLOR_MAKE(0x01, 0x07, 0x0d);
    lv_style_btn_tgl_released.text.color = COLOR_MAKE(0xc8, 0xdd, 0xf4);
    lv_style_btn_tgl_released.image.color = COLOR_MAKE(0xc8, 0xdd, 0xf4);
    lv_style_btn_tgl_released.line.color = COLOR_MAKE(0xc8, 0xdd, 0xf4);

    /*Button toggle pressed style*/
    memcpy(&lv_style_btn_tgl_pressed, &lv_style_btn_tgl_released, sizeof(lv_style_t));
    lv_style_btn_tgl_pressed.body.main_color = COLOR_MAKE(0x02, 0x14, 0x27);
    lv_style_btn_tgl_pressed.body.gradient_color = COLOR_MAKE(0x2b, 0x4c, 0x70);
    lv_style_btn_tgl_pressed.text.color = COLOR_MAKE(0xa4, 0xb5, 0xc6);
    lv_style_btn_tgl_pressed.image.color = COLOR_MAKE(0xa4, 0xb5, 0xc6);
    lv_style_btn_tgl_pressed.line.color = COLOR_MAKE(0xa4, 0xb5, 0xc6);

    /*Button inactive style*/
    memcpy(&lv_style_btn_inactive, &lv_style_btn_released, sizeof(lv_style_t));
    lv_style_btn_inactive.body.main_color = COLOR_MAKE(0xd8, 0xd8, 0xd8);
    lv_style_btn_inactive.body.gradient_color = COLOR_MAKE(0xd8, 0xd8, 0xd8);
    lv_style_btn_inactive.body.border.color = COLOR_MAKE(0x90, 0x90, 0x90);
    lv_style_btn_inactive.text.color = COLOR_MAKE(0x70, 0x70, 0x70);
    lv_style_btn_inactive.image.color = COLOR_MAKE(0x70, 0x70, 0x70);
    lv_style_btn_inactive.line.color = COLOR_MAKE(0x70, 0x70, 0x70);
}


/**
 * Copy a style to an other
 * @param dest pointer to the destination style
 * @param src pointer to the source style
 */
void lv_style_copy(lv_style_t * dest, const lv_style_t * src)
{
    memcpy(dest, src, sizeof(lv_style_t));
}

/**
 * Create an animation from a pre-configured 'lv_style_anim_t' variable
 * @param anim pointer to a pre-configured 'lv_style_anim_t' variable (will be copied)
 */
void lv_style_anim_create(lv_style_anim_t * anim)
{
    lv_style_anim_dsc_t * dsc;
    dsc = dm_alloc(sizeof(lv_style_anim_dsc_t));
    dsc->style_anim = anim->style_anim;
    memcpy(&dsc->style_start, anim->style_start, sizeof(lv_style_t));
    memcpy(&dsc->style_end, anim->style_end, sizeof(lv_style_t));

    anim_t a;
    a.var = (void*)dsc;
    a.start = 0;
    a.end = LV_STYLE_ANIM_RES;
    a.fp = (anim_fp_t)lv_style_aimator;
    a.path = anim_get_path(ANIM_PATH_LIN);
    a.end_cb = anim->end_cb;
    a.act_time = anim->act_time;
    a.time = anim->time;
    a.playback = anim->playback;
    a.playback_pause = anim->playback_pause;
    a.repeat = anim->repeat;
    a.repeat_pause = anim->repeat_pause;

    anim_create(&a);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Used by the style animations to set the values of a style according to start and end style.
 * @param dsc the 'animated variable' set by lv_style_anim_create()
 * @param val the current state of the animation between 0 and LV_STYLE_ANIM_RES
 */
static void lv_style_aimator(lv_style_anim_dsc_t * dsc, int32_t val)
{
    const lv_style_t * start = &dsc->style_start;
    const lv_style_t * end = &dsc->style_end;
    lv_style_t * act = dsc->style_anim;

    STYLE_ATTR_ANIM(body.opa, val);
    STYLE_ATTR_ANIM(body.radius, val);
    STYLE_ATTR_ANIM(body.border.width, val);
    STYLE_ATTR_ANIM(body.shadow.width, val);
    STYLE_ATTR_ANIM(body.padding.hor, val);
    STYLE_ATTR_ANIM(body.padding.ver, val);
    STYLE_ATTR_ANIM(body.padding.inner, val);
    STYLE_ATTR_ANIM(text.line_space, val);
    STYLE_ATTR_ANIM(text.letter_space, val);
    STYLE_ATTR_ANIM(line.width, val);
    STYLE_ATTR_ANIM(image.intense, val);

    act->body.main_color = color_mix(end->body.main_color, start->body.main_color, val);
    act->body.gradient_color = color_mix(end->body.gradient_color, start->body.gradient_color, val);
    act->body.border.color = color_mix(end->body.border.color, start->body.border.color, val);
    act->body.shadow.color = color_mix(end->body.shadow.color, start->body.shadow.color, val);
    act->text.color = color_mix(end->text.color, start->text.color, val);
    act->image.color = color_mix(end->image.color, start->image.color, val);
    act->line.color = color_mix(end->line.color, start->line.color, val);


    if(val == 0) {
        act->body.empty = start->body.empty;
        act->glass = start->glass;
        act->text.font = start->text.font;
        act->body.shadow.type = start->body.shadow.type;
    }

    if(val == LV_STYLE_ANIM_RES) {
        act->body.empty = end->body.empty;
        act->glass = end->glass;
        act->text.font = end->text.font;
        act->body.shadow.type = end->body.shadow.type;
    }

    lv_style_refr_objs(dsc->style_anim);


    if(val == LV_STYLE_ANIM_RES) {
        dm_free(dsc);
    }
}
