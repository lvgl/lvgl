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

If you intend to use `lv_timer_handler()` in a super-loop, a helper function`lv_run_timer_handler_in_period(__ms)` is provided to simplify the porting:

```c
while(1) {
    ...
    lv_timer_handler_run_period(5); /* run lv_timer_handler() every 5ms */
    ...
}
```

 In OS environment, you can use it together with the delay or sleep provided by OS:

```c
while (1) {
    lv_timer_handler_run_in_period(5); /* run lv_timer_handler() every 5ms */
    sleep(5); /* sleep 5ms */
} 
```

To learn more about timers visit the [Timer](/overview/timer) section.

