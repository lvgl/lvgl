/**
 * @file lv_demo_widgets_components.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_widgets_components.h"
#if LV_USE_DEMO_WIDGETS

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

/**********************
 *  GLOBAL VARIABLES
 **********************/
disp_size_t disp_size;
lv_style_t style_title;
lv_style_t style_text_muted;
lv_style_t style_icon;
lv_style_t style_bullet;

const lv_font_t * font_large;
const lv_font_t * font_normal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_widgets_components_init(void)
{
#if LV_USE_DEMO_BENCHMARK && LV_DEMO_BENCHMARK_ALIGNED_FONTS
    LV_FONT_DECLARE(lv_font_benchmark_montserrat_12_aligned)
    LV_FONT_DECLARE(lv_font_benchmark_montserrat_14_aligned)
    LV_FONT_DECLARE(lv_font_benchmark_montserrat_16_aligned)
    LV_FONT_DECLARE(lv_font_benchmark_montserrat_18_aligned)
    LV_FONT_DECLARE(lv_font_benchmark_montserrat_20_aligned)
    LV_FONT_DECLARE(lv_font_benchmark_montserrat_24_aligned)
#define USE_ALIGNED_FONTS
#endif

    if(LV_HOR_RES <= 320) disp_size = DISP_SMALL;
    else if(LV_HOR_RES < 720) disp_size = DISP_MEDIUM;
    else disp_size = DISP_LARGE;

    font_large = LV_FONT_DEFAULT;
    font_normal = LV_FONT_DEFAULT;

    if(disp_size == DISP_LARGE) {
#ifdef USE_ALIGNED_FONTS
        font_large     = &lv_font_benchmark_montserrat_24_aligned;
#elif LV_FONT_MONTSERRAT_24
        font_large     = &lv_font_montserrat_24;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_24 or LV_DEMO_BENCHMARK_ALIGNED_FONTS is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#ifdef USE_ALIGNED_FONTS
        font_normal    = &lv_font_benchmark_montserrat_16_aligned;
#elif LV_FONT_MONTSERRAT_16
        font_normal    = &lv_font_montserrat_16;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_16 or LV_DEMO_BENCHMARK_ALIGNED_FONTS is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }
    else if(disp_size == DISP_MEDIUM) {
#ifdef USE_ALIGNED_FONTS
        font_large     = &lv_font_benchmark_montserrat_20_aligned;
#elif LV_FONT_MONTSERRAT_20
        font_large     = &lv_font_montserrat_20;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_20 or LV_DEMO_BENCHMARK_ALIGNED_FONTS is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#ifdef USE_ALIGNED_FONTS
        font_normal    = &lv_font_benchmark_montserrat_14_aligned;
#elif LV_FONT_MONTSERRAT_14
        font_normal    = &lv_font_montserrat_14;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_14 or LV_DEMO_BENCHMARK_ALIGNED_FONTS is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }
    else {   /* disp_size == DISP_SMALL */
#ifdef USE_ALIGNED_FONTS
        font_large     = &lv_font_benchmark_montserrat_18_aligned;
#elif LV_FONT_MONTSERRAT_18
        font_large     = &lv_font_montserrat_18;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_18 or LV_DEMO_BENCHMARK_ALIGNED_FONTS is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
#ifdef USE_ALIGNED_FONTS
        font_normal    = &lv_font_benchmark_montserrat_12_aligned;
#elif LV_FONT_MONTSERRAT_12
        font_normal    = &lv_font_montserrat_12;
#else
        LV_LOG_WARN("LV_FONT_MONTSERRAT_12 or LV_DEMO_BENCHMARK_ALIGNED_FONTS is not enabled for the widgets demo. Using LV_FONT_DEFAULT instead.");
#endif
    }
#if LV_USE_THEME_DEFAULT
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,
                          font_normal);
#endif


    lv_obj_set_style_text_font(lv_screen_active(), font_normal, 0);


    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);

    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_50);


    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
    lv_style_set_text_font(&style_icon, font_large);

    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

}

lv_obj_t * lv_demo_widgets_title_create(lv_obj_t * parent, const char * text)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_add_style(label, &style_title, 0);
    lv_label_set_text_static(label, text);

    return label;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_DEMO_WIDGETS */
