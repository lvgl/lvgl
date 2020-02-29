#!/usr/bin/env python3.6

'''
Generates a checker file for lv_conf.h from lv_conf_templ.h define all the not defined values
'''


import re

fin = open("../lv_conf_template.h", "r")
fout = open("../src/lv_conf_internal.h", "w")


fout.write(
'''/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_internal.h
 * Make sure all the defines of lv_conf.h have a default value
**/

#ifndef LV_CONF_INTERNAL_H
#define LV_CONF_INTERNAL_H
/* clang-format off */

#include <stdint.h>

#if defined(LV_CONF_PATH)
#define __LV_TO_STR_AUX(x) #x
#define __LV_TO_STR(x) __LV_TO_STR_AUX(x)
#include __LV_TO_STR(LV_CONF_PATH)
#undef __LV_TO_STR_AUX
#undef __LV_TO_STR
#elif defined(LV_CONF_INCLUDE_SIMPLE)
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

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
    line = re.sub('\(.*?\)', '', r[1], 1)    #remove parentheses from macros
    fout.write(
      f'#ifndef {line}\n'
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
