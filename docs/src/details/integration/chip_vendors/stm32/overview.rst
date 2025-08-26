========
Overview
========


About
*****

ST Microelectronics is a chip vendor and board manufacturer of 32-bit ARM
MCUs and MPUs.


Application Development
-----------------------

STM32CubeIDE is an Eclipse-based IDE which is typically used to develop for STM32.
It can configure your project, build,
flash, and debug. It integrates LVGL similarly to any other Eclipse-based IDE.
The projects it generates are STM32 HAL based. The STM32 HAL is maintained by ST
and is a good fit for many applications. STM32CubeIDE can optionally generate projects that
use FreeRTOS.

STM32 projects can be created with STM32CubeIDE or with other frameworks that don't use the STM32 HAL.
There are many ways to develop for STM32
including manual bare metal, Zephry RTOS, NuttX RTOS, ChibiOS, and many more. Frameworks
like Zephyr and NuttX often cause a project to be treated like "a Zephry project on STM32"
rather than an "STM32 project running Zephry", so their documentation is where to start.
Also worth mentioning is STM32CubeMX; a graphical tool for generating the boilerplate code for an
STM32 HAL project to build it outside of STM32CubeIDE.
Furthermore, STM32CubeCLT is a toolset for integrating ST proprietary tools into
other IDEs such as Visual Studio Code.


Ready-to-Use Projects
---------------------

LVGL maintains a few projects for STM32 boards. See `the boards page <https://lvgl.io/boards#st>`__
for ST boards that have been certified by LVGL and have up-to-date LVGL integration, and check
`all the repos <https://github.com/orgs/lvgl/repositories?q=lv_port_stm>`__
for other ST board repos that exist. Follow the README.md files in those repos
for specific instructions to get started with them.


LVGL Support for STM32 Graphical Peripherals
--------------------------------------------

LVGL has good support for the graphical hardware acceleration peripherals that some STM32
models feature. Sometimes these features can be utilized together at the same time.
See the individual pages about them.

- :ref:`LTDC: display controller. <stm32 ltdc driver>`
- :ref:`DMA2D: asynchronous pixel data memory operations. <dma2d>``
- :ref:`NemaGFX: GPU for rendering 2D graphics primitives. <nema_gfx>``

Each page mentions if/how they can be used with each other.












Including LVGL in a Project
---------------------------

- Create or open a project in STM32CubeIDE.
- ``git clone`` or copy LVGL into your project directory.
- In the STM32CubeIDE **Project Explorer** pane: right click on the
  LVGL folder and select **Add/remove include path…**. If
  this doesn't appear or doesn't work, you can review your project
  include paths under the **Project** -> **Properties** menu, and then
  navigating to **C/C++ Build** -> **Settings** -> **Include paths**, and
  ensuring that the LVGL directory is listed.

Now that the source files are included in your project, follow the instructions to
:ref:`add LVGL to your project <adding_lvgl_to_your_project>` and to create the
``lv_conf.h`` file and initialize the display.
Before manually initializing your display though, check to see
if your project uses the LTDC (LCD-TFT Display Controller) peripheral. If it
does, you can simply :ref:`use LVGL's LTDC driver <stm32 ltdc driver>`.
For info about driving a display specifically on an STM32, read the
:ref:`STM32 LCD guide <lcd_stm32_guide>`.




















STM32 HAL Example
-----------------

A minimal example using STM32CubeIDE, and HAL. \* When setting up
**Pinout and Configuration** using the **Device Configuration Tool**,
select **System Core** -> **SYS** and ensure that **Timebase Source** is
set to **SysTick**. \* Configure any other peripherals (including the
LCD panel), and initialize them in *main.c*. \* ``#include "lvgl.h"`` in
the *main.c* file. \* Create some frame buffer(s) as global variables:

.. code-block:: c

   /* Frame buffers
    * Static or global buffer(s). The second buffer is optional
    * TODO: Adjust color format and choose buffer size. DISPLAY_WIDTH * 10 is one suggestion. */
   #define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /* will be 2 for RGB565 */
   #define BUFF_SIZE (DISPLAY_WIDTH * 10 * BYTES_PER_PIXEL)
   static uint8_t buf_1[BUFF_SIZE];
   static uint8_t buf_2[BUFF_SIZE];

- In your ``main()`` function, after initialising your CPU,
    peripherals, and LCD panel, call :cpp:func:`lv_init` to initialise LVGL.
    You can then create the display driver using
    :cpp:func:`lv_display_create`, and register the frame buffers using
    :cpp:func:`lv_display_set_buffers`.

    .. code-block:: c

        //Initialise LVGL UI library
        lv_init();

        lv_display_t * disp = lv_display_create(WIDTH, HEIGHT); /* Basic initialization with horizontal and vertical resolution in pixels */
        lv_display_set_flush_cb(disp, my_flush_cb); /* Set a flush callback to draw to the display */
        lv_display_set_buffers(disp, buf_1, buf_2, sizeof(buf_1), LV_DISPLAY_RENDER_MODE_PARTIAL); /* Set an initialized buffer */

- Create some dummy Widgets to test the output:

    .. code-block:: c

        /* Change Active Screen's background color */
        lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);
        lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);

        /* Create a spinner */
        lv_obj_t * spinner = lv_spinner_create(lv_screen_active());
        lv_obj_set_size(spinner, 64, 64);
        lv_obj_align(spinner, LV_ALIGN_BOTTOM_MID, 0, 0);


- Add a call to :cpp:func:`lv_timer_handler` inside your ``while(1)`` loop:

  .. code-block:: c

      /* Infinite loop */
      while (1)
      {
          lv_timer_handler();
          HAL_Delay(5);
      }


- Add a call to :cpp:func:`lv_tick_inc` inside the :cpp:func:`SysTick_Handler` function. Open the *stm32xxxx_it.c*
  file (the name will depend on your specific MCU), and update the :cpp:func:`SysTick_Handler` function:

  .. code-block:: c

      void SysTick_Handler(void)
      {
          /* USER CODE BEGIN SysTick_IRQn 0 */

          HAL_SYSTICK_IRQHandler();
          lv_tick_inc(1);
          #ifdef USE_RTOS_SYSTICK
              osSystickHandler();
          #endif

          /* USER CODE END SysTick_IRQn 0 */
          HAL_IncTick();
          /* USER CODE BEGIN SysTick_IRQn 1 */

          /* USER CODE END SysTick_IRQn 1 */
      }


- Finally, write the callback function, ``my_flush_cb``, which will send the display buffer to your LCD panel. Below is
  one example, but it will vary depending on your setup.

  .. code-block:: c

      void my_flush_cb(lv_display_t * disp, const lv_area_t * area, lv_color_t * color_p)
      {
          //Set the drawing region
          set_draw_window(area->x1, area->y1, area->x2, area->y2);

          int height = area->y2 - area->y1 + 1;
          int width = area->x2 - area->x1 + 1;

          //We will do the SPI write manually here for speed
          HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
          //CS low to begin data
          HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

          //Write colour to each pixel
          for (int i = 0; i < width * height; i++) {
              uint16_t color_full = (color_p->red << 11) | (color_p->green << 5) | (color_p->blue);
              parallel_write(color_full);

              color_p++;
          }

          //Return CS to high
          HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

          /* IMPORTANT!!!
           * Inform the graphics library that you are ready with the flushing */
          lv_display_flush_ready(disp);
      }


FreeRTOS Example
----------------

A minimal example using STM32CubeIDE, HAL, and CMSISv1 (FreeRTOS).
*Note that we have not used Mutexes in this example, however LVGL is* **NOT**
*thread safe and so Mutexes should be used*. See: :ref:`threading`
\* ``#include "lvgl.h"`` \* Create your frame buffer(s) as global variables:

.. code-block:: c

    /* Frame buffers
     * Static or global buffer(s). The second buffer is optional */
    #define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) /* will be 2 for RGB565 */
    /* TODO: Declare your own BUFF_SIZE appropriate to your system. */
    #define BUFF_SIZE (DISPLAY_WIDTH * 10 * BYTES_PER_PIXEL)
    static uint8_t buf_1[BUFF_SIZE];
    static uint8_t buf_2[BUFF_SIZE];

- In your ``main`` function, after your peripherals (SPI, GPIOs, LCD
  etc) have been initialised, initialise LVGL using :cpp:func:`lv_init`,
  create a new display driver using :cpp:func:`lv_display_create`, and
  register the frame buffers using :cpp:func:`lv_display_set_buffers`.

  .. code-block:: c

   /* Initialise LVGL UI library */
   lv_init();
   lv_display_t *display = lv_display_create(WIDTH, HEIGHT); /* Create the display */
   lv_display_set_flush_cb(display, my_flush_cb);            /* Set a flush callback to draw to the display */
   lv_display_set_buffers(display, buf_1, buf_2, sizeof(buf_1), LV_DISPLAY_RENDER_MODE_PARTIAL); /* Set an initialized buffer */

   /* Register the touch controller with LVGL - Not included here for brevity. */


- Create some dummy Widgets to test the output:

  .. code-block:: c

    /* Change Active Screen's background color */
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);

    /* Create a spinner */
    lv_obj_t * spinner = lv_spinner_create(lv_screen_active());
    lv_obj_set_size(spinner, 64, 64);
    lv_obj_align(spinner, LV_ALIGN_BOTTOM_MID, 0, 0);

- Create two threads to call :cpp:func:`lv_timer_handler`, and
  :cpp:func:`lv_tick_inc`.You will need two ``osThreadId`` handles for
  CMSISv1. These don't strictly have to be globally accessible in this
  case, however STM32Cube code generation does by default. If you are
  using CMSIS and STM32Cube code generation it should look something
  like this:

  .. code-block:: c

   //Thread Handles
   osThreadId lvgl_tickHandle;
   osThreadId lvgl_timerHandle;

   /* definition and creation of lvgl_tick */
   osThreadDef(lvgl_tick, LVGLTick, osPriorityNormal, 0, 1024);
   lvgl_tickHandle = osThreadCreate(osThread(lvgl_tick), NULL);

   //LVGL update timer
   osThreadDef(lvgl_timer, LVGLTimer, osPriorityNormal, 0, 1024);
   lvgl_timerHandle = osThreadCreate(osThread(lvgl_timer), NULL);

- And create the thread functions:

  .. code-block:: c

   /* LVGL timer for tasks. */
   void LVGLTimer(void const * argument)
   {
     for(;;)
     {
       lv_timer_handler();
       osDelay(20);
     }
   }
   /* LVGL tick source */
   void LVGLTick(void const * argument)
   {
     for(;;)
     {
       lv_tick_inc(10);
       osDelay(10);
     }
   }

- Finally, create the ``my_flush_cb`` function to output the frame
  buffer to your LCD. The specifics of this function will vary
  depending on which MCU features you are using. Below is a simple
  example of a parallel LCD interface, adjust this to suit your specific
  display and MCU capabilities.

  .. code-block:: c

   void my_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
   {
     uint16_t * color_p = (uint16_t *)px_map;

     //Set the drawing region
     set_draw_window(area->x1, area->y1, area->x2, area->y2);

     int height = area->y2 - area->y1 + 1;
     int width = area->x2 - area->x1 + 1;

     //Begin SPI Write for DATA
     HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
     HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

     //Write colour to each pixel
     for (int i = 0; i < width * height; i++) {
         parallel_write(color_p);
         color_p++;
     }

     //Return CS to high
     HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

     /* IMPORTANT!!!
      * Inform the graphics library that you are ready with the flushing */
     lv_display_flush_ready(display);
   }
