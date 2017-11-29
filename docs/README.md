# LittleV Graphics Libraray

![LittlevGL cover](http://www.gl.littlev.hu/home/main_cover_small.png)

Littlev Graphics Library provides everithing you need to add graphical user interface to your embedded stytem which meets the requirements in the 21th century.

Homepage: http://gl.littlev.hu

## Key features
- Everithing you need to build a GUI (buttons, charts, list, images etc)
- Animations, opacity, smooth scrolling, anti aliasing to impress your cutomers
- Multi language support with UTF-8 support
- Fully customizable appearance
- Scalable to operate with a few memory (80 kB flash, 10 kB RAM)
- Hardware independent to use with any MCU or display (TFT, LCD, monochrome)
- Touchpad, mouse, keyboard and external button support 
- Only a sinlge frame buffer required in internal RAM or in an external display conroller 
- OS, External memory or GPU suppoted but not required  
- Written in C for maximal compability
- PC simulator to develop without embedded hardware
- Tutorials, code exampes, style themes for rapid development
- Clear online documentation
- Advanced support, and professional GUI development service
- Free and open source

## Porting
### Tick interface
The LittlevGL uses a system tick. Your only task is to call `lv_tick_handler()` in every milliseconds in an interrupt.

### Display interface
To set up a diplay an 'lv_disp_drv_t' variable has to be initialized:
```c
lv_disp_drv_t disp_drv;
lv_disp_drv_init(&disp_drv);           /*Basic initialization*/
disp_drv. ... = ...                    /*Initilaize the field here. See below.*/
disp_drv_register(&disp_drv);
```

#### Using internal buffering (VDB)
The graphics library works with an internal buffering mechanism to to create advances graphics features with only one frame buffer. The inter buffer is called VDB (Virtual Display Buffer) in its size can be adjusted in lv_conf.h.
When `LV_VDB_SIZE` not zero then the internal buffering is used and you have to provide a function which flushes the buffers content to the display:
`disp_drv.disp_flush = my_disp_flush;`

In the flush function you can use DMA or any hardware to do the flushing in the background, but when the flushing is ready you **have to call `lv_flush_ready()`**

#### Using harware acceleration (GPU)
The `mem_blend` and `mem_fill` field of a display driver is used to interface with a GPU. 
`disp_drv.mem_blend = my_mem_blend;  /*blends two color arrays using opacity*/` 
`disp_drv.mem_fill = my_mem_fill;    /*fills an array with a color*/` 

The GPU related functions can be used only if internal buffering (VDB) is enabled

#### Unbuffered drawing
It is possible to draw directly to a framebuffer via two functions:
`disp_drv.disp_fill = my_mem_blend;  /*fill an area in the frame buffer*/` 
`disp_drv.disp_map = my_mem_fill;    /*copy a color_map (e.g. image) into the framebuffer*/` 

### Input device interface
To set up an input device an 'lv_indev_drv_t' variable has to be initialized:
```c    
lv_indev_drv_t indev_drv;
lv_indev_drv_init(&indev_drv);     /*Basic initialization*/
indev_drv.type = ...               /*See below.*/
indev_drv.read_fp = ...            /*See below.*/
lv_indev_register(&indev_drv);
```
The `type` can be `LV_INDEV_TYPE_POINTER` (e.g touchpad) or `LV_INDEV_TYPE_KEYPAD` (e.g. keyboard)
`read_fp` is a function pointer which will be called periodically to report the current state of an input device. It can also buffer data and return `false` when no more data te read ot `true` is the buffer is not empty.

#### Touchpad, mouse or any pointer
`
indev_drv.type = LV_INDEV_TYPE_POINTER;
indev_drv.read_fp = my_input_read; 
`
The read function should look like this:
```c
bool my_input_read(lv_indev_data_t *data) {
    data->point.x = touchpad_x;
    data->point.y = touchpad_y;
    data->state = LV_INDEV_EVENT_PR or LV_INDEV_EVENT_REL;

    return false;  /*No buffering so no more data read*/
```

#### Keypad or keyboard

The read function should look like this:
```c
bool keyboard_read(lv_indev_data_t *data) {
    if(key_pressed()) {
        data->state = LV_INDEV_EVENT_PR;
        data->key = get_key();
    } else {
        data->state = LV_INDEV_EVENT_REL;
        data->key = 0;
    }

    return false;   /*No buffering so no more data read*/
}
```

## Project set-up
1. Clone or download the lvgl repository: `git clone https://github.com/littlevgl/lvgl.git`
2. Create project with your prefered IDE and add the lvgl folder
3. Copy *lvgl/lv_conf_templ.h* as **lv_conf.h** next to the lvgl folder
4. In the *_conf.h files delete the first `#if 0` and its `#endif`. Let the default configurations at first.
5. In your *main.c*: #include "lvgl/lvgl.h"   
6. In your *main function*:
   * lvgl_init();
   * tick, display and input device initialization (se above)
10. To **test** create a label: `lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);`  
11. In the main *while(1)* call `lv_task_handler();` and make a few milliseconds delay (e.g. `my_delay_ms(5);`) 
12. Compile the code and load it to your embedded hardware

## PC Simulator
If you don't have got an embedded hardware you can test the graphics library in a PC simulator. The simulator uses [SDL2](https://www.libsdl.org/) to emulate a display on your monitor and a touch pad with your mouse.

There is a pre-configured PC project for **Eclipse CDT** in this repository: https://github.com/littlevgl/proj_pc

## Contributing
See [CONTRIBUTING.md](https://github.com/littlevgl/lvgl/blob/master/docs/CONTRIBUTING.md)

## Donate
If you are pleased with the graphics library and found it useful please support its further development:

[![Donate](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GJV3SC5EHDANS)

