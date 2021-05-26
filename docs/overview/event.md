```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/event.md
```
# Events

Events are triggered in LVGL when something happens which might be interesting to the user, e.g. if an object:
- is clicked
- is scrolled
- its value has changed
- redrawn, etc.

## Add events to the object

The user can assign callback functions to an object to see its events. In practice, it looks like this:
```c
lv_obj_t * btn = lv_btn_create(lv_scr_act());
lv_obj_add_event_cb(btn, my_event_cb, LV_EVENT_CLICKED, NULL);   /*Assign an event callback*/

...

static void my_event_cb(lv_event_t * event)
{
    printf("Clicked\n");
}
```
In the example `LV_EVENT_CLICKED` means that only the click event will call `my_event_cb`. See the [list of event codes](#event-codes) for all the options.
`LV_EVENT_ALL` can be used to receive all the events.

The last parameter of `lv_obj_add_event_cb` is a pointer to any custom data that will be available in the event. It will be described later in more detail.

More events can be added to an object, like this:
```c
lv_obj_add_event_cb(obj, my_event_cb_1, LV_EVENT_CLICKED, NULL);
lv_obj_add_event_cb(obj, my_event_cb_2, LV_EVENT_PRESSED, NULL);
lv_obj_add_event_cb(obj, my_event_cb_3, LV_EVENT_ALL, NULL);		/*No filtering, receive all events*/
```

Even the same event callback can be used on an object with different `user_data`. For example:
```c
lv_obj_add_event_cb(obj, increment_on_click, LV_EVENT_CLICKED, &num1);
lv_obj_add_event_cb(obj, increment_on_click, LV_EVENT_CLICKED, &num2);
```

The events will be called in the order as they were added. 


More objects can use the same *event callback*.


## Remove event(s) from an object

Events can be removed from an object with the `lv_obj_remove_event_cb(obj, event_cb)` function or `lv_obj_remove_event_dsc(obj, event_dsc)`. `event_dsc` is a pointer returned by `lv_obj_add_event_cb`. 

## Event codes

The event codes can be grouped into these categories:
- Input device events
- Drawing events
- Other events
- Special events
- Custom events

All objects (such as Buttons/Labels/Sliders etc.) regardless their type receive the *Input device*, *Drawing* and *Other* events. 

However the *Special events* are specific to a particular widget type. See the [widgets' documentation](/widgets/index) to learn when they are sent, 

*Custom events* are added by the user and therefore these are never sent by LVGL.

The following event codes exist:

### Input device events
- `LV_EVENT_PRESSED`      The object has been pressed
- `LV_EVENT_PRESSING`     The object is being pressed (called continuously while pressing)
- `LV_EVENT_PRESS_LOST`   The object is still being pressed but slid cursor/finger off of the object 
- `LV_EVENT_SHORT_CLICKED`    The object was pressed for a short period of time, then released it. Not called if scrolled.
- `LV_EVENT_LONG_PRESSED` Object has been pressed for at least the `long_press_time` specified in the input device driver.  Not called if scrolled.
- `LV_EVENT_LONG_PRESSED_REPEAT`  Called after `long_press_time` in every `long_press_repeat_time` ms.  Not called if scrolled.
- `LV_EVENT_CLICKED`      Called on release if the object not scrolled (regardless to long press)
- `LV_EVENT_RELEASED`     Called in every cases when the object has been released
- `LV_EVENT_SCROLL_BEGIN` Scrolling begins
- `LV_EVENT_SCROLL_END`   Scrolling ends
- `LV_EVENT_SCROLL`       The object was scrolled
- `LV_EVENT_GESTURE`      A gesture is detected. Get the gesture with `lv_indev_get_gesture_dir(lv_indev_get_act());`
- `LV_EVENT_KEY`          A key is sent to the object. Get the key with `lv_indev_get_key(lv_indev_get_act());`
- `LV_EVENT_FOCUSED`      The object is focused 
- `LV_EVENT_DEFOCUSED`    The object is defocused
- `LV_EVENT_LEAVE`        The object is defocused but still selected
- `LV_EVENT_HIT_TEST`     Perform advanced hit-testing. Use `lv_hit_test_info_t * a = lv_event_get_hit_test_info(e)` and check if `a->point` can click the object or not. If not set `a->res = false` 


### Drawing events
- `LV_EVENT_COVER_CHECK` Check if the object fully covers an area. The event parameter is `lv_cover_check_info_t *`.
- `LV_EVENT_REFR_EXT_DRAW_SIZE`  Get the required extra draw area around the object (e.g. for shadow). The event parameter is `lv_coord_t *` to store the size. Overwrite it only with a larger value.
- `LV_EVENT_DRAW_MAIN_BEGIN` Starting the main drawing phase.
- `LV_EVENT_DRAW_MAIN`   Perform the main drawing
- `LV_EVENT_DRAW_MAIN_END`   Finishing the main drawing phase
- `LV_EVENT_DRAW_POST_BEGIN` Starting the post draw phase (when all children are drawn)
- `LV_EVENT_DRAW_POST`   Perform the post draw phase (when all children are drawn)
- `LV_EVENT_DRAW_POST_END`   Finishing the post draw phase (when all children are drawn)
- `LV_EVENT_DRAW_PART_BEGIN` Starting to draw a part. The event parameter is `lv_obj_draw_dsc_t *`. Learn more [here](/overview/drawing).
- `LV_EVENT_DRAW_PART_END`   Finishing to draw a part. The event parameter is `lv_obj_draw_dsc_t *`. Learn more [here](/overview/drawing).

### Other events
- `LV_EVENT_DELETE`       Object is being deleted
- `LV_EVENT_CHILD_CHANGED`    Child was removed/added
- `LV_EVENT_SIZE_CHANGED`    Object coordinates/size have changed
- `LV_EVENT_STYLE_CHANGED`    Object's style has changed
- `LV_EVENT_BASE_DIR_CHANGED` The base dir has changed
- `LV_EVENT_GET_SELF_SIZE`    Get the internal size of a widget

### Special events
- `LV_EVENT_VALUE_CHANGED`    The object's value has changed (i.e. slider moved)
- `LV_EVENT_INSERT`       A text is being inserted to the object. The event data is `char *` being inserted.
- `LV_EVENT_REFRESH`      Notify the object to refresh something on it (for the user)
- `LV_EVENT_READY`        A process has finished
- `LV_EVENT_CANCEL`       A process has been canceled 


### Custom events
Any custom event codes can be registered by `uint32_t MY_EVENT_1 = lv_event_register_id();` 

And can be sent to any object with `lv_event_send(obj, MY_EVENT_1, &some_data)`

## Sending events

To manually send events to an object, use `lv_event_send(obj, <EVENT_CODE> &some_data)`.

For example, it can be used to manually close a message box by simulating a button press (although there are simpler ways of doing this):
```c
/*Simulate the press of the first button (indexes start from zero)*/
uint32_t btn_id = 0;
lv_event_send(mbox, LV_EVENT_VALUE_CHANGED, &btn_id);
```

### Refresh event

`LV_EVENT_REFRESH` is special event because it's designed to be used by the user to notify an object to refresh itself. Some examples:
- notify a label to refresh its text according to one or more variables (e.g. current time)
- refresh a label when the language changes
- enable a button if some conditions are met (e.g. the correct PIN is entered)
- add/remove styles to/from an object if a limit is exceeded, etc

## Fields of lv_event_t

`lv_event_t` is the only parameter passed to event callback and it contains all the data about the event. The following values can be get from it:
- `lv_event_get_code(e)` get the event code
- `lv_event_get_target(e)` get the object to which the event is sent
- `lv_event_get_original_target(e)` get the object to which the event is sent originally sent (different from `lv_event_get_target` if [event bubbling](#event-bubbling) is enabled)
- `lv_event_get_user_data(e)` get the pointer passed as the last parameter of `lv_obj_add_event_cb`.
- `lv_event_get_param(e)` get the parameter passed as the last parameter of `lv_event_send` 


## Event bubbling

If `lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE)` is enabled all events will be sent to the object's parent too. If the parent also has `LV_OBJ_FLAG_EVENT_BUBBLE` enabled the event will be sent to its parent too, and so on. 

The *target* parameter of the event is always the current target object, not the original object. To get the original target call `lv_event_get_original_target(e)` in the event handler.  



## Examples

```eval_rst

.. include:: ../../examples/event/index.rst

```
