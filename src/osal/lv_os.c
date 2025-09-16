/**
 * @file lv_os.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_os_private.h"
#include "lv_os_private.h"
#include "../core/lv_global.h"
#include "../tick/lv_tick.h"

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

#if LV_USE_OS != LV_OS_NONE
void lv_os_init(void)
{
    lv_mutex_init(&lv_general_mutex);
}


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

#else /*LV_USE_OS != LV_OS_NONE*/

void lv_os_init(void)
{

}

void lv_lock(void)
{
    /*Do nothing*/
}

lv_result_t lv_lock_isr(void)
{
    /*Do nothing*/
    return LV_RESULT_OK;
}

void lv_unlock(void)
{
    /*Do nothing*/
}

void lv_sleep_ms(uint32_t ms)
{
    lv_delay_ms(ms);
}


#endif /*LV_USE_OS != LV_OS_NONE*/

/**********************
 *   STATIC FUNCTIONS
 **********************/


