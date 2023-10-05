#! /usr/bin/env python3
#LVGL MicroPython binding batch-test  (iterates through all files in the test-case directory)

import sys
import os
import glob

sys.dont_write_bytecode = True
import test_constants as lv_cons


class Var:
    MICROPYTHON_COMMAND = "../../../../../../lv_micropython/ports/unix/build-standard/micropython"
    TESTCASE_RUNNER = "run_testcase.py"
    TESTCASE_FOLDER = "test_cases"

    TestCaseCount = 0
    TestResult = 0
    TestSuccessCount = 0
    FilePath = None
    FileList = glob.glob( TESTCASE_FOLDER + '/*.py' )


for Var.FilePath in sorted(Var.FileList):
    Var.TestCaseCount += 1
    print( "\nRunning test-case" , Var.TestCaseCount , ": ---------- " + Var.FilePath + " ----------" )
    Var.TestResult = os.system( Var.MICROPYTHON_COMMAND + " " + Var.TESTCASE_RUNNER + " " + Var.FilePath )
    if Var.TestResult == lv_cons.RESULT_OK: Var.TestSuccessCount += 1

print( "\nSuccessful tests:",Var.TestSuccessCount,"/",Var.TestCaseCount,"\n" )

exit( lv_cons.RESULT_OK  if Var.TestSuccessCount==Var.TestCaseCount  else lv_cons.ERROR_NOT_ALL_TESTS_PASSED )

