/**
 * @file lv_xml_chart_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_chart_parser.h"
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
static lv_chart_type_t chart_type_to_enum(const char * txt);
static lv_chart_update_mode_t chart_update_mode_to_enum(const char * txt);
static lv_chart_axis_t chart_axis_to_enum(const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_chart_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_chart_create(lv_xml_state_get_parent(state));

    return item;
}


void lv_xml_chart_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("point_count", name)) lv_chart_set_point_count(item, lv_xml_atoi(value));
        else if(lv_streq("type", name)) lv_chart_set_type(item, chart_type_to_enum(value));
        else if(lv_streq("update_mode", name)) lv_chart_set_update_mode(item, chart_update_mode_to_enum(value));
        else if(lv_streq("div_line_count", name)) {

            int32_t value1 = lv_xml_atoi_split(&value, ' ');
            int32_t value2 = lv_xml_atoi_split(&value, ' ');
            lv_chart_set_div_line_count(item, value1, value2);
        }
    }
}

void * lv_xml_chart_series_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * color = lv_xml_get_value_of(attrs, "color");
    const char * axis = lv_xml_get_value_of(attrs, "axis");
    void * item = lv_chart_add_series(lv_xml_state_get_parent(state), lv_color_hex(lv_xml_strtol(color, NULL, 16)),
                                      chart_axis_to_enum(axis));
    return item;
}

void lv_xml_chart_series_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);

    lv_obj_t * chart = lv_xml_state_get_parent(state);
    lv_chart_series_t * ser = lv_xml_state_get_item(state);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("values", name)) {
            while(value[0] != '\0') {
                int32_t v = lv_xml_atoi_split(&value, ' ');
                lv_chart_set_next_value(chart, ser, v);
            }
        }
    }
}

void * lv_xml_chart_cursor_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    const char * color = lv_xml_get_value_of(attrs, "color");
    const char * dir = lv_xml_get_value_of(attrs, "dir");
    void * item = lv_chart_add_cursor(lv_xml_state_get_parent(state), lv_color_hex(lv_xml_strtol(color, NULL, 16)),
                                      lv_xml_dir_to_enum(dir));

    return item;
}

void lv_xml_chart_cursor_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);

    lv_obj_t * chart = lv_xml_state_get_parent(state);
    lv_chart_cursor_t * cursor = lv_xml_state_get_item(state);

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];


        if(lv_streq("pos", name)) {
            int32_t x = lv_xml_atoi_split(&value, ' ');
            int32_t y = lv_xml_atoi_split(&value, ' ');
            lv_point_t p = {x, y};
            lv_chart_set_cursor_pos(chart, cursor, &p);
        }
    }
}

void * lv_xml_chart_axis_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);

    /*Nothing to create*/
    return lv_xml_state_get_parent(state);;
}

void lv_xml_chart_axis_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(state);
    LV_UNUSED(attrs);

    lv_obj_t * chart = lv_xml_state_get_parent(state);
    lv_chart_axis_t axis = chart_axis_to_enum(lv_xml_get_value_of(attrs, "axis"));

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("range", name)) {
            int32_t min_val = lv_xml_atoi_split(&value, ' ');
            int32_t max_val = lv_xml_atoi_split(&value, ' ');
            lv_chart_set_range(chart, axis, min_val, max_val);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_chart_type_t chart_type_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_CHART_TYPE_NONE;
    if(lv_streq("line", txt)) return LV_CHART_TYPE_LINE;
    if(lv_streq("bar", txt)) return LV_CHART_TYPE_BAR;
    if(lv_streq("scatter", txt)) return LV_CHART_TYPE_SCATTER;

    LV_LOG_WARN("%s is an unknown value for chart's chart_type", txt);
    return 0; /*Return 0 in lack of a better option. */
}

static lv_chart_update_mode_t chart_update_mode_to_enum(const char * txt)
{
    if(lv_streq("shift", txt)) return LV_CHART_UPDATE_MODE_SHIFT;
    if(lv_streq("circular", txt)) return LV_CHART_UPDATE_MODE_CIRCULAR;

    LV_LOG_WARN("%s is an unknown value for chart's chart_update_mode", txt);
    return 0; /*Return 0 in lack of a better option. */
}

static lv_chart_axis_t chart_axis_to_enum(const char * txt)
{
    if(lv_streq("primary_x", txt)) return LV_CHART_AXIS_PRIMARY_X;
    if(lv_streq("primary_y", txt)) return LV_CHART_AXIS_PRIMARY_Y;
    if(lv_streq("secondary_x", txt)) return LV_CHART_AXIS_SECONDARY_X;
    if(lv_streq("secondary_y", txt)) return LV_CHART_AXIS_SECONDARY_Y;

    LV_LOG_WARN("%s is an unknown value for chart's chart_axis", txt);
    return 0; /*Return 0 in lack of a better option. */
}

#endif /* LV_USE_XML */
