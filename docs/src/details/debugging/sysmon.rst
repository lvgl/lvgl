.. _sysmon:

=======================
System Monitor (sysmon)
=======================

The System Monitor module provides real-time monitoring of system performance
metrics directly on your display. It supports both performance monitoring
(CPU usage and FPS) and memory monitoring (used memory and fragmentation).



Dependencies
************

- Requires ``LV_USE_LABEL = 1`` in lv_conf.h
- Requires ``LV_USE_OBSERVER = 1`` in lv_conf.h
- Requires ``LV_USE_SYSMON = 1`` in lv_conf.h



.. _sysmon_usage:

Usage
*****


Configuration
--------------

Enable in ``lv_conf.h``:

.. code-block:: c

    /* Main sysmon enable */
    #define LV_USE_SYSMON 1

    /* Performance monitor (CPU% and FPS) */
    #define LV_USE_PERF_MONITOR 1

    /* Memory monitor (used + fragmentation) */
    #define LV_USE_MEM_MONITOR 1

    /* Optional: refresh period in ms */
    #define LV_SYSMON_REFR_PERIOD_DEF 300

    /* Optional: log to console instead of screen */
    #define LV_USE_PERF_MONITOR_LOG_MODE 0


Creating Monitors
-----------------

.. code-block:: c

    /* Create generic monitor */
    lv_obj_t * sysmon = lv_sysmon_create(lv_display_get_default());

    /* Create performance monitor */
    lv_sysmon_show_performance(NULL);  /* NULL = default display */

    /* Create memory monitor */
    lv_sysmon_show_memory(NULL);


Performance Monitor
-------------------

Tracks:

- FPS (Frames Per Second)
- CPU usage (%)
- Render time (ms)
- Flush time (ms)
- Self CPU usage (%) if enabled

Display format:

.. code-block:: text

    32 FPS, 45% CPU
    8 ms

Where:

- Line 1: FPS, Total CPU%
- Line 2: Total time (Render | Flush)



Pause and Resume
****************

:cpp:expr:`lv_sysmon_performance_pause(disp)` pauses the perf monitor.

:cpp:expr:`lv_sysmon_performance_resume(disp)` resumes the perf monitor.


Memory Monitor
--------------

Displays:

- Current memory usage (kB and %)
- Peak memory usage (kB)
- Fragmentation (%)

Display format:

.. code-block:: text

    24.8 kB (76%)
    32.4 kB max, 18% frag.


Positioning
-----------

Configure positions in lv_conf.h:

.. code-block:: c

    /* Top-right corner */
    #define LV_USE_PERF_MONITOR_POS LV_ALIGN_TOP_RIGHT

    /* Bottom-right corner */
    #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT



Implementation Details
**********************


Initialization
--------------

Maintains:

- Global memory monitor (``sysmon_mem``)
- Per-display performance structures


Performance Measurement
-----------------------

Event-based collection:

.. container:: tighter-table-3

    +----------------------+--------------------------------+
    | Event                | Measurement                    |
    +======================+================================+
    | LV_EVENT_REFR_START  | Refresh interval start         |
    +----------------------+--------------------------------+
    | LV_EVENT_REFR_READY  | Record refresh duration        |
    +----------------------+--------------------------------+
    | LV_EVENT_RENDER_START| Render time start              |
    +----------------------+--------------------------------+
    | LV_EVENT_RENDER_READY| Record render duration         |
    +----------------------+--------------------------------+
    | LV_EVENT_FLUSH_*     | Measure flush operations       |
    +----------------------+--------------------------------+


Timers
------

- Performance: ``perf_update_timer_cb``
- Memory: ``mem_update_timer_cb``
- Default period: 300ms (``LV_SYSMON_REFR_PERIOD_DEF``)
