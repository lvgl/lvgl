.. _coding-style:

============
Coding Style
============

File Template
*************

Use `misc/lv_templ.c <https://github.com/lvgl/lvgl/blob/master/src/misc/lv_templ.c>`__
and `misc/lv_templ.h <https://github.com/lvgl/lvgl/blob/master/src/misc/lv_templ.h>`__


Naming Conventions
******************

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


Coding Guide
************

-  Editor:

   -  Set editor to use 4 spaces for tab indentations (instead of tab characters).
   -  Exception:  the **Kconfig** file and any make files require leading tab characters
      on child items.

-  Functions:

   -  Write functions that use the single-responsibility principle.
   -  Make functions ``static`` when not part of that object's public API (where possible).

-  Variables:

   -  One line, one declaration (BAD: char x, y;).
   -  Use ``<stdint.h>`` (*uint8_t*, *int32_t* etc).
   -  Declare variables where needed (not all at function start).
   -  Use the smallest required scope.
   -  Variables in a file (outside functions) are always *static*.
   -  Do not use global variables (use functions to set/get static variables).


Comments
********

Before every function prototype in ``.h`` files, include a Doxygen-formatted comment
like this:

.. code-block:: c

    /**
     * Brief description.  Add a blank line + additional paragraphs when more detail is needed.
     * @param  parent     brief description of argument.  Additional detail can appear
     *                    on subsequent lines.  List of accepted values:
     *                    - value one
     *                    - value two
     *                    - value three
     * @return   brief description of return value.
     */
    type_name_t * lv_function_name(lv_obj_t * parent);

The normal comment prefix ``/**`` causes the comment to document the code member
*after* the comment.  When documenting a code member that is *before* the
comment, such as a struct member, use ``/**<`` like this:

.. code-block:: c

    /**
     * Brief description of struct
     *
     * When more detail is needed, add a blank line then the detail.
     */
    typedef struct {
        char      *text;    /**< Brief description of this member */
        uint16_t   length;  /**< Brief description of this member */
    } lv_example_type_t;

-  When commenting code, use block comments like this ``/* Description */``,
   not end-of-line comments like this ``// Description``.

-  Include a space after the ``/*`` or ``/**<`` and before the ``*/`` for improved readability.

-  Write readable code to avoid descriptive comments like:  ``x++; /* Add 1 to x */``.

-  The code should show clearly what you are doing.

-  You should write **why** you did it:  ``x++;  /* Point to closing '\0' of string */``

-  Short "code summaries" of a few lines are accepted: ``/* Calculate new coordinates */``

-  In comments use back-quotes (\`...\`) when referring to a code element, such as a variable, type,
   or struct name: ``/* Update value of `x_act` */``

-  When adding or modifying comments, priorities are (in order of importance):

       1.  clarity (the ease with which other programmers can understand your intention),
       2.  readability (the ease with which other programmers can read your comments),
       3.  brevity (the quality of using few words when speaking or writing).

-  Blank lines within comments are desirable when they improve clarity and readability.

-  Remember, when you are writing source code, you are not just teaching the computer
   what to do, but also teaching other programmers how it works, not only users of the
   API, but also future maintainers of your source code.  Comments add information
   about what you were thinking when the code was written, and **why** you did things
   that way---information that cannot be conveyed by the source code alone.


Doxygen Comment Specifics
-------------------------
Doxygen is the first program in a chain that generates the online LVGL API
documentation from the files in the LVGL repository.  Doxygen detects files it should
pay attention to by them having a ``@file`` command inside a Doxygen comment.  Doxygen
comments begin with a leading ``/**``.  It ignores comments that do not have exactly
two ``*``.

The following is an illustration of an API function prototype with documentation
illustrating most of the Doxygen commands used in LVGL.

.. code-block:: c

    /**
     * Set alignment of Widgets placed in containers with LV_STYLE_FLEX_FLOW style.
     *
     * The values for the `..._place` arguments come from the `lv_flex_align_t`
     * enumeration and have the same meanings as they do for flex containers in CSS.
     * @param  obj                   pointer to flex container.  It must have
     *                               `LV_STYLE_FLEX_FLOW` style or nothing will happen.
     * @param  main_place            where to place items on main axis (in their track).
     *                               (Any value of `lv_flex_align_t`.)
     * @param  cross_place           where to place item in track on cross axis.
     *                               - `LV_FLEX_ALIGN_START`
     *                               - `LV_FLEX_ALIGN_END`
     *                               - `LV_FLEX_ALIGN_CENTER`
     * @param  track_cross_place     where to place tracks in cross direction.
     *                               (Any value of `lv_flex_align_t`.)
     * Example for a title bar layout:
     * @code{.c}
     *     lv_obj_set_flex_align(title_bar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
     * @endcode
     * @see
     *     - https://css-tricks.com/snippets/css/a-guide-to-flexbox/
     *     - see  `lv_obj_set_flex_grow()` for additional information.
     */
    void lv_obj_set_flex_align(lv_obj_t * widget, lv_flex_align_t main_place, lv_flex_align_t cross_place,
                               lv_flex_align_t track_cross_place);


- Always start Doxygen comment with a brief description of the code element it documents.

- When more detail is needed, add a blank line below the brief description and add
  additional information that may be needed by LVGL API users, including preconditions
  for calling the function.  Doxygen needs the blank line to separate "brief" from
  "detail" portions of the description.

- Describe function parameters with the ``@param`` command.  When a function writes
  to the address contained by a pointer parameter, if not already obvious (e.g. when
  the parameter name contains the word "out"), include the direction in the command
  for clarity:

      ``@param[out]  param_name     description``.

- Describe return values with the ``@return`` command.

- Add at least 2 spaces after Doxygen commands for improved readability.

- Use back-quotes (\`...\`) around code elements (variables, type names, function names).  For type
  names and function names, Doxygen generates a hyperlink to that code member's
  documentation (when it exists) with or without the single back-quotes.

- Append empty "()" to function names.  Doxygen will not generate a hyperlink to the
  function's documentation without this.

- Use proper grammar for clarity.  Descriptions of parameters do not need periods
  after them unless they are full sentences.

- Align edges of text around lists of parameters for ease of reading.

- Lists (e.g. of accepted parameter values) can be created by using the '-' character.
  If the list needs to be numbered, numbers can also be used.

- Place example code in a code block by surrounding it with ``@code{.c}`` and ``@endcode`` commands.
  Doxygen doesn't need the ``{.c}`` part, but the downstream software does.

- Refer reader to additional information using the ``@see`` command.  Doxygen adds a
  "See also" paragraph.  The text following the ``@see`` command will be indented.

- If you create a new pair of ``.c`` and ``.h`` files (e.g. for a new driver), ensure
  a Doxygen comment like this is at the top of each new file.  Doxygen will not parse
  the file without the ``@file`` command being present.

.. code-block:: c

      /**
       * @file filename.c
       *
       */


Supported Doxygen Commands
--------------------------
-  ``@file``
   tells Doxygen to parse this file and also supplies documentation about
   the file itself when applicable (everything following it in the same comment).
-  ``@param  name  description``
   documents ``name`` as a function parameter, and ``description`` is the text that
   follows it until Doxygen encounters a blank line or another Doxygen command.
-  ``@return  description``
   documents the return value until Doxygen encounters a blank line or another Doxygen command.
-  ``@code{.c}/@endcode``
   surrounds code that should be placed in a code block.  While Doxygen knows to use C
   color-coding of code blocks in a .C file, the downstream part of the documentation
   generation sequence does not, so the ``{.c}`` appendage to the ``@code`` command
   is necessary.
-  ``@note  text``
   starts a paragraph where a note can be entered.  The note ends with a blank line,
   the end of the comment, or another Doxygen command that starts a new section.
   If the note contains more than one paragraph, additional paragraphs can be added
   by using additional ``@note`` commands.  At this writing, ``@par`` commands do not
   add additional paragraphs to notes as indicated in the Doxygen documentation.
-  ``@see  text``
   generates a "See also" pagraph in a highlighted section, helpful when additional
   information about a topic can be found elsewhere.


API Conventions
***************

To support the auto-generation of bindings, the LVGL C API must
follow some coding conventions:

- Use ``enum``\ s instead of macros. If inevitable to use ``define``\ s
  export them with :cpp:expr:`LV_EXPORT_CONST_INT(defined_value)` right after the ``define``.
- In function arguments use ``type name[]`` declaration for array parameters instead of :cpp:expr:`type * name`
- Use typed pointers instead of :cpp:expr:`void *` pointers
- Widget constructor must follow the ``lv_<widget_name>_create(lv_obj_t * parent)`` pattern.
- Widget members function must start with ``lv_<widget_name>`` and should receive :cpp:expr:`lv_obj_t *` as first
  argument which is a pointer to Widget object itself.
- ``struct`` APIs should follow the widgets' conventions. That is to receive a pointer to the ``struct`` as the
  first argument, and the prefix of the ``struct`` name should be used as the prefix of the
  function name as well (e.g. :cpp:expr:`lv_display_set_default(lv_display_t * disp)`)
- Functions and ``struct``\ s which are not part of the public API must begin with underscore in order to mark them as "private".
- Argument must be named in H files as well.
- Do not ``malloc`` into a static or global variables. Instead declare the variable in ``lv_global_t``
  structure in ``lv_global.h`` and mark the variable with :cpp:expr:`(LV_GLOBAL_DEFAULT()->variable)` when it's used.
- To register and use callbacks one of the following needs to be followed.

   - Pass a pointer to a ``struct`` as the first argument of both the registration function and the callback. That
     ``struct`` must contain ``void * user_data`` field.
   - The last argument of the registration function must be ``void * user_data`` and the same ``user_data``
     needs to be passed as the last argument of the callback.


To learn more refer to the documentation of `MicroPython <integration/bindings/micropython>`__.


Formatting
**********

Here is example to show bracket placement and use of white space:

.. code-block:: c

   /**
    * Set new text for a label.  Memory will be allocated by label to store text.
    *
    * @param  label  pointer to label Widget
    * @param  text   '\0' terminated character string.
    *                NULL to refresh with current text.
    */
   void lv_label_set_text(lv_obj_t * label, const char * text)
   {   /* Main brackets of functions in new line */

       if(label == NULL) return; /* No bracket only if command is inline with if statement */

       lv_obj_inv(label);

       lv_label_ext_t * ext = lv_obj_get_ext(label);

       /* Comment before a section */
       if(text == ext->txt || text == NULL) {  /* Bracket of statements starts on same line */
           lv_label_refr_text(label);
           return;
       }

       ...
   }

You can use **astyle** to format the code. Run ``code-format.py`` from
the ``scripts`` folder.


Includes
********

Various subsystems of LVGL can be enabled or disabled by setting a macro in
``lv_conf.h`` to 1 or 0 respectively.  The code files that contain the logic for
those subsystems are often arranged so that there is an ``#if <ENABLING_MACRO_NAME>``
directive near the top of the file, and its matching ``#endif`` is at the end of the
file.  If you add or modify such a subsystem in LVGL, whenever possible, the only
``#include`` that should be above those conditional directives should be just enough
to include the enabling/disabling macro.  Specifically:

- in the ``.c`` file:  the ``#include`` that includes the header with the closest
  correspondence to that ``.c`` file

- in the mated ``.h`` file:  ``#include "lv_conf_internal.h"``

which, itself includes ``lv_conf.h``.  See examples at
`lv_freetype.c <https://github.com/lvgl/lvgl/blob/master/src/libs/freetype/lv_freetype.c>`__,
`lv_freetype_private.h <https://github.com/lvgl/lvgl/blob/master/src/libs/freetype/lv_freetype_private.h>`__ and
`lv_freetype.h <https://github.com/lvgl/lvgl/blob/master/src/libs/freetype/lv_freetype.h>`__.


pre-commit
**********

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
*****

The ``format-source`` local hook (see ``.pre-commit-config.yaml``) runs
**astyle** on all the staged source and header files (that are not
excluded, see ``exclude`` key of the hook configuration) before entering
the commit message, if any file gets formatted by **astyle** you will
need to add the change to the staging area and run ``git commit`` again.

The ``trailing-whitespace`` hook fixes trailing whitespaces on all of
the files.


Skipping hooks
**************

If you want to skip any particular hook you can do so with:

.. code-block:: console

   SKIP=name-of-the-hook git commit


Testing hooks
*************

It is not necessary to do a commit to test the hooks, you can test hooks
by adding the files into the staging area and run:

.. code:: console

   pre-commit run name-of-the-hook

