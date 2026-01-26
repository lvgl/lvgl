.. _freertos:

========
FreeRTOS
========

Usage
*****

To enable LVGL's FreeRTOS support, just set ``LV_USE_OS LV_OS_FREERTOS`` in
``lv_conf.h``. After this, LVGL:

- GPUs will yield the task while waiting for the GPU to complete (lowering CPU usage)
- :cpp:expr:`lv_os_get_idle_percent` will return the OS-aware CPU usage
- :cpp:expr:`lv_sleep_ms` will be a wrapper to ``vTaskDelay``

Note that LVGL itself doesn't contain FreeRTOS, meaning it's assumed that
it's added and configured externally.

CPU Usage Measurement
*********************

If FreeRTOS is enabled, LVGL can measure the CPU usage by measuring the time spent in
the idle function. To enable this, the following needs to be added in
``User_FreeRTOSConfig.h``:

.. code-block:: c

    void lv_freertos_task_switch_in(const char * name);
    void lv_freertos_task_switch_out(void);

    #define traceTASK_SWITCHED_IN()   lv_freertos_task_switch_in(pxCurrentTCB->pcTaskName);
    #define traceTASK_SWITCHED_OUT()  lv_freertos_task_switch_out();
