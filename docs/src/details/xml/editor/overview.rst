.. include:: external_links.txt
.. include:: substitutions.txt
.. _editor_overview:

========
Overview
========



Introduction
************

|lvglpro| is the visual UI editor to build embedded UIs with LVGL faster, simpler and better.


It's core part is a powerful XML editor, preview them in real time in a pixel-perfect preview,
handle translations, add anaimations, create UI tests and many more.

In |lvglpro| Components, Screens, Tests, Aniomations, Translations, and basically
everything is described in XML files.

From the XML files |lvglpro| can export C code that can embedded into the firwares
just like any handwritten C code.

It's also possible to extend |lvglpro| also with widgets written in C. To make then work
in the Editor |lvglpro| can recompile its core preview engine from LVGL + the costom C code.
This way any custo code can be previewed, run and tested directly in the Editor.



Advantages
**********

|lvglpro| accelerates UI development by

- giving you an instant view of your additions and changes,
- speeding up building UIs as XML is way less versbbose than C,
- making it easy to share your UI online using :ref:`online_share`,
- accelearting the reimplementation of Figma_ designs with the help of a :ref:`Figma plugin <editor_figma>`,
- helping you to write :ref:`UI tests <xml_tests>` in seconds
- supporting :ref:`Data bindings <xml_subjects>` and versatile :ref:`Events <xml_events>`,
- integrating your UI via :ref:`command-line interface <editor_cli>` (CLI).

LVGL itself can load created UI from the generated C code, or load its parts from the
XML files directly at runtime, requiring no re-flashing.  Assets (images, fonts,
etc.) and XML UI component definitions can be read from an SD card, Wi-Fi, serial
port, or any other source.

Tests of the resulting UI can be built in, simulating clicks, waits, and screenshot
comparisons to instantly prove the UI is working as expected, or quickly highlighting
anything that needs attention.  This way is a great way to build up an automated and
thorough regression-testing pipeline for the UI, supporting high confidence with
every firmware release, small or large.

|lvglpro| can be run as:

- a stand-alone desktop application on Windows, Linux, and macOS;
- a VSCode Extension; and
- on-line (via a web browser).


All parts of the development sequence, including code-generation, Figma_ sync, XML
validation, and test execution, are available in a CLI tool, ready to integrate into
your CI/CD workflow.


Login and Licensing
*******************

.. TODO Gabor  (trial, buy licence on pro.lvgl.io, read more on the licences page)

|lvglpro| comes with a simple, scalable and royalty-free licensing model that works
for open-source projects, startups, and enterprises alike.  `See all plans`_.






