/**
 * @file lv_str.h
 *
 */

#ifndef LV_STR_H
#define LV_STR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include <stdint.h>
#include <stddef.h>

#include "lv_types.h"

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
 * Same as the standard `strncpy` but it always null-terminates the destination string
 * @param dst pointer to the character array to copy to
 * @param src pointer to the character array to copy from
 * @param count maximum number of character to copy (ie. the size of `dst`)
 * @return returns a copy of dest
 */
char * lv_strncpy(char * dst, const char * src, size_t count);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_STR_H*/
