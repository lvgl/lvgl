.. include:: custom_tools.txt

.. _files:

=====
Files
=====

XML File Format
***************

In most cases, you will not need to understand the XML file format in detail because
the editor's auto-completion features will always steer you in the right direction
about what is legal syntax where the cursor is, and what is not.  When you need help,
simply hit [Alt-Space] and a menu will pop up showing your legal options.

It can, however, be helpful understand what is possible to accomplish in the XML you
are editing, and for that, please refer to the `XML Module`_ in LVGL's User
Documentation.



Projects vs Component Libraries
*******************************

A Component Library is a set of 1 or more XML files that can be used in any
|lvglpro| Project.

|lvglpro| Projects use Component Libraries, but Component Libraries do not
use Projects.



How Can I Tell the Difference Between the Two?
----------------------------------------------

The difference between the directory structures two types of |lvglpro| "creation
environments" (Projects vs Component Libraries) is that a Project has a
``project.xml`` file in it (which ties together all of its parts), and a Component
Library does not.

