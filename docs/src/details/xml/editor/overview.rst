.. include:: external_links.txt
.. include:: substitutions.txt
.. _editor_overview:

========
Overview
========



Introduction
************

|lvglpro|'s Editor is an XML editor to simplify and speed up building embedded UIs.

Its core part is a powerful XML editor with a real-time pixel-perfect preview. Besides
supporting the creation and preview of components and screens, it also supports
describing UI tests, animations, translations, and basically everything defined in XML.

From the XML files, the Editor can export C code that can be embedded into the firmware
just like any handwritten C code.

The Editor can also recompile its preview from LVGL and custom widgets written in C.

This section of the documentation describes how the Editor can be installed and used. To
learn more about the XML format itself, visit :ref:`this page <xml_main>`.



Advantages
**********

Using the Editor of |lvglpro| instead of writing XML by hand has several advantages:

- see the components instantly in the preview,
- use an inspector tool to visualize widget sizes, paddings, etc.,
- autocompletion speeds up writing XML,
- accelerate the reimplementation of Figma_ designs with the help of a :ref:`Figma
  plugin <editor_figma>`,
- support writing and running :ref:`UI tests <xml_test>` in seconds,
- support adjusting the subject to test :ref:`Data bindings <xml_subjects>`.

The Editor can run as:

- a stand-alone desktop application on Windows, Linux, and macOS;
- a VSCode extension; and
- online (via a web browser).

