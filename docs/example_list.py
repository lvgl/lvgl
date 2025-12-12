#!/usr/bin/env python3
""" example_list.py -- Build `examples.rst`

`examples.rst` is built by gathering all examples recursively under
`lvgl/examples/`.  The recognized directives in that file are then
processed by Sphinx using `lvgl/docs/src/_ext/lv_example.py`.

Subsection names within `examples.rst` are made up of the capitalized
directory names ('_' and '-' characters are converted to spaces) or
are spelled out in `index.rst` files if the subsection names should be
spelled differently.  See "index.rst Format" section below for details.

An "example" is defined as:

    - being anywhere under the `lvgl/examples/` directory,
    - the presence of an `index.rst` in a directory with examples, and
    - example C code in file names typically starting with "lv_example_".

That C code and a live, interactive running example, is pulled into each
example in the EXAMPLES HTML page, as well as every doc where such
examples are included, by a directive like this:

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
    example_list.make_warnings_into_errors()  # Optional
    example_list.DEBUG_MODE = True            # Optional
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
        index.rst        (.. dir_order: pseudo-directive since sub-dirs
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
           levels are supported (up to 5 below document title).


index.rst Format
****************

Examples
--------
Examples are included in `index.rst` files when they are sibling files with
the example C code.

.. code::

    Example 1 Title  <-- required for each example
    ---------------  <-- required for each example
                                             <-- blank lines are ignored
    .. lv_example:: anim/lv_example_anim_1   <-- path relative to the `lvgl/examples/` dir
        :language: c

Repeat the above pattern for each example in the current directory.  That number
may be zero (0) for directories like `libs/` in which all examples are in directories
below that level.  See directory structure above.

For paths outside the current directory, simply provide the path to the code example
relative to the `lvgl/examples/` directory.  Example from
`lvgl/examples/widgets/scale/index.rst`:

.. code::

    ...

    A round scale style simulating a compass
    ----------------------------------------

    .. lv_example:: widgets/scale/lv_example_scale_12
      :language: c

    Axis ticks and labels with scrolling on a chart
    -----------------------------------------------

    .. lv_example:: widgets/chart/lv_example_chart_2     <-- path is outside scale/ dir
      :language: c

.. note::

    Starting the example code filename with `lv_example_` is not a requirement of the
    this script, but does make it clear that it is an example, so this pattern should
    be preserved for new and changed examples.


Custom Section Headings
-----------------------
If a section heading needs to be spelled differently than the capitalized name of the
parent directory, then an `index.rst` file in that directory may contain the desired
section-heading name in an ``.. example_heading`` pseudo-directive.  Example from
`lvgl/examples/libs/index.rst`:

.. code::

    .. example_heading: 3rd-Party Libraries


Directory Reordering
--------------------
There are cases where it is not appropriate to present the contents of a
set of subdirectories in alphabetical order.  When this is the case, a
pseudo-directive in the `index.rst` file in the parent directory can be specified
to govern the sequence its subdirectories are processed.  The example below
is from `lvgl/examples/widgets/index.rst`.  It is provided in order to
cause the "Base Widget" (obj) directory to be processed first (and thus
included in the output first).

.. code::

    .. dir_order:

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

If `make_warnings_into_errors()` was called, if there were any warnings
issued, after the output is completely generated, this script will exit
with a non-zero exit code.  This can be done by client modules like this:

.. code::

    import example_list
    example_list.make_warnings_into_errors()
    example_list.exec(intermediate_dir)

To turn it off:

.. code::

    example_list.make_warnings_into_errors(False)


Debug Output
************

Enable debug output like this:

.. code::

    example_list.DEBUG_MODE = True

Suppressing DEBUG_MODE is the default.


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
EXAMPLE_HEADING_DIRECTIVE = '.. example_heading:'
DIR_ORDER_DIRECTIVE = '.. dir_order:'
DIR_SEP = os.sep
INDEX_FILENAME = 'index.rst'
MAKE_WARNINGS_INTO_ERRORS = False
DEBUG_MODE = False
THIS_FILE = os.path.basename(__file__)
_warning_issued = False
_top_level_heading_count = 0

avoid_dirs = [
    os.path.join('examples', 'arduino'),
    os.path.join('examples', 'assets'),
]


def make_warnings_into_errors(val: bool = True):
    global MAKE_WARNINGS_INTO_ERRORS
    MAKE_WARNINGS_INTO_ERRORS = val


def _default_section_heading(level: int, path: str, is_file: bool) -> str:
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


def _warn(msg: str):
    global _warning_issued
    if MAKE_WARNINGS_INTO_ERRORS:
        warning_type = 'Error'
    else:
        warning_type = 'Warning'

    if is_silent_mode():
        print(f'{THIS_FILE}: ' + '\x1b[31m' + f'>>> {warning_type}:  ' + msg + '\x1b[0m')
    else:
        announce_colored(THIS_FILE, 'red', f'>>> {warning_type}:  ' + msg)

    _warning_issued = True


def _in_avoid_dirs_list(dir_bep: str) -> bool:
    result = False

    for avoid_dir in avoid_dirs:
        if avoid_dir in dir_bep:
            result = True
            break

    return result


def _validate_sub_dirs(sub_dirs: list[str], index_rst_path: str) -> bool:
    """ Validate sub-dirs that come from an `index.rst` `.. dir_order:` directive.

    1.  Check that that each one is an existing directory.
    2.  Check that there are none missing except those in `avoid_dirs`.

    Issue warning if either fails and set `_warning_issued`.
    If MAKE_WARNINGS_INTO_ERRORS, this will make script exit with an error
    code at the end.

    :param sub_dirs:        List of sub-dirs to validate
    :param index_rst_path:  Path to index.rst file
    :return:                Whether `sub_dirs` is valid per 1 and 2 above
    """
    result = True

    # Check that each sub-dir is an existing directory.
    for sub_dir in sub_dirs:
        if not os.path.isdir(sub_dir):
            result = False
            _warn(f'Dir-order directive in {index_rst_path} contains dir [{sub_dir}] that does not exist.')
            # We won't break here so that all such dirs can be listed.

    # Check that there are none missing except those in `avoid_dirs`.
    dir_path = os.path.dirname(index_rst_path)
    actual_dirs = []

    for dir_item in os.listdir(dir_path):
        path_bep = os.path.join(dir_path, dir_item)
        if os.path.isdir(path_bep):
            actual_dirs.append(path_bep)

    # Are there any missing that are not in `avoid_dirs` list?
    # If so, issue warning about each directory missing.
    for sub_dir in actual_dirs:
        if sub_dir not in sub_dirs:
            if not _in_avoid_dirs_list(sub_dir):
                _warn(f'Dir-order directive in {index_rst_path} is missing dir [{sub_dir}].')
                result = False

    return result


def _emit_heading(level: int, hdg: str, f: TextIOWrapper):
    """
    Emit reST heading using `header_defs`.

    :param level:  Directory depth below the top directory in tree.  [0-5]
    :param hdg:    Heading text
    :param f:      Output file (examples.rst)
    :return:
    """
    assert 0 <= level < len(header_defs), "level out of range"
    global _top_level_heading_count
    underline = header_defs[level] * len(hdg)
    announce(__file__, f'Section heading [{hdg}] at level [{level}].')

    if level == 0:
        _top_level_heading_count += 1
        if _top_level_heading_count > 1:
            f.write('\n\n\n')    # 3 extra lines above (doc) root titles, except 1st
        f.write(underline)
        f.write('\n')
    elif level == 1:
        f.write('\n\n')          # 2 extra lines above chapters
    elif level == 2:
        f.write('\n')            # 1 extra line above sections

    f.write(hdg)
    f.write('\n')
    f.write(underline)
    f.write('\n\n')


def _generate_output_from_dir(level: int,
                             root_len: int,
                             file_or_dir: str,
                             is_file: bool,
                             orig_dir_list: list[str],
                             f: TextIOWrapper) -> list[str]:
    """ Output to `f` based on contents of `file_or_dir`.

    :param level:          Directory depth below the top directory in tree.  [0-4]
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
    dir_order_override = []
    result_dir_list = orig_dir_list

    # It is an error to proceed with `level` out of range.  Clamping it to be in
    # range is also an error because it would cause the output to be corrupted with
    # an invalid section-heading underscore, which would generate an error later.
    if 0 <= level and level + 1 < len(header_defs):
        section_heading = _default_section_heading(level, file_or_dir, is_file)
        example_tuples = []
        relative_dir = ''

        if is_file:
            announce(__file__, f'Processing file [{file_or_dir}]...')
            # We are processing an index.rst file.
            with open(file_or_dir, 'r', encoding='utf-8') as fidx:
                # It is important that this is NOT fidx.readlines() because
                # it leaves blank lines containing '\n' instead of ''.
                lines = fidx.read().split('\n')

            example_title = ''
            prev_line = ''
            dir_path = os.path.dirname(file_or_dir)
            in_dir_order_directive = False

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
                elif stripped_line.startswith(EXAMPLE_HEADING_DIRECTIVE):
                    new_heading = stripped_line.replace(EXAMPLE_HEADING_DIRECTIVE, '').strip()
                    announce(__file__, f'Default section heading [{section_heading}] replaced with [{new_heading}]...')
                    section_heading = new_heading
                elif stripped_line.startswith('---'):
                    example_title = prev_line
                    announce(__file__, f'Found example   [{example_title}]...')
                elif stripped_line.startswith(LV_EXAMPLE_DIRECTIVE):
                    rel_path_from_examples_dir = stripped_line.replace(LV_EXAMPLE_DIRECTIVE, '').strip()
                    announce(__file__, f'                [{rel_path_from_examples_dir}]')
                    example_tuples.append( (example_title, rel_path_from_examples_dir) )
                elif not in_dir_order_directive and stripped_line.startswith(DIR_ORDER_DIRECTIVE):
                    in_dir_order_directive = True
                    announce(__file__, f'Found DIR-ORDER directive.')

                prev_line = stripped_line

        # Output section heading.  This occurs even when we have descended into an
        # empty directory with example sub-dirs below it.
        _emit_heading(level, section_heading, f)

        # Output examples, if any.  Will be empty when we are in an empty directory
        # with example sub-dirs below it.
        for example_tuple in example_tuples:
            example_title = example_tuple[0]
            rel_path_from_examples_dir = example_tuple[1]
            example_hdg_underline = header_defs[level + 1] * len(example_title)
            announce(__file__, f'Writing example [{rel_path_from_examples_dir}]...')
            _emit_heading(level + 1, example_title, f)
            f.write(LV_EXAMPLE_DIRECTIVE + ' ' + rel_path_from_examples_dir)
            f.write('\n\n')

    if dir_order_override:
        _validate_sub_dirs(dir_order_override, file_or_dir)
        result_dir_list = dir_order_override

    return result_dir_list


def process_dir_recursively(level: int, root_len: int, dir_bep: str, f: TextIOWrapper):
    """ Process dir `dir_bep` recursively, avoiding sub-dirs in `avoid_dirs`.

    :param level:     Directory depth below the top directory in tree.
    :param root_len:  Length of root path (gets removed from full path)
    :param dir_bep:   Directory *being processed*
    :param f:         Output file (examples.rst)
    """
    announce(__file__, f'Processing dir  [{dir_bep}]...')
    sub_dirs = []
    idx_files = []

    if not os.path.isdir(dir_bep):
        _warn(f'process_dir_recursively: `dir_bep` [{dir_bep}] does not exist.')
        return

    # For each "thing" found in `dir_bep`, build lists:  sub_dirs and idx_files.
    for dir_item in os.listdir(dir_bep):
        path_bep = os.path.join(dir_bep, dir_item)
        if os.path.isdir(path_bep):
            sub_dirs.append(path_bep)
        elif path_bep.endswith(INDEX_FILENAME):
            idx_files.append(path_bep)

    if idx_files:
        sub_dirs = _generate_output_from_dir(level, root_len, idx_files[0], True, sub_dirs, f)
        # `sub_dirs` can be replaced if `index.rst` contains a dir-order directive.
    else:
        _generate_output_from_dir(level, root_len, dir_bep, False, sub_dirs, f)

    # Now recursively process sub_dirs.
    for subdir in sub_dirs:
        if not _in_avoid_dirs_list(subdir):
            process_dir_recursively(level + 1, root_len, subdir, f)


def exec(intermediate_dir):
    announce_set_silent_mode(not DEBUG_MODE)
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

    if MAKE_WARNINGS_INTO_ERRORS and _warning_issued:
        exit(1)


if __name__ == '__main__':
    """Make module run-able as well as importable."""
    base_dir = os.path.abspath(os.path.dirname(__file__))
    os.chdir(base_dir)
    exec(sys.argv[1])
