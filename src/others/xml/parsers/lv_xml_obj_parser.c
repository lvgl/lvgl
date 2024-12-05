/**
 * @file lv_xml_obj_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_obj_parser.h"
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

void * lv_xml_obj_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_obj_create(lv_xml_state_get_parent(state));

    return item;
}

void lv_xml_obj_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("x", name)) lv_obj_set_x(item, lv_xml_to_size(value));
        if(lv_streq("y", name)) lv_obj_set_y(item, lv_xml_to_size(value));
        if(lv_streq("width", name)) lv_obj_set_width(item, lv_xml_to_size(value));
        if(lv_streq("height", name)) lv_obj_set_height(item, lv_xml_to_size(value));
        if(lv_streq("align", name)) lv_obj_set_align(item, lv_xml_align_string_to_enum_value(value));

        if(lv_streq("styles", name)) lv_xml_style_add_to_obj(state, item, value);

        if(lv_strlen(name) > 6 && lv_memcmp("style_", name, 6) == 0) {
            char name_local[512];
            lv_strlcpy(name_local, name, sizeof(name_local));

            lv_style_selector_t selector;
            const char * prop_name = lv_xml_style_string_process(name_local, &selector);

            if(lv_streq("style_radius", prop_name)) lv_obj_set_style_radius(item, lv_xml_atoi(value), selector);
            if(lv_streq("style_bg_color", prop_name)) lv_obj_set_style_bg_color(item, lv_xml_to_color(value), selector);
            if(lv_streq("style_bg_opa", prop_name)) lv_obj_set_style_bg_opa(item, lv_xml_atoi(value), selector);
            if(lv_streq("style_border_color", prop_name)) lv_obj_set_style_border_color(item, lv_xml_to_color(value), selector);
            if(lv_streq("style_border_opa", prop_name)) lv_obj_set_style_border_opa(item, lv_xml_atoi(value), selector);
            if(lv_streq("style_border_width", prop_name)) lv_obj_set_style_border_width(item, lv_xml_atoi(value), selector);
            if(lv_streq("style_bg_image_src", prop_name)) lv_obj_set_style_bg_image_src(item, lv_xml_get_image(value), selector);
            if(lv_streq("style_bg_image_tiled", prop_name)) lv_obj_set_style_bg_image_tiled(item, lv_xml_to_bool(value), selector);
            if(lv_streq("style_text_color", prop_name)) lv_obj_set_style_text_color(item, lv_xml_to_color(value), selector);
            if(lv_streq("style_text_font", prop_name)) lv_obj_set_style_text_font(item, lv_xml_get_font(value), selector);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


#endif /* LV_USE_XML */
