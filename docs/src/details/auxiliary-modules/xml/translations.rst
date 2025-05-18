.. _xml_translations:

============
Translations
============

Overview
********

The translation module allows translating strings and texts defined in XML.

Translations can be loaded from XML files, C arrays, or both at the same time.

Translation Sources
*******************

XML
---

Example of translations defined in XML:

.. code-block:: xml
    <translations languages="en de hu">
      <translation tag="dog"       en="The dog"     de="Der Hund"       hu="A kutya"/>
      <translation tag="cat"       en="The cat"     de="Die Katze"      hu="A cica"/>
      <translation tag="snake"     en="A snake"     de="Eine Schlange"  hu="A kígyó"/>
    </translations>

In the root `<translations>` tag, specify the languages to be used, e.g.,
``languages="en de hu"``. There are no strict rules for language IDs,
but ISO country/language codes are typically used.

Each `<translation>` entry defines the translations for a specific `tag`,
which acts as the identifier when referencing the translation.

Translations can be omitted. In such cases, :ref:`Fallbacks <xml_translations_fallback>` will apply.

XML translations can be registered via:

- :cpp:expr:`lv_xml_translation_register_from_file("path/to/file.xml")`
- :cpp:expr:`lv_xml_translation_register_from_data(xml_string)`

Multiple XML files can be registered. They are concatenated,
and all registered XMLs are checked when resolving a translation.

C Array
-------

Translations can also be registered from C arrays, without allocating memory for the strings—only pointers are stored.

The structure mimics the XML format:

.. code-block:: c
    const char * languages[] = {"en", "de", "es", NULL};
    const char * tags[] = {"tiger", "lion", "rabbit", "elephant", NULL};
    const char * translations[] = {
        "The Tiger", "Der Tiger", "El Tigre",
        "The Lion", NULL, "El León",
        "The Rabbit", "Das Kaninchen", "El Conejo",
        "The Elephant", "Der Elefant", NULL,
    };

Use `NULL` to indicate missing translations.

Register C array translations with:
:cpp:expr:`lv_xml_translation_register_from_array(languages, tags, translations)`

Any number of XML and/or C array translation sources can be registered,
and they will be combined.

Selecting a Language
********************

Use :cpp:expr:`lv_xml_set_language(language)` to select the active language.
The `language` must be defined in at least one registered XML or C array.

Note: widgets will not automatically update when the language changes,
as the translated texts are resolved when the widget is created.

Getting Translations
********************

Use :cpp:expr:`lv_xml_get_translation(tag)` to get a pointer to the translated string.

.. _xml_translations_fallback:

Fallbacks
---------

If a tag exists but the translation for the selected language is missing:

1. The translation from the first listed language will be used.
2. If it's also missing there, the tag itself will be returned.

If the tag is not found at all, the tag itself will be returned as the fallback.

Usage in XML
************

Some widgets support properties with a `*-translated` suffix. For example:
``<lv_label text-translated="dog"/>`` will use the translated string for the `"dog"` tag.
