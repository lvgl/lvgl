.. include:: custom_tools.txt

.. _creating components:

===================
Creating Components
===================

From the :ref:`project files pane`, place focus on the "components" folder,
right click and select "New File".  Name your file after the name you wish your
new Component to have, and end it with ``.xml``.

Once in the editor, the auto-completion assistance will help you to create the
Component's contents.

First create ``<component>`` tag itself.  Create any "local" enumerations, constants
or styles that will be specific to this Component, then create the ``<view>`` tag
to hold the Component itself.

The first attribute of the ``<view>`` tag is normally ``extends="..."`` where you
will name the LVGL Widget or another XML-based Component that this Component will
be based on.

After that, add any special properties that are particular to this Component, such
as states it may need to keep track of and reflect in its appearance.  Add any styles
needed along with the parts and states that they apply to.

As might be expected, global and/or Component-scope styles can be applied to any
Component by simply including a ``<style>`` element inside the XML element
for that Component.  Example:

.. code-block:: xml

    <style name="slider_knob_style" selector="knob"/>

You can test your Component at each phase using your mouse in the :ref:`preview pane`.
