/**
 * @file lv_demo_smartwatch_notifications.h
 *
 */

#ifndef LV_DEMO_SMARTWATCH_NOTIFICATIONS_H
#define LV_DEMO_SMARTWATCH_NOTIFICATIONS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"

#if LV_USE_DEMO_SMARTWATCH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create the notifications page. Called only once.
 */
void lv_demo_smartwatch_notifications_create(void);

/**
 * Clear the notification list
 */
void lv_demo_smartwatch_clear_notifications(void);

/**
 * Load the notifications screen
 * @param anim_type     screen load animation to use
 * @param time          animation time
 * @param delay         delay time before loading the screen
 */
void lv_demo_smartwatch_notifications_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay);

/**
 * Add a notification to the list
 * @param app_id    id of the app sending the notification (sets icon from the inbuilt icon list)
 * @param message   the notification message preview
 * @param index     position of the message that will be returned when clicked
 */
void lv_demo_smartwatch_add_notification(int app_id, const char * message, int index);

/**
 * Show a specific notification when in the notification screen
 * @param app_id    id of the app sending the notification (sets icon from the inbuilt icon list)
 * @param message   the notification message content
 * @param time      the notification time
 */
void lv_demo_smartwatch_show_notification(int app_id, const char * message, const char * time);

/**
 * Set the callback funtion when a notification is clicked
 * @param cb callback function [user should call lv_demo_smartwatch_show_notification() to show the notification]
 */
void lv_demo_smartwatch_set_notification_click_cb(lv_smartwatch_notification_click_cb_t cb);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_SMARTWATCH*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DEMO_SMARTWATCH_HOME_H*/
