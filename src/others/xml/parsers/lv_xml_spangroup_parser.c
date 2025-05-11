/**
 * @file lv_xml_spangroup_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_spangroup_parser.h"
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

static lv_span_overflow_t spangroup_overflow_to_enum(const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_spangroup_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_spangroup_create(lv_xml_state_get_parent(state));

    return item;
}


void lv_xml_spangroup_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("overflow", name)) lv_spangroup_set_overflow(item, spangroup_overflow_to_enum(value));
        else if(lv_streq("max_lines", name)) lv_spangroup_set_max_lines(item, lv_xml_atoi(value));
        else if(lv_streq("indent", name)) lv_spangroup_set_indent(item, lv_xml_atoi(value));
    }
}

void * lv_xml_spangroup_span_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_spangroup_add_span(lv_xml_state_get_parent(state));
    return item;
}

void lv_xml_spangroup_span_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);

    lv_obj_t * spangroup = lv_xml_state_get_parent(state);
    lv_span_t * span = lv_xml_state_get_item(state);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("text", name)) lv_spangroup_set_span_text(spangroup, span, value);
        else if(lv_streq("style", name)) {
            lv_xml_style_t * style_dsc = lv_xml_get_style_by_name(&state->scope, value);
            lv_spangroup_set_span_style(spangroup, span, &style_dsc->style);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_span_overflow_t spangroup_overflow_to_enum(const char * txt)
{
    if(lv_streq("clip", txt)) return LV_SPAN_OVERFLOW_CLIP;
    if(lv_streq("ellipsis", txt)) return LV_SPAN_OVERFLOW_ELLIPSIS;

    LV_LOG_WARN("%s is an unknown value for span's overflow", txt);
    return 0; /*Return 0 in lack of a better option. */
}

#endif /* LV_USE_XML */
