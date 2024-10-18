.. _file_explorer:

=============
File Explorer
=============

``lv_file_explorer`` provides an API to browse the contents of the file
system. ``lv_file_explorer`` only provides the file browsing function,
but does not provide the actual file operation function. In other words,
you can't click a picture file to open and view the picture like a PC.
``lv_file_explorer`` will tell you the full path and name of the
currently clicked file. The file operation function needs to be
implemented by the user.

The file list in ``lv_file_explorer`` is based on
:ref:`lv_table`, and the quick access bar is based on
:ref:`lv_list`. Therefore, care should be taken to ensure
that :ref:`lv_table` and :ref:`lv_list` are
enabled.

.. _file_explorer_usage:

Usage
-----

Enable :c:macro:`LV_USE_FILE_EXPLORER` in ``lv_conf.h``.

First use :cpp:expr:`lv_file_explorer_create(lv_screen_active())` to create a file
explorer, The default size is the screen size. After that, you can
customize the style like widget.

Quick access
~~~~~~~~~~~~

The quick access bar is optional. You can turn off
:c:macro:`LV_FILE_EXPLORER_QUICK_ACCESS` in ``lv_conf.h`` so that the quick
access bar will not be created. This can save some memory, but not much.
After the quick access bar is created, it can be hidden by clicking the
button at the top left corner of the browsing area, which is very useful
for small screen devices.

You can use
:cpp:expr:`lv_file_explorer_set_quick_access_path(file_explorer, LV_FILE_EXPLORER_QA_XX, "path")`
to set the path of the quick access bar. The items of the quick access
bar are fixed. Currently, there are the following items:

-  :cpp:enumerator:`LV_FILE_EXPLORER_QA_HOME`
-  :cpp:enumerator:`LV_FILE_EXPLORER_QA_MUSIC`
-  :cpp:enumerator:`LV_FILE_EXPLORER_QA_PICTURES`
-  :cpp:enumerator:`LV_FILE_EXPLORER_QA_VIDEO`
-  :cpp:enumerator:`LV_FILE_EXPLORER_QA_DOCS`
-  :cpp:enumerator:`LV_FILE_EXPLORER_QA_MNT`
-  :cpp:enumerator:`LV_FILE_EXPLORER_QA_FS`

.. _file_explorer_sort:

Sort
~~~~

You can use
:cpp:expr:`lv_file_explorer_set_sort(file_explorer, LV_EXPLORER_SORT_XX)` to set
sorting method.

There are the following sorting methods:

-  :cpp:enumerator:`LV_EXPLORER_SORT_NONE`
-  :cpp:enumerator:`LV_EXPLORER_SORT_KIND`

You can customize the sorting. Before custom sort, please set the
default sorting to :cpp:enumerator:`LV_EXPLORER_SORT_NONE`. The default is
:cpp:enumerator:`LV_EXPLORER_SORT_NONE`.

.. _file_explorer_events:

Events
------

-  :cpp:enumerator:`LV_EVENT_READY` Sent when a directory is opened. You can customize
   the sort.
-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when an item (file) in the file list
   is clicked.

You can use :cpp:func:`lv_file_explorer_get_cur_path` to get the current path
and :cpp:func:`lv_file_explorer_get_sel_fn` to get the name of the currently
selected file in the event processing function. For example:

.. code-block:: c

   static void file_explorer_event_handler(lv_event_t * e)
   {
       lv_event_code_t code = lv_event_get_code(e);
       lv_obj_t * obj = lv_event_get_target(e);

       if(code == LV_EVENT_VALUE_CHANGED) {
           char * cur_path =  lv_file_explorer_get_cur_path(widget);
           char * sel_fn = lv_file_explorer_get_sel_fn(widget);
           LV_LOG_USER("%s%s", cur_path, sel_fn);
       }
   }

You can also save the obtained **path** and **file** name into an array
through functions such as :cpp:func:`strcpy` and :cpp:func:`strcat` for later use.

.. _file_explorer_example:

Example
-------

.. include:: ../../examples/others/file_explorer/index.rst

.. _file_explorer_api:

API
---

