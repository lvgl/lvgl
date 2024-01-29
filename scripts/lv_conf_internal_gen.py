#!/usr/bin/env python3

'''
Generates lv_conf_internal.h from lv_conf_template.h to provide default values
'''

import os
import sys
import re

SCRIPT_DIR = os.path.dirname(__file__)
LV_CONF_TEMPLATE = os.path.join(SCRIPT_DIR, "..", "lv_conf_template.h")
LV_CONF_INTERNAL = os.path.join(SCRIPT_DIR, "..", "src", "lv_conf_internal.h")

if sys.version_info < (3,6,0):
  print("Python >=3.6 is required", file=sys.stderr)
  exit(1)

fin = open(LV_CONF_TEMPLATE)
fout = open(LV_CONF_INTERNAL, "w", newline='')

fout.write(
'''/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_internal.h
 * Make sure all the defines of lv_conf.h have a default value
**/

#ifndef LV_CONF_INTERNAL_H
#define LV_CONF_INTERNAL_H
/* clang-format off */

/*Config options*/
#define LV_OS_NONE          0
#define LV_OS_PTHREAD       1
#define LV_OS_FREERTOS      2
#define LV_OS_CMSIS_RTOS2   3
#define LV_OS_RTTHREAD      4
#define LV_OS_WINDOWS       5
#define LV_OS_CUSTOM        255

#define LV_STDLIB_BUILTIN           0
#define LV_STDLIB_CLIB              1
#define LV_STDLIB_MICROPYTHON       2
#define LV_STDLIB_RTTHREAD          3
#define LV_STDLIB_CUSTOM            255

#define LV_DRAW_SW_ASM_NONE         0
#define LV_DRAW_SW_ASM_NEON         1
#define LV_DRAW_SW_ASM_HELIUM       2
#define LV_DRAW_SW_ASM_CUSTOM       255

/* Handle special Kconfig options */
#ifndef LV_KCONFIG_IGNORE
    #include "lv_conf_kconfig.h"
    #ifdef CONFIG_LV_CONF_SKIP
        #define LV_CONF_SKIP
    #endif
#endif

/*If "lv_conf.h" is available from here try to use it later.*/
#ifdef __has_include
    #if __has_include("lv_conf.h")
        #ifndef LV_CONF_INCLUDE_SIMPLE
            #define LV_CONF_INCLUDE_SIMPLE
        #endif
    #endif
#endif

/*If lv_conf.h is not skipped include it*/
#ifndef LV_CONF_SKIP
    #ifdef LV_CONF_PATH                           /*If there is a path defined for lv_conf.h use it*/
        #define __LV_TO_STR_AUX(x) #x
        #define __LV_TO_STR(x) __LV_TO_STR_AUX(x)
        #include __LV_TO_STR(LV_CONF_PATH)
        #undef __LV_TO_STR_AUX
        #undef __LV_TO_STR
    #elif defined(LV_CONF_INCLUDE_SIMPLE)         /*Or simply include lv_conf.h is enabled*/
        #include "lv_conf.h"
    #else
        #include "../../lv_conf.h"                /*Else assume lv_conf.h is next to the lvgl folder*/
    #endif
    #if !defined(LV_CONF_H) && !defined(LV_CONF_SUPPRESS_DEFINE_CHECK)
        /* #include will sometimes silently fail when __has_include is used */
        /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80753 */
        #pragma message("Possible failure to include lv_conf.h, please read the comment in this file if you get errors")
    #endif
#endif

#ifdef CONFIG_LV_COLOR_DEPTH
    #define _LV_KCONFIG_PRESENT
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
  r = re.search(r'^([\s]*)#[\s]*(undef|define)[\s]+([^\s]+).*$', line)   # \s means any white space character

  if r:
    indent = r[1]

    name = r[3]
    name = re.sub('\(.*?\)', '', name, 1)    #remove parentheses from macros. E.g. MY_FUNC(5) -> MY_FUNC

    line = re.sub('[\s]*', '', line, 1)

    #If the value should be 1 (enabled) by default use a more complex structure for Kconfig checks because
    #if a not defined CONFIG_... value should be interpreted as 0 and not the LVGL default
    is_one = re.search(r'#[\s]*define[\s]*[A-Z0-9_]+[\s]+1([\s]*$|[\s]+)', line)
    if is_one:
      #1. Use the value if already set from lv_conf.h or anything else (i.e. do nothing)
      #2. In Kconfig environment use the CONFIG_... value if set, else use 0
      #3. In not Kconfig environment use the LVGL's default value

      fout.write(
        f'{indent}#ifndef {name}\n'
        f'{indent}    #ifdef _LV_KCONFIG_PRESENT\n'
        f'{indent}        #ifdef CONFIG_{name.upper()}\n'
        f'{indent}            #define {name} CONFIG_{name.upper()}\n'
        f'{indent}        #else\n'
        f'{indent}            #define {name} 0\n'
        f'{indent}        #endif\n'
        f'{indent}    #else\n'
        f'{indent}        {line}\n'
        f'{indent}    #endif\n'
        f'{indent}#endif\n'
      )
    else:
      #1. Use the value if already set from lv_conf.h or anything else  (i.e. do nothing)
      #2. Use the Kconfig value if set
      #3. Use the LVGL's default value

      fout.write(
        f'{indent}#ifndef {name}\n'
        f'{indent}    #ifdef CONFIG_{name.upper()}\n'
        f'{indent}        #define {name} CONFIG_{name.upper()}\n'
        f'{indent}    #else\n'
        f'{indent}        {line}\n'
        f'{indent}    #endif\n'
        f'{indent}#endif\n'
      )

  elif re.search('^ *typedef .*;.*$', line):
    continue   #ignore typedefs to avoid redeclaration
  else:
    fout.write(f'{line}\n')

fout.write(
'''

/*----------------------------------
 * End of parsing lv_conf_template.h
 -----------------------------------*/

#ifndef __ASSEMBLY__
LV_EXPORT_CONST_INT(LV_DPI_DEF);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_STRIDE_ALIGN);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_ALIGN);
#endif

#undef _LV_KCONFIG_PRESENT

/*Set some defines if a dependency is disabled*/
#if LV_USE_LOG == 0
    #define LV_LOG_LEVEL            LV_LOG_LEVEL_NONE
    #define LV_LOG_TRACE_MEM        0
    #define LV_LOG_TRACE_TIMER      0
    #define LV_LOG_TRACE_INDEV      0
    #define LV_LOG_TRACE_DISP_REFR  0
    #define LV_LOG_TRACE_EVENT      0
    #define LV_LOG_TRACE_OBJ_CREATE 0
    #define LV_LOG_TRACE_LAYOUT     0
    #define LV_LOG_TRACE_ANIM       0
#endif  /*LV_USE_LOG*/


/*If running without lv_conf.h add typedefs with default value*/
#ifdef LV_CONF_SKIP
    #if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /*Disable warnings for Visual Studio*/
        #define _CRT_SECURE_NO_WARNINGS
    #endif
#endif  /*defined(LV_CONF_SKIP)*/

#endif  /*LV_CONF_INTERNAL_H*/
'''
)

fin.close()
fout.close()
