.. _translation:

===========
Translation
===========

Overview
********

LVGL supports two ways of handling translations:

- `lv_i18n <https://github.com/lvgl/lv_i18n>`_: A comprehensive tool that extracts translatable strings from C files into YAML files, and generates C translation files from them. It also supports plural forms. See its README for details.

- ``lv_translation``: A simpler yet more flexible solution that allows adding translations statically or dynamically. This is the method documented here.



Add Translations
****************


Static Translations
-------------------

If most translations are known at compile time, they can be defined using string arrays:

.. code-block:: c

    static const char * languages[] = {"en", "de", "es", NULL};
    static const char * tags[] = {"tiger", "lion", "rabbit", "elephant", NULL};
    static const char * translations[] = {
        "The Tiger", "Der Tiger", "El Tigre",
        "The Lion", "Der Löwe", "El León",
        "The Rabbit", "Das Kaninchen", "El Conejo",
        "The Elephant", "Der Elefant", "El Elefante",
    };

    lv_translation_add_static(languages, tags, translations);

This method uses only a little extra RAM, as only the pointers to the strings are stored.


Dynamic Translations
--------------------

If translations are only available at runtime (e.g., from files, serial ports, or online sources), they can be added dynamically.

This approach involves memory allocation. See the example at the bottom of this page for reference.



Select a Language
*****************

Once translations are registered, use:

:cpp:expr:`lv_translation_set_language("language")`

to set the current language. The parameter must match one of the language names provided during registration.



Translate Strings
*****************

To retrieve a translation for a given tag, use:

- :cpp:expr:`lv_translation_get("tag")`
- or the shorthand: :cpp:expr:`lv_tr("tag")`

These return a translated string which can be used with widgets:

.. code-block:: c

    lv_label_set_text(label, lv_tr("settings"));
    lv_dropdown_set_options(dd, lv_tr("color_list"));


Fallbacks
---------

If a tag exists but the translation for the selected language is missing
the tag itself will be returned.

If the tag is not found at all, the tag itself will be used as a fallback as well.

.. _lv_translation_example:



Example
*******

.. include:: ../../examples/others/translation/index.rst

.. _lv_translation_api:



API
***
