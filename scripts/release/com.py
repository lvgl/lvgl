#!/usr/bin/env python3

import sys
import os.path
from os import path
import os, fnmatch
import re
import subprocess
import com

def cmd(c, ask_on_err = True):
  print("\n" + c)
  r = os.system(c)
  if r:
    print("### Error: " + str(r))
    if ask_on_err:
        input("Press Enter to continue execution...")

def define_set(fn, name, value):
    print("In " + fn + " set " + name + " to " + value)

    new_content = ""
    s = r'^ *# *define +' + str(name).rstrip() + ' +'

    f = open(fn, "r")
    for i in f.read().splitlines():
        r = re.search(s, i)
        if r:
            d = i.split("define")
            i = d[0] + "define " + name + " " + value
        new_content += i + '\n'

    f.close()

    f = open(fn, "w")
    f.write(new_content)
    f.close()

def ver_format(ver):
    s = "v" + str(ver[0]) + "."  + str(ver[1]) + "."  + str(ver[2])
    if(ver[3] != ""): s = s + "-" + ver[3]
    return s

def get_lvgl_version():
    print("Get lvgl's version ")

    ver = [0, 0, 0, ""]

    f = open("./lvgl.h", "r")

    lastNum = re.compile(r'(?:[^\d]*(\d+)[^\d]*)+')
    for i in f.read().splitlines():
        r = re.search(r'^#define LVGL_VERSION_MAJOR ', i)
        if r:
            m = lastNum.search(i)
            if m: ver[0] = m.group(1)

        r = re.search(r'^#define LVGL_VERSION_MINOR ', i)
        if r:
            m = lastNum.search(i)
            if m: ver[1] = m.group(1)

        r = re.search(r'^#define LVGL_VERSION_PATCH ', i)
        if r:
            m = lastNum.search(i)
            if m: ver[2] = m.group(1)

    f.close()
    
    print("Version found: " + ver_format(ver))

    return ver

def push(c):
    cmd("git push " + c)

def update_version(ver):
  ver_str = ver_format(ver)
  ver_num = ver[0] + "." + ver[1] + "." + ver[2]

  templ = fnmatch.filter(os.listdir('.'), '*_templ*.h')

  if len(templ) > 0 and templ[0]:
      print("Updating version in " + templ[0])
      cmd("sed -i -r 's/v[0-9]+\.[0-9]+\.[0-9]+.*/"+ "v" + ver_num + "/' " + templ[0])

  if os.path.exists("library.json"):
      print("Updating version in library.json")
      cmd("sed -i -r 's/[0-9]+\.[0-9]+\.[0-9]+/"+ ver_num +"/' library.json")

  if path.exists("library.properties"):
      print("Updating version in library.properties")
      cmd("sed -i -r 's/version=[0-9]+\.[0-9]+\.[0-9]+/"+ "version=" + ver_num + "/' library.properties")

  if path.exists("conf.py"):
      cmd("sed -i -r 's/v[0-9]+\.[0-9]+\.[0-9]+.*/" + ver_str + "/' conf.py")

  if path.exists("Kconfig"):
      cmd("sed -i -r 's/v[0-9]+\.[0-9]+\.[0-9]+.*/" + ver_str + "/' Kconfig")

  if path.exists("lvgl.h"):
      define_set("./lvgl.h", "LVGL_VERSION_MAJOR", str(ver[0]))
      define_set("./lvgl.h", "LVGL_VERSION_MINOR", str(ver[1]))
      define_set("./lvgl.h", "LVGL_VERSION_PATCH", str(ver[2]))
      define_set("./lvgl.h", "LVGL_VERSION_INFO", "\"" + ver[3] + "\"")
        
  cmd("git commit -am 'Update versions to " + ver_str + "'")
  
