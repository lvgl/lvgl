
/**
 * @file lv_linux_private.h
 *
 */

#ifndef LV_LINUX_PRIVATE_H
#define LV_LINUX_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

#define LV_PROC_STAT_PARAMS_LEN 7

/**********************
 *      TYPEDEFS
 **********************/


typedef union {
    struct {
        /*
         *  We ignore the iowait column as it's not reliable
         *  We ignore the guest and guest_nice columns because they're accounted
         *   for in user and nice respectively
         */
        uint32_t user, nice, system, idle, /*iowait,*/ irq, softirq,
                 steal /*, guest, guest_nice*/;
    } fields;
    uint32_t buffer[LV_PROC_STAT_PARAMS_LEN];
} lv_proc_stat_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

uint32_t lv_os_get_proc_idle_percent(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LINUX_PRIVATE_H*/
