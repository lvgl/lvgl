.. _file_system:

=======================
File System (lv_fs_drv)
=======================

LVGL has a "File system" abstraction module that enables you to attach
any type of file system.  A file system is identified by an assigned
identifier letter.  For example, if an SD card is associated with the letter
``'S'``, a file can be reached using ``"S:/path/to/file.txt"``.  See details
under :ref:`lv_fs_identifier_letters`.

.. note::

    If you want to skip the drive-letter prefix in Unix-like paths, you can use the
    :c:macro:`LV_FS_DEFAULT_DRIVER_LETTER` config parameter.



Ready-to-Use Drivers
********************

LVGL contains prepared drivers for the API of POSIX, standard C,
Windows, and `FATFS <http://elm-chan.org/fsw/ff/00index_e.html>`__.
Learn more :ref:`here <libs_filesystem>`.



.. _lv_fs_identifier_letters:

Identifier Letters
******************

As mentioned above, a file system is identified by an assigned identifier letter.
This identifier is merely a way for the LVGL File System abtraction logic to look up
the appropriate registered file-system driver for a given path.

**How it Works:**

You register a driver for your file system and assign it an identifier letter.  This
letter must be unique among all registered file-system drivers, and in the range [A-Z]
or the character '/'.  See :ref:`lv_fs_adding_a_driver` for how this is done.

Later, when using paths to files on your file system, you prefix the path with that
identifier character plus a colon (':').

.. note::

    Do not confuse this with a Windows or DOS drive letter.

**Example:**

Let's use the letter 'Z' as the identifier character, and "path_to_file" as the path,
then the path strings you pass to ``lv_fs_...()`` functions would look like this::

    "Z:path_to_file"
     ^ ^^^^^^^^^^^^
     |        |
     |        +-- This part gets passed to the OS-level file-system functions.
     |
     +-- This part LVGL strips from path string, and uses it to find the appropriate
         driver (i.e. set of functions) that apply to that file system.

Note also that the path can be a relative path or a "rooted path" (beginning with
``/``), though rooted paths are recommended since the driver does not yet provide a
way to set the default directory.

**Examples for Unix-like file systems:**

- "Z:/etc/images/splash.png"
- "Z:/etc/images/left_button.png"
- "Z:/etc/images/right_button.png"
- "Z:/home/users/me/wip/proposal.txt"

**Examples for Windows/DOS-like file systems:**

- "Z:C:/Users/me/wip/proposal.txt"
- "Z:/Users/me/wip/proposal.txt"  (if the default drive is known to be C:)
- "Z:C:/Users/Public/Documents/meeting_notes.txt"
- "Z:D:/to_print.docx"

Reminder:  Note carefully that the prefixed "Z:" has nothing to do with the "C:" and
"D:" Windows/DOS drive letters in 3 of the above examples, which are part of the path.
"Z:" is used to look up the driver for that file system in the list of all file-system
drivers registered with LVGL.



.. _lv_fs_adding_a_driver:

Adding a Driver
***************

Registering a driver
--------------------

To add a driver, a :cpp:type:`lv_fs_drv_t` object needs to be initialized and
registered in a way similar to the code below.  The :cpp:type:`lv_fs_drv_t` variable
needs to be static, global or dynamically allocated and not a local variable, since
its contents need to remain valid as long as the driver is in use.

.. code-block:: c

   static lv_fs_drv_t drv;                   /* Needs to be static or global */
   lv_fs_drv_init(&drv);                     /* Basic initialization */

   drv.letter = 'S';                         /* An uppercase letter to identify the drive */
   drv.cache_size = my_cache_size;           /* Cache size for reading in bytes. 0 to not cache. */

   drv.ready_cb = my_ready_cb;               /* Callback to tell if the drive is ready to use */
   drv.open_cb = my_open_cb;                 /* Callback to open a file */
   drv.close_cb = my_close_cb;               /* Callback to close a file */
   drv.read_cb = my_read_cb;                 /* Callback to read a file */
   drv.write_cb = my_write_cb;               /* Callback to write a file */
   drv.seek_cb = my_seek_cb;                 /* Callback to seek in a file (Move cursor) */
   drv.tell_cb = my_tell_cb;                 /* Callback to tell the cursor position  */

   drv.dir_open_cb = my_dir_open_cb;         /* Callback to open directory to read its content */
   drv.dir_read_cb = my_dir_read_cb;         /* Callback to read a directory's content */
   drv.dir_close_cb = my_dir_close_cb;       /* Callback to close a directory */

   drv.user_data = my_user_data;             /* Any custom data if required */

   lv_fs_drv_register(&drv);                 /* Finally register the drive */

Any of the callbacks can be ``NULL`` to indicate that operation is not
supported.

Implementing the callbacks
--------------------------

Open callback
~~~~~~~~~~~~~

The prototype of ``open_cb`` looks like this:

.. code-block:: c

   void * (*open_cb)(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);

``path`` is the path after the drive letter (e.g. "S:path/to/file.txt" -> "path/to/file.txt").
``mode`` can be :cpp:enumerator:`LV_FS_MODE_WR` or :cpp:enumerator:`LV_FS_MODE_RD` to open for writes or reads.

The return value is a pointer to a *file object* that describes the
opened file or ``NULL`` if there were any issues (e.g. the file wasn't
found). The returned file object will be passed to other file system
related callbacks. (See below.)

Other callbacks
---------------

The other callbacks are quite similar. For example ``write_cb`` looks
like this:

.. code-block:: c

   lv_fs_res_t (*write_cb)(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);

For ``file_p``, LVGL passes the return value of ``open_cb``, ``buf`` is
the data to write, ``btw`` is the number of "bytes to write", ``bw`` is the number of
"bytes written" (written to during the function call).

For a list of prototypes for these callbacks see
`lv_fs_template.c <https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_fs_template.c>`__.
This file also provides a template for new file-system drivers you can use if the
one you need is not already provided.

Drivers that come with LVGL
---------------------------

As of this writing, the list of already-available file-system drivers can be enabled
by setting one or more of the following macros to a non-zero value in ``lv_conf.h``.
The drivers are as implied by the macro names.

If you use more than one, each associated identifier letter you use must be unique.

- :c:macro:`LV_USE_FS_FATFS`
- :c:macro:`LV_USE_FS_STDIO`
- :c:macro:`LV_USE_FS_POSIX`
- :c:macro:`LV_USE_FS_WIN32`
- :c:macro:`LV_USE_FS_MEMFS`
- :c:macro:`LV_USE_FS_LITTLEFS`
- :c:macro:`LV_USE_FS_ARDUINO_ESP_LITTLEFS`
- :c:macro:`LV_USE_FS_ARDUINO_SD`



Limiting Directory Access
*************************

If you are using one of the following file-system drivers:

- :c:macro:`LV_USE_FS_STDIO`
- :c:macro:`LV_USE_FS_POSIX`
- :c:macro:`LV_USE_FS_WIN32`

you will have a ``LV_FS_xxx_PATH`` macro available to you in ``lv_conf.h`` that you
can use to provide a path that gets dynamically prefixed to the ``path_to_file``
portion of of the path strings provided to ``lv_fs_...()`` functions when files and
directories are opened.  This can be useful to limit directory access (e.g. when a
portion of a path can be typed by an end user), or simply to reduce the length of the
path strings provided to ``lv_fs_...()`` functions.

Do this by filling in the full path to the directory you wish his access to be
limited to in the applicable ``LV_FS_xxx_PATH`` macro in ``lv_conf.h``.  Do not
prefix the path with the driver-identifier letter, and do append a directory
separator character at the end.

**Examples for Unix-like file systems:**

.. code-block:: c

    #define LV_FS_WIN32_PATH   "/home/users/me/"

**Examples for Windows/DOS-like file systems:**

.. code-block:: c

    #define LV_FS_WIN32_PATH   "C:/Users/me/"

Then in both cases, path strings passed to ``lv_fs_...()`` functions in the
application get reduced to:

- "Z:wip/proposal.txt"



Usage Example
*************

The example below shows how to read from a file:

.. code-block:: c

   lv_fs_file_t f;
   lv_fs_res_t res;
   res = lv_fs_open(&f, "S:folder/file.txt", LV_FS_MODE_RD);
   if(res != LV_FS_RES_OK) my_error_handling();

   uint32_t read_num;
   uint8_t buf[8];
   res = lv_fs_read(&f, buf, 8, &read_num);
   if(res != LV_FS_RES_OK || read_num != 8) my_error_handling();

   lv_fs_close(&f);

The mode in :cpp:func:`lv_fs_open` can be :cpp:enumerator:`LV_FS_MODE_WR` to open for
writes only, :cpp:enumerator:`LV_FS_MODE_RD` for reads only, or
:cpp:enumerator:`LV_FS_MODE_RD` ``|`` :cpp:enumerator:`LV_FS_MODE_WR` for both.

This example shows how to read a directory's content. It's up to the
driver how to mark directories in the result but it can be a good
practice to insert a ``'/'`` in front of each directory name.

.. code-block:: c

   lv_fs_dir_t dir;
   lv_fs_res_t res;
   res = lv_fs_dir_open(&dir, "S:/folder");
   if(res != LV_FS_RES_OK) my_error_handling();

   char fn[256];
   while(1) {
       res = lv_fs_dir_read(&dir, fn, sizeof(fn));
       if(res != LV_FS_RES_OK) {
           my_error_handling();
           break;
       }

       /* fn is empty if there are no more files to read. */
       if(strlen(fn) == 0) {
           break;
       }

       printf("%s\n", fn);
   }

   lv_fs_dir_close(&dir);



Use Drives for Images
*********************

:ref:`Image <lv_image>` Widgets can be opened from files as well (besides
variables stored in the compiled program).

To use files in Image Widgets the following callbacks are required:

- open
- close
- read
- seek
- tell



.. _file_system_cache:

Optional File Buffering/Caching
*******************************

Files will buffer their reads if the corresponding ``LV_FS_*_CACHE_SIZE``
config option is set to a value greater than zero. Each open file will
buffer up to that many bytes to reduce the number of FS driver calls.

Generally speaking, file buffering can be optimized for different kinds
of access patterns. The one implemented here is optimal for reading large
files in chunks, which is what the image decoder does.
It has the potential to call the driver's ``read`` fewer
times than ``lv_fs_read`` is called. In the best case where the cache size is
\>= the size of the file, ``read`` will only be called once. This strategy is good
for linear reading of large files but less helpful for short random reads across a file bigger than the buffer
since data will be buffered that will be discarded after the next seek and read.
The cache should be sufficiently large or disabled in that case. Another case where the cache should be disabled
is if the file contents are expected to change by an external factor like with special OS files.

The implementation is documented below. Note that the FS functions make calls
to other driver FS functions when the cache is enabled. i.e., ``lv_fs_read`` may call the driver's ``seek``
so the driver needs to implement more callbacks when the cache is enabled.

``lv_fs_read`` :sub:`(behavior when cache is enabled)`
------------------------------------------------------

.. mermaid::
   :zoom:

   %%{init: {'theme':'neutral'}}%%
   flowchart LR
       A["call lv_fs_read and
          the cache is enabled"] --> B{{"is there cached data
                                         at the file position?"}}
       B -->|yes| C{{"does the cache have
                      all required bytes available?"}}
       C -->|yes| D["copy all required bytes from
                     the cache to the destination
                     buffer"]
       C -->|no| F["copy the available
                    required bytes
                    until the end of the cache
                    into the destination buffer"]
             --> G["seek the real file to the end
                    of what the cache had available"]
             --> H{{"is the number of remaining bytes
                     larger than the size of the whole cache?"}}
       H -->|yes| I["read the remaining bytes
                     from the real file to the
                     destination buffer"]
       H -->|no| J["eagerly read the real file
                    to fill the whole cache
                    or as many bytes as the
                    read call can"]
             --> O["copy the required bytes
                    to the destination buffer"]
       B -->|no| K["seek the real file to
                    the file position"]
             --> L{{"is the number of required
                     bytes greater than the
                     size of the entire cache?"}}
       L -->|yes| M["read the real file to
                     the destination buffer"]
       L -->|no| N["eagerly read the real file
                    to fill the whole cache
                    or as many bytes as the
                    read call can"]
             --> P["copy the required bytes
                    to the destination buffer"]

``lv_fs_write`` :sub:`(behavior when cache is enabled)`
-------------------------------------------------------

The part of the cache that coincides with the written content
will be updated to reflect the written content.

``lv_fs_seek`` :sub:`(behavior when cache is enabled)`
------------------------------------------------------

The driver's ``seek`` will not actually be called unless the ``whence``
is ``LV_FS_SEEK_END``, in which case ``seek`` and ``tell`` will be called
to determine where the end of the file is.

``lv_fs_tell`` :sub:`(behavior when cache is enabled)`
------------------------------------------------------

The driver's ``tell`` will not actually be called.



.. _file_system_api:

API
***
