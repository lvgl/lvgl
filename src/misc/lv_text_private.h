/**
 * @file lv_text_private.h
 *
 */

#ifndef LV_TEXT_PRIVATE_H
#define LV_TEXT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_text.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get the next line of text. Check line length and break chars too.
 * @param txt a '\0' terminated string
 * @param font pointer to a font
 * @param letter_space letter space
 * @param max_width max width of the text (break the lines to fit this size). Set COORD_MAX to avoid
 * line breaks
 * @param used_width When used_width != NULL, save the width of this line if
 * flag == LV_TEXT_FLAG_NONE, otherwise save -1.
 * @param flag settings for the text from 'txt_flag_type' enum
 * @return the index of the first char of the new line (in byte index not letter index. With UTF-8
 * they are different)
 */
uint32_t lv_text_get_next_line(const char * txt, const lv_font_t * font, int32_t letter_space,
                               int32_t max_width, int32_t * used_width, lv_text_flag_t flag);

/**
 * Insert a string into another
 * @param txt_buf the original text (must be big enough for the result text and NULL terminated)
 * @param pos position to insert (0: before the original text, 1: after the first char etc.)
 * @param ins_txt text to insert, must be '\0' terminated
 */
void lv_text_ins(char * txt_buf, uint32_t pos, const char * ins_txt);

/**
 * Delete a part of a string
 * @param txt string to modify, must be '\0' terminated and should point to a heap or stack frame, not read-only memory.
 * @param pos position where to start the deleting (0: before the first char, 1: after the first
 * char etc.)
 * @param len number of characters to delete
 */
void lv_text_cut(char * txt, uint32_t pos, uint32_t len);

/**
 * return a new formatted text. Memory will be allocated to store the text.
 * @param fmt `printf`-like format
 * @param ap items to print

 * @return pointer to the allocated text string.
 */
char * lv_text_set_text_vfmt(const char * fmt, va_list ap) LV_FORMAT_ATTRIBUTE(1, 0);

/**
 * Decode two encoded character from a string.
 * @param txt pointer to '\0' terminated string
 * @param letter the first decoded Unicode character or 0 on invalid data code
 * @param letter_next the second decoded Unicode character or 0 on invalid data code
 * @param ofs start index in 'txt' where to start.
 *                After the call it will point to the next encoded char in 'txt'.
 *                NULL to use txt[0] as index
 */
void lv_text_encoded_letter_next_2(const char * txt, uint32_t * letter, uint32_t * letter_next, uint32_t * ofs);

/**
 * Test if char is break char or not (a text can broken here or not)
 * @param letter a letter
 * @return false: 'letter' is not break char
 */
static inline bool lv_text_is_break_char(uint32_t letter)
{
    uint8_t i;
    bool ret = false;

    /*Compare the letter to TXT_BREAK_CHARS*/
    for(i = 0; LV_TXT_BREAK_CHARS[i] != '\0'; i++) {
        if(letter == (uint32_t)LV_TXT_BREAK_CHARS[i]) {
            ret = true; /*If match then it is break char*/
            break;
        }
    }

    return ret;
}

/**
 * Test if char is break char or not (a text can broken here or not)
 * @param letter a letter
 * @return false: 'letter' is not break char
 */
static inline bool lv_text_is_a_word(uint32_t letter)
{
    /*Cheap check on invalid letter*/
    if(letter == 0) return false;

    /*CJK Unified Ideographs*/
    if(letter >= 0x4E00 && letter <= 0x9FFF) {
        return true;
    }

    /*Fullwidth ASCII variants*/
    if(letter >= 0xFF01 && letter <= 0xFF5E) {
        return true;
    }

    /*CJK symbols and punctuation*/
    if(letter >= 0x3000 && letter <= 0x303F) {
        return true;
    }

    /*CJK Radicals Supplement*/
    if(letter >= 0x2E80 && letter <= 0x2EFF) {
        return true;
    }

    /*CJK Strokes*/
    if(letter >= 0x31C0 && letter <= 0x31EF) {
        return true;
    }

    /*Hiragana and Katakana*/
    if(letter >= 0x3040 && letter <= 0x30FF) {
        return true;
    }

    /*Chinese Vertical Forms*/
    if(letter >= 0xFE10 && letter <= 0xFE1F) {
        return true;
    }

    /*CJK Compatibility Forms*/
    if(letter >= 0xFE30 && letter <= 0xFE4F) {
        return true;
    }

    return false;
}

/**
 * Test if character can be treated as marker, and don't need to be rendered.
 * Note, this is not a full list. Add your findings to the list.
 *
 * @param letter a letter
 * @return true if so
 */
static inline bool lv_text_is_marker(uint32_t letter)
{
    if(letter < 0x20) return true;

    /*U+061C ARABIC LETTER MARK, see https://www.compart.com/en/unicode/block/U+0600*/
    if(letter == 0x061C) return true;

    /*U+115F HANGUL CHOSEONG FILLER, See https://www.compart.com/en/unicode/block/U+1100*/
    if(letter == 0x115F) return true;
    /*U+1160 HANGUL JUNGSEONG FILLER*/
    if(letter == 0x1160) return true;

    /*See https://www.compart.com/en/unicode/block/U+1800*/
    if(letter >= 0x180B && letter <= 0x180E) return true;

    /*See https://www.compart.com/en/unicode/block/U+2000*/
    if(letter >= 0x200B && letter <= 0x200F) return true;
    if(letter >= 0x2028 && letter <= 0x202F) return true;
    if(letter >= 0x205F && letter <= 0x206F) return true;

    /*U+FEFF ZERO WIDTH NO-BREAK SPACE, see https://www.compart.com/en/unicode/block/U+FE70*/
    if(letter == 0xFEFF) return true;

    if(letter == 0xF8FF) return true; /*LV_SYMBOL_DUMMY*/

    return false;
}

/***************************************************************
 *  GLOBAL FUNCTION POINTERS FOR CHARACTER ENCODING INTERFACE
 ***************************************************************/

/**
 * Give the size of an encoded character
 * @param txt  pointer to a character in a string
 * @return length of the encoded character (1,2,3 ...). O in invalid
 */
extern uint8_t (*const lv_text_encoded_size)(const char * txt);

/**
 * Convert a Unicode letter to encoded
 * @param letter_uni a Unicode letter
 * @return Encoded character in Little Endian to be compatible with C chars (e.g. 'Á', 'Ü')
 */
extern uint32_t (*const lv_text_unicode_to_encoded)(uint32_t letter_uni);

/**
 * Convert a wide character, e.g. 'Á' little endian to be compatible with the encoded format.
 * @param c a wide character
 * @return `c` in the encoded format
 */
extern uint32_t (*const lv_text_encoded_conv_wc)(uint32_t c);

/**
 * Decode the next encoded character from a string.
 * @param txt      pointer to '\0' terminated string
 * @param i_start  start index in 'txt' where to start.
 *                 After the call it will point to the next encoded char in 'txt'.
 *                 NULL to use txt[0] as index
 * @return the decoded Unicode character or 0 on invalid data code
 */
extern uint32_t (*const lv_text_encoded_next)(const char * txt, uint32_t * i_start);

/**
 * Get the previous encoded character form a string.
 *
 * @param  txt      pointer to '\0' terminated string
 * @param  i_start  index in 'txt' where to start. After the call it will point to the previous
 *                    encoded char in 'txt'.
 *
 * @return the decoded Unicode character or 0 on invalid data
 */
extern uint32_t (*const lv_text_encoded_prev)(const char * txt, uint32_t * i_start);

/**
 * Convert a letter index (in the encoded text) to byte index.
 * E.g. in UTF-8 "AÁRT" index of 'R' is 2 but start at byte 3 because 'Á' is 2 bytes long
 * @param txt a '\0' terminated UTF-8 string
 * @param utf8_id character index
 * @return byte index of the 'enc_id'th letter
 */
extern uint32_t (*const lv_text_encoded_get_byte_id)(const char * txt, uint32_t utf8_id);

/**
 * Convert a byte index (in an encoded text) to character index.
 * E.g. in UTF-8 "AÁRT" index of 'R' is 2 but start at byte 3 because 'Á' is 2 bytes long
 * @param txt a '\0' terminated UTF-8 string
 * @param byte_id byte index
 * @return character index of the letter at 'byte_id'th position
 */
extern uint32_t (*const lv_text_encoded_get_char_id)(const char * txt, uint32_t byte_id);

/**
 * Get the number of characters (and NOT bytes) in a string.
 * E.g. in UTF-8 "ÁBC" is 3 characters (but 4 bytes)
 * @param txt a '\0' terminated char string
 * @return number of characters
 */
extern uint32_t (*const lv_text_get_encoded_length)(const char * txt);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEXT_PRIVATE_H*/
