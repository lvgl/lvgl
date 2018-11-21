/**
 * @file lv_templ.h
 *
 */

#ifndef LV_PERSIAN_H
#define LV_PERSIAN_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl.h"
#if LV_TXT_RTL

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
	uint16_t letter_code;
	uint16_t alone_code;
	uint16_t prefix_code;
	uint16_t middle_code;
	uint16_t suffix_code;
	bool connect_to_previous;
	bool connect_to_next;
} persian_map_t;

struct text_block_s {
	int start;
	int end;
	bool utf;
	struct text_block_s*next;
};
/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**
 * return an object from lookup table based on letter sent to function
 * @param letter_code the utf8 code
 * @return type of persian_map_t
 */
persian_map_t* lv_get_persian_letter(uint32_t letter_code);
uint8_t* lv_get_reversed_buffer(uint8_t*buffer, uint16_t start,
		uint16_t end);
/**
 * return the converted letter .In some languages like farsi and arabic based on the position of a letter in 
 * a word the shape of the letter will change . this function process the position of letter then return the result letter
 * @param pre_letter the previous letter in the current word
 * @param letter  current letter that shoul be processed
 * @param next_letter the next letter in the current word
 * @return the converted letter
 */
uint32_t lv_get_converted_persian_letter(uint32_t previous_letter, uint32_t letter,
        uint32_t next_letter);

#endif /*LV_TXT_RTL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_PERSIAN_H*/
