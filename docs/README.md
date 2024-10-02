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
* sphinxcontrib-mermaid
* sphinx-design
* sphinx-rtd-dark-mode
* typing-extensions

To install using the `requirements.txt` file use the following command.

    pip install -r requirements.txt

Once you have all of the requirements installed you are ready to build them. To build the documentation use the following command.

    python build.py skip_latex clean

You may have to use the following command if you are on a Unix like OS

    python3 build.py skip_latex clean

The documentation will be output into the folder `out_html` in the root directory for LVGL.


## For Developers

The most important thing that has to be done when contributing to LVGL is ***EVERYTHING MUST BE DOCUMENTED***.

The below are some rules to follow when updating any of the `.rst` files located in the docs folder and any of it's subfolders.


### index.rst files

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

The first line is for the purposes of providing a uniquely-named link target that can be referenced elsewhere in the documentation.

    .. _{LINK NAME}:

Replace `{LINK NAME}` with a link name that is unique among all documents under the `./docs/` directory.  It can have multiple words if needed or otherwise appropriate. While separating multiple words with spaces technically can work, we prefer separating these words with hyphens.  Example:  `ref-base-object`, to refer to the "Base Object" page in the Reference section of the documentation.

That unique name is then used to provide a link reference elsewhere in the documentation like this:

    :ref:`{LINK NAME}`

The `.. _{LINK NAME}:` line should be above a heading and there **must** be a single empty line after it for the documentation-generation logic to process it correctly.


### Section Headings

[Section headers](https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html#sections) are created by underlining (and optionally overlining) the section title with a punctuation character, at least as long as the text.  Example:

```
=================
This Is a Heading
=================
```

reStructuredText does not impose any particular heading levels assigned to certain characters since the structure is determined from the succession of headings.  So if you are modifying an existing .RST file, please follow the pattern already in use.

If you are creating a new .RST file, this convention is used:

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

Note that the "underlining" can be longer than the heading title, but if it is shorter, the documentation-generation logic will fail with an error.

For improved readability in the .RST file, place at least 2 blank lines above headings.


### Code Blocks

* Do not use tab characters in code blocks.
* Indentations use 4 spaces.
* Include at least 1 empty line after a code block.
* There must be one empty line between the code block directive and the code.
* `.. code-block::` is the only directive that should be used.  Note carefully that unlike the link target directive above, this directive has 2 colons.  (The only ReST and sphinx directives that are valid with one `:` are link targets as shown above.)  Lone `::`, `:code:` or `.. code:` should not be used.
* For syntax coloring appropriate to the language, specify the language after the directive.  Some examples are:
  - `.. code-block:: python`,
  - `.. code-block:: c`,
  - `.. code-block:: shell`,
  - `.. code-block:: make`.
* If you want to separate code into easier-to-understand sections you can do so with a single empty line.  No more than ONE line.



### Bulleted Lists

To create a bulleted list, do the following:

    - item1 description
    - If you want to span multiple
      lines it must be done like this
    - item3:  If you want to use a code block it must be
      intended with the list item like this:

      .. code-block: python

          # this is some code

    - item4:  If you want to have several layers of bullets, indent
      each new layer with the parent list item like this:

      - level 2 item 1: text
      - level 2 item 2: text

End all lists (including nested lists) with at least 1 empty line.


### Referencing Portions of the API

If you want to reference portions of the LVGL code from the documentation (in .RST files) there are special directives to do this:

    :cpp:func:`lv_init`
    :c:macro:`LV_USE_FLEX`
    :cpp:type:`lv_event_t`
    :cpp:enum:`_lv_event_t`
    :cpp:enumerator:`LV_EVENT_ALL`
    :cpp:struct:`lv_image_dsc_t`
    :cpp:union:`lv_style_value_t`

There is a special directive when wanting to use a more complex expression.  For example when showing the arguments passed to a function.

    :cpp:expr:`lv_obj_set_layout(obj, LV_LAYOUT_FLEX)`

Arguments that are expressions (more than one word), or contain non-alphanumeric characters
will cause the `:cpp:expr:` interpreted-text role to fail.  Examples:

    :cpp:expr:`lv_obj_set_layout(obj, LV_LAYOUT_FLEX/GRID)`         <== arg with > 1 word
    :cpp:expr:`lv_obj_set_layout(obj, LV_LAYOUT_*)`                 <== asterisk
    :cpp:expr:`lv_obj_set_layout(*obj, LV_LAYOUT_FLEX)`             <== asterisk
    :cpp:expr:`lv_obj_set_layout((lv_obj_t *)obj, LV_LAYOUT_FLEX)`  <== cast
    :cpp:expr:`lv_obj_set_layout(&obj, LV_LAYOUT_FLEX);`            <== ampersand
    :cpp:expr:`lv_obj_set_layout(obj, ...)`                         <== elipsis

For such examples, simply use reStructuredText literal markup like this:

    ``lv_obj_set_layout(obj, LV_LAYOUT_FLEX/GRID)``
    ``lv_obj_set_layout(obj, LV_LAYOUT_*)``
    ``lv_obj_set_layout(*obj, LV_LAYOUT_FLEX)``
    ``lv_obj_set_layout((lv_obj_t *)obj, LV_LAYOUT_FLEX)``
    ``lv_obj_set_layout(&obj, LV_LAYOUT_FLEX);``
    ``lv_obj_set_layout(obj, ...)``
