/**
 * @file lv_demo_high_res.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_high_res_private.h"
#if LV_USE_DEMO_HIGH_RES

#if LV_FS_DEFAULT_DRIVE_LETTER == '\0'
    #error set a default drive letter (and enable an FS driver) for the high res demo
#endif

#if !LV_FONT_FMT_TXT_LARGE
    #error LV_FONT_FMT_TXT_LARGE needs to be enabled in lv_conf.h for the high res demo
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_demo_high_res_api_t * lv_demo_high_res(const char * assets_path,
                                          const char * logo_path,
                                          const char * slides_path,
                                          lv_demo_high_res_exit_cb_t exit_cb)
{
    if(assets_path == NULL) assets_path = "lvgl/demos/high_res/assets";
    if(logo_path == NULL) logo_path = "lvgl/demos/high_res/assets/img_lv_demo_high_res_lvgl_logo.png";
    if(slides_path == NULL) slides_path = "about_app_slides";
    lv_obj_t * base_obj = lv_demo_high_res_base_obj_create(assets_path, logo_path, slides_path, exit_cb);
    lv_demo_high_res_home(base_obj);

    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(base_obj);
    return &c->api;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DEMO_HIGH_RES*/
