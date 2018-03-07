/**
 * @file lv_text.h
 * 
 */

#ifndef LV_TXT_H
#define LV_TXT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "lv_area.h"
#include "lv_font.h"
#include "lv_area.h"

/*********************
 *      DEFINES
 *********************/
#define LV_TXT_COLOR_CMD  "#"

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    LV_TXT_FLAG_NONE =     0x00,
    LV_TXT_FLAG_RECOLOR =  0x01,   /*Enable parsing of recolor command*/
    LV_TXT_FLAG_EXPAND =   0x02,   /*Ignore width (Used by the library)*/
    LV_TXT_FLAG_NO_BREAK = 0x04,   /*Ignore line breaks (Used by the library)*/
    LV_TXT_FLAG_CENTER =   0x08,   /*Align the text to the middle*/
}lv_txt_flag_t;

typedef enum
{
    LV_TXT_CMD_STATE_WAIT,      /*Waiting for command*/
    LV_TXT_CMD_STATE_PAR,       /*Processing the parameter*/
    LV_TXT_CMD_STATE_IN,        /*Processing the command*/
}lv_txt_cmd_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get size of a text
 * @param size_res pointer to a 'point_t' variable to store the result
 * @param text pointer to a text
 * @param font pinter to font of the text
 * @param letter_space letter space of the text
 * @param line_space line space of the text
 * @param flags settings for the text from 'txt_flag_t' enum
 * @param max_width max with of the text (break the lines to fit this size) Set CORD_MAX to avoid line breaks
 */
void lv_txt_get_size(lv_point_t * size_res, const char * text, const lv_font_t * font,
                    lv_coord_t letter_space, lv_coord_t line_space, lv_coord_t max_width, lv_txt_flag_t flag);

/**
 * Get the next line of text. Check line length and break chars too.
 * @param txt a '\0' terminated string
 * @param font_p pointer to a font
 * @param letter_space letter space
 * @param max_width max with of the text (break the lines to fit this size) Set CORD_MAX to avoid line breaks
 * @param flags settings for the text from 'txt_flag_t' enum
 * @return the index of the first char of the new line
 */
uint16_t lv_txt_get_next_line(const char * txt, const lv_font_t * font_p,
                            lv_coord_t letter_space, lv_coord_t max_l, lv_txt_flag_t flag);

/**
 * Give the length of a text with a given font
 * @param txt a '\0' terminate string
 * @param char_num number of characters in 'txt'
 * @param font_p pointer to a font
 * @param letter_space letter space
 * @param flags settings for the text from 'txt_flag_t' enum
 * @return length of a char_num long text
 */
lv_coord_t lv_txt_get_width(const char * txt, uint16_t char_num,
                    const lv_font_t * font_p, lv_coord_t letter_space, lv_txt_flag_t flag);

/**
 * Check next character in a string and decide if te character is part of the command or not
 * @param state pointer to a txt_cmd_state_t variable which stores the current state of command processing
 * @param c the current character
 * @return true: the character is part of a command and should not be written,
 *         false: the character should be written
 */
bool lv_txt_is_cmd(lv_txt_cmd_state_t * state, uint32_t c);

/**
 * Insert a string into an other
 * @param txt_buf the original text (must be big enough for the result text)
 * @param pos position to insert (0: before the original text, 1: after the first char etc.)
 * @param ins_txt text to insert
 */
void lv_txt_ins(char * txt_buf, uint32_t pos, const char * ins_txt);

/**
 * Delete a part of a string
 * @param txt string to modify
 * @param pos position where to start the deleting (0: before the first char, 1: after the first char etc.)
 * @param len number of characters to delete
 */
void lv_txt_cut(char * txt, uint32_t pos, uint32_t len);

/**
 * Give the size of an UTF-8 coded character
 * @param c A character where the UTF-8 character starts
 * @return length of the UTF-8 character (1,2,3 or 4). O on invalid code
 */
uint8_t lv_txt_utf8_size(uint8_t c);


/**
 * Convert an Unicode letter to UTF-8.
 * @param letter_uni an Unicode letter
 * @return UTF-8 coded character in Little Endian to be compatible with C chars (e.g. 'Á', 'Ű')
 */
uint32_t lv_txt_unicode_to_utf8(uint32_t letter_uni);

/**
 * Decode an UTF-8 character from a string.
 * @param txt pointer to '\0' terminated string
 * @param i start index in 'txt' where to start.
 *                After the call it will point to the next UTF-8 char in 'txt'.
 *                NULL to use txt[0] as index
 * @return the decoded Unicode character or 0 on invalid UTF-8 code
 */
uint32_t lv_txt_utf8_next(const char * txt, uint32_t * i);

/**
 * Get previous UTF-8 character form a string.
 * @param txt pointer to '\0' terminated string
 * @param i_start index in 'txt' where to start. After the call it will point to the next UTF-8 char in 'txt'.
 * @return the decoded Unicode character or 0 on invalid UTF-8 code
 */
uint32_t lv_txt_utf8_prev(const char * txt, uint32_t * i_start);

/**
 * Convert a letter index (in an UTF-8 text) to byte index.
 * E.g. in "AÁRT" index of 'R' is 2 but start at byte 3 because 'Á' is 2 bytes long
 * @param txt a '\0' terminated UTF-8 string
 * @param utf8_id letter index
 * @return byte index of the 'utf8_id'th letter
 */
uint32_t txt_utf8_get_byte_id(const char * txt, uint32_t utf8_id);

/**
 * Convert a byte index (in an UTF-8 text) to character index.
 * E.g. in "AÁRT" index of 'R' is 2 but start at byte 3 because 'Á' is 2 bytes long
 * @param txt a '\0' terminated UTF-8 string
 * @param byte_id byte index
 * @return character index of the letter at 'byte_id'th position
 */
uint32_t lv_txt_utf8_get_char_id(const char * txt, uint32_t byte_id);

/**
 * Get the number of characters (and NOT bytes) in a string. Decode it with UTF-8 if enabled.
 * E.g.: "ÁBC" is 3 characters (but 4 bytes)
 * @param txt a '\0' terminated char string
 * @return number of characters
 */
uint32_t lv_txt_get_length(const char * txt);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*USE_TXT*/
