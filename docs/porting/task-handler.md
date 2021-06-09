```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/porting/task-handler.md
```
# Task Handler

To handle the tasks of LVGL you need to call `lv_timer_handler()` periodically in one of the following:
- *while(1)* of *main()* function 
- timer interrupt periodically (lower priority than `lv_tick_inc()`)
- an OS task periodically

The timing is not critical but it should be about 5 milliseconds to keep the system responsive.

Example:
```c
while(1) {
  lv_timer_handler();
  my_delay_ms(5);
}
```

To learn more about timers visit the [Timer](/overview/timer) section.

