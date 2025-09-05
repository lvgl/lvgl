.. _xml_test:

==========
UI Testing
==========

Overview
********

The XML test module is a powerful and flexible way to define functional UI tests.

Test XML files are similar to components but are wrapped in a ``<test>`` tag and consist of two main parts:

- **UI Definition**: Use ``<styles>``, ``<consts>``, and ``<view>`` to define how the UI should look. This is identical to how ``<component>``\ s are structured.
- **Test Steps**: Encapsulated in a ``<steps>`` tag, these define the actions and assertions for the test.

Step Types
----------

The following step types are currently supported (with example parameters):

- ``<move_to x="10" y="30"/>``: Move the mouse to the specified screen coordinates.
- ``<click_at x="10" y="30"/>``: Simulates a click at the specified screen coordinates.
- ``<press/>``: Press on the current screen coordinate.
- ``<release/>``: Release on the current screen coordinate.
- ``<wait ms="100"/>``: Waits for the given number of milliseconds. LVGL continues running, including animations and timers.
- ``<freeze ms="100"/>``: Pauses the UI and LVGL's internal time. Useful for visual debugging.
- ``<subject_set subject="subject1" value="5>`` Set an integer or string subject's value.
- ``<subject_compare subject="subject1" value="10">`` Compare an integer or strings subject's value against a reference value.
- ``<screenshot_compare path="path/to/image.png"/>``: Compares the current screen with a reference image.
  - If the image doesn't exist, it is created.
  - If the comparison fails, an image with `_err` suffix is saved for inspection.

Example:

.. code-block:: xml

    <test width="300" height="400">
        <view width="100%" height="100%" flex_flow="column">
            <lv_checkbox text="First one!"/>
            <lv_slider />
        </view>

        <steps>
            <screenshot_compare path="imgs/before.png"/>
            <click_at x="32" y="32"/>
            <subject_compare subject="subject2" value="50"/>
            <click_at x="50" y="64"/>
            <subject_set subject="subject2" value="10"/>
            <wait ms="300"/>
            <screenshot_compare path="imgs/after.png"/>
        </steps>
    </test>

Registering Tests
-----------------

Tests can be registered in two ways:

- From file: :cpp:expr:`lv_xml_test_register_from_file("path/to/test.xml", "A:ref_images")`
- From string: :cpp:expr:`lv_xml_test_register_from_data(xml_data, "A:ref_images")`

The second parameter specifies a prefix for screenshot comparison paths.

To unregister tests, use:

- :cpp:expr:`lv_xml_test_unregister()`

This is called automatically when registering a new test, so manual calls are usually unnecessary.

Running Tests
-------------

All steps
^^^^^^^^^

To execute the registered test, use :cpp:expr:`lv_xml_test_run_all(slowdown)`.

It will clean the screen and create a fresh instance of the ``view`` to be tested.

The `slowdown` parameter controls playback speed:

- `0`: Maximum speed
- `1`: Real-time speed
- `2`: Half-speed
- `10`: 10× slower
- ...and so on

`lv_xml_test_run_all()` blocks until all steps are completed. It is safe to call it multiple times.

The return value is the number of failed tests.

Step-by-step
^^^^^^^^^^^^

It's also possible to run each step separately, one after another.

First, call :cpp:expr:`lv_xml_test_run_init()` to prepare for executing the steps.
It will clean the screen and create a fresh instance of the ``view`` to be tested.

Get the number of steps using :cpp:expr:`lv_xml_test_get_step_count()`
and then call :cpp:expr:`lv_xml_test_run_next(slowdown)` as many times.

:cpp:expr:`lv_xml_test_run_next()` returns ``true`` if the given step passed, or ``false`` if it failed.

Finally, call :cpp:expr:`lv_xml_test_run_stop()` to clean up and exit testing mode.


Getting the Test Results
------------------------

Currently, only `screenshot_compare` steps can fail. However, the result of each step can be queried:

.. code-block:: c

    uint32_t step_cnt = lv_xml_test_get_step_count();
    for(uint32_t i = 0; i < step_cnt; i++) {
        if(lv_xml_test_get_step_type(i) == LV_XML_TEST_STEP_TYPE_SCREENSHOT_COMPARE) {
            if(lv_xml_test_get_status(i))
                printf("Step %d passed\n", i);
            else
                printf("Step %d failed\n", i);
        } else {
            printf("Step %d is not a screenshot comparison\n", i);
        }
    }

.. _lv_xml_test_api:

API
***
