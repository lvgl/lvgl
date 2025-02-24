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

#if LV_USE_OS != LV_OS_NONE

void lv_lock(void)
{
    lv_mutex_lock(&lv_general_mutex);
}

lv_result_t lv_lock_isr(void)
{
    return lv_mutex_lock_isr(&lv_general_mutex);
}

void lv_unlock(void)
{
    lv_mutex_unlock(&lv_general_mutex);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


#endif /*LV_USE_OS != LV_OS_NONE*/
