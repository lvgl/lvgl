.. _editor_overview:

========
Overview
========

TODO: describe the purpose and advantages, it's related to XML, has tools (CLI, online share, figma), 3-4 paragraph

Getting started
***************

TODO Work on Windows, Linux, MacOS and VSCode. Link the guides.

Login and Licences (trial, buy licence on pro.lvgl.io, read more on the licences page)

Check out the examples (Learn by examples page)
or learn more about XML

User Interface
**************

Screen shot, briefly describe the main parts

XML Editor
----------

Autocomplete, validation

Inspector
---------

- What is it? (show the rendered content, the user click on it to try out)
- How to use?  (Press Ctrl or turn on by the button)

  - visualise bounding box, padding, maring and click area
  - chege size and position by dragging
  - measurement
  - double click jumps to the XML file
  - single click focuses the matching XML

20 sec video to show all these


How does it work?
*****************

- LVGL has a built-in XML parser that can create UIs at runtime
- the inspector uses LVGL as it is
- the editor sends the XML files to LVGL
- the editor ask LVGL to create an instance of the opened XML file
- when the xml changes only the opened XML file is uploaded, the screen is cleared, and a new instance is created
- the XML editor parses the XML files of the LVGL widgets and the user's XML files to validate the XMLs
- code is also exported by parsing the XML files

Project
*******

- What is a project?
- What can you describe in project.xml?
- How does nested projects work?



