# Documentation


## Building

Building the documentation is easy.  Here are the requirements:

* Doxygen
* Python >= 3.10
* C compiler (gcc, msvc, clang, etc...)

Once Python is installed

    pip install -r requirements.txt

will install all the prerequisite packages:

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
* sphinx-copybutton
* sphinx-design
* typing-extensions
* sphinx-reredirects
* dirsync
* furo
* accessible-pygments

Now you are ready to build the documentation:

    python build.py html

or if you are on a Unix-like OS:

    python3 build.py html

Intermediate files are normally prepared in `./docs/intermediate/` and the final documentation will normally appear in `./docs/build/html/`.  (Both of these directories can be overridden using environment variables.  See documentation in `build.py` header comment for details.)

If the list of document source files has changed (names or paths):

    python build.py clean html

Will remove the old intermediate and build files and regenerate new ones matching the new structure, eliminating the orphan files that would otherwise result.

To see a list of options available:

    python build.py

Read the header comment in `build.py` for detailed documentation of each option.




## For Developers

One of our firm policies is ***EVERYTHING MUST BE DOCUMENTED***.

The below are some rules to follow when updating any of the `.rst` files located in the `./docs/src/` directory tree.



### reStructuredText

LVGL documentation uses **reStructuredText**.  The definitive reStructuredText reference is https://docutils.sourceforge.io/docs/ref/rst/restructuredtext.html.  Supplemented by [the Sphinx Documentation](https://www.sphinx-doc.org/en/master/usage/index.html), you should be able to find any any all reStructuredText tools you will ever need while creating or modifying LVGL documentation.



### Example-Based reStructuredText Quick-Reference

Because LVGL Documentation is currently using the `Furo` theme, this link to the [Furo-theme examples](https://sphinx-themes.org/sample-sites/furo/) takes you to a set of HTML pages whose source files can be very instructive about how to do different things using **reStructuredText**.  Just find an example of what you are trying to do (e.g. tables), and then click the "eye" icon at the top-right of that page, and it will take you to the `.rst` source file that was used to generate that HTML page.  Note that the sub-pages of the ["Kitchen Sink" page](https://sphinx-themes.org/sample-sites/furo/kitchen-sink/) contain a wide variety of **reStructuredText** constructs:  lists, tables, structural elements, admonitions, images, figures, typography, etc..

Note:  the section headings in these pages use a different convention than the one presented below.  For LVGL documentation, use the [section-heading convention presented below](https://github.com/lvgl/lvgl/tree/master/docs#section-headings).




### What to Name Your `.rst` File

The directory structure under the `./docs/src/` directory, and the filenames of the `.rst` files govern the eventual URLs that are generated in the HTML output.  These directories are organized so as to reflect the nature of the content.  Example:  the `.rst` files under `./docs/src/intro` contain introductory material—detailed reference material would not go there, but instead in an appropriate subdirectory of `./docs/src/details/`.  It is expected that the content and location of any new documents added would be in alignment with this directory structure, and placed and named according to their content.  Additionally, to be linked into the eventual generated documentation, the stem of the new filename needs to appear in at least one (normally *only one*) `.. toctree::` directive, normally in an `index.rst` file in the directory where it will appear in that page's table of contents (TOC).

Other than that, there are no restrictions on filenames.  Previous linking of filenames to generated API links has been removed and replaced by a better scheme.




### Text Format

With `.md` files, it is important to allow paragraphs to flow off to the right with one long line so that when they are formatted as `.html` files, the paragraphs will word-wrap with the width of the browser.  Thankfully, this liability is not present with reStructuredText (`.rst` files).  [Sphinx](https://www.sphinx-doc.org/en/master/) and its underlying [docutils parsing engine](https://docutils.sourceforge.io/docs/) conveniently combine grouped text into a proper paragraph with that word-wrapping behavior.  This allows the source text documents to be nicely word-wrapped so that they are more readable in text- and code-editors that do not have wide editing windows.  Please wrap the text around column 86 or narrower.  Wrapping at *exactly* column 86 is not important, but readability and ease of editing is.




### index.rst Files

If you create a new directory you will need an `index.rst` file in that directory and that index file needs to be pointed to in the `index.rst` file that is located in the parent directory.

Let's take a look at the `index.rst` file that is located in the `docs/src/details/common-widget-features/layouts` directory.

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
Layouts           <=== Document title, seen in documentation
=======

                  <=== any text introducing this topic and the TOC below

.. toctree::      <=== Table of contents directive
    :maxdepth: 2  <=== Internal use and needs to always be set this way

    flex          <=== relative path to .rst files located in the same directory
    grid
```

The first line is for the purposes of providing a uniquely-named **link target** that can be referenced elsewhere in the documentation.

    .. _{LINK NAME}:

Note that `{LINK NAME}`:

- **must** be preceded by a single underscore,
- **must** be followed by a single colon, and
- **must** be followed by at least one blank line for the doc-generation logic to process it correctly.

Replace `{LINK NAME}` with a link name that is unique among all documents under the `./docs/src/` directory.  It can have multiple words if needed to make it unique or when otherwise appropriate for clarity.  If multiple words are used, they can be separated with single spaces, hyphens or underscores.  Whatever you use, the `{LINK NAME}` string used to reference it must be identical.  `{LINK NAME}` strings are not case sensitive.

That unique name is then used to provide a link reference elsewhere in the documentation using one of two formats.

##### When the "link text" should be a title or section heading from the target document:

```reStructuredText
:ref:`{LINK NAME}`
```

This in-line markup (interpreted text using the Sphinx-defined custom `:ref:` role) is then replaced with a hyperlink whose "link text" is the name of the section heading or document title just below the **link target**.  For this reason, when using this syntax, `{LINK NAME}` must reference **link target**s that are just above a title or section heading.

##### When "link text" should be something else:

```reStructuredText
:ref:`other link text <{LINK NAME}>`
```

This latter syntax enables you to put a **link target** anywhere in an .RST file (not just above a heading) and link to it using this syntax.

Note:  This latter syntax was either added or fixed in Sphinx recently.  It did not work in Sphinx 7.3.7.




### Section Headings

[Section headings](https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html#sections) are created by underlining the section title with a punctuation character, at least as long as the text.  Example:

```
This Is a Heading
*****************
```

reStructuredText does not impose any particular heading levels assigned to certain characters since the structure is determined from the succession of headings.  However, the LVGL docs policy is to use the below heading convention:

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

Being consistent about this helps the Sphinx/docutils parser to format the tables of contents correctly.

Note that the "underlining" can be longer than the heading title, but if it is shorter, the documentation-generation logic will fail with an error.

For improved readability in the .RST file:

- place 3 blank lines above the 2nd and subsequent chapter titles (see above), and
- place 2 blank lines above section headings below chapters.



### Italics, Boldface and Underlining

Emphasis using italics is done by surrounding a word or phrase with single asterisks (`*`).

Emphasis using boldface is done by surrounding a word or phrase with double asterisks (`**`).

Normally underlining is not possible in **reStructuredText**.  Also, at this writing, the core **reStructuredText** parser (docutils) is unable to combine boldface, italics, and/or underlining.  However, LVGL documentation provides a work-around for these shortcomings using Text Roles.  All you have to remember is that the Text Role names combine the letters `i`, `b` and `u` to provide the desired combination.  Note that all possible permutations of these letters are supported so you do not have to remember what sequence works.  Examples:

| To Combine             | Text Role Name                           | Example       |
| ---------------------- | ---------------------------------------- | ------------- |
| underline              | `u` or `ul`                              | :ul:\`text\`  |
| underline and boldface | `ub` or `bu`                             | :ub:\`text\`  |
| underline and italics  | `iu` or `ui`                             | :iu:\`text\`  |
| italics and boldface   | `ib` or `bi`                             | :ib:\`text\`  |
| all three              | `ubi`, `uib`, `bui`, iub`, biu` or `ibu` | :ubi:\`text\` |



### Code Blocks

* Do not use tab characters in code blocks.
* Each indentation level use 4 spaces.
* Include at least 1 empty line after a code block.
* There must be one empty line between the code block directive and the code.
* `.. code-block::` is the only directive that should be used.  Note carefully that unlike the **link target** directive above, this directive has 2 colons.  (The only reST and Sphinx directives that are valid with one colon are **link target**s as shown above.)  Lone `::`, `:code:` or `.. code:` should not be used.
* If you want to separate code into easier-to-understand sections you can do so with a single empty line.
* For syntax highlighting appropriate to the language in the code block, specify the language after the directive.  Some examples are:

  - `.. code-block:: c`,
  - `.. code-block:: cpp`,
  - `.. code-block:: python`,
  - `.. code-block:: shell`,
  - `.. code-block:: bash`,
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

The full set of supported lexers are listed in the [Pygments Library documentation](https://pygments.org/docs/lexers/).



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



### Tightening Tables

Very long or very wide tables can be difficult to read and use.  You, as the writer, are responsible for making your tables as readable as they can be.  Here's how to "squeeze" them down to a smaller size when needed to make them more readable and usable:

1.  move your existing table under a `.. container:: tighter-table-N` directive (`N` = digit 1-7), and

2.  indent your table to be the container's "content", like this:

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



### Special Symbols

Because not all editors support special Unicode symbols well, it is encouraged to use **reStructuredText** substitutions to represent special symbols in `.rst` you create or modify.  A list of the most common of these can be found in `./docs/src/include/substitutions.txt`.  To use one of these substitutions, simply include this line at the top of the `.rst` file if it is not already there:

```rst
.. include:: /include/substitutions.txt
```

Then, any of those substitutions can be used in that `.rst` file.

(Technically this line can be anywhere in the `.rst` file, but we normally include it at the top so it is visible to anyone subsequently viewing or editing the file.)

Example:

```rst
The temperature outside is 20\ |deg|\ C.
```

results in

The temperature outside is 20°C.

Note that the spaces surrounding **reStructuredText** substitutions *are required* for the parser to parse them correctly.  Any time you need to remove those spaces in the final output, do so by escaping them with the `\` character.  Exception:  the `substitutions.txt` file contains 3 substitution definitions which are marked with the `:trim:` modifier. These do not need this escaping:

- `|nbsp|` (non-breaking space),
- `|shy|` (soft hyphen), and
- `|nbhyph|` (non-breaking hyphen used in titles and official names)



### Referencing API Documentation

#### Providing Links to API Pages

Let us say you are creating (or enhancing) documentation related to the `lv_scale_t` data type (one of the LVGL Widgets):  if you want the doc-build logic to generate appropriate links to LVGL API pages, place an API section at the end of your document (it must be at the end) like this:

```rst
API
***
```

and then, if you want the API-link-generation logic to generate hyperlinks to API pages based on an ***exact, case-sensitive string match*** with specific C symbols, follow it with a reStructuredText comment using this syntax:

```rst
.. API equals: lv_scale_t, lv_scale_create
```

What follows the colon is a comma- or space-separated list of exact C symbols documented somewhere in the `lvgl/src/` directory.  If the list is long, it can be wrapped to subsequent lines, though continuation lines must be all indented at the same level.  The list ends with the first blank line after this pseudo-directive.  This list is used to locate the `.h` files containing documentation for those symbols, and provide exactly one link to each applicable `.h` file API page.

If you instead want the API-link-generation logic to simply include links to code that ***starts with a specific string*** use this syntax instead.  The format of the list is the same as for `.. API equals:`:

```rst
.. API startswith: lv_scale, lv_obj_set_style
```

#### In-Line Code Expressions

If you want to reference documentation of of specific LVGL code elements, there are special Text Roles to do this:

    :cpp:func:`lv_init`   (note there are no parentheses after the function name)
    :c:macro:`LV_USE_FLEX`
    :cpp:type:`lv_event_t`
    :cpp:enum:`lv_state_t`
    :cpp:enumerator:`LV_STATE_CHECKED`
    :cpp:struct:`lv_image_dsc_t`
    :cpp:union:`lv_style_value_t`

#### More Complex Expressions

There is a special text role to reference parts of more complex expressions.  For example when showing the arguments passed to a function.

    :cpp:expr:`lv_obj_set_layout(widget, LV_LAYOUT_FLEX)`
    :cpp:expr:`lv_slider_set_mode(slider, LV_SLIDER_MODE_...)`

Arguments that contain more than one word or non-alphanumeric characters will cause the `:cpp:expr:` interpreted-text to fail.  Examples:

| Expression                                                   | Cause of Failure       |
| ------------------------------------------------------------ | ---------------------- |
| :cpp:expr:\`lv_obj_set_layout(widget, LV_LAYOUT_FLEX/GRID)\` | argument with > 1 word |
| :cpp:expr:\`lv_obj_set_layout(widget, LV_LAYOUT_*)\`         | asterisk               |
| :cpp:expr:\`lv_obj_set_layout(*widget, LV_LAYOUT_FLEX)\`     | asterisk               |
| :cpp:expr:\`lv_obj_set_layout((lv_obj_t *)widget, LV_LAYOUT_FLEX)\` | cast                   |
| :cpp:expr:\`lv_obj_set_layout(&widget, LV_LAYOUT_FLEX);\`    | ampersand & semicolon  |
| :cpp:expr:\`lv_obj_set_layout(widget, ...)\`                 | lone ellipsis          |

For such examples, simply use reStructuredText literal markup like this:

    ``lv_obj_set_layout(widget, LV_LAYOUT_FLEX/GRID)``
    ``lv_obj_set_layout(widget, LV_LAYOUT_*)``
    ``lv_obj_set_layout(*widget, LV_LAYOUT_FLEX)``
    ``lv_obj_set_layout((lv_obj_t *)widget, LV_LAYOUT_FLEX)``
    ``lv_obj_set_layout(&widget, LV_LAYOUT_FLEX);``
    ``lv_obj_set_layout(widget, ...)``

