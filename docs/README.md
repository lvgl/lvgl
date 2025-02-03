# Documentation


## Building

Building the documentation is pretty easy to do but it does have some requirements that have to be filled prior to building them.

Here are the requirements:

* Doxygen
* Python >= 3.10
* C compiler (gcc, msvc, clang, etc...)

There are also some Python specific libraries that need to be installed. You can either install these individually or you can use pip to read the requirements file to install everything that is needed for Python.

* Sphinx
* breathe
* imagesize
* importlib-metadata
* sphinx-rtd-theme
* sphinx-sitemap
* sphinxcontrib-applehelp
* sphinxcontrib-devhelp
* sphinxcontrib-htmlhelp
* sphinxcontrib-jsmath
* sphinxcontrib-qthelp
* sphinxcontrib-serializinghtml
* sphinxcontrib-mermaid==0.9.2
* sphinx-design
* sphinx-rtd-dark-mode
* typing-extensions
* sphinx-reredirects
* dirsync

To install using the `requirements.txt` file use the following command:

    pip install -r requirements.txt

Once you have all of the requirements installed you are ready to build the documentation.  Use the following command:

    python build.py skip_latex clean

You may have to use the following command if you are on a Unix like OS

    python3 build.py skip_latex clean

The documentation will be output into `./out_html/` in the root directory for LVGL.


## For Developers

The most important thing that has to be done when contributing to LVGL is ***EVERYTHING MUST BE DOCUMENTED***.

The below are some rules to follow when updating any of the `.rst` files located in the `./docs/` directory and any of it's subdirectories.


### What to Name Your `.rst` File

The documentation-generation logic uses the stem of the file name (i.e. "event" from file name "event.rst") and compares this with code-element names found by Doxygen.  If a match is found, then it appends hyperlinks to the API pages that contain those code elements (names of macros, enum/struct/union types, variables, namespaces, typedefs and functions).

If this is appropriate for the .RST file you are creating, ensure the stem of the file name matches the beginning part of the code-element name you want it to be associated with.

If this is *not* appropriate for the .RST file you are creating, ensure the stem of the file name DOES NOT match any code-element names found in the LVGL header files under the ./src/ directory.

In alignment with the above, use a file name stem that is appropriate to the topic being covered.


### Text Format

While with `.md` files, it is important to allow paragraphs to flow off to the right with one long line so that when they are formatted as `.html` files, the paragraphs will word-wrap with the width of the browser, this is not true with reStructuredText (`.rst` files).  [Sphinx](https://www.sphinx-doc.org/en/master/) and its underlying [docutils parsing engine](https://docutils.sourceforge.io/docs/) conveniently combine grouped text into a proper paragraph with that word-wrapping behavior.  This allows the source text documents to be nicely word-wrapped so that they are more readable in text- and code-editors that do not have wide editing windows.  So please wrap the text around column 86 or narrower.  Wrapping at *exactly* column 86 is not important, but readability and ease of editing is.


### index.rst Files

If you create a new directory you MUST have an `index.rst` file in that directory and that index file needs to be pointed to in the `index.rst` file that is located in the parent directory.

Let's take a look at the `index.rst` file that is located in the `docs/layouts` directory.

```
.. _layouts:

=======
Layouts
=======


.. toctree::
    :maxdepth: 2

    flex
    grid
```


The below explains the parts of this file.

```
.. _layouts:      <=== Creates an explicit link target
                  <=== Empty line -- important!
=======
Layouts           <=== Heading seen in documentation
=======


.. toctree::      <=== Table of contents
    :maxdepth: 2  <=== Internal use and needs to always be set this way

    flex          <=== .rst files located in directory with index.rst
    grid
```

The first line is for the purposes of providing a uniquely-named **link target** that can be referenced elsewhere in the documentation.

    .. _{LINK NAME}:

Note that `{LINK NAME}`:

- **must** be preceded by a single underscore, and
- **must** be followed by at least one blank line for the doc-generation logic to process it correctly.

Replace `{LINK NAME}` with a link name that is unique among all documents under the `./docs/` directory.  It can have multiple words if needed to make it unique or when otherwise appropriate for clarity.  If multiple words are used, they can be separated with single spaces, hyphens or underscores.  Whatever you use, the `{LINK NAME}` string used to reference it must be identical.  `{LINK NAME}` strings are not case sensitive.

That unique name is then used to provide a link reference elsewhere in the documentation using one of two formats.



##### When "link text" should be a title or section heading from the target document:

```reStructuredText
:ref:`{LINK NAME}`
```

This in-line markup (interpreted text using the Sphinx-defined custom `:ref:` role) is then replaced with a hyperlink whose "link text" is the name of the section heading just below the **link target**.  For this reason, when using this syntax, `{LINK NAME}` must reference **link target**s that are just above a title or section heading.



##### When "link text" should be something else:

```reStructuredText
:ref:`other link text <{LINK NAME}>`
```

This latter syntax enables you to put a **link target** anywhere in an .RST file (not just above a heading) and link to it using this syntax.

Note:  This latter syntax was either added or fixed in Sphinx recently.  It did not work in Sphinx 7.3.7.




### Section Headings

[Section headings](https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html#sections) are created by underlining (and optionally overlining) the section title with a punctuation character, at least as long as the text.  Example:

```
=================
This Is a Heading
=================
```

reStructuredText does not impose any particular heading levels assigned to certain characters since the structure is determined from the succession of headings.  So if you are modifying an existing .RST file, please follow the pattern already in use.

If you are creating a new .RST file, use this convention:

```
=====
Title
=====

Chapter
*******

Section
-------

Sub Section
~~~~~~~~~~~

Sub Sub Section
^^^^^^^^^^^^^^^

Sub Sub Sub Section
'''''''''''''''''''
```

Note that the "underlining" can be longer than the heading title, but if it is shorter, the documentation-generation logic will fail with an error.

For improved readability in the .RST file, place at least 2 blank lines above section headings.


### Code Blocks

* Do not use tab characters in code blocks.
* Each indentation level use 4 spaces.
* Include at least 1 empty line after a code block.
* There must be one empty line between the code block directive and the code.
* `.. code-block::` is the only directive that should be used.  Note carefully that unlike the **link target** directive above, this directive has 2 colons.  (The only ReST and sphinx directives that are valid with one colon are **link target**s as shown above.)  Lone `::`, `:code:` or `.. code:` should not be used.
* If you want to separate code into easier-to-understand sections you can do so with a single empty line.
* For syntax highlighting appropriate to the language in the code block, specify the language after the directive.  Some examples are:

  - `.. code-block:: c`,
  - `.. code-block:: cpp`,
  - `.. code-block:: python`,
  - `.. code-block:: shell`,
  - `.. code-block:: kconfig`,
  - `.. code-block:: json`,
  - `.. code-block:: yaml`,
  - `.. code-block:: csharp` (or "cs"),
  - `.. code-block:: vb.net`,
  - `.. code-block:: dot` (graphviz),
  - `.. code-block:: html`,
  - `.. code-block:: css`,
  - `.. code-block:: xml`,
  - `.. code-block:: make`.

The full set of supported lexers are listed here:  https://pygments.org/docs/lexers/ .


### Bulleted Lists

To create a bulleted list, do the following:

    - First item description
    - If you want to span multiple lines, indent subsequent
      lines to align with item text like this.
    - If you want to include a code block under a list item,
      it must be intended to align with the list item like this:

      .. code-block: python
                                 <=== blank line here is important
          # this is some code
                                 <=== blank line here is important
    - If you want to have nested bulleted lists, indent each
      new level to align with its parent list item like this:
                                 <=== blank line here is important
      - level 2 item 1: text
      - level 2 item 2: text
                                 <=== blank line here is important
    - Last list item.  Note that the nested list above is preceded
      and followed by 1 blank line.

All lists (including nested lists) **must** be preceded and followed with at least 1 blank line.  This is mandatory for the documentation-generation logic to process it correctly.


### Referencing API Documentation

If you want to reference portions of the LVGL code from the documentation (in .RST files) there are special directives to do this:

    :cpp:func:`lv_init`
    :c:macro:`LV_USE_FLEX`
    :cpp:type:`lv_event_t`
    :cpp:enum:`_lv_event_t`
    :cpp:enumerator:`LV_EVENT_ALL`
    :cpp:struct:`lv_image_dsc_t`
    :cpp:union:`lv_style_value_t`

There is a special directive when wanting to use a more complex expression.  For example when showing the arguments passed to a function.

    :cpp:expr:`lv_obj_set_layout(widget, LV_LAYOUT_FLEX)`
    :cpp:expr:`lv_slider_set_mode(slider, LV_SLIDER_MODE_...)`

Arguments that are expressions (more than one word), or contain non-alphanumeric characters will cause the `:cpp:expr:` interpreted-text to fail.  Examples:

    :cpp:expr:`lv_obj_set_layout(widget, LV_LAYOUT_FLEX/GRID)`         <== arg with > 1 word
    :cpp:expr:`lv_obj_set_layout(widget, LV_LAYOUT_*)`                 <== asterisk
    :cpp:expr:`lv_obj_set_layout(*widget, LV_LAYOUT_FLEX)`             <== asterisk
    :cpp:expr:`lv_obj_set_layout((lv_obj_t *)widget, LV_LAYOUT_FLEX)`  <== cast
    :cpp:expr:`lv_obj_set_layout(&widget, LV_LAYOUT_FLEX);`            <== ampersand & semicolon
    :cpp:expr:`lv_obj_set_layout(widget, ...)`                         <== lone ellipsis

For such examples, simply use reStructuredText literal markup like this:

    ``lv_obj_set_layout(widget, LV_LAYOUT_FLEX/GRID)``
    ``lv_obj_set_layout(widget, LV_LAYOUT_*)``
    ``lv_obj_set_layout(*widget, LV_LAYOUT_FLEX)``
    ``lv_obj_set_layout((lv_obj_t *)widget, LV_LAYOUT_FLEX)``
    ``lv_obj_set_layout(&widget, LV_LAYOUT_FLEX);``
    ``lv_obj_set_layout(widget, ...)``


