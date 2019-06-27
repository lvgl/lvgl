'''
Generates a checker file for lv_conf.h from lv_conf_templ.h define all the not defined values
'''


import re

fin = open("../lv_conf_template.h", "r")
fout = open("../src/lv_conf_checker.h", "w")


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

started = 0

for i in fin.read().splitlines():
  if not started:
    if '#define LV_CONF_H' in i:
      started = 1
      continue
    else:
      continue

  if '/*--END OF LV_CONF_H--*/' in i: break

  r = re.search(r'^ *# *define ([^\s]+).*$', i)
  if r:
    fout.write(
      f'#ifndef {r[1]}\n'
      f'{i}\n'
      '#endif\n'
    )
  elif re.search('^ *typedef .*;.*$', i):
    continue   #ignore typedefs to avoide redeclaration
  else:
    fout.write(f'{i}\n')


fout.write(
'''
#endif  /*LV_CONF_CHECKER_H*/
'''
)

fin.close()
fout.close()
