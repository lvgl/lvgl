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
    def assert_base (clss, comparison_value, actual_value, operator_fn, operator_symbol, format, subtest_name=None):
        def show_subtest ():
            print( "Subtest", str(clss.subtest_count)+":", subtest_name  if subtest_name  else "", end=" ... " )
        clss.subtest_count += 1
        #TIMEOUT_PERIODS = int( lv_const.TRANSITION_WAIT_TIME / lv_const.WAIT_ROUTINE_PERIOD )
        #for i in range( TIMEOUT_PERIODS ) :
        if ( operator_fn(comparison_value, actual_value) ):
            clss.success_count += 1
            if lv_const.SHOW_SUCCESSES: show_subtest(); print( "OK", end=" " )  #break
        #gc.collect() #lv.timer_handler() #time.sleep_ms( lv_const.WAIT_ROUTINE_PERIOD )
        else:  #if i >= TIMEOUT_PERIODS-1:
            show_subtest(); print( "Failed,  not true:", end="" )
            if lv_const.EXIT_ON_ERROR: sys.exit( lv_const.ERROR_SUBTEST_FAILED )
        if format != "": print (" (", format%comparison_value, operator_symbol, format%actual_value, ")" )
        else: print()

    @staticmethod
    def assert_always (subtest_name=None):
        lv_test.assert_base (True, True, lambda a,b: a==b, "always", "", subtest_name)
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
        lv_test.assert_base (comparison_value, actual_value, lambda a,b: a>b, ">", "%d", subtest_name)
    @staticmethod
    def assert_greater (comparison_value, actual_value, subtest_name=None):
        lv_test.assert_base (comparison_value, actual_value, lambda a,b: a<b, "<", "%d", subtest_name)

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
    def handle_driver_exception (clss, e):
        lv_utils.event_loop.current_instance().deinit()
        clss.driver_exception = e

    @staticmethod
    def wait (ms):  #a polling with timeout built into assert functions might be better than using this
        for i in range( int (ms / lv_const.WAIT_ROUTINE_PERIOD) ):
            gc.collect()
            lv.timer_handler();
            time.sleep_ms( lv_const.WAIT_ROUTINE_PERIOD )
            gc.collect()


if len(sys.argv) < 2 :
    print("Test-case filename is needed as argument!")
    sys.exit(lv_const.ERROR_TESTCASE_NOT_GIVEN)


display_driver_utils.driver( exception_sink = lv_test.handle_driver_exception )


try:
    lv_test.FOLDER = ""
    if os.getcwd().rsplit('/')[-1] != lv_const.TEST_FOLDER:  #so both local and runner scripts work
        lv_test.FOLDER = lv_const.TEST_FOLDER + "/"
    exec ( open(sys.argv[1]).read() )
    lv_test.wait( lv_const.TESTCASE_HOLD_TIME )
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

