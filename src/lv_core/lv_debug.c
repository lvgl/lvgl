/**
 * @file lv_debug.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool obj_valid_child(lv_obj_t * parent, lv_obj_t * obj_to_find);

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

bool lv_debug_check_obj_type(lv_obj_t * obj, const char * obj_type)
{
    lv_obj_type_t types;
    lv_obj_get_type(obj, &types);

    uint8_t i;
    for(i = 0; i < LV_MAX_ANCESTOR_NUM; i++) {
        if(strcmp(types.type[i], obj_type) == 0) return true;
    }

    return false;
}

bool lv_debug_check_obj_valid(lv_obj_t * obj)
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

bool lv_debug_check_malloc(void * p)
{
    if(p) return true;

    return false;
}

void lv_debug_log_error(const char * msg, unsigned long int  value)
{
    static const char hex[] = "0123456789ABCDEF";

    uint32_t msg_len = strlen(msg);
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

static bool obj_valid_child(lv_obj_t * parent, lv_obj_t * obj_to_find)
{
    /*Check all children of `parent`*/
    lv_obj_t * child = lv_obj_get_child(parent, NULL);
    while(child) {
        if(child == obj_to_find) return true;

        /*Check the children*/
        bool found = obj_valid_child(child, obj_to_find);
        if(found) return true;

        child = lv_obj_get_child(parent, child);
    }

    return false;
}
