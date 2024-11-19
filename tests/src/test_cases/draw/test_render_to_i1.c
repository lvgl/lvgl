#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../demos/lv_demos.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_display_set_color_format(NULL, LV_COLOR_FORMAT_XRGB8888);
}

void test_render_to_i1(void)
{
#if LV_BIN_DECODER_RAM_LOAD && LV_USE_DRAW_VG_LITE == 0
    lv_display_set_color_format(NULL, LV_COLOR_FORMAT_I1);

    lv_opa_t opa_values[2] = {0xff, 0xc0};
    uint32_t opa;
    for(opa = 0; opa < 2; opa++) {
        uint32_t i;
        for(i = 0; i < LV_DEMO_RENDER_SCENE_NUM; i++) {
            lv_demo_render(i, opa_values[opa]);

            char buf[128];
            lv_snprintf(buf, sizeof(buf), "draw/render/i1/demo_render_%s_opa_%d.png",
                        lv_demo_render_get_scene_name(i), opa_values[opa]);
            TEST_ASSERT_EQUAL_SCREENSHOT(buf);
        }
    }
#else
    /*Without LV_BIN_DECODER_RAM_LOAD can't test rotated images*/
    TEST_PASS();
#endif
}

#endif
