#!/usr/bin/env python

import sys
import os.path
from os import path
import re
import subprocess
import com

def make(repo_path, auto_push = False):
        os.chdir("./" + repo_path)
        com.cmd('git checkout master')
        print("Update lvgl");
        os.chdir("./lvgl")
        com.cmd("git checkout master")
        com.cmd("git pull origin --tags")

        out = subprocess.Popen(['git', 'tag', '--sort=-creatordate'],
           stdout=subprocess.PIPE,
           stderr=subprocess.STDOUT)
        stdout,stderr = out.communicate()

        ver_str = stdout.decode("utf-8").split('\n')[0]
        release_br = ver_str[1:] #trim "v"
        release_br = release_br.split('.')[0] # get the first number
        release_br = "release/v" + release_br

        com.cmd("git checkout " + release_br + "; git pull origin " + release_br)

        os.chdir("..")

        com.cmd("cp -f lvgl/lv_conf_template.h lv_conf.h")
        com.cmd("sed -i -r 's/#if 0/#if 1/' lv_conf.h")  # Enable lv_conf.h
        with open("confdef.txt") as f:
            for line in f:
                (key, val) = line.rstrip().split('\t')
                com.define_set("lv_conf.h", str(key), str(val))

        if os.path.exists("lv_examples"):
            print("Update lv_examples");
            com.cmd("cd lv_examples; git co " + release_br + "; git pull origin " + release_br)

        if os.path.exists("lv_drivers"):
            print("update lv_drivers");
            com.cmd("cd lv_drivers " + release_br + "; git pull origin " + release_br)

        msg = 'Update to ' + ver_str
        com.cmd("git add .")
        com.cmd('git commit -am "' + msg +  '"')
        com.cmd("git tag -a " + ver_str + " -m '" + msg + "' " )
        if auto_push:
            com.push("origin --tags")
            com.push("origin master")

        os.chdir("../")

if __name__ == '__main__':
     if(len(sys.argv) != 2):
        print("Missing argument. Usage ./projs_update.py repo_path")
        print("E.g. ./projs_update.py ./lv_sim_eclipse_sdl")
     else:
         make(sys.argv[1], sys.argv[2])
