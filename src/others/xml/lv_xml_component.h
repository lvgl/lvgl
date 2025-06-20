/**
 * @file lv_xml_component.h
 *
 */

#ifndef LV_XML_COMPONENT_H
#define LV_XML_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_types.h"
#if LV_USE_XML

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Process a component during parsing an XML. It create a widget and apply all the attributes
 * @param state     the current parsing state
 * @param name      name of the component
 * @param attrs     attributes of the widget
 * @return
 */
lv_obj_t * lv_xml_component_process(lv_xml_parser_state_t * state, const char * name, const char ** attrs);

/**
 * Load the styles, constants, another data of the component. It needs to be called only once for each component.
 * @param name      the name as the component will be referenced later in other components
 * @param xml_def   the XML definition of the component as a NULL terminated string
 * @return          LV_RES_OK: loaded successfully, LV_RES_INVALID: otherwise
 */
lv_result_t lv_xml_component_register_from_data(const char * name, const char * xml_def);

/**
 * Load the styles, constants, another data of the component. It needs to be called only once for each component.
 * @param path      path to an XML file
 * @return          LV_RES_OK: loaded successfully, LV_RES_INVALID: otherwise
 */
lv_result_t lv_xml_component_register_from_file(const char * path);

/**
 * Get the scope of a component which was registered by
 * `lv_xml_component_register_from_data` or `lv_xml_component_register_from_file`
 * @param component_name    name of the component
 * @return                  pointer the scope or NULL if not found
 */
lv_xml_component_scope_t * lv_xml_component_get_scope(const char * component_name);

/**
 * Remove a component from from the list.
 * @param name      the name of the component (used during registration)
 * @return          LV_RESULT_OK on successful  unregistration, LV_RESULT_INVALID otherwise.
 */
lv_result_t lv_xml_component_unregister(const char * name);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_COMPONENT_H*/


