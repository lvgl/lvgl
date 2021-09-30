#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void test_txt_next_line_should_handle_empty_string(void);


/* See #2615 for more information */
void test_txt_next_line_should_handle_empty_string(void)
{
    const lv_font_t *font_ptr = NULL;
    lv_coord_t letter_space = 0;
    lv_coord_t max_width = 0;
    lv_text_flag_t flag = LV_TEXT_FLAG_NONE;

    uint32_t next_line = _lv_txt_get_next_line("", font_ptr, letter_space, max_width, flag);

    TEST_ASSERT_EQUAL_UINT32(0, next_line);
}

#endif
