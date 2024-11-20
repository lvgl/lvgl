/**
 * @file lv_demo_high_res_private.h
 *
 */

#ifndef LV_DEMO_HIGH_RES_PRIVATE_H
#define LV_DEMO_HIGH_RES_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../src/lv_conf_internal.h"
#if LV_USE_DEMO_HIGH_RES

#include "lv_demo_high_res.h"
#include "../../src/core/lv_obj.h"
#include "../../src/others/observer/lv_observer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

enum {
    IMG_ALBUM_ART = 0,
    IMG_ARROW_LEFT,
    IMG_BACKWARD_ICON,
    IMG_COLD_ICON,
    IMG_DRY_ICON,
    IMG_ENERGY_ICON,
    IMG_EV_CHARGING_APP_ICON,
    IMG_EV_CHARGING_WIDGET3_1_BG,
    IMG_EV_CHARGING_WIDGET3_BG,
    IMG_FAN,
    IMG_FORWARD_ICON,
    IMG_HEALTH_ICON,
    IMG_HEALTH_ICON_BOLD,
    IMG_HEAT_ICON,
    IMG_LAMP,
    IMG_LOGOUT_ICON,
    IMG_MAIN_LIGHT_SLIDER,
    IMG_MINUS,
    IMG_PAGER_LEFT,
    IMG_PAGER_RIGHT,
    IMG_PLAY_ICON,
    IMG_PLAY_ICON_1,
    IMG_PLUS,
    IMG_RANGE_ICON,
    IMG_SECURITY_APP_ICON,
    IMG_SETTING_ICON,
    IMG_SETTING_ICON_BOLD,
    IMG_SMART_HOME_APP_ICON,
    IMG_SMART_HOME_WIDGET1_BG,
    IMG_SMART_METER_APP_ICON,
    IMG_THERMOSTAT_APP_ICON,
    IMG_TIME_ICON,
    IMG_UNLOCK,
    IMG_VOLUME,
    IMG_WEATHER,
    IMG_WIFI_ICON,
    IMG_LIGHT_BG_EV_CHARGING,
    IMG_DARK_BG_EV_CHARGING,
    IMG_LIGHT_BG_HOME,
    IMG_DARK_BG_HOME,
    IMG_LIGHT_BG_SECURITY,
    IMG_DARK_BG_SECURITY,
    IMG_LIGHT_BG_SMART_HOME,
    IMG_DARK_BG_SMART_HOME,
    IMG_LIGHT_BG_SMART_METER,
    IMG_DARK_BG_SMART_METER,
    IMG_LIGHT_BG_THERMOSTAT,
    IMG_DARK_BG_THERMOSTAT,
    IMG_LIGHT_DARK_THEME_ICON,
    IMG_DARK_DARK_THEME_ICON,
    IMG_LIGHT_LIGHT_THEME_ICON,
    IMG_DARK_LIGHT_THEME_ICON,
    IMG_LIGHT_WIDGET1_BG,
    IMG_DARK_WIDGET1_BG,
    IMG_LIGHT_WIDGET2_BG,
    IMG_DARK_WIDGET2_BG,
    IMG_LIGHT_WIDGET3_BG,
    IMG_DARK_WIDGET3_BG,
    IMG_LIGHT_WIDGET4_BG,
    IMG_DARK_WIDGET4_BG,
    IMG_LIGHT_WIDGET5_BG,
    IMG_DARK_WIDGET5_BG,
    IMG_COUNT
};

enum {
    STYLE_COLOR_BASE = 0,
    STYLE_COLOR_ACCENT,
    STYLE_COLOR_COUNT
};

enum {
    STYLE_TYPE_OBJ = 0,
    STYLE_TYPE_TEXT,
    STYLE_TYPE_A8_IMG,
    STYLE_TYPE_COUNT
};

enum {
    FONT_HEADING_MD = 0,
    FONT_HEADING_LG,
    FONT_HEADING_XL,
    FONT_HEADING_XXL,
    FONT_LABEL_XS,
    FONT_LABEL_SM,
    FONT_LABEL_MD,
    FONT_LABEL_LG,
    FONT_LABEL_XL,
    FONT_LABEL_2XL,
    FONT_COUNT
};

enum {
    SIZE_SM = 0,
    SIZE_MD,
    SIZE_LG,
    SIZE_COUNT
};

enum {
    ICON_SM = 0,
    ICON_MD,
    ICON_LG,
    ICON_XL,
    ICON_2XL,
    ICON_COUNT
};

typedef struct {
    lv_color_t base;
    lv_color_t accent;
} lv_demo_high_res_theme_t;

typedef struct {
    int32_t gap[10 + 1];
    int32_t icon[ICON_COUNT];
    int32_t card_long_edge;
    int32_t widget_long_edge;
    int32_t card_short_edge;
    int32_t ev_charging_arc_diameter;
    int32_t smart_meter_collapsed_part_height;
    int32_t slider_width;
    int32_t small_chart_height;
    int32_t large_chart_height;
    int32_t card_radius;
    int32_t health_panel_width;
    int32_t settings_panel_width;
    int32_t home_bottom_margin_height;
    void (*init_fonts_cb)(lv_style_t * fonts);
} lv_demo_high_res_sizes_t;

typedef struct {
    struct {
        lv_subject_t group;
        lv_subject_t * members[2];
    } logo;
    struct {
        lv_subject_t group;
        lv_subject_t * members[2];
    } time;
    struct {
        lv_subject_t group;
        lv_subject_t * members[3];
    } date;
    struct {
        lv_subject_t group;
        lv_subject_t * members[2];
    } temps_high_low;
    struct {
        lv_subject_t group;
        lv_subject_t * members[2];
    } gas_savings;
} lv_demo_high_res_subject_groups_t;

typedef struct {
    const lv_demo_high_res_sizes_t * sz;
    lv_image_dsc_t * imgs[IMG_COUNT];
    lv_style_t styles[STYLE_COLOR_COUNT][STYLE_TYPE_COUNT];
    lv_style_t fonts[FONT_COUNT];
    lv_subject_t th;
    char * base_path;
    bool top_margin_subjects_are_init;
    lv_subject_t top_margin_health_subject;
    lv_subject_t top_margin_setting_subject;
    lv_demo_high_res_subjects_t subjects;
    lv_demo_high_res_subject_groups_t subject_groups;
} lv_demo_high_res_ctx_t;

LV_ATTRIBUTE_EXTERN_DATA extern const lv_demo_high_res_theme_t lv_demo_high_res_theme_light;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_demo_high_res_theme_t lv_demo_high_res_theme_dark;
LV_ATTRIBUTE_EXTERN_DATA extern const lv_demo_high_res_sizes_t lv_demo_high_res_sizes_all[SIZE_COUNT];

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_demo_high_res_base_obj_create(const char * base_path);

void lv_demo_high_res_home(lv_obj_t * base_obj);
void lv_demo_high_res_app_smart_home(lv_obj_t * base_obj);
void lv_demo_high_res_app_ev_charging(lv_obj_t * base_obj);
void lv_demo_high_res_app_smart_meter(lv_obj_t * base_obj);
void lv_demo_high_res_app_thermostat(lv_obj_t * base_obj);
void lv_demo_high_res_app_security(lv_obj_t * base_obj);
lv_obj_t * lv_demo_high_res_top_margin_create(lv_obj_t * base_obj, lv_obj_t * parent, int32_t pad_hor, bool show_time,
                                              lv_demo_high_res_ctx_t * c);
void lv_demo_high_res_top_margin_deinit_subjects(lv_demo_high_res_ctx_t * c);

lv_obj_t * lv_demo_high_res_simple_container_create(lv_obj_t * parent, bool vertical, int32_t pad,
                                                    lv_flex_align_t align_cross_place);
void lv_demo_high_res_label_bind_text_tenths(lv_obj_t * label, lv_subject_t * subject, const char * fmt);
void lv_demo_high_res_fmt_tenths(char * dst, uint32_t dst_size, int32_t val);
void lv_demo_high_res_theme_observer_image_src_cb(lv_observer_t * observer, lv_subject_t * subject);
void lv_demo_high_res_theme_observer_obj_bg_image_src_cb(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_HIGH_RES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_HIGH_RES_PRIVATE_H*/
