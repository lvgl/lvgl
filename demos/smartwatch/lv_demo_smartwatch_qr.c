/**
 * @file lv_demo_smartwatch_qr.c
 * QR code screen layout & functions. Contains QR code for quick access to items such as contact cards etc.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include <string.h>
#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_qr.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void qr_screen_create(void);
static void qr_screen_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_obj_t * qr_screen;
static lv_obj_t * qr_panel;
static lv_obj_t * qr_item;
static lv_obj_t * qr_icon;
static lv_obj_t * qr_image;
static lv_obj_t * qr_label;

static const lv_image_dsc_t * qr_image_icons[] = {
    &img_chrns_icon,
    &img_wechat_icon,
    &img_facebook_icon,
    &img_penguin_icon,
    &img_twitter_x_icon,
    &img_whatsapp_icon,
    &img_wechat_pay_icon, /* wechat pay */
    &img_alipay_icon,     /* alipay */
    &img_paypal_icon,     /* paypal */
};

const char * qr_title_names[9] = {
    "Chronos",
    "Wechat",
    "Facebook",
    "QQ",
    "X (Twitter)",
    "Whatsapp",
    "Wechat Pay",
    "Alipay",
    "Paypal"
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_qr_create(void)
{
    qr_screen_create();

#if LV_USE_QRCODE == 1

    lv_demo_smartwatch_qr_list_add(0, "https://chronos.ke/");
    lv_demo_smartwatch_qr_list_add(1, "https://www.youtube.com/c/fbiego");
    lv_demo_smartwatch_qr_list_add(2, "https://play.google.com/store/apps/details?id=com.fbiego.chronos");
    lv_demo_smartwatch_qr_list_add(3, "https://github.com/fbiego/esp32-c3-mini");
    lv_demo_smartwatch_qr_list_add(4, "https://x.com/chronos_app");
    lv_demo_smartwatch_qr_list_add(5, "https://felix.fbiego.com");
    lv_demo_smartwatch_qr_list_add(6, "https://www.linkedin.com/in/fbiego/");
    lv_demo_smartwatch_qr_list_add(7, "https://ko-fi.com/fbiego");
    lv_demo_smartwatch_qr_list_add(8, "https://www.paypal.com/paypalme/biego");

#else
    LV_LOG_WARN("QR Code feature not enabled");

    lv_obj_t * info = lv_label_create(qr_panel);
    lv_obj_set_width(info, lv_pct(75));
    lv_obj_set_height(info, LV_SIZE_CONTENT);
    lv_obj_set_align(info, LV_ALIGN_CENTER);
    lv_label_set_text(info, "QR feature is not enabled. Enable it in lv_conf.h (LV_USE_QRCODE 1)");

#endif /*LV_USE_QRCODE*/
}

void lv_demo_smartwatch_qr_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay)
{
    lv_screen_load_anim(qr_screen, anim_type, time, delay, false);
}

void lv_demo_smartwatch_qr_list_clear(void)
{
    lv_obj_clean(qr_panel);
}

void lv_demo_smartwatch_qr_list_add(uint8_t id, const char * link)
{
#if LV_USE_QRCODE == 1
    qr_item = lv_obj_create(qr_panel);
    lv_obj_set_width(qr_item, lv_pct(85));
    lv_obj_set_height(qr_item, lv_pct(100));
    lv_obj_set_align(qr_item, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(qr_item, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(qr_item, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(qr_item, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(qr_item, LV_OBJ_FLAG_SNAPPABLE);
    lv_obj_remove_flag(qr_item, LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_set_style_bg_color(qr_item, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(qr_item, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(qr_item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    qr_icon = lv_image_create(qr_item);
    lv_image_set_src(qr_icon, qr_image_icons[id % 9]);
    lv_obj_set_width(qr_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(qr_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(qr_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(qr_icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(qr_icon, LV_OBJ_FLAG_SCROLLABLE);

    qr_image = lv_qrcode_create(qr_item);
    lv_qrcode_set_size(qr_image, 150);
    lv_qrcode_set_dark_color(qr_image, lv_color_black());
    lv_qrcode_set_light_color(qr_image, lv_color_white());
    lv_qrcode_update(qr_image, link, strlen(link));
    lv_obj_center(qr_image);
    lv_obj_set_style_border_color(qr_image, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_width(qr_image, 5, 0);
    lv_obj_set_align(qr_image, LV_ALIGN_CENTER);
    lv_obj_remove_flag(qr_image, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(qr_image, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(qr_image, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    qr_label = lv_label_create(qr_item);
    lv_obj_set_width(qr_label, LV_SIZE_CONTENT);
    lv_obj_set_height(qr_label, LV_SIZE_CONTENT);
    lv_obj_set_align(qr_label, LV_ALIGN_CENTER);
    lv_label_set_text(qr_label, qr_title_names[id % 9]);
    lv_obj_set_style_text_font(qr_label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
#else
    LV_LOG_WARN("QR Code not added, feature is not enabled");
#endif /*LV_USE_QRCODE*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void qr_screen_create(void)
{
    qr_screen = lv_obj_create(NULL);
    lv_obj_remove_flag(qr_screen, LV_OBJ_FLAG_SCROLLABLE);

    qr_panel = lv_obj_create(qr_screen);
    lv_obj_set_width(qr_panel, lv_pct(100));
    lv_obj_set_height(qr_panel, lv_pct(100));
    lv_obj_set_align(qr_panel, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(qr_panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(qr_panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_add_flag(qr_panel, LV_OBJ_FLAG_SCROLL_ONE);
    lv_obj_set_scrollbar_mode(qr_panel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_snap_x(qr_panel, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scroll_dir(qr_panel, LV_DIR_HOR);
    lv_obj_set_style_radius(qr_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(qr_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(qr_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(qr_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(qr_panel, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(qr_panel, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(qr_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(qr_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(qr_screen, qr_screen_event_cb, LV_EVENT_ALL, NULL);
}

static void qr_screen_event_cb(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_TOP) {
        if(lv_demo_smartwatch_get_load_app_list()) {
            lv_demo_smartwatch_home_load(LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0);
        }
        else {
            lv_demo_smartwatch_home_load(LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0);
        }
    }

    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_BOTTOM) {
        lv_demo_smartwatch_show_scroll_hint(LV_DIR_BOTTOM);
    }

    if(event_code == LV_EVENT_SCREEN_LOAD_START) {
        lv_obj_set_scrollbar_mode(qr_panel, lv_demo_smartwatch_get_scrollbar_mode());

        lv_obj_update_snap(qr_panel, LV_ANIM_OFF);

        lv_demo_smartwatch_show_scroll_hint(LV_DIR_BOTTOM);

    }
}

#endif /*LV_USE_DEMO_SMARTWATCH*/
