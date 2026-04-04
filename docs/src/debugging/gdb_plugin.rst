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
    obj@0x60700000dd10 0,0,799,599
    tabview@0x608000204ca0 0,0,799,599
        obj@0x607000025da0 0,0,799,69
        obj@0x607000025e80 0,70,799,599
        obj@0x60700002bd70 743,543,791,591
        btn@0x60700002c7f0 747,547,787,587
    keyboard@0x60d0000f7040 0,300,799,599
    dropdown-list@0x608000205420 0,0,129,129
        label@0x60d0000f7ba0 22,22,56,39
    (gdb)

The plugin provides the following commands.

- ``dump obj``: Dump the object tree.
- ``dump display``: Export display draw buffers to image files (BMP/PNG).
- ``dump cache``: Dump image or image header cache entries.
- ``check cache``: Run sanity check on image or image header cache.
- ``dump anim``: List all active animations.
- ``dump timer``: List all active timers.
- ``dump indev``: List all input devices.
- ``dump group``: List all focus groups with objects.
- ``dump image_decoder``: List all registered image decoders.
- ``dump fs_drv``: List all registered filesystem drivers.
- ``dump draw_task <expr>``: List draw tasks from a layer.
- ``dump dashboard``: Generate an HTML dashboard of all LVGL runtime state.
- ``info style``: Inspect style properties of an ``lv_style_t`` or an ``lv_obj_t``.
- ``info draw_unit``: Print raw struct details for each drawing unit.
- ``info obj_class <expr>``: Show object class hierarchy.
- ``info subject <expr>``: Show subject and its observers.
- ``lvglobal``: (NuttX only) Set which LVGL instance to inspect.

.. note::

    Some versions of ``gdb`` on Windows (e.g. those delivered with various versions
    of Perl) are compiled without Python support, so the ``source`` command will not
    be supported.



Dump Obj Tree
*************

``dump obj``: Dump the object tree.

``dump obj -L 2``: Dump the object tree with a depth of 2.

``dump obj <root_expr>``: Dump the object tree starting from the specified object.



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


Dump Display
************

``dump display``: Export the current display's draw buffers (buf_1, buf_2) to image files.

.. code:: bash

    (gdb) dump display
    (gdb) dump display -p /tmp/ -f png


Check Cache
***********

``check cache <type>``: Run sanity check on image or image header cache, validating
cross-pointers between red-black tree and linked list, decoded pointers, image sizes,
and source pointers. ``<type>`` is ``image`` or ``image_header``.


Dump Animations
***************

``dump anim``: List all active animations in a table with exec_cb, value range,
duration, repeat count, and status.

``dump anim --detail``: Print detailed info for each animation.


Dump Timers
***********

``dump timer``: List all active timers with callback, period, frequency, last_run,
repeat count, and paused state.


Dump Input Devices
******************

``dump indev``: List all registered input devices with type, state, read callback,
and configuration (long_press_time, scroll_limit, group).


Dump Focus Groups
*****************

``dump group``: List all focus groups with object count, frozen/editing/wrap state,
and focused object.


Dump Image Decoders
*******************

``dump image_decoder``: List all registered image decoders with name, info_cb,
open_cb, and close_cb.


Dump Filesystem Drivers
***********************

``dump fs_drv``: List all registered filesystem drivers with drive letter, driver type,
cache size, and callbacks (open, read, write, close).


Dump Draw Tasks
***************

``dump draw_task <layer_expr>``: Walk the draw task linked list from a layer expression
and display each task's type, state, area, opacity, and preferred draw unit id.


Dump Dashboard
**************

``dump dashboard``: Collect all LVGL runtime state (displays, object trees,
animations, timers, caches, input devices, groups, draw units/tasks,
subjects/observers, image decoders, filesystem drivers) and generate a
self-contained HTML file for offline browsing.

The dashboard supports three output modes:

- ``dump dashboard``: Generate ``lvgl_dashboard.html`` with all data embedded.
- ``dump dashboard --json``: Export raw JSON data to ``lvgl_dashboard.json``.
- ``dump dashboard --viewer``: Generate an empty HTML viewer (``lvgl_viewer.html``)
  that can load JSON files via drag-and-drop.

Use ``-o <path>`` to specify a custom output path.

Example:

.. code:: bash

    (gdb) dump dashboard
    Dashboard written to lvgl_dashboard.html (1.23s)

    (gdb) dump dashboard --json -o /tmp/state.json
    Dashboard written to /tmp/state.json (0.98s)

    (gdb) dump dashboard --viewer
    Viewer written to lvgl_viewer.html

The generated HTML is fully self-contained (no external dependencies) and
includes a sidebar for navigation, a search box for filtering, collapsible
object trees with style details, framebuffer image previews, and cross-reference
links between related objects.


Inspect Object Class
********************

``info obj_class <expr>``: Show the class hierarchy chain for an ``lv_obj_class_t``.

``info obj_class --all``: List all registered object classes in a table.

Example:

.. code-block:: none

   (gdb) info obj_class lv_button_class
   ObjClass: lv_button -> lv_obj -> lv_obj
     name           = lv_button
     base           = lv_obj
     size           = 48 editable=0 group_def=2
     editable       = 0
     group_def      = 2
     default_size   = (CONTENT, CONTENT) theme_inheritable=True
     theme_inh      = True

Inspect Subject
***************

``info subject <expr>``: Show a subject's type and all its observers.

Example:

.. code-block:: none

   (gdb) info subject &my_subject
   Subject: type=INT subscribers=2
     Observer: cb=<my_cb> target=0x... for_obj=True


Set LVGL Instance (NuttX)
*************************

``lvglobal``: Set which LVGL instance to inspect by finding the ``lv_global``
pointer. On single-instance systems, it auto-detects the global. On NuttX
multi-process systems, use ``--pid`` to specify the target process.

.. code:: bash

    (gdb) lvglobal
    (gdb) lvglobal --pid 3


Data Export API
***************

Each wrapper class provides a ``snapshot()`` method that returns a ``Snapshot``
object containing a pure Python dict (JSON-serializable) plus an optional
reference to the original wrapper via ``_source``.

.. code:: python

    from lvglgdb import LVTimer, curr_inst

    timers = list(curr_inst().timers())
    snap = timers[0].snapshot()

    # Dict-like access
    print(snap["timer_cb"], snap["period"])

    # JSON serialization
    import json
    print(json.dumps(snap.as_dict(), indent=2))

    # Bulk export
    snapshots = LVTimer.snapshots(timers)
    data = [s.as_dict() for s in snapshots]

The ``Snapshot`` class supports dict-like read access (``[]``, ``keys()``,
``len()``, ``in``, iteration) and ``as_dict()`` for JSON serialization.
All values in ``as_dict()`` are pure Python types (``str``, ``int``, ``float``,
``bool``, ``None``, ``dict``, ``list``) with no ``gdb.Value`` references.

Wrapper classes with ``snapshot()`` support: ``LVAnim``, ``LVTimer``,
``LVIndev``, ``LVGroup``, ``LVObject``, ``LVObjClass``, ``LVObserver``,
``LVSubject``, ``LVDrawTask``, ``LVDrawUnit``, ``LVFsDrv``,
``LVImageDecoder``, ``LVCache``, ``LVDisplay``,
``LVRedBlackTree``, ``LVEventDsc``, ``LVList``.

``LVStyle`` provides ``snapshots()`` (plural) which returns a list of
``Snapshot`` objects for each style property, but does not have a singular
``snapshot()`` method.

Most wrapper classes also provide a static ``snapshots(items)`` method for
bulk export (e.g. ``LVAnim.snapshots(anims)``). Additionally,
``LVImageCache`` and ``LVImageHeaderCache`` provide instance-level
``snapshots()`` methods that export all cache entries.


Architecture
************

The GDB plugin is organized into four layers. The overview below shows how
terminal commands and the HTML dashboard both flow through the same snapshot
abstraction down to raw GDB memory access:

.. mermaid::
   :zoom:

   graph TD
        subgraph "Rendering Layer"
            CLI["GDB Terminal<br/>dump obj, info style, ..."]
            DASH["HTML Dashboard<br/>dump dashboard"]
        end

        subgraph "Formatter / Renderer"
            FMT["formatter.py<br/>print_info · print_spec_table"]
            HR["html_renderer.py<br/>template + CSS + JS"]
        end

        subgraph "Data Collection"
            DC["data_collector.py<br/>collect_all() → JSON dict"]
            SNAP["Snapshot<br/>_data + _display_spec"]
        end

        subgraph "Value Wrappers (lvgl/)"
            W["LVObject · LVDisplay · LVAnim<br/>LVCache · LVTimer · LVDrawBuf<br/>LVIndev · LVGroup · ..."]
            GDB["gdb.Value (C struct memory)"]
        end

        CLI --> FMT
        FMT --> SNAP
        DASH --> HR
        HR --> DC
        DC --> SNAP
        SNAP --> W
        W --> GDB

        style CLI fill:#4CAF50,color:#fff
        style DASH fill:#4CAF50,color:#fff
        style FMT fill:#FF9800,color:#fff
        style HR fill:#FF9800,color:#fff
        style DC fill:#2196F3,color:#fff
        style SNAP fill:#2196F3,color:#fff
        style W fill:#9C27B0,color:#fff
        style GDB fill:#616161,color:#fff

Each wrapper class declares a ``_DISPLAY_SPEC`` describing its fields and
exports a ``snapshot()`` method that returns a self-describing ``Snapshot``
object carrying both the data dict and the display spec. The ``cmds/`` layer
simply passes snapshots to generic formatters (``print_info``,
``print_spec_table``) which read the embedded spec to render output — no
command needs to know the internal structure of any wrapper. The detailed
snapshot flow is shown below:

.. mermaid::
   :zoom:

   graph RL
        subgraph "cmds/ layer"
            CMD["GDB Commands<br/>(dump obj, info style, ...)"]
        end

        subgraph "formatter.py"
            PI["print_info(snapshot)"]
            PT["print_table()"]
            PST["print_spec_table(snapshots)"]
            RTC["resolve_table_columns(spec)"]
            PST --> RTC
            PST --> PT
        end

        subgraph "snapshot.py"
            SNAP["Snapshot<br/>._data (pure dict)<br/>._display_spec<br/>._source"]
        end

        subgraph "data_utils.py"
            DU["ptr_or_none()<br/>fmt_cb()<br/>..."]
        end

        subgraph "Wrapper classes"
            direction TB
            DS["_DISPLAY_SPEC<br/>{info, table, empty_msg}"]
            SN["snapshot() → Snapshot"]
            SNS["snapshots() → list[Snapshot]"]
            SN --> SNAP
            SN -. "display_spec=" .-> DS
            SNS --> SN
        end

        subgraph "Wrappers"
            direction LR
            W1["LVAnim"]
            W2["LVTimer"]
            W3["LVCache"]
            W4["LVObject"]
            W5["LVGroup"]
            W6["LVIndev"]
            W7["...others"]
        end

        CMD -- "wrapper.snapshot()" --> SN
        CMD -- "print_info(snap)" --> PI
        CMD -- "print_spec_table(snaps)" --> PST
        PI -- "reads _display_spec" --> SNAP
        PST -- "reads _display_spec" --> SNAP
        SN -- "uses" --> DU
        W1 & W2 & W3 & W4 & W5 & W6 & W7 -. "each defines" .-> DS

        style PI fill:#4CAF50,color:#fff
        style PT fill:#4CAF50,color:#fff
        style PST fill:#4CAF50,color:#fff
        style RTC fill:#4CAF50,color:#fff
        style SNAP fill:#2196F3,color:#fff
        style DU fill:#FF9800,color:#fff
        style DS fill:#9C27B0,color:#fff
