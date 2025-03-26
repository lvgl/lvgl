.. _logging:

=======
Logging
=======

LVGL has a built-in *Log* module to inform the user about what is
happening in the library.


Log Level
*********

To enable logging, set :c:macro:`LV_USE_LOG` in ``lv_conf.h`` and set
:c:macro:`LV_LOG_LEVEL` to one of the following values:

- :c:macro:`LV_LOG_LEVEL_TRACE`: A lot of logs to give detailed information
- :c:macro:`LV_LOG_LEVEL_INFO`: Log important events
- :c:macro:`LV_LOG_LEVEL_WARN`: Log if something unwanted happened but didn't cause a problem
- :c:macro:`LV_LOG_LEVEL_ERROR`: Only critical issues, where the system may fail
- :c:macro:`LV_LOG_LEVEL_USER`: Only user messages
- :c:macro:`LV_LOG_LEVEL_NONE`: Do not log anything

The events which have a higher level than the set log level will be logged
as well. E.g. if you :c:macro:`LV_LOG_LEVEL_WARN`, errors will be also logged.


Printing Logs
*************

Logging with printf
-------------------

If your system supports ``printf``, you just need to enable
:c:macro:`LV_LOG_PRINTF` in ``lv_conf.h`` to send the logs with ``printf``.

Custom log function
-------------------

If you can't use ``printf`` or want to use a custom function to log, you
can register a "logger" callback with :cpp:func:`lv_log_register_print_cb`.

For example:

.. code-block:: c

   void my_log_cb(lv_log_level_t level, const char * buf)
   {
     serial_send(buf, strlen(buf));
   }

   ...


   lv_log_register_print_cb(my_log_cb);


Add Logs
********

You can also use the log module via the
``LV_LOG_TRACE/INFO/WARN/ERROR/USER(text)`` or ``LV_LOG(text)``
functions. Here:

-  ``LV_LOG_TRACE/INFO/WARN/ERROR/USER(text)`` append the following information to your ``text``
-  Log Level
-  \__FILE\_\_
-  \__LINE\_\_
-  \__func\_\_
-  ``LV_LOG(text)`` is similar to ``LV_LOG_USER`` but has no extra information attached.

API
***
