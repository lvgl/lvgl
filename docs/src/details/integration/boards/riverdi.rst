=======
Riverdi
=======

`Riverdi <https://riverdi.com/>`__ specializes in making high quality
displays and the boards that carry them. They offer solutions that
range from simple display panels to intelligent displays, and
everything in between.

- STM32 Embedded Displays
- Single Board Computer Displays
- EVE Intelligent Displays
- HDMI Displays
- Evaluation Boards
- RGB, LVDS, MIPI DSI LCD Displays


STM32 Embedded Displays
***********************

The STM32 Embedded Displays have an STM32 MCU onboard which makes
it possible to embed an entire application into the device. LVGL
is well-suited to these boards.

lv_port_riverdi_stm32u5
-----------------------

`lv_port_riverdi_stm32u5 <https://github.com/lvgl/lv_port_riverdi_stm32u5>`_
is a port repo of LVGL which supports all 5-inch Riverdi STM32
Embedded Displays.

- `RVT50HQSNWC00-B <https://riverdi.com/product/5-inch-lcd-display-capacitive-touch-panel-optical-bonding-uxtouch-stm32u5-rvt50hqsnwc00-b>`_
- `RVT50HQSNWC00 <https://riverdi.com/product/5-inch-lcd-display-capacitive-touch-panel-air-bonding-uxtouch-stm32u5-rvt50hqsnwc00>`_
- `RVT50HQSFWCA0 <https://riverdi.com/product/5-inch-lcd-display-capacitive-touch-panel-air-bonding-atouch-frame-stm32u5-rvt50hqsfwca0>`_
- `RVT50HQSNWCA0 <https://riverdi.com/product/5-inch-lcd-display-capacitive-touch-panel-air-bonding-atouch-stm32u5-rvt50hqsnwca0>`_
- `RVT50HQSFWN00 <https://riverdi.com/product/5-inch-lcd-display-stm32u5-frame-rvt50hqsfwn00>`_
- `RVT50HQSNWN00 <https://riverdi.com/product/5-inch-lcd-display-stm32u5-rvt50hqsnwn00>`_

It natively supports the embedded NeoChrom GPU thanks to LVGL's support
for :ref:`Nema GFX <nema_gfx>`. NemaVG --- an extension to Nema's
base support --- is implemented by this MCU.

The MCU has an LCD driver peripheral which LVGL uses to update the
display. See the :ref:`LTDC <stm32 ltdc driver>` driver docs
for more info and how to customize its use.


Single-Board Computer Displays
******************************

The Single-Board Computer Displays are ready to use with
:ref:`Toradex <toradex>` Dahlia and Mallow carrier boards. In fact,
those carrier boards are compatible with all 34-pin Riverdi
MIPI-DSI displays.


Other Products
**************

The **EVE Intelligent Displays** feature a Bridgetek EVE graphics
controller IC so that the display can be controlled using
high-level drawing commands over a lower-bandwidth interface than
RGB, MIPI, etc.

The **HDMI Displays** sport an HDMI interface for streamlined
integration with PCs or SBCs. These displays are still low-profile
so they can be embedded into custom fixtures with ease.

The **Evaluation Boards** are affordable carrier boards for getting
Riverdi display panels up and running before designing or buying a
carrier board for a production application.
