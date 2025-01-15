.. _display_split_image:

===========
Split Image
===========

You can create a larger virtual display from an array of smaller ones.
You can create it by:

1. setting the resolution of the displays to the large display's resolution;
2. in :ref:`flush_callback`, truncate and modify the ``area`` parameter for each display; and
3. send the buffer's content to each real display with the truncated area.
