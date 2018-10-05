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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

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
enum
{
    LV_TXT_FLAG_NONE =     0x00,
    LV_TXT_FLAG_RECOLOR =  0x01,   /*Enable parsing of recolor command*/
    LV_TXT_FLAG_EXPAND =   0x02,   /*Ignore width to avoid automatic word wrapping*/
    LV_TXT_FLAG_CENTER =   0x04,   /*Align the text to the middle*/
    LV_TXT_FLAG_RIGHT  =   0x08,   /*Align the text to the right*/
};
typedef uint8_t lv_txt_flag_t;

enum
{
    LV_TXT_CMD_STATE_WAIT,      /*Waiting for command*/
    LV_TXT_CMD_STATE_PAR,       /*Processing the parameter*/
    LV_TXT_CMD_STATE_IN,        /*Processing the command*/
};
typedef uint8_t lv_txt_cmd_state_t;

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
 * @param length length of 'txt' in character count and not bytes(UTF-8 can be 1,2,3 or 4 bytes long)
 * @param font pointer to a font
 * @param letter_space letter space
 * @param flags settings for the text from 'txt_flag_t' enum
 * @return length of a char_num long text
 */
lv_coord_t lv_txt_get_width(const char * txt, uint16_t length,
                            const lv_font_t * font, lv_coord_t letter_space, lv_txt_flag_t flag);


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

/***************************************************************
 *  GLOBAL FUNCTION POINTERS FOR CAHRACTER ENCODING INTERFACE
 ***************************************************************/

/**
 * Give the size of an encoded  character
 * @param str pointer to a character in a string
 * @return length of the encoded character (1,2,3 ...). O in invalid
 */
extern uint8_t (*lv_txt_encoded_size)(const char *);


/**
 * Convert an Unicode letter to encoded
 * @param letter_uni an Unicode letter
 * @return Encoded character in Little Endian to be compatible with C chars (e.g. 'Á', 'Ü')
 */
extern uint32_t (*lv_txt_unicode_to_encoded)(uint32_t );

/**
 * Convert a wide character, e.g. 'Á' little endian to be compatible with the encoded format.
 * @param c a wide character
 * @return `c` in the encoded format
 */
extern uint32_t (*lv_txt_encoded_conv_wc) (uint32_t c);

/**
 * Decode the next encoded character from a string.
 * @param txt pointer to '\0' terminated string
 * @param i start index in 'txt' where to start.
 *                After the call it will point to the next encoded char in 'txt'.
 *                NULL to use txt[0] as index
 * @return the decoded Unicode character or 0 on invalid data code
 */
extern uint32_t (*lv_txt_encoded_next)(const char *, uint32_t * );

/**
 * Get the previous encoded character form a string.
 * @param txt pointer to '\0' terminated string
 * @param i_start index in 'txt' where to start. After the call it will point to the previous encoded char in 'txt'.
 * @return the decoded Unicode character or 0 on invalid data
 */
extern uint32_t (*lv_txt_encoded_prev)(const char *, uint32_t *);

/**
 * Convert a letter index (in an the encoded text) to byte index.
 * E.g. in UTF-8 "AÁRT" index of 'R' is 2 but start at byte 3 because 'Á' is 2 bytes long
 * @param txt a '\0' terminated UTF-8 string
 * @param enc_id letter index
 * @return byte index of the 'enc_id'th letter
 */
extern uint32_t (*lv_txt_encoded_get_byte_id)(const char *, uint32_t);

/**
 * Convert a byte index (in an encoded text) to character index.
 * E.g. in UTF-8 "AÁRT" index of 'R' is 2 but start at byte 3 because 'Á' is 2 bytes long
 * @param txt a '\0' terminated UTF-8 string
 * @param byte_id byte index
 * @return character index of the letter at 'byte_id'th position
 */
extern uint32_t (*lv_encoded_get_char_id)(const char *, uint32_t);

/**
 * Get the number of characters (and NOT bytes) in a string.
 * E.g. in UTF-8 "ÁBC" is 3 characters (but 4 bytes)
 * @param txt a '\0' terminated char string
 * @return number of characters
 */
extern uint32_t (*lv_txt_get_encoded_length)(const char *);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*USE_TXT*/
