===============
Libinput Driver
===============

Overview
--------

Libinput is an input stack for processes that need to provide events from commonly used input devices. That includes mice, keyboards, touchpads,
touchscreens and graphics tablets. Libinput handles device-specific quirks and provides an easy-to-use API to receive events from devices.

Prerequisites
-------------

You have the development version of libinput installed (usually ``libinput-dev``). If your input device requires quirks, make sure they are
installed as well (usually in ``/usr/share/libinput/*.quirks``). To test if your device is set up correctly for use with libinput, you can
run ``libinput list-devices``.

.. code:: console

    $ sudo libinput list-devices
    ...
    Device:           ETPS/2 Elantech Touchpad
    Kernel:           /dev/input/event5
    Group:            10
    Seat:             seat0, default
    Size:             102x74mm
    Capabilities:     pointer gesture
    Tap-to-click:     disabled
    Tap-and-drag:     enabled
    ...

If your device doesn't show up, you may have to configure udev and the appropriate udev rules to connect it.

Additionally, if you want full keyboard support, including letters and modifiers, you'll need the development version of libxkbcommon
installed (usually ``libxkbcommon-dev``).

Configuring the driver
----------------------

Enable the libinput driver support in lv_conf.h, by cmake compiler define or by KConfig.

.. code:: c

    #define LV_USE_LIBINPUT    1

Full keyboard support needs to be enabled separately.

.. code:: c

    #define LV_LIBINPUT_XKB            1
    #define LV_LIBINPUT_XKB_KEY_MAP    { .rules = NULL, .model = "pc101", .layout = "us", .variant = NULL, .options = NULL }
    
To find the right key map values, you may use the ``setxkbmap -query`` command.

Usage
-----

To set up an input device via the libinput driver, all you need to do is call ``lv_libinput_create`` with the respective device type
(``LV_INDEV_TYPE_POINTER`` or ``LV_INDEV_TYPE_KEYPAD``) and device node path (e.g. ``/dev/input/event5``).

.. code:: c

    lv_indev_t *indev = lv_libinput_create(LV_INDEV_TYPE_POINTER, "/dev/input/event5");

Note that touchscreens are treated as (absolute) pointer devices by the libinput driver and require ``LV_INDEV_TYPE_POINTER``.

Depending on your system, the device node paths might not be stable across reboots. If this is the case, you can use ``lv_libinput_find_dev``
to find the first device that has a specific capability.

.. code:: c

    char *path = lv_libinput_find_dev(LV_LIBINPUT_CAPABILITY_TOUCH, true);

The second argument controls whether or not all devices are rescanned. If you have many devices connected this can get quite slow.
Therefore, you should only specify ``true`` on the first call when calling this method multiple times in a row. If you want to find
all devices that have a specific capability, use ``lv_libinput_find_devs``.

If you want to connect a keyboard device to a textarea, create a dedicated input group and set it on both the indev and textarea.

.. code:: c

    lv_obj_t *textarea = lv_textarea_create(...);
    ...
    lv_group_t *keyboard_input_group = lv_group_create();
    lv_indev_set_group(indev, keyboard_input_group);
    lv_group_add_obj(keyboard_input_group, textarea);

