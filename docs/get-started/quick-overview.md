```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/get-started/quick-overview.md
```

# Quick overview

Here you can learn the most important things about LVGL.
You should read it first to get a general impression and read the detailed [Porting](/porting/index) and [Overview](/overview/index) sections after that.

## Get started in a simulator

Instead of porting LVGL to an embedded hardware, it's highly recommended to get started in a simulator first. 

LVGL is ported to many IDEs to be sure you will find your faviourite one. Go to [Simulators](/get-started/pc-simulator) to get ready-to-use projects which can be run on your PC. This way you can save the porting for now and make some experience with LVGL immediately. 

## Add LVGL into your project

The following steps show how to setup LVGL on an embedded system with a display and a touchpad.

- [Download](https://github.com/lvgl/lvgl/archive/master.zip) or Clone the library from GitHub with `git clone https://github.com/lvgl/lvgl.git`
- Copy the `lvgl` folder into your project
- Copy `lvgl/lv_conf_template.h` as `lv_conf.h` next to the `lvgl` folder, change the first `#if 0` to `1` to enable the file's content and set at least `LV_HOR_RES_MAX`, `LV_VER_RES_MAX` and `LV_COLOR_DEPTH` defines.
- Include `lvgl/lvgl.h` where you need to use LVGL related functions.
- Call `lv_tick_inc(x)` every `x` milliseconds **in a Timer or Task** (`x` should be between 1 and 10). It is required for the internal timing of LVGL. Alternatively, configure `LV_TICK_CUSTOM` (see `lv_conf.h`) so that LVGL can retrieve the current time directly.
- Call `lv_init()`
- Create a display buffer for LVGL. LVGL will render the graphics here first, and seed the rendered image to the display. The buffer size can be set freely but 1/10 screen size is a good starting point. 
```c
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * LV_VER_RES_MAX / 10];                     /*Declare a buffer for 1/10 screen size*/
lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX / 10);    /*Initialize the display buffer*/
```
- Implement and register a function which can **copy the rendered image** to an area of your display:
```c
lv_disp_drv_t disp_drv;               /*Descriptor of a display driver*/
lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
disp_drv.flush_cb = my_disp_flush;    /*Set your driver function*/
disp_drv.buffer = &disp_buf;          /*Assign the buffer to the display*/
lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/

void my_disp_flush(lv_disp_drv_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            set_pixel(x, y, *color_p);  /* Put a pixel to the display.*/
            color_p++;
        }
    }

    lv_disp_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}

```
- Implement and register a function which can **read an input device**. E.g. for a touch pad:
```c
lv_indev_drv_t indev_drv;                  /*Descriptor of a input device driver*/
lv_indev_drv_init(&indev_drv);             /*Basic initialization*/
indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

bool my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    data->state = touchpad_is_pressed() ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    if(data->state == LV_INDEV_STATE_PR) touchpad_get_xy(&data->point.x, &data->point.y);

    return false; /*Return `false` because we are not buffering and no more data to read*/
}
```
- Call `lv_task_handler()` periodically every few milliseconds in the main `while(1)` loop, in Timer interrupt or in an Operation system task.
It will redraw the screen if required, handle input devices etc.

For a more detailed guide go to the [Porting](https://docs.lvgl.io/v7/en/html/porting/index.html) section.

## Learn the basics

### Widgets

The graphical elements like Buttons, Labels, Sliders, Charts etc are called objects or widgets in LVGL. Go to [Widgets](/widgets/index) to see the full list of available widgets.

Every object has a parent object where it is create. For example if a label is created on a button, the button is the parent of label. 
The child object moves with the parent and if the parent is deleted the children will be deleted too. 

Children can be visible only on their parent. It other words, the parts of the children out of the parent are clipped.

A *screen* is the "root" parent. You can have any number of screens. To get the current screen call `lv_scr_act()`, and to load a screen use `lv_scr_load(scr1)`.

You can create a new object with `lv_<type>_create(parent, obj_to_copy)`. It will return an `lv_obj_t *` variable which should be used as a reference to the object to set its parameters.
The first parameter is the desired *parent*, the second parameters can be an object to copy (`NULL` if unused).
For example:
```c
lv_obj_t * slider1 = lv_slider_create(lv_scr_act(), NULL);
```

To set some basic attribute `lv_obj_set_<paramters_name>(obj, <value>)` function can be used. For example:
```c
lv_obj_set_x(btn1, 30);
lv_obj_set_y(btn1, 10);
lv_obj_set_size(btn1, 200, 50);
```

The objects has type specific parameters too which can be set by `lv_<type>_set_<paramters_name>(obj, <value>)` functions. For example:
```c
lv_slider_set_value(slider1, 70, LV_ANIM_ON);
```

To see the full API visit the documentation of the widgets or the related header file (e.g. [lvgl/src/lv_widgets/lv_slider.h](https://github.com/lvgl/lvgl/blob/master/src/lv_widgets/lv_slider.h)).

### Events
Events are used to inform the user if something has happened with an object. You can assign a callback to an object which will be called if the object is clicked, released, dragged, being deleted etc. It should look like this:

```c
lv_obj_set_event_cb(btn, btn_event_cb);                 /*Assign a callback to the button*/

...

void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        printf("Clicked\n");
    }
}
```

Learn more about the events in the [Event overview](/overview/event) section.

### Parts
Widgets might be built from one or more parts. For example a button has only one part called `LV_BTN_PART_MAIN`.
 However, a [Page](/widgets/page) has `LV_PAGE_PART_BG`, `LV_PAGE_PART_SCROLLABLE`, `LV_PAGE_PART_SCROLLBAR` and `LV_PAGE_PART_EDGE_FLASG`.

Some parts are *virtual* (they are not real object, just drawn on the fly, such as the scrollbar of a page) but other parts are *real* (they are real object, such as the scrollable part of the page).

Parts come into play when you want to set the styles and states of a given part of an object. (See below)

### States
The objects can be in a combination of the following states:
- **LV_STATE_DEFAULT** Normal, released
- **LV_STATE_CHECKED** Toggled or checked
- **LV_STATE_FOCUSED** Focused via keypad or encoder or clicked via touchpad/mouse
- **LV_STATE_EDITED** Edit by an encoder
- **LV_STATE_HOVERED** Hovered by mouse (not supported now)
- **LV_STATE_PRESSED** Pressed
- **LV_STATE_DISABLED** Disabled or inactive 

For example, if you press an object it will automatically get the `LV_STATE_PRESSED` state and when you release it, the state will be removed.
 
To get the current state use `lv_obj_get_state(obj, part)`. It will return the `OR`ed states. 
For example, this is a valid state for a checkbox: `LV_STATE_CHECKED | LV_STATE_PRESSED | LV_STATE_FOCUSED`

### Styles
Styles can be assigned to the parts of an object to change their appearance. 
A style can describe for example the background color, border width, text font and so on. See the full list [here](https://docs.lvgl.io/v7/en/html/overview/style.html#properties).

The styles can be cascaded (similarly to CSS). It means you can add more styles to a part of an object. 
For example `style_btn` can set a default button appearance, and `style_btn_red` can overwrite some properties to make the button red- 

Every style property you set is specific to a state. For example, you can set a different background color for `LV_STATE_DEFAULT` and `LV_STATE_PRESSED`. 
The library finds the best match between the state of the given part and the available style properties. For example if the object is in pressed state and the border width is specified for pressed state, then it will be used.
However, if it's not specified for pressed state, the `LV_STATE_DEFAULT`'s border width will be used. If the border width not defined for `LV_STATE_DEFAULT` either, a default value will be used.

Some properties (typically the text-related ones) can be inherited. It means if a property is not set in an object it will be searched in its parents too. 
For example you can set the font once in the screen's style and every text will inherit it by default. 

Local style properties also can be added to the objects.

### Themes
Themes are the default styles of the objects. 
The styles from the themes are applied automatically when the objects are created. 

You can select the theme to use in `lv_conf.h`. 

## Examples

### Button with label

```eval_rst
.. image:: /lv_examples/src/lv_ex_get_started/lv_ex_get_started_1.*
  :alt: Simple button with label with LVGL

.. literalinclude:: /lv_examples/src/lv_ex_get_started/lv_ex_get_started_1.c
  :language: c
```


### Styling buttons

```eval_rst
.. image:: /lv_examples/src/lv_ex_get_started/lv_ex_get_started_2.*
  :alt: Styling buttons with LVGL

.. literalinclude:: /lv_examples/src/lv_ex_get_started/lv_ex_get_started_2.c
  :language: c
```


### Slider and alignment

```eval_rst
.. image:: /lv_examples/src/lv_ex_get_started/lv_ex_get_started_3.*
  :alt: Create a slider with LVGL

.. literalinclude:: /lv_examples/src/lv_ex_get_started/lv_ex_get_started_3.c
  :language: c
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
