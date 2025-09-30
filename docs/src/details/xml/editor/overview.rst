.. include:: external_links.txt
.. include:: substitutions.txt
.. _editor_overview:

========
Overview
========

|lvglpro| accelerates UI development by

- giving you an instant view of your additions and changes,
- converting Figma_ designs to LVGL,
- making it easy to share your UI screens online with collaborators and stakeholders,
- helping you to write UI tests, and
- integrating your UI via command-line interface (CLI).

Because |lvglpro| allows you to build a UI with a smart XML editor with powerful
auto-completion, it allows UI designers to efficiently contribute finished UI
components to firmware projects without having to be C programmers.

Simultaneously, |lvglpro| reduces turn-around time between UI changes and seeing the
actual results to mere seconds, making it practical for UI designer, developers and
business teams to collaborate on UI content in real time.

LVGL itself can load created UI from the generated C code, or load its parts from the
XML files directly at runtime, requiring no re-flashing.  Assets (images, fonts,
etc.) and XML UI component definitions can be read from an SD card, Wi-Fi, serial
port, or any other input source.

Use the LVGL Figma_ plug-in to import style properties and images as XML code.  It
also supports automatic downloading of linked styles and images.

Tests of the resulting UI can be built in, simulating clicks, waits, and screenshot
comparisons to instantly prove the UI is working as expected, or quickly highlighting
anything that needs attention.  This way is a great way to build up an automated and
thorough regression-testing pipeline for the UI, supporting high confidence with
every firmware release, small or large.

All parts of the development sequence, including code-generation, Figma_ sync, XML
validation, and test execution, are available in a CLI tool, ready to integrate into
your workflow.

|lvglpro| comes with a simple, scalable and royalty-free licensing model that works
for open-source projects, startups, and enterprises alike.  `See all plans`_.

|lvglpro| comes in Linux, Windows, and macOS flavors so your team will have few, if
any, technical barriers to begin using it.

It can be run as:

- a stand-alone application on Windows, Linux, and macOS;
- a VSCode Extension; and
- on-line (via a web browser).



Login and Licensing
*******************

.. TODO Gabor  (trial, buy licence on pro.lvgl.io, read more on the licences page)






