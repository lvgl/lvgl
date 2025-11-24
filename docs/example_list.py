#!/usr/bin/env python3
""" example_list.py -- Gather all examples under ``lvgl/examples/`` into ``examples.rst``.

"Examples" are defined by the presence of an ``index.rst`` in a directory with
example code in a file starting with "lv_example_".  That C code is pulled into
each example in the EXAMPLES HTML page, as well as everywhere such examples are
included by putting a directive like this in the doc:

    .. include:: /examples/layouts/flex/index.rst

Note that in the intermediate dir (from which LVGL user docs are generated),
``./examples/`` is a top-level subdirectory, so no relative "../../.." etc.
is required in the path.  Using a path starting at "root" tells Sphinx to
start at the top-level directory where the docs are being generated,
typically ``lvgl/docs/intermediate/``.

See `build.py` for more information about the intermediate directory.
"""

import os
import sys

# -------------------------------------------------------------------------
# The below are a set of section headings that will be used in ``examples.rst``.
# The key value must match subdirectory names found under ``lvgl/examples/``.
# The entries in `chapter_section_headings` define the sequence of the
# top-level chapter headings in ``examples.rst``.
# -------------------------------------------------------------------------
# Directories under ``lvgl/examples/`` containing ``index.rst`` files (directly and indirectly)
chapter_section_headings = {
    "get_started"  : "Get Started",
    "styles"       : "Styles",
    "anim"         : "Animations",
    "event"        : "Events",
    "layouts"      : "Layouts",
    "scroll"       : "Scrolling",
    "widgets"      : "Widgets",
    "libs"         : "3rd-Party Libraries",
    "others"       : "Others",
    "porting"      : "Porting",
}

# Directories under ``lvgl/examples/layouts/`` containing ``index.rst`` files
layouts_section_headings = {
    "flex"         : "Flex",
    "grid"         : "Grid",
}

# Directories under ``lvgl/examples/widgets/`` containing ``index.rst`` files
widgets_section_headings = {
    "obj"          : "Base Widget",
    "animimg"      : "Animation Image",
    "arc"          : "Arc",
    "arclabel"     : "Arc Label",
    "bar"          : "Bar",
    "button"       : "Button",
    "buttonmatrix" : "Button Matrix",
    "calendar"     : "Calendar",
    "canvas"       : "Canvas",
    "chart"        : "Chart",
    "checkbox"     : "Checkbox",
    "dropdown"     : "Dropdown",
    "image"        : "Image",
    "imagebutton"  : "Image Button",
    "keyboard"     : "Keyboard",
    "label"        : "Label",
    "led"          : "LED",
    "line"         : "Line",
    "list"         : "List",
    "lottie"       : "Lottie",
    "menu"         : "Menu",
    "msgbox"       : "Message Box",
    "roller"       : "Roller",
    "scale"        :"Scale",
    "slider"       : "Slider",
    "span"         : "Span",
    "spinbox"      : "Spinbox",
    "spinner"      : "Spinner",
    "switch"       : "Switch",
    "table"        : "Table",
    "tabview"      : "Tabview",
    "textarea"     : "Textarea",
    "tileview"     : "Tileview",
    "win"          : "Window",
}

# Directories under ``lvgl/examples/libs/`` containing ``index.rst`` files
libs_section_headings = {
    "barcode"      : "Barcode",
    "bmp"          : "BMP",
    "ffmpeg"       : "FFmpeg",
    "freetype"     : "FreeType",
    "gif"          : "GIF",
    "gltf"         : "glTF",
    "gstreamer"    : "GStreamer",
    "libjpeg_turbo": "libjpeg-turbo",
    "libpng"       : "libpng",
    "libwebp"      : "LibWebP",
    "lodepng"      : "LodePNG",
    "qrcode"       : "QR-Code Generator",
    "rlottie"      : "rlottie",
    "svg"          : "SVG",
    "tiny_ttf"     : "Tiny TTF",
    "tjpgd"        : "Tiny JPEG Decompressor (TJpgDec)",
}

# Directories under ``lvgl/examples/others/`` containing ``index.rst`` files
others_section_headings = {
    "file_explorer": "File Explorer",
    "font_manager" : "Font Manager",
    "fragment"     : "Fragment Manager",
    "gestures"     : "Gestures",
    "gridnav"      : "Grid Navigation",
    "ime"          : "Pinyin IME",
    "imgfont"      : "Image Font",
    "monkey"       : "Monkey",
    "observer"     : "Observer",
    "snapshot"     : "Snapshot",
    "translation"  : "Translation",
    "xml"          : "XML Components",
}

# Directories under ``lvgl/examples/porting/`` containing ``index.rst`` files
porting_section_headings = {
    "osal"         : "OS Abstraction Layer (OSAL)",
}

# -------------------------------------------------------------------------
# This is the order that LVGL documentation uses for the section heading
# levels.  0 is the largest and 4 is the smallest.  If this order is not
# kept in the reST files Sphinx will complain, and have difficulty
# formatting the TOC correctly.
# -------------------------------------------------------------------------
HEADING = '='
CHAPTER = '*'
SECTION = '-'
SUBSECTION = '~'
SUBSUBSECTION = '^'

header_defs = {
    0: HEADING,
    1: CHAPTER,
    2: SECTION,
    3: SUBSECTION,
    4: SUBSUBSECTION
}


def process_index_rst(path):
    #  print(path)
    with open(path, 'r') as fp:
        data = fp.read()

    data = data.split('\n')

    last_line = ""
    title_tmp = ""

    for line in data:
        line = line.strip()

        if not line:
            continue

        if line.startswith('---'):
            title_tmp = last_line.strip()

        elif line.startswith('.. lv_example::'):
            name = line.replace('.. lv_example::', '').strip()
            yield name, title_tmp

        last_line = line


def write_header(h_num, text, f):
    if h_num == 2:
        f.write('\n')    # 1 extra blank line above sections
    elif h_num == 1:
        f.write('\n\n')  # 2 extra blank lines above chapters

    text = text.strip()

    if h_num == 0:
        f.write(header_defs[h_num] * len(text))
        f.write('\n')

    f.write(text + '\n')
    f.write(header_defs[h_num] * len(text))
    f.write('\n\n')


def print_item(path, lvl, d, fout):
    for k in d:
        v = d[k]
        if k.startswith(path + "/lv_example_"):
            write_header(lvl, v, fout)
            fout.write(f".. lv_example:: {k}\n")
            fout.write("\n")


def exec(intermediate_dir):
    output_path = os.path.join(intermediate_dir, 'examples.rst')
    paths = ["../examples/", "../demos/"]
    fout = open(output_path, "w")
    filelist = []

    # Recursively walk the directories in `paths` array for ``index.rst`` files.
    for path in paths:
        for root, dirs, files in os.walk(path):
            for f in files:
                # append the file name to the list
                if f.endswith("index.rst"):
                    filelist.append(os.path.join(root, f))

    d_all = {}
    # print all the file names
    for fn in filelist:
        d_all.update(dict(tuple(item for item in process_index_rst(fn))))

    # fout.write("```eval_rst\n")
    # fout.write(":github_url: |github_link_base|/examples.md\n")
    # fout.write("```\n")
    # fout.write("\n")

    fout.write('.. _examples:\n\n')
    write_header(0, 'Examples', fout)

    for chapter_hdg_key in chapter_section_headings:
        write_header(1, chapter_section_headings[chapter_hdg_key], fout)

        # If an immediate subdirectory below ``lvgl/examples/`` itself has
        # subdirectories before we get to any ``index.rst`` files, then that
        # subdirectory requires an IF in the below IF/ELSE chain.  The final
        # ELSE block handles subdirectories that directly contain ``index.rst``
        # files with no subdirectories.
        if chapter_hdg_key == "widgets":
            for key in widgets_section_headings:
                write_header(2, widgets_section_headings[key], fout)
                print_item(chapter_hdg_key + "/" + key, 3, d_all, fout)
        elif chapter_hdg_key == "layouts":
            for key in layouts_section_headings:
                write_header(2, layouts_section_headings[key], fout)
                print_item(chapter_hdg_key + "/" + key, 3, d_all, fout)
        elif chapter_hdg_key == "libs":
            for key in libs_section_headings:
                write_header(2, libs_section_headings[key], fout)
                print_item(chapter_hdg_key + "/" + key, 3, d_all, fout)
        elif chapter_hdg_key == "others":
            for key in others_section_headings:
                write_header(2, others_section_headings[key], fout)
                print_item(chapter_hdg_key + "/" + key, 3, d_all, fout)
        elif chapter_hdg_key == "porting":
            for key in porting_section_headings:
                write_header(2, porting_section_headings[key], fout)
                print_item(chapter_hdg_key + "/" + key, 3, d_all, fout)
        else:
            print_item(chapter_hdg_key, 2, d_all, fout)

        fout.write("")


if __name__ == '__main__':
    """Make module run-able as well as importable."""
    base_dir = os.path.abspath(os.path.dirname(__file__))
    os.chdir(base_dir)
    exec(sys.argv[1])
