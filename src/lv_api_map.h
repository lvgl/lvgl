/**
 * @file lv_api_map.h
 *
 */

#ifndef LV_API_MAP_H
#define LV_API_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl.h"

/*********************
 *      DEFINES
 *********************/

#define LV_NO_TASK_READY        LV_NO_TIMER_READY
#define LV_INDEV_STATE_REL      LV_INDEV_STATE_RELEASED
#define LV_INDEV_STATE_PR       LV_INDEV_STATE_PRESSED
#define LV_OBJ_FLAG_SNAPABLE    LV_OBJ_FLAG_SNAPPABLE   /*Fixed typo*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline LV_ATTRIBUTE_TIMER_HANDLER uint32_t lv_task_handler(void)
{
    return lv_timer_handler();
}

/**********************
 *      MACROS
 **********************/

 /**********************
  * DEPRECATED FUNCTIONS
  **********************/

inline uint32_t lv_obj_get_child_id(const struct _lv_obj_t* obj)
{
    LV_LOG_USER("lv_obj_get_child_id(obj) is deprecated, please use lv_obj_get_index(obj).");
    return lv_obj_get_index(obj);
}


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_API_MAP_H*/
