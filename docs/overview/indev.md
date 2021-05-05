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

Pointer input devices can have a cursor. (typically for mouses)

```c
...
lv_indev_t * mouse_indev = lv_indev_drv_register(&indev_drv);

LV_IMG_DECLARE(mouse_cursor_icon);                          /*Declare the image file.*/
lv_obj_t * cursor_obj =  lv_img_create(lv_scr_act(), NULL); /*Create an image object for the cursor */
lv_img_set_src(cursor_obj, &mouse_cursor_icon);             /*Set the image source*/
lv_indev_set_cursor(mouse_indev, cursor_obj);               /*Connect the image  object to the driver*/

```

Note that the cursor object should have `lv_obj_set_click(cursor_obj, false)`.
For images, *clicking* is disabled by default.

## Keypad and encoder

You can fully control the user interface without touchpad or mouse using a keypad or encoder(s). It works similar to the *TAB* key on the PC to select the element in an application or a web page.

### Groups

The objects, you want to control with keypad or encoder, needs to be added to a *Group*.
In every group, there is exactly one focused object which receives the pressed keys or the encoder actions.
For example, if a [Text area](/widgets/core/textarea) is focused and you press some letter on a keyboard, the keys will be sent and inserted into the text area.
Similarly, if a [Slider](/widgets/core/slider) is focused and you press the left or right arrows, the slider's value will be changed.

You need to associate an input device with a group. An input device can send the keys to only one group but, a group can receive data from more than one input device too.

To create a group use `lv_group_t * g = lv_group_create()` and to add an object to the group use `lv_group_add_obj(g, obj)`.

To associate a group with an input device use `lv_indev_set_group(indev, g)`, where `indev` is the return value of `lv_indev_drv_register()`

#### Keys
There are some predefined keys which have special meaning:
- **LV_KEY_NEXT** Focus on the next object
- **LV_KEY_PREV** Focus on the previous object
- **LV_KEY_ENTER** Triggers `LV_EVENT_PRESSED/CLICKED/LONG_PRESSED` etc. events
- **LV_KEY_UP** Increase value or move upwards
- **LV_KEY_DOWN** Decrease value or move downwards
- **LV_KEY_RIGHT** Increase value or move the the right
- **LV_KEY_LEFT** Decrease value or move the the left
- **LV_KEY_ESC**  Close or exit (E.g. close a [Drop down list](/widgets/core/dropdown))
- **LV_KEY_DEL**  Delete (E.g. a character on the right in a [Text area](/widgets/core/textarea))
- **LV_KEY_BACKSPACE** Delete a character on the left (E.g. in a [Text area](/widgets/core/textarea))
- **LV_KEY_HOME** Go to the beginning/top (E.g. in a [Text area](/widgets/core/textarea))
- **LV_KEY_END** Go to the end (E.g. in a [Text area](/widgets/core/textarea)))

The most important special keys are `LV_KEY_NEXT/PREV`, `LV_KEY_ENTER` and `LV_KEY_UP/DOWN/LEFT/RIGHT`.
In your `read_cb` function, you should translate some of your keys to these special keys to navigate in the group and interact with the selected object.

Usually, it's enough to use only `LV_KEY_LEFT/RIGHT` because most of the objects can be fully controlled with them.

With an encoder, you should use only `LV_KEY_LEFT`, `LV_KEY_RIGHT`, and `LV_KEY_ENTER`.

#### Edit and navigate mode

Since a keypad has plenty of keys, it's easy to navigate between the objects and edit them using the keypad. But, the encoders have a limited number of "keys" hence, it is difficult to navigate using the default options. *Navigate* and *Edit* are created to avoid this problem with the encoders.

In *Navigate* mode, the encoders `LV_KEY_LEFT/RIGHT` is translated to `LV_KEY_NEXT/PREV`. Therefore the next or previous object will be selected by turning the encoder.
Pressing `LV_KEY_ENTER` will change to *Edit* mode.

In *Edit* mode, `LV_KEY_NEXT/PREV` is usually used to edit the object.
Depending on the object's type, a short or long press of `LV_KEY_ENTER` changes back to *Navigate* mode.
Usually, an object which can not be pressed (like a [Slider](/widgets/core/slider)) leaves *Edit* mode on short click. But with objects where short click has meaning (e.g. [Button](/widgets/core/btn)), a long press is required.

### Styling

If an object is focused either by clicking it via touchpad, or focused via an encoder or keypad it goes to `LV_STATE_FOCUSED`. Hence focused styles will be applied on it. 

If the object goes to edit mode it goes to `LV_STATE_FOCUSED | LV_STATE_EDITED` state so these style properties will be shown. 

For a more detaild description read the [Style](https://docs.lvgl.io/v7/en/html/overview/style.html) section.

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
