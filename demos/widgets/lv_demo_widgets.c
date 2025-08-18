/**
 * @file lv_demo_widgets.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_widgets.h"

#if LV_USE_DEMO_WIDGETS
#include "lv_demo_widgets_components.h"
#include "lv_demo_widgets_profile.h"
#include "lv_demo_widgets_analytics.h"
#include "lv_demo_widgets_shop.h"

#include "../../lvgl_private.h"

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_MEM_SIZE < (38ul * 1024ul)
    #error Insufficient memory for lv_demo_widgets. Please set LV_MEM_SIZE to at least 38KB (38ul * 1024ul).  48KB is recommended.
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void color_changer_create(lv_obj_t * parent);
static void color_changer_event_cb(lv_event_t * e);
static void color_event_cb(lv_event_t * e);
static void scroll_anim_y_cb(void * var, int32_t v);
static void scroll_anim_y_cb(void * var, int32_t v);
static void slideshow_anim_completed_cb(lv_anim_t * a_old);
static void tabview_delete_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *  GLOBAL VARIABLES
 **********************/
lv_obj_t * tv;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_widgets(void)
{
    lv_demo_widgets_components_init();

    tv = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(tv, disp_size == DISP_LARGE ? 75 : 45);
    lv_obj_add_event_cb(tv, tabview_delete_event_cb, LV_EVENT_DELETE, NULL);


    lv_obj_t * t1 = lv_tabview_add_tab(tv, "Profile");
    lv_obj_t * t2 = lv_tabview_add_tab(tv, "Analytics");
    lv_obj_t * t3 = lv_tabview_add_tab(tv, "Shop");

    if(disp_size == DISP_LARGE) {
        lv_obj_t * tab_bar = lv_tabview_get_tab_bar(tv);
        lv_obj_set_style_pad_left(tab_bar, LV_HOR_RES / 2, 0);
        lv_obj_t * logo = lv_image_create(tab_bar);
        lv_obj_add_flag(logo, LV_OBJ_FLAG_IGNORE_LAYOUT);
        LV_IMAGE_DECLARE(img_lvgl_logo);
        lv_image_set_src(logo, &img_lvgl_logo);
        lv_obj_align(logo, LV_ALIGN_LEFT_MID, -LV_HOR_RES / 2 + 25, 0);

        lv_obj_t * label = lv_demo_widgets_title_create(tab_bar, "");
        lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_label_set_text_fmt(label, "LVGL v%d.%d.%d", lv_version_major(), lv_version_minor(), lv_version_patch());
        lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);

        label = lv_label_create(tab_bar);
        lv_label_set_text_static(label, "Widgets demo");
        lv_obj_add_flag(label, LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_obj_add_style(label, &style_text_muted, 0);
        lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);
    }

    lv_demo_widgets_profile_create(t1);
    lv_demo_widgets_analytics_create(t2);
    lv_demo_widgets_shop_create(t3);

    color_changer_create(tv);
}


void lv_demo_widgets_start_slideshow(void)
{
    lv_obj_update_layout(tv);

    lv_obj_t * cont = lv_tabview_get_content(tv);

    lv_obj_t * tab = lv_obj_get_child(cont, 0);

    int32_t v = lv_obj_get_scroll_bottom(tab);
    uint32_t t = lv_anim_speed(lv_display_get_dpi(NULL));
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, scroll_anim_y_cb);
    lv_anim_set_duration(&a, t);
    lv_anim_set_reverse_duration(&a, t);
    lv_anim_set_values(&a, 0, v);
    lv_anim_set_var(&a, tab);
    lv_anim_set_completed_cb(&a, slideshow_anim_completed_cb);
    lv_anim_start(&a);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void color_changer_create(lv_obj_t * parent)
{
    static lv_palette_t palette[] = {
        LV_PALETTE_BLUE, LV_PALETTE_GREEN, LV_PALETTE_BLUE_GREY,  LV_PALETTE_ORANGE,
        LV_PALETTE_RED, LV_PALETTE_PURPLE, LV_PALETTE_TEAL, LV_PALETTE_LAST
    };

    lv_obj_t * color_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(color_cont);
    lv_obj_set_flex_flow(color_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(color_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(color_cont, LV_OBJ_FLAG_FLOATING);

    lv_obj_set_style_bg_color(color_cont, lv_color_white(), 0);
    lv_obj_set_style_pad_right(color_cont, disp_size == DISP_SMALL ? LV_DPX(47) : LV_DPX(55), 0);
    lv_obj_set_style_bg_opa(color_cont, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(color_cont, LV_RADIUS_CIRCLE, 0);

    if(disp_size == DISP_SMALL) lv_obj_set_size(color_cont, LV_DPX(52), LV_DPX(52));
    else lv_obj_set_size(color_cont, LV_DPX(60), LV_DPX(60));

    lv_obj_align(color_cont, LV_ALIGN_BOTTOM_RIGHT, - LV_DPX(10),  - LV_DPX(10));

    uint32_t i;
    for(i = 0; palette[i] != LV_PALETTE_LAST; i++) {
        lv_obj_t * c = lv_button_create(color_cont);
        lv_obj_set_style_bg_color(c, lv_palette_main(palette[i]), 0);
        lv_obj_set_style_radius(c, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_opa(c, LV_OPA_TRANSP, 0);
        lv_obj_set_size(c, 20, 20);
        lv_obj_add_event_cb(c, color_event_cb, LV_EVENT_ALL, &palette[i]);
        lv_obj_remove_flag(c, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    }

    lv_obj_t * btn = lv_button_create(parent);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_FLOATING | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(btn, lv_color_white(), LV_STATE_CHECKED);
    lv_obj_set_style_pad_all(btn, 10, 0);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(btn, color_changer_event_cb, LV_EVENT_ALL, color_cont);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_bg_image_src(btn, LV_SYMBOL_TINT, 0);

    if(disp_size == DISP_SMALL) {
        lv_obj_set_size(btn, LV_DPX(42), LV_DPX(42));
        lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -LV_DPX(15), -LV_DPX(15));
    }
    else {
        lv_obj_set_size(btn, LV_DPX(50), LV_DPX(50));
        lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, -LV_DPX(15), -LV_DPX(15));
    }
}

static void color_changer_anim_cb(void * var, int32_t v)
{
    lv_obj_t * obj = var;
    int32_t max_w = lv_obj_get_width(lv_obj_get_parent(obj)) - LV_DPX(20);
    int32_t w;

    if(disp_size == DISP_SMALL) {
        w = lv_map(v, 0, 256, LV_DPX(52), max_w);
        lv_obj_set_width(obj, w);
        lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, - LV_DPX(10),  - LV_DPX(10));
    }
    else {
        w = lv_map(v, 0, 256, LV_DPX(60), max_w);
        lv_obj_set_width(obj, w);
        lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, - LV_DPX(10),  - LV_DPX(10));
    }

    if(v > LV_OPA_COVER) v = LV_OPA_COVER;

    uint32_t i;
    for(i = 0; i < lv_obj_get_child_count(obj); i++) {
        lv_obj_set_style_opa(lv_obj_get_child(obj, i), v, 0);
    }

}

static void color_changer_event_cb(lv_event_t * e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t * color_cont = lv_event_get_user_data(e);
        if(lv_obj_get_width(color_cont) < LV_HOR_RES / 2) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 0, 256);
            lv_anim_set_duration(&a, 200);
            lv_anim_start(&a);
        }
        else {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 256, 0);
            lv_anim_set_duration(&a, 200);
            lv_anim_start(&a);
        }
    }
}
static void color_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        lv_obj_t * color_cont = lv_obj_get_parent(obj);
        if(lv_obj_get_width(color_cont) < LV_HOR_RES / 2) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, color_cont);
            lv_anim_set_exec_cb(&a, color_changer_anim_cb);
            lv_anim_set_values(&a, 0, 256);
            lv_anim_set_duration(&a, 200);
            lv_anim_start(&a);
        }
    }
    else if(code == LV_EVENT_CLICKED) {
        extern lv_obj_t * chart1;
        extern lv_obj_t * chart2;

        extern lv_chart_series_t * ser1;
        extern lv_chart_series_t * ser3;


        lv_palette_t * palette_primary = lv_event_get_user_data(e);
        lv_palette_t palette_secondary = (*palette_primary) + 3; /*Use another palette as secondary*/
        if(palette_secondary >= LV_PALETTE_LAST) palette_secondary = 0;
#if LV_USE_THEME_DEFAULT
        lv_theme_default_init(NULL, lv_palette_main(*palette_primary), lv_palette_main(palette_secondary),
                              LV_THEME_DEFAULT_DARK, font_normal);
#endif
        lv_color_t color = lv_palette_main(*palette_primary);
        lv_style_set_text_color(&style_icon, color);
        lv_chart_set_series_color(chart1, ser1, color);
        lv_chart_set_series_color(chart2, ser3, color);
    }
}

static void scroll_anim_y_cb(void * var, int32_t v)
{
    lv_obj_scroll_to_y(var, v, LV_ANIM_OFF);
}

static void slideshow_anim_completed_cb(lv_anim_t * a_old)
{
    LV_UNUSED(a_old);

    lv_obj_t * cont = lv_tabview_get_content(tv);
    uint32_t tab_id = lv_tabview_get_tab_active(tv);
    tab_id += 1;
    if(tab_id > 2) tab_id = 0;
    lv_tabview_set_active(tv, tab_id, LV_ANIM_ON);

    lv_obj_t * tab = lv_obj_get_child(cont, tab_id);
    lv_obj_scroll_to_y(tab, 0, LV_ANIM_OFF);
    lv_obj_update_layout(tv);

    int32_t v = lv_obj_get_scroll_bottom(tab);
    uint32_t t = lv_anim_speed(lv_display_get_dpi(NULL));

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, scroll_anim_y_cb);
    lv_anim_set_duration(&a, t);
    lv_anim_set_reverse_duration(&a, t);
    lv_anim_set_values(&a, 0, v);
    lv_anim_set_var(&a, tab);
    lv_anim_set_completed_cb(&a, slideshow_anim_completed_cb);
    lv_anim_start(&a);
}


static void tabview_delete_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_DELETE) {
        lv_style_reset(&style_text_muted);
        lv_style_reset(&style_title);
        lv_style_reset(&style_icon);
        lv_style_reset(&style_bullet);
    }
}
#endif
