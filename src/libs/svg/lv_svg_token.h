/**
 * @file lv_svg_token.h
 *
 */

#ifndef LV_SVG_TOKEN_H
#define LV_SVG_TOKEN_H

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_SVG

#include "../../misc/lv_array.h"

/*********************
 *      DEFINES
 *********************/

#define SVG_TOKEN_LEN(t) ((t)->end - (t)->start)

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_SVG_TOKEN_BEGIN = 0,
    LV_SVG_TOKEN_END,
    LV_SVG_TOKEN_CONTENT,
} _lv_svg_token_type_t;

typedef struct {
    const char * name_start;
    const char * name_end;
    const char * value_start;
    const char * value_end;
} _lv_svg_token_attr_t;

typedef struct {
    const char * start;
    const char * end;
    _lv_svg_token_type_t type;
    bool flat;
    _lv_svg_token_attr_t * cur_attr;
    lv_array_t attrs;
} _lv_svg_token_t;

typedef bool (*svg_token_process)(_lv_svg_token_t * token, void * user_data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Parse SVG data and call a callback for each token
 * @param svg_data pointer to SVG data
 * @param len length of the SVG data
 * @param cb callback function to be called for each token
 * @param user_data custom data to be passed to the callback function
 * @return true: SVG data successfully parsed, false: error occurred
 */
bool _lv_svg_tokenizer(const char * svg_data, uint32_t len, svg_token_process cb, void * user_data);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_SVG*/

#endif /*LV_SVG_TOKEN_H*/
