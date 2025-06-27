/* Performance test for the lv_text and lv_font_* functions */
#if LV_BUILD_TEST_PERF
#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * label = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
    label = lv_label_create(active_screen);
}

void test_label(void)
{
    TEST_ASSERT_MAX_TIME(lv_label_set_text, 0.5, label,
                         "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut auctor sed dui interdum convallis. Proin in ante magna. Pellentesque placerat condimentum erat ac laoreet. Cras mi eros, convallis vitae massa ac, blandit sodales urna. Proin tincidunt fermentum leo a volutpat. Donec ut blandit tortor. Duis elementum nibh nec consequat sagittis. Lutrae sunt praeclarae");

}
#endif
