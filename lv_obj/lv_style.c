/**
 * @file lv_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

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

    lv_style_scr.mcolor = COLOR_MAKE(0xc9, 0xdb, 0xee);
    lv_style_scr.gcolor = COLOR_MAKE(0x4d, 0x91, 0xd5);
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

/**********************
 *   STATIC FUNCTIONS
 **********************/
