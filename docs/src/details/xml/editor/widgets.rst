.. include:: custom_tools.txt

.. _creating widgets:

================
Creating Widgets
================

From the :ref:`project files pane`, place focus on the "widgets" folder,
right click and select "New File".  Name your file after the name you wish your
new Widget to have, and end it with ``.xml``.

Once in the editor, the auto-completion assistance will help you to create the
Widget's contents.

First create ``<widget>`` tag itself.  Create any "local" enumerations, constants
or styles that will be specific to this Widget, then create the ``<view>`` tag
to hold the Widget itself.

The first attribute of the ``<view>`` tag is normally ``extends="..."`` where you
will name the LVGL Widget or another XML-based Widget that this Widget will
be based on.

If you are creating a brand new Widget ... TODO

After that, add any special properties that are particular to this Widget, such
as states it may need to keep track of and reflect in its appearance.  Add any styles
needed along with the parts and states that they apply to.

You can test your Widget at each phase using your mouse in the :ref:`preview pane`.



Usage
*****

.. todo:: fill in
