#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "../demos/lv_demos.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_XRGB8888);
}

/*Same algorithm as ARGB8888 with an other byte order, so it must match the ARGB8888 references*/
void test_render_to_abgr8888(void)
{
#if LV_DRAW_SW_SUPPORT_ABGR8888 == 0
    TEST_IGNORE_MESSAGE("LV_DRAW_SW_SUPPORT_ABGR8888 is disabled");
#else
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_ABGR8888);

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
            lv_snprintf(buf, sizeof(buf), "draw/render/argb8888/demo_render_%s_opa_%d.png",
                        lv_demo_render_get_scene_name(i), opa_values[opa]);
            TEST_ASSERT_EQUAL_SCREENSHOT(buf);
        }
    }
#endif
}

#endif
