#!/usr/bin/env python3
""" examples_list.py -- Build `examples.rst`

`examples.rst` is built by gathering all examples recursively under
lvgl/examples/.  The recognized directives in that file are then
processed by Sphinx using `lvgl/docs/src/_ext/lv_example.py`.

Subsection names within `examples.rst` are made up of the capitalized
directory names ('_' and '-' characters are converted to spaces)
or are spelled out in `index.rst` files if the subsection names should
be spelled differently.  See "index.rst Requirements" section below
for details.

An "example" is defined as:

  - being anywhere under the `lvgl/examples/` directory
  - the presence of an `index.rst` in a directory with
  - example C code in file names typically starting with "lv_example_".

That C code is pulled into each example in the EXAMPLES HTML page, as well
as every doc where such examples are included, by a directive like this:

.. code::

    .. include:: /examples/layouts/flex/index.rst

Note that in the intermediate dir (from which LVGL user docs are generated),
``./examples/`` is a top-level subdirectory, so no relative "../../.." etc.
is required in the path.  Using a path starting at "root" tells Sphinx to
start at the top-level directory where the docs are being generated,
typically ``lvgl/docs/intermediate/``.

See `build.py` for more information about the intermediate directory.

If a subdirectory is included in the `avoid_dirs` list herein, it will
not generate any output in the resulting `examples.rst` file.
Example:  `lvgl/examples/assets/`.


Usage
*****

.. code::

    import example_list
    example_list.exec(intermediate_dir)

or run it directly:

.. code:: bash

    python3  example_list.py  "/home/lvgl/docs/intermediate"

In both cases, the `intermediate_dir` passed is expected to have
an `./examples/` directory in it (copy of `lvgl/examples/`) as a
top-level subdirectory.


Examples Directory Requirements
*******************************

.. code:: text

    lvgl/examples/
        index.rst        (directory-order directive since sub-dirs
                         are not presented in alphabetical order)
        anim/
            index.rst    (see below for expected contents)
            lv_example_anim_1.c
            lv_example_anim_2.c
            lv_example_anim_3.c
            lv_example_anim_4.c
            lv_example_anim_timeline_1.c
        ...
        layouts/
            flex/
                index.rst    (see below for expected contents)
                lv_example_flex_1.c
                lv_example_flex_2.c
                lv_example_flex_3.c
                etc.
            grid/
                etc.
        libs/
            index.rst        (section-heading name: "3rd-Party Libraries"
                             [since it is different than parent directory name])
            barcode/
                index.rst    (see below for expected contents)
                lv_example_barcode_1.c
                lv_example_barcode_1.h
            bmp/
                etc.
            etc.
        etc.

.. note::  The above shows only 2 levels of directories but deeper
           levels are supported (up to 5).


index.rst Requirements
**********************

.. code::

    Example 1 Title  <-- required for each example
    ---------------  <-- required for each example
                                        <-- blank lines are ignored
    .. lv_example:: lv_example_anim_1   <-- relative path to stem of C filename
        :language: c

Repeat the above pattern for each example in current directory.  That number may be
zero (0) for directories like `libs/` in which all examples are in directories below
that level.  See directory structure above.

Provide relative paths to each example outside of the current directory, e.g. some
examples use 2 Widgets, so the example would be local to one `index.rst`, and provide
a relative path to the other.  Example from `lvgl/examples/widgets/scale/index.rst`:

.. code::

    ...

    Axis ticks and labels with scrolling on a chart
    -----------------------------------------------
    .. lv_example:: ../chart/lv_example_chart_2
      :language: c

.. note::

    Starting the example code filename with `lv_example_` is not a requirement of the
    this script, but does make it clear that it is an example, so this pattern should
    be preserved for new and changed examples.


Custom Section Headings
-----------------------
If a section heading needs to be spelled differently than the capitalized name of the
parent directory, then an `index.rst` file in that directory may contain the desired
section-heading name underscored with asterisks (*).  Example from
`lvgl/examples/libs/index.rst`:

.. code::

    3rd-Party Libraries
    *******************


Optional Directory Reordering
-----------------------------
There are cases where it is not appropriate to present the contents of a directory in
alphabetical order.  When this is the case, a directive in the `index.rst` file in
the parent directory can be specified to govern the sequence its subdirectories are
processed.  The example below is from `lvgl/examples/widgets/index.rst`.  It is
provided in order to cause the "Base Widget" (obj) directory to be processed first.

.. code::

    .. dir_order::

        obj
        animimg
        arc
        arclabel
        bar
        button
        buttonmatrix
        etc.

.. note::

    A warning is issued if either:

    - a subdirectory is named that does not exist, or
    - a subdirectory exists that is not in the list and not in the `avoid_dirs` list.


Making Warnings into Errors
***************************

If `MAKE_WARNINGS_INTO_ERRORS` is True, if there were any warnings issued,
after the output is completely generated, this script will exit with a
non-zero exit code.  This can be done by client modules like this:

.. code::

    import example_list
    example_list.make_warnings_into_errors()
    example_list.exec(intermediate_dir)


Debug Output
************

Enable debug output like this:

.. code::

    import example_list
    example_list.USE_DEBUG_MODE = True
    example_list.exec(intermediate_dir)

Suppress


"""
import os
import sys
from io import TextIOWrapper
from announce import *

# -------------------------------------------------------------------------
# This is the order that LVGL documentation uses for the section heading
# levels.  header_defs[0] is the highest and header_defs[5] is the lowest.
# If this order is not kept in the reST files Sphinx will complain, and
# have difficulty formatting the TOC correctly.
# -------------------------------------------------------------------------
TITLE = '='
CHAPTER = '*'
SECTION = '-'
SUBSECTION = '~'
SUBSUBSECTION = '^'
SUBSUBSUBSECTION = "'"

header_defs = [
    TITLE,
    CHAPTER,
    SECTION,
    SUBSECTION,
    SUBSUBSECTION,
    SUBSUBSUBSECTION,
]

LV_EXAMPLE_DIRECTIVE = '.. lv_example::'
DIR_ORDER_DIRECTIVE = '.. dir_order::'
DIR_SEP = os.sep
INDEX_FILENAME = 'index.rst'
MAKE_WARNINGS_INTO_ERRORS = False
USE_DEBUG_MODE = False

avoid_dirs = [
    os.path.join('examples', 'arduino'),
    os.path.join('examples', 'assets'),
]


def make_warnings_into_errors():
    global MAKE_WARNINGS_INTO_ERRORS
    MAKE_WARNINGS_INTO_ERRORS = True


def default_section_heading(level: int, path: str, is_file: bool) -> str:
    if is_file:
        dir_path = os.path.dirname(path)
    else:
        dir_path = path

    parent_dir = os.path.basename(dir_path)

    # Compose default section heading based on capitalized words in `parent_dir`.
    word_list = parent_dir.replace('_', ' ').replace('-', ' ').split(' ')
    result = ''

    for word in word_list:
        result += ' ' + word.capitalize()

    # Remove leading space.
    if result:
        result = result[1:]

    return result


def generate_output_from_dir(level: int,
                             root_len: int,
                             file_or_dir: str,
                             is_file: bool,
                             orig_dir_list: list[str],
                             f: TextIOWrapper) -> list[str]:
    """ Output to `f` based on contents of `file_or_dir`.

    :param level:          Directory depth below the top directory in tree.
    :param root_len:       Length of root path -- is deleted from paths so that
                             paths are relative to the `lvgl/examples/` dir.
    :param file_or_dir:    Path to "thing" being processed.
    :param is_file:        True if `file_or_dir` is an `index.rst` file.
    :param orig_dir_list:  List of sub-dirs below dir being processed.
                             Will be returned from this function if a
                             dir-order directive is NOT found in index file.
    :param f:              Output file (examples.rst)
    :return:               `orig_dir_list` or `dir_order_override` if a
                             dir-order directive is found in index file.
    """
    section_heading = default_section_heading(level, file_or_dir, is_file)
    example_tuples = []
    dir_order_override = []

    if is_file:
        announce(__file__, f'Processing file [{file_or_dir}]...')
        # We are processing an index.rst file.
        with open(file_or_dir, 'r', encoding='utf-8') as fidx:
            blob = fidx.read()

        lines = blob.split('\n')
        example_title = ''
        prev_line = ''
        dir_path = os.path.dirname(file_or_dir)
        rel_dir = dir_path[root_len:]
        in_dir_order_directive = False
        # Final path to example in example directive requires forward slashes.
        if DIR_SEP != '/' and DIR_SEP in rel_dir:
            rel_dir = rel_dir.replace(DIR_SEP, '/')

        # Accumulate data from `index.rst`.  This needs to be done
        # first in case it overrides the default section heading.
        for line in lines:
            if in_dir_order_directive:
                leading_non_blank = (len(line) > 0) and not (line[0] == ' ' or line[0] == '\t')

                if leading_non_blank:
                    # Leading non-blank character ends dir-order directive.
                    in_dir_order_directive = False
                else:
                    # Still in dir-order directive.
                    stripped_line = line.strip()
                    if stripped_line:
                        dir_order_override.append(os.path.join(dir_path, stripped_line))
                    continue

            stripped_line = line.strip()

            if not stripped_line:
                continue         # Skip blank line.
            elif stripped_line.startswith('***'):
                announce(__file__, f'Default section heading [{section_heading}] replaced with [{prev_line}]...')
                section_heading = prev_line
            elif stripped_line.startswith('---'):
                example_title = prev_line
                announce(__file__, f'Example [{example_title}]...')
            elif stripped_line.startswith(LV_EXAMPLE_DIRECTIVE):
                rel_path_from_index_rst = stripped_line.replace(LV_EXAMPLE_DIRECTIVE, '').strip()
                announce(__file__, f'        [{rel_path_from_index_rst}]')
                example_tuples.append( (example_title, rel_path_from_index_rst) )
            elif not in_dir_order_directive and stripped_line.startswith(DIR_ORDER_DIRECTIVE):
                in_dir_order_directive = True
                announce(__file__, f'Found DIR-ORDER directive...')

            prev_line = stripped_line

    # Output section heading.
    announce(__file__, f'Section heading [{section_heading}] at level [{level}].')
    section_heading_underline = header_defs[level] * len(section_heading)

    if level == 1:
        f.write('\n\n')      # 2 extra blank lines above chapters
    elif level == 2:
        f.write('\n')        # 1 extra blank line above sections
    elif level == 0:
        f.write('\n\n\n')    # 3 extra blank lines above (doc) root titles
        f.write(section_heading_underline)
        f.write('\n')

    f.write(section_heading)
    f.write('\n')
    f.write(section_heading_underline)
    f.write('\n\n')

    # Output examples, if any.
    for example_tuple in example_tuples:
        example_title = example_tuple[0]
        rel_path_from_index_rst = example_tuple[1]
        example_hdg_underline = header_defs[level + 1] * len(example_title)
        announce(__file__, f'Writing example [{rel_path_from_index_rst}]...')

        if level + 1 == 1:
            f.write('\n\n')  # 2 extra blank lines above chapters
        elif level + 1 == 2:
            f.write('\n')    # 1 extra blank line above sections

        f.write(example_title)
        f.write('\n')
        f.write(example_hdg_underline)
        f.write('\n\n')
        f.write(LV_EXAMPLE_DIRECTIVE + ' ' + rel_dir + '/' + rel_path_from_index_rst)
        f.write('\n\n')

    if dir_order_override:
        return dir_order_override
    else:
        return orig_dir_list


def need_to_avoid_dir(dir_bep: str) -> bool:
    result = False

    for avoid_dir in avoid_dirs:
        if avoid_dir in dir_bep:
            result = True
            break

    return result


def process_dir_recursively(level: int, root_len: int, dir_bep: str, f: TextIOWrapper):
    """ Process dir `dir_bep` recursively, avoiding sub-dirs in `avoid_dirs`.

    :param level:     Directory depth below the top directory in tree.
    :param root_len:  Length of root path (gets removed from full path)
    :param dir_bep:   Source directory *being processed*
    :param f:         Output file (examples.rst)
    """
    announce(__file__, f'Processing dir [{dir_bep}]...')
    sub_dirs = []
    idx_files = []

    # For each "thing" found in `dir_bep`, build lists:  sub_dirs and idx_files.
    for dir_item in os.listdir(dir_bep):
        path_bep = os.path.join(dir_bep, dir_item)
        if os.path.isdir(path_bep):
            sub_dirs.append(path_bep)
        elif path_bep.endswith(INDEX_FILENAME):
            idx_files.append(path_bep)

    if idx_files:
        sub_dirs = generate_output_from_dir(level, root_len, idx_files[0], True, sub_dirs, f)
    else:
        generate_output_from_dir(level, root_len, dir_bep, False, sub_dirs, f)

    # `sub_dirs` can be replaced if `index.rst` contains a dir-order directive.
    # Now recursively process sub_dirs.
    for subdir in sub_dirs:
        if not need_to_avoid_dir(subdir):
            process_dir_recursively(level + 1, root_len, subdir, f)


def exec(intermediate_dir):
    announce_set_silent_mode(not USE_DEBUG_MODE)
    output_path = os.path.join(intermediate_dir, 'examples.rst')
    input_paths = [
        os.path.join('..', 'examples'),
        # os.path.join('..', 'demos')
    ]

    with open(output_path, 'w', encoding='utf-8') as f:
        f.write('.. _examples:\n')
        f.write('\n')

        # Recursively walk the directories in `input_paths` array for
        # ``index.rst`` files.
        for root_path in input_paths:
            root_len = len(root_path) + 1
            process_dir_recursively(0, root_len, root_path, f)


if __name__ == '__main__':
    """Make module run-able as well as importable."""
    base_dir = os.path.abspath(os.path.dirname(__file__))
    os.chdir(base_dir)
    exec(sys.argv[1])
