/**
 * @file font.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "text.h"

/*********************
 *      DEFINES
 *********************/
#define TXT_NO_BREAK_FOUND  UINT16_MAX

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool txt_is_break_char(char letter);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Get the next line of text. Check line length and break chars too.
 * @param txt a '\0' terminated string
 * @param font_p pointer to a font
 * @param letter_space letter space
 * @param max_l max line length
 * @return the index of the first char of the new line
 */
uint16_t txt_get_next_line(const char * txt, const font_t * font_p, 
                           uint16_t letter_space, cord_t max_l)
{
    uint32_t i = 0;
    cord_t act_l = 0;
    uint16_t last_break = TXT_NO_BREAK_FOUND;
    
    while(txt[i] != '\0') {
        /*Check for new line chars*/
        if(txt[i] == '\n' || txt[i] == '\r') {
            /*Handle \n\r and \r\n as well*/
            if(txt[i] == '\n' && txt[i + 1] == '\r') {
                i++;
            } else if(txt[i] == '\r' && txt[i + 1] == '\n') {
                i++;
            }
            return i+1;    /*Return with the first letter of the next line*/

        } else { /*Check the actual length*/
            act_l += font_get_width(font_p, txt[i]);
            
            /*If the txt is too long then finish, this is the line end*/
            if(act_l > max_l) {
                /*If already a break character is found, then break there*/
                if(last_break != TXT_NO_BREAK_FOUND) {
                    i = last_break;
                }

                while(txt[i] == ' ') i++;

                return i;
            }
            /*If this char still can fit to this line then check if 
             * txt can be broken here later */
            else if(txt_is_break_char(txt[i])) {
                last_break = i;
                last_break++;/*Go to the next char, the break char stays in this line*/
            }
        }
        
        act_l += letter_space;
        i++;
    }
    
    return i;
}

/**
 * Give the length of a text with a given font
 * @param txt a '\0' terminate string
 * @param char_num number of characters in 'txt'
 * @param font_p pointer to a font
 * @param letter_space letter sapce
 * @return length of a char_num long text
 */
cord_t txt_get_width(const char * txt, uint16_t char_num, 
                      const font_t * font_p, uint16_t letter_space)
{
    uint16_t i;
    cord_t len = 0;
    
    if(char_num != 0) {
        for(i = 0; i < char_num; i++) {
            len += font_get_width(font_p, txt[i]);
            len += letter_space;
        }
        
        /*Trim closing spaces */
        for(i = char_num - 1; i > 0; i--) {
            if(txt[i] == ' ') {
                len -= font_get_width(font_p, txt[i]);
                len -= letter_space;
            } else {
                break;
            }
        }
        
        /*Correct the last letter space, 
         * because thee is no letter space after the last char*/
        len -= letter_space;
    }
    
    return len;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Test if char is break char or not (a text can broken here or not)
 * @param letter a letter
 * @return false: 'letter' is not break char
 */
static bool txt_is_break_char(char letter)
{
    uint8_t i;
    bool ret = false;
    
    /*Compare the letter to TXT_BREAK_CHARS*/
    for(i = 0; LV_TXT_BREAK_CHARS[i] != '\0'; i++) {
        if(letter == LV_TXT_BREAK_CHARS[i]) {
            ret = true; /*If match then it is break char*/
            break;
        }
    }
    
    return ret;
}
