
# File System Interfaces

LVGL has a [File system](https://docs.lvgl.io/master/overview/file-system.html) module to provide an abstraction layer for various file system drivers.

LVG has built in support for:
- [FATFS](http://elm-chan.org/fsw/ff/00index_e.html)
- STDIO (Linux and Windows using C standard function .e.g fopen, fread)
- POSIX (Linux and Windows using POSIX function .e.g open, read)
- WIN32 (Windows using Win32 API function .e.g CreateFileA, ReadFile)
- CBFS (Static data embedded in header files as arrays)

You still need to provide the drivers and libraries, this extension provides only the bridge between FATFS, STDIO, POSIX, WIN32 and LVGL.

## Usage

In `lv_conf.h` enable `LV_USE_FS_...` and assign an upper cased letter to `LV_FS_..._LETTER` (e.g. `'S'`).
After that you can access files using that driver letter. E.g. `"S:path/to/file.txt"`.

The work directory can be set with `LV_FS_..._PATH`. E.g. `"/home/joe/projects/"` The actual file/directory paths will be appended to it.

Cached reading is also supported if `LV_FS_..._CACHE_SIZE` is set to not `0` value. `lv_fs_read` caches this size of data to lower the number of actual reads from the storage.

### CBFS Usage

CBFS is a special filesystem emulator that allows you to create a virtual filesystem over arrays of data. This way you can embed JPGs, PNGs, TTFs, and other assets as headers and put them directly into your firmware. You then call lv_fs_cbfs_create(...) and pass it a buffer and a path to fill. It gives you a "path" that can be opened over the array as though it was any other read only file.