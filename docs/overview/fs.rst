.. _overview_file_system:

===========
File system
===========

LVGL has a 'File system' abstraction module that enables you to attach
any type of file system. A file system is identified by an assigned
drive letter. For example, if an SD card is associated with the letter
``'S'``, a file can be reached using ``"S:path/to/file.txt"``.

.. note::

	If you want to skip the drive prefix from the path, you can use the :c:macro:`LV_FS_DEFAULT_DRIVE_LETTER` config parameter.

Ready to use drivers
********************

LVGL contains prepared drivers for the API of POSIX, standard C,
Windows, and `FATFS <http://elm-chan.org/fsw/ff/00index_e.html>`__.
Learn more :ref:`here <libs_filesystem>`.

Adding a driver
***************

Registering a driver
--------------------

To add a driver, a :cpp:type:`lv_fs_drv_t` needs to be initialized like below.
The :cpp:type:`lv_fs_drv_t` needs to be static, global or dynamically allocated
and not a local variable.

.. code:: c

   static lv_fs_drv_t drv;                   /*Needs to be static or global*/
   lv_fs_drv_init(&drv);                     /*Basic initialization*/

   drv.letter = 'S';                         /*An uppercase letter to identify the drive */
   drv.cache_size = my_cache_size;           /*Cache size for reading in bytes. 0 to not cache.*/

   drv.ready_cb = my_ready_cb;               /*Callback to tell if the drive is ready to use */
   drv.open_cb = my_open_cb;                 /*Callback to open a file */
   drv.close_cb = my_close_cb;               /*Callback to close a file */
   drv.read_cb = my_read_cb;                 /*Callback to read a file */
   drv.write_cb = my_write_cb;               /*Callback to write a file */
   drv.seek_cb = my_seek_cb;                 /*Callback to seek in a file (Move cursor) */
   drv.tell_cb = my_tell_cb;                 /*Callback to tell the cursor position  */

   drv.dir_open_cb = my_dir_open_cb;         /*Callback to open directory to read its content */
   drv.dir_read_cb = my_dir_read_cb;         /*Callback to read a directory's content */
   drv.dir_close_cb = my_dir_close_cb;       /*Callback to close a directory */

   drv.user_data = my_user_data;             /*Any custom data if required*/

   lv_fs_drv_register(&drv);                 /*Finally register the drive*/

Any of the callbacks can be ``NULL`` to indicate that operation is not
supported.

Implementing the callbacks
--------------------------

Open callback
^^^^^^^^^^^^^

The prototype of ``open_cb`` looks like this:

.. code:: c

   void * (*open_cb)(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);

``path`` is the path after the drive letter (e.g. "S:path/to/file.txt" -> "path/to/file.txt").
``mode`` can be :cpp:enumerator:`LV_FS_MODE_WR` or :cpp:enumerator:`LV_FS_MODE_RD` to open for writes or reads.

The return value is a pointer to a *file object* that describes the
opened file or ``NULL`` if there were any issues (e.g. the file wasn't
found). The returned file object will be passed to other file system
related callbacks. (see below)

Other callbacks
---------------

The other callbacks are quite similar. For example ``write_cb`` looks
like this:

.. code:: c

   lv_fs_res_t (*write_cb)(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);

For ``file_p``, LVGL passes the return value of ``open_cb``, ``buf`` is
the data to write, ``btw`` is the Bytes To Write, ``bw`` is the actually
written bytes.

For a template of these callbacks see
`lv_fs_template.c <https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_fs_template.c>`__.

Usage example
*************

The example below shows how to read from a file:

.. code:: c

   lv_fs_file_t f;
   lv_fs_res_t res;
   res = lv_fs_open(&f, "S:folder/file.txt", LV_FS_MODE_RD);
   if(res != LV_FS_RES_OK) my_error_handling();

   uint32_t read_num;
   uint8_t buf[8];
   res = lv_fs_read(&f, buf, 8, &read_num);
   if(res != LV_FS_RES_OK || read_num != 8) my_error_handling();

   lv_fs_close(&f);

The mode in :cpp:func:`lv_fs_open` can be :cpp:enumerator:`LV_FS_MODE_WR` to open for writes
only or :cpp:enumerator:`LV_FS_MODE_RD` ``|`` :cpp:enumerator:`LV_FS_MODE_WR` for both

This example shows how to read a directory's content. It's up to the
driver how to mark directories in the result but it can be a good
practice to insert a ``'/'`` in front of each directory name.

.. code:: c

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

       /*fn is empty, if not more files to read*/
       if(strlen(fn) == 0) {
           break;
       }

       printf("%s\n", fn);
   }

   lv_fs_dir_close(&dir);

Use drives for images
*********************

:ref:`Image <lv_image>` objects can be opened from files too (besides
variables stored in the compiled program).

To use files in image widgets the following callbacks are required:

- open
- close
- read
- seek
- tell

.. _overview_file_system_cache:

Optional file buffering/caching
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

``lv_fs_read`` :sub:`(behavior when the cache is enabled)`
-------------------------------------------------

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

``lv_fs_write`` :sub:`(behavior when the cache is enabled)`
--------------------------------------------------

The part of the cache that coincides with the written content
will be updated to reflect the written content.

``lv_fs_seek`` :sub:`(behavior when the cache is enabled)`
-------------------------------------------------

The driver's ``seek`` will not actually be called unless the ``whence``
is ``LV_FS_SEEK_END``, in which case ``seek`` and ``tell`` will be called
to determine where the end of the file is.

``lv_fs_tell`` :sub:`(behavior when the cache is enabled)`
-------------------------------------------------

The driver's ``tell`` will not actually be called.

.. _overview_file_system_api:

API
***
