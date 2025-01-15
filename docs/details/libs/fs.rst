.. _libs_filesystem:

======================
File System Interfaces
======================

LVGL has a :ref:`overview_file_system` module
to provide an abstraction layer for various file system drivers.

LVG has built in support for:

- `FATFS <http://elm-chan.org/fsw/ff/00index_e.html>`__
- STDIO (Linux and Windows using C standard function .e.g ``fopen``, ``fread``)
- POSIX (Linux and Windows using POSIX function .e.g ``open``, ``read``)
- WIN32 (Windows using Win32 API function .e.g ``CreateFileA``, ``ReadFile``)
- MEMFS (read a file from a memory buffer)
- LITTLEFS (a little fail-safe filesystem designed for microcontrollers)
- Arduino ESP LITTLEFS (a little fail-safe filesystem designed for Arduino ESP)
- Arduino SD (allows for reading from and writing to SD cards)

You still need to provide the drivers and libraries, this extension
provides only the bridge between FATFS, STDIO, POSIX, WIN32 and LVGL.

.. _libs_filesystem_usage:

Usage
*****

In ``lv_conf.h`` enable ``LV_USE_FS_...`` and assign an upper cased
letter to ``LV_FS_..._LETTER`` (e.g. ``'S'``). After that you can access
files using that driver letter. E.g. ``"S:path/to/file.txt"``.

Working with common prefixes
""""""""""""""""""""""""""""

A **default driver letter** can be set by ``LV_FS_DEFAULT_DRIVER_LETTER``,
which allows skipping the drive prefix in file paths.

For example if ``LV_FS_DEFAULT_DRIVER_LETTER`` is set the ``'S'`` *"path/to/file.txt"* will mean *"S:path/to/file.txt"*.

This feature is useful if you have only a single driver and don't want to bother with LVGL's driver layer in the file paths.
It also helps to use a unified path with LVGL's file system and normal file systems.
The original mechanism is not affected, so a path starting with drive letter will still work.

The **working directory** can be set with ``LV_FS_..._PATH``. E.g.
``"/home/joe/projects/"`` The actual file/directory paths will be
appended to it, allowing to skip the common part.

Caching
"""""""

:ref:`Cached reading <overview_file_system_cache>` is also supported if ``LV_FS_..._CACHE_SIZE`` is set to
not ``0`` value. :cpp:func:`lv_fs_read` caches this size of data to lower the
number of actual reads from the storage.

To use the memory-mapped file emulation an ``lv_fs_path_ex_t`` object must be
created and initialized. This object can be passed to :cpp:func:`lv_fs_open` as
the file name:

.. code-block:: c

  lv_fs_path_ex_t mempath;
  lv_fs_file_t file;
  uint8_t *buffer;
  uint32_t size;

  /* Initialize buffer */
  ...

  lv_fs_make_path_from_buffer(&mempath, LV_FS_MEMFS_LETTER, (void*)buffer, size);
  lv_fs_res_t res = lv_fs_open(&file, (const char *)&mempath, LV_FS_MODE_RD);

.. _libs_filesystem_api:

API
***

