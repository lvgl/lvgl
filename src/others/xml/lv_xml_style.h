/**
 * @file lv_xml_style.h
 *
 */

#ifndef LV_XML_STYLE_H
#define LV_XML_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_types.h"
#include "../../misc/lv_style.h"
#include "../../core/lv_obj_style.h"

#if LV_USE_XML

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_xml_style_t {
    const char * name;
    const char * long_name;
    lv_style_t style;
} lv_xml_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Add a style to `ctx` and set the style properties from `attrs`
 * @param ctx       add styles here. (Constants should be already added as style properties might use them)
 * @param attrs     list of attribute names and values
 */
void lv_xml_style_register(lv_xml_component_ctx_t * ctx, const char ** attrs);

/**
 * Add the styles to an object. Handles multiple styles and selectors too.
 * @param state     the parser state
 * @param obj       the target widget
 * @param text      the styles' string, e.g. "blue red:pressed:knob"
 */
void lv_xml_style_add_to_obj(lv_xml_parser_state_t * state, lv_obj_t * obj, const char * text);

/**
 * Convert a style state to enum
 * @param txt       e.g. "pressed"
 * @return          the enum `LV_STATE_PRESSED`
 */
lv_state_t lv_xml_style_state_string_to_enum_value(const char * txt);

/**
 * Convert a style part to enum
 * @param txt       e.g. "knob"
 * @return          the enum `LV_PART_KNOB`
 */
lv_part_t lv_xml_style_part_string_to_enum_value(const char * txt);

/**
 * Decompose a string like `"style1:pressed:checked:knob"` to style name and selector
 * @param txt           the input string
 * @param selector      store the selectors here
 * @return              the style name or `NULL` on any error
 */
const char * lv_xml_style_string_process(char * txt, lv_style_selector_t * selector);

/**
 * Find a style by name which was added by `lv_xml_style_register`
 * @param ctx       the default context to search in
 * @param name      the name of the style. Can start with a component name prefix (e.g. `my_button.blue`) to overwrite the ctx
 * @return          the style structure
 */
lv_xml_style_t * lv_xml_get_style_by_name(lv_xml_component_ctx_t * ctx, const char * name);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_STYLE_H*/
