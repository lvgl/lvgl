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

/**
 * @brief Initialize the SVG parser
 * @param parser pointer to a parser object
 */
void _lv_svg_parser_init(_lv_svg_parser_t * parser);

/**
 * @brief Deinitialize the SVG parser
 * @param parser pointer to a parser object
 */
void _lv_svg_parser_deinit(_lv_svg_parser_t * parser);

/**
 * @brief Parse an SVG document
 * @param parser pointer to a parser object
 * @param token pointer to a token object
 * @return true: the parsing is finished, false: the parsing is not finished yet.
 */
bool _lv_svg_parser_token(_lv_svg_parser_t * parser, const _lv_svg_token_t * token);

/**
 * @brief Check if the parsing is finished
 * @param parser pointer to a parser object
 * @return true: the parsing is finished, false: the parsing is not finished yet.
 */
bool _lv_svg_parser_is_finish(_lv_svg_parser_t * parser);

/**
 * @brief Dump the SVG tree
 * @param root pointer to the root of the SVG tree
 * @param depth the depth of the current node in the tree
 */
void _lv_svg_dump_tree(lv_svg_node_t * root, int depth);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SVG*/

#endif /*LV_SVG_PARSER_H*/
