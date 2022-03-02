```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/overview/file-system.md
```
# File system

LVGL has a 'File system' abstraction module that enables you to attach any type of file system.
A file system is identified by an assigned drive letter.
For example, if an SD card is associated with the letter `'S'`, a file can be reached using `"S:path/to/file.txt"`.

## Ready to use drivers
The [lv_fs_if](https://github.com/lvgl/lv_fs_if) repository contains prepared drivers using POSIX, standard C and the [FATFS](http://elm-chan.org/fsw/ff/00index_e.html) API.
See its [README](https://github.com/lvgl/lv_fs_if#readme) for the details.

## Adding a driver

### Registering a driver
To add a driver, a `lv_fs_drv_t` needs to be initialized like below. The `lv_fs_drv_t` needs to be static, global or dynamically allocated and not a local variable.
```c
static lv_fs_drv_t drv;                   /*Needs to be static or global*/
lv_fs_drv_init(&drv);                     /*Basic initialization*/

drv.letter = 'S';                         /*An uppercase letter to identify the drive */
drv.cache_size = my_cahce_size;           /*Cache size for reading in bytes. 0 to not cache.*/

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

```

Any of the callbacks can be `NULL` to indicate that operation is not supported.


### Implementing the callbacks

#### Open callback
The prototype of `open_cb` looks like this:
```c
void * (*open_cb)(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
```

`path` is the path after the drive letter (e.g. "S:path/to/file.txt" -> "path/to/file.txt"). `mode` can be `LV_FS_MODE_WR` or `LV_FS_MODE_RD` to open for writes or reads.

The return value is a pointer to a *file object* that describes the opened file or `NULL` if there were any issues (e.g. the file wasn't found). 
The returned file object will be passed to other file system related callbacks. (see below)

### Other callbacks
The other callbacks are quite similar. For example `write_cb` looks like this:
```c
lv_fs_res_t (*write_cb)(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
```

For `file_p`, LVGL passes the return value of `open_cb`, `buf` is the data to write, `btw` is the Bytes To Write, `bw` is the actually written bytes. 

For a template of these callbacks see [lv_fs_template.c](https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_fs_template.c).


## Usage example

The example below shows how to read from a file:
```c
lv_fs_file_t f;
lv_fs_res_t res;
res = lv_fs_open(&f, "S:folder/file.txt", LV_FS_MODE_RD);
if(res != LV_FS_RES_OK) my_error_handling();

uint32_t read_num;
uint8_t buf[8];
res = lv_fs_read(&f, buf, 8, &read_num);
if(res != LV_FS_RES_OK || read_num != 8) my_error_handling();

lv_fs_close(&f);
```
*The mode in `lv_fs_open` can be `LV_FS_MODE_WR` to open for writes only or `LV_FS_MODE_RD | LV_FS_MODE_WR` for both*

This example shows how to read a directory's content. It's up to the driver how to mark directories in the result but it can be a good practice to insert a `'/'` in front of each directory name.
```c
lv_fs_dir_t dir;
lv_fs_res_t res;
res = lv_fs_dir_open(&dir, "S:/folder");
if(res != LV_FS_RES_OK) my_error_handling();

char fn[256];
while(1) {
    res = lv_fs_dir_read(&dir, fn);
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
```

## Use drives for images

[Image](/widgets/core/img) objects can be opened from files too (besides variables stored in the compiled program).

To use files in image widgets the following callbacks are required:
- open
- close
- read
- seek
- tell



## API

```eval_rst

.. doxygenfile:: lv_fs.h
  :project: lvgl

```
