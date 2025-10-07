/**
 * @file lv_xml_keyboard_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_keyboard_parser.h"
#if LV_USE_XML && LV_USE_KEYBOARD

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
static lv_keyboard_mode_t mode_text_to_enum_value(const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_keyboard_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_keyboard_create(lv_xml_state_get_parent(state));

    return item;
}

void lv_xml_keyboard_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        //        if(lv_streq("textarea", name)) lv_keyboard_set_mode(item, lv_obj_get_child_by_name());
        if(lv_streq("mode", name)) lv_keyboard_set_mode(item, mode_text_to_enum_value(value));
        if(lv_streq("popovers", name)) lv_keyboard_set_popovers(item, lv_xml_to_bool(value));
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_keyboard_mode_t mode_text_to_enum_value(const char * txt)
{
    if(lv_streq("text_upper", txt)) return LV_KEYBOARD_MODE_TEXT_UPPER;
    if(lv_streq("text_lower", txt)) return LV_KEYBOARD_MODE_TEXT_LOWER;
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    if(lv_streq("text_arabic", txt)) return LV_KEYBOARD_MODE_TEXT_ARABIC;
#endif
    if(lv_streq("number", txt)) return LV_KEYBOARD_MODE_NUMBER;
    if(lv_streq("special", txt)) return LV_KEYBOARD_MODE_SPECIAL;
    if(lv_streq("user_1", txt)) return LV_KEYBOARD_MODE_USER_1;
    if(lv_streq("user_2", txt)) return LV_KEYBOARD_MODE_USER_2;
    if(lv_streq("user_3", txt)) return LV_KEYBOARD_MODE_USER_3;
    if(lv_streq("user_4", txt)) return LV_KEYBOARD_MODE_USER_4;

    LV_LOG_WARN("%s is an unknown value for keyboard's mode", txt);
    return 0; /*Return 0 in lack of a better option. */
}

#endif /* LV_USE_XML */
