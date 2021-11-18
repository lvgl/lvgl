#include "../../lv_examples.h"
#if LV_USE_FFMPEG && LV_BUILD_EXAMPLES

/**
 * Open a image or video from a file
 */
void lv_example_ffmpeg_1(void)
{
    lv_obj_t * par = lv_scr_act();
    lv_obj_set_flex_flow(par, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(par, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * img = lv_img_create(par);
    lv_img_set_src(img, "./lvgl/examples/libs/ffmpeg/ffmpeg.png");

    lv_obj_t * player = lv_ffmpeg_player_create(par);
    lv_ffmpeg_player_set_src(player, "./lvgl/examples/libs/ffmpeg/NeverGonnaGiveYouUp.mp4");
    lv_ffmpeg_player_set_auto_restart(player, true);
    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);
}

#endif
