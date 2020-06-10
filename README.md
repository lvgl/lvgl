<h1 align="center"> LVGL - Light and Versatile Graphics Library</h1>
<p align="center">
<a href="https://github.com/lvgl/lvgl/blob/master/LICENCE.txt"><img src="https://img.shields.io/badge/licence-MIT-blue.svg"></a>
<a href="https://github.com/lvgl/lvgl/releases/tag/v7.0.0"><img src="https://img.shields.io/badge/version-7.0.0-blue.svg"></a>
</p>

<p align="center">
<img src="https://lvgl.io/assets/images/img_1.png">
</p>

<p align="center">
LVGL provides everything you need to create embedded GUI with easy-to-use graphical elements, beautiful visual effects and low memory footprint. 
</p>

<h4 align="center">
<a href="https://lvgl.io">Website </a> &middot; 
<a href="https://lvgl.io/demos">Live demo</a> &middot; 
<a href="https://docs.lvgl.io/">Docs</a> &middot; 
<a href="https://forum.lvgl.io">Forum</a> &middot;
<a href="https://blog.lvgl.io/">Blog</a>
</h4>

---

- [Features](#features)
- [Supported devices](#supported-devices)
- [Quick start in a simulator](#quick-start-in-a-simulator)
- [Add LVGL to your project](#add-lvgl-to-your-project)
- [Learn the basics](#learn-the-basics)
- [Examples](#examples)
- [Release policy](#release-policy)
- [Contributing](#contributing)


## Features
* **Powerful building blocks** buttons, charts, lists, sliders, images, etc.
* **Advanced graphics** with animations, anti-aliasing, opacity, smooth scrolling
* **Simultaneously use various input devices** touchscreen, mouse, keyboard, encoder, buttons, etc.
* **Simultaneously use multiple displays** i.e. monochrome and color display
* **Multi-language support** with UTF-8 encoding, Bidirectional support, and Arabic text handling
* **Fully customizable** graphical elements
* **Hardware independent** to use with any microcontroller or display
* **Scalable** to operate with little memory (64 kB Flash, 10 kB RAM)
* **OS, External memory and GPU** supported but not required
* **Single frame buffer** operation even with advances graphical effects
* **Written in C** for maximal compatibility (C++ compatible)
* **Micropython Binding** exposes [LVGL API in Micropython](https://blog.lvgl.io/2019-02-20/micropython-bindings)
* **Simulator** to develop on PC without embedded hardware
* **Tutorials, examples, themes** for rapid development
* **Documentation** and API references

## Supported devices
Basically, every modern controller  (which is able to drive a display) is suitable to run LVGL. The minimal requirements are:
- 16, 32 or 64 bit microcontroller or processor
- &gt; 16 MHz clock speed is recommended
- Flash/ROM: &gt; 64 kB for the very essential components (&gt; 180 kB is recommended)
- RAM: 
  - Static RAM usage: ~2 kB depending on the used features and objects types
  - Stack: &gt; 2kB (&gt; 8 kB is recommended)
  - Dynamic data (heap): &gt; 2 KB (&gt; 16 kB is recommended if using several objects).
    Set by `LV_MEM_SIZE` in *lv_conf.h*. 
  - Display buffer:  &gt; *"Horizontal resolution"* pixels (&gt; 10 &times; *"Horizontal resolution"* is recommended) 
- C99 or newer compiler

*Note that the memory usage might vary depending on the architecture, compiler and build options.*

Just to mention some **platforms**:
- STM32F1, STM32F3, [STM32F4](https://blog.lvgl.io/2017-07-15/stm32f429_disco_port), [STM32F7](https://github.com/lvgl/lv_port_stm32f746_disco_sw4stm32)
- Microchip dsPIC33, PIC24, PIC32MX, PIC32MZ
- NXP Kinetis, LPC, iMX
- [Linux frame buffer](https://blog.lvgl.io/2018-01-03/linux_fb) (/dev/fb)
- [Raspberry PI](http://www.vk3erw.com/index.php/16-software/63-raspberry-pi-official-7-touchscreen-and-littlevgl)
- [Espressif ESP32](https://github.com/lvgl/lv_port_esp32)
- Nordic nrf52
- Quectell M66

## Quick start in a simulator
The easiest way to get started with LVGL is to run it in a simulator on your PC without any embedded hardware. 

Choose a project with your favourite IDE:

|   Eclipse   |  CodeBlocks | Visual Studio | PlatformIO | Qt Creator |
|-------------|-------------|---------------|-----------|------------|
|  [![Eclipse](https://raw.githubusercontent.com/lvgl/docs/master/v7/misc//eclipse.jpg)](https://github.com/lvgl/lv_sim_eclipse_sdl) | [![CodeBlocks](https://raw.githubusercontent.com/lvgl/docs/master/v7/misc//codeblocks.jpg)](https://github.com/lvgl/lv_sim_codeblocks_win) | [![VisualStudio](https://raw.githubusercontent.com/lvgl/docs/master/v7/misc//visualstudio.jpg)](https://github.com/lvgl/lv_sim_visual_studio_sdl) |   [![PlatformIO](https://raw.githubusercontent.com/lvgl/docs/master/v7/misc//platformio.jpg)](https://github.com/lvgl/lv_platformio) | [![QtCreator](https://raw.githubusercontent.com/lvgl/docs/master/v7/misc//qtcreator.jpg)](https://blog.lvgl.io/2019-01-03/qt-creator) |
| Cross-platform<br>with SDL<br>(Recommended on<br>Linux and Mac) | Native Windows | Windows<br>with SDL | Cross-platform<br>with SDL | Cross-platform<br>with SDL |


## Add LVGL to your project

The steps below show how to setup LVGL on an embedded system with a display and a touchpad. 
You can use the [Simulators](https://docs.lvgl.io/v7/en/html/get-started/pc-simulator) to get ready to use projects which can be run on your PC. 

1. [Download](https://github.com/lvgl/lvgl/archive/master.zip) or [Clone](https://github.com/lvgl/lvgl) the library
2. Copy the `lvgl` folder into your project
3. Copy `lvgl/lv_conf_template.h` as `lv_conf.h` next to the `lvgl` folder, change the `#if 0` statement near the top of the file to `#if 1` and set at least `LV_HOR_RES_MAX`, `LV_VER_RES_MAX` and `LV_COLOR_DEPTH`.
4. Include `lvgl/lvgl.h` where you need to use LVGL related functions.
5. Call `lv_tick_inc(x)` every `x` milliseconds (should be 1..10) in a Timer or Task. It is required for the internal timing of LVGL.
6. Call `lv_init()`
7. Create a display buffer for LVGL
```c
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];                     /*Declare a buffer for 10 lines*/
lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);    /*Initialize the display buffer*/
```
8. Implement and register a function which can copy a pixel array to an area of your display:
```c
lv_disp_drv_t disp_drv;               /*Descriptor of a display driver*/
lv_disp_drv_init(&disp_drv);          /*Basic initialization*/
disp_drv.flush_cb = my_disp_flush;    /*Set your driver function*/
disp_drv.buffer = &disp_buf;          /*Assign the buffer to the display*/
lv_disp_drv_register(&disp_drv);      /*Finally register the driver*/
    
void my_disp_flush(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            my_set_pixel(x, y, *color_p);  /* Put a pixel to the display.*/
            color_p++;
        }
    }

    lv_disp_flush_ready(disp);         /* Indicate you are ready with the flushing*/
}
    
```
9. Implement and register a function which can read an input device. E.g. for a touch pad:
```c
lv_indev_drv_init(&indev_drv);             /*Descriptor of a input device driver*/
indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/

bool my_touchpad_read(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    data->state = my_touchpad_is_pressed() ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 
    if(data->state == LV_INDEV_STATE_PR) touchpad_get_xy(&data->point.x, &data->point.y);

    return false; /*Return `false` because we are not buffering and no more data to read*/
}
```
10. Call `lv_task_handler()` periodically every few milliseconds in the main `while(1)` loop, in Timer interrupt or in an Operation system task. 
It will redraw the screen if required, handle input devices etc. 

For more detailed desription visit the [Porting](https://docs.lvgl.io/v7/en/html/porting/index.html) section of the documentation.

## Learn the basics

In this section you can read the very basics of LVGL. 
For a more detailed guide check the [Quick overview](https://docs.lvgl.io/v7/en/html/get-started/quick-overview.html#learn-the-basics) in the documentation. 

### Widgets (Objects)

The graphical elements like Buttons, Labels, Sliders, Charts etc are called objects or widgets in LVGL. Go to [Widgets](https://docs.lvgl.io/v7/en/html/widgets/index) to see the full list of available types.

Every object has a parent object. The child object moves with the parent and if you delete the parent the children will be deleted too. Children can be visible only on their parent.

The *screen* are the "root" parents. To get the current screen call `lv_scr_act()`.

You can create a new object with `lv_<type>_create(parent, obj_to_copy)`. It will return an `lv_obj_t *` variable which should be used as a reference to the object to set its parameters later. 
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

The objects have type specific parameters too which can be set by `lv_<type>_set_<paramters_name>(obj, <value>)` functions. For example:
```c
lv_slider_set_value(slider1, 70, LV_ANIM_ON);
```

To see the full API visit the documentation of the object types or the related header file (e.g. `lvgl/src/lv_objx/lv_slider.h`).


To create a new screen pass `NULL` as the fisrt paramater of a *create* function:
```c
lv_obj_t * scr2 = lv_obj_create(NULL, NULL);    /*Create a screen*/
lv_scr_load(scr2);                              /*Load the new screen*/
```

### Styles
Widgets are created with a default appearance but it can be changed by adding new styles to them. A new style can be created like this:
```c
static lv_style_t style1; /*Should be static, global or dynamically allocated*/
lv_style_init(&style1);
lv_style_set_bg_color(&style1, LV_STATE_DEFAULT, LV_COLOR_RED);  /*Default background color*/ 
lv_style_set_bg_color(&style1, LV_STATE_PRESSED, LV_COLOR_BLUE); /*Pressed background color*/
```

The wigedt have *parts* which can be referenced via `LV_<TYPE>_PART_<PART_NAME>`. E.g. `LV_BTN_PART_MAIN` or `LV_SLIDER_PART_KNOB`. See the documentation of the widgets to see the exisitng parts.

To add the style to a button:
```c
lv_obj_add_style(btn1, LV_BTN_PART_MAIN, &style1);
```

To remove all styles from a part of an object:
```cc
lv_obj_reset_style_list(obj, LV_OBJ_PART_MAIN);
```

Learn more in [Style overview](https://docs.lvgl.io/v7/en/html/overview/style) section.

### Events
Events are used to inform the user if something has happened with an object. You can assign a callback to an object which will be called if the object is clicked, released, dragged, being deleted etc. It should look like this: 

```c
lv_obj_set_event_cb(btn, btn_event_cb);     /*Assign a callback to the button*/

...

void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        printf("Clicked\n");
    }
}
```

Learn more about the events in the [Event overview](https://docs.lvgl.io/v7/en/html/overview/event) section. 


## Examples 

### Button with label
```c
lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);     /*Add a button the current screen*/
lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
lv_obj_set_size(btn, 100, 50);                          /*Set its size*/
lv_obj_set_event_cb(btn, btn_event_cb);                 /*Assign a callback to the button*/

lv_obj_t * label = lv_label_create(btn, NULL);          /*Add a label to the button*/
lv_label_set_text(label, "Button");                     /*Set the labels text*/

...

void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        printf("Clicked\n");
    }
}
```
![LVGL button with label example](https://docs.lvgl.io/v7/en/misc/simple_button_example.gif)

### Use LVGL from Micropython
Learn more about [Micropython](https://docs.lvgl.io/en/html/get-started/micropython).
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

## Release policy
LVGL follows the rules of [Semantic versioning](https://semver.org/):
- Major versions for incompatible API changes. E.g. v5.0.0, v6.0.0
- Minor version for new but backward-compatible functionalities. E.g. v6.1.0, v6.2.0
- Patch version for backward-compatible bug fixes. E.g. v6.1.1, v6.1.2

Branches:
- `master` most recent version, patches are merged directly here. 
- `dev` merge new features here until they are merged into `master`.
- `release/vX` there is a branch for every major version to allow adding specific, not forward compatible fixes.

LVGL has a monthly periodic release cycle.
- **1st Tuesday of the month** 
  - Make a major, minor, or patch release from `master` depending on the new features.
  - After that merge only patches into `master` and add new features into the `dev`.
- **3rd Tuesday of the month** 
  - Make a patch release from `master`.
  - After that merge the new features from the `dev` to `master` branch. 
  - In the rest of the month merge only patches into `master` and new features into `dev` branch.
  
## Contributing
To ask questions please use the [Forum](https://forum.lvgl.io).
For development-related things (bug reports, feature suggestions) use [GitHub's Issue tracker](https://github.com/lvgl/lvgl/issues). 

If you are interested in contributing to LVGL you can
- **Help others** in the [Forum](https://forum.lvgl.io).
- **Inspire people** by speaking about your project in [My project](https://forum.lvgl.io/c/my-projects) category in the Forum.
- **Improve and/or translate the documentation.** Go to the [Documentation](https://github.com/lvgl/docs) repository to learn more
- **Write a blog post** about your experiences. See how to do it in the [Blog](https://github.com/lvgl/blog) repository
- **Report and/or fix bugs** in [GitHub's issue tracker](https://github.com/lvgl/lvgl/issues)
- **Help in the developement**. Check the [Open issues](https://github.com/lvgl/lvgl/issues) especially the ones with [Help wanted](https://github.com/lvgl/lvgl/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) label and tell your ideas about a topic or implement a feature.

Before sending Pull requests, please read the following guides:
- [Contributing guide](https://github.com/lvgl/lvgl/blob/master/docs/CONTRIBUTING.md)
- [Coding style guide](https://github.com/lvgl/lvgl/blob/master/docs/CODING_STYLE.md)
