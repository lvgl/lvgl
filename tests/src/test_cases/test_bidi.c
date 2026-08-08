#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"
#include "../../src/misc/lv_bidi_private.h"
#include <string.h>

#if LV_USE_BIDI

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

/*
 * Helper: call lv_bidi_process_paragraph with a guarded output buffer.
 * Allocates (len + 1 + 2*guard) bytes, fills guard zones with 0xAA,
 * runs the function, then checks guard zones are intact.
 * Returns true if no overflow detected.
 */
#define GUARD_SIZE 16
#define GUARD_BYTE 0xAA

static bool bidi_process_check_overflow(const char * str_in, uint32_t len, lv_base_dir_t base_dir)
{
    /* Allocate buffer with guard zones before and after */
    uint32_t total = GUARD_SIZE + len + 1 + GUARD_SIZE;
    uint8_t * buf = lv_malloc(total);
    TEST_ASSERT_NOT_NULL(buf);

    /* Fill entire buffer with guard byte */
    memset(buf, GUARD_BYTE, total);

    /* str_out points to the usable region */
    char * str_out = (char *)(buf + GUARD_SIZE);

    /* Clear the usable region */
    memset(str_out, 0, len + 1);

    lv_bidi_process_paragraph(str_in, str_out, len, base_dir, NULL, 0);

    /* Check pre-guard */
    bool ok = true;
    for(uint32_t i = 0; i < GUARD_SIZE; i++) {
        if(buf[i] != GUARD_BYTE) {
            ok = false;
            break;
        }
    }

    /* Check post-guard */
    for(uint32_t i = 0; i < GUARD_SIZE; i++) {
        if(buf[GUARD_SIZE + len + 1 + i] != GUARD_BYTE) {
            ok = false;
            break;
        }
    }

    lv_free(buf);
    return ok;
}

/*
 * Test 1: Pure Arabic text with RTL base direction.
 * Each Arabic char is 2 bytes. Odd len cuts a char in half.
 * Before fix: rtl_reverse writes past buffer end (right overflow).
 * Covers fix #1 and #2 in rtl_reverse().
 */
void test_bidi_rtl_arabic_odd_len_no_overflow(void)
{
    /* Arabic: U+0639 U+0631 U+0628 U+064A = "\xd8\xb9\xd8\xb1\xd8\xa8\xd9\x8a" (8 bytes) */
    const char arabic[] = "\xd8\xb9\xd8\xb1\xd8\xa8\xd9\x8a";

    /* len=7: cuts the last 2-byte char in half */
    TEST_ASSERT_TRUE(bidi_process_check_overflow(arabic, 7, LV_BASE_DIR_RTL));

    /* len=5: cuts in the middle */
    TEST_ASSERT_TRUE(bidi_process_check_overflow(arabic, 5, LV_BASE_DIR_RTL));

    /* len=3 */
    TEST_ASSERT_TRUE(bidi_process_check_overflow(arabic, 3, LV_BASE_DIR_RTL));

    /* len=1: minimal case */
    TEST_ASSERT_TRUE(bidi_process_check_overflow(arabic, 1, LV_BASE_DIR_RTL));
}

/*
 * Test 2: Mixed ASCII + Arabic with RTL base direction.
 * Neutral prefix (space) consumes 1 byte, leaving odd remainder for Arabic.
 * Before fix: lv_bidi_process_paragraph rd > len after neutral prefix,
 * causing wr underflow (left overflow).
 * Covers fix #3 (rd clamp) and fix #4 (run_len clamp).
 */
void test_bidi_rtl_mixed_neutral_prefix_no_overflow(void)
{
    /* Space + Arabic chars */
    const char mixed[] = " \xd8\xb9\xd8\xb1\xd8\xa8";

    /* len=6: space(1) + 2.5 Arabic chars, cuts mid-char */
    TEST_ASSERT_TRUE(bidi_process_check_overflow(mixed, 6, LV_BASE_DIR_RTL));

    /* len=4 */
    TEST_ASSERT_TRUE(bidi_process_check_overflow(mixed, 4, LV_BASE_DIR_RTL));

    /* len=2 */
    TEST_ASSERT_TRUE(bidi_process_check_overflow(mixed, 2, LV_BASE_DIR_RTL));
}

/*
 * Test 3: Arabic text with LTR base direction.
 * RTL run embedded in LTR context still calls rtl_reverse.
 * Covers fix #1 and #2 in rtl_reverse() for LTR base_dir path.
 */
void test_bidi_ltr_arabic_odd_len_no_overflow(void)
{
    const char arabic[] = "\xd8\xb9\xd8\xb1\xd8\xa8\xd9\x8a";

    TEST_ASSERT_TRUE(bidi_process_check_overflow(arabic, 7, LV_BASE_DIR_LTR));
    TEST_ASSERT_TRUE(bidi_process_check_overflow(arabic, 5, LV_BASE_DIR_LTR));
    TEST_ASSERT_TRUE(bidi_process_check_overflow(arabic, 3, LV_BASE_DIR_LTR));
}

/*
 * Test 4: get_next_run returns run_len > remaining length.
 * Multiple Arabic words separated by spaces create multiple runs.
 * Before fix: get_next_run could return len > max_len.
 * Covers fix #5, #6, #7 in get_next_run().
 */
void test_bidi_rtl_multi_run_boundary_no_overflow(void)
{
    /* Arabic word + space + Arabic word */
    const char multi[] = "\xd8\xb9\xd8\xb1 \xd8\xa8\xd9\x8a";

    /* Sweep through all len values to catch any boundary issue */
    uint32_t full_len = (uint32_t)strlen(multi);
    for(uint32_t len = 1; len <= full_len; len++) {
        TEST_ASSERT_TRUE(bidi_process_check_overflow(multi, len, LV_BASE_DIR_RTL));
    }
}

/*
 * Test 5: Hebrew text (also 2-byte UTF-8) with RTL.
 * Hebrew: U+05E9 U+05DC U+05D5 U+05DD = "\xd7\xa9\xd7\x9c\xd7\x95\xd7\x9d"
 */
void test_bidi_rtl_hebrew_odd_len_no_overflow(void)
{
    const char hebrew[] = "\xd7\xa9\xd7\x9c\xd7\x95\xd7\x9d";

    TEST_ASSERT_TRUE(bidi_process_check_overflow(hebrew, 7, LV_BASE_DIR_RTL));
    TEST_ASSERT_TRUE(bidi_process_check_overflow(hebrew, 5, LV_BASE_DIR_RTL));
    TEST_ASSERT_TRUE(bidi_process_check_overflow(hebrew, 3, LV_BASE_DIR_RTL));
    TEST_ASSERT_TRUE(bidi_process_check_overflow(hebrew, 1, LV_BASE_DIR_RTL));
}

/*
 * Test 6: 3-byte UTF-8 characters (Chinese) with RTL base direction.
 * Chinese chars are weak/neutral in bidi, 3 bytes each.
 * len not on 3-byte boundary triggers the same overflow.
 */
void test_bidi_rtl_chinese_non_aligned_len_no_overflow(void)
{
    /* Chinese: U+4F60 U+597D = "\xe4\xbd\xa0\xe5\xa5\xbd" (6 bytes) */
    const char chinese[] = "\xe4\xbd\xa0\xe5\xa5\xbd";

    TEST_ASSERT_TRUE(bidi_process_check_overflow(chinese, 5, LV_BASE_DIR_RTL));
    TEST_ASSERT_TRUE(bidi_process_check_overflow(chinese, 4, LV_BASE_DIR_RTL));
    TEST_ASSERT_TRUE(bidi_process_check_overflow(chinese, 2, LV_BASE_DIR_RTL));
    TEST_ASSERT_TRUE(bidi_process_check_overflow(chinese, 1, LV_BASE_DIR_RTL));
}

/*
 * Test 7: Digits (weak chars) mixed with Arabic (strong RTL).
 * Weak chars take the "weak" path in rtl_reverse.
 * Covers fix #2 (weak copy len clamp).
 */
void test_bidi_rtl_digits_arabic_no_overflow(void)
{
    /* "123" + Arabic */
    const char mixed[] = "123\xd8\xb9\xd8\xb1";

    TEST_ASSERT_TRUE(bidi_process_check_overflow(mixed, 6, LV_BASE_DIR_RTL));
    TEST_ASSERT_TRUE(bidi_process_check_overflow(mixed, 5, LV_BASE_DIR_RTL));
    TEST_ASSERT_TRUE(bidi_process_check_overflow(mixed, 4, LV_BASE_DIR_RTL));
}

/*
 * Test 8: Exact character boundary len should produce valid output.
 * No truncation needed — verify the output is correct.
 */
void test_bidi_rtl_exact_boundary_correct_output(void)
{
    /* Two Arabic chars: U+0639 U+0631 = "\xd8\xb9\xd8\xb1" (4 bytes) */
    const char arabic[] = "\xd8\xb9\xd8\xb1";
    char out[8] = {0};

    lv_bidi_process_paragraph(arabic, out, 4, LV_BASE_DIR_RTL, NULL, 0);

    /* RTL reversal: chars should be swapped */
    /* U+0631 first, then U+0639 */
    TEST_ASSERT_EQUAL_UINT8(0xd8, (uint8_t)out[0]);
    TEST_ASSERT_EQUAL_UINT8(0xb1, (uint8_t)out[1]);
    TEST_ASSERT_EQUAL_UINT8(0xd8, (uint8_t)out[2]);
    TEST_ASSERT_EQUAL_UINT8(0xb9, (uint8_t)out[3]);
    TEST_ASSERT_EQUAL_UINT8(0x00, (uint8_t)out[4]);
}

/*
 * Test 9: Brute-force sweep — the pattern from the crash demo.
 * Iterate all len values [1..full_len] for a realistic Arabic string.
 * This is the pattern that triggered the original crash on the device.
 */
void test_bidi_rtl_bruteforce_sweep_no_overflow(void)
{
    /* Longer Arabic string simulating real HRV page text */
    const char text[] = "\xd8\xb9\xd8\xb1\xd8\xa8\xd9\x8a "
                        "\xd8\xaa\xd8\xac\xd8\xb1\xd8\xa8\xd8\xa9 "
                        "HRV 123";
    uint32_t full_len = (uint32_t)strlen(text);

    for(uint32_t len = 1; len <= full_len; len++) {
        TEST_ASSERT_TRUE_MESSAGE(
            bidi_process_check_overflow(text, len, LV_BASE_DIR_RTL),
            "Overflow detected in brute-force sweep"
        );
    }
}

/*
 * Test 10: NULL str_out (direction detection only, no write).
 * Should not crash even with misaligned len.
 */
void test_bidi_rtl_null_output_no_crash(void)
{
    const char arabic[] = "\xd8\xb9\xd8\xb1\xd8\xa8";

    /* Should not crash — str_out is NULL */
    lv_bidi_process_paragraph(arabic, NULL, 5, LV_BASE_DIR_RTL, NULL, 0);
    lv_bidi_process_paragraph(arabic, NULL, 3, LV_BASE_DIR_RTL, NULL, 0);
    lv_bidi_process_paragraph(arabic, NULL, 1, LV_BASE_DIR_RTL, NULL, 0);
}

/*
 * Test 11: lv_bidi_process — multi-paragraph bidi processing.
 * Covers the completely uncovered lv_bidi_process() and lv_bidi_get_next_paragraph().
 */
void test_bidi_process_multi_paragraph(void)
{
    /* Two paragraphs separated by newline, with Arabic text */
    const char input[] = "Hello\n\xd8\xb9\xd8\xb1\xd8\xa8";
    char out[32] = {0};

    lv_bidi_process(input, out, LV_BASE_DIR_LTR);

    /* First paragraph "Hello" should remain LTR */
    TEST_ASSERT_EQUAL_UINT8('H', out[0]);
    TEST_ASSERT_EQUAL_UINT8('e', out[1]);
    TEST_ASSERT_EQUAL_UINT8('l', out[2]);
    TEST_ASSERT_EQUAL_UINT8('l', out[3]);
    TEST_ASSERT_EQUAL_UINT8('o', out[4]);
    /* Newline preserved */
    TEST_ASSERT_EQUAL_UINT8('\n', out[5]);
    /* Null terminator at end */
    TEST_ASSERT_EQUAL_UINT8('\0', out[12]);
}

/*
 * Test 12: lv_bidi_process with leading newlines.
 * Covers the leading newline/carriage-return skip loop in lv_bidi_process().
 */
void test_bidi_process_leading_newlines(void)
{
    const char input[] = "\n\rHello";
    char out[16] = {0};

    lv_bidi_process(input, out, LV_BASE_DIR_LTR);

    TEST_ASSERT_EQUAL_UINT8('\n', out[0]);
    TEST_ASSERT_EQUAL_UINT8('\r', out[1]);
    TEST_ASSERT_EQUAL_UINT8('H', out[2]);
}

/*
 * Test 13: lv_bidi_process with AUTO base direction.
 * Covers the base_dir == LV_BASE_DIR_AUTO branch in lv_bidi_process().
 */
void test_bidi_process_auto_dir(void)
{
    /* Arabic first -> auto-detect RTL */
    const char input[] = "\xd8\xb9\xd8\xb1\xd8\xa8";
    char out[16] = {0};

    lv_bidi_process(input, out, LV_BASE_DIR_AUTO);

    /* Should not crash, output should be non-empty */
    TEST_ASSERT_NOT_EQUAL(0, out[0]);
}

/*
 * Test 14: lv_bidi_process with RTL base direction and multiple paragraphs.
 * Covers the trailing newline loop inside the main while loop of lv_bidi_process().
 */
void test_bidi_process_rtl_multi_paragraph(void)
{
    /* \xd8\xb9\xd8\xb1 = 4 bytes, then \n\n, then \xd8\xa8\xd9\x8a = 4 bytes => 10 bytes total */
    const char input[] = "\xd8\xb9\xd8\xb1\n\n\xd8\xa8\xd9\x8a";
    char out[32] = {0};

    lv_bidi_process(input, out, LV_BASE_DIR_RTL);

    /* Newlines should be preserved at byte offsets 4 and 5 */
    TEST_ASSERT_EQUAL_UINT8('\n', out[4]);
    TEST_ASSERT_EQUAL_UINT8('\n', out[5]);
    /* Null terminator */
    uint32_t full_len = (uint32_t)strlen(input);
    TEST_ASSERT_EQUAL_UINT8('\0', out[full_len]);
}

/*
 * Test 15: lv_bidi_set_custom_neutrals_static — set and use custom neutrals.
 * Covers the completely uncovered lv_bidi_set_custom_neutrals_static() and
 * the custom_neutrals branch in lv_bidi_letter_is_neutral().
 */
void test_bidi_set_custom_neutrals(void)
{
    /* Set custom neutrals to only space and period */
    lv_bidi_set_custom_neutrals_static(" .");

    /* Process text with custom neutrals active */
    const char input[] = "AB . \xd8\xb9\xd8\xb1";
    char out[32] = {0};

    lv_bidi_process_paragraph(input, out, (uint32_t)strlen(input), LV_BASE_DIR_LTR, NULL, 0);

    /* Should not crash, output should be non-empty */
    TEST_ASSERT_NOT_EQUAL(0, out[0]);

    /* Restore default neutrals */
    lv_bidi_set_custom_neutrals_static(NULL);
}

/*
 * Test 16: lv_bidi_get_logical_pos with bidi_txt == NULL.
 * Covers the NULL bidi_txt branch (internal buffer freed).
 */
void test_bidi_get_logical_pos_null_bidi_txt(void)
{
    const char input[] = "\xd8\xb9\xd8\xb1\xd8\xa8"; /* 3 Arabic chars, 6 bytes */
    uint32_t len = (uint32_t)strlen(input);

    uint16_t pos = lv_bidi_get_logical_pos(input, NULL, len, LV_BASE_DIR_RTL, 0, NULL);
    /* Should return a valid position (not -1) */
    TEST_ASSERT_NOT_EQUAL((uint16_t) -1, pos);
}

/*
 * Test 17: lv_bidi_get_logical_pos with is_rtl output.
 * Covers the is_rtl != NULL branch.
 */
void test_bidi_get_logical_pos_with_is_rtl(void)
{
    const char input[] = "\xd8\xb9\xd8\xb1"; /* 2 Arabic chars, 4 bytes */
    uint32_t len = (uint32_t)strlen(input);
    bool is_rtl = false;
    char * bidi_txt = NULL;

    uint16_t pos = lv_bidi_get_logical_pos(input, &bidi_txt, len, LV_BASE_DIR_RTL, 0, &is_rtl);
    TEST_ASSERT_NOT_EQUAL((uint16_t) -1, pos);
    /* Arabic text in RTL context should be RTL */
    TEST_ASSERT_TRUE(is_rtl);

    if(bidi_txt) lv_free(bidi_txt);
}

/*
 * Test 18: lv_bidi_get_visual_pos with bidi_txt == NULL.
 * Covers the NULL bidi_txt branch in lv_bidi_get_visual_pos().
 */
void test_bidi_get_visual_pos_null_bidi_txt(void)
{
    const char input[] = "\xd8\xb9\xd8\xb1\xd8\xa8"; /* 3 Arabic chars */
    uint16_t len = (uint16_t)strlen(input);

    uint16_t pos = lv_bidi_get_visual_pos(input, NULL, len, LV_BASE_DIR_RTL, 0, NULL);
    TEST_ASSERT_NOT_EQUAL((uint16_t) -1, pos);
}

/*
 * Test 19: lv_bidi_get_visual_pos with is_rtl output.
 * Covers the is_rtl != NULL branch in lv_bidi_get_visual_pos().
 */
void test_bidi_get_visual_pos_with_is_rtl(void)
{
    const char input[] = "\xd8\xb9\xd8\xb1"; /* 2 Arabic chars */
    uint16_t len = (uint16_t)strlen(input);
    bool is_rtl = false;
    char * bidi_txt = NULL;

    uint16_t pos = lv_bidi_get_visual_pos(input, &bidi_txt, len, LV_BASE_DIR_RTL, 0, &is_rtl);
    TEST_ASSERT_NOT_EQUAL((uint16_t) -1, pos);
    TEST_ASSERT_TRUE(is_rtl);

    if(bidi_txt) lv_free(bidi_txt);
}

/*
 * Test 20: lv_bidi_get_visual_pos — logical_pos not found.
 * Covers the "not found" return path (returns -1) in lv_bidi_get_visual_pos().
 */
void test_bidi_get_visual_pos_not_found(void)
{
    const char input[] = "AB"; /* 2 LTR chars */
    uint16_t len = (uint16_t)strlen(input);

    /* logical_pos = 999 doesn't exist */
    uint16_t pos = lv_bidi_get_visual_pos(input, NULL, len, LV_BASE_DIR_LTR, 999, NULL);
    TEST_ASSERT_EQUAL_UINT16((uint16_t) -1, pos);
}

/*
 * Test 21: lv_bidi_calculate_align with AUTO base_dir and RTL text.
 * Covers the RTL alignment branch in lv_bidi_calculate_align().
 */
void test_bidi_calculate_align_auto_rtl(void)
{
    lv_text_align_t align = LV_TEXT_ALIGN_AUTO;
    lv_base_dir_t dir = LV_BASE_DIR_AUTO;
    const char arabic[] = "\xd8\xb9\xd8\xb1\xd8\xa8";

    lv_bidi_calculate_align(&align, &dir, arabic);

    TEST_ASSERT_EQUAL(LV_BASE_DIR_RTL, dir);
    TEST_ASSERT_EQUAL(LV_TEXT_ALIGN_RIGHT, align);
}

/*
 * Test 22: lv_bidi_calculate_align with explicit LTR dir and AUTO align.
 * Covers the LTR alignment branch.
 */
void test_bidi_calculate_align_ltr(void)
{
    lv_text_align_t align = LV_TEXT_ALIGN_AUTO;
    lv_base_dir_t dir = LV_BASE_DIR_LTR;

    lv_bidi_calculate_align(&align, &dir, "Hello");

    TEST_ASSERT_EQUAL(LV_BASE_DIR_LTR, dir);
    TEST_ASSERT_EQUAL(LV_TEXT_ALIGN_LEFT, align);
}

/*
 * Test 23: lv_bidi_process_paragraph with AUTO base_dir.
 * Covers Branch 0 at line 193 (base_dir == LV_BASE_DIR_AUTO in process_paragraph).
 */
void test_bidi_process_paragraph_auto_dir(void)
{
    const char input[] = "\xd8\xb9\xd8\xb1 Hello";
    char out[32] = {0};

    lv_bidi_process_paragraph(input, out, (uint32_t)strlen(input), LV_BASE_DIR_AUTO, NULL, 0);
    /* Auto-detect should pick RTL from the first strong Arabic char */
    TEST_ASSERT_NOT_EQUAL(0, out[0]);
}

/*
 * Test 24: LTR text with neutral prefix and NULL str_out.
 * Covers the str_out==NULL branch at line 231 (LTR neutral prefix path).
 */
void test_bidi_process_paragraph_ltr_null_str_out(void)
{
    /* Space (neutral) + LTR text */
    const char input[] = " Hello";
    uint32_t len = (uint32_t)strlen(input);
    uint32_t pos_conv_len = 6; /* 6 chars */
    uint16_t pos_conv[8];

    /* str_out is NULL but pos_conv_out is provided */
    lv_bidi_process_paragraph(input, NULL, len, LV_BASE_DIR_LTR, pos_conv, pos_conv_len);
    /* Should not crash */
}

/*
 * Test 25: RTL base_dir with neutral prefix and non-NULL str_out.
 * Covers Branch 0 at line 243 (str_out != NULL in RTL neutral prefix path).
 */
void test_bidi_process_paragraph_rtl_neutral_prefix_with_output(void)
{
    /* Space (neutral) + Arabic */
    const char input[] = " \xd8\xb9\xd8\xb1";
    uint32_t len = (uint32_t)strlen(input);
    char out[16] = {0};
    uint32_t pos_conv_len = 3; /* space + 2 Arabic chars */
    uint16_t pos_conv[8];

    lv_bidi_process_paragraph(input, out, len, LV_BASE_DIR_RTL, pos_conv, pos_conv_len);
    TEST_ASSERT_NOT_EQUAL(0, out[0]);
}

/*
 * Test 26: RTL base_dir with LTR run embedded — covers lines 270-271.
 * "Hello" (LTR) embedded in RTL context with str_out and pos_conv.
 */
void test_bidi_process_paragraph_rtl_with_ltr_run(void)
{
    /* Arabic + LTR word + Arabic */
    const char input[] = "\xd8\xb9 Hello \xd8\xb1";
    uint32_t len = (uint32_t)strlen(input);
    char out[32] = {0};
    uint32_t pos_conv_len = 9; /* 1 + 1 + 5 + 1 + 1 */
    uint16_t pos_conv[16];

    lv_bidi_process_paragraph(input, out, len, LV_BASE_DIR_RTL, pos_conv, pos_conv_len);
    TEST_ASSERT_NOT_EQUAL(0, out[0]);
}

/*
 * Test 27: lv_bidi_process with \r in trailing newlines.
 * Covers Branch 6 at line 94 (the \r path in trailing newline loop).
 */
void test_bidi_process_carriage_return(void)
{
    const char input[] = "Hello\r\nWorld";
    char out[32] = {0};

    lv_bidi_process(input, out, LV_BASE_DIR_LTR);

    TEST_ASSERT_EQUAL_UINT8('H', out[0]);
    TEST_ASSERT_EQUAL_UINT8('\r', out[5]);
    TEST_ASSERT_EQUAL_UINT8('\n', out[6]);
    TEST_ASSERT_EQUAL_UINT8('W', out[7]);
}

/*
 * Test 28: Arabic Presentation Forms characters.
 * Covers the uncovered ranges in lv_bidi_letter_is_rtl():
 *   0xFB50-0xFDFF (Arabic Presentation Forms-A)
 *   0xFE70-0xFEFF (Arabic Presentation Forms-B)
 *   0xFB1D-0xFB4F (Hebrew Presentation Forms)
 */
void test_bidi_arabic_presentation_forms(void)
{
    /* U+FB50 = \xEF\xAD\x90 (Arabic Presentation Forms-A) */
    const char pres_a[] = "\xef\xad\x90\xef\xad\x91";
    char out_a[16] = {0};
    lv_bidi_process_paragraph(pres_a, out_a, (uint32_t)strlen(pres_a), LV_BASE_DIR_RTL, NULL, 0);
    TEST_ASSERT_NOT_EQUAL(0, out_a[0]);

    /* U+FE70 = \xEF\xB9\xB0 (Arabic Presentation Forms-B) */
    const char pres_b[] = "\xef\xb9\xb0\xef\xb9\xb1";
    char out_b[16] = {0};
    lv_bidi_process_paragraph(pres_b, out_b, (uint32_t)strlen(pres_b), LV_BASE_DIR_RTL, NULL, 0);
    TEST_ASSERT_NOT_EQUAL(0, out_b[0]);

    /* U+FB1D = \xEF\xAC\x9D (Hebrew Presentation Forms) */
    const char heb_pres[] = "\xef\xac\x9d\xef\xac\x9e";
    char out_h[16] = {0};
    lv_bidi_process_paragraph(heb_pres, out_h, (uint32_t)strlen(heb_pres), LV_BASE_DIR_RTL, NULL, 0);
    TEST_ASSERT_NOT_EQUAL(0, out_h[0]);
}

/*
 * Test 29: RTL text with digits — weak chars in RTL context.
 * Covers the weak handling branches in get_next_run() lines 460-463
 * (run_dir==RTL, base_dir==RTL, weak -> LTR).
 */
void test_bidi_rtl_weak_digits_in_rtl_context(void)
{
    /* Arabic + digits + Arabic: triggers weak handling in RTL run */
    const char input[] = "\xd8\xb9" "123" "\xd8\xb1";
    uint32_t len = (uint32_t)strlen(input);
    char out[32] = {0};
    uint16_t pos_conv[16];
    uint32_t pos_conv_len = 5; /* 1 Arabic + 3 digits + 1 Arabic */

    lv_bidi_process_paragraph(input, out, len, LV_BASE_DIR_RTL, pos_conv, pos_conv_len);
    TEST_ASSERT_NOT_EQUAL(0, out[0]);
}

/*
 * Test 30: RTL reverse with NULL dest but non-NULL pos_conv.
 * Covers dest==NULL branches in rtl_reverse() at lines 553, 571.
 */
void test_bidi_rtl_reverse_null_dest_with_pos_conv(void)
{
    const char input[] = "\xd8\xb9\xd8\xb1";
    uint32_t len = (uint32_t)strlen(input);
    uint32_t pos_conv_len = 2;
    uint16_t pos_conv[4];

    /* str_out=NULL, pos_conv_out=non-NULL */
    lv_bidi_process_paragraph(input, NULL, len, LV_BASE_DIR_LTR, pos_conv, pos_conv_len);
    /* Should not crash, pos_conv should be filled */
}

/*
 * Test 31: Brackets with no strong char inside — falls through to previous char check.
 * Covers bracket_process lines 638-652 (check previous chars for direction).
 */
void test_bidi_brackets_no_strong_inside(void)
{
    /* Arabic + bracket with only neutrals inside + Arabic */
    const char input[] = "\xd8\xb9( . )\xd8\xb1";
    uint32_t len = (uint32_t)strlen(input);
    char out[32] = {0};

    lv_bidi_process_paragraph(input, out, len, LV_BASE_DIR_RTL, NULL, 0);
    TEST_ASSERT_NOT_EQUAL(0, out[0]);
}

/*
 * Test 32: Brackets at the very start of text — no previous strong chars.
 * Covers bracket_process line 656 (txt_i == 0, use base_dir).
 */
void test_bidi_brackets_at_start(void)
{
    /* Opening bracket at start with neutrals inside, then Arabic */
    const char input[] = "( . )\xd8\xb9\xd8\xb1";
    uint32_t len = (uint32_t)strlen(input);
    char out[32] = {0};

    lv_bidi_process_paragraph(input, out, len, LV_BASE_DIR_RTL, NULL, 0);
    TEST_ASSERT_NOT_EQUAL(0, out[0]);
}

/*
 * Test 33: Bracket with no matching closing bracket.
 * Covers bracket_process line 631 (txt_i > len, return NEUTRAL).
 */
void test_bidi_bracket_no_closing(void)
{
    /* Opening bracket with no closing */
    const char input[] = "\xd8\xb9(\xd8\xb1";
    uint32_t len = (uint32_t)strlen(input);
    char out[16] = {0};

    lv_bidi_process_paragraph(input, out, len, LV_BASE_DIR_RTL, NULL, 0);
    TEST_ASSERT_NOT_EQUAL(0, out[0]);
}

/*
 * Test 34: Deeply nested brackets to overflow bracket stack (depth > 4).
 * Covers bracket_process line 661 (br_stack_p == LV_BIDI_BRACKET_DEPTH).
 */
void test_bidi_bracket_stack_overflow(void)
{
    /* 5 nested opening brackets with Arabic inside each — exceeds LV_BIDI_BRACKET_DEPTH (4) */
    const char input[] = "([\xd8\xb9{<\xd8\xb1(\xd8\xa8";
    uint32_t len = (uint32_t)strlen(input);
    char out[32] = {0};

    lv_bidi_process_paragraph(input, out, len, LV_BASE_DIR_LTR, NULL, 0);
    /* Should not crash even with stack overflow */
    TEST_ASSERT_NOT_EQUAL(0, out[0]);
}

/*
 * Test 35: U+202E (RLO) character — the special RTL override.
 * Covers lv_bidi_letter_is_rtl line 358 (letter == 0x202E).
 */
void test_bidi_rlo_character(void)
{
    /* U+202E = \xE2\x80\xAE (RLO) + LTR text */
    const char input[] = "\xe2\x80\xae" "Hello";
    uint32_t len = (uint32_t)strlen(input);
    char out[16] = {0};

    lv_bidi_process_paragraph(input, out, len, LV_BASE_DIR_LTR, NULL, 0);
    TEST_ASSERT_NOT_EQUAL(0, out[0]);
}

/*
 * Test 36: get_next_run with run_dir != base_dir at end of string.
 * Covers the "exclude neutrals" else branch at end of get_next_run (lines 500-503).
 * LTR base_dir with RTL run followed by trailing neutrals.
 */
void test_bidi_ltr_base_rtl_run_trailing_neutrals(void)
{
    /* Arabic + trailing spaces */
    const char input[] = "\xd8\xb9\xd8\xb1  ";
    uint32_t len = (uint32_t)strlen(input);
    char out[16] = {0};

    lv_bidi_process_paragraph(input, out, len, LV_BASE_DIR_LTR, NULL, 0);
    TEST_ASSERT_NOT_EQUAL(0, out[0]);
}

/*
 * Test 37: get_next_run direction change with run_dir != base_dir.
 * Covers the "exclude neutrals" branch at lines 475-477 in get_next_run.
 * RTL run followed by neutral then LTR, with LTR base_dir.
 */
void test_bidi_dir_change_exclude_neutrals(void)
{
    /* Arabic + space + LTR text, base_dir=LTR */
    const char input[] = "\xd8\xb9\xd8\xb1 Hello";
    uint32_t len = (uint32_t)strlen(input);
    char out[32] = {0};

    lv_bidi_process_paragraph(input, out, len, LV_BASE_DIR_LTR, NULL, 0);
    TEST_ASSERT_NOT_EQUAL(0, out[0]);
}

#endif /*LV_USE_BIDI*/

#endif /*LV_BUILD_TEST*/
