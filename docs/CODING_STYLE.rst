.. _coding-style:

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
   (e.g. *MAX_LINE_NUM*)
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
Before every function prototype in ``.h`` files, include a Doxygen-formatted comment
like this:

.. code-block:: c

    /**
     * Brief description
     *
     * @return    short description of return value
     */
    example_type_t * lv_example(void);

Another example with arguments and more detail.

.. code-block:: c

    /**
     * Brief description
     *
     * Additional detail here in subsequent paragraphs.  Function accomplishes these results:
     * - description of result 1
     * - description of result 2
     *
     * @param  disp       object created by lv_x11_window_create()
     * @param  mouse_img  optional image description for mouse cursor
     *                        (NULL for no/invisible mouse cursor)
     *
     * @note  Include any subtle points an API user or maintainer would need to know.
     */
    void lv_other_example(lv_display_t * disp, lv_image_dsc_t const * mouse_img);

When documenting a code member that is *before* the comment, such as a struct member,
use ``/**<`` like this:

.. code-block:: c

    /**
     * Brief description of struct
     */
    typedef struct _lv_example_type_t {
        char      *text;    /**< Brief description of this member */
        uint16_t   length;  /**< Brief description of this member */
    } lv_example_type_t;

| When commenting code, use block comments like this ``/* Description */``,
| not end-of-line comments like this ``// Description``.

Include a space after the ``/*`` or ``/**<`` and before the ``*/`` for added readability.

Write readable code to avoid descriptive comments like:  ``x++; /* Add 1 to x */``.

The code should show clearly what you are doing.

You should write **why** you did it:  ``x++;  /* Point to closing '\0' of string */``

Short "code summaries" of a few lines are accepted: ``/* Calculate new coordinates */``

In comments use \`...\` when referring to a variable: ``/* Update value of `x_act` */``

When adding or modifying comments, priorities are (in order of importance):

    1.  clarity (the ease with which other programmers can understand your intention),
    2.  readability (the ease with which other programmers can read your comments),
    3.  brevity (the quality of using few words when speaking or writing).

Blank lines within comments are desirable when they improve clarity and readability.

Remember, when you are writing source code, you are not just teaching the computer
what to do, but also teaching other programmers how it works, not only users of the
API, but also future maintainers of your source code.  Comments add information
about what you were thinking when the code was written, and **why** you did things
that way -- information that cannot be conveyed by the source code alone.


Doxygen Comment Specifics
~~~~~~~~~~~~~~~~~~~~~~~~~
Doxygen is the first program in a data-flow chain that generates the online LVGL
API documentation from the files in the LVGL repository.  Doxygen detects comments
it should pay attention to by leading ``/**``.  It ignores comments that do not
have exactly two ``*``.

    | ``/** Description of code member AFTER this comment. */``
    | ``code_member_here {...}`` (e.g. function, struct, etc.)

    ``variable_or_struct_member;  /**< Description of code member BEFORE this comment. */``

Add 2 spaces after Doxygen commands (they start with '@') for improved readability.

Use \`...\` around variable names.  This *can* also be done with type, struct and
function names when it adds clarity or readability.  Append empty "()" after function
names. Doxygen generates a hyperlink to the type, struct or function's documentation
when it exists.  Note:  if you put any text inside the "()", Doxygen will not
generate a hyperlink in the generated coumentation.

    ``@param  disp   `lv_display_t` object created by lv_x11_window_create()``

Always include a brief description of the code member you are documenting.  For
documentation that appears *before* the code member, if more detail is needed,
include a blank line below the brief description and add the detail below it.
(Doxygen needs the blank line to separate the "brief description" from the
"additional detail", and it treats it accordingly.  The blank line also improves
readability in the source code.)  If a struct's member needs more than one line
to adequately describe it, it is acceptable to place the description before the
member.  In this case, remember to use ``/**``, not ``/**<``

To document a function's arguments, use the ``@param`` Doxygen command followed by 2
spaces and the name of the argument.  ``[in]``, ``[out]``, or ``[in,out]`` can be
appended to ``@param`` to clarify direction when it is important for clarity, like this:

    | ``@param[in]   value   input value``
    | ``@param[in]   factor  multiplier``
    | ``@param[out]  result  computed result``

Normally the brief description is simply a noun phrase like "``computed result``"
and so it neither needs to be capitalized nor does it need a period at the end.
However, when whole sentences are needed for clarity, capitalize the first letter
and use appropriate punctuation between sentences for clarity.

Align the beginning of each argument description for improved readability.  Provide
at least 2 spaces after the longest argument name for visual separation (improves
readability). If a description of an argument continues on subsequent lines, indent
the continuation lines by an additional 4 spaces to visually distinguish these lines
from the beginning of a new argument description, like this:

.. code-block:: c

      /**
       * Add event handler function for object `obj`.
       *
       * Used by user code to respond to event when it happens with object `obj`.
       * An object can have multiple event handlers.  They are called in the same
       * order as they were added.
       *
       * @param  obj        pointer to object to which to add event call-back
       * @param  filter     event code (e.g. `LV_EVENT_CLICKED`) indicating which
       *                        event should be called (`LV_EVENT_ALL` can be used
       *                        to receive all events)
       * @param  event_cb   address of event call-back function
       * @param  user_data  custom data to be made available to call-back function
       *                        in `e->user_data` field
       *
       * @return  handle to event (can be used in lv_obj_remove_event_dsc()).
       */
      lv_event_dsc_t * lv_obj_add_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data);

If you include a list of example values for an argument, do so by creating a
list using '-', like this:

.. code-block:: c

      /**
       * Set color format of display.
       *
       * @param  disp          pointer to display object
       * @param  color_format  possible values:
       *                           - LV_COLOR_FORMAT_RGB565
       *                           - LV_COLOR_FORMAT_RGB888
       *                           - LV_COLOR_FORMAT_XRGB888
       *                           - LV_COLOR_FORMAT_ARGB888
       *
       * @note  To change the endianness of rendered image in case of RGB565 format
       *        (i.e. swap the 2 bytes) call lv_draw_sw_rgb565_swap() in the `flush_cb`
       *        function.
       */
      void lv_display_set_color_format(lv_display_t * disp, lv_color_format_t color_format);

If a code example will be important to help other programmers better understand
how to use a function or data type (improving clarity), include an example using
the ``@code`` and ``@endcode`` Doxygen commands like this:

.. code-block:: c

      /**
       * Create X11 display.
       *
       * The minimal initialisation for X11 display driver with keyboard/mouse support:
       *
       * @code
       *     lv_display_t* disp = lv_x11_window_create("My Window Title", width, height);
       *     lv_x11_inputs_create(disp, NULL);
       * @endcode
       *
       * or with mouse cursor icon:
       *
       * @code
       *     lv_image_dsc_t mouse_symbol = {...};
       *     lv_display_t* disp = lv_x11_window_create("My Window Title", width, height);
       *     lv_x11_inputs_create(disp, &mouse_symbol);
       * @endcode
       *
       * @param  title    title of created X11 window
       * @param  hor_res  horizontal resolution (width) of X11 window
       * @param  ver_res  vertical resolution (height) of X11 window
       *
       * @return  pointer to display object
       */
      lv_display_t * lv_x11_window_create(char const * title, int32_t hor_res, int32_t ver_res);

To refer the reader to additional information, you can say something like
``See also `data_type_t`.`` or ``See function_name() for more information.``.
Doxygen will convert ``data_type_t`` or ``function_name()`` into a hyperlink to that
documentation when it exists.

If you create a new pair of ``.c`` and ``.h`` files (e.g. for a new driver), ensure a
Doxygen comment like this is at the top of each new file.  Doxygen will not parse the
file without it.

.. code-block:: c

      /**
       * @file filename.c
       *
       */


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

.. code-block:: c

   /**
    * Set new text for a label.  Memory will be allocated by label to store text.
    *
    * @param[in]  label  pointer to label object
    * @param[in]  text   '\0' terminated character string.
    *                        NULL to refresh with current text.
    */
   void lv_label_set_text(lv_obj_t * label, const char * text)
   {   /* Main brackets of functions in new line */

       if(label == NULL) return; /* No bracket only if command is inline with if statement */

       lv_obj_inv(label);

       lv_label_ext_t * ext = lv_obj_get_ext(label);

       /*Comment before a section*/
       if(text == ext->txt || text == NULL) {  /* Bracket of statements starts on same line */
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

.. code-block:: console

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

.. code-block:: console

   SKIP=name-of-the-hook git commit

Testing hooks
-------------

It is not necessary to do a commit to test the hooks, you can test hooks
by adding the files into the staging area and run:

.. code:: console

   pre-commit run name-of-the-hook
