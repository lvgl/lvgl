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

lv_demo_high_res_subjects_t * lv_demo_high_res(const char * assets_base_path)
{
    if(assets_base_path == NULL) assets_base_path = "lvgl/demos/high_res/assets";
    lv_obj_t * base_obj = lv_demo_high_res_base_obj_create(assets_base_path);
    lv_demo_high_res_home(base_obj);

    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(base_obj);
    return &c->subjects;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DEMO_HIGH_RES*/
