/**
 * @file lv_xml_test.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_xml_test.h"
#if LV_USE_XML

#include "../../lvgl.h"
#include "lv_xml.h"
#include "lv_xml_utils.h"
#include "../../misc/lv_fs.h"
#include "../../libs/expat/expat.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void start_metadata_handler(void * user_data, const char * name, const char ** attrs);
static void end_metadata_handler(void * user_data, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_xml_test_init(void)
{

}

lv_xml_test_t * lv_xml_test_register_from_data(const char * name, const char * xml_def)
{

	/*Register as a component first to allow creating the view of the test later*/
	lv_result_t res = lv_xml_component_register_from_data(name, xml_def);
	if(res != LV_RESULT_OK) {
		LV_LOG_WARN("Couldn't register test `%s` as a component");
		return NULL;
	}

	lv_xml_test_t * test = lv_malloc(sizeof(lv_xml_test_t));
	test->name = lv_strdup(name);

    /* Parse the XML to extract metadata */
    XML_Memory_Handling_Suite mem_handlers;
    mem_handlers.malloc_fcn = lv_malloc;
    mem_handlers.realloc_fcn = lv_realloc;
    mem_handlers.free_fcn = lv_free;
    XML_Parser parser = XML_ParserCreate_MM(NULL, &mem_handlers, test);
    XML_SetUserData(parser, NULL);
    XML_SetElementHandler(parser, start_metadata_handler, end_metadata_handler);

    if(XML_Parse(parser, xml_def, lv_strlen(xml_def), XML_TRUE) == XML_STATUS_ERROR) {
        LV_LOG_ERROR("XML parsing error: %s on line %lu",
                     XML_ErrorString(XML_GetErrorCode(parser)),
                     (unsigned long)XML_GetCurrentLineNumber(parser));
        XML_ParserFree(parser);
        return LV_RESULT_INVALID;
    }

    XML_ParserFree(parser);

    return test;
}


lv_xml_test_t * lv_xml_test_register_from_file(const char * path)
{
    /* Extract component name from path */
    /* Create a copy of the filename to modify */
    char * filename = lv_strdup(lv_fs_get_last(path));
    const char * ext = lv_fs_get_ext(filename);
    filename[lv_strlen(filename) - lv_strlen(ext) - 1] = '\0'; /*Trim the extension*/

    lv_fs_res_t fs_res;
    lv_fs_file_t f;
    fs_res = lv_fs_open(&f, path, LV_FS_MODE_RD);
    if(fs_res != LV_FS_RES_OK) {
        LV_LOG_WARN("Couldn't open %s", path);
        lv_free(filename);
        return LV_RESULT_INVALID;
    }


    /* Determine file size */
    lv_fs_seek(&f, 0, LV_FS_SEEK_END);
    uint32_t file_size = 0;
    lv_fs_tell(&f, &file_size);
    lv_fs_seek(&f, 0, LV_FS_SEEK_SET);

    /* Create the buffer */
    char * xml_buf = lv_zalloc(file_size + 1);
    if(xml_buf == NULL) {
        LV_LOG_WARN("Memory allocation failed for file %s (%d bytes)", path, file_size + 1);
        lv_fs_close(&f);
        return LV_RESULT_INVALID;
    }

    /* Read the file content  */
    uint32_t rn;
    lv_fs_read(&f, xml_buf, file_size, &rn);
    if(rn != file_size) {
        LV_LOG_WARN("Couldn't read %s fully", path);
        lv_free(xml_buf);
        lv_fs_close(&f);
        return LV_RESULT_INVALID;
    }

    /* Null-terminate the buffer */
    xml_buf[rn] = '\0';

    /* Register the test */
    lv_xml_test_t * test = lv_xml_test_register_from_data(filename, xml_buf);

    /* Housekeeping */
    lv_free(xml_buf);
    lv_fs_close(&f);

    return test;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void start_metadata_handler(void * user_data, const char * name, const char ** attrs)
{

    lv_xml_test_t * test= (lv_xml_test_t *)user_data;

    if(lv_streq(name, "steps")) {
    	test->processing_steps = 1;
    	return;
    }

    /*Process the steps only*/
    if(test->processing_steps == 0) return;

    if(lv_streq(name, "click")) {
    	test->step_cnt++;
    	const char * x = lv_xml_get_value_of(attrs, "x");
    	const char * y = lv_xml_get_value_of(attrs, "y");
    	lv_realloc(test->steps, sizeof(lv_xml_test_step_t) * test->step_cnt);
    	uint32_t idx = test->step_cnt - 1;
    	test->steps[idx].type = LV_XML_TEST_STEP_TYPE_CLICK;
    	test->steps[idx].param.click.x = lv_xml_atoi(x);
    	test->steps[idx].param.click.y = lv_xml_atoi(y);

    }
    else if(lv_streq(name, "screensot_compare")) {
    	test->step_cnt++;
    	const char * path = lv_xml_get_value_of(attrs, "path");
    	lv_realloc(test->steps, sizeof(lv_xml_test_step_t) * test->step_cnt);
    	uint32_t idx = test->step_cnt - 1;
    	test->steps[idx].type = LV_XML_TEST_STEP_TYPE_SCREENSHOT_COMPARE;
    	test->steps[idx].param.screenshot_compare.path = lv_strdup(path);
    }
}

static void end_metadata_handler(void * user_data, const char * name)
{
    lv_xml_test_t * test= (lv_xml_test_t *)user_data;

	if(lv_streq(name, "steps")) {
		test->processing_steps = 0;
		return;
	}
}


#endif /* LV_USE_XML */
