#!/usr/local/bin/micropython
#LVGL MicroPython single test-case runner script (test-case script is given as parameter)

import gc
import usys
import time
import lvgl as lv
#import lv_utils
import display_driver
import display_driver_utils

import test_definitions as lv_test


LV_TEST_TIMEOUT = 100 #ms

lv_testcase_result = 0
recorded_exception = None


def handle_exceptions(Null, e):
    lv_utils.event_loop.current_instance().deinit()
    if not recorded_exception:
        recorded_exception = e


if len(usys.argv) < 2 :
    print("Test-case filename is needed as argument!")
    usys.exit(lv_test.ERROR_TESTCASE_NOT_GIVEN)


display_driver_utils.driver(exception_sink = handle_exceptions)

try:
    exec ( open(usys.argv[1]).read() )
    #print( lv_testcase_result )
    time.sleep_ms(LV_TEST_TIMEOUT)
    if recorded_exception: raise recorded_exception
    gc.collect()
    if lv_testcase_result >= LV_TESTCASE_SUBTESTS:
        usys.exit(lv_test.RESULT_OK)
    else:
        print(" Only",lv_testcase_result,'of the',LV_TESTCASE_SUBTESTS,"subtests succeeded!")
        usys.exit( lv_test.ERROR_TESTCASE_FAILED )

except Exception as e:
    usys.print_exception(e)
    usys.exit( ERROR_BINDING_SYSTEM_FAULT )
