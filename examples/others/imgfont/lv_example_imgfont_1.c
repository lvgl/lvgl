#include "../../lv_examples.h"

#if LV_BUILD_EXAMPLES
#if LV_USE_IMGFONT

static const void * get_imgfont_path(const lv_font_t * font, uint32_t unicode, uint32_t unicode_next,
                                     int32_t * offset_y, void * user_data)
{
    LV_UNUSED(font);
    LV_UNUSED(unicode_next);
    LV_UNUSED(offset_y);
    LV_UNUSED(user_data);

    LV_IMAGE_DECLARE(emoji_F617);

    if(unicode < 0xF000) return NULL;

    if(unicode == 0xF617) {
        return &emoji_F617;
    }
    else if(unicode == 0xF600) {
#if LV_USE_FFMPEG
        return "lvgl/examples/assets/emoji/F600.png";
#else
        return "A:lvgl/examples/assets/emoji/F600.png";
#endif
    }

    return NULL;
}

/**
 * @title Image glyphs inside label text
 * @brief Replace private-use code points with PNG images rendered by `lv_imgfont`.
 *
 * `lv_imgfont_create` builds an 80 px image font whose path callback returns an
 * embedded `emoji_F617` image for `U+F617` and a file path for `U+F600` (prefixed
 * with `"A:"` unless `LV_USE_FFMPEG` is enabled). The font's `fallback` is set to
 * `LV_FONT_DEFAULT` so ASCII characters fall through. A centered label renders
 * `"12\uF600\uF617AB"`, mixing the two emoji images with normal text.
 */
void lv_example_imgfont_1(void)
{
    lv_font_t * imgfont = lv_imgfont_create(80, get_imgfont_path, NULL);
    if(imgfont == NULL) {
        LV_LOG_ERROR("imgfont init error");
        return;
    }

    imgfont->fallback = LV_FONT_DEFAULT;

    lv_obj_t * label1 = lv_label_create(lv_screen_active());
    lv_label_set_text(label1, "12\uF600\uF617AB");
    lv_obj_set_style_text_font(label1, imgfont, LV_PART_MAIN);
    lv_obj_center(label1);
}
#else

void lv_example_imgfont_1(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "imgfont is not installed");
    lv_obj_center(label);
}

#endif
#endif
