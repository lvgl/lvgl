```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/porting/os.md
```
# Operating system and interrupts

LVGL is **not thread-safe** by default.

However, in the following conditions it's valid to call LVGL related functions:
- In *events*. Learn more in [Events](/overview/event).
- In *lv_timer*. Learn more in [Timers](/overview/timer).


## Tasks and threads
If you need to use real tasks or threads, you need a mutex which should be invoked before the call of `lv_timer_handler` and released after it.
Also, you have to use the same mutex in other tasks and threads around every LVGL (`lv_...`) related function call and code.
This way you can use LVGL in a real multitasking environment. Just make use of a mutex to avoid the concurrent calling of LVGL functions.

## Interrupts
Try to avoid calling LVGL functions from interrupt handlers (except `lv_tick_inc()` and `lv_disp_flush_ready()`). But if you need to do this you have to disable the interrupt which uses LVGL functions while `lv_timer_handler` is running.
It's a better approach to set a flag or some value and periodically check it in an `lv_timer`.
