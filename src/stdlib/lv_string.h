/**
 * @file lv_string.h
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
#include "../misc/lv_types.h"

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
 * @brief Move a block of memory from source to destination
 * @param dst Pointer to the destination array where the content is to be copied.
 * @param src Pointer to the source of data to be copied.
 * @param len Number of bytes to copy
 * @return Pointer to the destination array.
 */
void * lv_memmove(void * dst, const void * src, size_t len);

/**
 * @brief This function will compare two memory blocks
 * @param p1 Pointer to the first memory block
 * @param p2 Pointer to the second memory block
 * @param len Number of bytes to compare
 * @return The difference between the value of the first unmatching byte.
 */
int lv_memcmp(const void * p1, const void * p2, size_t len);

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
 * @brief Computes the length of the string str up to (but not including) the terminating null character.
 * @param str Pointer to the null-terminated byte string to be examined.
 * @return The length of the string in bytes.
 */
size_t lv_strlen(const char * str);

/**
 * @brief Computes the length of the string str up to (but not including) the terminating null character,
 *        or the given maximum length.
 * @param str Pointer to byte string that is null-terminated or at least max_len bytes long.
 * @param max_len Maximum number of characters to examine.
 * @return The length of the string in bytes.
 */
size_t lv_strnlen(const char * str, size_t max_len);

/**
 * @brief Copies up to dst_size-1 (non-null) characters from src to dst. A null terminator is always added.
 * @param dst Pointer to the destination array where the content is to be copied.
 * @param src Pointer to the source of data to be copied.
 * @param dst_size Maximum number of characters to be copied to dst, including the null character.
 * @return The length of src. The return value is equivalent to the value returned by lv_strlen(src)
 */
size_t lv_strlcpy(char * dst, const char * src, size_t dst_size);

/**
 * @brief Copies up to dest_size characters from the string pointed to by src to the character array pointed to by dst
 *        and fills the remaining length with null bytes.
 * @param dst Pointer to the destination array where the content is to be copied.
 * @param src Pointer to the source of data to be copied.
 * @param dest_size Maximum number of characters to be copied to dst.
 * @return A pointer to the destination array, which is dst.
 * @note dst will not be null terminated if dest_size bytes were copied from src before the end of src was reached.
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

/**
 * @brief  This function will compare two strings without specified length.
 * @param s1    pointer to the first string
 * @param s2    pointer to the second string
 * @return      the difference between the value of the first unmatching character.
 */
int lv_strcmp(const char * s1, const char * s2);

/**
 * @brief  This function will compare two strings up to the given length.
 * @param s1    pointer to the first string
 * @param s2    pointer to the second string
 * @param len   the maximum amount of characters to compare
 * @return      the difference between the value of the first unmatching character.
 */
int lv_strncmp(const char * s1, const char * s2, size_t len);

/** Returns true if the two strings are equal.
 * Just a wrapper around strcmp for convenience.
 * @param s1    pointer to the first string
 * @param s2    pointer to the second string
 * @return      true: the strings are equal; false: otherwise
 */
static inline bool lv_streq(const char * s1, const char * s2)
{
    return lv_strcmp(s1, s2) == 0;
}

/**
 * @brief Duplicate a string by allocating a new one and copying the content.
 * @param src Pointer to the source of data to be copied.
 * @return A pointer to the new allocated string. NULL if failed.
 */
char * lv_strdup(const char * src);

/**
 * @brief Duplicate a string by allocating a new one and copying the content
 *        up to the end or the specified maximum length, whichever comes first.
 * @param src Pointer to the source of data to be copied.
 * @param max_len Maximum number of characters to be copied.
 * @return Pointer to a newly allocated null-terminated string. NULL if failed.
 */
char * lv_strndup(const char * src, size_t max_len);

/**
 * @brief Copies the string pointed to by src, including the terminating null character,
 *        to the end of the string pointed to by dst.
 * @param dst Pointer to the destination string where the content is to be appended.
 * @param src Pointer to the source of data to be copied.
 * @return A pointer to the destination string, which is dst.
 */
char * lv_strcat(char * dst, const char * src);

/**
 * @brief Copies up to src_len characters from the string pointed to by src
 *        to the end of the string pointed to by dst.
 *        A terminating null character is appended to dst even if no null character
 *        was encountered in src after src_len characters were copied.
 * @param dst Pointer to the destination string where the content is to be appended.
 * @param src Pointer to the source of data to be copied.
 * @param src_len Maximum number of characters from src to be copied to the end of dst.
 * @return A pointer to the destination string, which is dst.
 */
char * lv_strncat(char * dst, const char * src, size_t src_len);

/**
 * @brief Searches for the first occurrence of character c in the string str.
 * @param str Pointer to the null-terminated byte string to be searched.
 * @param c The character to be searched for.
 * @return A pointer to the first occurrence of character c in the string str, or a null pointer if c is not found.
 */
char * lv_strchr(const char * str, int c);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_STRING_H*/
