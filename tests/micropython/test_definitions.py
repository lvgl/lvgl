#LVGL MicroPython testing definitions (error codes, etc.) common for Python3 and MicroPython


#testing mode setting (error-forcing mode can be used to test the test-environment itself)
#TESTMODE_NORMAL      = 1
#TESTMODE_FORCE_ERROR = 0
TESTMODE = 1


#status codes (script return values)
RESULT_OK = 0

ERROR_BINDING_SYSTEM_FAULT = 255
ERROR_NOT_ALL_TESTS_PASSED = 254
ERROR_TESTCASE_NOT_GIVEN   = 253
ERROR_TESTCASE_NOT_FOUND   = 252
ERROR_TESTCASE_FAILED      = 251

#ERROR_SUBTEST_1_FAILED     = 241
#ERROR_SUBTEST_2_FAILED     = 242
#ERROR_SUBTEST_3_FAILED     = 243
#ERROR_SUBTEST_4_FAILED     = 244

WARNING_SUBTEST_COUNT_MISMATCH = 200
