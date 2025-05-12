.. _contributing_dco:

=======================================
Developer Certification of Origin (DCO)
=======================================

Overview
********

To ensure all licensing criteria are met for every repository of the
LVGL project, we apply a process called DCO (Developer's Certificate of
Origin).

The text of DCO can be read here: https://developercertificate.org/.

By contributing to any repositories of the LVGL project you agree that
your contribution complies with the DCO.

If your contribution fulfills the requirements of the DCO, no further
action is needed. If you are unsure feel free to ask us in a comment,
e.g. in your submitted :ref:`Pull Request <contributing_pull_requests>`.



Accepted Licenses and Copyright Notices
***************************************

To make the DCO easier to digest, here are some practical guides about
specific cases:

Your own work
-------------

The simplest case is when the contribution is solely your own work. In
this case you can just send a Pull Request without worrying about any
licensing issues.

Using code from an online source
--------------------------------

If the code you would like to add is based on an article, post or
comment on a website (e.g. StackOverflow) the license and/or rules of
that site should be followed.

For example in case of StackOverflow a notice like this can be used:

.. code-block::

   /* The original version of this code-snippet was published on StackOverflow.
    * Post: http://stackoverflow.com/questions/12345
    * Author: http://stackoverflow.com/users/12345/username
    * The following parts of the snippet were changed:
    * - Check this or that
    * - Optimize performance here and there
    */
    ... code snippet here ...

Using MIT-licensed code
-----------------------

As LVGL is MIT licensed, other MIT licensed code can be integrated
without issues. The MIT license requires a copyright notice be added to
the derived work. Any derivative work based on MIT licensed code must
copy the original work's license file or text.

Use GPL-licensed code
---------------------

The GPL license is not compatible with the MIT license. Therefore, LVGL
cannot accept GPL licensed code.
