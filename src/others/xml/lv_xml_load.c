/**
 * @file lv_xml_load.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_xml_load.h"
#if LV_USE_XML

#include "lv_xml_private.h"
#include "../../libs/expat/expat.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_XML_TYPE_UNKNOWN,
    LV_XML_TYPE_COMPONENT,
    LV_XML_TYPE_TRANSLATIONS,
} lv_xml_type_t;

typedef struct {
    XML_Parser parser;
    lv_xml_type_t type;
} load_from_file_parser_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t load_all_recursive(char * path_buf, const char * path);
static void load_from_file(const char * path);
static char * path_filename_without_extension(const char * path);
static void load_from_file_start_element_handler(void * user_data, const char * name, const char ** attrs);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_xml_load_all_from_path(const char * path)
{
    char path_buf[LV_FS_MAX_PATH_LENGTH];

    /* set the default asset path to the pack path so XML asset paths are relative to it */
    const char * asset_path = path;
    /* end the asset path with a '/' */
    char path_last_char = path[0] ? path[lv_strlen(path) - 1] : '\0';
    if(path_last_char != '\0' && path_last_char != ':'
       && path_last_char != '/' && path_last_char != '\\') {
        lv_snprintf(path_buf, sizeof(path_buf), "%s/", path);
        asset_path = path_buf;
    }
    lv_xml_set_default_asset_path(asset_path);

    return load_all_recursive(path_buf, path);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_result_t load_all_recursive(char * path_buf, const char * path)
{
    lv_result_t ret = LV_RESULT_OK;
    lv_fs_res_t fs_res;

    lv_fs_dir_t dir;
    fs_res = lv_fs_dir_open(&dir, path);
    if(fs_res != LV_FS_RES_OK) {
        LV_LOG_WARN("Couldn't open directory %s", path);
        return LV_RESULT_INVALID;
    }

    while(1) {
        fs_res = lv_fs_dir_read(&dir, path_buf, LV_FS_MAX_PATH_LENGTH);
        if(fs_res != LV_FS_RES_OK) {
            LV_LOG_WARN("Couldn't read directory %s", path);
            ret = LV_RESULT_INVALID;
            goto dir_close_out;
        }

        if(path_buf[0] == '\0') {
            break;
        }

        int full_path_len = lv_fs_path_join(NULL, 0, path, path_buf);
        char * full_path = lv_malloc(full_path_len + 1);
        LV_ASSERT_MALLOC(full_path);
        lv_fs_path_join(full_path, full_path_len + 1, path, path_buf);

        if(path_buf[0] == '/') {
            ret = load_all_recursive(path_buf, full_path);
        }
        else {
            load_from_file(full_path);
        }

        lv_free(full_path);

        if(ret != LV_RESULT_OK) {
            goto dir_close_out;
        }
    }

dir_close_out:
    fs_res = lv_fs_dir_close(&dir);
    if(fs_res != LV_FS_RES_OK) {
        LV_LOG_WARN("Error closing directory %s", path);
        ret = LV_RESULT_INVALID;
    }

    return ret;
}

static void load_from_file(const char * path)
{
    const char * ext = lv_fs_get_ext(path);

    if(lv_streq(ext, "xml")) {
        lv_fs_res_t fs_res;
        lv_fs_file_t f;
        fs_res = lv_fs_open(&f, path, LV_FS_MODE_RD);
        if(fs_res != LV_FS_RES_OK) {
            LV_LOG_WARN("Couldn't open %s", path);
            return;
        }

        /* Determine file size */
        lv_fs_seek(&f, 0, LV_FS_SEEK_END);
        uint32_t file_size = 0;
        lv_fs_tell(&f, &file_size);
        lv_fs_seek(&f, 0, LV_FS_SEEK_SET);

        /* Create the buffer */
        char * xml_buf = lv_malloc(file_size + 1);
        if(xml_buf == NULL) {
            LV_LOG_WARN("Memory allocation failed for file %s (%d bytes)", path, file_size + 1);
            lv_fs_close(&f);
            return;
        }

        /* Read the file content  */
        uint32_t rn;
        fs_res = lv_fs_read(&f, xml_buf, file_size, &rn);
        if(fs_res != LV_FS_RES_OK || rn != file_size) {
            LV_LOG_WARN("Couldn't read %s fully", path);
            lv_free(xml_buf);
            lv_fs_close(&f);
            return;
        }

        lv_fs_close(&f);

        /* Null-terminate the buffer */
        xml_buf[rn] = '\0';

        load_from_file_parser_data_t parser_data;

        XML_Memory_Handling_Suite mem_handlers;
        mem_handlers.malloc_fcn = lv_malloc;
        mem_handlers.realloc_fcn = lv_realloc;
        mem_handlers.free_fcn = lv_free;
        XML_Parser parser = XML_ParserCreate_MM(NULL, &mem_handlers, NULL);
        parser_data.parser = parser;
        parser_data.type = LV_XML_TYPE_UNKNOWN;
        XML_SetUserData(parser, &parser_data);
        XML_SetStartElementHandler(parser, load_from_file_start_element_handler);

        /* Parse the XML */
        enum XML_Status parser_res = XML_Parse(parser, xml_buf, file_size, XML_TRUE);
        enum XML_Error parser_error = XML_GetErrorCode(parser);
        if(parser_res == XML_STATUS_ERROR && parser_error != XML_ERROR_ABORTED) {
            LV_LOG_WARN("XML parsing error: %s on line %lu", XML_ErrorString(parser_error),
                        XML_GetCurrentLineNumber(parser));
            XML_ParserFree(parser);
            lv_free(xml_buf);
            return;
        }
        XML_ParserFree(parser);

        switch(parser_data.type) {
            case LV_XML_TYPE_COMPONENT: {
                    char * component_name = path_filename_without_extension(path);
                    lv_xml_component_register_from_data(component_name, xml_buf);
                    lv_free(component_name);
                    break;
                }
            case LV_XML_TYPE_TRANSLATIONS:
#if LV_USE_TRANSLATION
                lv_xml_translation_register_from_data(xml_buf);
#else
                LV_LOG_WARN("Translation XML found but translations not enabled");
#endif
                break;
            default:
                LV_LOG_WARN("Unknown XML type found in pack");
                break;
        }

        lv_free(xml_buf);
    }
    else {
        LV_LOG_INFO("Did not use '%s' from XML pack.", path);
    }
}

static char * path_filename_without_extension(const char * path)
{
    const char * last = lv_fs_get_last(path);
    const char * ext = lv_fs_get_ext(last);
    char * filename = lv_strdup(last);
    LV_ASSERT_MALLOC(filename);
    if(ext[0]) {
        filename[lv_strlen(last) - lv_strlen(ext) - 1] = '\0'; /*Trim the extension*/
    }
    return filename;
}

static void load_from_file_start_element_handler(void * user_data, const char * name, const char ** attrs)
{
    LV_UNUSED(attrs);
    load_from_file_parser_data_t * data = user_data;

    if(lv_streq(name, "component") || lv_streq(name, "screen") || lv_streq(name, "globals")) {
        data->type = LV_XML_TYPE_COMPONENT;
    }
    else if(lv_streq(name, "translations")) {
        data->type = LV_XML_TYPE_TRANSLATIONS;
    }

    XML_StopParser(data->parser, XML_FALSE);
}

#endif /*LV_USE_XML*/
