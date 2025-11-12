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

If the list of document source (including the `.h` files in the `./src/` directory) files has changed (names or paths):

    python build.py clean html

Will remove the old intermediate and build files and regenerate new ones matching the new structure, eliminating the orphan files that would otherwise result.

To see a list of options available:

    python build.py

Read the header comment in `build.py` for detailed documentation on each option.




## For Developers

One of our firm policies is ***EVERYTHING MUST BE DOCUMENTED***.

The below are some rules to follow when updating any of the `.rst` files located in the `./docs/src/` directory tree.



### reStructuredText

LVGL documentation uses **reStructuredText** (reST), rendered into HTML by Sphinx.  Combining the [reStructuredText reference](https://docutils.sourceforge.io/docs/ref/rst/restructuredtext.html) and the [Sphinx Documentation](https://www.sphinx-doc.org/en/master/usage/index.html), you should be able to find any areStructuredText tools you will need while creating or modifying LVGL documentation.

If you prefer to learn by examples, the [Furo-theme examples](https://sphinx-themes.org/sample-sites/furo/) are an excellent resource, especially the ["Kitchen Sink" page](https://sphinx-themes.org/sample-sites/furo/kitchen-sink/) page.  Find an example of what you want to create (e.g. tables), then click the "eye" icon at the top of that page, and it will take you to the `.rst` source file that was used to generate that HTML page.

Note:  the section headings in these pages use a different convention than the one presented below.  For LVGL documentation, use the [section-heading convention presented below](https://github.com/lvgl/lvgl/tree/master/docs#section-headings).




### Text Format

Please wrap the text around column 86 or narrower.  Wrapping at *exactly* column 86 is not important, but readability and ease of editing is.




### index.rst Files

If you create a new directory you will need an `index.rst` file in that directory and that index file needs to be pointed to in the `index.rst` file that is located in the parent directory.

Let's take a look at the `index.rst` file that is located in the `docs/src/details/common-widget-features/layouts` directory.

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

For improved readability in the `.rst` file:

- place 3 blank lines above the 2nd and subsequent chapter titles (see above), and
- place 2 blank lines above section headings below chapters.



### Italics, Boldface and Underlining

Emphasis using italics is done by surrounding a word or phrase with single asterisks (`*`).

Emphasis using boldface is done by surrounding a word or phrase with double asterisks (`**`).

Normally underlining is not possible in reStructuredText.  Also, at this writing, the core reStructuredText parser (docutils) is unable to combine boldface, italics, and/or underlining.  However, LVGL documentation provides a work-around for this using Text Roles.  All you have to remember is that the Text Role names combine the letters `i`, `b` and `u` to provide the desired combination.  All possible permutations of these letters are supported so you do not have to remember what sequence works.  Examples:

| To Combine             | Text Role Name                             | Example       |
| ---------------------- | ------------------------------------------ | ------------- |
| underline              | `u` or `ul`                                | :ul:\`text\`  |
| underline and boldface | `ub` or `bu`                               | :ub:\`text\`  |
| underline and italics  | `iu` or `ui`                               | :iu:\`text\`  |
| italics and boldface   | `ib` or `bi`                               | :ib:\`text\`  |
| all three              | `ubi`, `uib`, `bui`, `iub`, `biu` or `ibu` | :ubi:\`text\` |



### Code Blocks

* Do not use tab characters in code blocks.
* Each indentation level use 4 spaces.
* Include at least 1 empty line after a code block.
* There must be one empty line between the code block directive and the code.
* `.. code-block::` is the only directive that should be used.  Note that unlike the **link target** above, reST directives (like this one) have 2 colons.  (The only reST and Sphinx directives that are valid with one colon are **link target**s as shown above.)  Lone `::`, `:code:` or `.. code::` should not be used.
* If you want to separate code into easier-to-understand sections you can do so with a single empty line.
* For syntax highlighting appropriate to the language in the code block, specify the language after the directive.  Examples:

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

The full set of supported code lexers are listed in the [Pygments Library documentation](https://pygments.org/docs/lexers/).



### Bulleted Lists

To create a bulleted list, do the following:

```rst
- First item description
- If you want to span multiple lines, indent subsequent
  lines to align with item text like this.
- If you want to include a code block under a list item,
  it must be intended to align with the list item like this:

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

All lists (including nested lists) **must** be preceded and followed with at least 1 blank line.  This is mandatory for the documentation-generation logic to process it correctly.



### External Links

URLs are converted to links automatically. E.g. `Visit https://lvgl.io`.

To add links with custom text use

```rst
Visit `My Website <https://pro.lvgl.io>`__.
```

If an external link will be used on many pages:

- Put it in `./docs/src/include/external_links.txt` if it is not already there.  Example:

  ```rst
  .. _LVGL Pro:  https://pro.lvgl.io
  ```

- `.. include:  /include/external_links.txt` once at the top of each `.rst` file that uses it.

- Use it by name in text:

  ```rst
  For further details see `LVGL Pro`_.
  ```

Back-quotes are not needed if there are no spaces in the name.



### Internal Links

##### Linking to a Section on the Same Page

Given:

```rst
My Heading
**********
```

link to section headings on the same page like this:

```rst
See `My Heading`_ for details.
```

Backquotes are not needed if there are no spaces in the title.


##### Linking to a Section on a Different Page

Add an anchor before headings that will be linked to:

```rst
.. _unique_anchor_name:

My Heading
**********
```

`unique_anchor_name` must be unique throughout the entire set of documentation files.

Link to the anchor by:

```rst
Click :ref:`here <unique_anchor_name>` for more details.
```

Or use the heading's text as the link's text:

```rst
Learn more at :ref:`unique_anchor_name`.
```

Results in "Learn more at <span style="color:blue;"><u>My Heading</u></span>.".

`unique_anchor_name` may appear in places other than before a heading, but if so, custom link text (like "here" the first example above) must be provided.



### Tightening Tables

Very long or very wide tables can be difficult to read and use.  Squeeze them down to make them more readable and usable.  Here is how:

1.  move your existing table under a `.. container:: tighter-table-N` directive (`N` = digit 1-7 with 7 being the tightest), and

2.  indent your table to be the container directive's content, like this:

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

Because not all editors support special Unicode symbols well, it is encouraged to use reStructuredText substitutions to represent special symbols in `.rst` you create or modify.  A list of the most common of these can be found in `./docs/src/include/substitutions.txt`.  To use one of these substitutions, add this line at the top of the `.rst` file if it is not already there.  Technically this line can be anywhere in the `.rst` file, but we include it at the top so it is visible to anyone subsequently viewing or editing the file.

```rst
.. include:: /include/substitutions.txt
```

Then, any of those substitutions can be used in that `.rst` file.  Example:

```rst
The temperature outside is 20\ |deg|\ C.
```

Result:  "The temperature outside is 20°C."


The spaces surrounding substitutions *are required*, but when you need to remove them in the output (as in the example above), do so by escaping them with the `\` character.  Exception:  the `substitutions.txt` file contains 3 substitution definitions which are marked with the `:trim:` option since their use *always* removes these spaces in the output.  These do not need this escaping:

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

and then, if you want the API-link-generation logic to generate hyperlinks to API pages based on an ***exact, case-sensitive string match*** with specific C symbols, follow it with this pseudo-directive (it's really a reST comment) using this syntax:

```rst
.. API equals: lv_scale_t, lv_scale_create
                             <=== blank line here ends the list
```

What follows the colon is a comma- or space-separated list of exact C symbols documented somewhere in the `lvgl/src/` directory.  If the list is long, it can be wrapped to subsequent lines, though continuation lines must be all indented at the same level.  This list is used to locate the `.h` files containing documentation for those symbols, and provide exactly one link to each applicable `.h` file API page.

If you instead want the API-link-generation logic to simply include links to code that ***starts with a specific string*** use this syntax instead.  The format of the list is the same as for `.. API equals:`

```rst
.. API startswith: lv_scale, lv_obj_set_style
                             <=== blank line here ends the list
```

#### In-Line Code Expressions

If you want to reference documentation of specific LVGL code elements, there are special Text Roles to do this:

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

