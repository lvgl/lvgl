/**
 * @file lv_canonical_fixture.h
 *
 */

#ifndef LV_CANONICAL_FIXTURE_H
#define LV_CANONICAL_FIXTURE_H

#include "lv_api_fixture.h"

/**
 * TEST canonical typedef brief.
 *
 * TEST canonical typedef detail for a record count.
 */
typedef int test_canonical_number_t;

/**
 * TEST canonical enum brief.
 *
 * TEST canonical enum detail for record states.
 */
typedef enum test_canonical_state {
    TEST_CANONICAL_IDLE,  /**< TEST canonical idle member documentation. */
    TEST_CANONICAL_READY, /**< TEST canonical ready member documentation. */
} test_canonical_state_t;

/**
 * TEST canonical struct brief.
 *
 * TEST canonical struct detail for a typed record.
 */
typedef struct test_canonical_record {
    test_canonical_number_t count; /**< TEST canonical count field documentation. */
    test_canonical_state_t state;  /**< TEST canonical state field documentation. */
} test_canonical_record_t;

/**
 * TEST canonical union brief.
 *
 * TEST canonical union detail for a numeric value.
 */
typedef union test_canonical_value {
    int integer; /**< TEST canonical integer field documentation. */
    float real;  /**< TEST canonical real field documentation. */
} test_canonical_value_t;

/**
 * TEST canonical variable brief.
 *
 * TEST canonical variable detail for the immutable record limit.
 */
extern const test_canonical_number_t test_canonical_limit;

/**
 * TEST canonical helper brief.
 *
 * TEST canonical helper detail for record initialization.
 */
void test_function(void);

/**
 * TEST paragraph parameter function.
 *
 * A second function paragraph.
 * @param text First parameter paragraph.
 *
 *              Second parameter paragraph.
 */
void test_parameter_paragraphs(const char * text);

/**
 * TEST canonical function brief.
 *
 * TEST canonical function detail. The record can describe a `test_record_t`.
 * Call `test_function()` before updating it.
 * @param  record            TEST canonical record parameter documentation.
 *                           Use the state of `test_canonical_record_t`.
 * @param  values            TEST canonical array parameter documentation.
 *                           TEST canonical array continuation documentation.
 *                           Accepted sample values:
 *                           - TEST canonical first sample description
 *                           - TEST canonical second sample description
 * @param  label             TEST canonical nullable parameter documentation. @nullable
 *                           TEST canonical nullable continuation documentation for `NULL`.
 * @param[out]  written      TEST canonical output parameter documentation.
 *                           TEST canonical output continuation documentation.
 * @return  TEST canonical return documentation for `test_canonical_record_t`.
 *          TEST canonical return continuation documentation. Call `test_function()`
 *          before reuse; failure returns `NULL`.
 * @note  TEST canonical note documentation.
 * @see  `test_function()` for TEST canonical see documentation.
 * @deprecated  TEST canonical deprecated documentation. Use `test_function()` instead.
 */
test_canonical_record_t * test_canonical_record_update(test_canonical_record_t * record,
                                                       const int values[], const char * label, int * written);

/* Match LVGL's default marker expansion without adding export metadata. */
#ifndef LV_EXPORT_CONST_INT
    #define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning
#endif

/**
 * TEST exported macro documentation.
 */
#define TEST_EXPORTED_CONST (TEST_OTHER + 10)
LV_EXPORT_CONST_INT(TEST_EXPORTED_CONST);

#endif /*LV_CANONICAL_FIXTURE_H*/
