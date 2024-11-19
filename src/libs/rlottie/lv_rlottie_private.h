/**
 * @file lv_rlottie_private.h
 *
 */

#ifndef LV_RLOTTIE_PRIVATE_H
#define LV_RLOTTIE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_rlottie.h"
#if LV_USE_RLOTTIE
#include "../../widgets/image/lv_image_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** definition in lottieanimation_capi.c */
struct Lottie_Animation_S;

struct _lv_rlottie_t {
    lv_image_t img_ext;
    struct Lottie_Animation_S * animation;
    lv_timer_t * task;
    lv_image_dsc_t imgdsc;
    size_t total_frames;
    size_t current_frame;
    size_t framerate;
    uint32_t * allocated_buf;
    size_t allocated_buffer_size;
    size_t scanline_width;
    lv_rlottie_ctrl_t play_ctrl;
    size_t dest_frame;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RLOTTIE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_RLOTTIE_PRIVATE_H*/
