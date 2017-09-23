# LittleV Graphics Libraray

![LittlevGL cover](http://www.gl.littlev.hu/home/main_cover_small.png)

LittlevGL is a graphics library to create Graphical User Interfaces (GUI) on TFT, LCD or monochrome displays for microcontroller based embedded systems.

Transparency, anti-aliassing and smooth animations can be used with no double buffering so typically no external memories are required. Layouts, scrolling, word-wrapping, layers and other features make your job easier. 

The graphics library is written in C and it is completely hardware independent. You can even run it in a PC simulator without any embedded hardware.

Homepage: http://gl.littlev.hu

## Key features
* Hardware independent, support any modern microcontroller
* High resolution TFTs, monochrome or any type of display supported (24/16/8/1 bit color depth)
* External RAM, FPU or GPU not required just optional
* Build GUI from simple graphical objects
  * Buttons, Labels, Images
  * Charts, Lists, Bars, Sliders, Text areas etc.
* Create or delete graphical object dynamically in run time   
* High level graphical features without double buffering
  * Antialiassing (font or full screen)
  * Animations
  * Transparency
  * Gradient colors
  * Smooth dragging and scrolling
* Built-in features
  * Layouts (to auto-arrange items)
  * Scrolling
  * Auto-size (aligned to the content)
  * Word wrapping
  * Layers
* Customizable appearance with styles
* Applications for complex tasks
* Can run in a PC simulator
* Modular and well-structured source code
* Actively developed

## Porting
The following functions has to be provided
* hal/disp `disp_fill(x1, y1, x2, y2, color)` to fill area with a color
* hal/disp `disp_map(x1, y1, x2, y2, &color_array)` copy a color map to an area
* hal/disp `disp_color_cpy(dest, src, length, opa)` copy pixel, optional for GPU
* hal/indev `indev_get(id, &x, &y)` get the *x* and *y* coordinates from an input device (e.g. touch pad)
* hal/systick `systick_get()` get a system tick with 1 ms resolution
* hal/systick `systick_elapse(prev_time)` get the elapsed milliseconds sience *prev_time*

See the [example HAL](https://github.com/littlevgl/hal) repository!

## Requirements
* [Misc. library](https://github.com/littlevgl/misc) is used by the graphics library

## Project set-up
1. Clone or download the following repositories:
   * lvgl:`git clone https://github.com/littlevgl/lvgl.git`  
   * misc: `git clone https://github.com/littlevgl/misc.git` 
   * hal: `git clone https://github.com/littlevgl/hal.git`
2. Create project with your prefered IDE and add the **lvgl**, **misc** and **hal** folders 
3. Add your projects **root directory as include path** 
4. Write your display, touch pad and system tick **drivers in hal**
5. Copy *lvgl/lv_conf_templ.h* as **lv_conf.h** and *misc/misc_conf_templ.h* as **misc_conf.h** to the projects root folder
6. In the *_conf.h files delete the first `#if 0` and its `#endif`. Let the default configurations at first.
7. In your *main.c* include: 
   * #include "misc/misc.h" 
   * #include "misc/os/ptask.h"
   * #include "lvgl/lvgl.h"   
8. In your *main.c* intialize:
   * **misc_init()**;
   * your_systick_init();
   * your_disp_init();
   * your_indev_init();
   * **lv_init()**;
10. To **test** create a label: `lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);`  
11. In the main *while(1)* call `ptask_handler();` and make a few milliseconds delay (e.g. `your_delay_ms(5);`) 
12. Compile the code and load it to your embedded hardware

## PC Simulator
If you don't have got an embedded hardware you can test the graphics library in a PC simulator. The simulator uses [SDL2](https://www.libsdl.org/) to emulate a display on your monitor and a touch pad with your mouse.

There is a pre-configured PC project for **Eclipse CDT** in this repository: https://github.com/littlevgl/proj_pc

## Contributing
See [CONTRIBUTING.md](https://github.com/littlevgl/lvgl/blob/master/docs/CONTRIBUTING.md)

## Donate
If you are pleased with the graphics library and found it useful please support its further development:

[![Donate](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GJV3SC5EHDANS)

