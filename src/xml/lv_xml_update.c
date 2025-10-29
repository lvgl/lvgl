/**
 * @file lv_xml_update.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_update.h"
#if LV_USE_XML &&  LV_USE_OBJ_NAME

#include "../lvgl.h"
#include "lv_xml_widget.h"
#include "lv_xml_parser.h"
#include "../libs/expat/expat.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void start_handler(void * user_data, const char * name, const char ** attrs);
static void end_handler(void * user_data, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


lv_result_t lv_xml_update_from_data(const char * xml_def)
{
    /*Create a dummy parser state*/
    lv_xml_parser_state_t state;
    lv_xml_parser_state_init(&state);

    /* Parse the XML to extract metadata */
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetUserData(parser, &state);
    XML_SetElementHandler(parser, start_handler, end_handler);

    if(XML_Parse(parser, xml_def, lv_strlen(xml_def), XML_TRUE) == XML_STATUS_ERROR) {
        LV_LOG_ERROR("XML parsing error: %s on line %lu",
                     XML_ErrorString(XML_GetErrorCode(parser)),
                     (unsigned long)XML_GetCurrentLineNumber(parser));
        XML_ParserFree(parser);
        return LV_RESULT_INVALID;
    }
    XML_ParserFree(parser);
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void start_handler(void * user_data, const char * name, const char ** attrs)
{
    lv_xml_parser_state_t * state = (lv_xml_parser_state_t *)user_data;

    size_t update_len = lv_strlen("update-");
    size_t name_len = lv_strlen(name);
    if(name_len < update_len) {
        LV_LOG_WARN("%s doesn't start with `update-`", name);
        return;
    }

    if(lv_memcmp(name, "update-", update_len)) {
        LV_LOG_WARN("%s doesn't start with `update-`", name);
        return;
    }

    name = &name[update_len]; /*Get e.g. update-lv_slider -> lv_slider*/
    name_len -= update_len;

    lv_widget_processor_t * proc = lv_xml_widget_get_processor(name);
    if(proc == NULL) {
        LV_LOG_WARN("%s is not a known widget", name);
        return;
    }

    const char * widget_name = lv_xml_get_value_of(attrs, "name");
    if(widget_name == NULL) {
        LV_LOG_WARN("There is no name property");
        return;
    }

    lv_obj_t * obj = lv_obj_get_child_by_name(lv_screen_active(), widget_name);
    if(obj == NULL) {
        LV_LOG_WARN("No widget is found with the name of `%s`", widget_name);
        return;
    }

    /*Clean the name property the prevent applying it*/
    for(int i = 0; attrs[i]; i += 2) {
        if(lv_streq(attrs[i], "name")) {
            attrs[i] = "";
            attrs[i + 1] = "";
        }
    }
    state->item = obj;
    proc->apply_cb(state, attrs);
}

static void end_handler(void * user_data, const char * name)
{
    LV_UNUSED(user_data);
    LV_UNUSED(name);
}

#endif /* LV_USE_XML */
