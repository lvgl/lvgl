/**
 * @file lv_xml_base_types.h
 *
 */

#ifndef LV_XML_BASE_TYPES_H
#define LV_XML_BASE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../misc/lv_style.h"
#if LV_USE_XML

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Convert an state string to enum
 * @param txt       e.g. "pressed"
 * @return          the related enum, e.g. `LV_STATE_PRESSED`
 */
lv_state_t lv_xml_state_to_enum(const char * txt);

/**
 * Process inputs "content", "32", "32px", or "25%"
 * and convert them to integer
 * @param txt       the input string
 * @return          the integer size
 */
int32_t lv_xml_to_size(const char * txt);


/**
 * Convert an align string to enum
 * @param txt       e.g. "center"
 * @return          the related enum, e.g. `LV_ALIGN_CENTER`
 */
lv_align_t lv_xml_align_to_enum(const char * txt);

/**
 * Convert a direction string to enum
 * @param txt       e.g. "top"
 * @return          the related enum, e.g. `LV_DIR_TOP`
 */
lv_dir_t lv_xml_dir_to_enum(const char * txt);

/**
 * Convert a direction string to enum
 * @param txt       e.g. "top"
 * @return          the related enum, e.g. `LV_BORDER_SIDE_TOP`
 */
lv_border_side_t lv_xml_border_side_to_enum(const char * txt);

/**
 * Convert a base dir string to enum
 * @param txt       e.g. "rtl"
 * @return          the related enum, e.g. `LV_BASE_DIR_RTL`
 */
lv_base_dir_t lv_xml_base_dir_to_enum(const char * txt);

/**
 * Convert a grad dir string to enum
 * @param txt       e.g. "hor"
 * @return          the related enum, e.g. `LV_GRAD_DIR_HOR`
 */
lv_grad_dir_t lv_xml_grad_dir_to_enum(const char * txt);

/**
 * Convert a text align string to enum
 * @param txt       e.g. "left"
 * @return          the related enum, e.g. `LV_TEXT_ALIGN_LEFT`
 */
lv_text_align_t lv_xml_text_align_to_enum(const char * txt);

/**
 * Convert a text decor string to enum
 * @param txt       e.g. "underline"
 * @return          the related enum, e.g. `LV_TEXT_DECOR_UNDERLINE`
 */
lv_text_decor_t lv_xml_text_decor_to_enum(const char * txt);

/**
 * Convert a scroll snap string to enum
 * @param txt       e.g. "start"
 * @return          the related enum, e.g. `LV_SCROLL_SNAP_START`
 */
lv_scroll_snap_t lv_xml_scroll_snap_to_enum(const char * txt);

/**
 * Convert a scrollbar mode string to enum
 * @param txt       e.g. "active"
 * @return          the related enum, e.g. `LV_SCROLLBAR_MODE_ACTIVE`
 */
lv_scrollbar_mode_t lv_xml_scrollbar_mode_to_enum(const char * txt);

/**
 * Convert a flex flow string to enum
 * @param txt       e.g. "row_wrap"
 * @return          the related enum, e.g. `LV_FLEX_FLOW_ROW_WRAP`
 */
lv_flex_flow_t lv_xml_flex_flow_to_enum(const char * txt);

/**
 * Convert a flex align string to enum
 * @param txt       e.g. "space_between"
 * @return          the related enum, e.g. `LV_FLEX_ALIGN_SPACE_BETWEEN`
 */
lv_flex_align_t lv_xml_flex_align_to_enum(const char * txt);

/**
 * Convert a grid align string to enum
 * @param txt       e.g. "space_between"
 * @return          the related enum, e.g. `LV_GRID_ALIGN_SPACE_BETWEEN`
 */
lv_grid_align_t lv_xml_grid_align_to_enum(const char * txt);

/**
 * Convert a layout string to enum
 * @param txt       e.g. "flex"
 * @return          the related enum, e.g. `LV_LAYOUT_FLEX`
 */
lv_layout_t lv_xml_layout_to_enum(const char * txt);

/**
 * Convert a blend mode string to enum
 * @param txt       e.g. "additive"
 * @return          the related enum, e.g. `LV_BLEND_MODE_ADDITIVE`
 */
lv_blend_mode_t lv_xml_blend_mode_to_enum(const char * txt);

/**
 * Convert a blur quality string to enum
 * @param txt       e.g. "auto"
 * @return          the related enum, e.g. `LV_BLUR_QUALITY_AUTO`
 */
lv_blur_quality_t lv_xml_blur_quality_to_enum(const char * txt);

/**
 * Convert an event trigger string to enum
 * @param txt       e.g. "clicked"
 * @return          the related enum, e.g. `LV_EVENT_CLICKED`
 */
lv_event_code_t lv_xml_trigger_text_to_enum_value(const char * txt);

/**
 * Convert a screen animation type string to enum
 * @param txt       e.g. "over_right"
 * @return          the related enum, e.g. `LV_SCREEN_LOAD_ANIM_OVER_RIGHT`
 */
lv_screen_load_anim_t lv_xml_screen_load_anim_text_to_enum_value(const char * txt);

/**
 * Convert a style property string to enum
 * @param txt       e.g. "bg_color"
 * @return          the related enum, e.g. `LV_STYLE_BG_COLOR` or
 *                  `LV_STYLE_PROP_INV` if not found.
 */
lv_style_prop_t lv_xml_style_prop_to_enum(const char * txt);


/**
 * Convert a style state to enum
 * @param txt       e.g. "pressed"
 * @return          the enum `LV_STATE_PRESSED`
 */
lv_state_t lv_xml_style_state_to_enum(const char * txt);

/**
 * Convert a style part to enum
 * @param txt       e.g. "knob"
 * @return          the enum `LV_PART_KNOB`
 */
lv_part_t lv_xml_style_part_to_enum(const char * txt);


/**
 * Convert ORed style parts and states to an ORed selector
 * @param txt       e.g. "knob|pressed"
 * @return          the enum `LV_PART_KNOB|LV_STATE_PRESSED`
 */
lv_style_selector_t lv_xml_style_selector_text_to_enum(const char * str);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_BASE_TYPES_H*/
