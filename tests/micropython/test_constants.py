#! /bin/false
#LVGL MicroPython testing definitions (error codes, etc.) common for Python3 and MicroPython scripts

#settings
EXIT_ON_ERROR  = 0
SHOW_SUCCESSES = 1


#status codes (script return values):

RESULT_OK = 0

ERROR_TEST_SYSTEM_FAULT    = 255
ERROR_NOT_ALL_TESTS_PASSED = 254
ERROR_TESTCASE_IS_EMPTY    = 253
ERROR_TESTCASE_NOT_GIVEN   = 252
ERROR_TESTCASE_NOT_FOUND   = 251
ERROR_TESTCASE_FAILED      = 250
ERROR_SUBTEST_FAILED       = 249

