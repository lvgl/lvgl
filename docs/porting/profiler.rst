.. _profiler:

========
Profiler
========

As the complexity of the application increases, performance issues 
such as low FPS and frequent cache misses causing lag may arise. 
LVGL has internally set up some hooks for performance measurement 
to help developers analyze and locate performance issues.

Porting
*******

To enable profiler, set :c:macro:`LV_USE_PROFILER` in ``lv_conf.h`` and configure the following options:

- :c:macro:`LV_PROFILER_INCLUDE`: Provides a header file for the performance measurement function.
- :c:macro:`LV_PROFILER_BEGIN`: Performance measurement start point function.
- :c:macro:`LV_PROFILER_END`: Performance measurement end point function.

Example
*******

The following is an example of output performance measurements using LVGL logging systems:

Configure ``lv_conf.h``:

.. code:: c

   #define LV_USE_PROFILER 1
   #define LV_PROFILER_INCLUDE "lvgl/src/hal/lv_hal_tick.h"
   #define LV_PROFILER_BEGIN   uint32_t profiler_start = lv_tick_get()
   #define LV_PROFILER_END     LV_LOG_USER("cost %" LV_PRIu32 "ms", lv_tick_elaps(profiler_start))


Users can add the measured functions themselves:

.. code:: c

   LV_PROFILER_BEGIN;
   my_func();
   LV_PROFILER_END;
