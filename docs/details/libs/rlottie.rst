.. _rlottie:

==============
Rlottie player
==============

.. warning::
   Rlottie is deprecated. Consider using :ref:`lv_lottie` instead.

Allows playing Lottie animations in LVGL. Taken from `lv_rlottie <https://github.com/ValentiWorkLearning/lv_rlottie>`__.

LVGL provides the interface to `Samsung/rlottie <https://github.com/Samsung/rlottie>`__ library's C
API. That is the actual Lottie player is not part of LVGL, it needs to
be built separately.

Build Rlottie
-------------

To build Samsung's Rlottie C++14 compatible compiler and optionally
CMake 3.14 or higher is required.

To build on desktop you can follow the instructions from Rlottie's
`README <https://github.com/Samsung/rlottie/blob/master/README.md>`__.

In the most basic case it looks like this:

.. code-block:: shell

   mkdir rlottie_workdir
   cd rlottie_workdir
   git clone https://github.com/Samsung/rlottie.git
   mkdir build
   cd build
   cmake ../rlottie
   make -j
   sudo make install

And finally add the ``-lrlottie`` flag to your linker.

On embedded systems you need to take care of integrating Rlottie to the
given build system.

ESP-IDF example at bottom
~~~~~~~~~~~~~~~~~~~~~~~~~

.. _rlottie_usage:

Usage
-----

You can use animation from files or raw data (text). In either case
first you need to enable :c:macro:`LV_USE_RLOTTIE` in ``lv_conf.h``.

The ``width`` and ``height`` of the Widget be set in the *create*
function and the animation will be scaled accordingly.

Use Rlottie from file
~~~~~~~~~~~~~~~~~~~~~

To create a Lottie animation from file use:

.. code-block:: c

     lv_obj_t * lottie = lv_rlottie_create_from_file(parent, width, height, "path/to/lottie.json");

Note that, Rlottie uses the standard STDIO C file API, so you can use
the path "normally" and no LVGL specific driver letter is required.

Use Rlottie from raw string data
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``lv_example_rlottie_approve.c`` contains an example animation in raw
format. Instead storing the JSON string, a hex array is stored for the
following reasons:

- avoid escaping ``"`` character in the JSON file
- some compilers don't support very long strings

``lvgl/scripts/filetohex.py`` can be used to convert a Lottie file a hex
array. E.g.:

.. code-block:: shell

   ./filetohex.py path/to/lottie.json --filter-character --null-terminate > out.txt

``--filter-character`` filters out non-ASCII characters and ``--null-terminate`` makes sure that a trailing zero is appended to properly close the string.

To create an animation from raw data:

.. code-block:: c

   extern const uint8_t lottie_data[];
   lv_obj_t* lottie = lv_rlottie_create_from_raw(parent, width, height, (const char *)lottie_data);

Getting animations
------------------

Lottie is standard and popular format so you can find many animation
files on the web. For example: https://lottiefiles.com/

You can also create your own animations with Adobe After Effects or
similar software.

Controlling animations
----------------------

LVGL provides two functions to control the animation mode:
:cpp:func:`lv_rlottie_set_play_mode` and :cpp:func:`lv_rlottie_set_current_frame`.
You'll combine your intentions when calling the first method, like in
these examples:

.. code-block:: c

   lv_obj_t * lottie = lv_rlottie_create_from_file(scr, 128, 128, "test.json");
   lv_obj_center(lottie);
   // Pause to a specific frame
   lv_rlottie_set_current_frame(lottie, 50);
   lv_rlottie_set_play_mode(lottie, LV_RLOTTIE_CTRL_PAUSE); // The specified frame will be displayed and then the animation will pause

   // Play backward and loop
   lv_rlottie_set_play_mode(lottie, LV_RLOTTIE_CTRL_PLAY | LV_RLOTTIE_CTRL_BACKWARD | LV_RLOTTIE_CTRL_LOOP);

   // Play forward once (no looping)
   lv_rlottie_set_play_mode(lottie, LV_RLOTTIE_CTRL_PLAY | LV_RLOTTIE_CTRL_FORWARD);

The default animation mode is **play forward with loop**.

If you don't enable looping, a :cpp:enumerator:`LV_EVENT_READY` is sent when the
animation cannot make more progress without looping.

To get the number of frames in an animation or the current frame index,
you can cast the :c:struct:`lv_obj_t` instance to a :c:struct:`lv_rlottie_t` instance
and inspect the ``current_frame`` and ``total_frames`` members.

ESP-IDF Example
---------------

Background
~~~~~~~~~~

Rlottie can be expensive to render on embedded hardware. Lottie
animations tend to use a large amount of CPU time and can use large
portions of RAM. This will vary from lottie to lottie but in general for
best performance:

- Limit total # of frames in the animation
- Where possible, try to avoid bezier type animations
- Limit animation render size

If your ESP32 chip does not have SPIRAM you will face severe limitations
in render size.

To give a better idea on this, lets assume you want to render a 240x320
lottie animation.

In order to pass initialization of the lv_rlottie_t object, you need
240x320x32/8 (307k) available memory. The latest ESP32-S3 has 256kb RAM
available for this (before freeRtos and any other initialization starts
taking chunks out). So while you can probably start to render a 50x50
animation without SPIRAM, PSRAM is highly recommended.

Additionally, while you might be able to pass initialization of the
lv_rlottie_t object, as rlottie renders frame to frame, this consumes
additional memory. A 30 frame animation that plays over 1 second
probably has minimal issues, but a 300 frame animation playing over 10
seconds could very easily crash due to lack of memory as rlottie
renders, depending on the complexity of the animation.

Rlottie will not compile for the IDF using the ``-02`` compiler option at
this time.

For stability in lottie animations, I found that they run best in the
IDF when enabling :c:macro:`LV_MEM_CUSTOM` (using ``stdlib.h``)

For all its faults, when running right-sized animations, they provide a
wonderful utility to LVGL on embedded LCDs and can look really good when
done properly.

When picking/designing a lottie animation consider the following
limitations:

- Build the lottie animation to be sized for the intended size
- it can scale/resize, but performance will be best when the base lottie size is as intended
- Limit total number of frames, the longer the lottie animation is,
  the more memory it will consume for rendering (rlottie consumes IRAM for rendering)
- Build the lottie animation for the intended frame rate
- default lottie is 60fps, embedded LCDs likely won't go above 30fps

IDF Setup
~~~~~~~~~

Where the LVGL simulator uses the installed rlottie lib, the IDF works
best when using rlottie as a submodule under the components directory.

.. code-block:: shell

   cd 'your/project/directory'
   git add submodule
   git add submodule https://github.com/Samsung/rlottie.git ./components/rlottie/rlottie
   git submodule update --init --recursive

Now, Rlottie is available as a component in the IDF, but it requires
some additional changes and a CMakeLists file to tell the IDF how to
compile.

Rlottie patch file
~~~~~~~~~~~~~~~~~~

Rlottie relies on a dynamic linking for an image loader lib. This needs
to be disabled as the IDF doesn't play nice with dynamic linking.

A patch file is available in lvgl under:
``/env_support/esp/rlottie/0001-changes-to-compile-with-esp-idf.patch``

Apply the patch file to your rlottie submodule.

CMakeLists for IDF
~~~~~~~~~~~~~~~~~~

An example CMakeLists file has been provided at
``/env_support/esp/rlottie/CMakeLists.txt``

Copy this CMakeLists file to
``'your-project-directory'/components/rlottie/``

In addition to the component CMakeLists file, you'll also need to tell
your project level CMakeLists in your IDF project to require rlottie:

.. code-block:: console

   REQUIRES "lvgl" "rlottie"

From here, you should be able to use lv_rlottie objects in your ESP-IDF
project as any other widget in LVGL ESP examples. Please remember that
these animations can be highly resource constrained and this does not
guarantee that every animation will work.

Additional Rlottie considerations in ESP-IDF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

While unnecessary, removing the ``rlottie/rlottie/example`` folder can remove
many un-needed files for this embedded LVGL application

From here, you can use the relevant LVGL lv_rlottie functions to create
lottie animations in LVGL on embedded hardware!

Please note, that while lottie animations are capable of running on many
ESP chips, below is recommended for best performance.

-  ESP32-S3-WROOM-1-N16R8

   -  16mb quad spi flash
   -  8mb octal spi PSRAM

-  IDF4.4 or higher

The Esp-box devkit meets this spec and
https://github.com/espressif/esp-box is a great starting point to adding
lottie animations.

You will need to enable :c:macro:`LV_USE_RLOTTIE` through **idf.py** menuconfig under
LVGL component settings.

Additional changes to make use of SPIRAM
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:cpp:expr:`lv_alloc/realloc` do not make use of SPIRAM. Given the high memory usage
of lottie animations, it is recommended to shift as much out of internal
DRAM into SPIRAM as possible. In order to do so, SPIRAM will need to be
enabled in the menuconfig options for your given espressif chip.

There may be a better solution for this, but for the moment the
recommendation is to make local modifications to the lvgl component in
your espressif project. This is as simple as swapping
:cpp:expr:`lv_alloc/lv_realloc` calls in `lv_rlottie.c`` with :cpp:expr:`heap_caps_malloc` (for
IDF) with the appropriate :cpp:expr:`MALLOC_CAP` call - for SPIRAM usage this is
:cpp:expr:`MALLOC_CAP_SPIRAM`.

.. code-block:: c

   rlottie->allocated_buf = heap_caps_malloc(allocated_buf_size+1, MALLOC_CAP_SPIRAM);

.. _rlottie_example:

Example
-------

.. include:: ../../examples/libs/rlottie/index.rst

.. _rlottie_api:

API
---
