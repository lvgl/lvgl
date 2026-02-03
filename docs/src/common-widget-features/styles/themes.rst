.. _style_themes:

======
Themes
======

Overview
********

A Theme is a collection of styles with the knowledge of which subset of styles
are applied to which types of Widgets. If there is an active theme LVGL
applies it to every newly-created widget. This will give a default appearance
to the UI which can then be modified by adding further styles.

Every display can have a different theme. For example, you could have a
colorful theme on a TFT and monochrome theme on a secondary monochrome
display.

To set a theme for a display, two steps are required:

1. Initialize the theme.
2. Assign the initialized theme to the display.

Theme initialization functions can have different prototypes. This
example shows how to set the "default" theme:

.. code-block:: c

   lv_theme_t * th = lv_theme_default_init(display,                 /* Use DPI, size, etc. from this display */
                                           LV_COLOR_PALETTE_BLUE,   /* Primary and secondary palette */
                                           LV_COLOR_PALETTE_CYAN,
                                           false,                   /* Dark theme?  False = light theme. */
                                           &lv_font_montserrat_10,  /* Small, normal, large fonts */
                                           &lv_font_montserrat_14,
                                           &lv_font_montserrat_18);

   lv_display_set_theme(display, th); /* Assign theme to display */

The included themes are enabled in ``lv_conf.h``. If the default theme
is enabled by :c:macro:`LV_USE_THEME_DEFAULT` LVGL automatically initializes
and sets it when a display is created.

Extending themes
****************

Built-in themes can be extended. If a custom theme is created, a parent
theme can be selected. The parent theme's styles will be added before
the custom theme's styles. Any number of themes can be chained this way.
E.g. default theme -> custom theme -> dark theme.

:cpp:expr:`lv_theme_set_parent(new_theme, base_theme)` extends the
``base_theme`` with the ``new_theme``.

There is an example of this among the :ref:`Style examples <style_examples>`.

