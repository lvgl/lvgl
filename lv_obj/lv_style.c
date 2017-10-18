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
#define LV_STYLE_ANIM_RES       255    /*Animation max in 1024 steps*/
#define LV_STYLE_ANIM_SHIFT     8      /*log2(LV_STYLE_ANIM_RES)*/

#define VAL_PROP(v1, v2, r)   v1 + (((v2-v1) * r) >> LV_STYLE_ANIM_SHIFT)
#define STYLE_ATTR_ANIM(attr, r)   if(start->attr != end->attr) act->attr = VAL_PROP(start->attr, end->attr, r)

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    const lv_style_t * style_start;
    const lv_style_t * style_end;
    lv_style_t * style_anim;
}lv_style_anim_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_style_aimator(lv_style_anim_dsc_t * dsc, int32_t val);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_style_t lv_style_scr;
static lv_style_t lv_style_transp;
static lv_style_t lv_style_transp_tight;
static lv_style_t lv_style_plain;
static lv_style_t lv_style_plain_color;
static lv_style_t lv_style_pretty;
static lv_style_t lv_style_pretty_color;
static lv_style_t lv_style_btn_rel;
static lv_style_t lv_style_btn_pr;
static lv_style_t lv_style_btn_trel;
static lv_style_t lv_style_btn_tpr;
static lv_style_t lv_style_btn_ina;

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
    lv_style_scr.body.opa = OPA_COVER;
    lv_style_scr.body.color_main = COLOR_WHITE;
    lv_style_scr.body.color_grad = COLOR_WHITE;
    lv_style_scr.body.radius = 0;
    lv_style_scr.body.pad_ver = LV_DPI / 12;
    lv_style_scr.body.pad_hor = LV_DPI / 12;
    lv_style_scr.body.pad_obj = LV_DPI / 12;
    lv_style_scr.body.empty = 0;
    lv_style_scr.body.glass = 0;

    lv_style_scr.border.color = COLOR_BLACK;
    lv_style_scr.border.opa = OPA_COVER;
    lv_style_scr.border.width = 0;

    lv_style_scr.shadow.color = COLOR_GRAY;
    lv_style_scr.shadow.type = LV_STYPE_FULL;
    lv_style_scr.shadow.width = 0;

    lv_style_scr.txt.color = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_scr.txt.font = font_get(FONT_DEFAULT);
    lv_style_scr.txt.space_letter = 1 * LV_DOWNSCALE;
    lv_style_scr.txt.space_line = 2 * LV_DOWNSCALE;
    lv_style_scr.txt.align = LV_TXT_ALIGN_LEFT;

    lv_style_scr.img.color = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_scr.img.intense = OPA_TRANSP;

    lv_style_scr.line.color = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_scr.line.width = 1 * LV_DOWNSCALE;

    /*Plain style (by default near the same as the screen style)*/
    memcpy(&lv_style_plain, &lv_style_scr, sizeof(lv_style_t));

    /*Plain color style*/
    memcpy(&lv_style_plain_color, &lv_style_plain, sizeof(lv_style_t));
    lv_style_plain_color.txt.color = COLOR_MAKE(0xf0, 0xf0, 0xf0);
    lv_style_plain_color.img.color = COLOR_MAKE(0xf0, 0xf0, 0xf0);
    lv_style_plain_color.line.color = COLOR_MAKE(0xf0, 0xf0, 0xf0);
    lv_style_plain_color.body.color_main = COLOR_MAKE(0x55, 0x96, 0xd8);
    lv_style_plain_color.body.color_grad = lv_style_plain_color.body.color_main;

    /*Pretty style */
    memcpy(&lv_style_pretty, &lv_style_plain, sizeof(lv_style_t));
    lv_style_pretty.txt.color = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_pretty.img.color = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_pretty.line.color = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_pretty.body.color_main = COLOR_WHITE;
    lv_style_pretty.body.color_grad = COLOR_SILVER;
    lv_style_pretty.body.radius = LV_DPI / 15;
    lv_style_pretty.border.color = COLOR_MAKE(0x40, 0x40, 0x40);
    lv_style_pretty.border.width = LV_DPI / 50 >= 1 ? LV_DPI / 50  : 1;
    lv_style_pretty.border.opa = OPA_50;

    /*Pretty color style*/
    memcpy(&lv_style_pretty_color, &lv_style_pretty, sizeof(lv_style_t));
    lv_style_pretty_color.txt.color = COLOR_MAKE(0xe0, 0xe0, 0xe0);
    lv_style_pretty_color.img.color = COLOR_MAKE(0xe0, 0xe0, 0xe0);
    lv_style_pretty_color.line.color = COLOR_MAKE(0xe0, 0xe0, 0xe0);
    lv_style_pretty_color.body.color_main = COLOR_MAKE(0x6b, 0x9a, 0xc7);
    lv_style_pretty_color.body.color_grad = COLOR_MAKE(0x2b, 0x59, 0x8b);
    lv_style_pretty_color.border.color = COLOR_MAKE(0x15, 0x2c, 0x42);

    /*Transparent style*/
    memcpy(&lv_style_transp, &lv_style_plain, sizeof(lv_style_t));
    lv_style_transp.body.empty = 1;
    lv_style_transp.body.glass = 1;
    lv_style_transp.border.width = 0;

    /*Transparent tight style*/
    memcpy(&lv_style_transp_tight, &lv_style_transp, sizeof(lv_style_t));
    lv_style_transp_tight.body.pad_hor = 0;
    lv_style_transp_tight.body.pad_ver = 0;

    /*Button released style*/
    memcpy(&lv_style_btn_rel, &lv_style_plain, sizeof(lv_style_t));
    lv_style_btn_rel.body.color_main = COLOR_MAKE(0x76, 0xa2, 0xd0);
    lv_style_btn_rel.body.color_grad = COLOR_MAKE(0x19, 0x3a, 0x5d);
    lv_style_btn_rel.body.radius = LV_DPI / 15;
    lv_style_btn_rel.body.pad_hor = LV_DPI / 4;
    lv_style_btn_rel.body.pad_ver = LV_DPI / 6;
    lv_style_btn_rel.body.pad_obj = LV_DPI / 10;
    lv_style_btn_rel.border.color = COLOR_MAKE(0x0b, 0x19, 0x28);
    lv_style_btn_rel.border.width = LV_DPI / 50 >= 1 ? LV_DPI / 50  : 1;
    lv_style_btn_rel.border.opa = OPA_70;
    lv_style_btn_rel.txt.color = COLOR_MAKE(0xff, 0xff, 0xff);
    lv_style_btn_rel.txt.align = LV_TXT_ALIGN_MID;
    lv_style_btn_rel.shadow.color = COLOR_GRAY;
    lv_style_btn_rel.shadow.width = 0;

    /*Button pressed style*/
    memcpy(&lv_style_btn_pr, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_btn_pr.body.color_main = COLOR_MAKE(0x33, 0x62, 0x94);
    lv_style_btn_pr.body.color_grad = COLOR_MAKE(0x10, 0x26, 0x3c);
    lv_style_btn_pr.txt.color = COLOR_MAKE(0xa4, 0xb5, 0xc6);
    lv_style_btn_pr.img.color = COLOR_MAKE(0xa4, 0xb5, 0xc6);
    lv_style_btn_pr.line.color = COLOR_MAKE(0xa4, 0xb5, 0xc6);

    /*Button toggle released style*/
    memcpy(&lv_style_btn_trel, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_btn_trel.body.color_main = COLOR_MAKE(0x0a, 0x11, 0x22);
    lv_style_btn_trel.body.color_grad = COLOR_MAKE(0x37, 0x62, 0x90);
    lv_style_btn_trel.border.color = COLOR_MAKE(0x01, 0x07, 0x0d);
    lv_style_btn_trel.txt.color = COLOR_MAKE(0xc8, 0xdd, 0xf4);
    lv_style_btn_trel.img.color = COLOR_MAKE(0xc8, 0xdd, 0xf4);
    lv_style_btn_trel.line.color = COLOR_MAKE(0xc8, 0xdd, 0xf4);

    /*Button toggle pressed style*/
    memcpy(&lv_style_btn_tpr, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_btn_tpr.body.color_main = COLOR_MAKE(0x02, 0x14, 0x27);
    lv_style_btn_tpr.body.color_grad = COLOR_MAKE(0x2b, 0x4c, 0x70);
    lv_style_btn_tpr.txt.color = COLOR_MAKE(0xa4, 0xb5, 0xc6);
    lv_style_btn_tpr.img.color = COLOR_MAKE(0xa4, 0xb5, 0xc6);
    lv_style_btn_tpr.line.color = COLOR_MAKE(0xa4, 0xb5, 0xc6);

    /*Button inactive style*/
    memcpy(&lv_style_btn_ina, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_btn_ina.body.color_main = COLOR_MAKE(0xd8, 0xd8, 0xd8);
    lv_style_btn_ina.body.color_grad = COLOR_MAKE(0xd8, 0xd8, 0xd8);
    lv_style_btn_ina.border.color = COLOR_MAKE(0x90, 0x90, 0x90);
    lv_style_btn_ina.txt.color = COLOR_MAKE(0x70, 0x70, 0x70);
    lv_style_btn_ina.img.color = COLOR_MAKE(0x70, 0x70, 0x70);
    lv_style_btn_ina.line.color = COLOR_MAKE(0x70, 0x70, 0x70);
}


/**
 * Get style from its name
 * @param style_name an element of the 'lv_style_name_t' enum
 * @return pointer to the requested style (lv_style_def by default)
 */
lv_style_t * lv_style_get(lv_style_name_t style_name, lv_style_t * copy)
{
    lv_style_t * style = &lv_style_plain;

    switch(style_name) {
        case LV_STYLE_SCR:
            style = &lv_style_scr;
            break;
        case LV_STYLE_PLAIN:
            style = &lv_style_plain;
            break;
        case LV_STYLE_PLAIN_COLOR:
            style = &lv_style_plain_color;
            break;
        case LV_STYLE_PRETTY:
            style = &lv_style_pretty;
            break;
        case LV_STYLE_PRETTY_COLOR:
            style = &lv_style_pretty_color;
            break;
        case LV_STYLE_TRANSP:
            style = &lv_style_transp;
            break;
        case LV_STYLE_TRANSP_TIGHT:
            style = &lv_style_transp_tight;
            break;
        case LV_STYLE_BTN_REL:
            style = &lv_style_btn_rel;
            break;
        case LV_STYLE_BTN_PR:
            style = &lv_style_btn_pr;
            break;
        case LV_STYLE_BTN_TREL:
            style = &lv_style_btn_trel;
            break;
        case LV_STYLE_BTN_TPR:
            style = &lv_style_btn_tpr;
            break;
        case LV_STYLE_BTN_INA:
            style = &lv_style_btn_ina;
            break;
        default:
            style =  &lv_style_plain;
    }

    if(copy != NULL) memcpy(copy, style, sizeof(lv_style_t));

    return style;
}

/**
 * Copy a style to an other
 * @param dest pointer to the destination style
 * @param src pointer to the source style
 */
void lv_style_cpy(lv_style_t * dest, const lv_style_t * src)
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
    dsc->style_start = anim->style_start;
    dsc->style_end = anim->style_end;

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
    const lv_style_t * start = dsc->style_start;
    const lv_style_t * end = dsc->style_end;
    lv_style_t * act = dsc->style_anim;

    STYLE_ATTR_ANIM(body.opa, val);
    STYLE_ATTR_ANIM(body.radius, val);
    STYLE_ATTR_ANIM(border.width, val);
    STYLE_ATTR_ANIM(shadow.width, val);
    STYLE_ATTR_ANIM(body.pad_hor, val);
    STYLE_ATTR_ANIM(body.pad_ver, val);
    STYLE_ATTR_ANIM(body.pad_obj, val);
    STYLE_ATTR_ANIM(txt.space_line, val);
    STYLE_ATTR_ANIM(txt.space_letter, val);
    STYLE_ATTR_ANIM(line.width, val);
    STYLE_ATTR_ANIM(img.intense, val);

    act->body.color_main = color_mix(end->body.color_main, start->body.color_main, val);
    act->body.color_grad = color_mix(end->body.color_grad, start->body.color_grad, val);
    act->border.color = color_mix(end->border.color, start->border.color, val);
    act->shadow.color = color_mix(end->shadow.color, start->shadow.color, val);
    act->txt.color = color_mix(end->txt.color, start->txt.color, val);
    act->img.color = color_mix(end->img.color, start->img.color, val);
    act->line.color = color_mix(end->line.color, start->line.color, val);


    if(val == 0) {
        act->body.empty = start->body.empty;
        act->body.glass = start->body.glass;
        act->txt.font = start->txt.font;
        act->shadow.type = start->shadow.type;
        act->txt.align = start->txt.align;
    }

    if(val == LV_STYLE_ANIM_RES) {
        act->body.empty = end->body.empty;
        act->body.glass = end->body.glass;
        act->txt.font = end->txt.font;
        act->shadow.type = end->shadow.type;
        act->txt.align = end->txt.align;
    }

    lv_style_refr_objs(dsc->style_anim);
}
