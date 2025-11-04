/**
 * @file lv_xml_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_utils.h"
#include "../stdlib/lv_string.h"
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
static bool is_digit(char c, int base);

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
    /*fff, #fff, 0xfff*/
    if(lv_strlen(str) <= 5) return lv_color_hex3(lv_xml_strtol(str, NULL, 16));
    /*ffffff, #ffffff, 0xffffff*/
    else return lv_color_hex(lv_xml_strtol(str, NULL, 16));
}

lv_opa_t lv_xml_to_opa(const char * str)
{
    int32_t v = lv_xml_atoi(str);
    size_t len = lv_strlen(str);
    if(str[len - 1] == '%') {
        v = v * 255 / 100;
    }

    v = LV_CLAMP(0, v, 255);
    return (lv_opa_t)v;
}

bool lv_xml_to_bool(const char * str)
{
    return lv_streq(str, "false") ? false : true;
}

int32_t lv_xml_atoi_split(const char ** str, char delimiter)
{
    const char * s = *str;
    int32_t result = 0;
    int sign = 1;

    /* Skip leading whitespace and repeated delimiters */
    while(*s == delimiter || *s == ' ' || *s == '\t') s++;

    /* Handle optional sign */
    if(*s == '-') {
        sign = -1;
        s++;
    }
    else if(*s == '+') {
        s++;
    }

    /* Convert the string*/
    while(*s != delimiter) {
        if(*s >= '0' && *s <= '9') {
            int32_t digit = *s - '0';

            result = result * 10 + digit;
            s++;
        }
        else {
            break; /* Non-digit character */
        }
    }

    result = result * sign;
    while(*s != delimiter && *s != '\0') s++; /*Make sure to find the delimiter*/

    if(*s != '\0') s++; /*Skip the delimiter*/
    *str = s;
    return result;
}

int32_t lv_xml_atoi(const char * str)
{
    return lv_xml_atoi_split(&str, '\0');
}

#if LV_USE_FLOAT
float lv_xml_atof_split(const char ** str, char delimiter)
{
    const char * s = *str;
    float result = 0.0f;
    int sign = 1;

    /* Skip leading whitespace and repeated delimiters */
    while(*s == delimiter || *s == ' ' || *s == '\t') s++;

    /* Handle optional sign */
    if(*s == '-') {
        sign = -1;
        s++;
    }
    else if(*s == '+') {
        s++;
    }

    /* Convert the integer part */
    while(*s != delimiter && *s != '.' && *s != '\0') {
        if(*s >= '0' && *s <= '9') {
            float digit = *s - '0';
            result = result * 10.0f + digit;
            s++;
        }
        else {
            break; /* Non-digit character */
        }
    }

    /* Convert the fractional part */
    if(*s == '.') {
        s++; /* Skip the decimal point */
        float fraction = 0.0f;
        float divisor = 10.0f;

        while(*s != delimiter && *s != '\0') {
            if(*s >= '0' && *s <= '9') {
                float digit = *s - '0';
                fraction += digit / divisor;
                divisor *= 10.0f;
                s++;
            }
            else {
                break; /* Non-digit character */
            }
        }
        result += fraction;
    }

    result = result * sign;
    while(*s != delimiter && *s != '\0') s++; /*Make sure to find the delimiter*/

    if(*s != '\0') s++; /*Skip the delimiter*/
    *str = s;
    return result;
}


float lv_xml_atof(const char * str)
{
    return lv_xml_atof_split(&str, '\0');
}
#endif

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

        if(is_digit(*s, base)) {
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
                /* This should not happen due to is_digit check*/
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

char * lv_xml_split_str(char ** src, char delimiter)
{
    /*Skip multiple delimiters*/
    while(*src[0] == delimiter) {
        (*src)++;
    }

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

static bool is_digit(char c, int base)
{
    if(base <= 10) {
        return (c >= '0' && c < '0' + base);
    }
    else {
        return (c >= '0' && c <= '9') || (c >= 'a' && c < 'a' + (base - 10)) || (c >= 'A' && c < 'A' + (base - 10));
    }
}


#endif /* LV_USE_XML */
