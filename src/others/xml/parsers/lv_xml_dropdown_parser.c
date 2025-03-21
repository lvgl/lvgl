/**
 * @file lv_xml_dropdown_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_dropdown_parser.h"
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

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_dropdown_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_dropdown_create(lv_xml_state_get_parent(state));

    return item;
}


void lv_xml_dropdown_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("options", name)) lv_dropdown_set_options(item, value);
        if(lv_streq("text", name)) lv_dropdown_set_text(item, value);
        if(lv_streq("selected", name)) lv_dropdown_set_selected(item, lv_xml_atoi(value), LV_ANIM_OFF);
        if(lv_streq("symbol", name)) lv_dropdown_set_symbol(item, lv_xml_get_image(&state->ctx, value));
    }
}

void * lv_xml_dropdown_list_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);

    return lv_dropdown_get_list(lv_xml_state_get_parent(state));
}

void lv_xml_dropdown_list_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);

    lv_xml_obj_apply(state, attrs);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
