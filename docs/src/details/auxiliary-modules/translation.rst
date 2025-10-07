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


.. _translation_fallbacks:

Fallbacks
---------

If a tag exists but the translation for the selected language is missing
the tag itself will be returned.

If the tag is not found at all, the tag itself will be used as a fallback as well.

Dynamically Updating UI Text
****************************

When :cpp:expr:`lv_translation_set_language("language")` is called, LVGL sends ``LV_EVENT_TRANSLATION_LANGUAGE_CHANGED`` to every widget, allowing you to update text automatically.

The new language can be retrieved by either calling :cpp:expr:`lv_translation_get_language()` or by getting the event parameter in the event callback with :cpp:expr:`lv_event_get_param(e)`

Basic Example
-------------

.. code-block:: c

    static void on_language_change(lv_event_t * e)
    {
        lv_obj_t * label = lv_event_get_target_obj(e);
        const char * tag = (const char *) lv_event_get_user_data(e);
        lv_label_set_text(label, lv_tr(tag));

        /* You can get the new language with `lv_event_get_param`*/
        /* const char * language = (const char *) lv_event_get_param(e); */
        /* or with `lv_translation_get_language` */
        /* const char * language = lv_translation_get_language(); */
    }

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_add_event_cb(label, on_language_change, LV_EVENT_TRANSLATION_LANGUAGE_CHANGED, "tag1");

See the the bottom of this page for a complete example.

.. _lv_translation_example:



Example
*******

.. include:: ../../examples/others/translation/index.rst

.. _lv_translation_api:



API
***
