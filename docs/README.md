# Documentation

----------------------------------------------

## Building

Building the documentation is pretty easy to do but it does have some requirements
that have to be filled prior to building them.

Here are the requirements:

* Doxygen
* Python >= 3.10
* C compiler (gcc, msvc, clang, etc...)


There are also some Python specific libraries that need to be installed.
You can either install these individually or you can use pip to read the requirements
file to install everything that is needed for Python.

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


Once you have all of the requirements installed you are ready to build them.
To build the documentation use the following command.

    python build.py skip_latex clean

You may have to use the following command if you are on a Unix like OS

    python3 build.py skip_latex clean

The documentation will be output into the folder `out_html` in the root directory
for LVGL.

## For Developers

---------------------------------------

The most important thing that has to be done when contributing to LVGL is


***EVERYTHING MUST BE DOCUMENTED***

--------------------------------------

Some rules to follow when updating any of the `.rst` files located in the docs
folder and any of it's subfolders.

<br/>

### index.rst files

--------------------------

If you create a new directory you MUST have an `index.rst` file in that directory
and that index file needs to be pointed to in the `index.rst` file that is located
in the parent directory.

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


That is what you see... Below is what the various parts of the file are.

```
    .. _layouts:      <=== Creates a reference that is linkable

    =======
    Layouts           <=== Heading seen in documentation
    =======


    .. toctree::      <=== Table of contents
        :maxdepth: 2  <=== Internal use and need to always be set this way

        flex          <=== .rst files located in directory with index.rst
        grid
```

The first line is for the purposes of not having to statically link to other locations
in the documentation.  It makes it easier when things get moved around as the link will
change dynamically if that should occur.  In order to create the link it must be formatted
in this manner.

    .. _{LINK NAME}:

where you would replace `{LINK NAME}` with whatever name you wanted to provide.
That name is what is going to be used to reference the link. This is done by using

    :ref:`{LINK NAME}`

The `.. _{LINK NAME}:` line MUST be above a heading and there MUST be a single empty line
after it.  This is MANDATORY.




### Section Headings

------------------------------

[Section headers](https://www.sphinx-doc.org/en/master/usage/restructuredtext/basics.html#sections)
are created by underlining (and optionally overlining) the section title with
a punctuation character, at least as long as the text.  Example

=================
This Is a Heading
=================

reStructuredText does not impose any particular heading levels assigned to certain characters since the structure is determined from the succession of headings.  So if you are modifying an existing .RST file, please follow the pattern it is already using.

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
    
    Paragraph
    '''''''''

For improved readability in the .RST file, place at least 2 blank lines above headings.




### Code Blocks

--------------------------------------------------------------

* No tab characters are to be used in a code block.
* Indents are done using 4 spaces and only 4 spaces.
* Include 2 empty lines at the end of a code block.
* One empty line between the code block directive and the code.
* `.. code-block:` is the only directive that should be used. `::`, `:code:` or `.. code:` should not be used.
* Specify the language after the directive. Some examples are:
  - `.. code-block: python`,
  - `.. code-block: c`,
  - `.. code-block: shell`,
  - `.. code-block: make`.
* If you want to separate code into easier to understand sections you can do so with a single empty line.  No more than ONE line.



### Bulleted Lists

-------------------------------------------------------------

To create a bulleted list, do the following:


    - item1: description
    - item2: If you want to span multiple
      lines it must be done like this
    - item3: If you want to use a code block it must be done like this
    
      .. code-block: python
    
          # this is some code
    
    - item3: If you want to have several layers of bullets it needs to be done like this
    
      - level 2 item 1: text
      - level 2 item 2: text

End all lists with 2 empty lines except when it is a nested list.  Then you use a single empty line.  The same thing holds true for code blocks as well.  If it is nested into a list then a single empty line after.  If the nested list or code block is at the end of the first level then you need to use 2 empty lines.




### Referencing Portions of the API

------------------------

If you want to reference portions of the LVGL code from the documentation (in .RST files) there are special directives to do this:

    :cpp:func:`lv_init`
    :c:macro:`LV_USE_FLEX`
    :cpp:type:`lv_event_t`
    :cpp:enum:`_lv_event_t`
    :cpp:enumerator:`LV_EVENT_ALL`
    :cpp:struct:`lv_image_dsc_t`
    :cpp:union:`lv_style_value_t`

There is a special directive when wanting to use a more complex expression.
For example when showing the arguments passed to a function

    :cpp:expr:`lv_obj_set_layout(obj, LV_LAYOUT_FLEX)`

you CANNOT have expressions that are like this...

    :cpp:expr:`lv_obj_set_layout(obj, LV_LAYOUT_FLEX/GRID)`  <== arg with more than one word
    :cpp:expr:`lv_obj_set_layout(obj, LV_LAYOUT_*)`  <== asterisk
    :cpp:expr:`lv_obj_set_layout(*obj, LV_LAYOUT_FLEX)`  <== asterisk
    :cpp:expr:`lv_obj_set_layout((lv_obj_t *)obj, LV_LAYOUT_FLEX)`  <== cast/asterisk
    :cpp:expr:`lv_obj_set_layout(&obj, LV_LAYOUT_FLEX);`  <== ampersand
    :cpp:expr:`lv_obj_set_layout(obj, ...)`  <== elipsis

Those are all invalid.

