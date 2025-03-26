.. _configuration:

=============
Configuration
=============



.. _lv_conf:

lv_conf.h
*********


Creating lv_conf.h
------------------

When setting up your project for the first time, copy ``lvgl/lv_conf_template.h`` to
``lv_conf.h`` next to the ``lvgl`` folder.  Change the first ``#if 0`` to ``1`` to
enable the file's content and set the :c:macro:`LV_COLOR_DEPTH` define to align with
the color depth used by your display panel.  See comments in ``lv_conf.h`` for
details.

The layout of the files should look like this::

    lvgl/
    lv_conf.h
    other files and folders in your project

Alternatively, ``lv_conf.h`` can be copied to another place but then you
should add the :c:macro:`LV_CONF_INCLUDE_SIMPLE` define to your compiler
options (e.g. ``-DLV_CONF_INCLUDE_SIMPLE`` for GCC compiler) and set the
include path manually (e.g. ``-I../include/gui``). In this case LVGL
will attempt to include ``lv_conf.h`` simply with ``#include "lv_conf.h"``.

You can even use a different name for ``lv_conf.h``. The custom path can
be set via the :c:macro:`LV_CONF_PATH` define. For example
``-DLV_CONF_PATH="/home/joe/my_project/my_custom_conf.h"``. If this define
is set :c:macro:`LV_CONF_SKIP` is assumed to be ``0``. Please notice,
when defining the :c:macro:`LV_CONF_PATH`, you need to make sure it is
defined as a string, otherwise a build error will be raised.

If :c:macro:`LV_CONF_SKIP` is defined, LVGL will not try to include
``lv_conf.h``. Instead you can pass the config defines using build
options. For example ``"-DLV_COLOR_DEPTH=32 -DLV_USE_BUTTON=1"``.  Unset
options will get a default value which is the same as the content of
``lv_conf_template.h``.

LVGL also can be used via ``Kconfig`` and ``menuconfig``. You can use
``lv_conf.h`` together with Kconfig as well, but keep in mind that the values
from ``lv_conf.h`` or build settings (``-D...``) override the values
set in Kconfig. To ignore the configs from ``lv_conf.h`` simply remove
its content, or define :c:macro:`LV_CONF_SKIP`.


.. _configuration_settings:

Configuration Settings
----------------------

Once the ``lv_conf.h`` file is in place, you can modify this header to configure
LVGL's behavior, disable unused modules and features, adjust the size of buffers, etc.

The comments in ``lv_conf.h`` explain the meaning of each setting.  Be sure
to at least set :c:macro:`LV_COLOR_DEPTH` according to your display's color
depth.  Note that the examples and demos explicitly need to be enabled
in ``lv_conf.h`` if you need them.

TODO:  Add all things related to ``lv_conf.h`` file and its contents.


Multiple Instances of LVGL
~~~~~~~~~~~~~~~~~~~~~~~~~~
It is possible to run multiple, independent instances of LVGL.  To enable its
multi-instance feature, set :c:macro:`LV_GLOBAL_CUSTOM` in ``lv_conf.h``
and provide a custom function to :cpp:func:`lv_global_default` using ``__thread`` or
``pthread_key_t``.  It will allow running multiple LVGL instances by storing LVGL's
global variables in TLS (Thread-Local Storage).

For example:

.. code-block:: c

    lv_global_t * lv_global_default(void)
    {
        static __thread lv_global_t lv_global;
        return &lv_global;
    }



Kconfig
*******
TODO:  Add how to use LVGL with Kconfig.

