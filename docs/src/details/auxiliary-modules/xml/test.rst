.. _xml_test:

====
Test
====

Overview
********

The XML test module is a powerful and flexible way to define functional UI tests.

Test XML files are similar to components but are wrapped in a `<test>` tag and consist of two main parts:

- **UI Definition**: Use `<styles>`, `<consts>`, and `<view>` to define how the UI should look. This is identical to how `<component>`s are structured.
- **Test Steps**: Encapsulated in a `<steps>` tag, these define the actions and assertions for the test.

Step Types
----------

The following step types are currently supported (with example parameters):

- ``<click_at x="10" y="30"/>``: Simulates a click at the specified screen coordinates.
- ``<wait ms="100"/>``: Waits for the given number of milliseconds. LVGL continues running, including animations and timers.
- ``<freeze ms="100"/>``: Pauses the UI and LVGL's internal time. Useful for visual debugging.
- ``<subject_set_int subject="subject1" value="10">`` Set an integer subject's value.
- ``<subject_set_string subject="subject1" value="Hi!>`` Set string subject's value.
- ``<subject_compare_int subject="subject1" value="10">`` Compare an integer subject's value against a reference value.
- ``<subject_compare_string subject="subject1" value="Hi!>`` Compare a string subject's value against a reference value.
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
            <click_at x="50" y="64"/>
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

To execute the registered test, use:

- :cpp:expr:`lv_xml_test_run(slowdown)`

The `slowdown` parameter controls playback speed:

- `0`: Maximum speed
- `1`: Real-time speed
- `2`: Half-speed
- `10`: 10× slower
- ...and so on

`lv_xml_test_run()` blocks until all steps are completed. It is safe to call multiple times.

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
