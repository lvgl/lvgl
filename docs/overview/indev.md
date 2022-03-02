```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/indev.md
```
# Input devices

An input device usually means:
- Pointer-like input device like touchpad or mouse
- Keypads like a normal keyboard or simple numeric keypad
- Encoders with left/right turn and push options
- External hardware buttons which are assigned to specific points on the screen


``` important:: Before reading further, please read the [Porting](/porting/indev) section of Input devices
```

## Pointers

### Cursor

Pointer input devices (like a mouse) can have a cursor.

```c
...
lv_indev_t * mouse_indev = lv_indev_drv_register(&indev_drv);

LV_IMG_DECLARE(mouse_cursor_icon);                          /*Declare the image source.*/
lv_obj_t * cursor_obj = lv_img_create(lv_scr_act());       /*Create an image object for the cursor */
lv_img_set_src(cursor_obj, &mouse_cursor_icon);             /*Set the image source*/
lv_indev_set_cursor(mouse_indev, cursor_obj);               /*Connect the image  object to the driver*/
```

Note that the cursor object should have `lv_obj_clear_flag(cursor_obj, LV_OBJ_FLAG_CLICKABLE)`.
For images, *clicking* is disabled by default.

### Gestures
Pointer input devices can detect basic gestures. By default, most of the widgets send the gestures to its parent, so finally the gestures can be detected on the screen object in a form of an `LV_EVENT_GESTURE` event. For example:

```c
void my_event(lv_event_t * e)
{
  lv_obj_t * screen = lv_event_get_current_target(e);
  lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_act());
  switch(dir) {
    case LV_DIR_LEFT:
      ...
      break;
    case LV_DIR_RIGHT:
      ...
      break;
    case LV_DIR_TOP:
      ...
      break;
    case LV_DIR_BOTTOM:
      ...
      break;
  }
}

...

lv_obj_add_event_cb(screen1, my_event, LV_EVENT_GESTURE, NULL);
```

To prevent passing the gesture event to the parent from an object use `lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE)`.

Note that, gestures are not triggered if an object is being scrolled.

## Keypad and encoder

You can fully control the user interface without a touchpad or mouse by using a keypad or encoder(s). It works similar to the *TAB* key on the PC to select an element in an application or a web page.

### Groups

Objects you want to control with a keypad or encoder need to be added to a *Group*.
In every group there is exactly one focused object which receives the pressed keys or the encoder actions.
For example, if a [Text area](/widgets/core/textarea) is focused and you press some letter on a keyboard, the keys will be sent and inserted into the text area.
Similarly, if a [Slider](/widgets/core/slider) is focused and you press the left or right arrows, the slider's value will be changed.

You need to associate an input device with a group. An input device can send key events to only one group but a group can receive data from more than one input device.

To create a group use `lv_group_t * g = lv_group_create()` and to add an object to the group use `lv_group_add_obj(g, obj)`.

To associate a group with an input device use `lv_indev_set_group(indev, g)`, where `indev` is the return value of `lv_indev_drv_register()`

#### Keys
There are some predefined keys which have special meaning:
- **LV_KEY_NEXT** Focus on the next object
- **LV_KEY_PREV** Focus on the previous object
- **LV_KEY_ENTER** Triggers `LV_EVENT_PRESSED/CLICKED/LONG_PRESSED` etc. events
- **LV_KEY_UP** Increase value or move upwards
- **LV_KEY_DOWN** Decrease value or move downwards
- **LV_KEY_RIGHT** Increase value or move to the right
- **LV_KEY_LEFT** Decrease value or move to the left
- **LV_KEY_ESC**  Close or exit (E.g. close a [Drop down list](/widgets/core/dropdown))
- **LV_KEY_DEL**  Delete (E.g. a character on the right in a [Text area](/widgets/core/textarea))
- **LV_KEY_BACKSPACE** Delete a character on the left (E.g. in a [Text area](/widgets/core/textarea))
- **LV_KEY_HOME** Go to the beginning/top (E.g. in a [Text area](/widgets/core/textarea))
- **LV_KEY_END** Go to the end (E.g. in a [Text area](/widgets/core/textarea))

The most important special keys are `LV_KEY_NEXT/PREV`, `LV_KEY_ENTER` and `LV_KEY_UP/DOWN/LEFT/RIGHT`.
In your `read_cb` function, you should translate some of your keys to these special keys to support navigation in a group and interact with selected objects.

Usually, it's enough to use only `LV_KEY_LEFT/RIGHT` because most objects can be fully controlled with them.

With an encoder you should use only `LV_KEY_LEFT`, `LV_KEY_RIGHT`, and `LV_KEY_ENTER`.

#### Edit and navigate mode

Since a keypad has plenty of keys, it's easy to navigate between objects and edit them using the keypad. But encoders have a limited number of "keys" and hence it is difficult to navigate using the default options. *Navigate* and *Edit* modes are used to avoid this problem with encoders.

In *Navigate* mode, an encoder's `LV_KEY_LEFT/RIGHT` is translated to `LV_KEY_NEXT/PREV`. Therefore, the next or previous object will be selected by turning the encoder.
Pressing `LV_KEY_ENTER` will change to *Edit* mode.

In *Edit* mode, `LV_KEY_NEXT/PREV` is usually used to modify an object.
Depending on the object's type, a short or long press of `LV_KEY_ENTER` changes back to *Navigate* mode.
Usually, an object which cannot be pressed (like a [Slider](/widgets/core/slider)) leaves *Edit* mode upon a short click. But with objects where a short click has meaning (e.g. [Button](/widgets/core/btn)), a long press is required.

#### Default group
Interactive widgets - such as buttons, checkboxes, sliders, etc. - can be automatically added to a default group.
Just create a group with `lv_group_t * g = lv_group_create();` and set the default group with `lv_group_set_default(g);`

Don't forget to assign one or more input devices to the default group with ` lv_indev_set_group(my_indev, g);`.

### Styling

If an object is focused either by clicking it via touchpad or focused via an encoder or keypad it goes to the `LV_STATE_FOCUSED` state. Hence, focused styles will be applied to it. 

If an object switches to edit mode it enters the `LV_STATE_FOCUSED | LV_STATE_EDITED` states so these style properties will be shown. 

For a more detailed description read the [Style](https://docs.lvgl.io/v7/en/html/overview/style.html) section.

## API


### Input device

```eval_rst

.. doxygenfile:: lv_indev.h
  :project: lvgl

```

### Groups

```eval_rst

.. doxygenfile:: lv_group.h
  :project: lvgl

```
