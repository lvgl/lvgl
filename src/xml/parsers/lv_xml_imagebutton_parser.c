/**
 * @file lv_xml_imagebutton_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_imagebutton_parser.h"
#if LV_USE_XML && LV_USE_IMAGEBUTTON

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

static lv_imagebutton_state_t imagebutton_state_to_enum(const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_imagebutton_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_imagebutton_create(lv_xml_state_get_parent(state));

    return item;
}


void lv_xml_imagebutton_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("state", name)) lv_imagebutton_set_state(item, imagebutton_state_to_enum(value));
    }
}

void * lv_xml_imagebutton_src_left_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * state_str = lv_xml_get_value_of(attrs, "state");
    const char * src_str = lv_xml_get_value_of(attrs, "src");
    void * item = lv_xml_state_get_parent(state);
    lv_imagebutton_set_src_left(item, imagebutton_state_to_enum(state_str),
                                lv_xml_get_image(&state->scope, src_str));
    return item;
}

void lv_xml_imagebutton_src_left_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);

}

void * lv_xml_imagebutton_src_right_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * state_str = lv_xml_get_value_of(attrs, "state");
    const char * src_str = lv_xml_get_value_of(attrs, "src");
    void * item = lv_xml_state_get_parent(state);
    lv_imagebutton_set_src_right(item, imagebutton_state_to_enum(state_str),
                                 lv_xml_get_image(&state->scope, src_str));
    return item;
}

void lv_xml_imagebutton_src_right_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);

}

void * lv_xml_imagebutton_src_mid_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * state_str = lv_xml_get_value_of(attrs, "state");
    const char * src_str = lv_xml_get_value_of(attrs, "src");
    void * item = lv_xml_state_get_parent(state);
    lv_imagebutton_set_src_mid(item, imagebutton_state_to_enum(state_str),
                               lv_xml_get_image(&state->scope, src_str));
    return item;
}

void lv_xml_imagebutton_src_mid_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_imagebutton_state_t imagebutton_state_to_enum(const char * txt)
{
    if(lv_streq("released", txt)) return LV_IMAGEBUTTON_STATE_RELEASED;
    if(lv_streq("pressed", txt)) return LV_IMAGEBUTTON_STATE_PRESSED;
    if(lv_streq("disabled", txt)) return LV_IMAGEBUTTON_STATE_DISABLED;
    if(lv_streq("checked_released", txt)) return LV_IMAGEBUTTON_STATE_CHECKED_RELEASED;
    if(lv_streq("checked_pressed", txt)) return LV_IMAGEBUTTON_STATE_CHECKED_PRESSED;
    if(lv_streq("checked_disabled", txt)) return LV_IMAGEBUTTON_STATE_CHECKED_DISABLED;

    LV_LOG_WARN("%s is an unknown value for imagebutton's imagebutton_state", txt);
    return 0; /*Return 0 in lack of a better option. */
}


#endif /* LV_USE_XML */
