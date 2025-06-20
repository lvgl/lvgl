/**
 * @file lv_xml_tabview_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_tabview_parser.h"
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

void * lv_xml_tabview_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_tabview_create(lv_xml_state_get_parent(state));

    return item;
}


void lv_xml_tabview_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("active", name)) lv_tabview_set_active(item, lv_xml_atoi(value), 0);
        if(lv_streq("tab_bar_position", name)) lv_tabview_set_tab_bar_position(item, lv_xml_dir_to_enum(value));
        if(lv_streq("event_bubble", name)) {
			 lv_obj_t * cont = lv_tabview_get_content(item);
			 if (cont) {
				lv_obj_update_flag(cont, LV_OBJ_FLAG_EVENT_BUBBLE, lv_xml_to_bool(value));
			 }
		}
    }
}

void * lv_xml_tabview_tab_bar_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_tabview_get_tab_bar(lv_xml_state_get_parent(state));
    return item;
}


void lv_xml_tabview_tab_bar_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    /*Apply the common properties, e.g. width, height, styles flags etc*/
    lv_xml_obj_apply(state, attrs);
}

void * lv_xml_tabview_tab_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * text = lv_xml_get_value_of(attrs, "text");
    void * item = lv_tabview_add_tab(lv_xml_state_get_parent(state), text);
    return item;
}

void lv_xml_tabview_tab_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    /*Apply the common properties, e.g. width, height, styles flags etc*/
    lv_xml_obj_apply(state, attrs);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
