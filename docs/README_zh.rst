.. raw:: html

   <p align="left">
     <a href="https://github.com/sponsors/lvgl" target="_blank"><img align="left" src="https://lvgl.io/github-assets/sponsor.png" height="32px"></a>
   </p>

   <p align="right">
     <a href="../README.md">English</a>| <b>中文</b>| <a href="./README_pt_BR.rst">Português do Brasil</a> | <a href="./README_jp.rst">日本語</a>
   </p>

.. raw:: html

   <br>

.. raw:: html

   <p align="center">
     <img src="https://lvgl.io/github-assets/logo-colored.png" width=300px>
   </p>

.. raw:: html

   <h1 align="center">
      轻量级通用型图形库

.. raw:: html

   </h1>

.. raw:: html

   <div align="center">
     <img src="https://lvgl.io/github-assets/smartwatch-demo.gif">
     &nbsp;
     <img border="1px" src="https://lvgl.io/github-assets/widgets-demo.gif">
   </div>
   <br>
   <p align="center">
   <a href="https://lvgl.io" title="Homepage of LVGL">官网 </a> |
   <a href="https://docs.lvgl.io/" title="Detailed documentation with 100+ examples">文档</a> |
   <a href="https://forum.lvgl.io" title="Get help and help others">论坛</a> |
   <a href="https://lvgl.io/demos" title="Demos running in your browser">示例</a> |
   <a href="https://lvgl.io/services" title="Graphics design, UI implementation and consulting">服务</a>
   </p>
   <br>

.. _ledger-概况与总览:

📒 概况与总览
-------------

**成熟且知名**\

LVGL 是最流行的免费开源嵌入式图形库，可以为任何 MCU、MPU 和显示类型创建漂亮的 UI。它得到了行业领先供应商和项目的支持，如 Arm、STM32、NXP、Espressif、Nuvoton、Arduino、RT-Thread、Zephyr、NuttX、Adafruit 等。

**功能丰富**\

它拥有创建现代美观 GUI 的所有功能：30 多个内置控件、强大的样式系统、Web 启发的布局管理器和支持多种语言的排版系统。要将 LVGL 集成到您的平台中，您只需要至少 32 KB
RAM 和 128 KB Flash、C 编译器、帧缓冲区和至少 1/10 屏幕大小的渲染缓冲区。

**服务**\

我们的团队随时准备为您提供图形设计、UI 实现和咨询服务。如果您在开发下一个 GUI 项目时需要一些支持，请与我们联系。

.. _rocket-特性:

🚀 特性
-------

**免费和可移植性**

-  一个完全可移植的 C（C++ 兼容）库，没有外部依赖关系。
-  可以编译到任何 MCU 或 MPU，使用任何 RTOS 或者操作系统。
-  支持单色、ePaper、OLED、TFT 显示器或者模拟器。
   `移植指南 <https://docs.lvgl.io/master/intro/add-lvgl-to-your-project/connecting_lvgl.html>`__
-  该项目使用 MIT 许可证，因此您可以在商业项目中轻松使用它。
-  仅需 32 KB RAM 和 128 KB Flash，一个帧缓冲区，以及至少 1/10 屏幕大小的渲染缓冲区。
-  支持使用可选的操作系统、外部存储器和 GPU。

**控件、样式、布局等**

-  30+ 内置\ `控件 <https://docs.lvgl.io/master/details/widgets/index.html>`__:
    按钮、标签、滑块、图表、键盘、仪表、弧形、表格等等。
-  灵活的\ `样式系统 <https://docs.lvgl.io/master/details/base-widget/styles/style.html>`__
   支持约 100 个样式属性，可在任何状态下自定义控件的任何部分。
-  `Flex 布局 <https://docs.lvgl.io/master/details/base-widget/layouts/flex.html>`__ 和
   `Grid 布局 <https://docs.lvgl.io/master/details/base-widget/layouts/grid.html>`__
   可以响应式自动调整控件的大小和位置。
-  文本支持 UTF-8 编码，支持 CJK、泰语、印地语、阿拉伯语和波斯语书写系统。
-  支持自动换行、字距调整、文本滚动、亚像素渲染、拼音输入法、文本表情符号。
-  渲染引擎支持动画、抗锯齿、不透明度、平滑滚动、阴影、图形变换等。
-  支持鼠标、触摸板、小键盘、键盘、外部按钮、编码器\ `输入设备 <https://docs.lvgl.io/master/details/main-components/indev.html>`__\ 。
-  支持\ `多显示器 <https://docs.lvgl.io/master/details/main-components/display.html#multiple-display-support>`__\ 。

**绑定和构建支持**

-  `MicroPython 绑定 <https://blog.lvgl.io/2019-02-20/micropython-bindings>`__
   公开 LVGL的API
-  `PikaScript 绑定 <https://blog.lvgl.io/2022-08-24/pikascript-and-lvgl>`__
   在 MCU 上的更轻更简单的 Python 版本
-  未使用自定义生成系统。您可以在构建项目的其他文件时构建 LVGL。
-  支持开箱即用的 Make 和 \ `CMake <https://docs.lvgl.io/master/details/integration/building/cmake.html>`__\  编译系统。
-  支持在 \ `PC 上开发 <https://docs.lvgl.io/master/integration/ide/pc-simulator.html>`__\ ，并可以在嵌入式硬件上使用相同的 UI 代码。
-  支持使用我们的 \ `Emscripten 移植 <https://github.com/lvgl/lv_web_emscripten>`__\  从而将 C 写的 UI 代码转换为 HTML 文件。

**文档、工具和服务**

-  包含 \ `100 多个简单示例 <https://docs.lvgl.io/master/index.html>`__\ 的详细\ `文档 <https://docs.lvgl.io/>`__
-  `服务 <https://lvgl.io/services>`__
   如用户界面设计、实施和咨询，使 UI 开发更简单、更快。

.. _heart-赞助:

❤️ 赞助
-------

如果 LVGL 为您节省了大量时间和金钱，或者您只是在使用它时玩得很开心，请考虑\ `支持它的开发 <https://github.com/sponsors/lvgl>`__\ 。

**我们如何使用捐赠？**\

我们的目标是为 LVGL 做得最多的人提供经济补偿。这意味着不仅维护人员，而且任何实现伟大功能的人都应该从累积的资金中获得报酬。我们用捐款来支付服务器和相关服务等运营成本。

**如何捐赠？**\

我们使用 \ `GitHub Sponsors <https://github.com/sponsors/lvgl>`__\ ，您可以轻松发送一次性或定期捐款。您还可以以透明的方式查看我们的所有费用。

**如何从您的贡献中获取报酬？**\

如果有人实施或修复了一个标记为\ `赞助 <https://github.com/lvgl/lvgl/labels/Sponsored>`__\ 的问题，他或她将获得该工作的报酬。我们估计问题所需的时间、复杂性和重要性，并据此设定价格。直接评论一个赞助的问题，说“嗨，我想处理它。这就是我计划修复/实施它的方式…”。当维护人员批准并合并工作时，就认为它已经准备好了。之后，您可以在 \ `opencollective.com <https://opencollective.com/lvgl>`__\  上提交并支付费用，几天后您将收到付款。

**支持 LVGL 的组织**\

|Sponsors of LVGL|

**支持 LVGL 的个人**\

|Backers of LVGL|

.. _package-支持包:

📦 支持包
---------

LVGL 可用于以下几种：

-  `Arduino
   library <https://docs.lvgl.io/master/integration/framework/arduino.html>`__
-  `PlatformIO
   package <https://registry.platformio.org/libraries/lvgl/lvgl>`__
-  `Zephyr
   library <https://docs.lvgl.io/master/integration/os/zephyr.html>`__
-  `ESP-IDF(ESP32)
   component <https://components.espressif.com/components/lvgl/lvgl>`__
-  `NXP MCUXpresso
   component <https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY>`__
-  `NuttX
   library <https://docs.lvgl.io/master/integration/os/nuttx.html>`__
-  `RT-Thread
   RTOS <https://docs.lvgl.io/master/integration/os/rt-thread.html>`__
-  CMSIS-Pack
-  `RIOT OS
   package <https://doc.riot-os.org/group__pkg__lvgl.html#details>`__

.. _robot-示例:

🤖 示例
-------

请参阅创建控件、使用布局和应用样式的一些示例。您将找到 C 和 MicroPython 代码，以及在在线 MicroPython 编辑器中尝试或编辑示例的链接。

如果要查看更多示例，可查看 \ `Examples <https://github.com/lvgl/lvgl/tree/master/examples>`__  文件夹。

Hello world 标签
~~~~~~~~~~~~~~~~

.. image:: https://github.com/kisvegabor/test/raw/master/readme_example_1.png
   :alt: Simple Hello world label example in LVGL

.. raw:: html

   <details>
     <summary>C code</summary>

.. code-block:: c

   /* Change Active Screen's background color */
   lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

   /*Create a white label, set its text and align it to the center*/
   lv_obj_t * label = lv_label_create(lv_screen_active());
   lv_label_set_text(label, "Hello world");
   lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
   lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

.. raw:: html

   </details>

.. raw:: html

   <details>
     <summary>MicroPython code | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_direct=4ab7c40c35b0dc349aa2f0c3b00938d7d8e8ac9f" target="_blank">在线模拟器</a></summary>

.. code-block:: python

   # Change Active Screen's background color
   scr = lv.screen_active()
   scr.set_style_bg_color(lv.color_hex(0x003a57), lv.PART.MAIN)

   # Create a white label, set its text and align it to the center
   label = lv.label(lv.screen_active())
   label.set_text("Hello world")
   label.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN)
   label.align(lv.ALIGN.CENTER, 0, 0)

.. raw:: html

   </details>
   <br>

按钮与点击事件
~~~~~~~~~~~~~~

.. image:: https://github.com/kisvegabor/test/raw/master/readme_example_2.gif
   :alt: LVGL button with label example

.. raw:: html

   <details>
     <summary>C code</summary>

.. code-block:: c

   lv_obj_t * button = lv_button_create(lv_screen_active());                   /*Add a button to the current screen*/
   lv_obj_center(button);                                             /*Set its position*/
   lv_obj_set_size(button, 100, 50);                                  /*Set its size*/
   lv_obj_add_event_cb(button, button_event_cb, LV_EVENT_CLICKED, NULL); /*Assign a callback to the button*/

   lv_obj_t * label = lv_label_create(button);                        /*Add a label to the button*/
   lv_label_set_text(label, "Button");                             /*Set the labels text*/
   lv_obj_center(label);                                           /*Align the label to the center*/
   ...

   void button_event_cb(lv_event_t * e)
   {
     printf("Clicked\n");
   }

.. raw:: html

   </details>

.. raw:: html

   <details>
     <summary>MicroPython code | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=926bde43ec7af0146c486de470c53f11f167491e" target="_blank">在线模拟器</a></summary>

.. code-block:: python

   def button_event_cb(e):
     print("Clicked")

   # Create a Button and a Label
   button = lv.button(lv.screen_active())
   button.center()
   button.set_size(100, 50)
   button.add_event_cb(button_event_cb, lv.EVENT.CLICKED, None)

   label = lv.label(button)
   label.set_text("Button")
   label.center()

.. raw:: html

   </details>
   <br>

带布局的复选框
~~~~~~~~~~~~~~

.. image:: https://github.com/kisvegabor/test/raw/master/readme_example_3.gif
   :alt: Checkboxes with layout in LVGL

.. raw:: html

   <details>
     <summary>C code</summary>

.. code-block:: c


   lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);
   lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

   lv_obj_t * cb;
   cb = lv_checkbox_create(lv_screen_active());
   lv_checkbox_set_text(cb, "Apple");
   lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

   cb = lv_checkbox_create(lv_screen_active());
   lv_checkbox_set_text(cb, "Banana");
   lv_obj_add_state(cb, LV_STATE_CHECKED);
   lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

   cb = lv_checkbox_create(lv_screen_active());
   lv_checkbox_set_text(cb, "Lemon");
   lv_obj_add_state(cb, LV_STATE_DISABLED);
   lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

   cb = lv_checkbox_create(lv_screen_active());
   lv_obj_add_state(cb, LV_STATE_CHECKED | LV_STATE_DISABLED);
   lv_checkbox_set_text(cb, "Melon\nand a new line");
   lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

.. raw:: html

   </details>

.. raw:: html

   <details>
     <summary>MicroPython code | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=311d37e5f70daf1cb0d2cad24c7f72751b5f1792" target="_blank">在线模拟器</a></summary>

.. code-block:: python

   def event_handler(e):
       code = e.get_code()
       obj = e.get_target_obj()
       if code == lv.EVENT.VALUE_CHANGED:
           txt = obj.get_text()
           if obj.get_state() & lv.STATE.CHECKED:
               state = "Checked"
           else:
               state = "Unchecked"
           print(txt + ":" + state)


   lv.screen_active().set_flex_flow(lv.FLEX_FLOW.COLUMN)
   lv.screen_active().set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.START, lv.FLEX_ALIGN.CENTER)

   cb = lv.checkbox(lv.screen_active())
   cb.set_text("Apple")
   cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

   cb = lv.checkbox(lv.screen_active())
   cb.set_text("Banana")
   cb.add_state(lv.STATE.CHECKED)
   cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

   cb = lv.checkbox(lv.screen_active())
   cb.set_text("Lemon")
   cb.add_state(lv.STATE.DISABLED)
   cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

   cb = lv.checkbox(lv.screen_active())
   cb.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)
   cb.set_text("Melon")
   cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

.. raw:: html

   </details>
   <br>

设置滑块的样式
~~~~~~~~~~~~~~

.. image:: https://github.com/kisvegabor/test/raw/master/readme_example_4.gif
   :alt: Styling a slider with LVGL

.. raw:: html

   <details>
     <summary>C code</summary>

.. code-block:: c

   lv_obj_t * slider = lv_slider_create(lv_screen_active());
   lv_slider_set_value(slider, 70, LV_ANIM_OFF);
   lv_obj_set_size(slider, 300, 20);
   lv_obj_center(slider);

   /*Add local styles to MAIN part (background rectangle)*/
   lv_obj_set_style_bg_color(slider, lv_color_hex(0x0F1215), LV_PART_MAIN);
   lv_obj_set_style_bg_opa(slider, 255, LV_PART_MAIN);
   lv_obj_set_style_border_color(slider, lv_color_hex(0x333943), LV_PART_MAIN);
   lv_obj_set_style_border_width(slider, 5, LV_PART_MAIN);
   lv_obj_set_style_pad_all(slider, 5, LV_PART_MAIN);

   /*Create a reusable style sheet for the INDICATOR part*/
   static lv_style_t style_indicator;
   lv_style_init(&style_indicator);
   lv_style_set_bg_color(&style_indicator, lv_color_hex(0x37B9F5));
   lv_style_set_bg_grad_color(&style_indicator, lv_color_hex(0x1464F0));
   lv_style_set_bg_grad_dir(&style_indicator, LV_GRAD_DIR_HOR);
   lv_style_set_shadow_color(&style_indicator, lv_color_hex(0x37B9F5));
   lv_style_set_shadow_width(&style_indicator, 15);
   lv_style_set_shadow_spread(&style_indicator, 5);
   4
   /*Add the style sheet to the slider's INDICATOR part*/
   lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);

   /*Add the same style to the KNOB part as well and locally overwrite some properties*/
   lv_obj_add_style(slider, &style_indicator, LV_PART_KNOB);

   lv_obj_set_style_outline_color(slider, lv_color_hex(0x0096FF), LV_PART_KNOB);
   lv_obj_set_style_outline_width(slider, 3, LV_PART_KNOB);
   lv_obj_set_style_outline_pad(slider, -5, LV_PART_KNOB);
   lv_obj_set_style_shadow_spread(slider, 2, LV_PART_KNOB);

.. raw:: html

   </details>

.. raw:: html

   <details>
     <summary>MicroPython code |
   <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=c431c7b4dfd2cc0dd9c392b74365d5af6ea986f0" target="_blank">在线模拟器</a>
   </summary>

.. code-block:: python

   # Create a slider and add the style
   slider = lv.slider(lv.screen_active())
   slider.set_value(70, lv.ANIM.OFF)
   slider.set_size(300, 20)
   slider.center()

   # Add local styles to MAIN part (background rectangle)
   slider.set_style_bg_color(lv.color_hex(0x0F1215), lv.PART.MAIN)
   slider.set_style_bg_opa(255, lv.PART.MAIN)
   slider.set_style_border_color(lv.color_hex(0x333943), lv.PART.MAIN)
   slider.set_style_border_width(5, lv.PART.MAIN)
   slider.set_style_pad_all(5, lv.PART.MAIN)

   # Create a reusable style sheet for the INDICATOR part
   style_indicator = lv.style_t()
   style_indicator.init()
   style_indicator.set_bg_color(lv.color_hex(0x37B9F5))
   style_indicator.set_bg_grad_color(lv.color_hex(0x1464F0))
   style_indicator.set_bg_grad_dir(lv.GRAD_DIR.HOR)
   style_indicator.set_shadow_color(lv.color_hex(0x37B9F5))
   style_indicator.set_shadow_width(15)
   style_indicator.set_shadow_spread(5)

   # Add the style sheet to the slider's INDICATOR part
   slider.add_style(style_indicator, lv.PART.INDICATOR)
   slider.add_style(style_indicator, lv.PART.KNOB)

   # Add the same style to the KNOB part as well and locally overwrite some properties
   slider.set_style_outline_color(lv.color_hex(0x0096FF), lv.PART.KNOB)
   slider.set_style_outline_width(3, lv.PART.KNOB)
   slider.set_style_outline_pad(-5, lv.PART.KNOB)
   slider.set_style_shadow_spread(2, lv.PART.KNOB)

.. raw:: html

   </details>
   <br>

英语、希伯来语（ 双向文本排版 ）和中文
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. image:: https://github.com/kisvegabor/test/raw/master/readme_example_5.png
   :alt: English, Hebrew and Chinese texts with LVGL

.. raw:: html

   <details>
     <summary>C code</summary>

.. code-block:: c

   lv_obj_t * ltr_label = lv_label_create(lv_screen_active());
   lv_label_set_text(ltr_label, "In modern terminology, a microcontroller is similar to a system on a chip (SoC).");
   lv_obj_set_style_text_font(ltr_label, &lv_font_montserrat_16, 0);
   lv_obj_set_width(ltr_label, 310);
   lv_obj_align(ltr_label, LV_ALIGN_TOP_LEFT, 5, 5);

   lv_obj_t * rtl_label = lv_label_create(lv_screen_active());
   lv_label_set_text(rtl_label,"מעבד, או בשמו המלא יחידת עיבוד מרכזית (באנגלית: CPU - Central Processing Unit).");
   lv_obj_set_style_base_dir(rtl_label, LV_BASE_DIR_RTL, 0);
   lv_obj_set_style_text_font(rtl_label, &lv_font_dejavu_16_persian_hebrew, 0);
   lv_obj_set_width(rtl_label, 310);
   lv_obj_align(rtl_label, LV_ALIGN_LEFT_MID, 5, 0);

   lv_obj_t * cz_label = lv_label_create(lv_screen_active());
   lv_label_set_text(cz_label,
                     "嵌入式系统（Embedded System），\n是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。");
   lv_obj_set_style_text_font(cz_label, &lv_font_simsun_16_cjk, 0);
   lv_obj_set_width(cz_label, 310);
   lv_obj_align(cz_label, LV_ALIGN_BOTTOM_LEFT, 5, -5);

.. raw:: html

   </details>

.. raw:: html

   <details>
     <summary>MicroPython code | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=18bb38200a64e10ead1aa17a65c977fc18131842" target="_blank">在线模拟器</a></summary>

.. code-block:: python

   ltr_label = lv.label(lv.screen_active())
   ltr_label.set_text("In modern terminology, a microcontroller is similar to a system on a chip (SoC).")
   ltr_label.set_style_text_font(lv.font_montserrat_16, 0);

   ltr_label.set_width(310)
   ltr_label.align(lv.ALIGN.TOP_LEFT, 5, 5)

   rtl_label = lv.label(lv.screen_active())
   rtl_label.set_text("מעבד, או בשמו המלא יחידת עיבוד מרכזית (באנגלית: CPU - Central Processing Unit).")
   rtl_label.set_style_base_dir(lv.BASE_DIR.RTL, 0)
   rtl_label.set_style_text_font(lv.font_dejavu_16_persian_hebrew, 0)
   rtl_label.set_width(310)
   rtl_label.align(lv.ALIGN.LEFT_MID, 5, 0)

   font_simsun_16_cjk = lv.font_load("S:../../assets/font/lv_font_simsun_16_cjk.fnt")

   cz_label = lv.label(lv.screen_active())
   cz_label.set_style_text_font(font_simsun_16_cjk, 0)
   cz_label.set_text("嵌入式系统（Embedded System），\n是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。")
   cz_label.set_width(310)
   cz_label.align(lv.ALIGN.BOTTOM_LEFT, 5, -5)

.. raw:: html

   </details>

.. _arrow_forward-使用 lvgl 过程:

▶️ 使用 LVGL 过程
---------------

此列表将指导您逐步开始使用 LVGL。

**熟悉 LVGL**

1. 查看\ `在线演示 <https://lvgl.io/demos>`__\ ，了解 LVGL 的实际操作（3 分钟）
2. 阅读\ `文档 <https://docs.lvgl.io/master/intro/index.html>`__\ 的简介页（5 分钟）
3. 熟悉\ `快速概览 <https://docs.lvgl.io/master/intro/basics.html>`__
   页面上的基本知识（15 分钟）

**开始使用 LVGL**

4. 设置\ `模拟器 <https://docs.lvgl.io/master/integration/ide/pc-simulator.html#simulator>`__  （10 分钟）
5. 尝试一些\ `示例 <https://github.com/lvgl/lvgl/tree/master/examples>`__
6. 将LVGL端口连接到线路板。请参阅\ `移植 <https://docs.lvgl.io/master/intro/add-lvgl-to-your-project/index.html>`__\ 指南，或查看现成的\ `项目 <https://github.com/lvgl?q=lv_port_>`__

**成为专业人士**

7. 阅读\ `概述 <https://docs.lvgl.io/master/details/main-components/index.html>`__\ 页面以更好地了解图书馆（2-3 小时）
8. 查看\ `控件 <https://docs.lvgl.io/master/details/widgets/index.html>`__\ 的文档以查看其功能和用法

**获得帮助并帮助他人**

9.  如果您有问题，请访问\ `论坛 <http://forum.lvgl.io/>`__
10. 阅读\ `贡献 <https://docs.lvgl.io/master/CONTRIBUTING.html>`__\ 指南，了解如何帮助提高 LVGL（15 分钟）

.. _handshake-服务:

🤝 服务
-------

LVGL LLC 的成立旨在为 LVGL 库提供坚实的背景，并提供多种类型的服务来帮助您进行 UI 开发。凭借在用户界面和图形行业超过15年的经验，我们可以帮助您将 UI 提升到一个新的水平。

-  **平面设计**
   我们的内部图形设计师是创造美丽现代设计的专家，适合您的产品和硬件资源。
-  **UI 实现**
   我们还可以根据您或我们创建的设计来实现您的 UI。您可以确信，我们将充分利用您的硬件和 LVGL。如果 LVGL 中缺少某个功能或控件，请不要担心，我们会为您实现它。
-  **咨询和支持**
   我们也可以通过咨询来支持您，以避免在 UI 开发过程中出现昂贵和耗时的错误。
-  **板子认证**
   对于提供开发板或生产套件的公司，我们会进行板子认证，展示板如何运行 LVGL。

查看我们的 \ `Demos <https://lvgl.io/demos>`__\  作为参考。有关更多信息，请查看\ `服务页面 <https://lvgl.io/services>`__\ 。

`联系我们 <https://lvgl.io/#contact>`__\ ，告诉我们如何提供帮助。

.. _star2-贡献:

🌟 贡献
-------

LVGL 是一个开放的项目，我们非常欢迎您的贡献。有很多方法可以帮助您，从简单地谈论您的项目，到编写示例、改进文档、修复错误，甚至在 LVGL 组织下托管您自己的项目。

有关贡献的详细说明，请访问文件的\ `贡献 <https://docs.lvgl.io/master/CONTRIBUTING.html>`__\ 部分。

已经有 300 多人在 LVGL 留下了痕迹。期待你成为他们中的一员！并在下列贡献者中看到你！ 🙂

.. raw:: html

   <a href="https://github.com/lvgl/lvgl/graphs/contributors">
     <img src="https://contrib.rocks/image?repo=lvgl/lvgl&max=48" />
   </a>

... 等等其他人.

.. |Sponsors of LVGL| image:: https://opencollective.com/lvgl/organizations.svg?width=600
   :target: https://opencollective.com/lvgl
.. |Backers of LVGL| image:: https://opencollective.com/lvgl/individuals.svg?width=600
   :target: https://opencollective.com/lvgl
