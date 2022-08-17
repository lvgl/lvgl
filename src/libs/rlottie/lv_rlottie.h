/**
 * @file lv_rlottie.h
 *
 */

#ifndef LV_RLOTTIE_H
#define LV_RLOTTIE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_RLOTTIE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_RLOTTIE_CTRL_FORWARD  = 0,
    LV_RLOTTIE_CTRL_BACKWARD = 1,
    LV_RLOTTIE_CTRL_PAUSE    = 2,
    LV_RLOTTIE_CTRL_PLAY     = 0, /* Yes, play = 0 is the default mode */
    LV_RLOTTIE_CTRL_LOOP     = 8,
} lv_rlottie_ctrl_t;

/** definition in lottieanimation_capi.c */
struct Lottie_Animation_S;
typedef struct {
    lv_img_t img_ext;
    struct Lottie_Animation_S * animation;
    lv_timer_t * task;
    lv_img_dsc_t imgdsc;
    size_t total_frames;
    size_t current_frame;
    size_t framerate;
    uint32_t * allocated_buf;
    size_t allocated_buffer_size;
    size_t scanline_width;
    lv_rlottie_ctrl_t play_ctrl;
    size_t dest_frame;
    size_t start_frame;
} lv_rlottie_t;

extern const lv_obj_class_t lv_rlottie_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_rlottie_create_from_file(lv_obj_t * parent, lv_coord_t width, lv_coord_t height, const char * path);

lv_obj_t * lv_rlottie_create_from_raw(lv_obj_t * parent, lv_coord_t width, lv_coord_t height,
                                      const char * rlottie_desc);

void lv_rlottie_set_play_mode(lv_obj_t * rlottie, const lv_rlottie_ctrl_t ctrl);
void lv_rlottie_set_current_frame(lv_obj_t * rlottie, const size_t goto_frame);
void lv_rlottie_set_framerate(lv_obj_t * obj, const int framerate);
void lv_rlottie_set_render_width(lv_obj_t * obj, const int width);
void lv_rlottie_set_render_height(lv_obj_t * obj, const int height);
void lv_rlottie_set_dest_frame(lv_obj_t * obj, const int dest_frame);
void lv_rlottie_set_start_frame(lv_obj_t * obj, const int start_frame);

int lv_rlottie_get_framerate(lv_obj_t * obj);
int lv_rlottie_get_render_width(lv_obj_t * obj);
int lv_rlottie_get_render_height(lv_obj_t * obj);
size_t lv_rlottie_get_start_frame(lv_obj_t * obj);
size_t lv_rlottie_get_dest_frame(lv_obj_t * obj);
size_t lv_rlottie_get_current_frame(lv_obj_t * obj);
lv_rlottie_ctrl_t lv_rlottie_get_play_mode(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RLOTTIE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_RLOTTIE_H*/
