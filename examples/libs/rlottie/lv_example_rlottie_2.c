#include "../../lv_examples.h"
#if LV_BUILD_EXAMPLES
#if LV_USE_RLOTTIE

/**
 * @title Rlottie animation from file
 * @brief Play a Lottie JSON file loaded directly through the rlottie stdio path.
 *
 * `lv_rlottie_create_from_file` opens
 * `lvgl/examples/libs/rlottie/lv_example_rlottie_approve.json` as a 100x100
 * widget centered on the active screen. The path has no LVGL drive letter
 * because rlottie reads the file through its own STDIO API rather than
 * through the LVGL filesystem.
 */
void lv_example_rlottie_2(void)
{
    /*The rlottie library uses STDIO file API, so there is no driver letter for LVGL*/
    lv_obj_t * lottie = lv_rlottie_create_from_file(lv_screen_active(), 100, 100,
                                                    "lvgl/examples/libs/rlottie/lv_example_rlottie_approve.json");
    lv_obj_center(lottie);
}

#else
void lv_example_rlottie_2(void)
{
    /*TODO
     *fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Rlottie is not installed");
    lv_obj_center(label);
}

#endif
#endif
