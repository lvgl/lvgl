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
    lv_span_t * span = lv_spangroup_add_span(NULL);

    TEST_ASSERT(NULL == span);
    TEST_ASSERT_EQUAL_INT(0, lv_spangroup_get_span_count(spangroup));
}

void test_spangroup_new_span_with_valid_parameter_returns_not_null_object(void)
{
    lv_span_t * span = lv_spangroup_add_span(spangroup);

    TEST_ASSERT(NULL != span);
    TEST_ASSERT_EQUAL_INT(1, lv_spangroup_get_span_count(spangroup));
}

void test_spangroup_delete_span_span_is_null(void)
{
    lv_span_t * span = lv_spangroup_add_span(spangroup);

    lv_spangroup_delete_span(spangroup, span);

    TEST_ASSERT_EQUAL_INT(0, lv_spangroup_get_span_count(spangroup));
}

void test_span_set_text(void)
{
    const char * test_text = "Test Text";
    lv_span_t * span = lv_spangroup_add_span(spangroup);

    lv_span_set_text(span, test_text);

    TEST_ASSERT_EQUAL_STRING(span->txt, test_text);
}

void test_span_set_text_with_bad_parameter_no_action_performed(void)
{
    const char * test_text = "Test Text";
    lv_span_t * span = lv_spangroup_add_span(spangroup);

    lv_span_set_text(span, test_text);
    lv_span_set_text(span, NULL);

    TEST_ASSERT_EQUAL_STRING(span->txt, test_text);
}

void test_span_set_text_with_previous_test_overwrites(void)
{
    const char * old_test_text = "Old Test Text";
    const char * new_test_text = "New Test Text and it is longer";
    lv_span_t * span = lv_spangroup_add_span(spangroup);

    lv_span_set_text(span, old_test_text);
    lv_span_set_text(span, new_test_text);

    TEST_ASSERT_EQUAL_STRING(span->txt, new_test_text);
}

void test_span_set_text_static(void)
{
    const char * test_text = "Test Text";
    lv_span_t * span = lv_spangroup_add_span(spangroup);

    lv_span_set_text_static(span, test_text);

    TEST_ASSERT_EQUAL_STRING(span->txt, test_text);
}

void test_span_set_text_static_with_bad_parameter_no_action_performed(void)
{
    const char * test_text = "Test Text";
    lv_span_t * span = lv_spangroup_add_span(spangroup);

    lv_span_set_text_static(span, test_text);
    lv_span_set_text_static(span, NULL);

    TEST_ASSERT_EQUAL_STRING(span->txt, test_text);
}

void test_span_set_text_static_with_previous_text_overwrites(void)
{
    const char * old_test_text = "Old Test Text";
    const char * new_test_text = "New Test Text and it is longer";
    lv_span_t * span = lv_spangroup_add_span(spangroup);
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

    (void)lv_spangroup_add_span(spangroup);
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
    lv_span_t * span_1 = lv_spangroup_add_span(spangroup);
    lv_span_set_text(span_1, "This text is over 100 pixels width");

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_01.png");

    lv_span_t * span_2 = lv_spangroup_add_span(spangroup);
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
    lv_span_t * span_1 = lv_spangroup_add_span(spangroup);
    lv_span_t * span_2 = lv_spangroup_add_span(spangroup);

    TEST_ASSERT_EQUAL_PTR(span_2, lv_spangroup_get_child(spangroup, span_2_idx));
    TEST_ASSERT_EQUAL_PTR(span_1, lv_spangroup_get_child(spangroup, span_1_idx));
}

void test_spangroup_get_child_count(void)
{
    (void)lv_spangroup_add_span(spangroup);
    (void)lv_spangroup_add_span(spangroup);

    const uint32_t cnt = lv_spangroup_get_span_count(spangroup);

    TEST_ASSERT_EQUAL(2, cnt);
}

void test_spangroup_get_expand_width(void)
{
    const uint32_t experimental_size = 232;
    const uint32_t constrained_size = 232;
    active_screen = lv_screen_active();

    spangroup = lv_spangroup_create(active_screen);
    lv_span_t * span = lv_spangroup_add_span(spangroup);
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

    lv_span_set_text(lv_spangroup_add_span(spangroup), "Lorem\n");
    lv_span_set_text(lv_spangroup_add_span(spangroup), "ipsum");

    lv_span_set_text(lv_spangroup_add_span(spangroup), "\n\n");

    lv_span_set_text(lv_spangroup_add_span(spangroup), "dolor");
    lv_span_set_text(lv_spangroup_add_span(spangroup), "");
    lv_span_set_text(lv_spangroup_add_span(spangroup), "\nsit");

    /* carriage return is treated as equivalent to line feed */
    lv_span_set_text(lv_spangroup_add_span(spangroup), "\r");

    lv_span_set_text(lv_spangroup_add_span(spangroup), "amet,\n consectetur");
    lv_span_set_text(lv_spangroup_add_span(spangroup), " adipiscing");

    lv_span_set_text(lv_spangroup_add_span(spangroup), "\n");
    lv_span_set_text(lv_spangroup_add_span(spangroup), "");

    lv_span_set_text(lv_spangroup_add_span(spangroup), "\relit, sed\n");
    lv_span_set_text(lv_spangroup_add_span(spangroup), "do eiusmod");

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

    lv_span_t * span1 = lv_spangroup_add_span(spangroup);
    lv_span_set_text(span1, "八百标兵奔北坡");
    lv_span_t * span2 = lv_spangroup_add_span(spangroup);
    lv_span_set_text(span2, "炮兵并排北边跑");
    lv_style_set_text_color(&span2->style, lv_palette_main(LV_PALETTE_RED));
    lv_span_t * span3 = lv_spangroup_add_span(spangroup);
    lv_span_set_text(span3, "中英文测试。The quick brown fox jumps over a lazy dog. ");
    lv_style_set_text_color(&span3->style, lv_palette_main(LV_PALETTE_BLUE));
    lv_span_t * span4 = lv_spangroup_add_span(spangroup);
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

    lv_span_set_text(lv_spangroup_add_span(spangroup), "A");
    lv_span_set_text(lv_spangroup_add_span(spangroup), "B");
    lv_span_set_text(lv_spangroup_add_span(spangroup), "C");
    lv_span_set_text(lv_spangroup_add_span(spangroup), "D");
    lv_span_set_text(lv_spangroup_add_span(spangroup), "E");
    lv_span_set_text(lv_spangroup_add_span(spangroup), "F");

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_08.png");
}

#if LV_FONT_MONTSERRAT_24 && LV_FONT_MONTSERRAT_20
void test_spangroup_get_span_coords(void)
{
    /* Initialize the active screen and create a new span group */
    active_screen = lv_screen_active();
    spangroup = lv_spangroup_create(active_screen);

    const uint32_t span_count = 5;
    lv_span_t * spans[span_count];

    /* Set styles and properties for the span group */
    lv_obj_set_style_outline_width(spangroup, 1, 0);
    lv_spangroup_set_indent(spangroup, 20);
    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_BREAK);
    lv_obj_set_width(spangroup, 300);
    lv_obj_set_style_pad_all(spangroup, 20, LV_PART_MAIN);

    /* Create spans and set their properties */
    spans[0] = lv_spangroup_add_span(spangroup);
    lv_span_set_text(spans[0], "China is a beautiful country.");
    lv_style_set_text_color(lv_span_get_style(spans[0]), lv_palette_main(LV_PALETTE_RED));
    lv_style_set_text_decor(lv_span_get_style(spans[0]), LV_TEXT_DECOR_UNDERLINE);
    lv_style_set_text_opa(lv_span_get_style(spans[0]), LV_OPA_50);

    spans[1] = lv_spangroup_add_span(spangroup);
    lv_span_set_text_static(spans[1], "good good study, day day up.");
    lv_style_set_text_font(lv_span_get_style(spans[1]), &lv_font_montserrat_24);
    lv_style_set_text_color(lv_span_get_style(spans[1]), lv_palette_main(LV_PALETTE_GREEN));

    spans[2] = lv_spangroup_add_span(spangroup);
    lv_span_set_text_static(spans[2], "LVGL is an open-source graphics library.");
    lv_style_set_text_color(lv_span_get_style(spans[2]), lv_palette_main(LV_PALETTE_BLUE));

    spans[3] = lv_spangroup_add_span(spangroup);
    lv_span_set_text_static(spans[3], "the boy no name.");
    lv_style_set_text_color(lv_span_get_style(spans[3]), lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_text_font(lv_span_get_style(spans[3]), &lv_font_montserrat_20);
    lv_style_set_text_decor(lv_span_get_style(spans[3]), LV_TEXT_DECOR_UNDERLINE);

    spans[4] = lv_spangroup_add_span(spangroup);
    lv_span_set_text(spans[4], "I have a dream that hope to come true.");
    lv_style_set_text_decor(lv_span_get_style(spans[4]), LV_TEXT_DECOR_STRIKETHROUGH);

    /* Refresh the span group mode and update layout */
    lv_spangroup_refresh(spangroup);
    lv_obj_update_layout(spangroup);

    /* Define expected coordinates for testing */
    const lv_span_coords_t test_coords[] = {
        {.heading = {.x1 = 40, .y1 = 20, .x2 = 280, .y2 = 20}, .middle = {.x1 = 40, .y1 = 20, .x2 = 241, .y2 = 36}, .trailing = {.x1 = 0, .y1 = 0, .x2 = 0, .y2 = 0}},
        {.heading = {.x1 = 241, .y1 = 20, .x2 = 280, .y2 = 36}, .middle = {.x1 = 20, .y1 = 36, .x2 = 280, .y2 = 63}, .trailing = {.x1 = 20, .y1 = 63, .x2 = 155, .y2 = 90}},
        {.heading = {.x1 = 155, .y1 = 63, .x2 = 280, .y2 = 90}, .middle = {.x1 = 20, .y1 = 90, .x2 = 280, .y2 = 90}, .trailing = {.x1 = 20, .y1 = 90, .x2 = 188, .y2 = 112}},
        {.heading = {.x1 = 188, .y1 = 90, .x2 = 280, .y2 = 112}, .middle = {.x1 = 20, .y1 = 112, .x2 = 280, .y2 = 112}, .trailing = {.x1 = 20, .y1 = 112, .x2 = 116, .y2 = 134}},
        {.heading = {.x1 = 116, .y1 = 112, .x2 = 280, .y2 = 134}, .middle = {.x1 = 20, .y1 = 134, .x2 = 280, .y2 = 134}, .trailing = {.x1 = 20, .y1 = 134, .x2 = 160, .y2 = 150}}
    };

    /* Define colors for visual testing */
    const lv_color_t colors[] = {
        lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_GREEN), lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_YELLOW), lv_palette_main(LV_PALETTE_PURPLE), lv_palette_main(LV_PALETTE_ORANGE),
        lv_palette_main(LV_PALETTE_INDIGO), lv_palette_main(LV_PALETTE_BROWN), lv_palette_main(LV_PALETTE_GREY),
        lv_palette_main(LV_PALETTE_PINK)
    };
    const uint32_t color_count = sizeof(colors) / sizeof(colors[0]);
    const lv_area_t area = spangroup->coords;

    /* Iterate through spans and validate coordinates */
    for(uint32_t i = 0; i < span_count; i++) {
        lv_span_coords_t coords = lv_spangroup_get_span_coords(spangroup, spans[i]);
        TEST_ASSERT_EQUAL_MEMORY(&coords.heading, &test_coords[i].heading, sizeof(lv_span_coords_t));

        /* Visual testing */
        const lv_color_t color = colors[i % color_count];

        /* Create and style heading object */
        lv_obj_t * obj_head = lv_obj_create(active_screen);
        lv_obj_remove_style_all(obj_head);
        lv_obj_set_pos(obj_head, coords.heading.x1 + area.x1, coords.heading.y1 + area.y1);
        lv_obj_set_size(obj_head, coords.heading.x2 - coords.heading.x1, coords.heading.y2 - coords.heading.y1);
        lv_obj_set_style_bg_color(obj_head, color, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(obj_head, LV_OPA_50, LV_PART_MAIN);

        /* Create and style middle object */
        lv_obj_t * obj_middle = lv_obj_create(active_screen);
        lv_obj_remove_style_all(obj_middle);
        lv_obj_set_pos(obj_middle, coords.middle.x1 + area.x1, coords.middle.y1 + area.y1);
        lv_obj_set_size(obj_middle, coords.middle.x2 - coords.middle.x1, coords.middle.y2 - coords.middle.y1);
        lv_obj_set_style_bg_color(obj_middle, color, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(obj_middle, LV_OPA_50, LV_PART_MAIN);

        /* Create and style trailing object */
        lv_obj_t * obj_trailing = lv_obj_create(active_screen);
        lv_obj_remove_style_all(obj_trailing);
        lv_obj_set_pos(obj_trailing, coords.trailing.x1 + area.x1, coords.trailing.y1 + area.y1);
        lv_obj_set_size(obj_trailing, coords.trailing.x2 - coords.trailing.x1, coords.trailing.y2 - coords.trailing.y1);
        lv_obj_set_style_bg_color(obj_trailing, color, LV_PART_MAIN);
        lv_obj_set_style_bg_opa(obj_trailing, LV_OPA_50, LV_PART_MAIN);
    }

    /* Validate the final screenshot */
    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_09.png");
}
#endif

void test_spangroup_set_right_align_on_overflow(void)
{
    active_screen = lv_screen_active();
    spangroup = lv_spangroup_create(active_screen);

    lv_obj_set_style_outline_width(spangroup, 1, 0);

    lv_obj_set_width(spangroup, 180);
    lv_obj_set_height(spangroup, 20);

    lv_spangroup_set_align(spangroup, LV_TEXT_ALIGN_RIGHT);

    lv_span_t * span = lv_spangroup_new_span(spangroup);
    lv_span_set_text_static(span, "China is a beautiful country.");
    lv_spangroup_set_overflow(spangroup, LV_SPAN_OVERFLOW_ELLIPSIS);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_10.png");
}

void test_spangroup_rtl_mode_set_default_align(void)
{
    const char * message = "مرحبا بكم في LVGL.";

    active_screen = lv_screen_active();
    spangroup = lv_spangroup_create(active_screen);

    lv_obj_set_style_outline_width(spangroup, 1, 0);
    lv_obj_set_style_text_font(spangroup, &lv_font_dejavu_16_persian_hebrew, 0);
    lv_obj_set_style_base_dir(spangroup, LV_BASE_DIR_RTL, 0);
    lv_obj_set_size(spangroup, 300, lv_font_dejavu_16_persian_hebrew.line_height);

    lv_span_t * span = lv_spangroup_new_span(spangroup);
    lv_span_set_text_static(span, message);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_11.png");
}

void test_spangroup_rtl_mode_set_left_align(void)
{
    const char * message = "مرحبا بكم في LVGL.";

    active_screen = lv_screen_active();
    spangroup = lv_spangroup_create(active_screen);

    lv_obj_set_style_outline_width(spangroup, 1, 0);
    lv_obj_set_style_text_font(spangroup, &lv_font_dejavu_16_persian_hebrew, 0);
    lv_obj_set_style_base_dir(spangroup, LV_BASE_DIR_RTL, 0);
    lv_obj_set_size(spangroup, 300, lv_font_dejavu_16_persian_hebrew.line_height);
    lv_spangroup_set_align(spangroup, LV_TEXT_ALIGN_LEFT);

    lv_span_t * span = lv_spangroup_new_span(spangroup);
    lv_span_set_text_static(span, message);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_12.png");
}

void test_spangroup_rtl_mode_set_center_align(void)
{
    const char * message = "مرحبا بكم في LVGL.";

    active_screen = lv_screen_active();
    spangroup = lv_spangroup_create(active_screen);

    lv_obj_set_style_outline_width(spangroup, 1, 0);
    lv_obj_set_style_text_font(spangroup, &lv_font_dejavu_16_persian_hebrew, 0);
    lv_obj_set_style_base_dir(spangroup, LV_BASE_DIR_RTL, 0);
    lv_obj_set_size(spangroup, 300, lv_font_dejavu_16_persian_hebrew.line_height);
    lv_spangroup_set_align(spangroup, LV_TEXT_ALIGN_CENTER);

    lv_span_t * span = lv_spangroup_new_span(spangroup);
    lv_span_set_text_static(span, message);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/span_13.png");
}

#endif
