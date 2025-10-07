/**
 * @file lv_linux.h
 *
 */

#ifndef LV_LINUX_H
#define LV_LINUX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_os.h"
#ifdef __linux__
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
} lv_linux_proc_stat_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*__linux__*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LINUX_H*/
