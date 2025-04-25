#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_FFMPEG

#if LV_FFMPEG_PLAYER_USE_LV_FS
    #define PATH_PREFIX "A:"
#else
    #define PATH_PREFIX "./"
#endif

/**
 * Open a video from a file
 */
void lv_example_ffmpeg_2(void)
{
    /*birds.mp4 is downloaded from http://www.videezy.com (Free Stock Footage by Videezy!)
     *https://www.videezy.com/abstract/44864-silhouettes-of-birds-over-the-sunset*/
    /*It will use the LVGL filesystem abstraction (not the OS filesystem)
     *if `LV_FFMPEG_PLAYER_USE_LV_FS` is set.*/
    lv_obj_t * player = lv_ffmpeg_player_create(lv_screen_active());
    lv_ffmpeg_player_set_src(player, PATH_PREFIX "lvgl/examples/libs/ffmpeg/birds.mp4");
    lv_ffmpeg_player_set_auto_restart(player, true);
    lv_ffmpeg_player_set_cmd(player, LV_FFMPEG_PLAYER_CMD_START);
    lv_obj_center(player);
}

#else

void lv_example_ffmpeg_2(void)
{
    /*TODO
     *fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "FFmpeg is not installed");
    lv_obj_center(label);
}

#endif
#endif
