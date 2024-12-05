/**
 * @file lv_xml_base_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"
#if LV_USE_XML

#include "lv_xml_base_types.h"
#include "lv_xml_private.h"
#include "lv_xml_parser.h"
#include "lv_xml_style.h"
#include "lv_xml_component_private.h"

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

int32_t lv_xml_to_size(const char * txt)
{
    if(lv_streq(txt, "content")) return LV_SIZE_CONTENT;

    int32_t v = lv_xml_atoi(txt);
    if(txt[lv_strlen(txt) - 1] == '%') return lv_pct(v);
    else return v;
}

lv_align_t lv_xml_align_string_to_enum_value(const char * txt)
{
    if(lv_streq("top_left", txt)) return LV_ALIGN_TOP_LEFT;
    if(lv_streq("top_mid", txt)) return LV_ALIGN_TOP_MID;
    if(lv_streq("top_right", txt)) return LV_ALIGN_TOP_LEFT;
    if(lv_streq("bottom_left", txt)) return LV_ALIGN_BOTTOM_LEFT;
    if(lv_streq("bottom_mid", txt)) return LV_ALIGN_BOTTOM_MID;
    if(lv_streq("bottom_right", txt)) return LV_ALIGN_BOTTOM_RIGHT;
    if(lv_streq("right_mid", txt)) return LV_ALIGN_RIGHT_MID;
    if(lv_streq("left_mid", txt)) return LV_ALIGN_LEFT_MID;
    if(lv_streq("center", txt)) return LV_ALIGN_CENTER;

    LV_LOG_WARN("%s is an unknown value for align", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_dir_t lv_xml_dir_string_to_enum_value(const char * txt)
{
    if(lv_streq("top", txt)) return LV_DIR_TOP;
    if(lv_streq("bottom", txt)) return LV_DIR_BOTTOM;
    if(lv_streq("left", txt)) return LV_DIR_LEFT;
    if(lv_streq("right", txt)) return LV_DIR_RIGHT;
    if(lv_streq("all", txt)) return LV_DIR_ALL;

    LV_LOG_WARN("%s is an unknown value for dir", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_flex_flow_t lv_xml_flex_flow_string_to_enum_value(const char * txt)
{
    if(lv_streq("column", txt)) return LV_FLEX_FLOW_COLUMN;
    if(lv_streq("column_reverse", txt)) return LV_FLEX_FLOW_COLUMN_REVERSE;
    if(lv_streq("column_wrap", txt)) return LV_FLEX_FLOW_COLUMN_WRAP;
    if(lv_streq("column_wrap_reverse", txt)) return LV_FLEX_FLOW_COLUMN_WRAP_REVERSE;
    if(lv_streq("row", txt)) return LV_FLEX_FLOW_ROW;
    if(lv_streq("row_reverse", txt)) return LV_FLEX_FLOW_ROW_REVERSE;
    if(lv_streq("row_wrap", txt)) return LV_FLEX_FLOW_ROW_WRAP;
    if(lv_streq("row_wrap_reverse", txt)) return LV_FLEX_FLOW_ROW_WRAP_REVERSE;

    LV_LOG_WARN("%s is an unknown value for flex flow", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_flex_align_t lv_xml_flex_align_string_to_enum_value(const char * txt)
{
    if(lv_streq("center", txt)) return LV_FLEX_ALIGN_CENTER;
    if(lv_streq("end", txt)) return LV_FLEX_ALIGN_END;
    if(lv_streq("start", txt)) return LV_FLEX_ALIGN_START;
    if(lv_streq("space_around", txt)) return LV_FLEX_ALIGN_SPACE_AROUND;
    if(lv_streq("space_between", txt)) return LV_FLEX_ALIGN_SPACE_BETWEEN;
    if(lv_streq("space_evenly", txt)) return LV_FLEX_ALIGN_SPACE_EVENLY;

    LV_LOG_WARN("%s is an unknown value for flex align", txt);
    return 0; /*Return 0 in lack of a better option. */
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
