/**
 * @file lv_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#include "lv_style.h"
#include "lv_obj.h"
#include "../lv_misc/lv_mem.h"

/*********************
 *      DEFINES
 *********************/
#if USE_LV_ANIMATION
#define LV_STYLE_ANIM_RES       256
#define LV_STYLE_ANIM_SHIFT     8      /*log2(LV_STYLE_ANIM_RES)*/

#define VAL_PROP(v1, v2, r)   v1 + (((v2-v1) * r) >> LV_STYLE_ANIM_SHIFT)
#define STYLE_ATTR_ANIM(attr, r)   if(start->attr != end->attr) act->attr = VAL_PROP(start->attr, end->attr, r)
#endif

/**********************
 *      TYPEDEFS
 **********************/
#if USE_LV_ANIMATION
typedef struct {
    lv_style_t style_start;   /*Save not only pointers because can be same as 'style_anim' then it will be modified too*/
    lv_style_t style_end;
    lv_style_t *style_anim;
    void (*end_cb)(void *);
}lv_style_anim_dsc_t;
#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if USE_LV_ANIMATION
static void style_animator(lv_style_anim_dsc_t * dsc, int32_t val);
static void style_animation_common_end_cb(void *ptr);
#endif

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
lv_style_t lv_style_btn_rel;
lv_style_t lv_style_btn_pr;
lv_style_t lv_style_btn_tgl_rel;
lv_style_t lv_style_btn_tgl_pr;
lv_style_t lv_style_btn_ina;

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
    lv_style_scr.body.opa = LV_OPA_COVER;
    lv_style_scr.body.main_color = LV_COLOR_WHITE;
    lv_style_scr.body.grad_color = LV_COLOR_WHITE;
    lv_style_scr.body.radius = 0;
    lv_style_scr.body.padding.ver = LV_DPI / 12;
    lv_style_scr.body.padding.hor = LV_DPI / 12;
    lv_style_scr.body.padding.inner = LV_DPI / 12;

    lv_style_scr.body.border.color = LV_COLOR_BLACK;
    lv_style_scr.body.border.opa = LV_OPA_COVER;
    lv_style_scr.body.border.width = 0;
    lv_style_scr.body.border.part = LV_BORDER_FULL;

    lv_style_scr.body.shadow.color = LV_COLOR_GRAY;
    lv_style_scr.body.shadow.type = LV_SHADOW_FULL;
    lv_style_scr.body.shadow.width = 0;

    lv_style_scr.text.opa = LV_OPA_COVER;
    lv_style_scr.text.color = LV_COLOR_MAKE(0x30, 0x30, 0x30);
    lv_style_scr.text.font = LV_FONT_DEFAULT;
    lv_style_scr.text.letter_space = 2;
    lv_style_scr.text.line_space = 2;

    lv_style_scr.image.opa = LV_OPA_COVER;
    lv_style_scr.image.color = LV_COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_scr.image.intense = LV_OPA_TRANSP;

    lv_style_scr.line.opa = LV_OPA_COVER;
    lv_style_scr.line.color = LV_COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_scr.line.width = 1;

    /*Plain style (by default near the same as the screen style)*/
    memcpy(&lv_style_plain, &lv_style_scr, sizeof(lv_style_t));

    /*Plain color style*/
    memcpy(&lv_style_plain_color, &lv_style_plain, sizeof(lv_style_t));
    lv_style_plain_color.text.color = LV_COLOR_MAKE(0xf0, 0xf0, 0xf0);
    lv_style_plain_color.image.color = LV_COLOR_MAKE(0xf0, 0xf0, 0xf0);
    lv_style_plain_color.line.color = LV_COLOR_MAKE(0xf0, 0xf0, 0xf0);
    lv_style_plain_color.body.main_color = LV_COLOR_MAKE(0x55, 0x96, 0xd8);
    lv_style_plain_color.body.grad_color = lv_style_plain_color.body.main_color;

    /*Pretty style */
    memcpy(&lv_style_pretty, &lv_style_plain, sizeof(lv_style_t));
    lv_style_pretty.text.color = LV_COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_pretty.image.color = LV_COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_pretty.line.color = LV_COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_pretty.body.main_color = LV_COLOR_WHITE;
    lv_style_pretty.body.grad_color = LV_COLOR_SILVER;
    lv_style_pretty.body.radius = LV_DPI / 15;
    lv_style_pretty.body.border.color = LV_COLOR_MAKE(0x40, 0x40, 0x40);
    lv_style_pretty.body.border.width = LV_DPI / 50 >= 1 ? LV_DPI / 50  : 1;
    lv_style_pretty.body.border.opa = LV_OPA_30;

    /*Pretty color style*/
    memcpy(&lv_style_pretty_color, &lv_style_pretty, sizeof(lv_style_t));
    lv_style_pretty_color.text.color = LV_COLOR_MAKE(0xe0, 0xe0, 0xe0);
    lv_style_pretty_color.image.color = LV_COLOR_MAKE(0xe0, 0xe0, 0xe0);
    lv_style_pretty_color.line.color = LV_COLOR_MAKE(0xc0, 0xc0, 0xc0);
    lv_style_pretty_color.body.main_color = LV_COLOR_MAKE(0x6b, 0x9a, 0xc7);
    lv_style_pretty_color.body.grad_color = LV_COLOR_MAKE(0x2b, 0x59, 0x8b);
    lv_style_pretty_color.body.border.color = LV_COLOR_MAKE(0x15, 0x2c, 0x42);

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
    memcpy(&lv_style_btn_rel, &lv_style_plain, sizeof(lv_style_t));
    lv_style_btn_rel.body.main_color = LV_COLOR_MAKE(0x76, 0xa2, 0xd0);
    lv_style_btn_rel.body.grad_color = LV_COLOR_MAKE(0x19, 0x3a, 0x5d);
    lv_style_btn_rel.body.radius = LV_DPI / 15;
    lv_style_btn_rel.body.padding.hor = LV_DPI / 4;
    lv_style_btn_rel.body.padding.ver = LV_DPI / 6;
    lv_style_btn_rel.body.padding.inner = LV_DPI / 10;
    lv_style_btn_rel.body.border.color = LV_COLOR_MAKE(0x0b, 0x19, 0x28);
    lv_style_btn_rel.body.border.width = LV_DPI / 50 >= 1 ? LV_DPI / 50  : 1;
    lv_style_btn_rel.body.border.opa = LV_OPA_70;
    lv_style_btn_rel.text.color = LV_COLOR_MAKE(0xff, 0xff, 0xff);
    lv_style_btn_rel.body.shadow.color = LV_COLOR_GRAY;
    lv_style_btn_rel.body.shadow.width = 0;

    /*Button pressed style*/
    memcpy(&lv_style_btn_pr, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_btn_pr.body.main_color = LV_COLOR_MAKE(0x33, 0x62, 0x94);
    lv_style_btn_pr.body.grad_color = LV_COLOR_MAKE(0x10, 0x26, 0x3c);
    lv_style_btn_pr.text.color = LV_COLOR_MAKE(0xa4, 0xb5, 0xc6);
    lv_style_btn_pr.image.color = LV_COLOR_MAKE(0xa4, 0xb5, 0xc6);
    lv_style_btn_pr.line.color = LV_COLOR_MAKE(0xa4, 0xb5, 0xc6);

    /*Button toggle released style*/
    memcpy(&lv_style_btn_tgl_rel, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_btn_tgl_rel.body.main_color = LV_COLOR_MAKE(0x0a, 0x11, 0x22);
    lv_style_btn_tgl_rel.body.grad_color = LV_COLOR_MAKE(0x37, 0x62, 0x90);
    lv_style_btn_tgl_rel.body.border.color = LV_COLOR_MAKE(0x01, 0x07, 0x0d);
    lv_style_btn_tgl_rel.text.color = LV_COLOR_MAKE(0xc8, 0xdd, 0xf4);
    lv_style_btn_tgl_rel.image.color = LV_COLOR_MAKE(0xc8, 0xdd, 0xf4);
    lv_style_btn_tgl_rel.line.color = LV_COLOR_MAKE(0xc8, 0xdd, 0xf4);

    /*Button toggle pressed style*/
    memcpy(&lv_style_btn_tgl_pr, &lv_style_btn_tgl_rel, sizeof(lv_style_t));
    lv_style_btn_tgl_pr.body.main_color = LV_COLOR_MAKE(0x02, 0x14, 0x27);
    lv_style_btn_tgl_pr.body.grad_color = LV_COLOR_MAKE(0x2b, 0x4c, 0x70);
    lv_style_btn_tgl_pr.text.color = LV_COLOR_MAKE(0xa4, 0xb5, 0xc6);
    lv_style_btn_tgl_pr.image.color = LV_COLOR_MAKE(0xa4, 0xb5, 0xc6);
    lv_style_btn_tgl_pr.line.color = LV_COLOR_MAKE(0xa4, 0xb5, 0xc6);

    /*Button inactive style*/
    memcpy(&lv_style_btn_ina, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_btn_ina.body.main_color = LV_COLOR_MAKE(0xd8, 0xd8, 0xd8);
    lv_style_btn_ina.body.grad_color = LV_COLOR_MAKE(0xd8, 0xd8, 0xd8);
    lv_style_btn_ina.body.border.color = LV_COLOR_MAKE(0x90, 0x90, 0x90);
    lv_style_btn_ina.text.color = LV_COLOR_MAKE(0x70, 0x70, 0x70);
    lv_style_btn_ina.image.color = LV_COLOR_MAKE(0x70, 0x70, 0x70);
    lv_style_btn_ina.line.color = LV_COLOR_MAKE(0x70, 0x70, 0x70);
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

#if USE_LV_ANIMATION
/**
 * Create an animation from a pre-configured 'lv_style_anim_t' variable
 * @param anim pointer to a pre-configured 'lv_style_anim_t' variable (will be copied)
 */
void lv_style_anim_create(lv_style_anim_t * anim)
{
    lv_style_anim_dsc_t * dsc;
    dsc = lv_mem_alloc(sizeof(lv_style_anim_dsc_t));
    dsc->style_anim = anim->style_anim;
    memcpy(&dsc->style_start, anim->style_start, sizeof(lv_style_t));
    memcpy(&dsc->style_end, anim->style_end, sizeof(lv_style_t));
    dsc->end_cb = anim->end_cb;


    lv_anim_t a;
    a.var = (void*)dsc;
    a.start = 0;
    a.end = LV_STYLE_ANIM_RES;
    a.fp = (lv_anim_fp_t)style_animator;
    a.path = lv_anim_path_linear;
    a.end_cb = style_animation_common_end_cb;
    a.act_time = anim->act_time;
    a.time = anim->time;
    a.playback = anim->playback;
    a.playback_pause = anim->playback_pause;
    a.repeat = anim->repeat;
    a.repeat_pause = anim->repeat_pause;

    lv_anim_create(&a);
}
#endif
/**********************
 *   STATIC FUNCTIONS
 **********************/
#if USE_LV_ANIMATION
/**
 * Used by the style animations to set the values of a style according to start and end style.
 * @param dsc the 'animated variable' set by lv_style_anim_create()
 * @param val the current state of the animation between 0 and LV_STYLE_ANIM_RES
 */
static void style_animator(lv_style_anim_dsc_t * dsc, int32_t val)
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

    lv_opa_t opa = val == LV_STYLE_ANIM_RES ? LV_OPA_COVER : val;

    act->body.main_color = lv_color_mix(end->body.main_color, start->body.main_color, opa);
    act->body.grad_color = lv_color_mix(end->body.grad_color, start->body.grad_color, opa);
    act->body.border.color = lv_color_mix(end->body.border.color, start->body.border.color, opa);
    act->body.shadow.color = lv_color_mix(end->body.shadow.color, start->body.shadow.color, opa);
    act->text.color = lv_color_mix(end->text.color, start->text.color, opa);
    act->image.color = lv_color_mix(end->image.color, start->image.color, opa);
    act->line.color = lv_color_mix(end->line.color, start->line.color, opa);

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

    lv_obj_report_style_mod(dsc->style_anim);
}

/**
 * Called when a style animation is ready
 * It called the user defined call back and free the allocated memories
 * @param ptr the 'animated variable' set by lv_style_anim_create()
 */
static void style_animation_common_end_cb(void *ptr)
{
    lv_style_anim_dsc_t *dsc = ptr;     /*To avoid casting*/

    if(dsc->end_cb) dsc->end_cb(dsc);

    lv_mem_free(dsc);
}

#endif
