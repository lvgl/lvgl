/**
 * @file lv_test_conf.h
 *
 */

#ifndef LV_TEST_CONF_H
#define LV_TEST_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

uint32_t custom_tick_get(void);
#define LV_TICK_CUSTOM_SYS_TIME_EXPR custom_tick_get()

typedef int16_t lv_coord_t;
typedef void * lv_disp_drv_user_data_t;             /*Type of user data in the display driver*/
typedef void * lv_indev_drv_user_data_t;            /*Type of user data in the input device driver*/
typedef void * lv_font_user_data_t;
typedef void * lv_obj_user_data_t;
typedef void * lv_anim_user_data_t;
typedef void * lv_group_user_data_t;
typedef void * lv_fs_drv_user_data_t;
typedef void * lv_img_decoder_user_data_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEST_CONF_H*/
