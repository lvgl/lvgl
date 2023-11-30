/**
 * @file lv_svg_parser.h
 *
 */

#ifndef LV_SVG_PARSER_H
#define LV_SVG_PARSER_H

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_SVG

#include "lv_svg.h"
#include "lv_svg_token.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_SVG_PARSER_PROCESS = 0,
    LV_SVG_PARSER_IGNORE,
} _lv_svg_parser_state_t;

typedef struct {
    uint16_t state;
    char * ignore_name;
    uint32_t ignore_len;
    int32_t dpi;
    lv_svg_node_t * doc_root;
    lv_svg_node_t * cur_node;
} _lv_svg_parser_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void _lv_svg_parser_init(_lv_svg_parser_t * parser);

void _lv_svg_parser_deinit(_lv_svg_parser_t * parser);

bool _lv_svg_parser_token(_lv_svg_parser_t * parser, const _lv_svg_token_t * token);

bool _lv_svg_parser_is_finish(_lv_svg_parser_t * parser);
/**********************
 *      MACROS
 **********************/
void _lv_svg_dump_tree(lv_svg_node_t * root, int depth);
#endif /*LV_USE_SVG*/

#endif /*LV_SVG_PARSER_H*/
