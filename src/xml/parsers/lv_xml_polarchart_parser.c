/**
 * @file lv_xml_polarchart_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_polarchart_parser.h"
#if LV_USE_XML && LV_USE_POLARCHART

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
static lv_polarchart_type_t polarchart_type_to_enum(const char * txt);
static lv_polarchart_update_mode_t polarchart_update_mode_to_enum(const char * txt);
static lv_polarchart_axis_t polarchart_axis_to_enum(const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_polarchart_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_polarchart_create(lv_xml_state_get_parent(state));

    return item;
}


void lv_xml_polarchart_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("point_count", name)) {
            int32_t cnt = lv_xml_atoi(value);
            if(cnt < 0) {
                LV_LOG_WARN("chart's point count can't be negative");
                cnt = 0;
            }
            lv_polarchart_set_point_count(item, cnt);
        }
        else if(lv_streq("type", name)) lv_polarchart_set_type(item, polarchart_type_to_enum(value));
        else if(lv_streq("update_mode", name)) lv_polarchart_set_update_mode(item, polarchart_update_mode_to_enum(value));
        else if(lv_streq("angle_div_line_count", name)) {
            lv_polarchart_set_angle_div_line_count(item, lv_xml_atoi(value));
        }
        else if(lv_streq("radial_div_line_count", name)) {
            lv_polarchart_set_radial_div_line_count(item, lv_xml_atoi(value));
        }
    }
}

void * lv_xml_polarchart_series_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * color = lv_xml_get_value_of(attrs, "color");
    const char * axis = lv_xml_get_value_of(attrs, "axis");
    if(color == NULL) color = "0xff0000";
    if(axis == NULL) axis = "radial";

    void * item = lv_polarchart_add_series(lv_xml_state_get_parent(state), lv_color_hex(lv_xml_strtol(color, NULL, 16)),
                                           polarchart_axis_to_enum(axis));
    return item;
}

void lv_xml_polarchart_series_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);

    lv_obj_t * chart = lv_xml_state_get_parent(state);
    lv_polarchart_series_t * ser = lv_xml_state_get_item(state);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("values", name)) {
            while(value[0] != '\0') {
                int32_t v = lv_xml_atoi_split(&value, ' ');
                lv_polarchart_set_next_value(chart, ser, v);
            }
        }
    }
}

void * lv_xml_polarchart_cursor_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * color = lv_xml_get_value_of(attrs, "color");
    const char * dir = lv_xml_get_value_of(attrs, "dir");
    if(color == NULL) color = "0x0000ff";
    if(dir == NULL) dir = "all";

    void * item = lv_polarchart_add_cursor(lv_xml_state_get_parent(state), lv_color_hex(lv_xml_strtol(color, NULL, 16)),
                                           lv_xml_dir_to_enum(dir));

    return item;
}

void lv_xml_polarchart_cursor_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);

    lv_obj_t * chart = lv_xml_state_get_parent(state);
    lv_polarchart_cursor_t * cursor = lv_xml_state_get_item(state);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("pos_x", name)) lv_polarchart_set_cursor_pos_angle(chart, cursor, lv_xml_atoi(value));
        if(lv_streq("pos_y", name)) lv_polarchart_set_cursor_pos_radial(chart, cursor, lv_xml_atoi(value));
    }
}

void * lv_xml_polarchart_axis_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);

    /*Nothing to create*/
    return lv_xml_state_get_parent(state);
}

void lv_xml_polarchart_axis_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);

    lv_obj_t * chart = lv_xml_state_get_parent(state);
    lv_polarchart_axis_t axis = polarchart_axis_to_enum(lv_xml_get_value_of(attrs, "axis"));

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("min_value", name)) lv_polarchart_set_axis_min_value(chart, axis, lv_xml_atoi(value));
        if(lv_streq("max_value", name)) lv_polarchart_set_axis_max_value(chart, axis, lv_xml_atoi(value));
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_polarchart_type_t polarchart_type_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_POLARCHART_TYPE_NONE;
    if(lv_streq("line", txt)) return LV_POLARCHART_TYPE_LINE;

    LV_LOG_WARN("%s is an unknown value for chart's polarchart_type", txt);
    return 0; /*Return 0 in lack of a better option. */
}

static lv_polarchart_update_mode_t polarchart_update_mode_to_enum(const char * txt)
{
    if(lv_streq("shift", txt)) return LV_POLARCHART_UPDATE_MODE_SHIFT;
    if(lv_streq("circular", txt)) return LV_POLARCHART_UPDATE_MODE_CIRCULAR;

    LV_LOG_WARN("%s is an unknown value for chart's polarchart_update_mode", txt);
    return 0; /*Return 0 in lack of a better option. */
}

static lv_polarchart_axis_t polarchart_axis_to_enum(const char * txt)
{
    if(lv_streq("radial", txt)) return LV_POLARCHART_AXIS_RADIAL;
    if(lv_streq("angle", txt)) return LV_POLARCHART_AXIS_ANGLE;

    LV_LOG_WARN("%s is an unknown value for chart's polarchart_axis", txt);
    return 0; /*Return 0 in lack of a better option. */
}

#endif /* LV_USE_XML */
