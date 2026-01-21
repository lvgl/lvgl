.. _lv_form:

================
Form (lv_form)
================

Overview
********

The Form widget is a container that helps organize and manage multiple input fields in a structured way. It provides:

- **Automatic layout**: Organizes fields with proper spacing and alignment
- **Sections**: Group related fields together with optional titles
- **Validation**: Built-in support for required fields and validation
- **Data management**: Easy retrieval of form data
- **Flexible input support**: Works with textareas, dropdowns, checkboxes, switches, etc.

The Form widget is ideal for creating:
- User registration forms
- Settings pages
- Data entry forms
- Configuration dialogs

.. _lv_form_parts_and_styles:

Parts and Styles
****************

**Main Form** (``lv_form_t``)
  - :cpp:enumerator:`LV_PART_MAIN` - The background of the form
  - Uses Flex layout by default for vertical arrangement
  - Supports vertical, horizontal, and grid layouts

**Section** (``lv_form_section_t``)
  - :cpp:enumerator:`LV_PART_MAIN` - Section container with border and padding
  - :cpp:enumerator:`LV_PART_ITEMS` - Section title label (if present)
  - Groups related fields together

**Field** (``lv_form_field_t``)
  - :cpp:enumerator:`LV_PART_MAIN` - Field container
  - :cpp:enumerator:`LV_PART_ITEMS` - Field label
  - :cpp:enumerator:`LV_PART_INDICATOR` - Input widget container

.. _lv_form_usage:

Usage
*****

Creating a Form
---------------

.. code-block:: c

    lv_obj_t * form = lv_form_create(parent);
    lv_obj_set_size(form, 300, LV_SIZE_CONTENT);
    lv_obj_center(form);

Adding Sections
---------------

Sections group related fields and can have titles:

.. code-block:: c

    lv_obj_t * section = lv_form_add_section(form, "Personal Info");
    /* Section will have a border and title */

Adding Fields
-------------

Fields combine a label and an input widget:

.. code-block:: c

    /* Create an input widget */
    lv_obj_t * ta = lv_textarea_create(form);
    lv_textarea_set_placeholder(ta, "Enter name");

    /* Add it to a section with a label */
    lv_form_add_field(section, "Name:", ta);

Supported input widgets:
- :ref:`lv_textarea`
- :ref:`lv_dropdown`
- :ref:`lv_checkbox`
- :ref:`lv_switch`
- :ref:`lv_button`
- :ref:`lv_slider`
- Any other widget

Setting Layout
--------------

.. code-block:: c

    /* Vertical (default) */
    lv_form_set_layout(form, LV_FORM_LAYOUT_VERTICAL);

    /* Horizontal */
    lv_form_set_layout(form, LV_FORM_LAYOUT_HORIZONTAL);

    /* Grid */
    lv_form_set_layout(form, LV_FORM_LAYOUT_GRID);

Field Validation
----------------

Mark fields as required:

.. code-block:: c

    lv_obj_t * field = lv_form_add_field(section, "Email:", ta_email);
    lv_form_field_set_required(field, true);

Validate all fields:

.. code-block:: c

    bool is_valid = lv_form_validate(form);
    if(!is_valid) {
        /* Handle validation error */
    }

Getting Form Data
-----------------

Retrieve all field values as a formatted string:

.. code-block:: c

    char buffer[256];
    lv_form_get_data(form, buffer, sizeof(buffer));
    /* buffer contains:
       [Personal Info]
         Name: John Doe
         Email: john@example.com
    */

Handling Events
---------------

.. code-block:: c

    void form_event_cb(lv_event_t * e)
    {
        lv_form_event_data_t * data = lv_event_get_param(e);
        LV_LOG_USER("Form valid: %s", data->is_valid ? "Yes" : "No");
    }

    /* Add to form */
    lv_obj_add_event_cb(form, form_event_cb, LV_EVENT_SUBMIT, NULL);

Cleaning the Form
-----------------

.. code-block:: c

    lv_form_clean(form);  /* Removes all sections and fields */

.. _lv_form_examples:

Examples
********

.. include:: /examples/widgets/form/index.rst

.. _lv_form_api:

API
***

.. doxygenfile:: lv_form.h
