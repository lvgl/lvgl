#!/usr/bin/env python3

'''
Generates lv_conf_internal.h from lv_conf_template.h to provide default values
'''

import sys
import re

if sys.version_info < (3,6,0):
  print("Python >=3.6 is required", file=sys.stderr)
  exit(1)

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

/* Handle special Kconfig options */
#ifndef LV_KCONFIG_IGNORE
#  include "lv_conf_kconfig.h"
#  ifdef CONFIG_LV_CONF_SKIP
#    define LV_CONF_SKIP
#  endif
#endif

/*If "lv_conf.h" is available from here try to use it later.*/
#ifdef __has_include
#  if __has_include("lv_conf.h")
#    ifndef LV_CONF_INCLUDE_SIMPLE
#      define LV_CONF_INCLUDE_SIMPLE
#    endif
#  endif
#endif

/*If lv_conf.h is not skipped include it*/
#ifndef LV_CONF_SKIP
#  ifdef LV_CONF_PATH                           /*If there is a path defined for lv_conf.h use it*/
#    define __LV_TO_STR_AUX(x) #x
#    define __LV_TO_STR(x) __LV_TO_STR_AUX(x)
#    include __LV_TO_STR(LV_CONF_PATH)
#    undef __LV_TO_STR_AUX
#    undef __LV_TO_STR
#  elif defined(LV_CONF_INCLUDE_SIMPLE)        /*Or simply include lv_conf.h is enabled*/
#    include "lv_conf.h"
#  else
#    include "../../lv_conf.h"                 /*Else assume lv_conf.h is next to the lvgl folder*/
#  endif
#endif

#ifdef CONFIG_LV_COLOR_DEPTH
#  define _LV_KCONFIG_PRESENT
#endif

/*----------------------------------
 * Start parsing lv_conf_template.h
 -----------------------------------*/
'''
)

started = 0

for line in fin.read().splitlines():
  if not started:
    if '#define LV_CONF_H' in line:
      started = 1
      continue
    else:
      continue

  if '/*--END OF LV_CONF_H--*/' in line: break

  #Is there a #define in this line?
  r = re.search(r'^[\s]*#[\s]*define[\s]+([^\s]+).*$', line)   # \s means any white space character

  if r:
    name = r[1]
    name = re.sub('\(.*?\)', '', name, 1)    #remove parentheses from macros. E.g. MY_FUNC(5) -> MY_FUNC

    name_and_value = re.sub('[\s]*#[\s]*define', '', line, 1)

    #If the value should be 1 (enabled) by default use a more complex structure for Kconfig checks because
    #if a not defined CONFIG_... value should be interpreted as 0 and not the LVGL default
    is_one = re.search(r'[\s]*#[\s]*define[\s]*[A-Z0-9_]+[\s]+1[\s]*$', line)
    if(is_one):
      #1. Use the value if already set from lv_conf.h or anything else (i.e. do nothing)
      #2. In Kconfig environment use the CONFIG_... value if set, else use 0
      #3. In not Kconfig environment use the LVGL's default value

      fout.write(
        f'#ifndef {name}\n'
        f'#  ifdef _LV_KCONFIG_PRESENT\n'
        f'#    ifdef CONFIG_{name.upper()}\n'
        f'#      define {name} CONFIG_{name.upper()}\n'
        f'#    else\n'
        f'#      define {name} 0\n'
        f'#    endif\n'
        f'#  else\n'
        f'#    define{name_and_value}\n'
        f'#  endif\n'
        f'#endif\n'
      )
    else:
      #1. Use the value if already set from lv_conf.h or anything else  (i.e. do nothing)
      #2. Use the Kconfig value if set
      #3. Use the LVGL's default value

      fout.write(
        f'#ifndef {name}\n'
        f'#  ifdef CONFIG_{name.upper()}\n'
        f'#    define {name} CONFIG_{name.upper()}\n'
        f'#  else\n'
        f'#    define{name_and_value}\n'
        f'#  endif\n'
        f'#endif\n'
      )

  elif re.search('^ *typedef .*;.*$', line):
    continue   #ignore typedefs to avoide redeclaration
  else:
    fout.write(f'{line}\n')

fout.write(
'''

/*----------------------------------
 * End of parsing lv_conf_template.h
 -----------------------------------*/

LV_EXPORT_CONST_INT(LV_DPI_DEF);

#undef _LV_KCONFIG_PRESENT

/*If running without lv_conf.h add typdesf with default value*/
#ifdef LV_CONF_SKIP
#  if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /*Disable warnings for Visual Studio*/
#    define _CRT_SECURE_NO_WARNINGS
#  endif
#endif  /*defined(LV_CONF_SKIP)*/

#endif  /*LV_CONF_INTERNAL_H*/
'''
)

fin.close()
fout.close()
