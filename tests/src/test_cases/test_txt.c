#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include "../../../src/misc/lv_text_private.h"
#include <string.h>

void test_txt_should_insert_string_into_another(void)
{
    const char * msg = "Hello ";
    const char * suffix = "World";
    char target[20] = {0};
    size_t msg_len = strlen(msg);

    strcpy(target, msg);

    lv_text_ins(target, msg_len, suffix);

    TEST_ASSERT_EQUAL_STRING("Hello World", target);
}

void test_txt_should_handle_null_pointers_when_inserting(void)
{
    const char * msg = "Hello ";
    char target[20] = {0};
    size_t msg_len = strlen(msg);

    strcpy(target, msg);

    lv_text_ins(target, msg_len, NULL);

    TEST_ASSERT_EQUAL_STRING("Hello ", target);
}

void test_txt_cut_should_handle_null_pointer_to_txt(void)
{
    lv_text_cut(NULL, 0, 6);
}

void test_txt_cut_happy_path(void)
{
    char msg[] = "Hello World";

    lv_text_cut(msg, 0, 6);

    TEST_ASSERT_EQUAL_STRING("World", msg);
}

void test_txt_cut_should_handle_len_longer_than_string_length(void)
{
    char msg[] = "Hello World";

    lv_text_cut(msg, 0, 30);

    TEST_ASSERT_EQUAL_UINT8(msg[0], 0x00);
}

void test_txt_get_encoded_next_should_decode_valid_ascii(void)
{
    char msg[] = "Hello World!";
    uint32_t result = 0;

    result = lv_text_encoded_next(msg, NULL);

    TEST_ASSERT_EQUAL_UINT32((uint32_t) 'H', result);
}

void test_txt_get_encoded_next_detect_valid_2_byte_input(void)
{
    char msg[] = "\xc3\xb1";
    uint32_t result = 0;

    result = lv_text_encoded_next(msg, NULL);

    TEST_ASSERT_EQUAL_UINT32(241, result);
}

void test_txt_get_encoded_next_detect_invalid_2_byte_input(void)
{
    char msg[] = "\xc3\x28";
    uint32_t result = 0;

    result = lv_text_encoded_next(msg, NULL);

    TEST_ASSERT_EQUAL_UINT32(0, result);
}

void test_txt_get_encoded_next_detect_valid_3_byte_input(void)
{
    char msg[] = "\xe2\x82\xa1";
    uint32_t result = 0;

    result = lv_text_encoded_next(msg, NULL);

    TEST_ASSERT_EQUAL_UINT32(8353, result);
}

void test_txt_get_encoded_next_detect_invalid_3_byte_input(void)
{
    char msg[] = "\xe2\x28\xa1";
    uint32_t result = 0;

    result = lv_text_encoded_next(msg, NULL);

    TEST_ASSERT_EQUAL_UINT32(0, result);
}

void test_txt_get_encoded_next_detect_valid_4_byte_input(void)
{
    char msg[] = "\xf0\x90\x8c\xbc";
    uint32_t result = 0;

    result = lv_text_encoded_next(msg, NULL);

    TEST_ASSERT_EQUAL_UINT32(66364, result);
}

void test_txt_get_encoded_next_detect_invalid_4_byte_input(void)
{
    char msg[] = "\xf0\x28\x8c\x28";
    uint32_t result = 0;

    result = lv_text_encoded_next(msg, NULL);

    TEST_ASSERT_EQUAL_UINT32(0, result);
}

/* See #2615 for more information */
void test_txt_next_line_should_handle_empty_string(void)
{
    const lv_font_t * font_ptr = NULL;
    int32_t letter_space = 0;
    int32_t max_width = 0;
    lv_text_flag_t flag = LV_TEXT_FLAG_NONE;

    uint32_t next_line = lv_text_get_next_line("", 0, font_ptr, letter_space, max_width, NULL, flag);

    TEST_ASSERT_EQUAL_UINT32(0, next_line);
}

#endif
