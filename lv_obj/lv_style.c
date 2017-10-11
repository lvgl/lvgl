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
    lv_style_scr.ccolor = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_scr.opa = OPA_COVER;

    lv_style_scr.mcolor = COLOR_WHITE;
    lv_style_scr.gcolor = COLOR_WHITE;
    lv_style_scr.bcolor = COLOR_BLACK;
    lv_style_scr.scolor = COLOR_GRAY;
    lv_style_scr.radius = 0;
    lv_style_scr.bwidth = 0;
    lv_style_scr.swidth = 0;
    lv_style_scr.stype = LV_STYPE_FULL;
    lv_style_scr.vpad = LV_DPI / 12;
    lv_style_scr.hpad = LV_DPI / 12;
    lv_style_scr.opad = LV_DPI / 12;
    lv_style_scr.bopa = OPA_COVER;
    lv_style_scr.empty = 0;
    lv_style_scr.glass = 0;

    lv_style_scr.font = font_get(FONT_DEFAULT);
    lv_style_scr.letter_space = 1 * LV_DOWNSCALE;
    lv_style_scr.line_space = 2 * LV_DOWNSCALE;
    lv_style_scr.txt_align = LV_TXT_ALIGN_LEFT;
    lv_style_scr.img_recolor = OPA_TRANSP;
    lv_style_scr.line_width = 1 * LV_DOWNSCALE;

    /*Plain style (by default near the same as the screen style)*/
    memcpy(&lv_style_plain, &lv_style_scr, sizeof(lv_style_t));
    lv_style_plain.mcolor = COLOR_WHITE;
    lv_style_plain.gcolor = COLOR_WHITE;
    lv_style_plain.bcolor = COLOR_WHITE;

    /*Plain color style*/
    memcpy(&lv_style_plain_color, &lv_style_plain, sizeof(lv_style_t));
    lv_style_plain_color.ccolor = COLOR_MAKE(0xf0, 0xf0, 0xf0);
    lv_style_plain_color.mcolor = COLOR_MAKE(0x55, 0x96, 0xd8);
    lv_style_plain_color.gcolor = lv_style_plain_color.mcolor;

    /*Pretty style */
    memcpy(&lv_style_pretty, &lv_style_plain, sizeof(lv_style_t));
    lv_style_pretty.ccolor = COLOR_MAKE(0x20, 0x20, 0x20);
    lv_style_pretty.mcolor = COLOR_WHITE;
    lv_style_pretty.gcolor = COLOR_SILVER;
    lv_style_pretty.bcolor = COLOR_MAKE(0x40, 0x40, 0x40);
    lv_style_pretty.radius = LV_DPI / 15;
    lv_style_pretty.bwidth = LV_DPI / 50 >= 1 ? LV_DPI / 50  : 1;
    lv_style_pretty.bopa = OPA_50;

    /*Pretty color style*/
    memcpy(&lv_style_pretty_color, &lv_style_pretty, sizeof(lv_style_t));
    lv_style_pretty_color.ccolor = COLOR_MAKE(0xe0, 0xe0, 0xe0);
    lv_style_pretty_color.mcolor = COLOR_MAKE(0x6b, 0x9a, 0xc7);
    lv_style_pretty_color.gcolor = COLOR_MAKE(0x2b, 0x59, 0x8b);
    lv_style_pretty_color.bcolor = COLOR_MAKE(0x15, 0x2c, 0x42);

    /*Transparent style*/
    memcpy(&lv_style_transp, &lv_style_plain, sizeof(lv_style_t));
    lv_style_transp.empty = 1;
    lv_style_transp.bwidth = 0;
    lv_style_transp.glass = 1;

    /*Transparent tight style*/
    memcpy(&lv_style_transp_tight, &lv_style_transp, sizeof(lv_style_t));
    lv_style_transp_tight.hpad = 0;
    lv_style_transp_tight.vpad = 0;

    /*Button released style*/
    memcpy(&lv_style_btn_rel, &lv_style_plain, sizeof(lv_style_t));
    lv_style_btn_rel.mcolor = COLOR_MAKE(0x76, 0xa2, 0xd0);
    lv_style_btn_rel.gcolor = COLOR_MAKE(0x19, 0x3a, 0x5d);
    lv_style_btn_rel.bcolor = COLOR_MAKE(0x0b, 0x19, 0x28);
    lv_style_btn_rel.ccolor = COLOR_MAKE(0xff, 0xff, 0xff);
    lv_style_btn_rel.bwidth = LV_DPI / 50 >= 1 ? LV_DPI / 50  : 1;
    lv_style_btn_rel.radius = LV_DPI / 15;
    lv_style_btn_rel.bopa = OPA_70;
    lv_style_btn_rel.scolor = COLOR_GRAY;
    lv_style_btn_rel.swidth = 0;
    lv_style_btn_rel.hpad = LV_DPI / 4;
    lv_style_btn_rel.vpad = LV_DPI / 6;
    lv_style_btn_rel.opad = LV_DPI / 10;
    lv_style_btn_rel.txt_align = LV_TXT_ALIGN_MID;

    /*Button pressed style*/
    memcpy(&lv_style_btn_pr, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_btn_pr.mcolor = COLOR_MAKE(0x33, 0x62, 0x94);
    lv_style_btn_pr.gcolor = COLOR_MAKE(0x10, 0x26, 0x3c);
    lv_style_btn_pr.ccolor = COLOR_MAKE(0xa4, 0xb5, 0xc6);

    /*Button toggle released style*/
    memcpy(&lv_style_btn_trel, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_btn_trel.mcolor = COLOR_MAKE(0x0a, 0x11, 0x22);
    lv_style_btn_trel.gcolor = COLOR_MAKE(0x37, 0x62, 0x90);
    lv_style_btn_trel.bcolor = COLOR_MAKE(0x01, 0x07, 0x0d);
    lv_style_btn_trel.ccolor = COLOR_MAKE(0xc8, 0xdd, 0xf4);

    /*Button toggle pressed style*/
    memcpy(&lv_style_btn_tpr, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_btn_tpr.mcolor = COLOR_MAKE(0x02, 0x14, 0x27);
    lv_style_btn_tpr.gcolor = COLOR_MAKE(0x2b, 0x4c, 0x70);
    lv_style_btn_tpr.ccolor = COLOR_MAKE(0xa4, 0xb5, 0xc6);

    /*Button inactive style*/
    memcpy(&lv_style_btn_ina, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_btn_ina.mcolor = COLOR_MAKE(0xd8, 0xd8, 0xd8);
    lv_style_btn_ina.gcolor = COLOR_MAKE(0xd8, 0xd8, 0xd8);
    lv_style_btn_ina.bcolor = COLOR_MAKE(0x90, 0x90, 0x90);
    lv_style_btn_ina.ccolor = COLOR_MAKE(0x70, 0x70, 0x70);
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

    STYLE_ATTR_ANIM(opa, val);
    STYLE_ATTR_ANIM(radius, val);
    STYLE_ATTR_ANIM(bwidth, val);
    STYLE_ATTR_ANIM(swidth, val);
    STYLE_ATTR_ANIM(hpad, val);
    STYLE_ATTR_ANIM(vpad, val);
    STYLE_ATTR_ANIM(opad, val);
    STYLE_ATTR_ANIM(line_space, val);
    STYLE_ATTR_ANIM(letter_space, val);
    STYLE_ATTR_ANIM(line_width, val);
    STYLE_ATTR_ANIM(img_recolor, val);

    act->mcolor = color_mix(end->mcolor, start->mcolor, val);
    act->gcolor = color_mix(end->gcolor, start->gcolor, val);
    act->bcolor = color_mix(end->bcolor, start->bcolor, val);
    act->scolor = color_mix(end->scolor, start->scolor, val);
    act->ccolor = color_mix(end->ccolor, start->ccolor, val);


    if(val == 0) {
        act->empty = start->empty;
        act->glass = start->glass;
        act->font = start->font;
        act->stype = start->stype;
        act->txt_align = start->txt_align;
    }

    if(val == LV_STYLE_ANIM_RES) {
        act->empty = end->empty;
        act->glass = end->glass;
        act->font = end->font;
        act->stype = end->stype;
        act->txt_align = end->txt_align;
    }

    lv_style_refr_objs(dsc->style_anim);
}
