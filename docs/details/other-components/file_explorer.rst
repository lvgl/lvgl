.. _file_explorer:

=============
File Explorer
=============

``lv_file_explorer`` provides a UI enabling the end user to browse the contents of a
file system.  Its main area is called the "Browsing Area" and provides the list of
files contained in the currently-viewed directory.

When enabled, there is also a "Quick-Access" panel on the left, which provides a
convenient way to reach parts of the file system that are frequently accessed.
Available "Quick-Access" destinations are:

- File System,
- HOME,
- Video,
- Pictures,
- Music, and
- Documents.

You specify what paths these lead to during ``lv_file_explorer``\ 's initialization.

``lv_file_explorer`` only provides the file browsing and events caused by user
activity (e.g. clicking a file), but does not provide the actual file operations.
Client code must hook various events and decide what to do when they are emitted
(e.g. a click or double-click on a file).  The actions taken might to open the file,
display it, send it to some other part of the application, etc..
``lv_file_explorer`` passes the full path and name of file that was clicked to the
event callback functions.  What happens next is up to the application designer.

``lv_file_explorer`` uses the :ref:`lv_table` Widget for the "Browsing Area", and the
:ref:`lv_list` Widget for the "Quick-Access" panel when it is enabled.  Thus,
:c:macro:`LV_USE_TABLE` macro must be set to a non-zero value in ``lv_conf.h`` in
order to use ``lv_file_explorer``, and and :c:macro:`LV_USE_LIST` must be set to a
non-zero value to use the "Quick-Access" panel.

.. note::

    In order to use File Explorer, :ref:`file_system` has to be set up and
    know about all the drive letters you use when passing paths to File System
    (described below).



Prerequisites
*************

If you haven't already done so, you will need to learn about the LVGL :ref:`File
System abstraction <file_system>`, since it must be set up and be functional
for File Explorer to work.



.. _file_explorer_usage:

Usage
*****

Set :c:macro:`LV_USE_FILE_EXPLORER` to a non-zero value in ``lv_conf.h``.

First use :cpp:expr:`lv_file_explorer_create(lv_screen_active())` to create a File
Explorer.  The default size is the screen size.  After that, you can
customize the style like any Widget.

The size of the ``current_path`` buffer is set by :c:macro:`LV_FILE_EXPLORER_PATH_MAX_LEN`
in ``lv_conf.h``.

The object hierarchy of a freshly-created File Explorer looks like this:

- ``File Explorer``:  occupies full area of parent Widget, typically a Screen (Flex-Flow COLUMN)

  - ``Container``:  occupies full area of File Explorer (Flex grow 1)

    - ``Quick-Access Panel``:

      - ``Device List``:  grows to accommodate children

        - ``File System``:  button

      - ``Places List``:  grows to accommodate children

        - ``HOME``:  button
        - ``Video``:  button
        - ``Pictures``:  button
        - ``Music``:  button
        - ``Documents``:  button

    - ``Browser Panel``:

      - ``Header``:  14% of ``Browser Panel`` height

        - ``Current Path``:  label

      - ``File Table``:  with 1 column, 86% of ``Browser Panel`` height

  - Fields:

    - ``home_dir`` = NULL
    - ``video_dir`` = NULL
    - ``pictures_dir`` = NULL
    - ``music_dir`` = NULL
    - ``docs_dir`` = NULL
    - ``fs_dir`` = NULL
    - ``current_path`` = [empty buffer]
    - ``sel_fn`` (selected file)
    - ``sort`` (default :cpp:enumerator:`LV_EXPLORER_SORT_NONE`)


Accessing the Parts
-------------------

This list of functions provides access to the parts shown in diagram above:

- :cpp:expr:`lv_file_explorer_get_selected_file_name(explorer)`  (pointer
  to NUL-terminated string containing file-path user selected; typically used inside
  an :cpp:enumerator:`LV_EVENT_CLICKED` event)
- :cpp:expr:`lv_file_explorer_get_current_path(explorer)` (pointer to ``current_path`` ``char`` buffer)
- :cpp:expr:`lv_file_explorer_get_file_table(explorer)`  (pointer to ``File Table`` :ref:`lv_table` Widget)
- :cpp:expr:`lv_file_explorer_get_header(explorer)`  (pointer to ``Header`` :ref:`base_widget` Widget)
- :cpp:expr:`lv_file_explorer_get_path_label(explorer)`  (pointer to ``Current Path Label`` :ref:`lv_label` Widget)
- :cpp:expr:`lv_file_explorer_get_quick_access_area(explorer)`  (pointer to ``Quick-Access Panel`` :ref:`base_widget`)
- :cpp:expr:`lv_file_explorer_get_places_list(explorer)`    (pointer to ``Places List`` :ref:`lv_list` Widget)
- :cpp:expr:`lv_file_explorer_get_device_list(explorer)`    (pointer to ``Device List`` :ref:`lv_list` Widget)


Quick-Access Panel
------------------

The ``Quick-Access Panel`` behaves like a typical navigation panel and appears on the
left, while the ``Browser Panel`` appears on the right

This panel is optional.  If you set :c:macro:`LV_FILE_EXPLORER_QUICK_ACCESS` to ``0``
in ``lv_conf.h``, the ``Quick-Access Panel`` will not be created.  This saves only a
little bit of memory.

Soon after the File Explorer is created, you typically use
:cpp:expr:`lv_file_explorer_set_quick_access_path(explorer, LV_EXPLORER_XXX_DIR, "path")`
to set the path that will be navigated to when the buttons in the ``Quick-Access Panel``
are clicked, which is currently a fixed list.  The corresponding values you will need
to pass as the 2nd argument are the following:

-  :cpp:enumerator:`LV_EXPLORER_HOME_DIR`
-  :cpp:enumerator:`LV_EXPLORER_MUSIC_DIR`
-  :cpp:enumerator:`LV_EXPLORER_PICTURES_DIR`
-  :cpp:enumerator:`LV_EXPLORER_VIDEO_DIR`
-  :cpp:enumerator:`LV_EXPLORER_DOCS_DIR`
-  :cpp:enumerator:`LV_EXPLORER_FS_DIR`


.. _file_explorer_sort:

Sort
----

You can use
:cpp:expr:`lv_file_explorer_set_sort(explorer, LV_EXPLORER_SORT_XX)` to set
the sorting method.

These are the possible sorting methods:

-  :cpp:enumerator:`LV_EXPLORER_SORT_NONE` (default)
-  :cpp:enumerator:`LV_EXPLORER_SORT_KIND`

:cpp:expr:`lv_file_explorer_get_sort(explorer)` returns the current sorting method.



.. _file_explorer_events:

Events
******

- :cpp:enumerator:`LV_EVENT_READY` Sent when a directory is opened, which can happen:

  - when the File Explorer is initially opened,
  - after a user clicks on a ``Quick-Access Panel`` navigation button, and
  - after the user clicks on a directory displayed in the ``Browser Panel``.

  You can use it to, for example, customize the file sort.

- :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent once when any item (file) in the
  ``Brwoser Panel``\ 's file list is clicked.

- :cpp:enumerator:`LV_EVENT_CLICKED` Sent twice when an item in the ``Browser Panel``
  is clicked:  once as a result of the input-device :cpp:enumerator:`LV_EVENT_RELEASED`
  event and a second as a result of the input device :cpp:enumerator:`LV_EVENT_CLICKED`
  event.  This applies to files, directories, and the "< Back" item in the ``Browser Panel``.

In these events you can use :cpp:func:`lv_file_explorer_get_current_path` to get the
current path and :cpp:func:`lv_file_explorer_get_selected_file_name` to get the name
of the currently selected file in the event processing function. For example:

.. code-block:: c

   static void file_explorer_event_handler(lv_event_t * e)
   {
       lv_event_code_t code = lv_event_get_code(e);
       lv_obj_t * obj = lv_event_get_target(e);

       if(code == LV_EVENT_VALUE_CHANGED) {
           char * cur_path =  lv_file_explorer_get_current_path(widget);
           char * sel_fn = lv_file_explorer_get_selected_file_name(widget);
           LV_LOG_USER("%s%s", cur_path, sel_fn);
       }
   }

You can also save the obtained **path** and **file** name into an array
through functions such as :cpp:func:`strcpy` and :cpp:func:`strcat` for later use.



.. _file_explorer_example:

Example
*******

.. include:: ../../examples/others/file_explorer/index.rst



.. _file_explorer_api:

API
***

