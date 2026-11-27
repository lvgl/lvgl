#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "../demos/lv_demos.h"

#include "unity/unity.h"

/*Scenes that render slightly differently in 32 and 64 bit builds have a reference for each*/
static inline const char * ref_img_ext(uint32_t scene)
{
    LV_UNUSED(scene);
    bool per_bitness = false;
#if LV_USE_VECTOR_GRAPHIC
    if(scene == LV_DEMO_RENDER_SCENE_VECTOR) per_bitness = true;
#endif
#if LV_USE_DRAW_VG_LITE
    if(scene == LV_DEMO_RENDER_SCENE_MASK_RECT) per_bitness = true;
    if(scene == LV_DEMO_RENDER_SCENE_DROP_SHADOW) per_bitness = true;
#endif
    if(!per_bitness) return ".png";

#ifndef NON_AMD64_BUILD
    return ".lp64.png";
#else
    return ".lp32.png";
#endif
}

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_XRGB8888);
}

void test_render_to_argb8888(void)
{
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_ARGB8888);

    lv_opa_t opa_values[2] = {0xff, 0x80};
    uint32_t opa;
    for(opa = 0; opa < 2; opa++) {
        uint32_t i;
        for(i = 0; i < LV_DEMO_RENDER_SCENE_NUM; i++) {

            /*Skip test with transformed indexed images if they are not loaded to RAM*/
            if(LV_BIN_DECODER_RAM_LOAD == 0 &&
               (i == LV_DEMO_RENDER_SCENE_IMAGE_NORMAL_3 ||
                i == LV_DEMO_RENDER_SCENE_IMAGE_RECOLOR_3)) continue;

            lv_demo_render(i, opa_values[opa]);

            char buf[128];
            lv_snprintf(buf, sizeof(buf), "draw/render/argb8888/demo_render_%s_opa_%d%s",
                        lv_demo_render_get_scene_name(i), opa_values[opa], ref_img_ext(i));
            TEST_ASSERT_EQUAL_SCREENSHOT(buf);
        }
    }
}

#endif
