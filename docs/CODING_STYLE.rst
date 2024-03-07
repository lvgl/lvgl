Coding style
============

File template
-------------

Use `misc/lv_templ.c <https://github.com/lvgl/lvgl/blob/master/src/misc/lv_templ.c>`__
and `misc/lv_templ.h <https://github.com/lvgl/lvgl/blob/master/src/misc/lv_templ.h>`__

Naming conventions
------------------

-  Words are separated by '\_'
-  In variable and function names use only lower case letters
   (e.g. *height_tmp*)
-  In enums and defines use only upper case letters
   (e.g. *e.g. MAX_LINE_NUM*)
-  Global names (API):

   -  start with *lv*
   -  followed by module name: *button*, *label*, *style* etc.
   -  followed by the action (for functions): *set*, *get*, etc.
   -  closed with the subject: *name*, *size*, *state* etc.

-  Typedefs

   -  prefer ``typedef struct`` and ``typedef enum`` instead of
      ``struct name`` and ``enum name``
   -  always end ``typedef struct`` and ``typedef enum`` type names with
      ``_t``

-  Abbreviations:

   -  The following abbreviations are used and allowed:

      - ``dsc`` descriptor
      - ``param`` parameter
      - ``indev`` input device
      - ``anim`` animation
      - ``buf``  buffer
      - ``str`` string
      - ``min/max`` minimum/maximum
      - ``alloc`` allocate
      - ``ctrl`` control
      - ``pos`` position
   -  Avoid adding new abbreviations

Coding guide
------------

-  Functions:

   -  Write function with single responsibility
   -  Make the functions ``static`` where possible

-  Variables:

   -  One line, one declaration (BAD: char x, y;)
   -  Use ``<stdint.h>`` (*uint8_t*, *int32_t* etc)
   -  Declare variables where needed (not all at function start)
   -  Use the smallest required scope
   -  Variables in a file (outside functions) are always *static*
   -  Do not use global variables (use functions to set/get static
      variables)

Comments
--------

Before every function have in ``.h`` files a comment like this:

.. code:: c

   /**
    * Return with the screen of an object
    * @param obj pointer to an object
    * @return pointer to a screen
    */
   lv_obj_t * lv_obj_get_screen(lv_obj_t * obj);

Always use ``/*Something*/`` format and NOT ``//Something``

Write readable code to avoid descriptive comments like:
``x++; /*Add 1 to x*/``. The code should show clearly what you are
doing.

You should write **why** have you done this:
``x++; /*Because of closing '\0' of the string*/``

Short "code summaries" of a few lines are accepted. E.g.
``/*Calculate the new coordinates*/``

In comments use \` \` when referring to a variable. E.g.
:literal:`/\*Update the value of \`x_act`*/`


API Conventions
----------------------

To support the auto-generation of bindings, the LVGL C API must
follow some coding conventions:

- Use ``enum``\ s instead of macros. If inevitable to use ``define``\ s
  export them with :cpp:expr:`LV_EXPORT_CONST_INT(defined_value)` right after the ``define``.
- In function arguments use ``type name[]`` declaration for array parameters instead of :cpp:expr:`type * name`
- Use typed pointers instead of :cpp:expr:`void *` pointers
- Widget constructor must follow the ``lv_<widget_name>_create(lv_obj_t * parent)`` pattern.
- Widget members function must start with ``lv_<widget_name>`` and should receive :cpp:expr:`lv_obj_t *` as first
  argument which is a pointer to widget object itself.
- ``struct`` APIs should follow the widgets' conventions. That is to receive a pointer to the ``struct`` as the
  first argument, and the prefix of the ``struct`` name should be used as the prefix of the
  function name too (e.g. :cpp:expr:`lv_display_set_default(lv_display_t * disp)`)
- Functions and ``struct``\ s which are not part of the public API must begin with underscore in order to mark them as "private".
- Argument must be named in H files too.
- Do not ``malloc`` into a static or global variables. Instead declare the variable in ``lv_global_t``
  structure in ``lv_global.h`` and mark the variable with :cpp:expr:`(LV_GLOBAL_DEFAULT()->variable)` when it's used.
- To register and use callbacks one of the following needs to be followed.

   - Pass a pointer to a ``struct`` as the first argument of both the registration function and the callback. That
     ``struct`` must contain ``void * user_data`` field.
   - The last argument of the registration function must be ``void * user_data`` and the same ``user_data``
     needs to be passed as the last argument of the callback.


To learn more refer to the documentation of `MicroPython <integration/bindings/micropython>`__.


Formatting
----------

Here is example to show bracket placing and using of white spaces:

.. code:: c

   /**
    * Set a new text for a label. Memory will be allocated to store the text by the label.
    * @param label pointer to a label object
    * @param text '\0' terminated character string. NULL to refresh with the current text.
    */
   void lv_label_set_text(lv_obj_t * label, const char * text)
   {   /*Main brackets of functions in new line*/

       if(label == NULL) return; /*No bracket only if the command is inline with the if statement*/

       lv_obj_inv(label);

       lv_label_ext_t * ext = lv_obj_get_ext(label);

       /*Comment before a section*/
       if(text == ext->txt || text == NULL) {  /*Bracket of statements start inline*/
           lv_label_refr_text(label);
           return;
       }

       ...
   }

Use 4 spaces indentation instead of tab.

You can use **astyle** to format the code. Run ``code-format.py`` from
the ``scripts`` folder.

pre-commit
----------

`pre-commit <https://pre-commit.com/>`__ is a multi-language package
manager for pre-commit hooks. See the `installation
guide <https://pre-commit.com/#installation>`__ to get pre-commit python
package installed into your development machine.

Once you have ``pre-commit`` installed you will need to `set up the git
hook scripts <https://pre-commit.com/#3-install-the-git-hook-scripts>`__
with:

.. code:: console

   pre-commit install

now ``pre-commit`` will run automatically on ``git commit``!

Hooks
-----

The ``format-source`` local hook (see ``.pre-commit-config.yaml``) runs
**astyle** on all the staged source and header files (that are not
excluded, see ``exclude`` key of the hook configuration) before entering
the commit message, if any file gets formatted by **astyle** you will
need to add the change to the staging area and run ``git commit`` again.

The ``trailing-whitespace`` hook fixes trailing whitespaces on all of
the files.

Skipping hooks
--------------

If you want to skip any particular hook you can do so with:

.. code:: console

   SKIP=name-of-the-hook git commit

Testing hooks
-------------

It's no necessary to do a commit to test the hooks, you can test hooks
by adding the files into the staging area and run:

.. code:: console

   pre-commit run name-of-the-hook
