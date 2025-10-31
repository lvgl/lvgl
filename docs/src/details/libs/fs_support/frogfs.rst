.. _frogfs:

======
FrogFS
======

`frogfs <https://github.com/jkent/frogfs>`__
is a read-only :ref:`filesystem <file_system>` for packing a directory
tree of files into a single "blob" which can be distributed easily or
built into a firmware. It is suitable for packaging XML files into your
app. See the intro to :ref:`XML in LVGL <xml_main>` if you are
interested in using XML.

A copy of the frogfs source is inside LVGL. It has been
reduced to a subset of the original functionality for simple loading
of blobs in LVGL.


Create a frogfs blob
********************

You can create a frogfs blob for use in LVGL by using the upstream
frogfs project. Get it from `GitHub <https://github.com/jkent/frogfs>`__
with ``git clone``.

.. code-block:: shell

    git clone https://github.com/jkent/frogfs
    cd frogfs

Create a ``frogfs.yaml`` file inside.
Here is the minimum required content of the file.
``files`` in this case is a directory next to ``frogfs.yaml``
that has all the files you want to include in the blob.
The directory structure will be preserved.

.. code-block:: yaml

    collect:
      - files/*

.. code-block:: shell

    rm -rf build_dir
    mkdir build_dir
    python3 tools/mkfrogfs.py frogfs.yaml build_dir frogfs.bin

If it succeeds, you will have ``frogfs.bin`` which is the
filesystem blob to embed in your LVGL application. One way to get it
in your C app is to convert the binary file to a C array.

.. code-block:: shell

    echo const > frogfs_bin.c
    xxd -i frogfs.bin >> frogfs_bin.c

This method will create a C file with two global variables called
``frogfs_bin`` and ``frogfs_bin_len`` which can be declared in other
C files like:

.. code-block:: c

    extern const unsigned char frogfs_bin[];
    extern unsigned int frogfs_bin_len;

See the frogfs ``README.md`` for info about using CMake to automate
blob builds.


Usage in LVGL
*************

Set ``LV_USE_FS_FROGFS`` to ``1`` in your ``lv_conf.h``. Set
``LV_FS_FROGFS_LETTER`` to a letter like ``'F'``.

.. code-block:: c

    extern const unsigned char frogfs_bin[];
    lv_fs_frogfs_register_blob(frogfs_bin, "main_blob"); /* returns LV_RESULT_OK if the blob is ok */

    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, "F:main_blob/my_image.png"); /* the blob is specified by the path prefix */

    extern const unsigned char frogfs_bin_2[];
    lv_fs_frogfs_register_blob(frogfs_bin_2, "another_blob");

    lv_obj_t * img2 = lv_image_create(lv_screen_active());
    lv_image_set_src(img2, "F:another_blob/my_image.png");

    lv_obj_delete(img);
    lv_fs_frogfs_unregister_blob("main_blob"); /* optional ability to unregister blobs */

If you have a frogfs blob stored in a different :ref:`filesystem <file_system>`
accessible to LVGL, you can load the blob into ram and register it from there.
Ensure there is enough ram for the blob. Driver letter ``'A'`` in this example
could be an SD card or an OS filesystem, etc.
See the :ref:`other filesystems available in LVGL <libs_filesystem>`.

.. code-block:: c

    uint32_t blob_size;
    lv_fs_path_get_size("A:blobs/new_blob.bin", &blob_size);
    void * blob = malloc(blob_size);
    lv_fs_load_to_buf(blob, blob_size, "A:blobs/new_blob.bin");

    lv_fs_frogfs_register_blob(blob, "new_blob");


Compression and Minification
****************************

The frogfs in LVGL does not have the decompression support that the upstream
frogfs has. Typically, asset formats like PNG are already compressed.

Compared to image assets, XML files have a relatively small size even when
the described UI is complex. Regardless, the XML file size can be reduced
through so-called "minification". In the simplest case, it means removing
all comments and insignificant whitespace. This is a form of compression
that does not require a decompression step. The only drawback is that the
incorrect line number relative to the un-minified version will be logged if
there is a parse failure.
See the intro to :ref:`XML in LVGL <xml_main>`.

You can prepare a frogfs blob with minified XML files. In your ``frogfs``
clone directory make a copy of ``tools/transform-html-minifier.js`` called
``transform-xml-minifier.js`` and set the ``const options = { ... };``
to the below. Not all the original aggressive HTML minifications are standard
XML, so this is a subset that preserves standard XML syntax.

.. code-block:: javascript

    const options = {
        keepClosingSlash: true,
        collapseWhitespace: true,
        removeComments: true
    };

You can then add the filter to your ``frogfs.yaml``. Minified versions of the
XML files in the ``files/`` directory in this example will be put in the blob.

.. code-block:: yaml

    collect:
      - files/*

    filter:
      '*.xml':
        - xml-minifier

You can create custom filters very easily. See the frogfs ``README.md`` for
more info. Any Python or JavaScript file in
``tools`` that begins with ``transform-`` will be treated as a possible
transform which will be passed the file to minify via ``stdin`` and the
minified output is expected from ``stdout``. The ``filter-name`` part of
``transform-filter-name.py`` shall be given in your ``frogfs.yaml`` ``filter``
list for certain types or all types of files.
