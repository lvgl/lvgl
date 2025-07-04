#!/usr/bin/env python3
import os
import re
import argparse
from collections import defaultdict


style_properties_type = {
    "LV_STYLE_BG_COLOR": "LV_PROPERTY_TYPE_COLOR",
    "LV_STYLE_BG_GRAD_COLOR": "LV_PROPERTY_TYPE_COLOR",
    "LV_STYLE_BG_IMAGE_SRC": "LV_PROPERTY_TYPE_IMGSRC",
    "LV_STYLE_BG_IMAGE_RECOLOR": "LV_PROPERTY_TYPE_COLOR",
    "LV_STYLE_BORDER_COLOR": "LV_PROPERTY_TYPE_COLOR",
    "LV_STYLE_OUTLINE_COLOR": "LV_PROPERTY_TYPE_COLOR",
    "LV_STYLE_SHADOW_COLOR": "LV_PROPERTY_TYPE_COLOR",
    "LV_STYLE_IMAGE_RECOLOR": "LV_PROPERTY_TYPE_COLOR",
    "LV_STYLE_ARCH_IMAGE_SRC": "LV_PROPERTY_TYPE_IMGSRC",
    "LV_STYLE_ARCH_COLOR": "LV_PROPERTY_TYPE_COLOR",
    "LV_STYLE_TEXT_COLOR": "LV_PROPERTY_TYPE_COLOR",
    "LV_STYLE_TEXT_FONT": "LV_PROPERTY_TYPE_FONT",
    "LV_STYLE_LINE_COLOR": "LV_PROPERTY_TYPE_COLOR",
}


class Property:
    def __init__(self, widget, name, type, index, id):
        self.widget = widget
        self.name = name
        self.type = type
        self.index = index
        self.id = id


def find_headers(directory):
    if os.path.isfile(directory):
        yield directory
        return

    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.h'):
                yield os.path.join(root, file)


def read_widget_properties(directory):

    def match_properties(file_path):
        pattern = r'^\s*LV_PROPERTY_ID2?\((\w+),\s*(\w+),\s*(\w+)(,\s*(\w+))?,\s*(\d+)\)'
        with open(file_path, 'r', encoding='utf-8') as file:
            for line in file.readlines():
                match = re.match(pattern, line)
                if match:
                    id = f"LV_PROPERTY_{match.group(1).upper()}_{match.group(2).upper()}"
                    yield Property(
                        match.group(1).lower(),
                        match.group(2).lower(), match.group(3), match.group(4),
                        id)

    def match_styles(file_path):
        pattern = r'^\s+LV_STYLE_(\w+)\s*=\s*(\d+),'
        with open(file_path, 'r', encoding='utf-8') as file:
            for line in file.readlines():
                match = re.match(pattern, line)
                if match:
                    name = match.group(1).upper()
                    id = f"LV_PROPERTY_STYLE_{name}"
                    yield Property("style",
                                   match.group(1).lower(), "style",
                                   match.group(2), id)

    properties_by_widget = defaultdict(list)
    for file_path in find_headers(directory):
        for property in match_properties(file_path):
            properties_by_widget[property.widget].append(property)

        for property in match_styles(file_path):
            properties_by_widget[property.widget].append(property)

        for widget, properties in properties_by_widget.items():
            # sort properties by property name
            properties.sort(key=lambda x: x.name)
            properties_by_widget[widget] = properties

    return properties_by_widget


def write_widget_properties(output, properties_by_widget):
    # Open header file for update.
    with open(f'{output}/lv_obj_property_names.h', "w") as header:
        header.write(f'''
/**
 * @file lv_obj_property_names.h
 * GENERATED FILE, DO NOT EDIT IT!
 */
#ifndef LV_OBJ_PROPERTY_NAMES_H
#define LV_OBJ_PROPERTY_NAMES_H

#include "../../misc/lv_types.h"

#if LV_USE_OBJ_PROPERTY && LV_USE_OBJ_PROPERTY_NAME

''')

        for widget in sorted(properties_by_widget.keys()):
            properties = properties_by_widget[widget]
            file_name = f'lv_{widget}_properties.c'
            output_file = f'{output}/{file_name}'

            count = len(properties)
            if widget == 'style':
                include = "lv_style_properties.h"
                guard = None
            elif widget == "obj":
                include = "../../core/lv_obj.h"
                guard = None
            else:
                include = f'../{widget}/lv_{widget}.h'
                guard = f"#if LV_USE_{widget.upper()}"

            with open(output_file, 'w') as f:
                f.write(f'''
/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file {file_name}
 */

#include "{include}"

#if LV_USE_OBJ_PROPERTY && LV_USE_OBJ_PROPERTY_NAME

{guard if guard else ""}
/**
 * {widget.capitalize() + ' widget' if widget != 'style' else 'Style'} property names, name must be in order.
 * Generated code from properties.py
 */
/* *INDENT-OFF* */
const lv_property_name_t lv_{widget}_property_names[{count}] = {{
''')

                for property in properties:
                    name = property.name
                    name_str = '"' + name + '",'
                    f.write(f"    {{{name_str :25} {property.id},}},\n")

                f.write('};\n')
                if guard:
                    f.write(f"#endif /*LV_USE_{widget.upper()}*/\n\n")
                f.write("/* *INDENT-ON* */\n")
                f.write('#endif\n')
            header.write(
                f'    extern const lv_property_name_t lv_{widget}_property_names[{count}];\n'
            )
        header.write('#endif\n')
        header.write('#endif\n')


def write_style_header(output, properties_by_widget):
    properties = properties_by_widget['style']

    output_file = f'{output}/lv_style_properties.h'

    with open(output_file, 'w') as f:
        f.write(f'''
/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_style_properties.h
 */
#ifndef LV_STYLE_PROPERTIES_H
#define LV_STYLE_PROPERTIES_H

#include "../../core/lv_obj_property.h"
#if LV_USE_OBJ_PROPERTY


/* *INDENT-OFF* */
enum _lv_property_style_id_t {{
''')

        for property in properties:
            name = property.name
            id_type = style_properties_type.get(f"LV_STYLE_{name.upper()}",
                                                "LV_PROPERTY_TYPE_INT")
            f.write(
                f"    LV_PROPERTY_ID(STYLE, {name.upper() + ',' :25} {id_type+',' :28} LV_STYLE_{name.upper()}),\n"
            )

        f.write('};\n\n')
        f.write('#endif\n')
        f.write('#endif\n')


def main(directory, output):
    """Generate property names"""
    property = read_widget_properties(directory)
    write_widget_properties(output, property)
    write_style_header(output, property)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Search files and filter lines.')
    parser.add_argument('-d', '--directory',
                        help='Directory to lvgl root path')
    parser.add_argument(
        '-o', '--output', help='Folders to write generated properties for all widgets.')
    args = parser.parse_args()

    # default directory is the lvgl root path of where this script sits
    if args.directory is None:
        args.directory = os.path.join(os.path.dirname(__file__), "../")

    if args.output is None:
        args.output = os.path.join(args.directory, "src/widgets/property/")

    # create output directory if it doesn't exist
    os.makedirs(args.output, exist_ok=True)

    main(args.directory, args.output)
