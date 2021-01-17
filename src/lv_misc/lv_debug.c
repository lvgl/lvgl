/**
 * @file lv_debug.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_debug.h"

#if LV_USE_DEBUG

#include "lv_mem.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#ifndef LV_DEBUG_STR_MAX_LENGTH
    #define LV_DEBUG_STR_MAX_LENGTH (1024 * 8)
#endif

#ifndef LV_DEBUG_STR_MAX_REPEAT
    #define LV_DEBUG_STR_MAX_REPEAT 8
#endif
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

bool lv_debug_check_null(const void * p)
{
    if(p) return true;

    return false;
}

bool lv_debug_check_mem_integrity(void)
{
    return lv_mem_test() == LV_RES_OK ? true : false;
}

bool lv_debug_check_str(const void * str)
{
    const uint8_t * s = (const uint8_t *)str;
    uint8_t last_byte = 0;
    uint32_t rep = 0;
    uint32_t i;

    for(i = 0; i < LV_DEBUG_STR_MAX_LENGTH && s[i] != '\0'; i++) {
        if(s[i] != last_byte) {
            last_byte = s[i];
            rep = 1;
        }
        else if(s[i] > 0x7F) {
            rep++;
            if(rep > LV_DEBUG_STR_MAX_REPEAT) {
                LV_LOG_WARN("lv_debug_check_str: a non-ASCII char has repeated more than LV_DEBUG_STR_MAX_REPEAT times)");
                return false;
            }
        }

        if(s[i] < 10) {
            LV_LOG_WARN("lv_debug_check_str: invalid char in the string (< 10 value)");
            return false;   /*Shouldn't occur in strings*/
        }
    }

    if(s[i] == '\0') return true;

    LV_LOG_WARN("lv_debug_check_str: string is longer than LV_DEBUG_STR_MAX_LENGTH");
    return false;
}

void lv_debug_log_error(const char * msg, uint64_t value)
{
    static const char hex[] = "0123456789ABCDEF";

    size_t msg_len = strlen(msg);
    uint32_t value_len = sizeof(unsigned long int);

    if(msg_len < 230) {
        char buf[255];
        char * bufp = buf;

        /*Add the function name*/
        _lv_memcpy(bufp, msg, msg_len);
        bufp += msg_len;

        /*Add value in hey*/
        *bufp = ' ';
        bufp ++;
        *bufp = '(';
        bufp ++;
        *bufp = '0';
        bufp ++;
        *bufp = 'x';
        bufp ++;

        int8_t i;
        for(i = value_len * 2 - 1; i >= 0; i--) {
            uint8_t x = (unsigned long int)((unsigned long int)value >> (i * 4)) & 0xF;

            *bufp = hex[x];
            bufp++;
        }

        *bufp = ')';
        bufp ++;

        *bufp = '\0';
        LV_LOG_ERROR(buf);
    }
    else {
        LV_LOG_ERROR(msg);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DEBUG*/
