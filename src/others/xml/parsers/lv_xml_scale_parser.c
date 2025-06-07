/**
 * @file lv_xml_scale_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_scale_parser.h"
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
static lv_scale_mode_t scale_mode_to_enum(const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_scale_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_scale_create(lv_xml_state_get_parent(state));

    return item;
}


void lv_xml_scale_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/


    //    <prop name="text_src" type="string[NULL]" help=""/>

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("mode", name)) lv_scale_set_mode(item, scale_mode_to_enum(value));
        else if(lv_streq("total_tick_count", name)) lv_scale_set_total_tick_count(item, lv_xml_atoi(value));
        else if(lv_streq("major_tick_every", name)) lv_scale_set_major_tick_every(item, lv_xml_atoi(value));
        else if(lv_streq("label_show", name)) lv_scale_set_label_show(item, lv_xml_to_bool(value));
        else if(lv_streq("post_draw", name)) lv_scale_set_post_draw(item, lv_xml_to_bool(value));
        else if(lv_streq("draw_ticks_on_top", name)) lv_scale_set_draw_ticks_on_top(item, lv_xml_to_bool(value));
        else if(lv_streq("min_value", name)) lv_scale_set_min_value(item, lv_xml_atoi(value));
        else if(lv_streq("max_value", name)) lv_scale_set_max_value(item, lv_xml_atoi(value));
        else if(lv_streq("angle_range", name)) lv_scale_set_angle_range(item, lv_xml_atoi(value));
        else if(lv_streq("rotation", name)) lv_scale_set_rotation(item, lv_xml_to_bool(value));
    }
}

void * lv_xml_scale_section_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_scale_add_section(lv_xml_state_get_parent(state));
    return item;
}

void lv_xml_scale_section_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);

    lv_obj_t * scale = lv_xml_state_get_parent(state);
    lv_scale_section_t * section = lv_xml_state_get_item(state);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("min_value", name)) lv_scale_set_section_min_value(scale, section, lv_xml_atoi(value));
        else if(lv_streq("max_value", name)) lv_scale_set_section_max_value(scale, section, lv_xml_atoi(value));
        else if(lv_streq("style_main", name)) {
            lv_xml_style_t * style_dsc = lv_xml_get_style_by_name(&state->scope, value);
            lv_scale_set_section_style_main(scale, section, &style_dsc->style);
        }
        else if(lv_streq("style_indicator", name)) {
            lv_xml_style_t * style_dsc = lv_xml_get_style_by_name(&state->scope, value);
            lv_scale_set_section_style_indicator(scale, section, &style_dsc->style);
        }
        else if(lv_streq("style_items", name)) {
            lv_xml_style_t * style_dsc = lv_xml_get_style_by_name(&state->scope, value);
            lv_scale_set_section_style_items(scale, section, &style_dsc->style);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_scale_mode_t scale_mode_to_enum(const char * txt)
{
    if(lv_streq("horizontal_top", txt)) return LV_SCALE_MODE_HORIZONTAL_TOP;
    if(lv_streq("horizontal_bottom", txt)) return LV_SCALE_MODE_HORIZONTAL_BOTTOM;
    if(lv_streq("vertical_left", txt)) return LV_SCALE_MODE_VERTICAL_LEFT;
    if(lv_streq("vertical_right", txt)) return LV_SCALE_MODE_VERTICAL_RIGHT;
    if(lv_streq("round_inner", txt)) return LV_SCALE_MODE_ROUND_INNER;
    if(lv_streq("round_outer", txt)) return LV_SCALE_MODE_ROUND_OUTER;

    LV_LOG_WARN("%s is an unknown value for scale's mode", txt);
    return 0; /*Return 0 in lack of a better option. */
}


#endif /* LV_USE_XML */
