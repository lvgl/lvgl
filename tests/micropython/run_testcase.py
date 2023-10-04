#!/usr/local/bin/micropython
#LVGL MicroPython single test-case runner script (test-case script is given as parameter)

import gc
import usys
import time

import lvgl as lv
import lv_utils
import display_driver
import display_driver_utils

import test_definitions as lv_test


LV_TEST_TIMEOUT = 100 #ms

lv_subtest_running = 0
lv_subtest_count = 0
lv_success_count = 0
lv_driver_exception = None


def lv_handle_driver_exceptions (Null, e):
    lv_utils.event_loop.current_instance().deinit()
    if not lv_driver_exception: lv_driver_exception = e

def lv_subtest_start (subtest_name, subtest_id=None): #for more verbose output
    global lv_subtest_count, lv_subtest_running
    lv_subtest_running = 1
    lv_subtest_count += 1
    print( "Subtest", subtest_id  if subtest_id  else "("+str(lv_subtest_count)+")"
           , "started:", subtest_name  if subtest_name  else "", end=" ... " )

def lv_subtest_success (subtest_name=None, success_id=None):
    global lv_success_count, lv_subtest_running
    lv_subest_running = 0
    lv_success_count += 1
    if ( type(subtest_name) == int ):
        success_id = subtest_name
        subtest_name = None
    print( "Subtest", success_id  if success_id  else "("+str(lv_success_count)+")"
           , "succeeded", ": "+str(subtest_name)  if subtest_name  else "" )


if len(usys.argv) < 2 :
    print("Test-case filename is needed as argument!")
    usys.exit(lv_test.ERROR_TESTCASE_NOT_GIVEN)


display_driver_utils.driver(exception_sink = lv_handle_driver_exceptions)

try:
    exec ( open(usys.argv[1]).read() )
    #print( lv_success_count )
    time.sleep_ms(LV_TEST_TIMEOUT)
    if lv_driver_exception: raise lv_driver_exception
    gc.collect()
    if lv_utils.event_loop.is_running():
        lv_utils.event_loop.current_instance().deinit()
    if lv_success_count >= LV_TESTCASE_SUBTESTS:
        usys.exit(lv_test.RESULT_OK)
    else:
        if lv_subtest_running: print()
        print("***ERROR*** Only",lv_success_count,'of the',LV_TESTCASE_SUBTESTS,"subtests succeeded!")
        usys.exit( lv_test.ERROR_TESTCASE_FAILED )

except Exception as e:
    print( "Driver issue happened!" );
    usys.print_exception(e)
    usys.exit( lv_test.ERROR_BINDING_SYSTEM_FAULT )

