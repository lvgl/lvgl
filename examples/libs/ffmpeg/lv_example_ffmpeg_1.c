#include "../../lv_examples.h"
#if LV_USE_FFMPEG && LV_BUILD_EXAMPLES

static lv_obj_t * ffmpeg_img_test(const char * path)
{
    lv_obj_t * img = lv_img_create(lv_scr_act());

    lv_img_set_src(img, path);
    return img;
}

static lv_obj_t * ffmpeg_video_test(const char * path)
{
    lv_obj_t * player = lv_ffmpeg_player_create(lv_scr_act());

    lv_ffmpeg_player_set_src(player, path);
    lv_ffmpeg_player_set_auto_restart(player, true);
    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);
    return player;
}

/**
 * Open a image or video from a file
 */
void lv_example_ffmpeg_1(const char * path)
{
    lv_obj_t * img;

    if(lv_ffmpeg_get_frame_num(path) > 1) {
      img = ffmpeg_video_test(path);
    }
    else {
      img = ffmpeg_img_test(path);
    }

    lv_obj_center(img);
}

#endif
