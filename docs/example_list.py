#!/usr/bin/env python3
import os


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


h1 = {
    "get_started": "Get started",
    "styles": "Styles",
    "anim": "Animations",
    "event": "Events",
    "layouts": "Layouts",
    "scroll": "Scrolling",
    "widgets": "Widgets"
}

widgets = {
    "obj": "Base object",
    "arc": "Arc",
    "bar": "Bar",
    "btn": "Button",
    "btnmatrix": "Button matrix",
    "calendar": "Calendar",
    "canvas": "Canvas",
    "chart": "Chart",
    "checkbox": "Checkbox",
    "colorwheel": "Colorwheel",
    "dropdown": "Dropdown",
    "img": "Image",
    "imagebutton": "Image button",
    "keyboard": "Keyboard",
    "label": "Label",
    "led": "LED",
    "line": "Line",
    "list": "List",
    "menu": "Menu",
    "meter": "Meter",
    "msgbox": "Message box",
    "roller": "Roller",
    "scale":"Scale",
    "slider": "Slider",
    "span": "Span",
    "spinbox": "Spinbox",
    "spinner": "Spinner",
    "switch": "Switch",
    "table": "Table",
    "tabview": "Tabview",
    "textarea": "Textarea",
    "tileview": "Tileview",
    "win": "Window",
}

HEADING = '='
CHAPTER = '#'
SECTION = '*'
SUBSECTION = '='
SUBSUBSECTION = '-'


def write_header(h_num, text, f):
    text = text.strip()
    if h_num == 0:
        f.write(header_defs[h_num] * len(text))
        f.write('\n')

    f.write(text + '\n')
    f.write(header_defs[h_num] * len(text))
    f.write('\n\n')


# This is the order that Sphinx uses for the headings/titles. 0 is the
# largest and 4 is the smallest. If this order is not kept in the reST files
# Sphinx will complain
header_defs = {
    0: HEADING,
    1: CHAPTER,
    2: SECTION,
    3: SUBSECTION,
    4: SUBSUBSECTION
}

layouts = {
    "flex": "Flex",
    "grid": "Grid",
}


def print_item(path, lvl, d, fout):
    for k in d:
        v = d[k]
        if k.startswith(path + "/lv_example_"):
            write_header(lvl, v, fout)
            fout.write(f".. lv_example:: {k}\n")
            fout.write("\n")


def exec(temp_directory):
    output_path = os.path.join(temp_directory, 'examples.rst')

    paths = ["../examples/", "../demos/"]
    fout = open(output_path, "w")
    filelist = []

    for path in paths:
        for root, dirs, files in os.walk(path):
            for f in files:
                # append the file name to the list
                filelist.append(os.path.join(root, f))

    filelist = [fi for fi in filelist if fi.endswith("index.rst")]

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

    for h in h1:
        write_header(1, h1[h], fout)

        if h == "widgets":
            for w in widgets:
                write_header(2, widgets[w], fout)
                print_item(h + "/" + w, 3, d_all, fout)
        elif h == "layouts":
            for l in layouts:
                write_header(2, layouts[l], fout)
                print_item(h + "/" + l, 3, d_all, fout)
        else:
            print_item(h, 2, d_all, fout)

        fout.write("")
