.. raw:: html

   <p align="left">
     <a href="https://github.com/sponsors/lvgl" target="_blank"><img align="left" src="https://lvgl.io/github-assets//sponsor.png" height="32px"></a>
   </p>

   <p align="right">
     <a href="../README.md">English</a>| <a href="./README_zh.rst">中文</a>| <a href="./README_pt_BR.rst">Português do Brasil</a> | <b>日本語</b>
   </p>

.. raw:: html

   </p>

.. raw:: html

   <p align="center">

.. raw:: html

   </p>

.. raw:: html

   <h1 align="center">

Light and Versatile Graphics Library

.. raw:: html

   </h1>

.. raw:: html

   <p align="center">

Website \| Docs \| Forum :gb: \| Demos \| Services \| SquareLine Studio
:gb:

.. raw:: html

   </p>

:ledger:
Overview
--------

**実績**\  LVGL
は、フリー＆オープンソースの組み込み用グラフィックスライブラリです。
あらゆるMCU、MPU、ディスプレイタイプに対応しており、美しいUI(User
Interface)を実現できます。 ARM, STM32, NXP, Espressif, Nuvoton, Arduino,
RT-Thread, Zephyr, NuttX,
Adafruitなど、業界をリードするベンダーやプロジェクトによりサポートされています。

**機能豊富**\
モダンで美しいGUIを作成するための機能をすべて備えています。
30以上の組み込みウィジェット、強力なスタイルシステム、WEB由来のレイアウトマネージャ、多くの言語をサポートする文字グラフィックシステムなどです。
LVGL のシステム要件は、RAM 32KB、Flash
128KB、Cコンパイラ、フレームバッファ、1/10スクリーンサイズのレンダリング用バッファです。

**UIエディタ**\  SquareLine Studio
は、LVGL用のプロフェッショナル＆リーズナブルなドラッグ＆ドロップ型のUIエディターです。
Windows、Linux、macOS
で動作し、ウェブサイトへの登録なしで試すことができます。

**サービス**\  LVGL LLC
では、グラフィックデザイン、UI実装、コンサルティングサービスに対する技術サポートが可能です。GUIプロジェクトの開発において何らかのサポートが必要な場合には、お気軽にお問い合わせください。

:rocket:
特徴
-------------

**フリー & 移植可能** - 外部依存関係がなく、完全に移植可能な
Cライブラリ。(C++互換) -
任意の(RT)OS、任意のMCU・MPU用にコンパイル可能。 -
電子ペーパー、OLEDディスプレイ、TFTディスプレイ、白黒ディスプレイ、モニターに対応。
`Porting
Guide <https://docs-lvgl-io.translate.goog/master/porting/project.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
- MITライセンスにより商用利用可能。 - システム要件：RAM 32KB、Flash
128KB、フレームバッファ、レンダリング用に1/10以上のスクリーンサイズのバッファ。
- OS、外部メモリ、GPUもサポート。

**ウィジェット、スタイル、レイアウトなど** - 30以上の組み込み
`ウィジェット <https://docs-lvgl-io.translate.goog/master/widgets/index.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__: ボタン、ラベル、スライダー、グラフ、キーボード、メーター、円弧、表など。
-
ウィジェットの任意の部分を任意の状態にカスタマイズ可能な豊富なスタイルプロパティを備えた柔軟な
`スタイルシステム <https://docs-lvgl-io.translate.goog/master/overview/style.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__\ 。
-
`Flexbox <https://docs-lvgl-io.translate.goog/master/layouts/flex.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
および
`グリッド <https://docs-lvgl-io.translate.goog/master/layouts/grid.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
風のレイアウトエンジンにより、ウィジェットのサイズと位置を自動的に設定。
-
テキスト表示(UTF-8対応)は、中国語、日本語、韓国語、タイ語、ヒンディー語、アラビア語、ペルシア語をサポート。
-
ワードラッピング、カーニング、テキストスクロール、サブピクセルレンダリング、ピンイン-IME中国語入力、テキスト中の絵文字に対応。
-
アニメーション、アンチエイリアシング、不透明度、スムーズスクロール、シャドウ、画像変換などをサポートするレンダリングエンジン。
-
マウス、タッチパッド、キーパッド、キーボード、外部ボタン、エンコーダ等の
`入力デバイス <https://docs-lvgl-io.translate.goog/master/porting/indev.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
をサポート。 -
`マルチディスプレイ <https://docs-lvgl-io.translate.goog/master/overview/display.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
対応。

**Binding と Build をサポート** - `MicroPython
Binding <https://blog-lvgl-io.translate.goog/2019-02-20/micropython-bindings?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
が LVGL API を公開。 -
カスタムビルドシステムは使用せず、プロジェクトの他のファイルをビルドするときに、LVGLをビルド可能。
- Make と
`CMake <https://docs-lvgl-io.translate.goog/master/get-started/platforms/cmake.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
が含まれており、すぐ使えるようにサポート。 -
`PCのシミュレータで開発したUIコード <https://docs-lvgl-io.translate.goog/master/get-started/platforms/pc-simulator.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
は、そのまま組込み用ハードウェアでも使用可能。 - `Emscripten
port <https://github.com/lvgl/lv_web_emscripten>`__ :gb:
によりC言語のUIコードをHTMLファイルに変換。

**ドキュメント, ツール, 技術サービス** -
`ドキュメント <https://docs-lvgl-io.translate.goog/master/index.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__\ は\ `100以上の簡単なサンプルプログラム <https://github.com/lvgl/lvgl/tree/master/examples>`__
:gb: 入り 。 - `SquareLine Studio <https://squareline.io/>`__ :gb: -
UI開発をスピードアップおよび簡素化するためのプロフェッショナルで使いやすいUIエディターソフトウェア。
-
UI開発をよりシンプルかつ迅速にするための、ユーザーインターフェイスの設計、実装、コンサルティングなどの
`技術サービス <https://lvgl-io.translate.goog/services?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__\ 。

:package:
パッケージ
--------------------

LVGL は以下で利用可能です。 - `Arduino
library <https://docs-lvgl-io.translate.goog/master/get-started/platforms/arduino.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
- `PlatformIO
package <https://registry.platformio.org/libraries/lvgl/lvgl>`__ :gb: -
`Zephyr
library <https://docs-zephyrproject-org.translate.goog/latest/index.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
- `ESP32
component <https://docs-lvgl-io.translate.goog/master/get-started/platforms/espressif.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
- `NXP MCUXpresso
component <https://www-nxp-com.translate.goog/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
- `NuttX
library <https://docs-lvgl-io.translate.goog/master/get-started/os/nuttx.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
- `RT-Thread
RTOS <https://docs-lvgl-io.translate.goog/master/get-started/os/rt-thread.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
- NXP MCUXpresso library - CMSIS-Pack

:robot:
サンプルプログラム
--------------------------

ウィジェット・レイアウト・スタイルのサンプルプログラムを用意しました。 C
と MicroPython のコードを選べます。 オンラインの MicroPythonエディタ
へのリンクにより、サンプルプログラムの動作確認・編集もできます。

その他のサンプルプログラムは `Examples
フォルダ <https://github.com/lvgl/lvgl/tree/master/examples>`__ :gb:
を確認してください。

Button with Click Event
~~~~~~~~~~~~~~~~~~~~~~~

.. figure:: https://github.com/kisvegabor/test/raw/master/readme_example_1.gif
   :alt: ラベル付きボタンのLVGLサンプルプログラム

   ラベル付きボタンのLVGLサンプルプログラム

.. raw:: html

   <details>

.. raw:: html

   <summary>

C code

.. raw:: html

   </summary>

.. code:: c

   lv_obj_t * btn = lv_button_create(lv_screen_active());                   /*Add a button to the current screen*/
   lv_obj_center(btn);                                             /*Set its position*/
   lv_obj_set_size(btn, 100, 50);                                  /*Set its size*/
   lv_obj_add_event(btn, btn_event_cb, LV_EVENT_CLICKED, NULL); /*Assign a callback to the button*/

   lv_obj_t * label = lv_label_create(btn);                        /*Add a label to the button*/
   lv_label_set_text(label, "Button");                             /*Set the labels text*/
   lv_obj_center(label);                                           /*Align the label to the center*/
   ...

   void btn_event_cb(lv_event_t * e)
   {
     printf("Clicked\n");
   }

.. raw:: html

   </details>

.. raw:: html

   <details>

.. raw:: html

   <summary>

MicroPython code \| Online Simulator :gb:

.. raw:: html

   </summary>

.. code:: python

   def btn_event_cb(e):
     print("Clicked")

   # Create a Button and a Label
   btn = lv.btn(lv.scr_act())
   btn.center()
   btn.set_size(100, 50)
   btn.add_event(btn_event_cb, lv.EVENT.CLICKED, None)

   label = lv.label(btn)
   label.set_text("Button")
   label.center()

.. raw:: html

   </details>

Checkboxes with Layout
~~~~~~~~~~~~~~~~~~~~~~

.. figure:: https://github.com/kisvegabor/test/raw/master/readme_example_2.gif
   :alt: Checkboxes with layout in LVGL

   Checkboxes with layout in LVGL

.. raw:: html

   <details>

.. raw:: html

   <summary>

C code

.. raw:: html

   </summary>

.. code:: c


   lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);
   lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

   lv_obj_t * cb;
   cb = lv_checkbox_create(lv_screen_active());
   lv_checkbox_set_text(cb, "Apple");
   lv_obj_add_event(cb, event_handler, LV_EVENT_ALL, NULL);

   cb = lv_checkbox_create(lv_screen_active());
   lv_checkbox_set_text(cb, "Banana");
   lv_obj_add_state(cb, LV_STATE_CHECKED);
   lv_obj_add_event(cb, event_handler, LV_EVENT_ALL, NULL);

   cb = lv_checkbox_create(lv_screen_active());
   lv_checkbox_set_text(cb, "Lemon");
   lv_obj_add_state(cb, LV_STATE_DISABLED);
   lv_obj_add_event(cb, event_handler, LV_EVENT_ALL, NULL);

   cb = lv_checkbox_create(lv_screen_active());
   lv_obj_add_state(cb, LV_STATE_CHECKED | LV_STATE_DISABLED);
   lv_checkbox_set_text(cb, "Melon\nand a new line");
   lv_obj_add_event(cb, event_handler, LV_EVENT_ALL, NULL);

.. raw:: html

   </details>

.. raw:: html

   <details>

.. raw:: html

   <summary>

MicroPython code \| Online Simulator :gb:

.. raw:: html

   </summary>

.. code:: python

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


   lv.scr_act().set_flex_flow(lv.FLEX_FLOW.COLUMN)
   lv.scr_act().set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.START, lv.FLEX_ALIGN.CENTER)

   cb = lv.checkbox(lv.scr_act())
   cb.set_text("Apple")
   cb.add_event(event_handler, lv.EVENT.ALL, None)

   cb = lv.checkbox(lv.scr_act())
   cb.set_text("Banana")
   cb.add_state(lv.STATE.CHECKED)
   cb.add_event(event_handler, lv.EVENT.ALL, None)

   cb = lv.checkbox(lv.scr_act())
   cb.set_text("Lemon")
   cb.add_state(lv.STATE.DISABLED)
   cb.add_event(event_handler, lv.EVENT.ALL, None)

   cb = lv.checkbox(lv.scr_act())
   cb.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)
   cb.set_text("Melon")
   cb.add_event(event_handler, lv.EVENT.ALL, None)

.. raw:: html

   </details>

Styling a Slider
~~~~~~~~~~~~~~~~

.. figure:: https://github.com/kisvegabor/test/raw/master/readme_example_3.gif
   :alt: Styling a slider with LVGL

   Styling a slider with LVGL

.. raw:: html

   <details>

.. raw:: html

   <summary>

C code

.. raw:: html

   </summary>

.. code:: c

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

   /*Add the style sheet to the slider's INDICATOR part*/
   lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);

   /*Add the same style to the KNOB part too and locally overwrite some properties*/
   lv_obj_add_style(slider, &style_indicator, LV_PART_KNOB);

   lv_obj_set_style_outline_color(slider, lv_color_hex(0x0096FF), LV_PART_KNOB);
   lv_obj_set_style_outline_width(slider, 3, LV_PART_KNOB);
   lv_obj_set_style_outline_pad(slider, -5, LV_PART_KNOB);
   lv_obj_set_style_shadow_spread(slider, 2, LV_PART_KNOB);

.. raw:: html

   </details>

.. raw:: html

   <details>

.. raw:: html

   <summary>

MicroPython code \| Online Simulator :gb:

.. raw:: html

   </summary>

.. code:: python

   # Create a slider and add the style
   slider = lv.slider(lv.scr_act())
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

   # Add the same style to the KNOB part too and locally overwrite some properties
   slider.set_style_outline_color(lv.color_hex(0x0096FF), lv.PART.KNOB)
   slider.set_style_outline_width(3, lv.PART.KNOB)
   slider.set_style_outline_pad(-5, lv.PART.KNOB)
   slider.set_style_shadow_spread(2, lv.PART.KNOB)

.. raw:: html

   </details>

English, Hebrew (mixed LRT-RTL) and Chinese texts
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. figure:: https://github.com/kisvegabor/test/raw/master/readme_example_4.png
   :alt: English, Hebrew and Chinese texts with LVGL

   English, Hebrew and Chinese texts with LVGL

.. raw:: html

   <details>

.. raw:: html

   <summary>

C code

.. raw:: html

   </summary>

.. code:: c

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

.. raw:: html

   <summary>

MicroPython code \| Online Simulator :gb:

.. raw:: html

   </summary>

.. code:: python

   ltr_label = lv.label(lv.scr_act())
   ltr_label.set_text("In modern terminology, a microcontroller is similar to a system on a chip (SoC).")
   ltr_label.set_style_text_font(lv.font_montserrat_16, 0);

   ltr_label.set_width(310)
   ltr_label.align(lv.ALIGN.TOP_LEFT, 5, 5)

   rtl_label = lv.label(lv.scr_act())
   rtl_label.set_text("מעבד, או בשמו המלא יחידת עיבוד מרכזית (באנגלית: CPU - Central Processing Unit).")
   rtl_label.set_style_base_dir(lv.BASE_DIR.RTL, 0)
   rtl_label.set_style_text_font(lv.font_dejavu_16_persian_hebrew, 0)
   rtl_label.set_width(310)
   rtl_label.align(lv.ALIGN.LEFT_MID, 5, 0)

   font_simsun_16_cjk = lv.font_load("S:../../assets/font/lv_font_simsun_16_cjk.fnt")

   cz_label = lv.label(lv.scr_act())
   cz_label.set_style_text_font(font_simsun_16_cjk, 0)
   cz_label.set_text("嵌入式系统（Embedded System），\n是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。")
   cz_label.set_width(310)
   cz_label.align(lv.ALIGN.BOTTOM_LEFT, 5, -5)

.. raw:: html

   </details>

:arrow_forward:
はじめに
------------------------

LVGLを使い始める時は、以下の順に進める事をおすすめします。

**LVGLに触れてみましょう**

1. LVGLの動きを
   `オンラインデモ <https://lvgl-io.translate.goog/demos?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
   で確認しましょう。 (3分間)
2. ドキュメントの
   `Introduction <https://docs-lvgl-io.translate.goog/master/intro/index.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
   を読みましょう。 (5分間)
3. LVGLの基本に慣れるため `Quick
   overview <https://docs-lvgl-io.translate.goog/master/get-started/quick-overview.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
   を読みましょう。 (15分間)

**LVGLを使ってみましょう**

4. `シミュレータ <https://docs-lvgl-io.translate.goog/master/get-started/platforms/pc-simulator.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
   をセットアップしましょう。 (10 minutes)
5. `サンプルプログラム <https://github.com/lvgl/lvgl/tree/master/examples>`__
   :gb: を動かしてみましょう。
6. `移植ガイド <https://docs-lvgl-io.translate.goog/master/porting/index.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
   を参考に、LVGLを開発ボードに移植してみましょう。すぐ使える形の
   `プロジェクト <https://github.com/lvgl?q=lv_port_>`__ :gb:
   も用意してあります。

**より詳しく体験してみましょう**

7. ライブラリの理解を深めるため
   `Overview <https://docs-lvgl-io.translate.goog/master/overview/index.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
   を読みましょう。 (2～3時間)
8. ウィジェットの機能や使い方の詳細は
   `Widgets <https://docs-lvgl-io.translate.goog/master/widgets/index.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
   でご確認ください。

**助け合いましょう**

9.  質問がある場合は `Forum <http://forum.lvgl.io/>`__ :gb:
    で質問して下さい。
10. LVGLの改善への協力は大歓迎です。詳細は `Contributing
    guide <https://docs-lvgl-io.translate.goog/master/CONTRIBUTING.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
    をご覧ください。 (15分間)

**さらに理解を深めましょう**

11. `SquareLine Studio <https://squareline.io/>`__ :gb:
    をダウンロードして試用してみましょう。
12. 技術的サポートが必要であれば、\ `技術サービス <https://lvgl.io/services>`__
    :gb: に問い合わせて下さい。

:handshake:
技術サービス
------------------------

`LVGL
LLC <https://www.digikey.com/en/design-services-providers/lvgl-kft>`__
は、LVGLライブラリの確かな背景を元に、UI開発のための様々な技術サービスを提供するために設立されました。
UIとグラフィックス業界における15年以上の実績を活かし、UIを次のレベルに引き上げるお手伝いを致します。

-  **グラフィックデザイン**
   当社のグラフィックデザイナーは、製品とハードウェアのリソースに合わせて美しくモダンなデザインにするエキスパートです。
-  **UI実装**
   お客様または弊社で作成したデザインを元に、UIを実装することも可能です。お客様のハードウェアとLVGLを最大限に活用することをお約束します。
   LVGLにない機能やウィジェットは、私たちが実装しますのでご安心ください。
-  **コンサルタント＆技術サポート**
   UI開発において、価格と時間を要する作業でのリスクを減らすため、コンサルティングも含めてサポート致します。
-  **Board certification** development board または production ready kit
   を提供している企業に対しては、ボードがLVGLを実行できるようにするためのボード認定を行います。

サンプルは
`Demos <https://lvgl-io.translate.goog/demos?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
をご覧ください。 詳しくは `Services
page <https://lvgl-io.translate.goog/services?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
をご覧ください。

お問い合わせは `問い合わせフォーム <https://lvgl.io/#contact>`__ :gb:
より送信して下さい。

:star2:
協力
------------

LVGLはオープンプロジェクトであり、協力は大歓迎です。
色々な方法で協力できます。 協力方法の例 -
LVGLを使用した作品やプロジェクトの公表 - サンプルプログラムの作成 -
ドキュメントの改善 - バグの修正

協力方法の詳細については、ドキュメントの `Contributing
section <https://docs-lvgl-io.translate.goog/master/CONTRIBUTING.html?_x_tr_sl=en&_x_tr_tl=ja&_x_tr_hl=ja>`__
をご覧ください。

すでに 300人以上がLVGLに足跡を残しています。いっしょに活動しましょう!
:slightly_smiling_face:

… and many other.
