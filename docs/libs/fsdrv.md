
# File System Interfaces

LVGL has a [File system](https://docs.lvgl.io/master/overview/file-system.html) module to provide an abstraction layer for various file system drivers.
You still need to provide the drivers and libraries, this extension provides only the bridge between FATFS, LittleFS, STDIO, POSIX, WIN32 and LVGL.

## Built in wrappers

### FATFS

Bridge for [FatFS](http://elm-chan.org/fsw/ff/00index_e.html). FatFS itself is not part of LVGL, but can be added and initialized externally.


### LittleFS

Though `lv_fs_littlefs` uses [LittleFS]((https://github.com/littlefs-project/littlefs)) API, the LittleFS library needs other external libraries that handle the mounting of partitions and low-level accesses, according to the given architecture. The functions for the latter are given to the lfs_t structure as pointers by an external low-level library.

There's a convenience function called `lv_fs_littlefs_set_driver(LV_FS_LITTLEFS_LETTER, my_lfs)`, specific to `lv_fs_littlefs`, to attach a `lfs_t` object's pointer to a registered driver-letter. See its comments for more info.


[esp_littlefs](https://components.espressif.com/components/joltwallet/littlefs) is a wrapper for LittleFS to be used in Espressif ESP-devices. It handles the mounting and has the low-level `littlefs_api` functions to read/write/erase blocks that LittleFS library needs. On mounting by `esp_littlefs` the `lfs_t` structures are created. You need to get a handle to these to use ESP with `lv_fs_littlefs`, as all functions use that `lfs_t` in LittleFS to identify the mounted partition. 


In case you don't find a special function in the `lv_fs_littlefs` wrapper, you can look for it in the `esp_littlefs` API and use it directly, as `lv_fs_littlefs` and the `esp_littlefs` APIs can be used side-by-side. 

### STDIO

Bride to C standard functions on Linux and Windows. For example `fopen`, `fread`, etc.

### POSIX 

Bride to POSIX functions on Linux and Windows. For example `open`, `read`, etc.

### WIN32 

Bride to Win32 API function. For example `CreateFileA`, `ReadFile`, etc.

## Usage

In `lv_conf.h` enable `LV_USE_FS_...` and assign an upper cased letter to `LV_FS_..._LETTER` (e.g. `'S'`).
After that you can access files using that driver letter. E.g. `"S:path/to/file.txt"`.

The work directory can be set with `LV_FS_..._PATH`. E.g. `"/home/joe/projects/"` The actual file/directory paths will be appended to it.

Cached reading is also supported if `LV_FS_..._CACHE_SIZE` is set to not `0` value. `lv_fs_read` caches this size of data to lower the number of actual reads from the storage.
