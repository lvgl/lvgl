/**
 * @file lv_xml_switch_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_switch_parser.h"
#if LV_USE_XML

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
static lv_switch_orientation_t orientation_text_to_enum_value(const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_switch_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);

    void * item = lv_switch_create(lv_xml_state_get_parent(state));
    return item;
}

void lv_xml_switch_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);
    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("orientation", name)) lv_switch_set_orientation(item, orientation_text_to_enum_value(value));
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_switch_orientation_t orientation_text_to_enum_value(const char * txt)
{
    if(lv_streq("auto", txt)) return LV_SWITCH_ORIENTATION_AUTO;
    if(lv_streq("horizontal", txt)) return LV_SWITCH_ORIENTATION_HORIZONTAL;
    if(lv_streq("vertical", txt)) return LV_SWITCH_ORIENTATION_VERTICAL;

    LV_LOG_WARN("%s is an unknown value for switch's orientation", txt);

    return 0; /*Return 0 in lack of a better option. */
}

#endif /* LV_USE_XML */
