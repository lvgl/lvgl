.. _xml_translation:

=====================
Language Translations
=====================

Overview
********

The XML translation module allows defining and using translated strings directly within XML files.

It's built on top of :ref:`LVGL's translation module <translation>`.

Check it out to learn more about selecting the active language, retrieving
translations, and fallback behavior.



Usage
*****

Example XML translation definition:

.. code-block:: xml

    <translations languages="en de hu">
      <translation tag="dog"       en="The dog"     de="Der Hund"       hu="A kutya"/>
      <translation tag="cat"       en="The cat"     de="Die Katze"      hu="A cica"/>
      <translation tag="snake"     en="A snake"     de="Eine Schlange"  hu="A kígyó"/>
    </translations>

In the root ``<translations>`` tag, the ``languages`` attribute defines the available languages,
e.g., ``languages="en de hu"``. Language codes are free-form, but ISO-style codes are recommended.

Each ``<translation>`` defines a ``tag``, which acts as the lookup key, and attributes for each language.

Translations may be omitted --- fallbacks will be applied when needed. See :ref:`translation_fallbacks` for more details.

To register XML translations:

- :cpp:expr:`lv_xml_register_translation_from_file("path/to/file.xml")`
- :cpp:expr:`lv_xml_register_translation_from_data(xml_string)`

Multiple XML sources can be registered; they will be merged and searched collectively.
