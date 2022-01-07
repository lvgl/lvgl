/**
 * @file lv_bin.h
 *
 */

#ifndef LV_BIN_H
#define LV_BIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lv_conf_internal.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/
/** Use this macro to declare an image in a C file */
#define LV_IMG_DECLARE(var_name) extern const lv_img_dsc_t var_name;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BMP_H*/


