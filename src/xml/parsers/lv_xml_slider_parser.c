/**
 * @file lv_xml_slider_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_slider_parser.h"
#if LV_USE_XML && LV_USE_SLIDER

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
static lv_slider_orientation_t orientation_text_to_enum_value(const char * txt);
static lv_slider_mode_t mode_text_to_enum_value(const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_slider_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);

    void * item = lv_slider_create(lv_xml_state_get_parent(state));
    return item;
}

void lv_xml_slider_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);
    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("value", name)) {
            int32_t v = lv_xml_atoi(value);
            const char * anim_str = lv_xml_get_value_of(attrs, "value-animated");
            bool anim = anim_str ? lv_xml_to_bool(anim_str) : false;
            lv_slider_set_value(item, v, anim);
        }
        else if(lv_streq("start_value", name)) {
            int32_t v = lv_xml_atoi(value);
            const char * anim_str = lv_xml_get_value_of(attrs, "start_value-animated");
            bool anim = anim_str ? lv_xml_to_bool(anim_str) : false;
            lv_slider_set_start_value(item, v, anim);
        }
        else if(lv_streq("bind_value", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                lv_slider_bind_value(item, subject);
            }
            else {
                LV_LOG_WARN("Subject \"%s\" doesn't exist in slider bind_value", value);
            }
        }
        else if(lv_streq("orientation", name)) lv_slider_set_orientation(item, orientation_text_to_enum_value(value));
        else if(lv_streq("mode", name)) lv_slider_set_mode(item, mode_text_to_enum_value(value));
        else if(lv_streq("min_value", name)) lv_slider_set_min_value(item, lv_xml_atoi(value));
        else if(lv_streq("max_value", name)) lv_slider_set_max_value(item, lv_xml_atoi(value));
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_slider_orientation_t orientation_text_to_enum_value(const char * txt)
{
    if(lv_streq("auto", txt)) return LV_SLIDER_ORIENTATION_AUTO;
    if(lv_streq("horizontal", txt)) return LV_SLIDER_ORIENTATION_HORIZONTAL;
    if(lv_streq("vertical", txt)) return LV_SLIDER_ORIENTATION_VERTICAL;

    LV_LOG_WARN("%s is an unknown value for slider's orientation", txt);
    return 0; /*Return 0 in lack of a better option. */
}

static lv_slider_mode_t mode_text_to_enum_value(const char * txt)
{
    if(lv_streq("normal", txt)) return LV_SLIDER_MODE_NORMAL;
    if(lv_streq("range", txt)) return LV_SLIDER_MODE_RANGE;
    if(lv_streq("symmetrical", txt)) return LV_SLIDER_MODE_SYMMETRICAL;

    LV_LOG_WARN("%s is an unknown value for slider's mode", txt);
    return 0; /*Return 0 in lack of a better option. */
}

#endif /* LV_USE_XML */
