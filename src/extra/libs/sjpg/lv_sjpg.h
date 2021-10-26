/**
 * @file lv_sjpg.h
 *
 */

#ifndef LV_SJPEG_H
#define LV_SJPEG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stdio.h>

#if LV_USE_SJPG

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_split_jpeg_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SJPG*/

#ifdef __cplusplus
}
#endif

#endif /* LV_SJPEG_H */
