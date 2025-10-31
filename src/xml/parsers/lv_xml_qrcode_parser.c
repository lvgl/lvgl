/**
 * @file lv_xml_qrcode_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_qrcode_parser.h"
#if LV_USE_XML && LV_USE_QRCODE

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

void * lv_xml_qrcode_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);

    void * item = lv_qrcode_create(lv_xml_state_get_parent(state));
    return item;
}

void lv_xml_qrcode_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);
    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name  = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("size", name)) lv_qrcode_set_size(item, lv_xml_atoi(value));
        else if(lv_streq("dark_color", name)) lv_qrcode_set_dark_color(item, lv_xml_to_color(value));
        else if(lv_streq("light_color", name)) lv_qrcode_set_light_color(item, lv_xml_to_color(value));
        else if(lv_streq("data", name)) lv_qrcode_set_data(item, value);
        else if(lv_streq("quiet_zone", name)) lv_qrcode_set_quiet_zone(item, lv_xml_to_bool(value));
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
