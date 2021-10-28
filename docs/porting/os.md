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

Here is some pseudocode to illustrate the concept:

```c
static mutex_t lvgl_mutex;

void lvgl_thread(void)
{
    while(1) {
        mutex_lock(&lvgl_mutex);
        lv_task_handler();
        mutex_unlock(&lvgl_mutex);
        thread_sleep(10); /* sleep for 10 ms */
    }
}

void other_thread(void)
{
    /* You must always hold the mutex while using LVGL APIs */
    mutex_lock(&lvgl_mutex);
    lv_obj_t *img = lv_img_create(lv_scr_act());
    mutex_unlock(&lvgl_mutex);
    
    while(1) {
        mutex_lock(&lvgl_mutex);
        /* change to the next image */
        lv_img_set_src(img, next_image);
        mutex_unlock(&lvgl_mutex);
        thread_sleep(2000);
    }
}
```

## Interrupts
Try to avoid calling LVGL functions from interrupt handlers (except `lv_tick_inc()` and `lv_disp_flush_ready()`). But if you need to do this you have to disable the interrupt which uses LVGL functions while `lv_timer_handler` is running.

It's a better approach to simply set a flag or some value in the interrupt, and periodically check it in an LVGL timer (which is run by `lv_timer_handler`).
