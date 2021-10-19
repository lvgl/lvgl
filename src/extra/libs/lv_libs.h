/**
 * @file lv_libs.h
 *
 */

#ifndef LV_LIBS_H
#define LV_LIBS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "bmp/lv_bmp.h"
#include "fsdrv/lv_fsdrv.h"
#include "png/lv_png.h"
#include "gif/lv_gif.h"
#include "qrcode/lv_qrcode.h"
#include "sjpg/lv_sjpg.h"
#include "freetype/lv_freetype.h"
#include "rlottie/lv_rlottie.h"

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
#if LV_USE_LOG && LV_LOG_TRACE_LAYOUT
#  define LV_TRACE_LAYOUT(...) LV_LOG_TRACE( __VA_ARGS__)
#else
#  define LV_TRACE_LAYOUT(...)
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LAYOUTS_H*/
