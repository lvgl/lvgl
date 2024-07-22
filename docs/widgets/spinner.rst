.. _lv_spinner:

====================
Spinner (lv_spinner)
====================

Overview
********

The Spinner object is a spinning arc over a ring.

.. _lv_spinner_parts_and_styles:

Parts and Styles
****************

The parts are identical to the parts of :ref:`lv_arc`.

.. _lv_spinner_usage:

Usage
*****

Create a spinner
----------------

To create a spinner use
:cpp:expr:`lv_spinner_create(parent)`.

Using :cpp:expr:`lv_spinner_set_anim_params(spinner, spin_duration, angle)` the duration
of one revolution and the length of he arc can be customized.

.. _lv_spinner_events:

Events
******

No special events are sent to the Spinner.

See the events of the `Arc </widgets/arc>`__ too.

Learn more about :ref:`events`.

.. _lv_spinner_keys:

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`indev_keys`.

.. _lv_spinner_example:

Example
*******

.. include:: ../examples/widgets/spinner/index.rst

.. _lv_spinner_api:

API
***
