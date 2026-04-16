#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_FFMPEG && LV_FFMPEG_PLAYER_USE_LV_FS

/**
 * @title Decode image with FFmpeg
 * @brief Open a PNG through the FFmpeg integration and center it on the screen.
 *
 * An image widget is created on the active screen and its source is set to
 * `A:lvgl/examples/libs/ffmpeg/ffmpeg.png`. The FFmpeg image path always
 * routes through the LVGL filesystem abstraction regardless of
 * `LV_FFMPEG_PLAYER_USE_LV_FS`, so the file is read via the driver registered
 * under drive letter `A`.
 */
void lv_example_ffmpeg_1(void)
{
    /*It always uses the LVGL filesystem abstraction (not the OS filesystem)
     *to open the image, unlike `lv_ffmpeg_player_set_src` which depends on
     *the setting of `LV_FFMPEG_PLAYER_USE_LV_FS`.*/
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "A:lvgl/examples/libs/ffmpeg/ffmpeg.png");
    lv_obj_center(img);
}

#else

void lv_example_ffmpeg_1(void)
{
    /*TODO
     *fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "FFmpeg is not installed");
    lv_obj_center(label);
}

#endif
#endif
