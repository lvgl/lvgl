
# File System Interfaces

LVGL has a [File system](https://docs.lvgl.io/master/overview/file-system.html) module to provide an abstraction layer for various file system drivers.
You still need to provide the drivers and libraries, this extension provides only the bridge between FATFS, STDIO, POSIX, WIN32 and LVGL.

## LVGL has built in support for:

### [FATFS](http://elm-chan.org/fsw/ff/00index_e.html)

The FAT-filesystem should be initialized externally.

### [LittleFS](https://github.com/littlefs-project/littlefs)

Though lv_fs_littlefs uses LittleFS API, the LittleFS library needs other external libraries that handle the mounting of partitions and low-level accesses, according to the given architecture. The functions for the latter are given to the lfs_t structure as pointers by an external low-level library.
(One example of this library is esp_littlefs library for Espressif ESP-devices. That library handles the mounting and has the littlefs_api functions to read/write/erase blocks that LittleFS library needs. On mounting by esp_littlefs the lfs_t structures are created. You need to get a handle to these to use ESP with lv_fs_littlefs, as every function uses that lfs_t in LittleFS to identify the mounted partition. In case you don't find a special function in lv_fs_littlefs wrapper, you can look for it in esp_littlefs API, and the lv_fs_littlefs and esp_littlefs APIs can be used side-by-side. Also there's a convenience function called lv_fs_littlefs_set_driver(), specific to lv_fs_littlefs, to attach a lfs_t object's pointer to a registered driver-letter. See its comments for more info. )

### STDIO (Linux and Windows using C standard function .e.g fopen, fread)

### POSIX (Linux and Windows using POSIX function .e.g open, read)

### WIN32 (Windows using Win32 API function .e.g CreateFileA, ReadFile)

## Usage

In `lv_conf.h` enable `LV_USE_FS_...` and assign an upper cased letter to `LV_FS_..._LETTER` (e.g. `'S'`).
After that you can access files using that driver letter. E.g. `"S:path/to/file.txt"`.

The work directory can be set with `LV_FS_..._PATH`. E.g. `"/home/joe/projects/"` The actual file/directory paths will be appended to it.

Cached reading is also supported if `LV_FS_..._CACHE_SIZE` is set to not `0` value. `lv_fs_read` caches this size of data to lower the number of actual reads from the storage.
