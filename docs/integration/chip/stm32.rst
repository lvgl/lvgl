=====
STM32
=====

LVGL Can be added to `STM32CubeIDE <https://www.st.com/en/development-tools/stm32cubeide.html>`__
in a similar fashion to any other Eclipse-based IDE.

Including LVGL in a Project
---------------------------

- Create or open a project in STM32CubeIDE.
- Copy the entire LVGL folder to *[project_folder]/Drivers/lvgl*.
- In the STM32CubeIDE **Project Explorer** pane: right click on the
  LVGL folder that you copied (you may need to refresh the view first
  before it will appear), and select **Add/remove include path…**. If
  this doesn't appear, or doesn't work, you can review your project
  include paths under the **Project** -> **Properties** menu, and then
  navigating to **C/C++ Build** -> **Settings** -> **Include paths**, and
  ensuring that the LVGL directory is listed.

Now that the source files are included in your project, follow the
instructions for `Porting <https://docs.lvgl.io/master/porting/project.html>`__ your
project to create the ``lv_conf.h`` file, and initialise the display.

Bare Metal Example
------------------

A minimal example using STM32CubeIDE, and HAL. \* When setting up
**Pinout and Configuration** using the **Device Configuration Tool**,
select **System Core** -> **SYS** and ensure that **Timebase Source** is
set to **SysTick**. \* Configure any other peripherals (including the
LCD panel), and initialise them in *main.c*. \* ``#include "lvgl.h"`` in
the *main.c* file. \* Create some frame buffer(s) as global variables:

.. code:: c

   //Frame buffers
   /*Static or global buffer(s). The second buffer is optional*/
   static lv_color_t buf_1[BUFF_SIZE]; //TODO: Chose a buffer size. DISPLAY_WIDTH * 10 is one suggestion.
   static lv_color_t buf_2[BUFF_SIZE];

- In your ``main()`` function, after initialising your CPU,
  peripherals, and LCD panel, call :cpp:func:`lv_init` to initialise LVGL.
  You can then create the display driver using
  :cpp:func:`lv_disp_create`, and register the frame buffers using
  :cpp:func:`lv_display_set_buffers`.

.. code:: c

   //Initialise LVGL UI library
   lv_init();

   lv_disp_t * disp = lv_disp_create(WIDTH, HEIGHT); /*Basic initialization with horizontal and vertical resolution in pixels*/
   lv_display_set_flush_cb(disp, my_flush_cb); /*Set a flush callback to draw to the display*/
   lv_display_set_buffers(disp, buf_1, buf_2, sizeof(buf_1), LV_DISP_RENDER_MODE_PARTIAL); /*Set an initialized buffer*/

- Create some dummy objects to test the output:

.. code:: c

   // Change the active screen's background color
   lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);
   lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);

   /*Create a spinner*/
   lv_obj_t * spinner = lv_spinner_create(lv_screen_active(), 1000, 60);
   lv_obj_set_size(spinner, 64, 64);
   lv_obj_align(spinner, LV_ALIGN_BOTTOM_MID, 0, 0);

- Add a call to :cpp:func:`lv_timer_handler` inside your ``while(1)`` loop:

.. code:: c

   /* Infinite loop */
   while (1)
   {
     lv_timer_handler();
     HAL_Delay(5);
   }

- Add a call to :cpp:func:`lv_tick_inc` inside the :cpp:func:`SysTick_Handler`
  function. Open the *stm32xxxx_it.c* file (the name will depend on
  your specific MCU), and update the :cpp:func:`SysTick_Handler` function:

.. code:: c

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

- Finally, write the callback function, ``my_flush_cb``, which will
  send the display buffer to your LCD panel. Below is one example, but
  it will vary depending on your setup.

.. code:: c

   void my_flush_cb(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p)
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
     * Inform the graphics library that you are ready with the flushing*/
     lv_disp_flush_ready(disp);
   }

FreeRTOS Example
----------------

A minimal example using STM32CubeIDE, HAL, and CMSISv1 (FreeRTOS). *Note
that we have not used Mutexes in this example, however LVGL is* **NOT**
*thread safe and so Mutexes should be used*. See: :ref:`os_interrupt`
\* ``#include "lvgl.h"`` \* Create your frame buffer(s) as global
variables:

.. code:: c

   //Frame buffers
   /*A static or global variable to store the buffers*/
   static lv_disp_draw_buf_t disp_buf;

   /*Static or global buffer(s). The second buffer is optional*/
   static lv_color_t buf_1[BUFF_SIZE]; //TODO: Declare your own BUFF_SIZE appropriate to your system.
   static lv_color_t buf_2[BUFF_SIZE];

- In your ``main`` function, after your peripherals (SPI, GPIOs, LCD
  etc) have been initialised, initialise LVGL using :cpp:func:`lv_init`,
  register the frame buffers using :cpp:func:`lv_disp_draw_buf_init`, and
  create a new display driver using :cpp:func:`lv_disp_drv_init`.

.. code:: c

   //Initialise LVGL UI library
   lv_init();
   lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, BUFF_SIZE);

   static lv_disp_drv_t disp_drv;          /*A variable to hold the drivers. Must be static or global.*/
   lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
   disp_drv.draw_buf = &disp_buf;          /*Set an initialized buffer*/
   disp_drv.flush_cb = my_flush_cb;        /*Set a flush callback to draw to the display*/
   disp_drv.hor_res = WIDTH;                 /*Set the horizontal resolution in pixels*/
   disp_drv.ver_res = HEIGHT;                 /*Set the vertical resolution in pixels*/

   lv_disp_t * disp;
   disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/

   // Register the touch controller with LVGL - Not included here for brevity.

- Create some dummy objects to test the output:

.. code:: c

   // Change the active screen's background color
   lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);
   lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);

   /*Create a spinner*/
   lv_obj_t * spinner = lv_spinner_create(lv_screen_active(), 1000, 60);
   lv_obj_set_size(spinner, 64, 64);
   lv_obj_align(spinner, LV_ALIGN_BOTTOM_MID, 0, 0);

- Create two threads to call :cpp:func:`lv_timer_handler`, and
  :cpp:func:`lv_tick_inc`.You will need two ``osThreadId`` handles for
  CMSISv1. These don't strictly have to be globally accessible in this
  case, however STM32Cube code generation does by default. If you are
  using CMSIS and STM32Cube code generation it should look something
  like this:

.. code:: c

   //Thread Handles
   osThreadId lvgl_tickHandle;
   osThreadId lvgl_timerHandle;

   /* definition and creation of lvgl_tick */
   osThreadDef(lvgl_tick, LGVLTick, osPriorityNormal, 0, 1024);
   lvgl_tickHandle = osThreadCreate(osThread(lvgl_tick), NULL);

   //LVGL update timer
   osThreadDef(lvgl_timer, LVGLTimer, osPriorityNormal, 0, 1024);
   lvgl_timerHandle = osThreadCreate(osThread(lvgl_timer), NULL);

- And create the thread functions:

.. code:: c

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
  depending on which MCU features you are using. Below is an example
  for a typical MCU interface.

.. code:: c

   void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
   {
     //Set the drawing region
     set_draw_window(area->x1, area->y1, area->x2, area->y2);

     int height = area->y2 - area->y1 + 1;
     int width = area->x2 - area->x1 + 1;

     //Begin SPI Write for DATA
     HAL_GPIO_WritePin(DC_PORT, DC_PIN, GPIO_PIN_SET);
     HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

     //Write colour to each pixel
     for (int i = 0; i < width * height; i++) {
         parallel_write(color_p->full);
         color_p++;
     }

     //Return CS to high
     HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

     /* IMPORTANT!!!
      * Inform the graphics library that you are ready with the flushing*/
     lv_disp_flush_ready(disp_drv);
   }
