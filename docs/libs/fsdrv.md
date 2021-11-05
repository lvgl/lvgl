```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/fsdrv.md
```

# File System Interfaces

LVGL has a [File system](https://docs.lvgl.io/master/overview/file-system.html) module to provides an abstraction layer for various file system drivers.

LVG has build in support for
- [FATFS](http://elm-chan.org/fsw/ff/00index_e.html)
- STDIO (Linux and Windows using C standard function .e.g fopen, fread)
- POSIX (Linux and Windows using POSIX function .e.g open, read)
- WIN32 (Windows using Win32 API function .e.g CreateFileA, ReadFile)

You still need to provide the drivers and libraries, this extensions provide only the bridge between FATFS, STDIO, POSIX, WIN32 and LVGL.

## Usage

In `lv_conf.h` set a driver letter for one or more `LV_FS_USE_...` define(s). After that you can access files using that driver letter. Setting `'\0'` will disable use of that interface.  
