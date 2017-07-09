/**
 * @file lv_app_terminal.h
 *
 */

#ifndef LV_APP_TERMINAL_H
#define LV_APP_TERMINAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lv_app/lv_app.h"

#if LV_APP_ENABLE != 0 && USE_LV_APP_TERMINAL != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
    LV_APP_TERMINAL_FORMAT_ASCII,
    LV_APP_TERMINAL_FORMAT_HEX,
}lv_app_terminal_format_t;

typedef struct
{
    lv_app_terminal_format_t format;    /*Data display format*/
    lv_app_com_type_t com_type;          /*The listened communication type (channel) */
}lv_app_terminal_conf_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
const lv_app_dsc_t * lv_app_terminal_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_APP_ENABLE != 0 && USE_LV_APP_TERMINAL != 0*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_APP_TERMINAL_H */
