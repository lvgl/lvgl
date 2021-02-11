#!/usr/bin/env python3

'''
Generates a checker file for lv_conf.h from lv_conf_template.h define all the not defined values
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
#include "lv_conf_kconfig.h"

#ifdef CONFIG_LV_CONF_SKIP
#define LV_CONF_SKIP
#endif

/* If "lv_conf.h" is available from here try to use it later.*/
#if defined __has_include
#  if __has_include("lv_conf.h")
#   ifndef LV_CONF_INCLUDE_SIMPLE
#    define LV_CONF_INCLUDE_SIMPLE
#   endif
#  endif
#endif

/*If lv_conf.h is not skipped include it*/
#if !defined(LV_CONF_SKIP)
#  if defined(LV_CONF_PATH)											/*If there is a path defined for lv_conf.h use it*/
#    define __LV_TO_STR_AUX(x) #x
#    define __LV_TO_STR(x) __LV_TO_STR_AUX(x)
#    include __LV_TO_STR(LV_CONF_PATH)
#    undef __LV_TO_STR_AUX
#    undef __LV_TO_STR
#  elif defined(LV_CONF_INCLUDE_SIMPLE)        /*Or simply include lv_conf.h is enabled*/
#    include "lv_conf.h"
#  else
#    include "../../lv_conf.h"                 /*Else assume lv_conf.h is next to the lvgl folder */
#  endif
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

#ifndef LV_USE_BTN               /*Only if not defined in lv_conf.h*/
#  ifdef CONFIG_LV_USE_BTN    /*Use KConfig value if set*/
#    define LV_USE_BTN  CONFIG_LV_USE_BTN
#  else
#    define LV_USE_BTN      1      /*Use default value*/
#  endif
#endif

  if r:
    line = re.sub('\(.*?\)', '', r[1], 1)    #remove parentheses from macros
    dr = re.sub('.*# *define', '', i, 1)
    d = "#    define " + dr
		
    fout.write(
      f'#ifndef {line}\n'
      f'#  ifdef CONFIG_{line.upper()}\n'
      f'#    define {line} CONFIG_{line.upper()}\n'
      f'#  else\n'
      f'{d}\n'
      f'#  endif\n'
      f'#endif\n'
    )
  elif re.search('^ *typedef .*;.*$', i):
    continue   #ignore typedefs to avoide redeclaration
  else:
    fout.write(f'{i}\n')

fout.write(
'''

/*If running without lv_conf.h add typdesf with default value*/
#if defined(LV_CONF_SKIP)

  /* Type of coordinates. Should be `int16_t` (or `int32_t` for extreme cases) */
  typedef int16_t lv_coord_t;

#  if LV_USE_ANIMATION
  /*Declare the type of the user data of animations (can be e.g. `void *`, `int`, `struct`)*/
  typedef void * lv_anim_user_data_t;
#  endif

#  if LV_USE_GROUP
  typedef void * lv_group_user_data_t;
#  endif

#  if LV_USE_FILESYSTEM
  typedef void * lv_fs_drv_user_data_t;
#  endif

  typedef void * lv_img_decoder_user_data_t;

  typedef void * lv_disp_drv_user_data_t;             /*Type of user data in the display driver*/
  typedef void * lv_indev_drv_user_data_t;            /*Type of user data in the input device driver*/

  typedef void * lv_font_user_data_t;

#  if LV_USE_USER_DATA
  typedef void * lv_obj_user_data_t;
#  endif

#endif

#endif  /*LV_CONF_INTERNAL_H*/
'''
)

fin.close()
fout.close()
