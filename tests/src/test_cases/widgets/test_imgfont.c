#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

static const void * get_imgfont_path(const lv_font_t * font, uint32_t unicode, uint32_t unicode_next,
                                     int32_t * offset_y, void * user_data)
{
    LV_UNUSED(font);
    LV_UNUSED(unicode_next);
    LV_UNUSED(offset_y);
    LV_UNUSED(user_data);

    LV_IMAGE_DECLARE(emoji_F617);

    if(unicode == 0xF617) {
        return &emoji_F617;
    }
    else if(unicode == 0xF600) {
        return "A:src/test_assets/test_img_emoji_F600.png";
    }

    return NULL;
}

void test_imgfont_creation(void)
{
    lv_font_t * imgfont = lv_imgfont_create(80, get_imgfont_path, NULL);
    TEST_ASSERT_NOT_NULL(imgfont);

    imgfont->fallback = LV_FONT_DEFAULT;

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "12\uF600\uF617\uF000AB");
    lv_obj_set_style_text_font(label, imgfont, LV_PART_MAIN);
    lv_obj_center(label);

    lv_refr_now(NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/imgfont_1.png");

    lv_imgfont_destroy(imgfont);
}

#endif
