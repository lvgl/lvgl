#!/usr/bin/env python3

import defines
import build
import shutil
import test
import sys
import os


def build_conf(title, defs):
    print("")
    print("")
    print("============================================")
    print(title)
    print("============================================")
    print("", flush=True)

    build.clean()
    build.build(defs)


test_only = "test" in sys.argv
test_report = "report" in sys.argv
test_noclean = "noclean" in sys.argv

if not test_only:
    build_conf("Minimal config monochrome", defines.minimal_monochrome)
    build_conf("Normal config, 8 bit color depth", defines.normal_8bit)
    build_conf("Minimal config, 16 bit color depth", defines.minimal_16bit)
    build_conf("Normal config, 16 bit color depth swapped",
               defines.normal_16bit_swap)
    build_conf("Full config, 32 bit color depth", defines.full_32bit)


files = test.prepare()
if test_noclean == False:
    build.clean()

for f in files:
    name = f[:-2]  # test_foo.c -> test_foo
    build.build_test(defines.test, name)

if test_report:
    print("")
    print("Generating report")
    print("-----------------------", flush=True)
    try:
        shutil.rmtree('report')
    except FileNotFoundError:
        pass
    os.mkdir('report')
    os.system("gcovr -r ../ --html-details -o report/index.html  --exclude-directories '\.\./examples' --exclude-directories 'src/.*' --exclude-directories 'unity' --exclude 'lv_test_.*\.c'")
    os.system("gcovr -r ../ -x report/coverage.xml  --exclude-directories '\.\./examples' --exclude-directories 'src/.*' --exclude-directories 'unity' --exclude 'lv_test_.*\.c'")
    print("Done: See report/index.html", flush=True)
