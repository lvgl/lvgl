/**
 * @file lv_text_ap.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "lv_bidi.h"
#include "lv_text.h"
#include "lv_text_ap.h"
#include "../stdlib/lv_mem.h"
#include "../draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint8_t char_offset;
    uint16_t char_end_form;
    int8_t char_beginning_form_offset;
    int8_t char_middle_form_offset;
    int8_t char_isolated_form_offset;
    struct {
        uint8_t conj_to_previous;
        uint8_t conj_to_next;
    } ap_chars_conjunction;
} ap_chars_map_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
static uint32_t lv_ap_get_char_index(uint16_t c);
static uint32_t lv_text_lam_alef(uint32_t ch_curr, uint32_t ch_next);
static bool lv_text_is_arabic_vowel(uint16_t c);

/**********************
 *  STATIC VARIABLES
 **********************/

const ap_chars_map_t ap_chars_map[] = {
    /*{Key Offset, End, Beginning, Middle, Isolated, {conjunction}}*/
    {1, 0xFE84, -1, 0, -1,  {1, 0}}, // U+0623 Arabic Letter Alef with Hamza Above
    {2, 0xFE86, -1, 0, -1,  {1, 0}}, // U+0624 Arabic Letter Waw with Hamza Above
    {3, 0xFE88, -1, 0, -1,  {1, 0}}, // U+FE87 Arabic Letter Alef with Hamza Below Isolated Form
    {4, 0xFE8A, 1, 2, -1,  {1, 0}}, // U+0626 Arabic Letter Yeh with Hamza Above
    {5, 0xFE8E, -1, 0, -1,  {1, 0}}, // U+0622 Arabic Letter Alef with Madda Above
    {6, 0xFE90, 1, 2, -1,  {1, 1}}, // U+0628 Arabic Letter Beh
    {92, 0xFB57, 1, 2, -1,  {1, 1}}, // U+067E Arabic Letter Peh
    {8, 0xFE96, 1, 2, -1,  {1, 1}}, // U+062A Arabic Letter Teh
    {9, 0xFE9A, 1, 2, -1,  {1, 1}}, // U+062B Arabic Letter Theh
    {10, 0xFE9E, 1, 2, -1,  {1, 1}}, // U+062C Arabic Letter Jeem
    {100, 0xFB7B, 1, 2, -1,  {1, 1}}, // U+0686 Arabic Letter Tcheh
    {11, 0xFEA2, 1, 2, -1,  {1, 1}}, // U+062D Arabic Letter Hah
    {12, 0xFEA6, 1, 2, -1,  {1, 1}}, // U+062E Arabic Letter Khah
    {13, 0xFEAA, -1, 0, -1,  {1, 0}}, // U+062F Arabic Letter Dal
    {14, 0xFEAC, -1, 0, -1,  {1, 0}}, // U+0630 Arabic Letter Thal
    {15, 0xFEAE, -1, 0, -1,  {1, 0}}, // U+0631 Arabic Letter Reh
    {16, 0xFEB0, -1, 0, -1,  {1, 0}}, // U+0632 Arabic Letter Zain
    {118, 0xFB8B, -1, 0, -1,  {1, 0}}, // U+0698 Arabic Letter Jeh
    {17, 0xFEB2, 1, 2, -1,  {1, 1}}, // U+0633 Arabic Letter Seen
    {18, 0xFEB6, 1, 2, -1,  {1, 1}}, // U+0634 Arabic Letter Sheen
    {19, 0xFEBA, 1, 2, -1,  {1, 1}}, // U+0635 Arabic Letter Sad
    {20, 0xFEBE, 1, 2, -1,  {1, 1}}, // U+0636 Arabic Letter Dad
    {21, 0xFEC2, 1, 2, -1,  {1, 1}}, // U+0637 Arabic Letter Tah
    {22, 0xFEC6, 1, 2, -1,  {1, 1}}, // U+0638 Arabic Letter Zah
    {23, 0xFECA, 1, 2, -1,  {1, 1}}, // U+0639 Arabic Letter Ain
    {24, 0xFECE, 1, 2, -1,  {1, 1}}, // U+063A Arabic Letter Ghain
    {30, 0x0640, 0, 0, 0,  {1, 1}}, // U+0640 Arabic Tatweel
    {31, 0xFED2, 1, 2, -1,  {1, 1}}, // U+0641 Arabic Letter Feh
    {32, 0xFED6, 1, 2, -1,  {1, 1}}, // U+0642 Arabic Letter Qaf
    {135, 0xFB8F, 1, 2, -1,  {1, 1}}, // U+06A9 Arabic Letter Keheh
    {33, 0xFEDA, 1, 2, -1,  {1, 1}}, // U+FED9 Arabic Letter Kaf Isolated Form
    {141, 0xFB93, 1, 2, -1,  {1, 1}}, // U+06AF Arabic Letter Gaf
    {34, 0xFEDE, 1, 2, -1,  {1, 1}}, // U+0644 Arabic Letter Lam
    {35, 0xFEE2, 1, 2, -1,  {1, 1}}, // U+0645 Arabic Letter Meem
    {36, 0xFEE6, 1, 2, -1,  {1, 1}}, // U+0646 Arabic Letter Noon
    {38, 0xFEEE, -1, 0, -1,  {1, 0}}, // U+0648 Arabic Letter Waw
    {37, 0xFEEA, 1, 2, -1,  {1, 1}}, // U+0647 Arabic Letter Heh
    {39, 0xFEF0, 0, 0, -1, {1, 0}}, // U+0649 Arabic Letter Alef Maksura
    {40, 0xFEF2, 1, 2, -1,  {1, 1}}, // U+064A Arabic Letter Yeh
    {170, 0xFBFD, 1, 2, -1,  {1, 1}}, // U+06CC Arabic Letter Farsi Yeh
    {7, 0xFE94, 1, 2, -1,  {1, 0}}, // U+0629 Arabic Letter Teh Marbuta
    {206, 0x06F0, 1, 2, -1,  {0, 0}}, // U+06F0 Extended Arabic-Indic Digit Zero
    {207, 0x06F1, 0, 0, 0,  {0, 0}}, // U+06F1 Extended Arabic-Indic Digit One
    {208, 0x06F2, 0, 0, 0,  {0, 0}}, // U+06F2 Extended Arabic-Indic Digit Two
    {209, 0x06F3, 0, 0, 0,  {0, 0}}, // U+06F3 Extended Arabic-Indic Digit Three
    {210, 0x06F4, 0, 0, 0,  {0, 0}}, // U+06F4 Extended Arabic-Indic Digit Four
    {211, 0x06F5, 0, 0, 0,  {0, 0}}, // U+06F5 Extended Arabic-Indic Digit Five
    {212, 0x06F6, 0, 0, 0,  {0, 0}}, // U+06F6 Extended Arabic-Indic Digit Six
    {213, 0x06F7, 0, 0, 0,  {0, 0}}, // U+06F7 Extended Arabic-Indic Digit Seven
    {214, 0x06F8, 0, 0, 0,  {0, 0}}, // U+06F8 Extended Arabic-Indic Digit Eight
    {215, 0x06F9, 0, 0, 0,  {0, 0}}, // U+06F9 Extended Arabic-Indic Digit Nine
    LV_AP_END_CHARS_LIST
};
/**********************
*      MACROS
**********************/

/**********************
*   GLOBAL FUNCTIONS
**********************/
uint32_t _lv_text_ap_calc_bytes_cnt(const char * txt)
{
    uint32_t txt_length = 0;
    uint32_t chars_cnt = 0;
    uint32_t current_ap_idx = 0;
    uint32_t i, j;
    uint32_t ch_enc;

    txt_length = _lv_text_get_encoded_length(txt);

    i = 0;
    j = 0;
    while(i < txt_length) {
        ch_enc = _lv_text_encoded_next(txt, &j);
        current_ap_idx = lv_ap_get_char_index(ch_enc);

        if(current_ap_idx != LV_UNDEF_ARABIC_PERSIAN_CHARS)
            ch_enc = ap_chars_map[current_ap_idx].char_end_form;

        if(ch_enc < 0x80)
            chars_cnt++;
        else if(ch_enc < 0x0800)
            chars_cnt += 2;
        else if(ch_enc < 0x010000)
            chars_cnt += 3;
        else
            chars_cnt += 4;

        i++;
    }

    return chars_cnt + 1;
}

void _lv_text_ap_proc(const char * txt, char * txt_out)
{
    uint32_t txt_length = 0;
    uint32_t index_current, idx_next, idx_previous, i, j;
    uint32_t * ch_enc;
    uint32_t * ch_fin;
    char * txt_out_temp;

    txt_length = _lv_text_get_encoded_length(txt);

    ch_enc = (uint32_t *)lv_malloc(sizeof(uint32_t) * (txt_length + 1));
    ch_fin = (uint32_t *)lv_malloc(sizeof(uint32_t) * (txt_length + 1));

    i = 0;
    j = 0;
    while(j < txt_length)
        ch_enc[j++] = _lv_text_encoded_next(txt, &i);

    ch_enc[j] = 0;

    i = 0;
    j = 0;
    idx_previous = LV_UNDEF_ARABIC_PERSIAN_CHARS;
    while(i < txt_length) {
        index_current = lv_ap_get_char_index(ch_enc[i]);
        idx_next = lv_ap_get_char_index(ch_enc[i + 1]);

        if(lv_text_is_arabic_vowel(ch_enc[i])) {  // Current character is a vowel
            ch_fin[j] = ch_enc[i];
            i++;
            j++;
            continue;   // Skip this character
        }
        else if(lv_text_is_arabic_vowel(ch_enc[i + 1])) {    // Next character is a vowel
            idx_next = lv_ap_get_char_index(ch_enc[i + 2]); // Skip the vowel character to join with the character after it
        }

        if(index_current == LV_UNDEF_ARABIC_PERSIAN_CHARS) {
            ch_fin[j] = ch_enc[i];
            j++;
            i++;
            idx_previous = LV_UNDEF_ARABIC_PERSIAN_CHARS;
            continue;
        }

        uint8_t conjunction_to_previuse = (i == 0 ||
                                           idx_previous == LV_UNDEF_ARABIC_PERSIAN_CHARS) ? 0 : ap_chars_map[idx_previous].ap_chars_conjunction.conj_to_next;
        uint8_t conjunction_to_next = ((i == txt_length - 1) ||
                                       idx_next == LV_UNDEF_ARABIC_PERSIAN_CHARS) ? 0 : ap_chars_map[idx_next].ap_chars_conjunction.conj_to_previous;

        uint32_t lam_alef = lv_text_lam_alef(index_current, idx_next);
        if(lam_alef) {
            if(conjunction_to_previuse) {
                lam_alef ++;
            }
            ch_fin[j] = lam_alef;
            idx_previous = LV_UNDEF_ARABIC_PERSIAN_CHARS;
            i += 2;
            j++;
            continue;
        }

        if(conjunction_to_previuse && conjunction_to_next)
            ch_fin[j] = ap_chars_map[index_current].char_end_form + ap_chars_map[index_current].char_middle_form_offset;
        else if(!conjunction_to_previuse && conjunction_to_next)
            ch_fin[j] = ap_chars_map[index_current].char_end_form + ap_chars_map[index_current].char_beginning_form_offset;
        else if(conjunction_to_previuse && !conjunction_to_next)
            ch_fin[j] = ap_chars_map[index_current].char_end_form;
        else
            ch_fin[j] = ap_chars_map[index_current].char_end_form + ap_chars_map[index_current].char_isolated_form_offset;
        idx_previous = index_current;
        i++;
        j++;
    }
    ch_fin[j] = 0;
    for(i = 0; i < txt_length; i++)
        ch_enc[i] = 0;
    for(i = 0; i < j; i++)
        ch_enc[i] = ch_fin[i];
    lv_free(ch_fin);

    txt_out_temp = txt_out;
    i = 0;

    while(i < txt_length) {
        if(ch_enc[i] < 0x80) {
            *(txt_out_temp++) = ch_enc[i] & 0xFF;
        }
        else if(ch_enc[i] < 0x0800) {
            *(txt_out_temp++) = ((ch_enc[i] >> 6) & 0x1F) | 0xC0;
            *(txt_out_temp++) = ((ch_enc[i] >> 0) & 0x3F) | 0x80;
        }
        else if(ch_enc[i] < 0x010000) {
            *(txt_out_temp++) = ((ch_enc[i] >> 12) & 0x0F) | 0xE0;
            *(txt_out_temp++) = ((ch_enc[i] >> 6) & 0x3F) | 0x80;
            *(txt_out_temp++) = ((ch_enc[i] >> 0) & 0x3F) | 0x80;
        }
        else if(ch_enc[i] < 0x110000) {
            *(txt_out_temp++) = ((ch_enc[i] >> 18) & 0x07) | 0xF0;
            *(txt_out_temp++) = ((ch_enc[i] >> 12) & 0x3F) | 0x80;
            *(txt_out_temp++) = ((ch_enc[i] >> 6) & 0x3F) | 0x80;
            *(txt_out_temp++) = ((ch_enc[i] >> 0) & 0x3F) | 0x80;
        }

        i++;
    }
    *(txt_out_temp) = '\0';
    lv_free(ch_enc);
}
/**********************
*   STATIC FUNCTIONS
**********************/

static uint32_t lv_ap_get_char_index(uint16_t c)
{
    for(uint8_t i = 0; ap_chars_map[i].char_end_form; i++) {
        if(c == (ap_chars_map[i].char_offset + LV_AP_ALPHABET_BASE_CODE))
            return i;
        else if(c == ap_chars_map[i].char_end_form                                                  //is it an End form
                || c == (ap_chars_map[i].char_end_form + ap_chars_map[i].char_beginning_form_offset)     //is it a Beginning form
                || c == (ap_chars_map[i].char_end_form + ap_chars_map[i].char_middle_form_offset)       //is it a middle form
                || c == (ap_chars_map[i].char_end_form + ap_chars_map[i].char_isolated_form_offset)) {  //is it an isolated form
            return i;
        }
    }
    return LV_UNDEF_ARABIC_PERSIAN_CHARS;
}

static uint32_t lv_text_lam_alef(uint32_t ch_curr, uint32_t ch_next)
{
    uint32_t ch_code = 0;
    if(ap_chars_map[ch_curr].char_offset != 34) {
        return 0;
    }
    if(ch_next == LV_UNDEF_ARABIC_PERSIAN_CHARS) {
        return 0;
    }
    ch_code = ap_chars_map[ch_next].char_offset + LV_AP_ALPHABET_BASE_CODE;
    if(ch_code == 0x0622) {
        return 0xFEF5;    // (lam-alef) mad
    }
    if(ch_code == 0x0623) {
        return 0xFEF7;    // (lam-alef) top hamza
    }
    if(ch_code == 0x0625) {
        return 0xFEF9;    // (lam-alef) bot hamza
    }
    if(ch_code == 0x0627) {
        return 0xFEFB;    // (lam-alef) alef
    }
    return 0;
}

static bool lv_text_is_arabic_vowel(uint16_t c)
{
    return (c >= 0x064B) && (c <= 0x0652);
}

#endif
