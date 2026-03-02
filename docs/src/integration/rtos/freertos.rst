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

Alternatively, on multi-core environments (like the ESP32 SMP), this default
implementation can be insufficient or inaccurate. In this case, you can set
``LV_OS_IDLE_PERCENT_CUSTOM`` to ``1`` in ``lv_conf.h`` to disable the default
implementation. Then, you can provide a custom implementation of
:cpp:expr:`lv_os_get_idle_percent` tailored to your environment
(e.g., using ``uxTaskGetSystemState()``) without modifying LVGL source files.

Example: Custom implementation for ESP32 SMP
********************************************

For multi-core environments like the ESP32 (which uses dual-core SMP), the default 
implementation might not track all idle tasks. Below is an example of a custom 
implementation using ``uxTaskGetSystemState()``. This requires 
``configGENERATE_RUN_TIME_STATS`` and ``configUSE_TRACE_FACILITY`` to be enabled 
in your ``FreeRTOSConfig.h`` (or via menuconfig for ESP-IDF).

.. code-block:: c

    #include "FreeRTOS.h"
    #include "task.h"
    #include <string.h>

    uint32_t lv_os_get_idle_percent(void) {
        static uint32_t last_idle_time = 0;
        static uint32_t last_total_time = 0;
        
        uint32_t ulTotalRunTime;
        uint32_t ulIdleTime = 0;
        
        TaskStatus_t *pxTaskStatusArray;
        UBaseType_t uxArraySize, x;
        
        uxArraySize = uxTaskGetNumberOfTasks();
        pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));
        
        if (pxTaskStatusArray == NULL) {
            return 0;
        }
        
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);
        for (x = 0; x < uxArraySize; x++) {
            /* ESP32 has IDLE0 and IDLE1 tasks */
            if (strcmp(pxTaskStatusArray[x].pcTaskName, "IDLE0") == 0 ||
                strcmp(pxTaskStatusArray[x].pcTaskName, "IDLE1") == 0) {
                ulIdleTime += pxTaskStatusArray[x].ulRunTimeCounter;
            }
        }
        vPortFree(pxTaskStatusArray);
        
        uint32_t idle_diff = ulIdleTime - last_idle_time;
        uint32_t total_diff = ulTotalRunTime - last_total_time;
        
        last_idle_time = ulIdleTime;
        last_total_time = ulTotalRunTime;
        
        if (total_diff == 0) return 0;
        
        /* Combined idle percentage across both cores (0-200 on dual-core) */
        uint32_t idle_pct = (idle_diff * 100) / total_diff;

        /* For Sysmon, we usually want average load across all cores (0-100) */
        return idle_pct / 2; /* Adjust divisor based on your CPU core count */
    }
