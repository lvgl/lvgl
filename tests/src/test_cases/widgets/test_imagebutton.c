#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

void test_imagebutton_basic_appearance(void)
{
    /**
     * from lv_example_imagebutton_1
    */

    LV_IMAGE_DECLARE(imagebutton_left);
    LV_IMAGE_DECLARE(imagebutton_right);
    LV_IMAGE_DECLARE(imagebutton_mid);

    /*Create an image button*/
    lv_obj_t * imagebutton1 = lv_imagebutton_create(lv_screen_active());
    lv_imagebutton_set_src(imagebutton1, LV_IMAGEBUTTON_STATE_RELEASED, &imagebutton_left, &imagebutton_mid,
                           &imagebutton_right);
    lv_obj_set_style_text_color(imagebutton1, lv_color_white(), 0);

    lv_obj_set_width(imagebutton1, 100);
    lv_obj_align(imagebutton1, LV_ALIGN_CENTER, -150, 0);

    /*Create a label on the image button*/
    lv_obj_t * label = lv_label_create(imagebutton1);
    lv_label_set_text(label, "Button");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -4);

    /**
     * from lv_demo_music
    */

    LV_IMAGE_DECLARE(music_button_play);

    lv_obj_t * play_button = lv_imagebutton_create(lv_screen_active());
    lv_imagebutton_set_src(play_button, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &music_button_play, NULL);
    lv_obj_align(play_button, LV_ALIGN_CENTER, 100, 0);

    /**
     * compare screenshot
    */

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/imagebutton_1.png");
}

#endif
