# About FrogFS

FrogFS (Fast Read-Only General-purpose File System) is a read-only filesystem
designed for embedded use. It can be easily used with a CMake project &mdash;
including [ESP-IDF](https://github.com/espressif/esp-idf). It has built-in
filters to save space. Files in `frogfs-clockwise-demo` are reduced by about
half using the default filters. Here are links to the example repositories:

  * [frogfs-standalone-demo](https://github.com/jkent/frogfs-standalone-demo)
  * [frogfs-clockwise-demo](https://github.com/jkent/frogfs-clockwise-demo)

Transform filters include:
  * babel-convert
  * babel-minify
  * deflate
  * gzip
  * brotli
  * html-minifier
  * rename
  * terminate
  * uglify-js
  * uglifycss

Compression filters include:
  * none
  * gzip
  * [heatshrink](https://github.com/atomicobject/heatshrink)
  * zlib

Transform filters are intended to be _compile-time_ operations that do not
incur a run-time cost while compression filters are **expected** to incur a
_run-time_ decompression cost.

This means for an HTTP server, zlib (deflate), brotli (br) or gzip compressed
files can be passed through untouched! This saves processing time and bandwidth.
Beware however that some browsers don't accept brotli content other HTTP (only HTTPS).

# Getting started with ESP-IDF

To use this component with ESP-IDF, within your projects directory run

    idf.py add-dependency jkent/frogfs

## Embedding a FrogFS image

Embed FrogFS within your project binary with the folowing CMake function:

    target_add_frogfs(<target> [CONFIG yaml] [NAME name])

If **CONFIG** is not specified, `frogfs.yaml` will be used. If **NAME** is not
specified, it will default to `frogfs`.

As an example for ESP-IDF, in your project's toplevel CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(my_project)

target_add_frogfs(${PROJECT_NAME}.elf)
```

In C, this results in these two global symbols being available to your
application:

```C
extern const uint8_t frogfs_bin[];
extern const size_t frogfs_bin_len;
```

## Making a FrogFS binary and flashing it

You have the option of creating a binary without linking it with your
application. A CMake function is provided to output a binary with target
`generate_${name}`.

    declare_frogfs_bin(path [CONFIG yaml] [NAME name])

If **CONFIG** is not specified, `frogfs.yaml` is used. If **NAME** is not
specifed, `frogfs` is used.

Here's an example of what you can add to your toplevel CMakeLists.txt:

```cmake
set(FROGFS_NAME frogfs)
declare_frogfs_bin(NAME ${FROGFS_NAME})

idf_component_get_property(main_args esptool_py FLASH_ARGS)
idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
esptool_py_flash_target(${FROGFS_NAME}-flash "${main_args}" "${sub_args}" ALWAYS_PLAINTEXT)

esptool_py_flash_to_partition(${FROGFS_NAME}-flash storage ${BUILD_DIR}/CMakeFiles/${FROGFS_NAME}.bin)
add_dependencies(${FROGFS_NAME}-flash generate_${FROGFS_NAME}_bin)
```

You can invoke the flash process by running `idf.py frogfs-flash`.

## Configuration

FrogFS expects a yaml configuration file.  There are 3 different sections:
define, collect and filter. All but collect is optional.

Define is a list or dict of variable definitions. There are 2 predefined
variables: `$cwd` and `$frogfs`. You can also reference environment variables
with the `${ENV:varname}` syntax.

Collect 'gathers' up files and directories and places them in the frogfs
filesystem. Glob patterns are allowed in paths. It is recommended to use a
list, but a string can be used. If paths end with a slash, the contents of
that path are placed in the destination. Otherwise, the path itself is placed
in the destination. The destination defaults to the root directory, or in the
case of a dictionary key, a root relative path is used.

For exmple:

```yaml
collect:
  - files/*        # everything in files except dotfiles in /
  - files/         # everything in files in /
  - files          # files directory in / (/files)
  - files/*: dir   # everything in files except dotfiles in /dir
  - files/: dir    # everything in files in /dir
  - files: dir     # files directory in /dir (/dir/files)
```

Variables can be used on both source paths and destination paths.

Filter allows you to do post-processing on the files before they are
integrated. Filter is a list or dict of dicts; with a glob pattern to a list
of verbs. Varibales are expanded and all patterns are evaluated for each file
or directory, top down. Transforms are applied first, then an optional final
compression before caching the file.

Verbs are applied in descending order. You can prefix a transforms or the
`compress` verb with `no` to disable it. There are a couple of special verbs:
`discard` which prevents inclusion and `cache` (default), which caches the
file in the build cache. See `frogfs_example.yaml` for example usage.

## Usage

Two interfaces are available: the [bare API](#bare-api) or when using IDF
there is the [VFS interface](#vfs-interface) which builds on top of the bare
API. You should use the VFS interface in IDF projects, as it uses the portable
and familiar `posix` and `stdio` C functions with it. There is nothing
preventing you from mix and matching both at the same time, however.

### Shared initialization

Configuration requries defining a `frogfs_config_t` structure and passing it
to `frogfs_init`. Two different ways to specify the filesystem:

  1. a memory address using the `addr` variable:

```C
frogfs_config_t frogfs_config = {
    .addr = frogfs_bin,
};
```

 2. a partition name using the `part_label` string:

```C
frogfs_config_t frogfs_config = {
    .part_label = "storage",
};
```

Then it is just a matter of passing the `frogfs_config` to `frogfs_init`
function and checking its return variable:

```C
frogfs_fs_t *fs = frogfs_init(&frogfs_config);
assert(fs != NULL);
```

When done, and all file handles are closed, you can call `frogfs_deinit`:

```C
frogfs_deinit(fs);
```

### VFS interface

The VFS interface has a similar method of initialization; you define a
`frogfs_vfs_conf_t` structure:

  * **base_path** - path to mount FrogFS
  * **fs** - a `frogfs_fs_t` instance
  * **max_files** - max number of files that can be open at a time

```C
frogfs_vfs_conf_t frogfs_vfs_conf = {
    .base_path = "/frogfs",
    .fs = fs,
    .max_files = 5,
};
frogfs_vfs_register(&frogfs_vfs_conf);
```

### Bare API

#### Filesystem functions:

  * frogfs_fs_t *[frogfs_init](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_init)(const frogfs_config_t *conf)
  * void [frogfs_deinit](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_deinit)(frogfs_fs_t *fs)

#### Object functions:

  * const frogfs_entry_t *[frogfs_get_entry](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_get_entry)(const frogfs_fs_t *fs, const char *path)
  * char *[frogfs_get_name](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_get_name)(const frogfs_entry_t *entry)
  * char *[frogfs_get_path](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_get_path)(const frogfs_fs_t *fs, const frogfs_entry_t *entry)
  * int [frogfs_is_dir](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_is_dir)(const frogfs_entry_t *entry)
  * int [frogfs_is_file](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_is_file)(const frogfs_entry_t *entry)
  * void [frogfs_stat](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_stat)(const frogfs_fs_t *fs, const frogfs_entry_t *entry, frogfs_stat_t *st)
  * frogfs_fh_t *[frogfs_open](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_open)(const frogfs_fs_t *fs, const frogfs_entry_t *entry, unsigned int flags)
  * void [frogfs_close](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_close)(frogfs_fh_t *fh)
  * int [frogfs_is_raw](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_is_raw)(frogfs_fh_t *fh)
  * size_t [frogfs_read](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_read)(frogfs_fh_t *fh, void *buf, size_t len)
  * ssize_t [frogfs_seek](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_seek)(frogfs_fh_t *fh, long offset, int mode)
  * size_t [frogfs_tell](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_tell)(frogfs_fh_t *fh)
  * size_t [frogfs_access](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_access)(frogfs_fh_t *fh, void **buf)

#### Directory Functions:

  * frogfs_dh_t *[frogfs_opendir](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_opendir)(frogfs_fs_t *fs, const frogfs_entry_t *entry)
  * void [frogfs_closedir](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_closedir)(frogfs_dh_t *dh)
  * const frogfs_entry_t *[frogfs_readdir](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_readdir)(frogfs_dh_t *dh)
  * void [frogfs_seekdir](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_seekdir)(frogfs_dh_t *dh, long loc)
  * long [frogfs_telldir](https://frogfs.readthedocs.io/en/latest/api-reference/bare.html#c.frogfs_telldir)(frogfs_dh_t *dh)

# How it works

Under the hood there is a hash table consisting of djb2 path hashes to entry
offsets, which allow for fast lookups using a binary search algorithm. All
entries except the root entry have a parent locator offset. Directory entries
have a sorted list of offsets to child entries.

FrogFS binaries can be either embedded in your application, or accessed using
memory mapped I/O. It is not possible (at this time) to use FrogFS without the
file system binary existing in data address space.

Creation of a FrogFS filesystem is handled by a single tool,
`tools/mkfrogfs.py`. It uses transforms in the `tools` directory, or you can
add your own transforms by creating a `tools` directory in your projects root
directory, with a filename starting with `transform-` and ending with `.js` or
`.py`. Transform tools take data on stdin and produce output on stdout.

Both transform and compresors can accept arguments. See `frogfs_example.yaml`
for an example.

# History and Acknowledgements

FrogFS was split off of Chris Morgan (chmorgan)'s
[libesphttpd](https://github.com/chmorgan/libesphttpd/) project (MPL 2.0),
which is a fork of Jeroen Domburg (Sprite_tm)'s
[libesphttpd](https://github.com/spritetm/libesphttpd/) (BEER-WARE). This
project would never have existed without them.

Thank you to all the contributors to this project!
