/**
 * @file lv_xml_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml.h"
#if LV_USE_XML

#include "lv_xml_private.h"
#include "lv_xml_parser.h"
#include "lv_xml_style.h"
#include "lv_xml_component_private.h"
#include "lv_xml_base_types.h"

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

void lv_xml_parser_state_init(lv_xml_parser_state_t * state)
{
    lv_memzero(state, sizeof(lv_xml_parser_state_t));
    lv_ll_init(&state->parent_ll, sizeof(lv_obj_t *));
    lv_xml_component_scope_init(&state->scope);
}

void lv_xml_parser_start_section(lv_xml_parser_state_t * state, const char * name)
{
    /* Check for context changes */
    if(lv_streq(name, "api")) {
        state->section = LV_XML_PARSER_SECTION_API;
        return;
    }
    if(lv_streq(name, "gradients")) {
        state->section = LV_XML_PARSER_SECTION_GRAD;
        return;
    }
    if(state->section == LV_XML_PARSER_SECTION_GRAD && lv_streq(name, "stop")) {
        state->section = LV_XML_PARSER_SECTION_GRAD_STOP;
        return;
    }
    else if(lv_streq(name, "consts")) {
        state->section = LV_XML_PARSER_SECTION_CONSTS;
        return;
    }
    else if(lv_streq(name, "styles")) {
        state->section = LV_XML_PARSER_SECTION_STYLES;
        return;
    }
    else if(lv_streq(name, "images")) {
        state->section = LV_XML_PARSER_SECTION_IMAGES;
        return;
    }
    else if(lv_streq(name, "fonts")) {
        state->section = LV_XML_PARSER_SECTION_FONTS;
        return;
    }
    else if(lv_streq(name, "subjects")) {
        state->section = LV_XML_PARSER_SECTION_SUBJECTS;
        return;
    }
    else if(lv_streq(name, "animation")) {
        state->section = LV_XML_PARSER_SECTION_ANIMATION;
        return;
    }
    else if(lv_streq(name, "include_timeline")) {
        state->section = LV_XML_PARSER_SECTION_INCLUDE_TIMELINE;
        return;
    }
    else if(lv_streq(name, "timeline")) {
        state->section = LV_XML_PARSER_SECTION_TIMELINE;
        return;
    }
    else if(lv_streq(name, "view")) {
        state->section = LV_XML_PARSER_SECTION_VIEW;
        return;
    }
}

void lv_xml_parser_end_section(lv_xml_parser_state_t * state, const char * name)
{

    /* Reset context when leaving a block */
    if(lv_streq(name, "params") ||
       lv_streq(name, "consts") ||
       lv_streq(name, "gradients") ||
       lv_streq(name, "styles") ||
       lv_streq(name, "view")) {
        state->section = LV_XML_PARSER_SECTION_NONE;
    }

    /*When processing gradient stops, but not a stop was closed got bacg to gradient processing
     * E.g. </linear_gradient>*/
    if(state->section == LV_XML_PARSER_SECTION_GRAD_STOP && !lv_streq(name, "stop")) {
        state->section = LV_XML_PARSER_SECTION_GRAD;
    }
}

void * lv_xml_state_get_parent(lv_xml_parser_state_t * state)
{
    return state->parent;
}

void * lv_xml_state_get_item(lv_xml_parser_state_t * state)
{
    return state->item;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
