.. _display_mirroring:

===================
Mirroring a Display
===================

To mirror the image of a display to another display, you don't need to use
multi-display support. Just transfer the buffer received in the first display's
:ref:`flush_callback` to the other display as well.
