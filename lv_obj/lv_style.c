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
//static lv_style_t lv_style_focus;
//static lv_style_t lv_style_focus_color;
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
    /*Screen style*/
    lv_style_set_ccolor(&lv_style_scr, COLOR_MAKE(0x20, 0x20, 0x20));
    lv_style_set_opa(&lv_style_scr, OPA_COVER);
    lv_style_set_opa_prop(&lv_style_scr, true);

    lv_style_set_mcolor(&lv_style_scr, COLOR_WHITE);
    lv_style_set_gcolor(&lv_style_scr, COLOR_WHITE);
    lv_style_set_bcolor(&lv_style_scr, COLOR_WHITE);
    lv_style_set_scolor(&lv_style_scr, COLOR_GRAY);
    lv_style_set_radius(&lv_style_scr, 0);
    lv_style_set_bwidth(&lv_style_scr, 0);
    lv_style_set_swidth(&lv_style_scr, 0);
    lv_style_set_vpad(&lv_style_scr, LV_DPI / 6);
    lv_style_set_hpad(&lv_style_scr, LV_DPI / 4);
    lv_style_set_opad(&lv_style_scr, LV_DPI / 6);
    lv_style_set_bopa(&lv_style_scr, OPA_COVER);
    lv_style_set_empty(&lv_style_scr, false);

    lv_style_set_font(&lv_style_scr, font_get(LV_FONT_DEFAULT));
    lv_style_set_letter_space(&lv_style_scr, 1 * LV_DOWNSCALE);
    lv_style_set_line_space(&lv_style_scr, 5 * LV_DOWNSCALE);
    lv_style_set_txt_align(&lv_style_scr, 0);

    lv_style_set_img_recolor(&lv_style_scr, OPA_TRANSP);

    lv_style_set_line_width(&lv_style_scr, 1 * LV_DOWNSCALE);

    /*Plain style (by default the same as the screen style)*/
    memcpy(&lv_style_plain, &lv_style_scr, sizeof(lv_style_t));

    /*Plain color style*/
    memcpy(&lv_style_plain_color, &lv_style_plain, sizeof(lv_style_t));
    lv_style_set_ccolor(&lv_style_plain_color, COLOR_RED);//MAKE(0xf0, 0xf0, 0xf0));
    lv_style_set_mcolor(&lv_style_plain_color, COLOR_MAKE(0x40, 0x60, 0x80));
    lv_style_set_gcolor(&lv_style_plain_color, COLOR_MAKE(0x40, 0x60, 0x80));

    /*Pretty style */
    memcpy(&lv_style_pretty, &lv_style_plain, sizeof(lv_style_t));
    lv_style_set_mcolor(&lv_style_pretty, COLOR_WHITE);
    lv_style_set_gcolor(&lv_style_pretty, COLOR_SILVER);
    lv_style_set_bcolor(&lv_style_pretty, COLOR_GRAY);
    lv_style_set_radius(&lv_style_pretty, LV_DPI / 10);
    lv_style_set_bwidth(&lv_style_pretty, LV_DPI / 20 >= 1 ? LV_DPI / 30 >= 1 : 1);
//    lv_style_set_swidth(&lv_style_pretty, LV_DPI / 6);
//    lv_style_set_scolor(&lv_style_pretty, COLOR_BLACK);

    /*Pretty color style*/
    memcpy(&lv_style_pretty_color, &lv_style_pretty, sizeof(lv_style_t));
    lv_style_set_ccolor(&lv_style_pretty_color, COLOR_RED);//MAKE(0xf0, 0xf0, 0xf0));
    lv_style_set_mcolor(&lv_style_pretty_color, COLOR_WHITE);
    lv_style_set_gcolor(&lv_style_pretty_color, COLOR_CYAN);
    lv_style_set_scolor(&lv_style_pretty_color, COLOR_BLACK);
    lv_style_set_swidth(&lv_style_pretty_color, LV_DPI / 2);

    /*Transparent style*/
    memcpy(&lv_style_transp, &lv_style_plain, sizeof(lv_style_t));
    lv_style_set_empty(&lv_style_transp, true);
    lv_style_set_bwidth(&lv_style_transp, 0);

    /*Transparent tight style*/
    memcpy(&lv_style_transp_tight, &lv_style_transp, sizeof(lv_style_t));
    lv_style_set_hpad(&lv_style_transp_tight, 0);
    lv_style_set_vpad(&lv_style_transp_tight, 0);

    /*Button released style*/
    memcpy(&lv_style_btn_rel, &lv_style_plain, sizeof(lv_style_t));
    lv_style_set_mcolor(&lv_style_btn_rel, COLOR_WHITE);
    lv_style_set_gcolor(&lv_style_btn_rel, COLOR_GRAY);

    /*Button pressed style*/
    memcpy(&lv_style_btn_pr, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_set_mcolor(&lv_style_btn_pr, COLOR_BLACK);
    lv_style_set_ccolor(&lv_style_btn_pr, COLOR_SILVER);
    lv_style_set_scolor(&lv_style_btn_pr, COLOR_GRAY);
    lv_style_set_swidth(&lv_style_btn_pr, 10);

    /*Button toggle released style*/
    memcpy(&lv_style_btn_trel, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_set_mcolor(&lv_style_btn_trel, COLOR_LIME);

    /*Button toggle pressed style*/
    memcpy(&lv_style_btn_tpr, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_set_mcolor(&lv_style_btn_tpr, COLOR_GREEN);

    /*Button inactive style*/
    memcpy(&lv_style_btn_ina, &lv_style_btn_rel, sizeof(lv_style_t));
    lv_style_set_mcolor(&lv_style_btn_ina, COLOR_YELLOW);
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
 * Set the content color of a style
 * @param style pointer to style
 * @param ccolor content color
 */
void lv_style_set_ccolor(lv_style_t * style, color_t ccolor)
{
    style->ccolor = ccolor;
}

/**
 * Set the opacity of a style
 * @param style pointer to style
 * @param opa opacity (OPA_COVER, OPA_TRANSP, OPA_10, OPA_20 ... OPA_90)
 */
void lv_style_set_opa(lv_style_t * style, opa_t opa)
{
    style->opa = opa;
}

/**
 * Set the proportional opacity attribute of a style (make the opacity relative to the parent)
 * @param style pointer to style
 * @param opa_prop true: enabled, false: disabled
 */
void lv_style_set_opa_prop(lv_style_t * style, bool opa_prop)
{
    style->opa_prop = opa_prop == false ? 0 : 1;
}

/**
 * Set the container main color of a style
 * @param style pointer to style
 * @param mcolor main color of the background
 */
void lv_style_set_mcolor(lv_style_t * style, color_t mcolor)
{
    style->mcolor = mcolor;
}


/**
 * Set the container gradient color of a style
 * @param style pointer to style
 * @param gcolor gradient color of the background
 */
void lv_style_set_gcolor(lv_style_t * style, color_t gcolor)
{
    style->gcolor = gcolor;
}

/**
 * Set the container border color of a style
 * @param style pointer to style
 * @param bcolor border color of the background
 */
void lv_style_set_bcolor(lv_style_t * style, color_t bcolor)
{
    style->bcolor = bcolor;
}


/**
 * Set the container shadow color of a style
 * @param style pointer to style
 * @param scolor shadow color of the background
 */
void lv_style_set_scolor(lv_style_t * style, color_t scolor)
{
    style->scolor = scolor;
}

/**
 * Set the container corner radius of a style
 * @param style pointer to style
 * @param radius corner radius of the background (>= 0)
 */
void lv_style_set_radius(lv_style_t * style, cord_t radius)
{
    style->radius = radius;
}


/**
 * Set the container border width of a style
 * @param style pointer to style
 * @param bwidth border width of the background (>= 0, 0 means no border)
 */
void lv_style_set_bwidth(lv_style_t * style, cord_t bwidth)
{
    style->bwidth = bwidth;
}


/**
 * Set the container shadow width of a style
 * @param style pointer to style
 * @param swidth shadow width of the background (>= 0, 0 means no shadow)
 */
void lv_style_set_swidth(lv_style_t * style, cord_t swidth)
{
    style->swidth = swidth;
}


/**
 * Set the container vertical padding of a style
 * @param style pointer to style
 * @param vpad vertical padding on the background
 */
void lv_style_set_vpad(lv_style_t * style, cord_t vpad)
{
    style->vpad = vpad;
}


/**
 * Set the container horizontal padding of a style
 * @param style pointer to style
 * @param hpad horizontal padding on the background
 */
void lv_style_set_hpad(lv_style_t * style, cord_t hpad)
{
    style->hpad = hpad;
}

/**
 * Set the container object padding of a style
 * @param style pointer to style
 * @param opad padding between objects on the background
 */
void lv_style_set_opad(lv_style_t * style, cord_t opad)
{
    style->opad = opad;
}

/**
 * Set the container border opacity of a style (relative to the object opacity)
 * @param style pointer to style
 * @param bopa border opacity of the background  (OPA_COVER, OPA_TRANSP, OPA_10, OPA_20 ... OPA_90)
 */
void lv_style_set_bopa(lv_style_t * style, opa_t bopa)
{
    style->bopa = bopa;
}


/**
 * Set container empty attribute of a style (transparent background but border drawn)
 * @param style pointer to style
 * @param empty true: empty enable, false: empty disable
 */
void lv_style_set_empty(lv_style_t * style, bool empty)
{
    style->empty = empty == false ? 0 : 1;
}

/**
 * Set the font of a style
 * @param style pointer to style
 * @param font pointer to a fint
 */
void lv_style_set_font(lv_style_t * style, const font_t * font)
{
    style->font = font;
}


/**
 * Set the letter space of a style
 * @param style pointer to style
 * @param letter_space new letter space
 */
void lv_style_set_letter_space(lv_style_t * style, cord_t letter_space)
{
    style->letter_space = letter_space;
}


/**
 * Set the line space of a style
 * @param style pointer to style
 * @param line_space new letter space
 */
void lv_style_set_line_space(lv_style_t * style, cord_t line_space)
{
    style->line_space = line_space;
}

/**
 * Set the text align of a style
 * @param style pointer to style
 * @param align TODO
 */
void lv_style_set_txt_align(lv_style_t * style, uint8_t align)
{
    style->txt_align = align;
}


/**
 * Set the image re-color intensity of a style
 * @param style pointer to style
 * @param recolor re-coloring intensity (OPA_TRANSP: do nothing, OPA_COVER: fully re-color, OPA_10: little re-color)
 */
void lv_style_set_img_recolor(lv_style_t * style, opa_t recolor)
{
    style->img_recolor = recolor;
}


/**
 * Set the line width of a style
 * @param style pointer to style
 * @param width new line width (>=0)
 */
void lv_style_set_line_width(lv_style_t * style, cord_t width)
{
    style->line_width = width;
}

/*************************
 *   GET FUNTIONS
 *************************/
/**
 * Get the content color of a style
 * @param style pointer to style
 * @return content color
 */
color_t lv_style_get_ccolor(lv_style_t * style)
{
    return style->ccolor;
}

/**
 * Get the opacity of a style
 * @param style pointer to style
 * @return opacity (OPA_COVER, OPA_TRANSP, OPA_10, OPA_20 ... OPA_90)
 */
opa_t lv_style_get_opa(lv_style_t * style)
{
    return style->opa;
}

/**
 * Get the proportional opacity attribute of a style (make the opacity relative to the parent)
 * @param style pointer to style
 * @return true: enabled, false: disabled
 */
bool lv_style_get_opa_prop(lv_style_t * style)
{
    return style->opa_prop == 0 ? false : true;
}

/**
 * Get the container main color of a style
 * @param style pointer to style
 * @return main color of the background
 */
color_t lv_style_get_mcolor(lv_style_t * style)
{
    return style->mcolor;
}


/**
 * Get the container gradient color of a style
 * @param style pointer to style
 * @return gradient color of the background
 */
color_t lv_style_get_gcolor(lv_style_t * style)
{
    return style->gcolor;
}

/**
 * Get the container border color of a style
 * @param style pointer to style
 * @return border color of the background
 */
color_t lv_style_get_bcolor(lv_style_t * style)
{
    return style->bcolor;
}


/**
 * Get the container shadow color of a style
 * @param style pointer to style
 * @return shadow color of the background
 */
color_t lv_style_get_Scolor(lv_style_t * style)
{
    return style->scolor;
}

/**
 * Get the container corner radius of a style
 * @param style pointer to style
 * @return corner radius of the background (>= 0)
 */
   cord_t lv_style_get_radius(lv_style_t * style)
{
    return style->radius;
}


/**
 * Get the container border width of a style
 * @param style pointer to style
 * @return border width of the background (>= 0, 0 means no border)
 */
cord_t lv_style_get_bwidth(lv_style_t * style)
{
    return style->bwidth;
}


/**
 * Get the container shadow width of a style
 * @param style pointer to style
 * @return shadow width of the background (>= 0, 0 means no shadow)
 */
cord_t lv_style_get_swidth(lv_style_t * style)
{
    return style->swidth;
}


/**
 * Get the container vertical padding of a style
 * @param style pointer to style
 * @return vertical padding on the background
 */
cord_t lv_style_get_vpad(lv_style_t * style)
{
    return style->vpad;
}


/**
 * Get the container horizontal padding of a style
 * @param style pointer to style
 * @return horizontal padding on the background
 */
cord_t lv_style_get_hpad(lv_style_t * style)
{
    return style->hpad;
}

/**
 * Get the container object padding of a style
 * @param style pointer to style
 * @return padding between objects on the background
 */
cord_t lv_style_get_opad(lv_style_t * style)
{
    return style->opad;
}

/**
 * Get the container border opacity of a style (relative to the object opacity)
 * @param style pointer to style
 * @return border opacity of the background  (OPA_COVER, OPA_TRANSP, OPA_10, OPA_20 ... OPA_90)
 */
opa_t lv_style_get_bopa(lv_style_t * style)
{
    return style->bopa;
}


/**
 * Get container empty attribute of a style (transparent background but border drawn)
 * @param style pointer to style
 * @return true: empty enable, false: empty disable
 */
bool lv_style_get_empty(lv_style_t * style, bool empty)
{
    return style->empty == false ? 0 : 1;
}

/**
 * Get the font of a style
 * @param style pointer to style
 * @return pointer to a fint
 */
const font_t * lv_style_get_font(lv_style_t * style)
{
    return style->font;
}


/**
 * Get the letter space of a style
 * @param style pointer to style
 * @return new letter space
 */
cord_t lv_style_get_letter_space(lv_style_t * style)
{
    return style->letter_space;
}


/**
 * Get the line space of a style
 * @param style pointer to style
 * @return new letter space
 */
cord_t lv_style_get_line_space(lv_style_t * style)
{
    return style->line_space;
}

/**
 * Get the text align of a style
 * @param style pointer to style
 * @return TODO
 */
uint8_t lv_style_get_txt_align(lv_style_t * style)
{
    return style->txt_align;
}


/**
 * Get the image re-color intensity of a style
 * @param style pointer to style
 * @return re-coloring intensity (OPA_TRANSP: do nothing, OPA_COVER: fully re-color, OPA_10: little re-color)
 */
opa_t lv_style_get_img_recolor(lv_style_t * style)
{
    return style->img_recolor;
}


/**
 * Get the line width of a style
 * @param style pointer to style
 * @return new line width (>=0)
 */
cord_t lv_style_get_line_width(lv_style_t * style)
{
    return style->line_width;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
