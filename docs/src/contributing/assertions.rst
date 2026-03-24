.. _assertions_verification:

============================
Assertions and Verification
============================

LVGL provides two sets of macros for validating conditions at runtime:
**assertions** and **verification**. This page explains when and how to use
each within LVGL's codebase.



Overview
********

+---------------------------+---------------------------+---------------------------+
| Feature                   | LV_ASSERT                 | LV_VERIFY                 |
+===========================+===========================+===========================+
| Purpose                   | Catch programming errors  | Validate runtime input    |
+---------------------------+---------------------------+---------------------------+
| On failure                | Halts (LV_ASSERT_HANDLER) | Configurable action       |
+---------------------------+---------------------------+---------------------------+
| Log level                 | Error                     | Warning                   |
+---------------------------+---------------------------+---------------------------+
| Recovery possible         | No                        | Yes / Maybe               |
+---------------------------+---------------------------+---------------------------+
| Typical use               | Internal invariants       | API input validation      |
+---------------------------+---------------------------+---------------------------+

**Use assertions** for conditions that indicate bugs in the code—situations
that should never happen if the code is correct.

**Use verification** for conditions that might legitimately fail at runtime
(e.g., user-provided input, resource availability) and require graceful handling.



Assertions
**********

Assertions check for programming errors and halt the program when they fail.
They are typically enabled only in debug builds and may be disabled in release builds.


Configuration
-------------

Enable assertions in ``lv_conf.h``:

- :c:macro:`LV_USE_ASSERT_NULL` - Check for NULL pointers
- :c:macro:`LV_USE_ASSERT_MALLOC` - Check for failed memory allocations
- :c:macro:`LV_USE_ASSERT_MEM_INTEGRITY` - Check memory integrity

Configure the assert handler with :c:macro:`LV_ASSERT_HANDLER`. The default
behavior is ``while(1);`` which halts the program.


Usage
-----

:c:macro:`LV_ASSERT`
   Basic assertion with the condition stringified in the log message:

   .. code-block:: c

      LV_ASSERT(pi > 0);

:c:macro:`LV_ASSERT_MSG`
   Assertion with a plain string message:

   .. code-block:: c

      LV_ASSERT_MSG(pi > 0, "pi should be positive");

:c:macro:`LV_ASSERT_FORMAT_MSG`
   Assertion with a printf-style format message:

   .. code-block:: c

      LV_ASSERT_FORMAT_MSG(pi > 0, ": was %f", pi);


Specialized Assertions
----------------------

:c:macro:`LV_ASSERT_NULL`
   Check that a pointer is not NULL:

   .. code-block:: c

      LV_ASSERT_NULL(obj);

:c:macro:`LV_ASSERT_MALLOC`
   Check that a memory allocation succeeded:

   .. code-block:: c

      void * buf = lv_malloc(size);
      LV_ASSERT_MALLOC(buf);

:c:macro:`LV_ASSERT_MEM_INTEGRITY`
   Check that LVGL's memory pool is not corrupted:

   .. code-block:: c

      LV_ASSERT_MEM_INTEGRITY();



Verification
************

Verification macros provide a more flexible approach than assertions: they log
a warning and allow you to specify a recovery action (such as returning from a
function or continuing with a fallback).

These macros are useful for:

- Input validation at the start of functions
- Checking preconditions without halting the program
- Defensive programming with graceful error handling
- Logging invariant violations that may not be critical but should be noted


Configuration
-------------

The verification macros use the :ref:`logging` system to output warnings.
Make sure logging is enabled by setting :c:macro:`LV_USE_LOG` to ``1`` in ``lv_conf.h``.


Basic Verification
------------------

:c:macro:`LV_VERIFY`
   Takes a condition, an action to execute on failure, and optional printf-style
   arguments for the log message:

   .. code-block:: c

      void draw_to_display(lv_obj_t * display, int len)
      {
          /* Return early if obj is NULL */
          LV_VERIFY(display != NULL, return, ": display must be provided - we can't draw onto thin air!");
      }

      int get_deviation(int** data, size_t len)

          /* Return -1 if len is invalid, logging the actual value */
          LV_VERIFY(len % 2 == 0, return -1, ": need an even number of data points, but got %d", (int)len);

          /* Continue with normal operation... */
      }

   The condition is stringified and included in the log message, so you can see
   exactly what check failed. Only supply a format string and arguments if you
   want to include additional context beyond the condition itself.

:c:macro:`LV_VERIFY_MSG`
   Same as :c:macro:`LV_VERIFY` but with a plain string message:

   .. code-block:: c

      const char * error_msg = get_validation_error();
      LV_VERIFY_MSG(is_valid, return, error_msg);


Log-Only Verification
---------------------

Sometimes you want to log a warning without taking any action:

:c:macro:`LV_VERIFY_OR_LOG`
   Log with a printf-style format:

   .. code-block:: c

      /* Log if obj is NULL, but continue anyway */
      LV_VERIFY_OR_LOG(screen != NULL, ", using primary screen (%s)", primary_screen_name);

:c:macro:`LV_VERIFY_OR_LOG_MSG`
   Log with a plain string message:

   .. code-block:: c

      LV_VERIFY_OR_LOG_MSG(obj != NULL, "continuing with defaults");


Return Helpers
--------------

For common return patterns, convenience macros are provided:

:c:macro:`LV_VERIFY_OR_RETURN`
   Return from a void function:

   .. code-block:: c

      void draw_to_display(lv_obj_t * display)
      {
          LV_VERIFY_OR_RETURN(display != NULL, "display needs to be provided");
          /* ... */
      }

:c:macro:`LV_VERIFY_OR_RETURN_VAL`
   Return a specific value:

   .. code-block:: c

      int get_width(lv_obj_t * obj)
      {
          LV_VERIFY_OR_RETURN_VAL(obj != NULL, -1, "tried to get width of NULL object, returning -1");
          /* ... */
      }


Assert on Failure
-----------------

When a condition is critical and should halt the program (be careful to have your assert handler configured appropriately for non-debug builds):

:c:macro:`LV_VERIFY_OR_ASSERT`
   Logs a warning and invokes :c:macro:`LV_ASSERT_HANDLER`:

   .. code-block:: c

      LV_VERIFY_OR_ASSERT(len % 2 == 0, "data must come in pairs, but got %d data points", len);

:c:macro:`LV_VERIFY_OR_ASSERT_MSG`
   Same as above, but with a plain string message:

   .. code-block:: c

      LV_VERIFY_OR_ASSERT_MSG(obj != NULL, "can't draw on thin air");



Macros Reference
****************

Assertions
----------

.. list-table::
   :widths: 35 65
   :header-rows: 1

   * - Macro
     - Description
   * - :c:macro:`LV_ASSERT`
     - Assert condition; on failure log and halt
   * - :c:macro:`LV_ASSERT_MSG`
     - Assert with plain string message
   * - :c:macro:`LV_ASSERT_FORMAT_MSG`
     - Assert with printf-style format message
   * - :c:macro:`LV_ASSERT_NULL`
     - Assert pointer is not NULL (if enabled)
   * - :c:macro:`LV_ASSERT_MALLOC`
     - Assert allocation succeeded (if enabled)
   * - :c:macro:`LV_ASSERT_MEM_INTEGRITY`
     - Assert memory pool integrity (if enabled)


Verification
------------

.. list-table::
   :widths: 35 65
   :header-rows: 1

   * - Macro
     - Description
   * - :c:macro:`LV_VERIFY`
     - Verify condition; on failure log and execute action
   * - :c:macro:`LV_VERIFY_MSG`
     - Same as LV_VERIFY but with plain string message
   * - :c:macro:`LV_VERIFY_OR_LOG`
     - Verify condition; on failure log only (no action)
   * - :c:macro:`LV_VERIFY_OR_LOG_MSG`
     - Same as LV_VERIFY_OR_LOG but with plain string message
   * - :c:macro:`LV_VERIFY_OR_RETURN`
     - Verify condition; on failure log and return (void function)
   * - :c:macro:`LV_VERIFY_OR_RETURN_VAL`
     - Verify condition; on failure log and return specified value
   * - :c:macro:`LV_VERIFY_OR_ASSERT`
     - Verify condition; on failure log and call LV_ASSERT_HANDLER
   * - :c:macro:`LV_VERIFY_OR_ASSERT_MSG`
     - Same as LV_VERIFY_OR_ASSERT but with plain string message

