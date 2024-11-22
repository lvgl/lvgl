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

void lv_xml_check_file(const char * filepath)
{
    lv_fs_file_t f;
    lv_fs_res_t res = lv_fs_open(&f, filepath, LV_FS_MODE_RD);

    if(res == LV_FS_RES_OK) {
        uint32_t size;
        uint8_t buffer[10];
        lv_fs_read(&f, buffer, 0, &size);

        lv_fs_seek(&f, 0, LV_FS_SEEK_END);
        lv_fs_tell(&f, &size);
        LV_LOG_USER("File %s exists (%u bytes)", filepath, size);


        lv_fs_close(&f);

        lv_image_header_t info;
        lv_image_decoder_get_info(filepath, &info);
        LV_LOG_USER("Image info: %d x %d, %d color", info.w, info.h, info.cf);
    }
    else {
        LV_LOG_ERROR("Failed to open file: %s", filepath);
    }
}

void lv_xml_image_apply(lv_xml_parser_state_t * state, const char ** attrs)
{
    void * item = lv_xml_state_get_item(state);

    if(item == NULL) {
        LV_LOG_ERROR("Failed to get image");
        return;
    }

    lv_xml_obj_apply(state, attrs); /*Apply the common properties, e.g. width, height, styles flags etc*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];

        if(lv_streq("src", name)) {
            const void * img_src = lv_xml_get_image(value);
            if(img_src == NULL) {
                LV_LOG_WARN("Failed to get image source for '%s'", value);
                return;
            }

            lv_image_set_src(item, (const char *)img_src);

        }

    }

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
