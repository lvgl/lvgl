.. _introduction:

============
Introduction
============

LVGL (Light and Versatile Graphics Library) is a free and open-source C graphics
library providing everything you need to create an embedded GUI with easy-to-use
graphical elements, beautiful visual effects, and a low memory footprint.

LVGL provides the "GUI engine" that handles all end-user interaction with your
application, including not only drawing the contents of the display and accepting
user input and turning it into events "catchable" by the application, but also
providing 30+ built-in widgets, 100+ style properties, scrolling, animations,
advanced software rendering, built-in GPU support from MCUs to OpenGL, and much more.
The combination of these enable you to efficiently develop UIs without having to
re-invent the wheel.

Using LVGL in a firmware project effectively reduces the GUI-developer's job to
defining the GUI and furnishing it with appropriate event hooks to carry out the
valuable work of the GUI.

LVGL can use GUIs that are 100% defined in C code you write (for which any updates to
the GUI require modifying that C code and re-flashing the firmware), or GUI
definitions provided in XML files (for which updates to the GUI require only
modifying the XML files that define the GUI for that firmware), or any combination of
these two that suits your project.



.. _key_features:

Key Features
************

- Powerful building blocks such as :ref:`buttons, charts, lists, sliders, images <widgets>`, etc.
- Advanced graphics with animations, anti-aliasing, opacity, smooth scrolling
- Various input devices such as touchpad, mouse, keyboard, encoder, etc.
- Multi-language support with UTF-8 encoding
- Multi-display support, even with mixed color formats
- Fully customizable graphic elements with CSS-like styles
- Hardware independent:  use with any microcontroller or display
- Scalable: able to operate with little memory (64 kB Flash, 16 kB RAM)
- :ref:`OS <threading>`, external memory and :ref:`GPU <draw>` are supported but not required
- Single frame buffer operation even with advanced graphic effects
- Written in C for maximal compatibility (C++ compatible)
- :ref:`Simulator <simulator>` to start embedded GUI design on a PC without embedded hardware
- User code developed under simulator can be shared with firmware to make UI development more efficient.
- Binding to :ref:`MicroPython`
- Tutorials, examples, themes for rapid GUI design
- Documentation is available online
- Free and open-source under MIT license
- Free for commercial projects



.. toctree::
    :maxdepth: 2

    requirements
    license
    faq
    repo
