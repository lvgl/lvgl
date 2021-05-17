```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/get-started/quick-overview.md
```

# Quick overview

Here you can learn the most important things about LVGL.
You should read it first to get a general impression and read the detailed [Porting](/porting/index) and [Overview](/overview/index) sections after that.

## Get started in a simulator

Instead of porting LVGL to an embedded hardware, it's highly recommended to get started in a simulator first. 

LVGL is ported to many IDEs to be sure you will find your favorite one. 
Go to the [Simulators](/get-started/pc-simulator) section to get ready-to-use projects that can be run on your PC. 
This way you can save the time of  porting for now and make some experience with LVGL immediately. 

## Add LVGL into your project
If you rather want to try LVGL on your own project follow these steps:

- [Download](https://github.com/lvgl/lvgl/archive/master.zip) or Clone the library from GitHub with `git clone https://github.com/lvgl/lvgl.git`.
- Copy the `lvgl` folder into your project.
- Copy `lvgl/lv_conf_template.h` as `lv_conf.h` next to the `lvgl` folder, change the first `#if 0` to `1` to enable the file's content and set the `LV_COLOR_DEPTH` defines.
- Include `lvgl/lvgl.h` in files where you need to use LVGL related functions.
- Call `lv_tick_inc(x)` every `x` milliseconds in a Timer or Task (`x` should be between 1 and 10). It is required for the internal timing of LVGL. 
Alternatively, configure `LV_TICK_CUSTOM` (see `lv_conf.h`) so that LVGL can retrieve the current time directly.
- Call `lv_init()`
- Create a draw buffer: LVGL will render the graphics here first, and seed the rendered image to the display. 
The buffer size can be set freely but 1/10 screen size is a good starting point. 
```c
static lv_disp_darw_buf_t draw_buf;
static lv_color_t buf1[DISP_HOR_RES * DISP_VER_RES / 10];                        /*Declare a buffer for 1/10 screen size*/
lv_disp_draw_buf_init(&draw_buf, buf1, NULL, MY_DISP_HOR_RES * MY_DISP_VER_SER / 10);  /*Initialize the display buffer.*/
```
- Implement and register a function which can copy the rendered image to an area of your display:
```c
lv_disp_drv_t disp_drv;               /*Descriptor of a display driver*/
lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
disp_drv.flush_cb = my_disp_flush;    /*Set your driver function*/
disp_drv.buffer = &draw_buf;          /*Assign the buffer to the display*/
disp_drv.hor_res = MY_DISP_HOR_RES;   /*Set the horizontal resolution of the display*/
disp_drv.hor_res = MY_DISP_VER_RES;   /*Set the verizontal resolution of the display*/
lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/

void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    int32_t x, y;
    /*It's a very slow but simple implementation.
     *`set_pixel` needs to be written by you to a set pixel on the screen*/
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            set_pixel(x, y, *color_p);
            color_p++;
        }
    }

    lv_disp_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}

```
- Implement and register a function which can read an input device. E.g. for a touch pad:
```c
lv_indev_drv_t indev_drv;                  /*Descriptor of a input device driver*/
lv_indev_drv_init(&indev_drv);             /*Basic initialization*/
indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

bool my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    /*`touchpad_is_pressed` and `touchpad_get_xy` needs to be implemented by you*/
    if(touchpad_is_pressed()) {
      data->state = LV_INDEV_STATE_PRESSED;
      touchpad_get_xy(&data->point.x, &data->point.y);
    } else {
      data->state = LV_INDEV_STATE_RELEASED;
    }
 
}
```
- Call `lv_timer_handler()` periodically every few milliseconds in the main `while(1)` loop or in an Operation system task.
It will redraw the screen if required, handle input devices, animation etc.

For a more detailed guide go to the [Porting](/porting/index) section.

## Learn the basics

### Widgets

The graphical elements like Buttons, Labels, Sliders, Charts etc. are called objects or widgets. Go to [Widgets](/widgets/index) to see the full list of available widgets.

Every object has a parent object where it is create. For example if a label is created on a button, the button is the parent of label. 

The child object moves with the parent and if the parent is deleted the children will be deleted too. 

Children can be visible only on their parent. It other words, the parts of the children out of the parent are clipped.

A Screen is the "root" parent. You can have any number of screens. 

To get the current screen call `lv_scr_act()`, and to load a screen use `lv_scr_load(scr1)`.

You can create a new object with `lv_<type>_create(parent)`. It will return an `lv_obj_t *` variable that can be used as a reference to the object to set its parameters.

For example:
```c
lv_obj_t * slider1 = lv_slider_create(lv_scr_act());
```

To set some basic attribute `lv_obj_set_<paramters_name>(obj, <value>)` function can be used. For example:
```c
lv_obj_set_x(btn1, 30);
lv_obj_set_y(btn1, 10);
lv_obj_set_size(btn1, 200, 50);
```

The widgets have type specific parameters too which can be set by `lv_<widget_type>_set_<paramters_name>(obj, <value>)` functions. For example:
```c
lv_slider_set_value(slider1, 70, LV_ANIM_ON);
```

To see the full API visit the documentation of the widgets or the related header file (e.g. [lvgl/src/widgets/lv_slider.h](https://github.com/lvgl/lvgl/blob/master/src/widgets/lv_slider.h)).



### Events
Events are used to inform the user if something has happened with an object. 
You can assign one or more callbacks to an object which will be called if the object is clicked, released, dragged, being deleted etc. 

It should look like this:

```c
lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL); /*Assign a callback to the button*/

...

void btn_event_cb(lv_event_t * e)
{
    printf("Clicked\n");
}
```

Instead of `LV_EVENT_CLICKED` `LV_EVENT_ALL` can be used too to call the callback for any event. 

From `lv_event_t * e` the current event code can be get with
```c
lv_event_code_t code = lv_event_get_code(e);
```

The object that triggered the event can be retrieved with
```c
lv_obj_t * obj = lv_event_get_target(e);
```

To learn all features of the events go to the [Event overview](/overview/event) section.

### Parts
Widgets might be built from one or more *parts*. For example a button has only one part called `LV_PART_MAIN`.
However, a [Slider](/widgets/core/slider) has `LV_PART_MAIN`, `LV_PART_INDICATOR` and `LV_PART_KNOB`.

By using parts you can apply different styles to different parts. (See below)

To learn which parts are used by which object read the widgets' documentation.

### States
The objects can be in a combination of the following states:
- `LV_STATE_DEFAULT`  Normal, released state
- `LV_STATE_CHECKED`  Toggled or checked state
- `LV_STATE_FOCUSED` Focused via keypad or encoder or clicked via touchpad/mouse 
- `LV_STATE_FOCUS_KEY`  Focused via keypad or encoder but not via touchpad/mouse 
- `LV_STATE_EDITED` Edit by an encoder
- `LV_STATE_HOVERED` Hovered by mouse (not supported now)
- `LV_STATE_PRESSED` eing pressed
- `LV_STATE_SCROLLED` Being scrolled
- `LV_STATE_DISABLED` Disabled 

For example, if you press an object it will automatically goes to `LV_STATE_FOCUSED` and `LV_STATE_PRESSED` state and when you release it, the  `LV_STATE_PRESSED` state will be removed.
 
To check if an object is in a geven state use `lv_obj_has_state(obj, LV_STATE_...)`. It will return `true` if the object "has" the given state at that moment.

To manually add remove the states use
```c
lv_obj_add_state(obj, LV_STATE_...);
lv_obj_clear_state(obj, LV_STATE_...);
```

### Styles
Styles contains properties such as background color, border width, font, etc to describe the appearance of the objects.

The styles are `lv_style_t` variables. Only their pointer is saved in the objects so they need to be static or global.
Before using a style it needs to be initialized with `lv_style_init(&style1)`. After that properties can be added. For example:
```
static lv_style_t style1;
lv_style_init(&style1);
lv_style_set_bg_color(&style1, lv_color_hex(0xa03080))
lv_style_set_border_width(&style1, 2))
```
See the full list of properties go [here](/overview/style.html#properties).


The styles are assigned to an object's part and state. For example to *"Use this style on the slider's indicator when the slider is pressed"*:
```c
lv_obj_add_style(slider1, &style1, LV_PART_INDICATOR | LV_STATE_PRESSED);
```

If the *part* is `LV_PART_MAIN` it can be omitted:
```c
lv_obj_add_style(btn1, &style1, LV_STATE_PRESSED); /*Equal to LV_PART_MAIN | LV_STATE_PRESSED*/
```

Similarly, `LV_STATE_DEFAULT` can be omitted too:
```c
lv_obj_add_style(slider1, &style1, LV_PART_INDICATOR); /*Equal to LV_PART_INDICATOR | LV_STATE_DEFAULT*/
```

For `LV_STATE_DEFAULT` and `LV_PART_MAIN` simply write `0`:
```c
lv_obj_add_style(btn1, &style1, 0); /*Equal to LV_PART_MAIN | LV_STATE_DEFAULT*/
```


The styles can be cascaded (similarly to CSS). It means you can add more styles to a part of an object. 
For example `style_btn` can set a default button appearance, and `style_btn_red` can overwrite the background color to make the button red: 
```c
lv_obj_add_style(btn1, &style_btn, 0);
lv_obj_add_style(btn1, &style1_btn_red, 0);
```


If a property is not set on for the current state the style with `LV_STATE_DEFAULT` will be used. If the property is not defined even in the default state a default value is used.

Some properties (typically the text-related ones) can be inherited. It means if a property is not set in an object it will be searched in its parents too. 
For example, you can set the font once in the screen's style and every text will inherit it by default. 


Local style properties also can be added to the objects. It creates a style is inside the object that is used only by the object:
```c
lv_obj_set_style_bg_color(slider1, lv_color_hex(0x2080bb), LV_PART_INDICATOR | LV_STATE_PRESSED);
``` 

To learn all the features of styles see the [Style overview](/overview/style) section


### Themes
Themes are the default styles of the objects. 
The styles from the themes are applied automatically when the objects are created. 

You can select the theme to use in `lv_conf.h`. 

## Examples

```eval_rst

.. include:: ../../examples/get_started/index.rst
```

## Micropython
Learn more about [Micropython](/get-started/micropython).
```python
# Create a Button and a Label
scr = lv.obj()
btn = lv.btn(scr)
btn.align(lv.scr_act(), lv.ALIGN.CENTER, 0, 0)
label = lv.label(btn)
label.set_text("Button")

# Load the screen
lv.scr_load(scr)
```

