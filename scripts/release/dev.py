#!/usr/bin/env python

import re
import os, fnmatch
import os.path
from os import path
from datetime import date
import com

def lvgl_update_ver(ver, br):
    os.chdir("./lvgl")

    com.cmd("git checkout " + br)
    com.update_version(ver)
    com.cmd("git commit -am 'Update " + br + " version'")
    com.push("origin " + br)
    com.cmd("git checkout master")

    os.chdir("../")

def docs_update_ver(ver, br):
    os.chdir("./docs")
    com.cmd("git checkout " + br + " --")
    com.update_version(ver)
    com.cmd("git commit -am 'Update " + br + " version'")
    com.push("origin " + br)
    com.cmd("git checkout master --")

    os.chdir("../")

def merge_dev_to_master():
    os.chdir("./lvgl")
    com.cmd("git checkout master")
    com.cmd("git merge dev")
    com.push("origin master")
    os.chdir("../")

    os.chdir("./docs")
    com.cmd("git checkout dev -- ")  #'dev' sometimes is not recognized so switch to it first
    com.cmd("git checkout latest --")
    com.cmd("git merge dev -X theirs --no-edit;")
    com.push("origin latest")
    os.chdir("../")

def make(mode):
    if(mode == "bugfix"):
        ver = com.get_lvgl_version("master")
        ver[3] = "dev"
        ver[2] = str(int(ver[2]) + 1)
        lvgl_update_ver(ver, "master")
        docs_update_ver(ver, "latest")
    else:
        ver = com.get_lvgl_version("dev")
        ver[3] = "dev"
        merge_dev_to_master()
        if(mode == "major"):
            ver[0] = str(int(ver[0]) + 1)
            ver[1] = '0'
            ver[2] = '0'
        if(mode == "minor"):
            ver[1] = str(int(ver[1]) + 1)
            ver[2] = '0'

        lvgl_update_ver(ver, "dev")
        docs_update_ver(ver, "dev")

if __name__ == '__main__':
     if(len(sys.argv) != 2):
        print("Missing argument. Usage ./dev.py major | minor | bugfix")
        print("E.g. ./dev.py minor")
     else:
         make(sys.argv[1])
