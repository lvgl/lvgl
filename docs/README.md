# Documentation


## Building

Building the documentation is easy.  Here are the requirements:

- Doxygen
- Python >= 3.10
- C compiler (gcc, msvc, clang, etc...)

Once Python is installed

    pip install -r requirements.txt

will install all the prerequisite packages.

To build the documentation on Windows:

    python build.py html

On Linux:

    python3 build.py html

Intermediate files are normally prepared in `./docs/intermediate/` and the final documentation will normally appear in `./docs/build/html/`.  Both of these directories can be overridden using environment variables.  See documentation in `build.py` header comment for details.

If the list of document source files (including the `.h` files in the `./src/` directory) has changed (names or paths):

    python build.py clean

Will remove the old intermediate and build files, eliminating the orphan files that would otherwise result.

To see a list of options available:

    python build.py

Read the header comment in `build.py` for detailed documentation on each option.



## For Developers

One of our firm policies is ***EVERYTHING MUST BE DOCUMENTED***.

The below are some rules to follow when updating any of the `.rst` files located in the `./docs/src/` directory tree.



## reStructuredText Content

LVGL documentation uses **reStructuredText** (reST), rendered into HTML by Sphinx.  You will find the following is a fairly-complete list of references about how to do things using reStructuredText:

| Docutils References (Fundamentals)                                                | Sphinx References (What Sphinx Adds to Docutils)                               |
| --------------------------------------------------------------------------------- | ------------------------------------------------------------------------------ |
| [Introduction](https://docutils.sourceforge.io/docs/ref/rst/introduction.html)    | [Configuration](https://www.sphinx-doc.org/en/master/usage/configuration.html) |
| [Markup Specification](https://docutils.sourceforge.io/docs/ref/rst/restructuredtext.html) | [Directives](https://www.sphinx-doc.org/en/master/usage/restructuredtext/directives.html) |
| [Markup Specification ∙ Tables](https://docutils.sourceforge.io/docs/ref/rst/restructuredtext.html#tables) | [Directives ∙ Admonitions](https://www.sphinx-doc.org/en/master/usage/restructuredtext/directives.html#admonitions-messages-and-warnings) |
| [Markup Specification ∙ Substitution References](https://docutils.sourceforge.io/docs/ref/rst/restructuredtext.html#substitution-references) | [Referencing](https://www.sphinx-doc.org/en/master/usage/referencing.html) |
| [Directives](https://docutils.sourceforge.io/docs/ref/rst/directives.html)        | [Interpreted Text Roles](https://www.sphinx-doc.org/en/master/usage/restructuredtext/roles.html) |
| [Interpreted Text Roles](https://docutils.sourceforge.io/docs/ref/rst/roles.html) | [Glossary](https://www.sphinx-doc.org/en/master/glossary.html)                 |
| [Quickstart](https://docutils.sourceforge.io/docs/user/rst/quickstart.html)       | [Furo-theme Examples](https://sphinx-themes.org/sample-sites/furo/)            |
| [Examples](https://docutils.sourceforge.io/docs/user/rst/demo.html)               | [Kitchen Sink Page](https://sphinx-themes.org/sample-sites/furo/kitchen-sink/) |
| [Quick Reference](https://docutils.sourceforge.io/docs/user/rst/quickref.html)    | [Sphinx Themes Gallery](https://sphinx-themes.org/)                            |

If you prefer to learn by examples, the [Furo-theme Examples](https://sphinx-themes.org/sample-sites/furo/) and especially the [Kitchen Sink Page](https://sphinx-themes.org/sample-sites/furo/kitchen-sink/) are excellent resources.  View the `.rst` source file that generated that page by clicking the "eye" icon at the top of the page.

Note:  the section headings in these pages use a different convention than the one presented below.  For LVGL documentation, use the [section-heading convention presented below](https://github.com/lvgl/lvgl/tree/master/docs#section-headings).



### Text Format

Please wrap the text around column 86 or narrower.  Wrapping at *exactly* column 86 is not important, but readability and ease of editing is.

Indent using 4 spaces (not tab characters).  This applies to code blocks as well.



### index.rst Files

If you create a new directory you will need an `index.rst` file in that directory and that index file needs to be pointed to in the `index.rst` file that is located in the parent directory.

Let's take a look at the `index.rst` file that is located in the `docs/src/common-widget-features/layouts` directory.

```rst
.. _layouts:

=======
Layouts
=======


.. toctree::
    :maxdepth: 2

    overview
    flex
    grid
```


Explanation:

```rst
.. _layouts:      <=== Creates an explicit link target
                  <=== Empty line -- important!
=======
Layouts           <=== Document title, seen in documentation
=======

                  <=== any text introducing this topic and the TOC below

.. toctree::      <=== Table of contents directive
    :maxdepth: 2  <=== Internal use and needs to always be set this way

    overview      <=== relative path to .rst files located in the same directory
    flex
    grid
```



### Section Headings

Section headings are created by underlining the section title with a punctuation character, at least as long as the text.  Example:

```
This Is a Heading
*****************
```

Use these conventions for section headings in LVGL documentation:

```
==============
Document Title
==============

Section
*******

Sub Section
-----------

Sub Sub Section
~~~~~~~~~~~~~~~
```

1.  ``====``, ``****``, ``----`` are encouraged.
2.  ``~~~~`` only if really needed.

Being consistent about this helps the reST parser to format the tables of contents correctly.

For improved readability in text editors:

- place 3 blank lines above the 2nd and subsequent "Section" titles (see above),
- 2 blank lines above "Sub Section" headings, and
- 1 at least blank line above all lower-level section headings.



### Italics, Boldface and Underlining

Emphasis using `*italics*`.  Emphasis using `**boldface**`.

Normally underlining and combining these text styles is not possible in reStructuredText.  However, LVGL documentation provides a work-around for this using reST <u>Interpreted Text Roles</u>.  Just remember that the Interpreted Text Role names combine the letters `i`, `b` and `u` to provide the desired combination.  All possible permutations of these letters are supported so you do not have to remember what sequence works.  Examples:  ``:u:`underline` ``, ``:ub:`underline and bold` ``, `` :bi:`bold italic` ``.



### Code Blocks

* Indent using 4 spaces (not tab characters).
* Include at least 1 empty line after a code block.
* There must be one empty line between the code block directive and the code.
* `.. code-block::` is the only directive that should be used.  Do not use lone `::`, `:code:` or `.. code::`.
* Specify the language after the directive for appropriate syntax highlighting.  Examples:

  - `.. code-block:: c`,
  - `.. code-block:: cpp`,
  - `.. code-block:: python`,
  - `.. code-block:: shell`,
  - `.. code-block:: bash`,
  - `.. code-block:: kconfig`,
  - `.. code-block:: json`,
  - `.. code-block:: yaml`,
  - `.. code-block:: dot` (graphviz),
  - `.. code-block:: html`,
  - `.. code-block:: css`,
  - `.. code-block:: xml`,
  - `.. code-block:: make`.

See [the full set of supported code lexers](https://pygments.org/docs/lexers/) for more details.



### Bulleted Lists

```rst
- First item description
- If you want to span multiple lines, indent subsequent
  lines to align with item text like this.
- If you want to include multiple paragraphs and/or code blocks under a
  list item, it must be intended to align with the list item like this:

  Second paragraph.

  .. code-block: python
                             <=== blank line here is important
      # Python code here
                             <=== blank line here is important
- If you want to have nested bulleted lists, indent each
  new level to align with its parent list item like this:
                             <=== blank line here is important
  - level 2 item 1: text
  - level 2 item 2: text
                             <=== blank line here is important
- Last list item.  Note that the nested list above is preceded
  and followed by 1 blank line.
```

All lists (including nested lists) **must** be preceded and followed with at least 1 blank line for the reST parser to process it correctly.



### External Links

URLs are converted to links automatically. E.g. `Visit https://lvgl.io`.

To add links with custom link text use

```rst
Visit `My Website <https://pro.lvgl.io>`__.
```

If an external link will be used on many pages:

- Add it to `./docs/src/include/external_links.txt` if not already there.  Example:

  ```rst
  .. _LVGL Pro:  https://pro.lvgl.io
  ```

- `.. include:  /include/external_links.txt` once at the top of each `.rst` file that uses it.

- Use it by name in text:

  ```rst
  For further details see `LVGL Pro`_.
  ```

  Note:  back-quotes are not needed if there are no spaces in the name.



### Internal Links

Add a link target (anchor) before heading or paragraph that will be linked to:

```rst
.. _unique_anchor_name:

My Heading
**********
```

`unique_anchor_name` must be unique throughout all `.rst` files under `./docs/src/`.

Reference the link (anchor) by:

```rst
Click :ref:`here <unique_anchor_name>` for more details.
```

Result:  "Click **_here_** for more details."

Or use the heading's text as the link's text:

```rst
Click :ref:`unique_anchor_name` for more details.
```

Result:  "Click **_My Heading_** for more details."

`unique_anchor_name` may appear in places other than before a heading, but if so, custom link text (like "here" the first example above) must be provided.



### Tightening Tables

reStructuredText syntax for creating tables can be found in the [reST examples](https://sphinx-themes.org/sample-sites/furo/kitchen-sink/tables/) referred to above.  (Click on the "eye" icon to see the source file.)

Very long or very wide tables can be difficult to read and use.  To squeeze them down to make them more readable and usable, move your existing table under a `.. container:: tighter-table-N` directive (`N` = digit 1-7 with 7 being the tightest), and indent your table to make it the "content" of the directive.  Example:

```rst
.. container:: tighter-table-3

    +-----------+--------------+--------------+--------+
    | Heading 1 | Heading 2    | Heading 3    | Hdg 4  |
    +===========+==============+==============+========+
    | row 1 c 1 | row 1 col 3  | row 1 col 3  | r1 c4  |
    +-----------+--------------+--------------+--------+
    | row 2 c 1 | row 2 col 3  | row 2 col 3  | r2 c4  |
    +-----------+--------------+--------------+--------+
    | row 3 c 1 | row 3 col 3  | row 3 col 3  | r3 c4  |
    +-----------+--------------+--------------+--------+
```

This works for all types of tables.



### Special Symbols

Because not everyone has editors that deal with Unicode characters well, please reST substitutions to insert special characters into documentation.  A list of the most commonly-used special symbols can be found in `./docs/src/include/substitutions.txt`.  To use one of these, add this line at the top of the `.rst` file if it is not already there:

```rst
.. include:: /include/substitutions.txt
```

Then, any of those substitutions can be used in that `.rst` file.  Example:

```rst
The temperature outside is 20\ |deg|\ C.
```

Result:  "The temperature outside is 20°C."

The spaces surrounding substitutions *are required for parsing*, but when you need to remove them in the output (as in the example above), do so by escaping them with the `\` character.  Exception:  the `substitutions.txt` file contains 3 substitution definitions which are marked with the `:trim:` option since their use *always* removes these spaces in the output.  These do not need this escaping:

- `|nbsp|` (non-breaking space),
- `|shy|` (soft hyphen), and
- `|nbhyph|` (non-breaking hyphen used in titles and official names)

If you need a substitution that is not already in `substitutions.txt`, please add it.



### Referencing API Documentation

Using the following generates links to API documentation that the reader can click directly in the text.

#### In-Line Code Expressions

Use the following Interpreted Text Roles in text to include in-line C code that links to documentation on that symbol when available:

    :cpp:func:`lv_init`   (note there are no parentheses after the function name)
    :c:macro:`LV_USE_FLEX`
    :cpp:type:`lv_event_t`
    :cpp:enum:`lv_state_t`
    :cpp:enumerator:`LV_STATE_CHECKED`
    :cpp:struct:`lv_image_dsc_t`
    :cpp:union:`lv_style_value_t`

#### More Complex Expressions

Use the `:cpp:expr:` Interpreted Text Role for more complex expressions, for example when showing the arguments passed to a function.

    :cpp:expr:`lv_obj_set_layout(widget, LV_LAYOUT_FLEX)`
    :cpp:expr:`lv_slider_set_mode(slider, LV_SLIDER_MODE_...)`

Arguments that contain more than one word or non-alphanumeric characters will cause the `:cpp:expr:` interpreted-text to fail.  Examples:

| Expression                                                   | Cause of Failure       |
| ------------------------------------------------------------ | ---------------------- |
| :cpp:expr:\`lv_obj_set_layout(widget, LV_LAYOUT_FLEX/GRID)\` | argument with > 1 word |
| :cpp:expr:\`lv_obj_set_layout(widget, LV_LAYOUT_*)\`         | asterisk               |
| :cpp:expr:\`lv_obj_set_layout(*widget, LV_LAYOUT_FLEX)\`     | asterisk               |
| :cpp:expr:\`lv_obj_set_layout((lv_obj_t *)widget, LV_LAYOUT_FLEX)\` | cast            |
| :cpp:expr:\`lv_obj_set_layout(&widget, LV_LAYOUT_FLEX);\`    | semicolon              |
| :cpp:expr:\`lv_obj_set_layout(widget, ...)\`                 | lone ellipsis          |

For such examples, simply use reStructuredText literal markup like this:

```rst
``lv_obj_set_layout(widget, LV_LAYOUT_FLEX/GRID)``
``lv_obj_set_layout(widget, LV_LAYOUT_*)``
``lv_obj_set_layout(*widget, LV_LAYOUT_FLEX)``
``lv_obj_set_layout((lv_obj_t *)widget, LV_LAYOUT_FLEX)``
``lv_obj_set_layout(&widget, LV_LAYOUT_FLEX);``
``lv_obj_set_layout(widget, ...)``
```

#### Providing Links to API Pages

To create a link to 1 or more API pages, set up a section at the end of your `.rst` file that looks like this, and use one or both types of the ``.. API `` pseudo-directives below:

```rst
API
***

.. API equals: lv_scale_t, lv_scale_create

.. API startswith:

    lv_scale
    lv_obj_set_style
```

The list of symbols (or prefixes) can be separated by commas or spaces, and they can wrap onto subsequent lines of text so long as they are indented.  Each list is terminated by the next ``.. API `` pseudo-directive or end-of-file, whichever comes first.

The API-page generation logic will add at most 1 link to each API documentation page containing matched symbols.  The links are to the whole API page, not to the symbols.  The purpose is to provide the reader with links to applicable API pages.  Links directly to code (e.g. function documentation) are accomplished by using the In-Line Code Expressions documented above.
