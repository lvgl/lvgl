/**
 * @file lv_demo_ebike.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_ebike.h"
#if LV_USE_DEMO_EBIKE

#include "lv_demo_ebike_home.h"
#include "lv_demo_ebike_settings.h"
#include "lv_demo_ebike_stats.h"
#include "lv_demo_ebike_private.h"
#include "translations/lv_i18n.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void menu_bar_create(void);
static void menu_item_click_event_cb(lv_event_t * e);
static void language_observer_cb(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * main_cont;
#if LV_USE_LOTTIE
    #if LV_DEMO_EBIKE_PORTRAIT
        LV_DRAW_BUF_DEFINE_STATIC(lottie_ebike_draw_buf, 120, 120, LV_COLOR_FORMAT_ARGB8888);
    #else
        LV_DRAW_BUF_DEFINE_STATIC(lottie_ebike_draw_buf, 100, 100, LV_COLOR_FORMAT_ARGB8888);
    #endif
#endif

static lv_theme_t * theme_original;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*The language subject is globally available*/
lv_subject_t ebike_subject_language;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_ebike(void)
{
    lv_i18n_init(lv_i18n_language_pack);

#if LV_USE_LOTTIE
    LV_DRAW_BUF_INIT_STATIC(lottie_ebike_draw_buf);
#endif

    lv_subject_init_int(&ebike_subject_language, 0);
    lv_subject_add_observer_obj(&ebike_subject_language, language_observer_cb, lv_screen_active(), NULL);
    lv_demo_ebike_home_init();
    lv_demo_ebike_stats_init();

    /*Use the simple the to make styling simpler*/
    lv_display_t * display = lv_display_get_default();
    theme_original = lv_display_get_theme(display);
    lv_theme_t * theme = lv_theme_simple_init(display);
    lv_display_set_theme(display, theme);

    lv_obj_set_flex_flow(lv_screen_active(), LV_DEMO_EBIKE_PORTRAIT ? LV_FLEX_FLOW_COLUMN : LV_FLEX_FLOW_ROW);
    lv_obj_remove_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_white(), 0);

    lv_obj_t * bg = lv_image_create(lv_screen_active());
#if LV_DEMO_EBIKE_PORTRAIT
    LV_IMAGE_DECLARE(img_ebike_bg_large);
    lv_image_set_src(bg, &img_ebike_bg_large);
#else
    LV_IMAGE_DECLARE(img_ebike_bg);
    lv_image_set_src(bg, &img_ebike_bg);
#endif

    lv_obj_align(bg, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_flag(bg, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xffeeaa), 0);

    /*Create a container for the main content*/
    main_cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(main_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_grow(main_cont, 1);
    lv_obj_set_style_bg_opa(main_cont, 0, 0);

    lv_demo_ebike_home_create(main_cont);

    menu_bar_create();
}

#if LV_USE_LOTTIE
lv_draw_buf_t * lv_demo_ebike_get_lottie_draw_buf(void)
{
    return &lottie_ebike_draw_buf;
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void menu_delete_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_subject_deinit(&ebike_subject_language);
    lv_demo_ebike_home_deinit();
    lv_demo_ebike_stats_deinit();

    lv_display_set_theme(lv_display_get_default(), theme_original);
}

static void menu_bar_create(void)
{
    lv_obj_t * menu_cont = lv_obj_create(lv_screen_active());
    lv_obj_set_style_bg_color(menu_cont, lv_color_black(), 0);
    lv_obj_set_flex_flow(menu_cont, LV_DEMO_EBIKE_PORTRAIT ? LV_FLEX_FLOW_ROW : LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(menu_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
#if LV_DEMO_EBIKE_PORTRAIT
    lv_obj_set_size(menu_cont, lv_pct(100), 64);
    lv_obj_set_style_pad_gap(menu_cont, 32, 0);
#else
    lv_obj_set_size(menu_cont, 44, lv_pct(100));
    lv_obj_set_style_pad_gap(menu_cont, 16, 0);
#endif
    /*The menu always exists. If it's deleted, the demo was deleted, so clean up*/
    lv_obj_add_event_cb(menu_cont, menu_delete_event_cb, LV_EVENT_DELETE, NULL);


    LV_IMAGE_DECLARE(img_ebike_settings);
    lv_obj_t * icon1 = lv_image_create(menu_cont);
    lv_image_set_src(icon1, &img_ebike_settings);
    lv_obj_add_event_cb(icon1, menu_item_click_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_size(icon1, 44, 44);
    lv_obj_set_ext_click_area(icon1, 8);
    lv_obj_add_flag(icon1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_image_opa(icon1, LV_OPA_50, 0);

    LV_IMAGE_DECLARE(img_ebike_stats);
    lv_obj_t * icon2 = lv_image_create(menu_cont);
    lv_image_set_src(icon2, &img_ebike_stats);
    lv_obj_add_event_cb(icon2, menu_item_click_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_size(icon2, 44, 44);
    lv_obj_set_ext_click_area(icon2, 8);
    lv_obj_add_flag(icon2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_image_opa(icon2, LV_OPA_50, 0);

    LV_IMAGE_DECLARE(img_ebike_home);
    lv_obj_t * icon3 = lv_image_create(menu_cont);
    lv_image_set_src(icon3, &img_ebike_home);
    lv_obj_add_event_cb(icon3, menu_item_click_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_size(icon3, 44, 44);
    lv_obj_set_ext_click_area(icon3, 8);
    lv_obj_add_flag(icon3, LV_OBJ_FLAG_CLICKABLE);
}

static void menu_item_click_event_cb(lv_event_t * e)
{
    lv_obj_clean(main_cont);

    lv_obj_t  * icon = lv_event_get_target(e);
    lv_obj_t  * menu_cont = lv_obj_get_parent(icon);
    uint32_t idx = lv_obj_get_index(icon);

    lv_obj_set_style_image_opa(lv_obj_get_child(menu_cont, 0), LV_OPA_50, 0);
    lv_obj_set_style_image_opa(lv_obj_get_child(menu_cont, 1), LV_OPA_50, 0);
    lv_obj_set_style_image_opa(lv_obj_get_child(menu_cont, 2), LV_OPA_50, 0);

    lv_obj_set_style_image_opa(lv_obj_get_child(menu_cont, idx), LV_OPA_100, 0);
    switch(idx) {
        case 0:
            lv_demo_ebike_settings_create(main_cont);
            break;
        case 1:
            lv_demo_ebike_stats_create(main_cont);
            break;
        case 2:
            lv_demo_ebike_home_create(main_cont);
            break;
        default:
            break;
    }
}

static void language_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(observer);

    static uint32_t lang_current = 0;
    uint32_t lang_new = lv_subject_get_int(subject);
    if(lang_new != lang_current) {
        lang_current = lang_new;
        switch(lang_current) {
            case 0:
                lv_i18n_set_locale("en");
                break;
            case 1:
                lv_i18n_set_locale("zh");
                break;
            case 2:
                lv_i18n_set_locale("ar");
                break;
        }
        lv_obj_clean(main_cont);
        lv_demo_ebike_settings_create(main_cont);
    }
}

#endif /*LV_USE_DEMO_EBIKE*/
