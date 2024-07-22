/**
 * @file lv_os.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_os.h"
#include "lv_os_private.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define lv_general_mutex LV_GLOBAL_DEFAULT()->lv_general_mutex

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
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

void lv_os_init(void)
{
#if LV_USE_OS != LV_OS_NONE
    lv_mutex_init(&lv_general_mutex);
#endif /*LV_USE_OS != LV_OS_NONE*/
}

void lv_lock(void)
{
#if LV_USE_OS != LV_OS_NONE
    lv_mutex_lock(&lv_general_mutex);
#endif /*LV_USE_OS != LV_OS_NONE*/
}

lv_result_t lv_lock_isr(void)
{
#if LV_USE_OS != LV_OS_NONE
    return lv_mutex_lock_isr(&lv_general_mutex);
#else /*LV_USE_OS != LV_OS_NONE*/
    return LV_RESULT_OK;
#endif /*LV_USE_OS != LV_OS_NONE*/
}

void lv_unlock(void)
{
#if LV_USE_OS != LV_OS_NONE
    lv_mutex_unlock(&lv_general_mutex);
#endif /*LV_USE_OS != LV_OS_NONE*/
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

