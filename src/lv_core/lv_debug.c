/**
 * @file lv_debug.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "lv_debug.h"

#if LV_USE_DEBUG

/*********************
 *      DEFINES
 *********************/
#ifndef LV_DEBUG_STR_MAX_LENGTH
#define LV_DEBUG_STR_MAX_LENGTH  (1024 * 8)
#endif

#ifndef LV_DEBUG_STR_MAX_REPEAT
#define LV_DEBUG_STR_MAX_REPEAT  8
#endif
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool obj_valid_child(const lv_obj_t * parent, const  lv_obj_t * obj_to_find);

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

bool lv_debug_check_obj_type(const lv_obj_t * obj, const char * obj_type)
{
    if(obj_type[0] == '\0') return true;

    lv_obj_type_t types;
    lv_obj_get_type((lv_obj_t *)obj, &types);

    uint8_t i;
    for(i = 0; i < LV_MAX_ANCESTOR_NUM; i++) {
        if(strcmp(types.type[i], obj_type) == 0) return true;
    }

    return false;
}

bool lv_debug_check_obj_valid(const lv_obj_t * obj)
{
    lv_disp_t * disp = lv_disp_get_next(NULL);
    while(disp) {
        lv_obj_t * scr;
        LV_LL_READ(disp->scr_ll, scr) {

            if(scr == obj) return true;
            bool found = obj_valid_child(scr, obj);
            if(found) return true;
        }

        disp = lv_disp_get_next(disp);
    }

    return false;
}

bool lv_debug_check_style(const lv_style_t * style)
{
    if(style == NULL) return true;  /*NULL style is still valid*/

#if LV_USE_ASSERT_STYLE
    if(style->debug_sentinel != LV_STYLE_DEGUG_SENTINEL_VALUE) {
        LV_LOG_WARN("Invalid style (local variable or not initialized?)");
        return false;
    }
#endif

    return true;
}

bool lv_debug_check_str(const void * str)
{
    const uint8_t * s = (const uint8_t *)str;
    uint8_t last_byte = 0;
    uint32_t rep = 0;
    uint32_t i;

    for(i = 0; s[i] != '\0' && i < LV_DEBUG_STR_MAX_LENGTH; i++) {
        if(s[i] != last_byte) {
            last_byte = s[i];
            rep = 1;
        } else if(s[i] > 0x7F){
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
        memcpy(bufp, msg, msg_len);
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
    } else {
        LV_LOG_ERROR(msg);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool obj_valid_child(const lv_obj_t * parent, const lv_obj_t * obj_to_find)
{
    /*Check all children of `parent`*/
    lv_obj_t * child;
    LV_LL_READ(parent->child_ll, child) {
        if(child == obj_to_find) return true;

        /*Check the children*/
        bool found = obj_valid_child(child, obj_to_find);
        if(found) return true;
    }

    return false;
}

#endif /*LV_USE_DEBUG*/

