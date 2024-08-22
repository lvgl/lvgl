.. _lfs:

==============
littlefs
==============

littlefs is a little fail-safe filesystem designed for microcontrollers.

Detailed introduction: https://github.com/littlefs-project/littlefs


Usage
-----

Enable :c:macro:`LV_USE_FS_LITTLEFS` and define a :c:macro:`LV_FS_LITTLEFS_LETTER` in ``lv_conf.h``.

When enabled :c:macro:`lv_littlefs_set_handler` can be used to set up a mount point.

Example
-------

.. code:: c
    #include "lfs.h"

    // configuration of the filesystem is provided by this struct
    const struct lfs_config cfg = {
        // block device operations
        .read  = user_provided_block_device_read,
        .prog  = user_provided_block_device_prog,
        .erase = user_provided_block_device_erase,
        .sync  = user_provided_block_device_sync,

        // block device configuration
        .read_size = 16,
        .prog_size = 16,
        .block_size = 4096,
        .block_count = 128,
        .cache_size = 16,
        .lookahead_size = 16,
        .block_cycles = 500,
    };

    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }

    lv_littlefs_set_handler(&lfs);


API
---


