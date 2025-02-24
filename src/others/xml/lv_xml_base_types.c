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

lv_align_t lv_xml_align_to_enum(const char * txt)
{
    if(lv_streq("top_left", txt)) return LV_ALIGN_TOP_LEFT;
    if(lv_streq("top_mid", txt)) return LV_ALIGN_TOP_MID;
    if(lv_streq("top_right", txt)) return LV_ALIGN_TOP_RIGHT;
    if(lv_streq("bottom_left", txt)) return LV_ALIGN_BOTTOM_LEFT;
    if(lv_streq("bottom_mid", txt)) return LV_ALIGN_BOTTOM_MID;
    if(lv_streq("bottom_right", txt)) return LV_ALIGN_BOTTOM_RIGHT;
    if(lv_streq("right_mid", txt)) return LV_ALIGN_RIGHT_MID;
    if(lv_streq("left_mid", txt)) return LV_ALIGN_LEFT_MID;
    if(lv_streq("center", txt)) return LV_ALIGN_CENTER;

    LV_LOG_WARN("%s is an unknown value for align", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_dir_t lv_xml_dir_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_DIR_NONE;
    if(lv_streq("top", txt)) return LV_DIR_TOP;
    if(lv_streq("bottom", txt)) return LV_DIR_BOTTOM;
    if(lv_streq("left", txt)) return LV_DIR_LEFT;
    if(lv_streq("right", txt)) return LV_DIR_RIGHT;
    if(lv_streq("all", txt)) return LV_DIR_ALL;

    LV_LOG_WARN("%s is an unknown value for dir", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_border_side_t lv_xml_border_side_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_BORDER_SIDE_NONE;
    if(lv_streq("top", txt)) return LV_BORDER_SIDE_TOP;
    if(lv_streq("bottom", txt)) return LV_BORDER_SIDE_BOTTOM;
    if(lv_streq("left", txt)) return LV_BORDER_SIDE_LEFT;
    if(lv_streq("right", txt)) return LV_BORDER_SIDE_RIGHT;
    if(lv_streq("full", txt)) return LV_BORDER_SIDE_FULL;

    LV_LOG_WARN("%s is an unknown value for border_side", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_grad_dir_t lv_xml_grad_dir_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_GRAD_DIR_NONE;
    if(lv_streq("hor", txt)) return LV_GRAD_DIR_HOR;
    if(lv_streq("ver", txt)) return LV_GRAD_DIR_VER;

    LV_LOG_WARN("%s is an unknown value for grad_dir", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_base_dir_t lv_xml_base_dir_to_enum(const char * txt)
{
    if(lv_streq("auto", txt)) return LV_BASE_DIR_AUTO;
    if(lv_streq("ltr", txt)) return LV_BASE_DIR_LTR;
    if(lv_streq("rtl", txt)) return LV_BASE_DIR_RTL;

    LV_LOG_WARN("%s is an unknown value for base_dir", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_text_align_t lv_xml_text_align_to_enum(const char * txt)
{
    if(lv_streq("left", txt)) return LV_TEXT_ALIGN_LEFT;
    if(lv_streq("right", txt)) return LV_TEXT_ALIGN_RIGHT;
    if(lv_streq("center", txt)) return LV_TEXT_ALIGN_CENTER;
    if(lv_streq("auto", txt)) return LV_TEXT_ALIGN_AUTO;

    LV_LOG_WARN("%s is an unknown value for text_align", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_text_decor_t lv_xml_text_decor_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_TEXT_DECOR_NONE;
    if(lv_streq("underline", txt)) return LV_TEXT_DECOR_UNDERLINE;
    if(lv_streq("strikethrough", txt)) return LV_TEXT_DECOR_STRIKETHROUGH;

    LV_LOG_WARN("%s is an unknown value for text_decor", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_flex_flow_t lv_xml_flex_flow_to_enum(const char * txt)
{
    if(lv_streq("column", txt)) return LV_FLEX_FLOW_COLUMN;
    if(lv_streq("column_reverse", txt)) return LV_FLEX_FLOW_COLUMN_REVERSE;
    if(lv_streq("column_wrap", txt)) return LV_FLEX_FLOW_COLUMN_WRAP;
    if(lv_streq("column_wrap_reverse", txt)) return LV_FLEX_FLOW_COLUMN_WRAP_REVERSE;
    if(lv_streq("row", txt)) return LV_FLEX_FLOW_ROW;
    if(lv_streq("row_reverse", txt)) return LV_FLEX_FLOW_ROW_REVERSE;
    if(lv_streq("row_wrap", txt)) return LV_FLEX_FLOW_ROW_WRAP;
    if(lv_streq("row_wrap_reverse", txt)) return LV_FLEX_FLOW_ROW_WRAP_REVERSE;

    LV_LOG_WARN("%s is an unknown value for flex_flow", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_flex_align_t lv_xml_flex_align_to_enum(const char * txt)
{
    if(lv_streq("center", txt)) return LV_FLEX_ALIGN_CENTER;
    if(lv_streq("end", txt)) return LV_FLEX_ALIGN_END;
    if(lv_streq("start", txt)) return LV_FLEX_ALIGN_START;
    if(lv_streq("space_around", txt)) return LV_FLEX_ALIGN_SPACE_AROUND;
    if(lv_streq("space_between", txt)) return LV_FLEX_ALIGN_SPACE_BETWEEN;
    if(lv_streq("space_evenly", txt)) return LV_FLEX_ALIGN_SPACE_EVENLY;

    LV_LOG_WARN("%s is an unknown value for flex_align", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_grid_align_t lv_xml_grid_align_to_enum(const char * txt)
{
    if(lv_streq("center", txt)) return LV_GRID_ALIGN_CENTER;
    if(lv_streq("end", txt)) return LV_GRID_ALIGN_END;
    if(lv_streq("start", txt)) return LV_GRID_ALIGN_START;
    if(lv_streq("stretch", txt)) return LV_GRID_ALIGN_STRETCH;
    if(lv_streq("space_around", txt)) return LV_GRID_ALIGN_SPACE_AROUND;
    if(lv_streq("space_between", txt)) return LV_GRID_ALIGN_SPACE_BETWEEN;
    if(lv_streq("space_evenly", txt)) return LV_GRID_ALIGN_SPACE_EVENLY;

    LV_LOG_WARN("%s is an unknown value for grid_align", txt);
    return 0; /*Return 0 in lack of a better option. */
}


lv_layout_t lv_xml_layout_to_enum(const char * txt)
{
    if(lv_streq("none", txt)) return LV_LAYOUT_NONE;
    if(lv_streq("flex", txt)) return LV_LAYOUT_FLEX;
    if(lv_streq("grid", txt)) return LV_LAYOUT_GRID;

    LV_LOG_WARN("%s is an unknown value for layout", txt);
    return 0; /*Return 0 in lack of a better option. */
}

lv_blend_mode_t lv_xml_blend_mode_to_enum(const char * txt)
{
    if(lv_streq("normal", txt)) return LV_BLEND_MODE_NORMAL;
    if(lv_streq("additive", txt)) return LV_BLEND_MODE_ADDITIVE;
    if(lv_streq("subtractive", txt)) return LV_BLEND_MODE_SUBTRACTIVE;
    if(lv_streq("multiply", txt)) return LV_BLEND_MODE_MULTIPLY;
    if(lv_streq("difference", txt)) return LV_BLEND_MODE_DIFFERENCE;

    LV_LOG_WARN("%s is an unknown value for blend_mode", txt);
    return 0; /*Return 0 in lack of a better option. */
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
