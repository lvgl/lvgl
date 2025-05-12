.. _getting_lvgl:

============
Getting LVGL
============

LVGL is available on GitHub: https://github.com/lvgl/lvgl.

You can clone it or
`Download <https://github.com/lvgl/lvgl/archive/refs/heads/master.zip>`__
the latest version of the library from GitHub.

The graphics library itself is the ``lvgl`` directory.  It contains several
directories but to use LVGL you only need the ``.c`` and ``.h`` files under
the ``src`` directory, plus ``lvgl/lvgl.h``, and ``lvgl/lv_version.h``.


Demos and Examples
------------------

The ``lvgl`` directory also contains an ``examples`` and a ``demos``
directory.  If your project needs examples and/or demos, add the these
directories to your project.  If ``make`` or :ref:`build_cmake` handle the
examples and demos directories, no extra action is required.

