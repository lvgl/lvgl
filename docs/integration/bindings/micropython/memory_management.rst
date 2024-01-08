Memory Management
-----------------

| When LVGL runs in Micropython, all dynamic memory allocations (:cpp:func:`lv_malloc`) are handled by Micropython's memory
  manager which is `garbage-collected <https://en.wikipedia.org/wiki/Garbage_collection_(computer_science)>`__ (GC).
| To prevent GC from collecting memory prematurely, all dynamic allocated RAM must be reachable by GC.
| GC is aware of most allocations, except from pointers on the `Data Segment <https://en.wikipedia.org/wiki/Data_segment>`__:

    - Pointers which are global variables
    - Pointers which are static global variables
    - Pointers which are static local variables

Such pointers need to be defined in a special way to make them reachable by GC


Identify The Problem
^^^^^^^^^^^^^^^^^^^^

Problem happens when an allocated memory's pointer (return value of :cpp:func:`lv_malloc`) is stored only in either **global**,
**static global** or **static local** pointer variable and not as part of a previously allocated ``struct`` or other variable.


Solve The Problem
^^^^^^^^^^^^^^^^^

- Replace the global/static local var with :cpp:expr:`(LV_GLOBAL_DEFAULT()->_var)`
- Include ``lv_global.h`` on files that use ``LV_GLOBAL_DEFAULT``
- Add ``_var`` to ``lv_global_t`` on ``lv_global.h``

Example
^^^^^^^


More Information
^^^^^^^^^^^^^^^^

- `In the README <https://github.com/lvgl/lv_binding_micropython#memory-management>`__
- `In the Blog <https://blog.lvgl.io/2019-02-20/micropython-bindings#i-need-to-allocate-a-littlevgl-struct-such-as-style-color-etc-how-can-i-do-that-how-do-i-allocatedeallocate-memory-for-it>`__
