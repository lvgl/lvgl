/**
 * @file lv_xml_image_parser.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_image_parser.h"
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
static lv_image_align_t image_align_to_enum(const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_xml_image_create(lv_xml_parser_state_t * state, const char ** attrs)
{
    LV_UNUSED(attrs);
    void * item = lv_image_create(lv_xml_state_get_parent(state));

    if(item == NULL) {
        LV_LOG_ERROR("Failed to create image");
        return NULL;
    }

    return item;
}

void lv_xml_image_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("src", name)) lv_image_set_src(item, lv_xml_get_image(&state->scope, value));
        if(lv_streq("inner_align", name)) lv_image_set_inner_align(item, image_align_to_enum(value));
        if(lv_streq("rotation", name)) lv_image_set_rotation(item, lv_xml_atoi(value));
        if(lv_streq("scale_x", name)) lv_image_set_scale_x(item, lv_xml_atoi(value));
        if(lv_streq("scale_y", name)) lv_image_set_scale_y(item, lv_xml_atoi(value));
        if(lv_streq("pivot", name)) {
            int32_t x = lv_xml_atoi_split(&value, ' ');
            int32_t y = lv_xml_atoi_split(&value, ' ');
            lv_image_set_pivot(item, x, y);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_image_align_t image_align_to_enum(const char * txt)
{
    if(lv_streq("top_left", txt)) return LV_IMAGE_ALIGN_TOP_LEFT;
    if(lv_streq("top_mid", txt)) return LV_IMAGE_ALIGN_TOP_MID;
    if(lv_streq("top_right", txt)) return LV_IMAGE_ALIGN_TOP_RIGHT;
    if(lv_streq("bottom_left", txt)) return LV_IMAGE_ALIGN_BOTTOM_LEFT;
    if(lv_streq("bottom_mid", txt)) return LV_IMAGE_ALIGN_BOTTOM_MID;
    if(lv_streq("bottom_right", txt)) return LV_IMAGE_ALIGN_BOTTOM_RIGHT;
    if(lv_streq("right_mid", txt)) return LV_IMAGE_ALIGN_RIGHT_MID;
    if(lv_streq("left_mid", txt)) return LV_IMAGE_ALIGN_LEFT_MID;
    if(lv_streq("center", txt)) return LV_IMAGE_ALIGN_CENTER;
    if(lv_streq("stretch", txt)) return LV_IMAGE_ALIGN_STRETCH;
    if(lv_streq("tile", txt)) return LV_IMAGE_ALIGN_TILE;

    LV_LOG_WARN("%s is an unknown value for image align", txt);
    return 0; /*Return 0 in lack of a better option. */
}


#endif /* LV_USE_XML */
