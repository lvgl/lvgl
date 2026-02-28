.. _gdb_plugin:

===========
GDB Plug-In
===========

Debugging LVGL with GDB
***********************

To facilitate debugging LVGL with GDB, a GDB plugin is provided. This plugin
can be found in the ``lvgl/scripts/gdb`` directory. The GDB plugin can be used
with any target where GDB is available. For example, you can use it to debug a
device connected to a PC via JLink, which provides a GDB server. Additionally,
if your device crashes and you have a core dump, you can use GDB to analyze the
core dump. To load the LVGL GDB plugin within GDB's command line, type the
following command:

``source lvgl/scripts/gdb/gdbinit.py``


Example of usage:

.. code:: bash

    (gdb) source lvgl/scripts/gdb/gdbinit.py

    (gdb) dump obj -L 2
    obj@0x60700000dd10 (0,0,799,599)
    tabview@0x608000204ca0 (0,0,799,599)
        obj@0x607000025da0 (0,0,799,69)
        obj@0x607000025e80 (0,70,799,599)
        obj@0x60700002bd70 (743,543,791,591)
        btn@0x60700002c7f0 (747,547,787,587)
    keyboard@0x60d0000f7040 (0,300,799,599)
    dropdown-list@0x608000205420 (0,0,129,129)
        label@0x60d0000f7ba0 (22,22,56,39)
    (gdb)

The plugin provides the following commands.

- ``dump obj``: Dump the object tree.
- ``info style``: Inspect style properties of an ``lv_style_t`` or an ``lv_obj_t``.
- ``info draw_unit``: Display all current drawing unit information.

.. note::

    Some versions of ``gdb`` on Windows (e.g. those delivered with various versions
    of Perl) are compiled without Python support, so the ``source`` command will not
    be supported.



Dump Obj Tree
*************

``dump obj``: Dump the object tree.

``dump obj -L 2``: Dump the object tree with a depth of 2.

``dump obj -a 0x60700000dd10``: Dump the object tree starting from the specified address.



Inspect Style
*************

``info style <style_var>``: Inspect a single ``lv_style_t`` variable. Properties are
displayed with resolved names and formatted values (colors shown as hex).

``info style --obj <obj_var>``: Inspect all styles of an ``lv_obj_t``, grouped by
style slot with selector and flags.

Example:

.. code:: bash

    (gdb) info style my_style
    +-----------+---------+
    | prop      | value   |
    +-----------+---------+
    | BG_COLOR  | #ff0000 |
    | BG_OPA    | 255     |
    +-----------+---------+

    (gdb) info style --obj lv_global->disp_default->act_scr
    [0] MAIN|DEFAULT  local
    +-----------+---------+
    | prop      | value   |
    +-----------+---------+
    | BG_COLOR  | #ff0000 |
    +-----------+---------+



Connecting to a Debugger
************************

This command provides the ability to connect and debug GDB Python Script using IDE.

Connect to ``PyCharm`` / ``VSCode`` / ``Eclipse (not supported yet)``

``debugger -t pycharm``

``debugger -t vscode``

``debugger -t eclipse``

Perform a web search for ``pydevd_pycharm`` or ``debugpy`` for details about how to
use the debugger.
