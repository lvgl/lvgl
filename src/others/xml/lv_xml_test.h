/**
 * @file lv_xml_test.h
 *
 */

#ifndef LV_LABEL_XML_TEST_H
#define LV_LABEL_XML_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_types.h"
#if LV_USE_XML

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
	LV_XML_TEST_STEP_TYPE_CLICK,
	LV_XML_TEST_STEP_TYPE_WAIT,
	LV_XML_TEST_STEP_TYPE_SCREENSHOT_COMPARE,
}lv_xml_test_step_type_t;

typedef struct {
	lv_xml_test_step_type_t type;
	union {
		struct {
			int32_t x;
			int32_t y;
		}click;

		struct {
			int32_t ms;
		}wait;

		struct {
			const char * path;
		}screenshot_compare;
	}param;
}lv_xml_test_step_t;

typedef struct {
	const char * name;
	uint32_t step_cnt;
	uint32_t step_act;
	lv_xml_test_step_t * steps;
	uint32_t processing_steps :1;
}lv_xml_test_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Load the styles, constants, another data of the test. It needs to be called only once for each test.
 * @param name      the name as the test will be referenced later in other tests
 * @param xml_def   the XML definition of the test as a NULL terminated string
 * @return          LV_RES_OK: loaded successfully, LV_RES_INVALID: otherwise
 */
lv_xml_test_t * lv_xml_test_register_from_data(const char * name, const char * xml_def);

/**
 * Load the styles, constants, another data of the test. It needs to be called only once for each test.
 * @param path      path to an XML file
 * @return          LV_RES_OK: loaded successfully, LV_RES_INVALID: otherwise
 */
lv_xml_test_t * lv_xml_test_register_from_file(const char * path);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_TEST_H*/


