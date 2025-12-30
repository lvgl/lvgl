.. _extending_combining_displays:

============================
Extending/Combining Displays
============================

.. _display_mirroring:

Mirroring a Display
*******************

To mirror the image of a display to another display, you don't need to use
multi-display support. Just transfer the buffer received in the first display's
:ref:`flush_callback` to the other display as well.



.. _display_split_image:

Splitting an Image
******************

You can create a larger virtual display from an array of smaller ones.
You can create it by:

1. setting the resolution of the displays to the large display's resolution;
2. in :ref:`flush_callback`, truncate and modify the ``area`` parameter for each display; and
3. send the buffer's content to each real display with the truncated area.
