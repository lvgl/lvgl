# LittleV Graphics Libraray

![LittlevGL cover](http://www.gl.littlev.hu/home/main_cover.png)

The LittlevGL is a graphics library which allows you to create graphical user interfaces (GUI) from the most simple monochrome designs to amazing GUIs for high resolution TFTs. No double buffering is required to use transparency and smooth animations.

The graphics library is written in C and it is completely hardware independent. You can even run it in a PC simulator without any embedded hardware.

Visit the http://gl.littlev.hu

## Key features
* Hardware independent
* Support any modern microcontroller
* No external RAM, FPU or GPU required
* High resolution TFTs and monochrome displays are also supported
* Scalable and modular
* Clear and well-structured source code
* Color depth options: 24 bit, 16 bit, 8 bot or 1 bit
* Build GUI from simple graphical objects
  * Buttons, Labels, Images
  * Charts, Lists, Bars, Sliders, Text areas etc.
* High level graphical features:
  * Antialiassing (font or full screen)
  * Animations
  * Transparency
  * Gradient colors
  * Smooth dragging and scrolling
* Layers
* Customizable appearance with styles
* Applications for complex tasks
* Can run in a PC simulator
* Actively developed

## Porting
The following functions has to be provided
* hal/disp `disp_fill(x1, y1, x2, y2, color)` to fill area with a color
* hal/disp `disp_map(x1, y1, x2, y2, &color_array)` copy a color map to an area
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
1. Fork it!
2. Create your feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin my-new-feature`
5. Submit a pull request!

If you find an issue, please report it via GitHub!

## Donate
If you are pleased with the graphics library and found it useful pleas support its further development:

[![Donate](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GJV3SC5EHDANS)

