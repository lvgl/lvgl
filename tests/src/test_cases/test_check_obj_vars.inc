/**
 * test_check_obj_vars.inc
 *
 * Static variables and forward declarations shared by all test_check_obj_*.c
 * variants. Must be included before setUp/tearDown are defined in the .c file,
 * and before test_check_obj_impl.inc.
 */

#include "unity/unity.h"
#include <string.h>

#define TEST_ASSERT_LOG_CONTAINS(substr) \
    do { \
        if(strstr(last_log_buf, substr) == NULL) { \
            TEST_PRINTF("Expected log to contain: '%s'. Actual log: '%s'", substr, last_log_buf); \
            TEST_FAIL(); \
        } \
    } while(0)

static bool log_warned;
static char last_log_buf[512];
static void log_cb(lv_log_level_t level, const char * buf);

