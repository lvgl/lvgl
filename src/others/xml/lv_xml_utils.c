/**
 * @file lv_xml_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_utils.h"
#include "../../stdlib/lv_string.h"
#if LV_USE_XML

#if LV_USE_STDLIB_STRING == LV_STDLIB_CLIB
    #include <stdlib.h>
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_USE_STDLIB_STRING != LV_STDLIB_CLIB
    static bool lv_xml_is_digit(char c, int base);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


const char * lv_xml_get_value_of(const char ** attrs, const char * name)
{
    if(attrs == NULL) return NULL;
    if(name == NULL) return NULL;

    for(int i = 0; attrs[i]; i += 2) {
        if(lv_streq(attrs[i], name)) return attrs[i + 1];
    }

    return NULL;
}

lv_color_t lv_xml_to_color(const char * str)
{
    return lv_color_hex(lv_xml_strtol(str, NULL, 16));
}

bool lv_xml_to_bool(const char * str)
{
    return lv_streq(str, "false") ? false : true;
}


#if LV_USE_STDLIB_STRING == LV_STDLIB_CLIB
int32_t lv_xml_atoi(const char * str)
{
    return atoi(str);
}


int32_t lv_xml_strtol(const char * str, char ** endptr, int32_t base)
{
    return strtol(str, endptr, base);
}

#else /*LV_USE_STDLIB_STRING == LV_STDLIB_CLIB*/

int32_t lv_xml_atoi(const char * str)
{
    const char * s = str;
    int32_t result = 0;
    int sign = 1;

    /* Skip leading whitespace */
    while(*s == ' ' || *s == '\t')  s++;

    /* Handle optional sign */
    if(*s == '-') {
        sign = -1;
        s++;
    }
    else if(*s == '+') {
        s++;
    }

    /* Convert the string*/
    while(*s) {
        if(*s >= '0' && *s <= '9') {
            int32_t digit = *s - '0';

            /* Check for overflow before it happens */
            if(result > (INT_MAX - digit) / 10) {
                return (sign == 1) ? INT_MAX : INT_MIN; // Return limits on overflow
            }

            result = result * 10 + digit;
            s++;
        }
        else {
            break; // Non-digit character
        }
    }

    return result * sign;
}

int32_t lv_xml_strtol(const char * str, char ** endptr, int32_t base)
{
    const char * s = str;
    int32_t result = 0;
    int32_t sign = 1;

    /* Skip leading whitespace */
    while(*s == ' ' || *s == '\t') s++;

    /* Handle optional sign*/
    if(*s == '-') {
        sign = -1;
        s++;
    }
    else if(*s == '+') {
        s++;
    }

    /* Determine base if 0 is passed as base*/
    if(base == 0) {
        if(*s == '0') {
            if(*(s + 1) == 'x' || *(s + 1) == 'X') {
                base = 16;
                s += 2;
            }
            else {
                base = 8;
                s++;
            }
        }
        else {
            base = 10;
        }
    }

    /* Convert the string*/
    while(*s) {
        int32_t digit;

        if(lv_xml_is_digit(*s, base)) {
            if(*s >= '0' && *s <= '9') {
                digit = *s - '0';
            }
            else if(*s >= 'a' && *s <= 'f') {
                digit = *s - 'a' + 10;
            }
            else if(*s >= 'A' && *s <= 'F') {
                digit = *s - 'A' + 10;
            }
            else {
                /* This should not happen due to lv_xml_is_digit check*/
                break;
            }

            /* Check for overflow */
            if(result > (INT32_MAX - digit) / base) {
                result = (sign == 1) ? INT32_MAX : INT32_MIN;
                if(endptr) *endptr = (char *)s;
                return result;
            }

            result = result * base + digit;
        }
        s++;
    }

    /* Set end pointer to the last character processed*/
    if(endptr) {
        *endptr = (char *)s;
    }

    return result * sign;
}

#endif /*LV_USE_STDLIB_STRING == LV_STDLIB_CLIB*/

char * lv_xml_split_str(char ** src, char delimiter)
{
    if(*src[0] == '\0') return NULL;

    char * src_first = *src;
    char * src_next = *src;

    /*Find the delimiter*/
    while(*src_next != '\0') {
        if(*src_next == delimiter) {
            *src_next = '\0';       /*Close the string on the delimiter*/
            *src = src_next + 1;    /*Change the source continue after the found delimiter*/
            return src_first;
        }
        src_next++;
    }

    /*No delimiter found, return the string as it is*/
    *src = src_next;    /*Move the source point to the end*/

    return src_first;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_USE_STDLIB_STRING != LV_STDLIB_CLIB
static bool lv_xml_is_digit(char c, int base)
{
    if(base <= 10) {
        return (c >= '0' && c < '0' + base);
    }
    else {
        return (c >= '0' && c <= '9') || (c >= 'a' && c < 'a' + (base - 10)) || (c >= 'A' && c < 'A' + (base - 10));
    }
}

#endif /*LV_USE_STDLIB_STRING == LV_STDLIB_CLIB*/

#endif /* LV_USE_XML */
