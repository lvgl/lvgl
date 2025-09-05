.. _xml_translation:

============
Translations
============

Overview
********

The XML translation module allows defining and using translated strings directly within XML files.

It's built on top of :ref:`LVGL's translation module <translation>`.
Check it out to learn more about selecting the active language, retrieving translations, and fallback behavior.


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

Translations may be omitted—:ref:``Fallbacks <xml_translations_fallback>`` will be applied when needed.

To register XML translations:

- :cpp:expr:`lv_xml_translation_register_from_file("path/to/file.xml")`
- :cpp:expr:`lv_xml_translation_register_from_data(xml_string)`

Multiple XML sources can be registered; they will be merged and searched collectively.



Usage in XML
************

Some widget properties support a ``*-translated`` suffix to refer to translation tags. For example:

.. code-block:: xml

    <lv_label text-translated="dog"/>

This sets the label's text to the translated string for ``"dog"``.

