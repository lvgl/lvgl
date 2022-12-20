#include "../../lv_examples.h"
#include <stdio.h>

#if LV_BUILD_EXAMPLES
#if LV_USE_IMGFONT

LV_IMG_DECLARE(emoji_F617)
static bool get_imgfont_path(const lv_font_t * font, void * img_src,
                             uint16_t len, uint32_t unicode, uint32_t unicode_next,
                             lv_coord_t * offset_y, void * user_data)
{
    LV_UNUSED(font);
    LV_UNUSED(unicode_next);
    LV_UNUSED(offset_y);
    LV_UNUSED(user_data);
    LV_ASSERT_NULL(img_src);

    if(unicode < 0xF000) return false;

    if(unicode == 0xF617) {
        memcpy(img_src, &emoji_F617, sizeof(lv_img_dsc_t));
    }
    else {
        char * path = (char *)img_src;
#if LV_USE_FFMPEG
        lv_snprintf(path, len, "%s/%04X.png", "lvgl/examples/assets/emoji", unicode);
#elif LV_USE_PNG
        lv_snprintf(path, len, "%s/%04X.png", "A:lvgl/examples/assets/emoji", unicode);
#endif
    }

    return true;
}

/**
 * draw img in label or span obj
 */
void lv_example_imgfont_1(void)
{
    lv_font_t * imgfont = lv_imgfont_create(80, get_imgfont_path, NULL);
    if(imgfont == NULL) {
        LV_LOG_ERROR("imgfont init error");
        return;
    }

    imgfont->fallback = LV_FONT_DEFAULT;

    lv_obj_t * label1 = lv_label_create(lv_scr_act());
    lv_label_set_text(label1, "12\uF600\uF617AB");
    lv_obj_set_style_text_font(label1, imgfont, LV_PART_MAIN);
    lv_obj_center(label1);
}
#else

void lv_example_imgfont_1(void)
{
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "imgfont is not installed");
    lv_obj_center(label);
}

#endif
#endif
