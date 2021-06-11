#!/opt/bin/lv_micropython -i

import lvgl as lv
try:
    import sys
except ImportError:
    import usys as sys

# JS requires a special import
if sys.platform == 'javascript':
  import imp
  sys.path.append('https://raw.githubusercontent.com/lvgl/lv_binding_micropython/4c04dba836a5affcf86cef107b538e45278117ae/lib')

import display_driver
