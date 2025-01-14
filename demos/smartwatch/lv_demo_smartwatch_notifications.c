/**
 * @file lv_demo_smartwatch_notifications.c
 * Notifications screen layout & functions. Contains the list of available notifications
 * as well as each individual notification when opened.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_notifications.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_screen_notifications(void);
static void notification_screen_events_cb(lv_event_t * e);
static void notification_clicked_event_cb(lv_event_t * e);

static void set_notification_icon(lv_obj_t * obj, int app_id);
static int get_notification_icon_index(int id);

static lv_smartwatch_notification_click_cb_t notification_click_cb;

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * notification_screen;
static lv_obj_t * message_info_panel;
static lv_obj_t * message_panel;
static lv_obj_t * message_icon;
static lv_obj_t * message_time;
static lv_obj_t * message_content;
static lv_obj_t * message_list_panel;
static lv_obj_t * message_list;
static lv_obj_t * empty_info;

static const lv_image_dsc_t * notification_icons[] = {
    &img_sms_icon,       /* SMS */
    &img_mail_icon,      /* Mail */
    &img_penguin_icon,   /* Penguin (QQ) */
    &img_skype_icon,     /* Skype */
    &img_whatsapp_icon,  /* WhatsApp */
    &img_mail_icon,      /* Mail */
    &img_line_icon,      /* Line */
    &img_twitter_x_icon, /* Twitter */
    &img_facebook_icon,  /* Facebook */
    &img_messenger_icon, /* Messenger */
    &img_instagram_icon, /* Instagram */
    &img_weibo_icon,     /* Weibo */
    &img_kakao_icon,     /* Kakao */
    &img_viber_icon,     /* Viber */
    &img_vkontakte_icon, /* Vkontakte */
    &img_telegram_icon,  /* Telegram */
    &img_chrns_icon,     /* Chronos */
    &img_wechat_icon     /* Wechat */
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_notifications_create(void)
{
    create_screen_notifications();


    /* add demo notifications */
    lv_demo_smartwatch_add_notification(0x03, "Sample Message test", 0x03);
    lv_demo_smartwatch_add_notification(0x0B, "Sample Message test", 0x0B);
    lv_demo_smartwatch_add_notification(0x08, "Sample Message test", 0x08);
    lv_demo_smartwatch_add_notification(0x14, "Sample Message test", 0x14);
    lv_demo_smartwatch_add_notification(0x0A, "Sample Message test", 0x0A);
    lv_demo_smartwatch_add_notification(0x12, "Sample Message test", 0x12);
    lv_demo_smartwatch_add_notification(0x16, "Sample Message test", 0x16);
    lv_demo_smartwatch_add_notification(0x17, "Sample Message test", 0x17);
}

void lv_demo_smartwatch_notifications_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay)
{
    lv_screen_load_anim(notification_screen, anim_type, time, delay, false);
}

void lv_demo_smartwatch_clear_notifications(void)
{
    lv_obj_clean(message_list);
    lv_obj_remove_flag(empty_info, LV_OBJ_FLAG_HIDDEN);
}

void lv_demo_smartwatch_add_notification(int app_id, const char * message, int index)
{
    lv_obj_t * notification_item = lv_obj_create(message_list);
    lv_obj_set_width(notification_item, lv_pct(90));
    lv_obj_set_height(notification_item, LV_SIZE_CONTENT);
    lv_obj_set_align(notification_item, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(notification_item, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(notification_item, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(notification_item, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(notification_item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(notification_item, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(notification_item, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(notification_item, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(notification_item, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(notification_item, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(notification_item, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(notification_item, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(notification_item, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(notification_item, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(notification_item, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * notification_icon = lv_image_create(notification_item);
    lv_image_set_src(notification_icon, &img_chrns_icon);
    lv_obj_set_width(notification_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(notification_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(notification_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(notification_icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(notification_icon, LV_OBJ_FLAG_SCROLLABLE);
    set_notification_icon(notification_icon, app_id);

    lv_obj_t * notification_text = lv_label_create(notification_item);
    lv_obj_set_width(notification_text, 140);
    lv_obj_set_height(notification_text, LV_SIZE_CONTENT);
    lv_obj_set_align(notification_text, LV_ALIGN_CENTER);
    lv_label_set_long_mode(notification_text, LV_LABEL_LONG_DOT);
    lv_label_set_text(notification_text, message);

    lv_obj_add_flag(empty_info, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_event_cb(notification_item, notification_clicked_event_cb, LV_EVENT_CLICKED, (void *)(intptr_t)index);
}

void lv_demo_smartwatch_show_notification(int app_id, const char * message, const char * time)
{
    lv_label_set_text(message_time, time);
    lv_label_set_text(message_content, message);
    set_notification_icon(message_icon, app_id);
    lv_obj_scroll_to_y(message_panel, 0, LV_ANIM_ON);

    lv_tileview_set_tile_by_index(notification_screen, 1, 0, LV_ANIM_ON);
}

void lv_demo_smartwatch_set_notification_click_cb(lv_smartwatch_notification_click_cb_t cb)
{
    notification_click_cb = cb;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int get_notification_icon_index(int id)
{
    switch(id) {
        case 0x03:
            return 0;
        case 0x04:
            return 1;
        case 0x07:
            return 2;
        case 0x08:
            return 3;
        case 0x0A:
            return 4;
        case 0x0B:
            return 5;
        case 0x0E:
            return 6;
        case 0x0F:
            return 7;
        case 0x10:
            return 8;
        case 0x11:
            return 9;
        case 0x12:
            return 10;
        case 0x13:
            return 11;
        case 0x14:
            return 12;
        case 0x16:
            return 13;
        case 0x17:
            return 14;
        case 0x18:
            return 15;
        case 0xC0:
            return 16;
        case 0x09:
            return 17;
        default:
            return 0;
    }
}

static void set_notification_icon(lv_obj_t * obj, int app_id)
{
    lv_image_set_src(obj, notification_icons[get_notification_icon_index(app_id)]);
}

static void notification_clicked_event_cb(lv_event_t * e)
{
    intptr_t index = (intptr_t)lv_event_get_user_data(e);

    /* send clicked action to the user callback function if defined, otherwise load a test message */
    if(notification_click_cb != NULL) {
        notification_click_cb(index);
    }
    else {
        /* test notification */
        lv_demo_smartwatch_show_notification(index, "Sample Message test", "12:45");
    }
}

static void notification_screen_events_cb(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_SCREEN_LOAD_START) {
        lv_tileview_set_tile_by_index(notification_screen, 0, 0, LV_ANIM_OFF);

        lv_obj_scroll_by(message_list, 0, 1, LV_ANIM_OFF);
        lv_obj_scroll_by(message_list, 0, -1, LV_ANIM_OFF);

        lv_obj_set_scrollbar_mode(notification_screen, lv_demo_smartwatch_get_scrollbar_mode());
        lv_obj_set_scrollbar_mode(message_panel, lv_demo_smartwatch_get_scrollbar_mode());
        lv_obj_set_scrollbar_mode(message_list, lv_demo_smartwatch_get_scrollbar_mode());

        if(lv_demo_smartwatch_get_load_app_list()) {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT);
        }
        else {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_RIGHT);
        }

    }

    if(event_code == LV_EVENT_VALUE_CHANGED) {
        if(lv_tileview_get_tile_active(notification_screen) == message_list_panel) {

            if(lv_demo_smartwatch_get_load_app_list()) {
                lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT);
            }
            else {
                lv_demo_smartwatch_show_scroll_hint(LV_DIR_RIGHT);
            }
        }
        else if(lv_tileview_get_tile_active(notification_screen) == message_info_panel) {
            if(lv_demo_smartwatch_get_load_app_list()) {
                lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT);
            }
            else {
                lv_demo_smartwatch_show_scroll_hint(LV_DIR_RIGHT | LV_DIR_LEFT);
            }
        }
    }

    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_LEFT) {
        if(!lv_demo_smartwatch_get_load_app_list()) {
            lv_demo_smartwatch_home_load(LV_SCR_LOAD_ANIM_OUT_LEFT, 500, 0);
        }
        else {
            LV_LOG_WARN("Swipe right to exit");
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT);
        }
    }
    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        if(lv_demo_smartwatch_get_load_app_list()) {
            lv_demo_smartwatch_list_load(LV_SCR_LOAD_ANIM_MOVE_RIGHT, 500, 0);
        }
        else {
            LV_LOG_WARN("Swipe left to exit");
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_RIGHT);
        }
    }

    if(event_code == LV_EVENT_GESTURE && (lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_TOP ||
                                          lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_BOTTOM) &&
       lv_tileview_get_tile_active(notification_screen) == message_list_panel) {
        if(lv_demo_smartwatch_get_load_app_list()) {
            LV_LOG_WARN("Swipe right to exit");
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT);
        }
        else {
            LV_LOG_WARN("Swipe left to exit");
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_RIGHT);
        }
    }


}

static void create_screen_notifications(void)
{

    notification_screen = lv_tileview_create(NULL);
    lv_obj_set_style_bg_color(notification_screen, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(notification_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    message_info_panel = lv_tileview_add_tile(notification_screen, 1, 0, LV_DIR_LEFT);
    lv_obj_set_width(message_info_panel, lv_pct(100));
    lv_obj_set_height(message_info_panel, lv_pct(100));

    message_panel = lv_obj_create(message_info_panel);
    lv_obj_set_width(message_panel, lv_pct(100));
    lv_obj_set_height(message_panel, lv_pct(100));
    lv_obj_set_align(message_panel, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(message_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(message_panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(message_panel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(message_panel, LV_DIR_VER);
    lv_obj_set_style_radius(message_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(message_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(message_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(message_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(message_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(message_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(message_panel, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(message_panel, 70, LV_PART_MAIN | LV_STATE_DEFAULT);

    message_icon = lv_image_create(message_panel);
    lv_image_set_src(message_icon, &img_chrns_icon);
    lv_obj_set_width(message_icon, LV_SIZE_CONTENT);
    lv_obj_set_height(message_icon, LV_SIZE_CONTENT);
    lv_obj_set_align(message_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(message_icon, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_remove_flag(message_icon, LV_OBJ_FLAG_SCROLLABLE);

    message_time = lv_label_create(message_panel);
    lv_obj_set_width(message_time, LV_SIZE_CONTENT);
    lv_obj_set_height(message_time, LV_SIZE_CONTENT);
    lv_obj_set_align(message_time, LV_ALIGN_CENTER);
    lv_label_set_text(message_time, "Chronos");

    message_content = lv_label_create(message_panel);
    lv_obj_set_width(message_content, 180);
    lv_obj_set_height(message_content, LV_SIZE_CONTENT);
    lv_obj_set_align(message_content, LV_ALIGN_CENTER);
    lv_label_set_text(message_content, "Download from Google Play to sync time and receive notifications");

    message_list_panel = lv_tileview_add_tile(notification_screen, 0, 0, LV_DIR_NONE);
    lv_obj_set_width(message_list_panel, lv_pct(100));
    lv_obj_set_height(message_list_panel, lv_pct(100));

    message_list = lv_obj_create(message_list_panel);

    lv_obj_set_width(message_list, lv_pct(100));
    lv_obj_set_height(message_list, lv_pct(100));
    lv_obj_set_align(message_list, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(message_list, LV_FLEX_FLOW_COLUMN_REVERSE);
    lv_obj_set_flex_align(message_list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(message_list, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(message_list, LV_DIR_VER);
    lv_obj_set_style_radius(message_list, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(message_list, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(message_list, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(message_list, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(message_list, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(message_list, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(message_list, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(message_list, 70, LV_PART_MAIN | LV_STATE_DEFAULT);

    empty_info = lv_label_create(message_list_panel);
    lv_obj_set_width(empty_info, 180);
    lv_obj_set_height(empty_info, LV_SIZE_CONTENT);
    lv_obj_set_align(empty_info, LV_ALIGN_CENTER);
    lv_label_set_text(empty_info, "The are no notifications currently, come back later");
    lv_obj_add_flag(empty_info, LV_OBJ_FLAG_HIDDEN);

    lv_obj_add_event_cb(message_list, lv_demo_smartwatch_scroll_event, LV_EVENT_ALL, NULL);

    lv_obj_add_event_cb(notification_screen, notification_screen_events_cb, LV_EVENT_ALL, NULL);
}

#endif /*LV_USE_DEMO_SMARTWATCH*/
