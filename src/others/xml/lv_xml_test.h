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
    LV_XML_TEST_STEP_TYPE_NONE,
    LV_XML_TEST_STEP_TYPE_CLICK_AT,
    LV_XML_TEST_STEP_TYPE_WAIT,
    LV_XML_TEST_STEP_TYPE_FREEZE,
    LV_XML_TEST_STEP_TYPE_SCREENSHOT_COMPARE,
} lv_xml_test_step_type_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Load the styles, constants, another data of the test. It needs to be called only once for each test.
 * @param name                      the name as the test will be referenced later in other tests
 * @param xml_def                   the XML definition of the test as a NULL terminated string
 * @param ref_image_path_prefix     prefix for the path of reference images
 * @return          LV_RES_OK: loaded successfully, LV_RES_INVALID: otherwise
 */
lv_result_t lv_xml_test_register_from_data(const char * xml_def, const char * ref_image_path_prefix);

/**
 * Load the styles, constants, another data of the test. It needs to be called only once for each test.
 * @param path                      path to an XML file
 * @param ref_image_path_prefix     prefix for the path of reference images
 * @return                          LV_RES_OK: loaded successfully, LV_RES_INVALID: otherwise
 */
lv_result_t lv_xml_test_register_from_file(const char * path, const char * ref_image_path_prefix);

/**
 * Free resources allocated for testing.
 */
void lv_xml_test_unregister(void);

/**
 * Run all the test steps
 * @param slowdown  0: max speed, 1: real speed, 2: half speed, ... ,10: ten times slower
 * @return          number of failed tests
 */
uint32_t lv_xml_test_run(uint32_t slowdown);

/**
 * Get the number of steps in a test
 * @return      the number of `<step>`s
 */
uint32_t lv_xml_test_get_step_count(void);

/**
 * Get the type of a step
 * @param idx   the index of a step (< step_count)
 * @return      element of `lv_xml_test_step_type_t`
 */
lv_xml_test_step_type_t lv_xml_test_get_step_type(uint32_t idx);

/**
 * Check if the a step was passed. Can be called after lv_xml_test_run()
 * @param idx   the index of a step (< step_count)
 * @return      true: the step passed, false: the step failed
 */
bool lv_xml_test_get_status(uint32_t idx);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_XML */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_XML_TEST_H*/


