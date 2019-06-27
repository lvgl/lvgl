#!/usr/bin/env python3

'''
Generates a chechker file for lv_conf.h from lv_conf_templ.h define all the not defined values
'''


import re

fin = open("../lv_conf_template.h", "r")
fout = open("../src/lv_conf_checker.h", "w")

ignored = (
  "LV_OBJ_FREE_NUM_TYPE"
)

fout.write(
'''/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_checker.h
 * Make sure all the defines of lv_conf.h have a default value
**/

#ifndef LV_CONF_CHECKER_H
#define LV_CONF_CHECKER_H
'''
)

inlines = fin.read().splitlines()

started = 0

for i in inlines:
  if not started:
    if '#define LV_CONF_H' in i:
      started = 1
      continue
    else:
      continue

  if '/*--END OF LV_CONF_H--*/' in i: break

  r = re.search('^ *# *define *([^ ]+).*$', i)

  if r and r[1] in ignored: continue

  if r:
    new = re.sub('^ *# *define', '#define ', i)
    new = re.sub(' +', ' ', new)                 #Remove extra white spaces
    splitted = new.split(' ')
    fout.write('#ifndef ' + splitted[1] + '\n')
    fout.write(i + '\n')
    fout.write('#endif\n')
  elif re.search('^ *typedef .*;.*$', i):
    continue;   #igonre typedefs to avoide redeclaration
  else:
    fout.write(i + '\n')


fout.write(
'''
#endif  /*LV_CONF_CHECKER_H*/
''')

fin.close()
fout.close()
