/**
 * @file lv_svg.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_svg.h"
#if LV_USE_SVG

#include "../../misc/lv_assert.h"
#include "../../misc/lv_log.h"
#include "../../stdlib/lv_mem.h"

#include "lv_svg_token.h"
#include "lv_svg_parser.h"

/*********************
*      DEFINES
*********************/

/**********************
*      TYPEDEFS
**********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_svg_node_constructor(const lv_tree_class_t * class_p, lv_tree_node_t * node)
{
    LV_UNUSED(class_p);
    lv_svg_node_t * t = (lv_svg_node_t *)node;
    t->xml_id = NULL;
    t->type = LV_SVG_TAG_INVALID;
    lv_array_init(&t->attrs, 4, sizeof(lv_svg_attr_t));
    t->render_obj = NULL;
}

static void lv_svg_node_destructor(const lv_tree_class_t * class_p, lv_tree_node_t * node)
{
    LV_UNUSED(class_p);
    lv_svg_node_t * t = (lv_svg_node_t *)node;
    if(t->xml_id) {
        lv_free(t->xml_id);
    }
    for(uint32_t i = 0; i < lv_array_size(&t->attrs); i++) {
        lv_svg_attr_t * attr = lv_array_at(&t->attrs, i);
        if(attr->val_type == LV_SVG_ATTR_VALUE_PTR) {
            lv_free(attr->value.val);
        }
    }
    lv_array_deinit(&t->attrs);
}

static bool svg_token_process_cb(_lv_svg_token_t * token, void * data)
{
    _lv_svg_parser_t * parser = (_lv_svg_parser_t *)data;
    return _lv_svg_parser_token(parser, token);
}

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_tree_class_t lv_svg_node_class = {
    .base_class = &lv_tree_node_class,
    .instance_size = sizeof(lv_svg_node_t),
    .constructor_cb = lv_svg_node_constructor,
    .destructor_cb = lv_svg_node_destructor,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_svg_node_t * lv_svg_load_data(const char * svg_data, uint32_t data_len)
{
    LV_ASSERT_NULL(svg_data);
    LV_ASSERT(data_len > 0);

    _lv_svg_parser_t parser;
    _lv_svg_parser_init(&parser);

    if(_lv_svg_tokenizer(svg_data, data_len, svg_token_process_cb, &parser)) {
        if(_lv_svg_parser_is_finish(&parser)) {
            lv_svg_node_t * doc = parser.doc_root;
            parser.doc_root = NULL;
            _lv_svg_parser_deinit(&parser);
#if LV_USE_SVG_DEBUG
            _lv_svg_dump_tree(doc, 0);
#endif
            return doc;
        }
        else {
            _lv_svg_parser_deinit(&parser);
            LV_LOG_ERROR("svg document parser raise errors!");
            return NULL;
        }
    }
    else {
        _lv_svg_parser_deinit(&parser);
        LV_LOG_ERROR("svg document tokenizer raise errors!");
        return NULL;
    }
}

lv_svg_node_t * lv_svg_node_create(lv_svg_node_t * parent)
{
    lv_tree_node_t * node = lv_tree_node_create(&lv_svg_node_class, (lv_tree_node_t *)parent);
    return (lv_svg_node_t *)node;
}

void lv_svg_node_delete(lv_svg_node_t * node)
{
    lv_tree_node_delete((lv_tree_node_t *)node);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_SVG*/
