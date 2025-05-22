/**
 * @file lv_xml_roller_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_roller_parser.h"
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
static lv_roller_mode_t mode_text_to_enum_value(const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_roller_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);

    void * item = lv_roller_create(lv_xml_state_get_parent(state));
    return item;
}

void lv_xml_roller_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);
    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("selected", name)) {
            char buf[64];
            lv_strlcpy(buf, value, sizeof(buf));
            char * buf_p = buf;
            int32_t v1 = lv_xml_atoi(lv_xml_split_str(&buf_p, ' '));
            bool v2 = lv_xml_to_bool(buf_p);
            lv_roller_set_selected(item, v1, v2);
        }
        if(lv_streq("visible_row_count", name)) {
            lv_roller_set_visible_row_count(item, lv_xml_atoi(value));
        }

        if(lv_streq("options", name)) {
            /*E.g. 'a\nb\nc' true'*/
            size_t opts_len = lv_strlen(value);
            char * opts_buf = lv_malloc(opts_len + 1);
            lv_memcpy(opts_buf, value, opts_len + 1);
            LV_ASSERT_MALLOC(opts_buf);

            /*Find the last space and trim the rest*/
            uint32_t space_pos_from_back = 1;
            while(space_pos_from_back < opts_len && value[opts_len - space_pos_from_back] != ' ') {
                space_pos_from_back++;
            }

            opts_buf[opts_len - space_pos_from_back - 1] = '\0'; /*Also trim the `'`*/

            lv_roller_mode_t mode = mode_text_to_enum_value(&opts_buf[opts_len - space_pos_from_back + 1]);

            /*Also skip the leading `'`*/
            lv_roller_set_options(item, opts_buf + 1, mode);

            lv_free(opts_buf);

        }
        else if(lv_streq("bind_value", name)) {
            lv_subject_t * subject = lv_xml_get_subject(&state->scope, value);
            if(subject) {
                lv_roller_bind_value(item, subject);
            }
            else {
                LV_LOG_WARN("Subject \"%s\" doesn't exist in roller bind_value", value);
            }
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_roller_mode_t mode_text_to_enum_value(const char * txt)
{
    if(lv_streq("normal", txt)) return LV_ROLLER_MODE_NORMAL;
    if(lv_streq("infinite", txt)) return LV_ROLLER_MODE_INFINITE;

    LV_LOG_WARN("%s is an unknown value for roller's mode", txt);
    return 0; /*Return 0 in lack of a better option. */
}

#endif /* LV_USE_XML */
