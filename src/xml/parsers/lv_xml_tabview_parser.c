/**
 * @file lv_xml_tabview_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_tabview_parser.h"
#if LV_USE_XML && LV_USE_TABVIEW

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

void * lv_xml_tabview_tab_button_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    lv_obj_t * tv = lv_xml_state_get_parent(state);
    int32_t btn_cnt = lv_tabview_get_tab_count(tv);
    if(btn_cnt == 0) {
        LV_LOG_WARN("There are no buttons on the tab view. Get tab buttons when the tabs are already created");
        return NULL;
    }

    const char * index_str = lv_xml_get_value_of(attrs, "index");
    int32_t index_int = index_str ? lv_xml_atoi(index_str) : 0;

    void * item = lv_tabview_get_tab_button(tv, index_int);

    if(item == NULL) {
        LV_LOG_WARN("tabindex is out of range, using the first tab instead");
        item = lv_tabview_get_tab_button(tv, 0);
    }

    return item;
}

void lv_xml_tabview_tab_button_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    /*Apply the common properties, e.g. width, height, styles flags etc*/
    lv_xml_obj_apply(state, attrs);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
