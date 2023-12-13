/**
 * @file lv_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>

#include "lv_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * _lv_utils_bsearch(const void * key, const void * base, uint32_t n, uint32_t size,
                         int32_t (*cmp)(const void * pRef, const void * pElement))
{
    const char * middle;
    int32_t c;

    for(middle = base; n != 0;) {
        middle += (n / 2) * size;
        if((c = (*cmp)(key, middle)) > 0) {
            n    = (n / 2) - ((n & 1) == 0);
            base = (middle += size);
        }
        else if(c < 0) {
            n /= 2;
            middle = base;
        }
        else {
            return (char *)middle;
        }
    }
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
