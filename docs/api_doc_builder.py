"""api_doc_builder.py  Create and provide links to API pages in LVGL doc build.

Uses DoxygenXml class in doxygen_xml.py to make available:

- Doxygen output, and
- Doxygen-documented symbols from the C code.
"""
import os
import doxygen_xml
from announce import *

html_files = {}
EMIT_WARNINGS = True
section_line_char = '='


def _create_rst_files_for_dir(a_src_root_len: int, a_src_dir_bep, h_files: [str], sub_dirs: [str], a_out_root):
    """Create the .rst files for `a_src_dir_bep` now that we have
    the lists of eligible files and directories.

    :param a_src_root_len:  Length of source-root path
    :param a_src_dir_bep:   Source directory being processed
    :param h_files:         List of eligible .H files
    :param sub_dirs:        List of eligible subdirectories
    :param a_out_root:      Output root directory (used for building paths)
    :return:                n/a
    """
    indent = '    '
    sub_path = a_src_dir_bep[a_src_root_len:]
    out_dir = str(os.path.join(a_out_root, sub_path))

    # Ensure dir exists.  Multiple dirs MAY have to be created
    # since top-level dirs are not guaranteed to produce .rst files
    # until later in sequence.
    if not os.path.isdir(out_dir):
        os.makedirs(out_dir)

    # Make a quick adjustment when we are finishing off with
    # the top-level directory.
    if len(sub_path) == 0 and out_dir.endswith(os.sep):
        # Trim trailing slash from `out_dir`.
        out_dir = out_dir[:-1]

    # index.rst
    with open(os.path.join(out_dir, 'index.rst'), 'w') as f:
        subdir_stem = os.path.split(out_dir)[-1]
        section_line = (section_line_char * len(subdir_stem)) + '\n'
        f.write(section_line)
        f.write(subdir_stem + '\n')
        f.write(section_line)
        f.write('\n')
        f.write('.. toctree::\n    :maxdepth: 2\n\n')

        for h_file in h_files:
            filename = os.path.basename(h_file)
            stem = os.path.splitext(filename)[0]
            f.write(indent + stem + '\n')

        for sub_dir in sub_dirs:
            stem = os.path.split(sub_dir)[-1]
            f.write(indent + stem + '/index\n')

    # One .rst file per h_file
    for h_file in h_files:
        filename = os.path.basename(h_file)
        stem = os.path.splitext(filename)[0]
        rst_file = os.path.join(out_dir, stem + '.rst')
        html_file = os.path.join(sub_path, stem + '.html')
        html_files[stem] = html_file

        with open(rst_file, 'w') as f:
            f.write(f'.. _{stem}_h:\n\n')
            section_line = (section_line_char * len(filename)) + '\n'
            f.write(section_line)
            f.write(filename + '\n')
            f.write(section_line)
            f.write('\n')
            f.write('.. doxygenfile:: ' + filename + '\n')
            f.write('    :project: lvgl\n\n')


def _recursively_create_api_rst_files(a_depth, a_src_root_len, a_src_dir_bep, a_out_root) -> int:
    """
    Process all .H files in directory then recursively process subdirs.
    After this, generate index.rst:
        - each .RST file corresponding with an .H file
        - each subdir/index corresponding with a subdir
          that contained (directly or indirectly) any .H files.

    :param a_depth:         Only used for testing/debugging
    :param a_src_root_len:  Length of source-root path
    :param a_src_dir_bep:   Source directory being processed
    :param a_out_root:      Output root directory (used to build output paths)
    :return:                Number of .H files encountered.
                            This is for the caller to know whether that
                            directory recursively had any .H files in it
                            to know whether it should be included at the
                            higher level `index.rst`.
    """
    h_files = []
    sub_dirs = []
    sub_dirs_w_h_files = []

    # For each "thing" found in `a_src_dir_bep`, build lists:
    # `sub_dirs` and `h_files`.
    for dir_item in os.listdir(a_src_dir_bep):
        if 'private' not in dir_item:
            path_bep = os.path.join(a_src_dir_bep, dir_item)
            if os.path.isdir(path_bep):
                sub_dirs.append(path_bep)         # Add to sub-dir list.
            else:
                if dir_item.lower().endswith('.h'):
                    eligible = (dir_item in doxygen_xml.files)
                    if eligible:
                        h_files.append(path_bep)  # Add to .H file list.

    # For each subdir...
    for sub_dir in sub_dirs:
        found_count = _recursively_create_api_rst_files(a_depth + 1, a_src_root_len, sub_dir, a_out_root)

        if found_count > 0:
            sub_dirs_w_h_files.append(sub_dir)

    # Now that we have all files and dirs in list...
    h_file_count = len(h_files) + len(sub_dirs_w_h_files)

    if h_file_count > 0:
        # Create index.rst plus .RST files for any direct .H files in dir.
        _create_rst_files_for_dir(a_src_root_len, a_src_dir_bep, h_files, sub_dirs_w_h_files, a_out_root)

    return h_file_count


def create_api_rst_files(a_src_root, a_out_root):
    src_root_len = len(a_src_root) + 1
    _recursively_create_api_rst_files(0, src_root_len, a_src_root, a_out_root)


def clean_name(nme):
    # Handle error:
    #     AttributeError: 'NoneType' object has no attribute 'startswith'
    if nme is None:
        return nme

    if nme.startswith('_lv_'):
        nme = nme[4:]
    elif nme.startswith('lv_'):
        nme = nme[3:]

    if nme.endswith('_t'):
        nme = nme[:-2]

    return nme


# Definitions:
# - "section" => The name "abc_def" has 2 sections.
# - N = number of sections in `item_name`.
# After removing leading '_lv_', 'lv_' and trailing '_t' from `obj_name`,
# do the remaining first N "sections" of `obj_name` match `item_name`
# (case sensitive)?
def is_name_match(item_name, obj_name):
    # Handle error:
    #     AttributeError: 'NoneType' object has no attribute 'split'
    if obj_name is None:
        return False

    u_num = item_name.count('_') + 1

    obj_name = obj_name.split('_')

    # Reject (False) if `obj_name` doesn't have as many sections as `item_name`.
    if len(obj_name) < u_num:
        return False

    obj_name = '_'.join(obj_name[:u_num])

    return item_name == obj_name


def get_includes(name1, name2, obj, includes):
    name2 = clean_name(name2)

    if not is_name_match(name1, name2):
        return

    if obj.parent is not None and hasattr(obj.parent, 'header_file'):
        header_file = obj.parent.header_file
    elif hasattr(obj, 'header_file'):
        header_file = obj.header_file
    else:
        return

    if not header_file:
        return

    if header_file not in html_files:
        return

    includes.add((header_file, html_files[header_file]))


def run(lvgl_src_dir, intermediate_dir, doxyfile_src_file, silent=False, *doc_paths):
    """
    This function does 2 things:
    1.  Generates .RST files for the LVGL header files that will have API
        pages generated for them.  It places these in <tmp_dir>/API/...
        following the <project_path>/src/ directory structure.
    2.  Add Sphinx hyperlinks to the end of source .RST files found
        in the `doc_paths` array directories, whose file-name stems
        match code-element names found by Doxygen.

    :param lvgl_src_dir:       platform-appropriate path to LVGL src directory
    :param intermediate_dir:   platform-appropriate path to temp dir being operated on
    :param doxyfile_src_file:  full path to src doxygen configuration file
    :param silent:             suppress action announcements?
    :param doc_paths:          list of platform-appropriate paths to find source .RST files.
    """
    doxygen_xml.EMIT_WARNINGS = EMIT_WARNINGS

    # - Generate Doxyfile replacing tokens,
    # - run Doxygen generating XML,
    # - load the XML from Doxygen output,
    xml_parser = doxygen_xml.DoxygenXml(lvgl_src_dir,
                                        intermediate_dir,
                                        doxyfile_src_file,
                                        silent
                                        )

    # Generate .RST files for API pages.
    announce(__file__, "Generating API documentation .RST files...")
    api_path = os.path.join(intermediate_dir, 'API')
    create_api_rst_files(lvgl_src_dir, api_path)
    # create_api_rst_files_orig('API', '', lvgl_src_dir, api_path)

    # ---------------------------------------------------------------------
    # For each directory entry in `doc_paths` array...
    # - add API hyperlinks to .RST files in the directories in passed array.
    # ---------------------------------------------------------------------
    announce(__file__, "Adding API-page hyperlinks to source docs...")

    for folder in doc_paths:
        # Fetch a list of '.rst' files excluding 'index.rst'.
        rst_files = list(
            (os.path.splitext(item)[0], os.path.join(folder, item))
            for item in os.listdir(folder)
            if item.endswith('.rst') and 'index.rst' not in item
        )

        # For each .RST file in that directory...
        for stem, path in rst_files:
            # Start with an empty set.
            html_includes = set()

            # Build `html_includes` set as a list of tuples containing
            # (name, html_file).  Example:  "draw.rst" has `stem` == 'draw',
            # and generates a list of tuples from .H files where matching
            # C-code-element names were found.  Example:
            # {('lv_draw_line', 'draw\\lv_draw_line.html'),
            #  ('lv_draw_sdl', 'draw\\sdl\\lv_draw_sdl.html'),
            #  ('lv_draw_sw_blend_to_i1', 'draw\\sw\\blend\\lv_draw_sw_blend_to_i1.html'),
            #  etc.}
            for container in (
                doxygen_xml.defines,
                doxygen_xml.enums,
                doxygen_xml.variables,
                doxygen_xml.namespaces,
                doxygen_xml.structures,
                doxygen_xml.unions,
                doxygen_xml.typedefs,
                doxygen_xml.functions
            ):
                for n, o in container.items():
                    get_includes(stem, n, o, html_includes)

            if html_includes:
                # Convert `html_includes` set to a list of strings containing the
                # Sphinx hyperlink syntax "link references".  Example from above:
                # [':ref:`lv_draw_line_h`\n',
                #  ':ref:`lv_draw_sdl_h`\n',
                #  ':ref:`lv_draw_sw_blend_to_i1_h`\n',
                #  etc.]
                html_includes = list(
                    ':ref:`{0}_h`\n'.format(inc)
                    for inc, _ in html_includes
                )

                # Convert that list to a single string of Sphinx hyperlink
                # references with blank lines between them.
                # :ref:`lv_draw_line_h`
                #
                # :ref:`lv_draw_sdl_h`
                #
                # :ref:`lv_draw_sw_blend_to_i1_h`
                #
                # etc.
                output = ('\n'.join(html_includes)) + '\n'

                # Append that string to the  source .RST file being processed.
                with open(path, 'rb') as f:
                    try:
                        data = f.read().decode('utf-8')
                    except UnicodeDecodeError:
                        print(path)
                        raise

                data = data.split('.. Autogenerated', 1)[0]

                data += '.. Autogenerated\n\n'
                data += output

                with open(path, 'wb') as f:
                    f.write(data.encode('utf-8'))
