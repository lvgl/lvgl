# Documentation

----------------------------------------------

## Building

Building the documentation is pretty easy to do but it does have some requirements
That have to be filled prior to building them.

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


The areguments that can be used with the build script are as follows

* clean: this will remove the output path if it exists
* latex: this will build the LVGL documentation PDF file
* develop: This will stop the removal of the build folder if the default temp path is used
* ignore-missing-warnings: Stops the files from being generated to "MISSING-DOCSTRINGS"
* build-path={absolute path}: setting a custom temporary build path (defaults to the system temporary path)
* output-path={absolute path}: where to output the documentation to (defaults to `out_html`)


When using the `build-path` argument if the directory exists and `clean` is supplied the directory gets 
deleted and then remade. If the directory exists `develop` is assumed and the folder doesn't get removed.
If the directory doesn't exist and `develop` is supplied then the directory doesn't get removed at the 
end of the build.


Once you have all of the requirements installed you are ready to build them.
To build the documentation use the following command.

    python build.py clean

You may have to use the following command if you are un a Unix like OS

    python3 build.py clean

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

lets take a look at the `index.rst` file that is located in the `docs/layouts` directory.

    .. _layouts:

    =======
    Layouts
    =======


    .. toctree::
        :maxdepth: 2

        flex
        grid


That is what you see... Below is what the various parts of the file are.


    .. _layouts:   <==== This is to create a reference that is linkable

    =======
    Layouts        <====== This is the heading that is seen in the documentation
    =======


    .. toctree::      <=== Table of contents
        :maxdepth: 2  <=== Internal use and need to always be set this way  

        flex          <=== .rst files located in the directory the index.rst file is located
        grid


The first line is for the purposes of not having to statically link to other locations 
in the documentation. It makes it easier when thing get moved around as the link will 
change dynamically if that should occur. In order to create the link it must be formatted 
in this manner.

    .. _{LINK NAME}:

where you would replace `{LINK NAME}` with whatever name you wanted to provide.
That name is what is going to be used to reference the link. This is done by using 

    :ref:`{LINK NAME}`

The `.. _{LINK NAME}:` line MUST be above a headling and there MUST be a single empty line 
between the two. This is a MANDATORY thing no exceptions.


### headings

------------------------------


This is important because if it is not done properly you will end up with errors
You have to pay attention and look at index.rst files to determine the proper heading 
markers to use.

    =====
    Title
    =====

    *******
    Chapter
    *******

    Section
    ^^^^^^^

    Sub Section
    -----------

    Sub Sub Section
    ***************

    Paragraph
    ~~~~~~~~~


You MUST follow that order. Look at the heading in the `index.rst` file that includes
the file you are working om to know what the first heading should be set as. Then you 
work down the above list from there. The primers (markers) that instruct the build that 
a heading is to be used MUST be exactly the same length as the text above and below those 
primers. If it is not you will have an error. You must have 2 blank lines between a heading
and whatever documentation/text is above it except when using the directive to create a 
reference. The there need to be a single blank line above the heading and 2 blank lines above 
the directive.


### Code blocks

--------------------------------------------------------------

* No tabs are to be used in a code block
* Indents are done using 4 spaces and only 4 spaces
* 2 empty lines at the end of a clode block
* One empty line between the code block directive and the code
* `.. code-block:` is the only directive that should be used. `::`, `:code:` or `.. code:` should not be used
* specify the language after the directive. Sopme examples are: `.. code-block: python`, `.. code-block: c`, `.. code-block: shell`, `.. code-block: make`
* If you want to separate code into easier to understand sections you can do so with a single empty line. No more than ONE line.


### bulleted lists

-------------------------------------------------------------

to create a bulleted list you do the follow


    - item1: description
    - item2: If you want to span multiple
      lines it must be done like this
    - item3: If you want to use a code block it must be done like this
      
      .. code-block: python

          # this is some code

    - item3: If you want to have several layers of bullets it needs to be done like this

      - level 2 item 1: text
      - level 2 item 2: text


you must end all lists with 2 empty lines except when it is a nested list. Then you use a 
single empty line. The same thing holds true for code blocks as well. if it is nested into 
a list then a single empty line after. If the nested list or code block is at the end of the 
first level then you need to use 2 empty lines


### referencing portions of the API

------------------------

If you want to reference portions of the LVGL code from the documentation
there are special primers/directives to do this...

    :cpp:func:`lv_init`
    :c:macro:`LV_USE_FLEX`
    :cpp:type:`lv_event_t`
    :cpp:enum:`_lv_event_t`
    :cpp:enumerator:`LV_EVENT_ALL`
    :cpp:struct:`lv_image_dsc_t`
    :cpp:union:`lv_style_value_t`


There is a special directive when wanting to use a more complex exspression. 
For example when showing the arguments passed to a function

    :cpp:expr:`lv_obj_set_layout(obj, LV_LAYOUT_FLEX)`

you CANNOT have expressions that are like this...

    :cpp:expr:`lv_obj_set_layout(obj, LV_LAYOUT_FLEX/GRID)`
    :cpp:expr:`lv_obj_set_layout(obj, LV_LAYOUT_*)`
    :cpp:expr:`lv_obj_set_layout(*obj, LV_LAYOUT_FLEX)`
    :cpp:expr:`lv_obj_set_layout((lv_obj_t *)obj, LV_LAYOUT_FLEX)`
    :cpp:expr:`lv_obj_set_layout(&obj, LV_LAYOUT_FLEX);`
    :cpp:expr:`lv_obj_set_layout(obj, ...)`

Those eare all invalid.

I will be adding onto this readme, so keep an eye open for new information...