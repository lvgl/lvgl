#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_LOTTIE

/**
 * @title Lottie animation from file path
 * @brief Play a Lottie JSON animation loaded through the filesystem driver.
 *
 * An `lv_lottie` widget is created on the active screen and receives
 * its source through `lv_lottie_set_src_file`, pointing at
 * `lvgl/examples/widgets/lottie/lv_example_lottie_approve.json`, so a
 * filesystem driver for the working directory must be registered. A
 * 64x64 ARGB8888 premultiplied buffer is attached via
 * `lv_lottie_set_buffer` or `lv_lottie_set_draw_buf` depending on the
 * draw buffer alignment, then the widget is centered.
 */
void lv_example_lottie_2(void)
{

    lv_obj_t * lottie = lv_lottie_create(lv_screen_active());
    lv_lottie_set_src_file(lottie, "lvgl/examples/widgets/lottie/lv_example_lottie_approve.json");

#if LV_DRAW_BUF_ALIGN == 4 && LV_DRAW_BUF_STRIDE_ALIGN == 1
    /*If there are no special requirements, just declare a buffer
      x4 because the Lottie is rendered in ARGB8888_PREMULTIPLIED format*/
    static uint8_t buf[64 * 64 * 4];
    lv_lottie_set_buffer(lottie, 64, 64, buf);
#else
    /*For GPUs and special alignment/strid setting use a draw_buf instead*/
    LV_DRAW_BUF_DEFINE_STATIC(draw_buf, 64, 64, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED);
    lv_lottie_set_draw_buf(lottie, &draw_buf);
#endif

    lv_obj_center(lottie);
}

#else

void lv_example_lottie_2(void)
{
    /*fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Lottie cannot be previewed online");
    lv_obj_center(label);
}

#endif /*LV_USE_LOTTIE*/

#endif /*LV_BUILD_EXAMPLES*/
