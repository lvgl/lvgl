/**
 * @file font.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_TEXT != 0

#include "lv_text.h"
#include "lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define TXT_NO_BREAK_FOUND  UINT32_MAX

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool txt_is_break_char(uint32_t letter);

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
 * Get size of a text
 * @param size_res pointer to a 'point_t' variable to store the result
 * @param text pointer to a text
 * @param font pinter to font of the text
 * @param letter_space letter space of the text
 * @param txt.line_space line space of the text
 * @param flags settings for the text from 'txt_flag_t' enum
 * @param max_width max with of the text (break the lines to fit this size) Set CORD_MAX to avoid line breaks
 */
void txt_get_size(point_t * size_res, const char * text, const font_t * font,
                    cord_t letter_space, cord_t line_space, cord_t max_width, txt_flag_t flag)
{
    size_res->x = 0;
    size_res->y = 0;

    if(text == NULL) return;
    if(font == NULL) return;

    if(flag & TXT_FLAG_EXPAND) max_width = CORD_MAX;

    uint32_t line_start = 0;
    uint32_t new_line_start = 0;
    cord_t act_line_length;
    uint8_t letter_height = font_get_height_scale(font);

    /*Calc. the height and longest line*/
    while (text[line_start] != '\0') {
        new_line_start += txt_get_next_line(&text[line_start], font, letter_space, max_width, flag);
        size_res->y += letter_height ;
        size_res->y += line_space;

        /*Calculate the the longest line*/
        act_line_length = txt_get_width(&text[line_start], new_line_start - line_start,
                                       font, letter_space, flag);

        size_res->x = MATH_MAX(act_line_length, size_res->x);
        line_start = new_line_start;
    }

    if(line_start != 0 && (text[line_start - 1] == '\n' || text[line_start - 1] == '\r')) {
        size_res->y += letter_height + line_space;
    }

    /*Correction with the last line space or set the height manually if the text is empty*/
    if(size_res->y == 0) size_res->y = letter_height;
    else size_res->y -= line_space;

}

/**
 * Get the next line of text. Check line length and break chars too.
 * @param txt a '\0' terminated string
 * @param font pointer to a font
 * @param letter_space letter space
 * @param max_width max with of the text (break the lines to fit this size) Set CORD_MAX to avoid line breaks
 * @param flags settings for the text from 'txt_flag_type' enum
 * @return the index of the first char of the new line (in byte index not letter index. With UTF-8 they are different)
 */
uint16_t txt_get_next_line(const char * txt, const font_t * font,
                           cord_t letter_space, cord_t max_width, txt_flag_t flag)
{
    if(txt == NULL) return 0;
    if(font == NULL) return 0;

    if(flag & TXT_FLAG_EXPAND) max_width = CORD_MAX;

    uint32_t i = 0;
    cord_t act_l = 0;
    uint32_t last_break = TXT_NO_BREAK_FOUND;
    txt_cmd_state_t cmd_state = TXT_CMD_STATE_WAIT;
    uint32_t letter = 0;
    
    while(txt[i] != '\0') {
        letter = txt_utf8_next(txt, &i);

        /*Handle the recolor command*/
        if((flag & TXT_FLAG_RECOLOR) != 0) {
            if(txt_is_cmd(&cmd_state, letter) != false) {
                continue;   /*Skip the letter is it is part of a command*/
            }
        }
        /*Check for new line chars*/
        if((flag & TXT_FLAG_NO_BREAK) == 0 && (letter == '\n' || letter == '\r')) {
            /*Handle \r\n as well*/
            uint32_t i_tmp = i;
            uint32_t letter_next = txt_utf8_next(txt, &i_tmp);
            if(letter == '\r' &&  letter_next == '\n') i = i_tmp;

            return i;    /*Return with the first letter of the next line*/

        } else { /*Check the actual length*/
            act_l += font_get_width_scale(font, letter);

            /*If the txt is too long then finish, this is the line end*/
            if(act_l > max_width) {
                /*If this a break char then break here.*/
                if(txt_is_break_char(letter)) {
                    /* Now 'i' points to the next char because of txt_utf8_next()
                     * But we need the first char of the next line so keep it.
                     * Hence do nothing here*/
                }
                /*If already a break character is found, then break there*/
                else if(last_break != TXT_NO_BREAK_FOUND ) {
                    i = last_break;
                } else {
                    /* Now this character is out of the area but 'i' points to the next char because of txt_utf8_next()
                     * So step back one char*/
                    txt_utf8_prev(txt, &i);
                }

                /* Do not let to return without doing nothing.
                 * Find at least one character (Avoid infinite loop )*/
                if(i == 0) txt_utf8_next(txt, &i);

                return i;
            }
            /*If this char still can fit to this line then check if 
             * txt can be broken here later */
            else if(txt_is_break_char(letter)) {
                last_break = i; /*Save the first char index  after break*/
            }
        }
        
        act_l += letter_space;
    }
    
    return i;
}

/**
 * Give the length of a text with a given font
 * @param txt a '\0' terminate string
 * @param length length of 'txt' in bytes
 * @param font pointer to a font
 * @param letter_space letter space
 * @param flags settings for the text from 'txt_flag_t' enum
 * @return length of a char_num long text
 */
cord_t txt_get_width(const char * txt, uint16_t length, 
                      const font_t * font, cord_t letter_space, txt_flag_t flag)
{
    if(txt == NULL) return 0;
    if(font == NULL) return 0;

    uint32_t i = 0;
    cord_t width = 0;
    txt_cmd_state_t cmd_state = TXT_CMD_STATE_WAIT;
    uint32_t letter;
    
    if(length != 0) {
        while(i < length) {
            letter = txt_utf8_next(txt, &i);
            if((flag & TXT_FLAG_RECOLOR) != 0) {
                if(txt_is_cmd(&cmd_state, letter) != false) {
                    continue;
                }
            }
            width += font_get_width_scale(font, letter);
            width += letter_space;
        }
        
        /*Trim closing spaces. Important when the text is aligned to the middle */
        for(i = length - 1; i > 0; i--) {
            if(txt[i] == ' ') {
                width -= font_get_width_scale(font, txt[i]);
                width -= letter_space;
            } else {
                break;
            }
        }
    }
    
    return width;
}

/**
 * Check next character in a string and decide if the character is part of the command or not
 * @param state pointer to a txt_cmd_state_t variable which stores the current state of command processing
 *             (Initied. to TXT_CMD_STATE_WAIT )
 * @param c the current character
 * @return true: the character is part of a command and should not be written,
 *         false: the character should be written
 */
bool txt_is_cmd(txt_cmd_state_t * state, uint32_t c)
{
    bool ret = false;

    if(c == TXT_RECOLOR_CMD) {
       if(*state == TXT_CMD_STATE_WAIT) { /*Start char*/
           *state = TXT_CMD_STATE_PAR;
           ret = true;
       } else if(*state == TXT_CMD_STATE_PAR) { /*Other start char in parameter is escaped cmd. char */
           *state = TXT_CMD_STATE_WAIT;
       }else if(*state == TXT_CMD_STATE_IN) { /*Command end */
           *state = TXT_CMD_STATE_WAIT;
           ret = true;
       }
   }

   /*Skip the color parameter and wait the space after it*/
   if(*state == TXT_CMD_STATE_PAR) {
       if(c == ' ') {
           *state = TXT_CMD_STATE_IN; /*After the parameter the text is in the command*/
       }
       ret = true;
   }

   return ret;
}

/**
 * Insert a string into an other
 * @param txt_buf the original text (must be big enough for the result text)
 * @param pos position to insert. Expressed in character index and not byte index (Different in UTF-8)
 *            0: before the original text, 1: after the first char etc.
 * @param ins_txt text to insert
 */
void txt_ins(char * txt_buf, uint32_t pos, const char * ins_txt)
{
    uint32_t old_len = strlen(txt_buf);
    uint32_t ins_len = strlen(ins_txt);
    uint32_t new_len = ins_len + old_len;
#if TXT_UTF8 != 0
    pos = txt_utf8_get_byte_id(txt_buf, pos);   /*Convert to byte index instead of letter index*/
#endif
    /*Copy the second part into the end to make place to text to insert*/
    int32_t i;
    for(i = new_len; i >= pos + ins_len; i--){
        txt_buf[i] = txt_buf[i - ins_len];
    }

    /* Copy the text into the new space*/
    memcpy(txt_buf + pos, ins_txt, ins_len);
}

/**
 * Delete a part of a string
 * @param txt string to modify
 * @param pos position where to start the deleting (0: before the first char, 1: after the first char etc.)
 * @param len number of characters to delete
 */
void txt_cut(char * txt, uint32_t pos, uint32_t len)
{

    uint32_t old_len = strlen(txt);
#if TXT_UTF8 != 0
    pos = txt_utf8_get_byte_id(txt, pos);   /*Convert to byte index instead of letter index*/
    len = txt_utf8_get_byte_id(&txt[pos], len);
#endif

    /*Copy the second part into the end to make place to text to insert*/
    uint32_t i;
    for(i = pos; i <= old_len - len; i++){
        txt[i] = txt[i+len];
    }
}

/**
 * Give the size of an UTF-8 coded character
 * @param c A character where the UTF-8 character starts
 * @return length of the UTF-8 character (1,2,3 or 4). O on invalid code
 */
uint8_t txt_utf8_size(uint8_t c)
{
    if((c & 0b10000000) == 0) return 1;
    else if((c & 0b11100000) == 0b11000000) return 2;
    else if((c & 0b11110000) == 0b11100000) return 3;
    else if((c & 0b11111000) == 0b11110000) return 4;
    return 0;
}


/**
 * Convert an Unicode letter to UTF-8.
 * @param letter_uni an Unicode letter
 * @return UTF-8 coded character in Little Endian to be compatible with C chars (e.g. 'Á', 'Ű')
 */
uint32_t txt_unicode_to_utf8(uint32_t letter_uni)
{
    if(letter_uni < 128) return letter_uni;
    uint8_t bytes[4];

    if (letter_uni < 0x0800) {
        bytes[0] = ((letter_uni>>6)  & 0x1F) | 0xC0;
        bytes[1] = ((letter_uni>>0)  & 0x3F) | 0x80;
        bytes[2] = 0;
        bytes[3] = 0;
    }
    else if (letter_uni < 0x010000) {
        bytes[0] = ((letter_uni>>12) & 0x0F) | 0xE0;
        bytes[1] = ((letter_uni>>6)  & 0x3F) | 0x80;
        bytes[2] = ((letter_uni>>0)  & 0x3F) | 0x80;
        bytes[3] = 0;
    }
    else if (letter_uni < 0x110000) {
        bytes[0] = ((letter_uni>>18) & 0x07) | 0xF0;
        bytes[1] = ((letter_uni>>12) & 0x3F) | 0x80;
        bytes[2] = ((letter_uni>>6)  & 0x3F) | 0x80;
        bytes[3] = ((letter_uni>>0)  & 0x3F) | 0x80;
    }

    return *((uint32_t *) bytes);
}

/**
 * Decode an UTF-8 character from a string.
 * @param txt pointer to '\0' terminated string
 * @param i start index in 'txt' where to start.
 *                After the call it will point to the next UTF-8 char in 'txt'.
 *                NULL to use txt[0] as index
 * @return the decoded Unicode character or 0 on invalid UTF-8 code
 */
uint32_t txt_utf8_next(const char * txt, uint32_t * i)
{
#if TXT_UTF8 == 0
    if(i == NULL) return txt[1];    /*Get the next char */

    uint8_t letter = txt[*i] ;
    (*i)++;
    return letter;
#else
    /* Unicode to UTF-8
     * 00000000 00000000 00000000 0xxxxxxx -> 0xxxxxxx
     * 00000000 00000000 00000yyy yyxxxxxx -> 110yyyyy 10xxxxxx
     * 00000000 00000000 zzzzyyyy yyxxxxxx -> 1110zzzz 10yyyyyy 10xxxxxx
     * 00000000 000wwwzz zzzzyyyy yyxxxxxx -> 11110www 10zzzzzz 10yyyyyy 10xxxxxx
     * */

    uint32_t result = 0;

    /*Dummy 'i' pointer is required*/
    uint32_t i_tmp = 0;
    if(i == NULL) i = &i_tmp;

    /*Normal ASCII*/
    if((txt[*i] & 0x80) == 0) {
        result = txt[*i];
        (*i)++;
    }
    /*Real UTF-8 decode*/
    else {
        /*2 bytes UTF-8 code*/
        if((txt[*i] & 0b11100000) == 0b11000000) {
            result = (txt[*i] & 0b00011111) << 6;
            (*i)++;
            if((txt[*i] & 0b11000000) != 0b10000000) return 0;  /*Invalid UTF-8 code*/
            result += (txt[*i] & 0b00111111);
            (*i)++;
        }
        /*3 bytes UTF-8 code*/
        else if((txt[*i] & 0b11110000) == 0b11100000) {
            result = (txt[*i] & 0b00001111) << 12;
            (*i)++;

            if((txt[*i] & 0b11000000) != 0b10000000) return 0;  /*Invalid UTF-8 code*/
            result += (txt[*i] & 0b00111111) << 6;
            (*i)++;

            if((txt[*i] & 0b11000000) != 0b10000000) return 0;  /*Invalid UTF-8 code*/
            result += (txt[*i] & 0b00111111);
            (*i)++;
        }
        /*3 bytes UTF-8 code*/
        else if((txt[*i] & 0b11111000) == 0b11110000) {
            result = (txt[*i] & 0b00001111) << 18;
            (*i)++;

            if((txt[*i] & 0b11000000) != 0b10000000) return 0;  /*Invalid UTF-8 code*/
            result += (txt[*i] & 0b00111111) << 12;
            (*i)++;

            if((txt[*i] & 0b11000000) != 0b10000000) return 0;  /*Invalid UTF-8 code*/
            result += (txt[*i] & 0b00111111) << 6;
            (*i)++;

            if((txt[*i] & 0b11000000) != 0b10000000) return 0;  /*Invalid UTF-8 code*/
            result += (txt[*i] & 0b00111111) << 6;
            (*i)++;
        } else {
            (*i)++; /*Not UTF-8 char. Go the next.*/
        }
    }
    return result;
#endif
}

/**
 * Get previous UTF-8 character form a string.
 * @param txt pointer to '\0' terminated string
 * @param i_start index in 'txt' where to start. After the call it will point to the next UTF-8 char in 'txt'.
 * @return the decoded Unicode character or 0 on invalid UTF-8 code
 */
uint32_t txt_utf8_prev(const char * txt, uint32_t * i_start)
{
    uint8_t c_size;
    uint8_t cnt = 0;

    /*Try to find a !0 long UTF-8 char by stepping one character back*/
    (*i_start)--;
    do {
        if(cnt >= 4) return 0;      /*No UTF-8 char found before the initial*/

        c_size = txt_utf8_size(txt[*i_start]);
        if(c_size == 0) {
            if(*i_start != 0) (*i_start)--;
            else return 0;
        }
        cnt++;
    } while(c_size == 0);

    uint32_t i_tmp = *i_start;
    uint32_t letter = txt_utf8_next(txt, &i_tmp);   /*Character found, get it*/

    return letter;
}

/**
 * Convert a character index (in an UTF-8 text) to byte index.
 * E.g. in "AÁRT" index of 'R' is 2 but start at byte 3 because 'Á' is 2 bytes long
 * @param txt a '\0' terminated UTF-8 string
 * @param utf8_id letter index
 * @return byte index of the 'utf8_id'th letter
 */
uint32_t txt_utf8_get_byte_id(const char * txt, uint32_t utf8_id)
{
#if TXT_UTF8 == 0
    return utf8_id;     /*In Non UTF-8 no difference*/
#else
    uint32_t i;
    uint32_t byte_cnt = 0;
    for(i = 0; i < utf8_id; i++) {
        byte_cnt += txt_utf8_size(txt[byte_cnt]);
    }

    return byte_cnt;
#endif
}


/**
 * Convert a byte index (in an UTF-8 text) to character index.
 * E.g. in "AÁRT" index of 'R' is 2 but start at byte 3 because 'Á' is 2 bytes long
 * @param txt a '\0' terminated UTF-8 string
 * @param byte_id byte index
 * @return character index of the letter at 'byte_id'th position
 */
uint32_t txt_utf8_get_char_id(const char * txt, uint32_t byte_id)
{
#if TXT_UTF8 == 0
    return byte_id;     /*In Non UTF-8 no difference*/
#else
    uint32_t i = 0;
    uint32_t char_cnt = 0;

    while(i < byte_id) {
        txt_utf8_next(txt, &i); /*'i' points to the next letter so use the prev. value*/
        char_cnt++;
    }

    return char_cnt;
#endif

}

/**
 * Get the number of characters (and NOT bytes) in a string. Decode it with UTF-8 if enabled.
 * E.g.: "ÁBC" is 3 characters (but 4 bytes)
 * @param txt a '\0' terminated char string
 * @return number of characters
 */
uint32_t txt_len(const char * txt)
{
#if TXT_UTF8 == 0
    return strlen(txt);
#else
    uint32_t len = 0;
    uint32_t i = 0;

    while(txt[i] != '\0') {
        txt_utf8_next(txt, &i);
        len++;
    }

    return len;
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Test if char is break char or not (a text can broken here or not)
 * @param letter a letter
 * @return false: 'letter' is not break char
 */
static bool txt_is_break_char(uint32_t letter)
{
    uint8_t i;
    bool ret = false;
    
    /*Compare the letter to TXT_BREAK_CHARS*/
    for(i = 0; TXT_BREAK_CHARS[i] != '\0'; i++) {
        if(letter == TXT_BREAK_CHARS[i]) {
            ret = true; /*If match then it is break char*/
            break;
        }
    }
    
    return ret;
}

#endif /*USE_TEXT*/
