Spinner (lv_spinner)
====================

Overview
********

The Spinner object is a spinning arc over a ring.

Parts and Styles
****************

The parts are identical to the parts of `lv_arc </widgets/arc>`__.

Usage
*****

Create a spinner
----------------

To create a spinner use
:cpp:expr:`lv_spinner_create(parent, spin_time, arc_length)`. ``spin time`` sets
the spin time in milliseconds, ``arc_length`` sets the length of the spinning arc in degrees.

Events
******

No special events are sent to the Spinner.

See the events of the `Arc </widgets/arc>`__ too.

Learn more about :ref:`events`.

Keys
****

No *Keys* are processed by the object type.

Learn more about :ref:`indev_keys`.

Example
*******

.. include:: ../examples/widgets/spinner/index.rst

API
***
