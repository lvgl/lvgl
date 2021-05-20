```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/porting/indev.md
```
# Input device interface

## Types of input devices

To register an input device an `lv_indev_drv_t` variable has to be initialized:

```c
lv_indev_drv_t indev_drv;
lv_indev_drv_init(&indev_drv);      /*Basic initialization*/
indev_drv.type =...                 /*See below.*/
indev_drv.read_cb =...              /*See below.*/
/*Register the driver in LVGL and save the created input device object*/
lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);
```

`type` can be
- `LV_INDEV_TYPE_POINTER` touchpad or mouse
- `LV_INDEV_TYPE_KEYPAD` keyboard or keypad
- `LV_INDEV_TYPE_ENCODER` encoder with left/right turn and push options
- `LV_INDEV_TYPE_BUTTON` external buttons virtually pressing the screen

`read_cb` is a function pointer which will be called periodically to report the current state of an input device.

Visit [Input devices](/overview/indev) to learn more about input devices in general.

###  Touchpad, mouse or any pointer
Input devices that can click points of the screen belong to this category.

```c
indev_drv.type = LV_INDEV_TYPE_POINTER;
indev_drv.read_cb = my_input_read;

...

void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data)
{
  if(touchpad_pressed) {
    data->point.x = touchpad_x;
    data->point.y = touchpad_y;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED; 
  }
}
```

To set a mouse cursor use `lv_indev_set_cursor(my_indev, &img_cursor)`. (`my_indev` is the return value of `lv_indev_drv_register`)

### Keypad or keyboard

Full keyboards with all the letters or simple keypads with a few navigation buttons belong here.

To use a keyboard/keypad:
- Register a `read_cb` function with `LV_INDEV_TYPE_KEYPAD` type.
- An object group has to be created: `lv_group_t * g = lv_group_create()`  and objects have to be added to it with `lv_group_add_obj(g, obj)`
- The created group has to be assigned to an input device: `lv_indev_set_group(my_indev, g)` (`my_indev` is the return value of `lv_indev_drv_register`)
- Use `LV_KEY_...` to navigate among the objects in the group. See `lv_core/lv_group.h` for the available keys.

```c
indev_drv.type = LV_INDEV_TYPE_KEYPAD;
indev_drv.read_cb = keyboard_read;

...

void keyboard_read(lv_indev_drv_t * drv, lv_indev_data_t*data){
  data->key = last_key();            /*Get the last pressed or released key*/

  if(key_pressed()) data->state = LV_INDEV_STATE_PRESSED;
  else data->state = LV_INDEV_STATE_RELEASED;
}
```

### Encoder
With an encoder you can do 4 things:
1. Press its button
2. Long-press its button
3. Turn left
4. Turn right

In short, the Encoder input devices work like this:
- By turning the encoder you can focus on the next/previous object.
- When you press the encoder on a simple object (like a button), it will be clicked.
- If you press the encoder on a complex object (like a list, message box, etc.) the object will go to edit mode whereby turning the encoder you can navigate inside the object.
- To leave edit mode press long the button.


To use an *Encoder* (similarly to the *Keypads*) the objects should be added to groups.


```c
indev_drv.type = LV_INDEV_TYPE_ENCODER;
indev_drv.read_cb = encoder_read;

...

void encoder_read(lv_indev_drv_t * drv, lv_indev_data_t*data){
  data->enc_diff = enc_get_new_moves();

  if(enc_pressed()) data->state = LV_INDEV_STATE_PRESSED;
  else data->state = LV_INDEV_STATE_RELEASED;
}
```

#### Using buttons with Encoder logic
In addition to standard encoder behavior, you can also utilize its logic to navigate(focus) and edit widgets using buttons. 
This is especially handy if you have only few buttons available, or you want to use other buttons in addition to encoder wheel.

You need to have 3 buttons available:
- `LV_KEY_ENTER` will simulate press or pushing of the encoder button
- `LV_KEY_LEFT` will simulate turning encoder left
- `LV_KEY_RIGHT` will simulate turning encoder right
- other keys will be passed to the focused widget

If you hold the keys it will simulate encoder click with period specified in `indev_drv.long_press_rep_time`.

```c
indev_drv.type = LV_INDEV_TYPE_ENCODER;
indev_drv.read_cb = encoder_with_keys_read;

...

bool encoder_with_keys_read(lv_indev_drv_t * drv, lv_indev_data_t*data){
  data->key = last_key();            /*Get the last pressed or released key*/
                                     /* use LV_KEY_ENTER for encoder press */
  if(key_pressed()) data->state = LV_INDEV_STATE_PRESSED;
  else {
      data->state = LV_INDEV_STATE_RELEASED;
      /* Optionally you can also use enc_diff, if you have encoder*/
      data->enc_diff = enc_get_new_moves();
  }

  return false; /*No buffering now so no more data read*/
}
```

### Button
*Buttons* mean external "hardware" buttons next to the screen which are assigned to specific coordinates of the screen.
If a button is pressed it will simulate the pressing on the assigned coordinate. (Similarly to a touchpad)

To assign buttons to coordinates use `lv_indev_set_button_points(my_indev, points_array)`.   
`points_array` should look like `const lv_point_t points_array[] =  { {12,30},{60,90}, ...}`

``` important::  The points_array can't go out of scope. Either declare it as a global variable or as a static variable inside a function.
```

```c
indev_drv.type = LV_INDEV_TYPE_BUTTON;
indev_drv.read_cb = button_read;

...

void button_read(lv_indev_drv_t * drv, lv_indev_data_t*data){
    static uint32_t last_btn = 0;   /*Store the last pressed button*/
    int btn_pr = my_btn_read();     /*Get the ID (0,1,2...) of the pressed button*/
    if(btn_pr >= 0) {               /*Is there a button press? (E.g. -1 indicated no button was pressed)*/
       last_btn = btn_pr;           /*Save the ID of the pressed button*/
       data->state = LV_INDEV_STATE_PRESSED;  /*Set the pressed state*/
    } else {
       data->state = LV_INDEV_STATE_RELEASED; /*Set the released state*/
    }

    data->btn = last_btn;            /*Save the last button*/
}
```

## Other features

### Parameters

The default value of the following parameters can changed in `lv_indev_drv_t`:
- `scroll_limit` Number of pixels to slide before actually scrolling the object.
- `scroll_throw`  Scroll throw (momentum) slow-down in [%]. Greater value means faster slow-down.
- `long_press_time` Press time to send `LV_EVENT_LONG_PRESSED` (in milliseconds)
- `long_press_rep_time` Interval of sending `LV_EVENT_LONG_PRESSED_REPEAT` (in milliseconds)
- `read_timer` pointer to the `lv_rimer` which reads the input device. Its parameters can be changed by `lv_timer_...()` functions. `LV_INDEV_DEF_READ_PERIOD` in `lv_conf.h` sets the default read period.

### Feedback

Besides `read_cb` a `feedback_cb` callback can be also specified in `lv_indev_drv_t`.
`feedback_cb` is called when any type of event is sent by the input devices. (independently from its type). It allows making feedback for the user e.g. to play a sound on `LV_EVENT_CLICKED`.


### Associating with a display
Every Input device is associated with a display. By default, a new input device is added to the lastly created or the explicitly selected (using `lv_disp_set_default()`) display.
The associated display is stored and can be changed in `disp` field of the driver.

### Buffered reading
By default LVGL calls `read_cb` periodically. This way there is a chance that some user gestures are missed. 

To solve this you can write an event driven driver for your input device that buffers measured data. In `read_cb` you can set the buffered data instead of reading the input device. 
You can set the `data->continue_reding` flag to tell that LVGL there is more data to read and it should call the `read_cb` again.

## Further reading

- [lv_port_indev_template.c](https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_indev_template.c) for a template for your own driver.
- [INdev features](/overview/display) to learn more about higher level input device features.

## API

```eval_rst

.. doxygenfile:: lv_hal_indev.h
  :project: lvgl

```
