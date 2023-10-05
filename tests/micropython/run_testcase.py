#! /usr/bin/env micropython
#LVGL MicroPython single test-case runner script (test-case script is given as parameter)

import gc
import usys
import time

import lvgl as lv
import lv_utils
import display_driver
import display_driver_utils

import test_constants as lv_cons


class lv_test:
    DO = 1  #set to 0 to test the framework itself (in testcases this variable can be a test-condition)
    subtest_count = 0
    success_count = 0
    driver_exception = None
    TESTCASE_HOLD_TIME = 100 #ms

    @classmethod
    def check (cls, tested_expression, expected_value, subtest_name=None):
        cls.subtest_count += 1
        print( "Subtest", str(cls.subtest_count)+":", subtest_name  if subtest_name  else "", end=" ... " )
        if (tested_expression == expected_value):
            cls.success_count += 1
            print( "OK" )
        else: print( "Failed (",tested_expression,"vs expected",expected_value,")" )

    @classmethod
    def handle_driver_exception (cls, e):
        lv_utils.event_loop.current_instance().deinit()
        cls.driver_exception = e

    @classmethod
    def wait (cls, ms): time.sleep_ms( ms )


if len(usys.argv) < 2 :
    print("Test-case filename is needed as argument!")
    usys.exit(lv_cons.ERROR_TESTCASE_NOT_GIVEN)


display_driver_utils.driver( exception_sink = lv_test.handle_driver_exception )


try:
    exec ( open(usys.argv[1]).read() )
    time.sleep_ms(lv_test.TESTCASE_HOLD_TIME)
    if lv_test.driver_exception: raise lv_test.driver_exception
    if lv_utils.event_loop.is_running():
        lv_utils.event_loop.current_instance().deinit()
    gc.collect()

    if lv_test.subtest_count == 0 :
        print("***ERROR*** Tests are not defined!")
        usys.exit(lv_cons.ERROR_TESTCASE_IS_EMPTY)

    if lv_test.success_count == lv_test.subtest_count:
        usys.exit(lv_cons.RESULT_OK)
    else:
        print("***ERROR*** Only",lv_test.success_count,'of the',lv_test.subtest_count,"subtests succeeded!")
        usys.exit( lv_cons.ERROR_TESTCASE_FAILED )


except Exception as e:
    print( "Test System issue!" );
    usys.print_exception(e)
    usys.exit( lv_cons.ERROR_TEST_SYSTEM_FAULT )

