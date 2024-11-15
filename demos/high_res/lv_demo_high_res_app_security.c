/**
 * @file lv_demo_high_res_app_security.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_high_res_private.h"
#if LV_USE_DEMO_HIGH_RES

#include "../../src/widgets/image/lv_image.h"
#include "../../src/widgets/label/lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void back_clicked_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_high_res_app_security(lv_obj_t * base_obj)
{
    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(base_obj);

    /* background */

    lv_obj_t * bg = base_obj;
    lv_obj_remove_style_all(bg);
    lv_obj_set_size(bg, LV_PCT(100), LV_PCT(100));

    lv_obj_t * bg_img = lv_image_create(bg);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_image_src_cb, bg_img,
                                (void *)&c->imgs[IMG_LIGHT_BG_HOME]);

    lv_obj_t * bg_cont = lv_obj_create(bg);
    lv_obj_remove_style_all(bg_cont);
    lv_obj_set_size(bg_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_top(bg_cont, c->sz->gap[7], 0);
    lv_obj_set_style_pad_bottom(bg_cont, c->sz->gap[10], 0);
    lv_obj_set_style_pad_left(bg_cont, c->sz->gap[10], 0);
    lv_obj_set_style_pad_right(bg_cont, c->sz->gap[10], 0);

    /* top margin */

    lv_obj_t * top_margin = lv_obj_create(bg_cont);
    lv_obj_remove_style_all(top_margin);
    lv_obj_set_size(top_margin, LV_PCT(100), c->sz->gap[10]);
    lv_obj_set_flex_flow(top_margin, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top_margin, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * date_label = lv_label_create(top_margin);
    lv_label_set_text_static(date_label, "Tuesday, 31 October");
    lv_obj_add_style(date_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(date_label, &c->fonts[FONT_LABEL_SM], 0);

    lv_obj_t * time_label = lv_label_create(top_margin);
    lv_label_set_text_static(time_label, "09:36");
    lv_obj_add_style(time_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(time_label, &c->fonts[FONT_LABEL_SM], 0);

    lv_obj_t * top_margin_right_cluster = lv_demo_high_res_simple_container_create(top_margin, false, c->sz->gap[6],
                                                                                   LV_FLEX_ALIGN_CENTER);

    lv_obj_t * wifi_icon = lv_image_create(top_margin_right_cluster);
    lv_image_set_src(wifi_icon, c->imgs[IMG_WIFI_ICON]);
    lv_obj_add_style(wifi_icon, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_A8_IMG], 0);

    lv_obj_t * health_icon = lv_image_create(top_margin_right_cluster);
    lv_image_set_src(health_icon, c->imgs[IMG_HEALTH_ICON]);
    lv_obj_add_style(health_icon, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_A8_IMG], 0);

    lv_obj_t * setting_icon = lv_image_create(top_margin_right_cluster);
    lv_image_set_src(setting_icon, c->imgs[IMG_SETTING_ICON]);
    lv_obj_add_style(setting_icon, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_A8_IMG], 0);

    /* app info */

    lv_obj_t * app_info = lv_demo_high_res_simple_container_create(bg_cont, true, c->sz->gap[4], LV_FLEX_ALIGN_START);
    lv_obj_align_to(app_info, top_margin, LV_ALIGN_OUT_BOTTOM_LEFT, 0, c->sz->gap[7]);

    lv_obj_t * back = lv_demo_high_res_simple_container_create(app_info, false, c->sz->gap[2], LV_FLEX_ALIGN_CENTER);
    lv_obj_add_event_cb(back, back_clicked_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * back_icon = lv_image_create(back);
    lv_image_set_src(back_icon, c->imgs[IMG_ARROW_LEFT]);
    lv_obj_add_style(back_icon, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_A8_IMG], 0);
    lv_obj_add_flag(back_icon, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * back_label = lv_label_create(back);
    lv_label_set_text_static(back_label, "Back");
    lv_obj_set_style_text_opa(back_label, LV_OPA_60, 0);
    lv_obj_add_style(back_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(back_label, &c->fonts[FONT_HEADING_MD], 0);

    lv_obj_t * app_label = lv_label_create(app_info);
    lv_label_set_text_static(app_label, "Security");
    lv_obj_add_style(app_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(app_label, &c->fonts[FONT_HEADING_LG], 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void back_clicked_cb(lv_event_t * e)
{
    lv_obj_t * back = lv_event_get_target_obj(e);

    lv_obj_t * base_obj = lv_obj_get_parent(lv_obj_get_parent(lv_obj_get_parent(back)));
    lv_obj_clean(base_obj);
    lv_demo_high_res_home(base_obj);
}

#endif /*LV_USE_DEMO_HIGH_RES*/
