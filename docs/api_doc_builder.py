"""api_doc_builder.py  Create and provide links to API pages in LVGL doc build.

Uses DoxygenXml class in doxygen_xml.py to make available:

- Doxygen output, and
- Doxygen-documented symbols from the C code.
"""
import os
import doxygen_xml

html_files = {}
EMIT_WARNINGS = True


def iter_src(n, p, _src_dir, _api_path):
    if p:
        out_path = os.path.join(_api_path, p)
        src_path = os.path.join(_src_dir, p)
    else:
        out_path = _api_path
        src_path = _src_dir

    index_file = None
    folders = []

    for file in os.listdir(src_path):
        if 'private' in file:
            continue

        if os.path.isdir(os.path.join(src_path, file)):
            folders.append((file, os.path.join(p, file)))
            continue

        if not file.endswith('.h'):
            continue

        if not os.path.exists(out_path):
            os.makedirs(out_path)

        if index_file is None:
            index_file = open(os.path.join(out_path, 'index.rst'), 'w')
            if n:
                index_file.write('=' * len(n))
                index_file.write('\n' + n + '\n')
                index_file.write('=' * len(n))
                index_file.write('\n\n\n')

            index_file.write('.. toctree::\n    :maxdepth: 2\n\n')

        name = os.path.splitext(file)[0]
        index_file.write('    ' + name + '\n')

        rst_file = os.path.join(out_path, name + '.rst')
        html_file = os.path.join(p, name + '.html')
        html_files[name] = html_file

        with open(rst_file, 'w') as f:
            f.write('.. _{0}_h:'.format(name))
            f.write('\n\n')
            f.write('=' * len(file))
            f.write('\n')
            f.write(file)
            f.write('\n')
            f.write('=' * len(file))
            f.write('\n\n\n')

            f.write('.. doxygenfile:: ' + file)
            f.write('\n')
            f.write('    :project: lvgl')
            f.write('\n\n')

    for name, folder in folders:
        if iter_src(name, folder, _src_dir, _api_path):
            if index_file is None:
                index_file = open(os.path.join(out_path, 'index.rst'), 'w')

                if n:
                    index_file.write('=' * len(n))
                    index_file.write('\n' + n + '\n')
                    index_file.write('=' * len(n))
                    index_file.write('\n\n\n')

                index_file.write('.. toctree::\n    :maxdepth: 2\n\n')

            index_file.write('    ' + os.path.split(folder)[-1] + '/index\n')

    if index_file is not None:
        index_file.write('\n')
        index_file.close()
        return True

    return False


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


def announce(*args):
    _args = []

    for arg in args:
        # Avoid the single quotes `repr()` puts around strings.
        if type(arg) is str:
            _args.append(arg)
        else:
            _args.append(repr(arg))

    print(f'{os.path.basename(__file__)}: ', ' '.join(_args))


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

    announce("Generating API documentation .RST files...")
    api_path = os.path.join(intermediate_dir, 'API')
    if not os.path.isdir(api_path):
        os.makedirs(api_path)

    # Generate .RST files for API pages.
    iter_src('API', '', lvgl_src_dir, api_path)

    # ---------------------------------------------------------------------
    # For each directory entry in `doc_paths` array...
    # - add API hyperlinks to .RST files in the directories in passed array.
    # ---------------------------------------------------------------------
    announce("Adding API-page hyperlinks to source docs...")

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
