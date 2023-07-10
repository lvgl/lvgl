/**
 * @file lv_stringn.h
 *
 */

#ifndef LV_STRING_H
#define LV_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include <stdint.h>
#include <stddef.h>

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
 * @brief Copies a block of memory from a source address to a destination address.
 * @param dst Pointer to the destination array where the content is to be copied.
 * @param src Pointer to the source of data to be copied.
 * @param len Number of bytes to copy.
 * @return Pointer to the destination array.
 * @note The function does not check for any overlapping of the source and destination memory blocks.
 */
void * lv_memcpy(void * dst, const void * src, size_t len);

/**
 * @brief Fills a block of memory with a specified value.
 * @param dst Pointer to the destination array to fill with the specified value.
 * @param v Value to be set. The value is passed as an int, but the function fills
 *          the block of memory using the unsigned char conversion of this value.
 * @param len Number of bytes to be set to the value.
 */
void lv_memset(void * dst, uint8_t v, size_t len);

/**
 * Same as `memset(dst, 0x00, len)`.
 * @param dst pointer to the destination buffer
 * @param len number of byte to set
 */
static inline void lv_memzero(void * dst, size_t len)
{
    lv_memset(dst, 0x00, len);
}

/**
 * @brief Computes the length of the string str up to, but not including the terminating null character.
 * @param str Pointer to the null-terminated byte string to be examined.
 * @return The length of the string in bytes.
 */
size_t lv_strlen(const char * str);

/**
 * @brief Copies up to dest_size characters from the string pointed to by src to the character array pointed to by dst.
 * @param dst Pointer to the destination array where the content is to be copied.
 * @param src Pointer to the source of data to be copied.
 * @param dest_size Maximum number of characters to be copied to dst, including the null character.
 * @return A pointer to the destination array, which is dst.
 */
char * lv_strncpy(char * dst, const char * src, size_t dest_size);

/**
 * @brief Copies the string pointed to by src, including the terminating null character,
 *        to the character array pointed to by dst.
 * @param dst Pointer to the destination array where the content is to be copied.
 * @param src Pointer to the source of data to be copied.
 * @return A pointer to the destination array, which is dst.
 */
char * lv_strcpy(char * dst, const char * src);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_STRING_H*/
