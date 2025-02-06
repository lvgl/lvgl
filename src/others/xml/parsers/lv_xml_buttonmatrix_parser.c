/**
 * @file lv_xml_buttonmatrix_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_buttonmatrix_parser.h"
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
static lv_buttonmatrix_ctrl_t ctrl_text_to_enum_value(const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_buttonmatrix_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);

    void * item = lv_buttonmatrix_create(lv_xml_state_get_parent(state));
    return item;
}

void lv_xml_buttonmatrix_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);
    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("map", name)) {
            char * value2 = lv_strdup(value);
            uint32_t str_array_size = 8;
            char ** str_array = lv_malloc(sizeof(char *) * str_array_size);
            char * text_start = NULL;
            /*Count the `'`s (`\'`) is an escape sequence*/
            uint32_t j = 0;
            uint32_t btn_cnt = 0;
            bool in_text = false;
            for(j = 0; value2[j]; j++) {
                /*Skip the escaped `\`*/
                if(value2[j] == '\\' && value2[j + 1] == '\\') {
                    j++;
                    continue;
                }

                if(value2[j] == '\'') {
                    /*Not escaped*/
                    if(j == 0 || value2[j - 1] != '\\') {
                        if(!in_text) {
                            in_text = true;
                            text_start = &value2[j + 1];
                        }
                        /*Trailing*/
                        else {
                            value2[j] = '\0';
                            if(btn_cnt >= str_array_size) {
                                str_array_size += 4;
                                str_array = lv_realloc(str_array, sizeof(char *) * str_array_size);
                            }
                            if(lv_streq("\\n", text_start)) text_start = "\n";
                            str_array[btn_cnt] = lv_strdup(text_start);
                            btn_cnt++;
                            in_text = false;
                        }
                    }
                }
            }

            lv_free(value2);

            if(btn_cnt >= str_array_size) {
                str_array_size += 4;
                str_array = lv_realloc(str_array, sizeof(char *) * str_array_size);
            }
            str_array[btn_cnt] = NULL;

            lv_buttonmatrix_set_map(item, (const char * const *)str_array);
            ((lv_buttonmatrix_t *)item)->auto_free_map = 1;
        }
        else if(lv_streq("ctrl_map", name)) {
            char buf[512];
            lv_strlcpy(buf, value, sizeof(buf));
            char * buf_p = buf;
            char * ctrls_ored = lv_xml_split_str(&buf_p, ' ');
            uint32_t btn_i = 0;
            while(ctrls_ored) {
                lv_buttonmatrix_ctrl_t ctrl_enum = 0;
                char * ctrl = ctrls_ored;
                ctrl = lv_xml_split_str(&ctrls_ored, '|');
                while(ctrl) {
                    ctrl_enum |= ctrl_text_to_enum_value(ctrl);
                    ctrl = lv_xml_split_str(&ctrls_ored, '|');
                }
                lv_buttonmatrix_set_button_ctrl(item, btn_i, ctrl_enum);
                ctrls_ored = lv_xml_split_str(&buf_p, ' ');
                btn_i++;
            }
        }
        else if(lv_streq("selected_button", name)) lv_buttonmatrix_set_selected_button(item, lv_xml_atoi(value));
        else if(lv_streq("one_checked", name)) lv_buttonmatrix_set_one_checked(item, lv_xml_to_bool(value));
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_buttonmatrix_ctrl_t ctrl_text_to_enum_value(const char * txt)
{
    if(lv_streq("none", txt)) return LV_BUTTONMATRIX_CTRL_NONE;
    if(lv_streq("width_1", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_1;
    if(lv_streq("width_2", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_2;
    if(lv_streq("width_3", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_3;
    if(lv_streq("width_4", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_4;
    if(lv_streq("width_5", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_5;
    if(lv_streq("width_6", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_6;
    if(lv_streq("width_7", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_7;
    if(lv_streq("width_8", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_8;
    if(lv_streq("width_9", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_9;
    if(lv_streq("width_10", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_10;
    if(lv_streq("width_11", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_11;
    if(lv_streq("width_12", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_12;
    if(lv_streq("width_13", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_13;
    if(lv_streq("width_14", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_14;
    if(lv_streq("width_15", txt)) return LV_BUTTONMATRIX_CTRL_WIDTH_15;
    if(lv_streq("hidden", txt)) return LV_BUTTONMATRIX_CTRL_HIDDEN;
    if(lv_streq("no_repeat", txt)) return LV_BUTTONMATRIX_CTRL_NO_REPEAT;
    if(lv_streq("disabled", txt)) return LV_BUTTONMATRIX_CTRL_DISABLED;
    if(lv_streq("checkable", txt)) return LV_BUTTONMATRIX_CTRL_CHECKABLE;
    if(lv_streq("checked", txt)) return LV_BUTTONMATRIX_CTRL_CHECKED;
    if(lv_streq("click_trig", txt)) return LV_BUTTONMATRIX_CTRL_CLICK_TRIG;
    if(lv_streq("popover", txt)) return LV_BUTTONMATRIX_CTRL_POPOVER;
    if(lv_streq("recolor", txt)) return LV_BUTTONMATRIX_CTRL_RECOLOR;
    if(lv_streq("reserved_1", txt)) return LV_BUTTONMATRIX_CTRL_RESERVED_1;
    if(lv_streq("reserved_2", txt)) return LV_BUTTONMATRIX_CTRL_RESERVED_2;
    if(lv_streq("custom_1", txt)) return LV_BUTTONMATRIX_CTRL_CUSTOM_1;
    if(lv_streq("custom_2", txt)) return LV_BUTTONMATRIX_CTRL_CUSTOM_2;

    LV_LOG_WARN("%s is an unknown value for buttonmatrix's map_ctrl", txt);
    return 0; /*Return 0 in lack of a better option. */
}


#endif /* LV_USE_XML */
