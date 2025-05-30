/**
 * @file lv_xml_calendar_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_calendar_parser.h"
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

void * lv_xml_calendar_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_calendar_create(lv_xml_state_get_parent(state));

    return item;
}


void lv_xml_calendar_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("today_year", name)) lv_calendar_set_today_year(item, lv_xml_atoi(value));
        else if(lv_streq("today_month", name)) lv_calendar_set_today_month(item, lv_xml_atoi(value));
        else if(lv_streq("today_day", name)) lv_calendar_set_today_day(item, lv_xml_atoi(value));
        else if(lv_streq("shown_year", name)) lv_calendar_set_shown_year(item, lv_xml_atoi(value));
        else if(lv_streq("shown_month", name)) lv_calendar_set_shown_month(item, lv_xml_atoi(value));
    }
}

void * lv_xml_calendar_header_dropdown_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_calendar_add_header_dropdown(lv_xml_state_get_parent(state));
    return item;
}

void lv_xml_calendar_header_dropdown_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/
}

void * lv_xml_calendar_header_arrow_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_calendar_add_header_arrow(lv_xml_state_get_parent(state));

    return item;
}

void lv_xml_calendar_header_arrow_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
