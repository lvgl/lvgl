#include "../../lv_examples.h"
#if LV_USE_SNAPSHOT && LV_BUILD_EXAMPLES

/*The extension selects the image encoder. PNG and JPEG need their encoder to be enabled,
 *any other extension is saved in LVGL's binary image format.*/
#if LV_USE_LIBPNG_ENCODER
    #define SCREENSHOT_PATH "A:screenshot.png"
#elif LV_USE_LIBJPEG_TURBO_ENCODER
    #define SCREENSHOT_PATH "A:screenshot.jpg"
#else
    #define SCREENSHOT_PATH "A:screenshot.bin"
#endif

static void screenshot_cb(lv_event_t * e)
{
    lv_obj_t * status = (lv_obj_t *) lv_event_get_user_data(e);

    /*Render the active screen into a new draw buffer*/
    lv_draw_buf_t * snapshot = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_ARGB8888);
    if(snapshot == NULL) {
        lv_label_set_text(status, "Snapshot failed");
        return;
    }

    /*Encode it. Use the default quality and compression level*/
    lv_image_encoder_args_t args;
    lv_image_encoder_args_init(&args);
    lv_result_t res = lv_image_encoder_save(snapshot, SCREENSHOT_PATH, &args);
    lv_draw_buf_destroy(snapshot);

    lv_label_set_text(status, res == LV_RESULT_OK ? "Saved " SCREENSHOT_PATH : "Saving failed");
}

/**
 * @title Save a screenshot to a file
 * @brief Take a snapshot of the screen and save it as a PNG or JPEG file.
 *
 * A few widgets and a button are created on the active screen. Pressing the button
 * renders the whole screen with `lv_snapshot_take` and writes it with
 * `lv_image_encoder_save` to `A:screenshot.png`, through the filesystem driver
 * registered under drive letter `A`. The file format comes from the extension, so
 * `.jpg` gives a JPEG file instead. PNG needs `LV_USE_LIBPNG_ENCODER` and JPEG needs
 * `LV_USE_LIBJPEG_TURBO_ENCODER`, without them the screenshot is saved in LVGL's binary
 * image format.
 */
void lv_example_snapshot_2(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * arc = lv_arc_create(scr);
    lv_arc_set_value(arc, 65);

    lv_obj_t * slider = lv_slider_create(scr);
    lv_slider_set_value(slider, 40, LV_ANIM_OFF);

    lv_obj_t * status = lv_label_create(scr);
    lv_label_set_text(status, "Press the button to take a screenshot");

    lv_obj_t * btn = lv_button_create(scr);
    lv_obj_add_event_cb(btn, screenshot_cb, LV_EVENT_CLICKED, status);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Screenshot");
}

#endif
