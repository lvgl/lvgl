/**
 * @file lv_xml_spinner_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_xml_spinner_parser.h"
#if LV_USE_XML && LV_USE_SPINNER

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

void * lv_xml_spinner_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);

    void * item = lv_spinner_create(lv_xml_state_get_parent(state));
    return item;
}

void lv_xml_spinner_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("anim_duration", name)) lv_spinner_set_anim_duration(item, lv_xml_atoi(value));
        else if(lv_streq("arc_sweep", name)) lv_spinner_set_arc_sweep(item, lv_xml_atoi(value));
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_XML && LV_USE_SPINNER*/
