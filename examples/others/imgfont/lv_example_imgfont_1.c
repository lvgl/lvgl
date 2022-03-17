#include "../../lv_examples.h"
#include <stdio.h>

#if LV_BUILD_EXAMPLES
#if LV_USE_IMGFONT

LV_IMG_DECLARE(emoji_F617)
static bool get_imgfont_path(const lv_font_t * font, void * img_src,
                             uint16_t len, uint32_t unicode, uint32_t unicode_next)
{
    LV_UNUSED(font);
    LV_UNUSED(unicode_next);
    LV_ASSERT_NULL(img_src);

    if(unicode == 0xF617) {
        memcpy(img_src, &emoji_F617, sizeof(lv_img_dsc_t));
    }
    else {
        char * path = (char *)img_src;
        snprintf(path, len, "%s/%04X.%s", "A:lvgl/examples/assets/emoji", unicode, "png");
        path[len - 1] = '\0';
    }

    return true;
}

/**
 * draw img in label or span obj
 */
void lv_example_imgfont_1(void)
{
    lv_font_t * imgfont = lv_imgfont_create(80, get_imgfont_path);
    if(imgfont == NULL) {
        LV_LOG_ERROR("imgfont init error");
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
