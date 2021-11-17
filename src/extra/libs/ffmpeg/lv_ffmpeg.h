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
struct ffmpeg_context_s;

extern const lv_obj_class_t lv_ffmpeg_player_class;

typedef struct {
    lv_img_t img;
    lv_timer_t * timer;
    lv_img_dsc_t imgdsc;
    bool auto_restart;
    struct ffmpeg_context_s * ffmpeg_ctx;
} lv_ffmpeg_player_t;

typedef enum {
    LV_FFMPEG_PLAYER_CMD_START,
    LV_FFMPEG_PLAYER_CMD_STOP,
    LV_FFMPEG_PLAYER_CMD_PAUSE,
    LV_FFMPEG_PLAYER_CMD_RESUME,
    _LV_FFMPEG_PLAYER_CMD_LAST
} lv_ffmpeg_player_cmd_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register FFMPEG image decoder
 */
void lv_ffmpeg_init(void);

/**
 * Get the number of frames contained in the file
 * @param path image or video file name
 * @return Number of frames, less than 0 means failed
 */
int lv_ffmpeg_get_frame_num(const char * path);

/**
 * Create ffmpeg_player object
 * @param parent pointer to an object, it will be the parent of the new player
 * @return pointer to the created ffmpeg_player
 */
lv_obj_t * lv_ffmpeg_player_create(lv_obj_t * parent);

/**
 * Set the path of the file to be played
 * @param ffmpeg_player pointer to an ffmpeg_player
 * @param path video file path
 * @return LV_RES_OK: no error; LV_RES_INV: can't get the info.
 */
lv_res_t lv_ffmpeg_player_set_src(lv_obj_t * ffmpeg_player, const char * path);

/**
 * Set command control video player
 * @param ffmpeg_player pointer to an ffmpeg_player
 * @param cmd control commands
 */
void lv_ffmpeg_player_set_cmd(lv_obj_t * ffmpeg_player, lv_ffmpeg_player_cmd_t cmd);

/**
 * Set the video to automatically replay
 * @param ffmpeg_player pointer to an ffmpeg_player
 * @param en true: enable the auto restart
 */
void lv_ffmpeg_player_set_auto_restart(lv_obj_t * ffmpeg_player, bool en);

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FFMPEG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FFMPEG*/
