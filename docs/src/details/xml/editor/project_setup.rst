.. include:: custom_tools.txt

.. _project setup:

=============
Project Setup
=============

Project setup is very simple.

If you wish to merely create a UI Component Library (that can be included in another
|lvglpro| Project), launch |lvglpro| and click the [Create Component Library] button.

If you wish to create a UI Project (that can be included in a firmware project),
launch |lvglpro| and click the [Create Project] button.

At this point you will see a "Save As" dialog box.  Navigate to the directory you want
to be *above* the your project directory, and give your new project a name that is
also a valid directory name for your system.  |lvglpro| will create an
empty directory with that name and populate your new project with the core set of
files needed.



``globals.xml`` File
********************

A ``globals.xml`` file appears in both |lvglpro| Projects as well as Component
Libraries.  It houses definitions that can be referred to by name throughout the
Project (or Component Library) that contains it.



``project.xml`` File
********************

The ``project.xml`` file ties together all the parts of an |lvglpro| Project.

If the project uses one or more externally-defined Component Libraries, they are
referenced in this file.  This can be done by adding a ``<folders>`` list in the
``project.xml`` file, and adding one ``<folder>`` element for each externally-defined
Component Library.

