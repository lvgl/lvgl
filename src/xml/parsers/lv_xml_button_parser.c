/**
 * @file lv_xml_button_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_xml_button_parser.h"
#if LV_USE_XML && LV_USE_BUTTON
#include "../../../lvgl.h"
#include "../../../lvgl_private.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_button_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);

    void * item = lv_button_create(lv_xml_state_get_parent(state));

    return item;
}

void lv_xml_button_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    /*Apply the common properties, e.g. width, height, styles flags etc*/
    lv_xml_obj_apply(state, attrs);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
