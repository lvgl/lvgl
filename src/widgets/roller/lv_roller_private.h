/**
 * @file lv_roller_private.h
 *
 */

#ifndef LV_ROLLER_PRIVATE_H
#define LV_ROLLER_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_roller.h"

#if LV_USE_ROLLER != 0
#include "../../core/lv_obj_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_roller_t {
    lv_obj_t obj;
    uint32_t option_cnt;          /**< Number of options*/
    uint32_t sel_opt_id;          /**< Index of the current option*/
    uint32_t sel_opt_id_ori;      /**< Store the original index on focus*/
    uint32_t inf_page_cnt;        /**< Number of extra pages added to make the roller look infinite */
    lv_roller_mode_t mode : 2;
    uint32_t moved : 1;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_ROLLER != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ROLLER_PRIVATE_H*/
