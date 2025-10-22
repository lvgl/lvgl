.. include:: substitutions.txt
.. _user interface:

==============
User Interface
==============



First Run
*********

The first time you launch |lvglpro|, it will pop up a window with the message

    "Pulling Container image for LVGL, this may take a few minutes...".

This message means the Editor is downloading a pre-configured container image that
includes all the necessary tools and dependencies for building LVGL projects.
|lvglpro| handles (and updates) the container automatically in the background.  The
process can take a few minutes the first time, depending on your internet speed, but
it only happens once.  After that, the container is reused for future sessions.

When a new version of the container is published (e.g. with updated toolchains, build
logic, etc.), the Editor automatically checks for and pulls the latest version.  You
won’t need to do anything manually, it stays up-to-date in the background.



Login
*****

1. enter your email address
2. you will receive an email with an OTP (One-Time Password)
3. it will create a user for you (no password is needed)



Licence Selection
*****************

After logging in you need select a licence for the list.



Launch Screen
*************

.. figure:: /_static/images/editor_initial_ui.png
    :align: center
    :alt: |lvglpro| Opening User Interface

    ..

    |lvglpro| Opening User Interface


The Launch Screen opens up every time you start the Editor and are already logged in.

Here you can:

- Open an existing project
- Create a new project
- Open any recent project

Working on a Project
********************

.. figure:: /_static/images/editor_interface.png
    :align: center
    :alt: |lvglpro| User Interface

    ..

    |lvglpro| User Interface


Menu
----

|lvglpro| has the usual File / View / Edit / etc. application menu.


.. _project files pane:

Project Files Pane
------------------

The left panel is a typical "project-tree" navigation panel.  It allows you to
efficiently and conveniently create, open and rename files and manage your project.

The subdirectories shown in the project are for the respective type of UI components
(i.e. Screens, Widgets, Components, Fonts and Images).


.. _editor pane:

Editor Pane
-----------

The Editor Pane appears in the middle of the window.  This is where you make
|lvglpro| do valuable work for you, building your UI or Component Library.  It is here
that you can observe and edit the contents of any file in your project, but its
specialty is editing XML that defines UI components (:ref:`xml_widgets`,
:ref:`xml_components` and :ref:`xml_screens`).  While editing XML, |lvglpro| has a
powerful auto-completion-enabled user interface, which makes defining, editing and
validating UI components a breeze.  Similar to context-sensitive assistance
available in other "smart editors", the Editor Pane assists you with almost every
keystroke, considerably shortening the learning curve required to produce finished,
professional-quality UIs with |lvglpro|.


.. _right panel:

Right Panel
-----------

.. _preview pane:
.. _inspector:

Preview Pane and Inspector
~~~~~~~~~~~~~~~~~~~~~~~~~~

When the Editor Pane is editing an XML file defining a Component or Widget, the
Preview Pane shows a live preview of the XML UI definition being edited, pushed
through *real LVGL rendering* so you can instantly see the results of your work, that
will match the final result produced by firmware, accurate down to the pixel.

Simultaneously, it allows you to interact with that UI to see how it will look and
feel at runtime.

How it Works:

- LVGL has a built-in XML parser that can create UIs at runtime
- the inspector uses LVGL as it is
- the editor sends the XML files to LVGL
- the editor asks LVGL to create an instance of the opened XML file
- when the xml changes only the opened XML file is uploaded, the screen is cleared,
  and a new instance is created
- the XML editor parses the XML files of the LVGL widgets and the user's XML files to
  validate the XMLs
- code is also exported by parsing the XML files

Turn the Preview Pane into an Inspector by holding down the [Ctrl] key.  The Inspector
allows you to:

- visualise bounding box, padding, margins and click area
- check size and position by dragging
- perform measurement
- double click to jump to the XML file
- single click to change editor focus to the matching XML element

.. TODO Gabor 20 sec video to show all these

.. _console/subjects pane:

Output Pane
~~~~~~~~~~~

The Output Pane contains a number of tabs where you can see various results, and
manage certain lists as part of your project.

:Console Tab:   contains "log output" of the internal actions taken and their results.

:Subjects Tab:  contains list of the Observer-Pattern Subjects implemented in your XML
                source files.  Each Subject represents a single "datum" in the
                application, that UI elements (Widgets and Components) can "observe"
                and reflect visually. See :ref:`xml_subjects` for more details.

:Test Tab:      TODO:  Description of Test Tab

Tabs Coming Soon:  Tests, Translations, Animations

