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
#include "../misc/lv_types.h"
#include "../misc/lv_style.h"
#include "../core/lv_obj_style.h"

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
 * @param scope     add styles here. (Constants should be already added as style properties might use them)
 * @param attrs     list of attribute names and values
 */
lv_result_t lv_xml_register_style(lv_xml_component_scope_t * scope, const char ** attrs);

/**
 * Decompose a string like `"style1:pressed:checked:knob"` to style name and selector
 * @param txt           the input string
 * @param selector      store the selectors here
 * @return              the style name or `NULL` on any error
 */
const char * lv_xml_style_string_process(char * txt, lv_style_selector_t * selector);

/**
 * Find a style by name which was added by `lv_xml_register_style`
 * @param scope     the default context to search in
 * @param name      the name of the style. Can start with a component name prefix (e.g. `my_button.blue`) to overwrite the ctx
 * @return          the style structure
 */
lv_xml_style_t * lv_xml_get_style_by_name(lv_xml_component_scope_t * scope, const char * name);

/**
 * Get a gradient descriptor defined for a component
 * @param scope component context where the gradient should be found
 * @param name  name of the gradient
 * @return      a gradient descriptor
 */
lv_grad_dsc_t * lv_xml_component_get_grad(lv_xml_component_scope_t * scope, const char * name);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_STYLE_H*/
