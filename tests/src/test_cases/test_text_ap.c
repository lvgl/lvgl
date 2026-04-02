#if LV_BUILD_TEST

#include <stdlib.h>
#include <string.h>
#include "../lvgl.h"
#include "unity/unity.h"
#include "../src/misc/lv_text_ap.h"

static char * output = NULL;

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
    free(output);
    output = NULL;
}
void test_ap_strlen_empty_string_returns_zero(void)
{
    /* An empty string should not crash and return 0 */
    TEST_ASSERT_EQUAL_SIZE_T(0, lv_text_ap_strlen(""));
}

void test_ap_strlen_ascii_only(void)
{
    /* Pure ASCII has no Arabic/Persian chars – result equals original byte count */
    const char * txt = "Hello World";
    size_t result = lv_text_ap_strlen(txt);
    TEST_ASSERT_EQUAL_SIZE_T(strlen(txt), result);
}

void test_ap_strlen_single_arabic_char(void)
{
    /* U+0622 ARABIC LETTER ALEF WITH MADDA ABOVE – LV_AP_ALPHABET_BASE_CODE */
    const char * txt = "\xD8\xA2"; /* UTF-8 for U+0622 */
    size_t result = lv_text_ap_strlen(txt);
    TEST_ASSERT_EQUAL_SIZE_T(3, result);
}

void test_ap_strlen_arabic_sentence(void)
{
    /* Simple Arabic sentence: "مرحبا" (Marhaba / Hello) */
    const char * txt = "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7";
    size_t result = lv_text_ap_strlen(txt);
    TEST_ASSERT_EQUAL_SIZE_T(15, result);
}

void test_ap_strlen_mixed_arabic_latin(void)
{
    /* Mixed text: "abc مرحبا xyz" */
    const char * txt = "abc \xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7 xyz";
    size_t result = lv_text_ap_strlen(txt);
    TEST_ASSERT_EQUAL_SIZE_T(23, result);
}

void test_ap_strlen_treats_invalid_utf8_as_raw_bytes(void)
{
    const char * txt = "\xFF\xFE"; /* Invalid UTF-8 */
    size_t result = lv_text_ap_strlen(txt);
    TEST_ASSERT_EQUAL(2, result);
}
void test_ap_proc_null_output(void)
{
    const char * input = "Hello";
    lv_text_ap_proc(input, NULL);
    /* Should get here without crashing*/
    TEST_PASS();
}

void test_ap_proc_empty_string(void)
{
    const char * input = "";
    char s_output[16] = {0};
    lv_text_ap_proc(input, s_output);
    TEST_ASSERT_EQUAL_STRING("", s_output);
}

void test_ap_proc_ascii_passthrough(void)
{
    const char * input = "Hello";
    char s_output[32] = {0};
    lv_text_ap_proc(input, s_output);
    TEST_ASSERT_EQUAL_STRING(input, s_output);
}

void test_ap_proc_output_not_null_for_arabic(void)
{
    /* "مرحبا" */
    const char * input = "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7";
    size_t out_bytes = lv_text_ap_strlen(input);

    output = (char *)calloc(out_bytes + 1, 1);
    TEST_ASSERT_NOT_NULL(output);

    lv_text_ap_proc(input, output);

    TEST_ASSERT_EQUAL_STRING("\xEF\xBB\xA3\xEF\xBA\xAE\xEF\xBA\xA3\xEF\xBA\x92\xEF\xBA\x8E", output);
}

void test_ap_proc_output_length_matches_calc(void)
{
    /* The byte count reported by calc must match what proc actually writes */
    const char * input = "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7";
    size_t expected_len = lv_text_ap_strlen(input);

    output = (char *)calloc(expected_len + 1, 1);
    TEST_ASSERT_NOT_NULL(output);

    lv_text_ap_proc(input, output);

    TEST_ASSERT_EQUAL_SIZE_T(expected_len, strlen(output));
}

void test_ap_proc_idempotent_ascii(void)
{
    /* Running proc twice on ASCII must yield the same result */
    const char * input = "Test 123";
    char out1[64] = {0};
    char out2[64] = {0};

    lv_text_ap_proc(input, out1);
    lv_text_ap_proc(input, out2);

    TEST_ASSERT_EQUAL_STRING(out1, out2);
}

void test_ap_proc_does_not_overflow_with_long_arabic(void)
{
    /* Stress: 10 repetitions of "مرحبا" */
    const char * word = "\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7";
    char input[256] = {0};
    for(int i = 0; i < 10; i++) strcat(input, word);

    size_t len = lv_text_ap_strlen(input);

    /* len + \0 + sentinel value*/
    output = (char *)calloc(len + 2, 1);
    TEST_ASSERT_NOT_NULL(output);

    output[len + 1] = 0x7E;

    lv_text_ap_proc(input, output); /* Must not crash / overwrite sentinel */
    TEST_ASSERT_NOT_EQUAL('\0', output[0]);
    TEST_ASSERT_EQUAL('\0', output[len]);
    TEST_ASSERT_EQUAL(0x7E, output[len + 1]);
}

#endif /*LV_BUILD_TEST*/
