.. _libs_filesystem:

======================
File System Interfaces
======================

LVGL's :ref:`file_system` module provides an abstraction that enables you to attach
any type of file system for LVGL's use.  File systems already supported are:

- `FATFS <http://elm-chan.org/fsw/ff/00index_e.html>`__
- STDIO (Linux and Windows using C standard function .e.g ``fopen``, ``fread``)
- POSIX (Linux and Windows using POSIX function .e.g ``open``, ``read``)
- WIN32 (Windows using Win32 API function .e.g ``CreateFileA``, ``ReadFile``)
- MEMFS (read a file from a memory buffer)
- LITTLEFS (a little fail-safe filesystem designed for microcontrollers)
- Arduino ESP LITTLEFS (a little fail-safe filesystem designed for Arduino ESP)
- Arduino SD (allows for reading from and writing to SD cards)

You still need to provide the drivers and libraries, this extension
provides only the bridge between LVGL and these file systems.



.. _libs_filesystem_usage:

Usage
*****

In ``lv_conf.h`` enable ``LV_USE_FS_...`` (by setting its value to ``1``) and assign
an upper cased letter to ``LV_FS_..._DRIVER_LETTER`` (e.g. ``'S'``).  If more than
one file system is enabled, each driver will need to have a unique driver-identifier
letter.  After that you can access files using that driver letter.  Example with
driver identifier letter ``'S'``:

:Linux-like relative path:    ``"S:path/to/file.txt"``
:Linux-like absolute path:    ``"S:/path/to/file.txt"``
:Windows-like relative path:  ``"S:C:path/to/file.txt"``
:Windows-like absolute path:  ``"S:C:/path/to/file.txt"``

Do not confuse the driver-identifier letter with the Windows/DOS/FAT "drive letter",
which is part of the path passed to the OS-level functions.  For more details, see
:ref:`lv_fs_identifier_letters`.

:ref:`Cached reading <file_system_cache>` is also supported if ``LV_FS_..._CACHE_SIZE`` is set to
a non-zero value.

For further details, including how to create support for your own file system, see
:ref:`file_system`.

