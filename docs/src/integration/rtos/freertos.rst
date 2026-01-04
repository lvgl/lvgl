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
