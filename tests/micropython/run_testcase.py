#! ../../../../../../lv_micropython/ports/unix/build-standard/micropython
#LVGL MicroPython single test-case runner script (test-case script is given as parameter)

import gc
import os
import usys as sys
import time

import lvgl as lv
import lv_utils
import display_driver
import display_driver_utils

import test_constants as lv_const


class lv_test:
    subtest_count = 0
    success_count = 0
    driver_exception = None
    LV_TEST_FOLDER = ""  #modified as needed, so both local and runner scripts work

    @classmethod
    def assert_base (cls, expected_value, actual_value, operator_fn, operator_symbol, format, subtest_name=None):
        def show_subtest ():
            print( "Subtest", str(cls.subtest_count)+":", subtest_name  if subtest_name  else "", end=" ... " )
        cls.subtest_count += 1
        if ( operator_fn(expected_value, actual_value) ):
            cls.success_count += 1
            if lv_const.SHOW_SUCCESSES: show_subtest(); print( "OK", end=" " )
        else:
            show_subtest(); print( "Failed,  not true:", end="" )
            if lv_const.EXIT_ON_ERROR: sys.exit( lv_const.ERROR_SUBTEST_FAILED )
        print (" (", format%expected_value, operator_symbol, format%actual_value, ")" )

    @staticmethod
    def assert_true (actual_value, subtest_name=None):
        lv_test.assert_base (True, actual_value, lambda a,b: a==b, "is", "%r", subtest_name)
    @staticmethod
    def assert_false (actual_value, subtest_name=None):
        lv_test.assert_base (False, actual_value, lambda a,b: a==b, "is", "%r", subtest_name)
    @staticmethod
    def assert_equal (expected_value, actual_value, subtest_name=None):
        lv_test.assert_base (expected_value, actual_value, lambda a,b: a==b, "=", "%d", subtest_name)
    @staticmethod
    def assert_differ (comparison_value, actual_value, subtest_name=None):
        lv_test.assert_base (comparison_value, actual_value, lambda a,b: a!=b, "!=", "%d", subtest_name)
    @staticmethod
    def assert_less (comparison_value, actual_value, subtest_name=None):
        lv_test.assert_base (comparison_value, actual_value, lambda a,b: a<b, "<", "%d", subtest_name)
    @staticmethod
    def assert_greater (comparison_value, actual_value, subtest_name=None):
        lv_test.assert_base (comparison_value, actual_value, lambda a,b: a>b, ">", "%d", subtest_name)

    def color_to_hex (color):
        return (color.red << 16) | (color.green << 8) | color.blue
    @staticmethod
    def assert_colormatch (expected_value, actual_value, subtest_name=None):
        lv_test.assert_base ( lv_test.color_to_hex( expected_value ), lv_test.color_to_hex( actual_value )
                              , lambda a,b: a==b, "=", "$%06X", subtest_name )
    @staticmethod
    def assert_colordiff (comparison_value, actual_value, subtest_name=None):
        lv_test.assert_base ( lv_test.color_to_hex( comparison_value ), lv_test.color_to_hex( actual_value )
                              , lambda a,b: a!=b, "!=", "$%06X", subtest_name )

    @classmethod
    def handle_driver_exception (cls, e):
        lv_utils.event_loop.current_instance().deinit()
        cls.driver_exception = e

    @staticmethod
    def wait (ms): time.sleep_ms( ms )


if len(sys.argv) < 2 :
    print("Test-case filename is needed as argument!")
    sys.exit(lv_const.ERROR_TESTCASE_NOT_GIVEN)


display_driver_utils.driver( exception_sink = lv_test.handle_driver_exception )


try:
    lv_test.FOLDER = ""
    if os.getcwd().rsplit('/')[-1] != lv_const.TEST_FOLDER:  #so both local and runner scripts work
        lv_test.FOLDER = lv_const.TEST_FOLDER + "/"
    exec ( open(sys.argv[1]).read() )
    time.sleep_ms(lv_const.TESTCASE_HOLD_TIME)
    if lv_test.driver_exception: raise lv_test.driver_exception
    if lv_utils.event_loop.is_running():
        lv_utils.event_loop.current_instance().deinit()
    gc.collect()

    if lv_test.subtest_count == 0 :
        print("***ERROR*** Tests are not defined!")
        sys.exit(lv_const.ERROR_TESTCASE_IS_EMPTY)

    if lv_test.success_count == lv_test.subtest_count:
        sys.exit(lv_const.RESULT_OK)
    else:
        print("***ERROR*** Only",lv_test.success_count,'of the',lv_test.subtest_count,"subtests succeeded!")
        sys.exit( lv_const.ERROR_TESTCASE_FAILED )


except Exception as e:
    print( "Test System issue!" );
    sys.print_exception(e)
    sys.exit( lv_const.ERROR_TEST_SYSTEM_FAULT )

