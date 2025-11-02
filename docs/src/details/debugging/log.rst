.. _logging:

=======
Logging
=======

LVGL has a built-in *Logging* module to inform the user about what is
happening in the library.



Configuring Logging
*******************


Log Level
---------

To enable logging, set :c:macro:`LV_USE_LOG` in ``lv_conf.h`` to a non-zero value and
set :c:macro:`LV_LOG_LEVEL` to one of the following values.  They are prioritized as
follows (from most to least verbose):

- :c:macro:`LV_LOG_LEVEL_TRACE`: A lot of logs to give detailed information
- :c:macro:`LV_LOG_LEVEL_INFO`: Log important events.
- :c:macro:`LV_LOG_LEVEL_WARN`: Log if something unwanted happened but didn't cause a problem.
- :c:macro:`LV_LOG_LEVEL_ERROR`: Log only critical issues, where the system may fail.
- :c:macro:`LV_LOG_LEVEL_USER`: Log only custom log messages added by the user.
- :c:macro:`LV_LOG_LEVEL_NONE`: Do not log anything.

When you set :c:macro:`LV_LOG_LEVEL` to a certain level, only messages with that level
or higher priority (less verbose) will be logged.

Example:  you set :c:macro:`LV_LOG_LEVEL` to :c:macro:`LV_LOG_LEVEL_WARN`, then
:c:macro:`LV_LOG_LEVEL_WARN`, :c:macro:`LV_LOG_LEVEL_ERROR` and
:c:macro:`LV_LOG_LEVEL_USER` messages will be logged.


Log Output
----------

If your system supports ``printf``, you just need to enable
:c:macro:`LV_LOG_PRINTF` in ``lv_conf.h`` to output log messages with ``printf``.

If you can't use ``printf`` or want to use a custom function to log, you
can register a "logging" function with :cpp:func:`lv_log_register_print_cb`.

For example:

.. code-block:: c

   void my_log_cb(lv_log_level_t level, const char * buf)
   {
     serial_send(buf, strlen(buf));
   }

   ...


   lv_log_register_print_cb(my_log_cb);



Using Logging
*************

You use the log module via the following macros:

- ``LV_LOG_TRACE(text)``
- ``LV_LOG_INFO(text)``
- ``LV_LOG_WARN(text)``
- ``LV_LOG_ERROR(text)``
- ``LV_LOG_USER(text)``
- ``LV_LOG(text)``

The first 5 macros append the following information to your ``text``:

-  Log Level name ("Trace", "Info", "Warn", "Error", "User")
-  \__FILE\_\_
-  \__LINE\_\_
-  \__func\_\_

``LV_LOG(text)`` is similar to ``LV_LOG_USER`` but has no extra information added.



API
***

.. API equals:  lv_log_register_print_cb
