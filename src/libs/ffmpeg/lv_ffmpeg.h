/**
 * @file lv_ffmpeg.h
 *
 */
#ifndef LV_FFMPEG_H
#define LV_FFMPEG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_FFMPEG != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register FFMPEG image decoder
 */
void lv_ffmpeg_init(void);

#endif /*LV_USE_FFMPEG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FFMPEG_H*/
