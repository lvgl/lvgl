#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static lv_obj_t * spangroup = NULL;
static lv_obj_t * active_screen = NULL;

void setUp(void)
{
    spangroup = lv_spangroup_create(NULL);
}

void tearDown(void)
{
    lv_obj_delete(spangroup);

    if(active_screen) {
        lv_obj_clean(active_screen);
    }

    spangroup = NULL;
    active_screen = NULL;
}

void test_spangroup_create_returns_not_null_object(void)
{
    lv_obj_t * obj = lv_spangroup_create(NULL);

    TEST_ASSERT(NULL != obj);
}

void test_spangroup_new_span_with_null_parameter_returns_null_object(void)
{
    lv_span_t * span = lv_spangroup_new_span(NULL);

    TEST_ASSERT(NULL == span);
    TEST_ASSERT_EQUAL_INT(0, lv_spangroup_get_span_count(spangroup));
}

void test_spangroup_new_span_with_valid_parameter_returns_not_null_object(void)
{
    lv_span_t * span = lv_spangroup_new_span(spangroup);

    TEST_ASSERT(NULL != span);
    TEST_ASSERT_EQUAL_INT(1, lv_spangroup_get_span_count(spangroup));
}

void test_spangroup_delete_span_span_is_null(void)
{
    lv_span_t * span = lv_spangroup_new_span(spangroup);

    lv_spangroup_delete_span(spangroup, span);

    TEST_ASSERT_EQUAL_INT(0, lv_spangroup_get_span_count(spangroup));
}

void test_span_set_text(void)
{
    const char * test_text = "Test Text";
    lv_span_t * span = lv_spangroup_new_span(spangroup);

    lv_span_set_text(span, test_text);

    TEST_ASSERT_EQUAL_STRING(span->txt, test_text);
}

void test_span_set_text_with_bad_parameter_no_action_performed(void)
{
    const char * test_text = "Test Text";
    lv_span_t * span = lv_spangroup_new_span(spangroup);

    lv_span_set_text(span, test_text);
    lv_span_set_text(span, NULL);

    TEST_ASSERT_EQUAL_STRING(span->txt, test_text);
}

void test_span_set_text_with_previous_test_overwrites(void)
{
    const char * old_test_text = "Old Test Text";
    const char * new_test_text = "New Test Text and it is longer";
    lv_span_t * span = lv_spangroup_new_span(spangroup);

    lv_span_set_text(span, old_test_text);
    lv_span_set_text(span, new_test_text);

    TEST_ASSERT_EQUAL_STRING(span->txt, new_test_text);
}

void test_span_set_text_static(void)
{
    const char * test_text = "Test Text";
    lv_span_t * span = lv_spangroup_new_span(spangroup);

    lv_span_set_text_static(span, test_text);

    TEST_ASSERT_EQUAL_STRING(span->txt, test_text);
}

void test_span_set_text_static_with_bad_parameter_no_action_performed(void)
{
    const char * test_text = "Test Text";
    lv_span_t * span = lv_spangroup_new_span(spangroup);

    lv_span_set_text_static(span, test_text);
    lv_span_set_text_static(span, NULL);

    TEST_ASSERT_EQUAL_STRING(span->txt, test_text);
}

void test_span_set_text_static_with_previous_text_overwrites(void)
{
    const char * old_test_text = "Old Test Text";
    const char * new_test_text = "New Test Text and it is longer";
    lv_span_t * span = lv_spangroup_new_span(spangroup);
    lv_span_set_text_static(span, old_test_text);
    lv_span_set_text_static(span, new_test_text);

    TEST_ASSERT_EQUAL_STRING(span->txt, new_test_text);
}

void test_spangroup_set_align(void)
{
    const lv_text_align_t align = LV_TEXT_ALIGN_CENTER;
    lv_spangroup_set_align(spangroup, align);

    TEST_ASSERT_EQUAL(align, lv_spangroup_get_align(spangroup));
}

void test_spangroup_set_overflow(void)
{
    const lv_span_overflow_t overflow = LV_SPAN_OVERFLOW_ELLIPSIS;
    lv_spangroup_set_overflow(spangroup, overflow);

    TEST_ASSERT_EQUAL(overflow, lv_spangroup_get_overflow(spangroup));
}

void test_spangroup_set_overflow_invalid_parameters_overflow_not_changed(void)
{
    lv_spangroup_set_overflow(spangroup, LV_SPAN_OVERFLOW_ELLIPSIS);
    lv_spangroup_set_overflow(spangroup, LV_SPAN_OVERFLOW_LAST);

    TEST_ASSERT_EQUAL(LV_SPAN_OVERFLOW_ELLIPSIS, lv_spangroup_get_overflow(spangroup));
}

void test_spangroup_set_indent(void)
{
    const int32_t indent = 100;

    lv_spangroup_set_indent(spangroup, indent);

    TEST_ASSERT_EQUAL(indent, lv_spangroup_get_indent(spangroup));
}

void test_spangroup_set_mode(void)
{
    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_EXPAND);
    TEST_ASSERT_EQUAL(LV_SPAN_MODE_EXPAND, lv_spangroup_get_mode(spangroup));

    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_BREAK);
    TEST_ASSERT_EQUAL(LV_SPAN_MODE_BREAK, lv_spangroup_get_mode(spangroup));

    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_FIXED);
    TEST_ASSERT_EQUAL(LV_SPAN_MODE_FIXED, lv_spangroup_get_mode(spangroup));
}

void test_spangroup_set_mode_invalid_parameter_mode_not_changed(void)
{
    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_EXPAND);
    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_LAST);
    TEST_ASSERT_EQUAL(LV_SPAN_MODE_EXPAND, lv_spangroup_get_mode(spangroup));
}

void test_spangroup_set_max_lines(void)
{
    const int32_t lines = 100;

    lv_spangroup_set_max_lines(spangroup, lines);

    TEST_ASSERT_EQUAL_INT32(lines, lv_spangroup_get_max_lines(spangroup));
}

void test_spangroup_get_max_lines_for_new_span_group(void)
{
    int32_t lines;

    lines = lv_spangroup_get_max_lines(spangroup);

    TEST_ASSERT_EQUAL_INT32(-1, lines);

    lines = 3;
    lv_spangroup_set_max_lines(spangroup, lines);
    lines = lv_spangroup_get_max_lines(spangroup);

    TEST_ASSERT_EQUAL_INT32(3, lines);
}

void test_spangroup_get_max_line_h(void)
{
    int32_t max_line;

    max_line = lv_spangroup_get_max_line_height(spangroup);

    TEST_ASSERT_EQUAL(0, (uint32_t)max_line);

    (void)lv_spangroup_new_span(spangroup);
    max_line = lv_spangroup_get_max_line_height(spangroup);

    // TODO: find out why this magic numberdd
    TEST_ASSERT_EQUAL(16, (uint32_t)max_line);
}

void test_spangroup_draw(void)
{
    active_screen = lv_screen_active();
    spangroup = lv_spangroup_create(active_screen);
    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_BREAK);
    lv_obj_set_width(spangroup, 100);
    lv_span_t * span_1 = lv_spangroup_new_span(spangroup);
    lv_span_set_text(span_1, "This text is over 100 pixels width");

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_01.png");

    lv_span_t * span_2 = lv_spangroup_new_span(spangroup);
    lv_span_set_text(span_2, "This text is also over 100 pixels width");
    lv_style_set_text_decor(&span_2->style, LV_TEXT_DECOR_STRIKETHROUGH);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_02.png");

    lv_spangroup_set_align(spangroup, LV_TEXT_ALIGN_CENTER);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_03.png");

    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_EXPAND);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_04.png");

    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_FIXED);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_05.png");
}

void test_spangroup_get_child(void)
{
    const int32_t span_1_idx = 0;
    const int32_t span_2_idx = 1;
    lv_span_t * span_1 = lv_spangroup_new_span(spangroup);
    lv_span_t * span_2 = lv_spangroup_new_span(spangroup);

    TEST_ASSERT_EQUAL_PTR(span_2, lv_spangroup_get_child(spangroup, span_2_idx));
    TEST_ASSERT_EQUAL_PTR(span_1, lv_spangroup_get_child(spangroup, span_1_idx));
}

void test_spangroup_get_child_count(void)
{
    (void)lv_spangroup_new_span(spangroup);
    (void)lv_spangroup_new_span(spangroup);

    const uint32_t cnt = lv_spangroup_get_span_count(spangroup);

    TEST_ASSERT_EQUAL(2, cnt);
}

void test_spangroup_get_expand_width(void)
{
    const uint32_t experimental_size = 232;
    const uint32_t constrained_size = 232;
    active_screen = lv_screen_active();

    spangroup = lv_spangroup_create(active_screen);
    lv_span_t * span = lv_spangroup_new_span(spangroup);
    lv_span_set_text(span, "This text is over 100 pixels width");

    TEST_ASSERT_EQUAL_INT(experimental_size,
                          lv_spangroup_get_expand_width(spangroup, UINT32_MAX));

    TEST_ASSERT_EQUAL_INT(constrained_size,
                          lv_spangroup_get_expand_width(spangroup, experimental_size));
}

void test_spangroup_newlines(void)
{
    active_screen = lv_screen_active();
    spangroup = lv_spangroup_create(active_screen);
    lv_obj_set_size(spangroup, LV_PCT(100), LV_PCT(100));

    lv_span_set_text(lv_spangroup_new_span(spangroup), "Lorem\n");
    lv_span_set_text(lv_spangroup_new_span(spangroup), "ipsum");

    lv_span_set_text(lv_spangroup_new_span(spangroup), "\n\n");

    lv_span_set_text(lv_spangroup_new_span(spangroup), "dolor");
    lv_span_set_text(lv_spangroup_new_span(spangroup), "");
    lv_span_set_text(lv_spangroup_new_span(spangroup), "\nsit");

    /* carriage return is treated as equivalent to line feed */
    lv_span_set_text(lv_spangroup_new_span(spangroup), "\r");

    lv_span_set_text(lv_spangroup_new_span(spangroup), "amet,\n consectetur");
    lv_span_set_text(lv_spangroup_new_span(spangroup), " adipiscing");

    lv_span_set_text(lv_spangroup_new_span(spangroup), "\n");
    lv_span_set_text(lv_spangroup_new_span(spangroup), "");

    lv_span_set_text(lv_spangroup_new_span(spangroup), "\relit, sed\n");
    lv_span_set_text(lv_spangroup_new_span(spangroup), "do eiusmod");

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_06.png");
}

#if LV_USE_FREETYPE && __WORDSIZE == 64

void test_spangroup_chinese_break_line(void)
{
    lv_font_t * font = lv_freetype_font_create("src/test_files/fonts/noto/NotoSansSC-Regular.ttf",
                                               LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 24, LV_FREETYPE_FONT_STYLE_NORMAL);

    if(!font) {
        LV_LOG_ERROR("freetype font create failed.");
        TEST_FAIL();
    }

    active_screen = lv_screen_active();
    spangroup = lv_spangroup_create(active_screen);
    lv_obj_set_size(spangroup, LV_PCT(100), LV_PCT(100));
    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_BREAK);

    lv_obj_set_style_text_font(spangroup, font, 0);
    lv_obj_set_style_border_width(spangroup, 2, 0);
    lv_obj_set_width(spangroup, 250);

    lv_span_t * span1 = lv_spangroup_new_span(spangroup);
    lv_span_set_text(span1, "八百标兵奔北坡");
    lv_span_t * span2 = lv_spangroup_new_span(spangroup);
    lv_span_set_text(span2, "炮兵并排北边跑");
    lv_style_set_text_color(&span2->style, lv_palette_main(LV_PALETTE_RED));
    lv_span_t * span3 = lv_spangroup_new_span(spangroup);
    lv_span_set_text(span3, "中英文测试。The quick brown fox jumps over a lazy dog. ");
    lv_style_set_text_color(&span3->style, lv_palette_main(LV_PALETTE_BLUE));
    lv_span_t * span4 = lv_spangroup_new_span(spangroup);
    lv_span_set_text(span4, "abcdefghijklmn中英文测试");
    lv_style_set_text_color(&span4->style, lv_palette_main(LV_PALETTE_GREEN));

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_07.png");

    lv_obj_set_style_text_font(spangroup, LV_FONT_DEFAULT, 0);
    lv_freetype_font_delete(font);
}

#else

void test_spangroup_chinese_break_line(void)
{
}

#endif

void test_spangroup_style_text_letter_space(void)
{
    active_screen = lv_screen_active();
    spangroup = lv_spangroup_create(active_screen);

    lv_obj_set_style_outline_width(spangroup, 1, 0);
    lv_obj_set_style_text_letter_space(spangroup, 20, 0);

    lv_span_set_text(lv_spangroup_new_span(spangroup), "A");
    lv_span_set_text(lv_spangroup_new_span(spangroup), "B");
    lv_span_set_text(lv_spangroup_new_span(spangroup), "C");
    lv_span_set_text(lv_spangroup_new_span(spangroup), "D");
    lv_span_set_text(lv_spangroup_new_span(spangroup), "E");
    lv_span_set_text(lv_spangroup_new_span(spangroup), "F");

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_08.png");
}

#endif
