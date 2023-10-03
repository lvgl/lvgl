#!/usr/bin/env python3
#LVGL MicroPython binding batch-test  (iterates through all files in the test-case directory)

import sys
import os
import glob

sys.dont_write_bytecode = True
import test_definitions as lv_test


MICROPYTHON_COMMAND = "../../../../../../lv_micropython/ports/unix/build-standard/micropython"
TESTCASE_RUNNER = "run_testcase.py"
TESTCASE_FOLDER = "test_cases"


TestCaseCount = 0
TestSuccessCount = 0

FileList = glob.glob( TESTCASE_FOLDER + '/*.py' )


for FilePath in sorted(FileList):
    TestCaseCount += 1
    print( "Running test-case: " + FilePath )
    lv_testcase_result = os.system( MICROPYTHON_COMMAND + " " + TESTCASE_RUNNER + " " + FilePath )
    if lv_testcase_result != lv_test.RESULT_OK: print("Problem occurred!")
    else: TestSuccessCount += 1

print( "\nSuccessful tests:",TestSuccessCount,"/",TestCaseCount,"\n" )

