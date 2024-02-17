import os
import sys
import argparse
import shutil
import tempfile
import json
import subprocess
import threading

base_path = os.path.abspath(os.path.dirname(__file__))
sys.path.insert(0, base_path)

project_path = os.path.abspath(os.path.join(base_path, '..', '..'))
docs_path = os.path.join(project_path, 'docs')
sys.path.insert(0, docs_path)

import pycparser_monkeypatch  # NOQA
import pycparser  # NOQA

DEVELOP = False


class STDOut:
    def __init__(self):
        self._stdout = sys.stdout
        sys.__stdout__ = self
        sys.stdout = self

    def write(self, data):
        pass

    def __getattr__(self, item):
        if item in self.__dict__:
            return self.__dict__[item]

        return getattr(self._stdout, item)

    def reset(self):
        sys.stdout = self._stdout


def run(output_path, lvgl_config_path, output_to_stdout, *compiler_args):
    # stdout = STDOut()

    if not output_to_stdout:
        event = threading.Event()

        def _do():
            while not event.is_set():
                event.wait(1)
                sys.stdout.write('.')
                sys.stdout.flush()
            print()

        t = threading.Thread(target=_do)
        t.daemon = True
        t.start()

    temp_directory = tempfile.mkdtemp(suffix='.lvgl_json')
    fake_libc_path = os.path.join(base_path, 'fake_libc_include')
    lvgl_path = project_path
    lvgl_src_path = os.path.join(lvgl_path, 'src')
    lvgl_header_path = os.path.join(lvgl_path, 'lvgl.h')
    temp_lvgl = os.path.join(temp_directory, 'lvgl')
    os.mkdir(temp_lvgl)

    shutil.copytree(lvgl_src_path, os.path.join(temp_lvgl, 'src'))
    shutil.copyfile(lvgl_header_path, os.path.join(temp_lvgl, 'lvgl.h'))

    lvgl_header_path = os.path.join(temp_lvgl, 'lvgl.h')

    pp_file = os.path.join(temp_directory, 'lvgl.pp')

    if lvgl_config_path is None:
        lvgl_config_path = os.path.join(lvgl_path, 'lv_conf_template.h')

        with open(lvgl_config_path, 'rb') as f:
            data = f.read().decode('utf-8').split('\n')

        for i, line in enumerate(data):
            if line.startswith('#if 0'):
                data[i] = '#if 1'
            else:
                for item in (
                    'LV_USE_LOG',
                    'LV_USE_OBJ_ID',
                    'LV_USE_OBJ_ID_BUILTIN',
                    'LV_USE_FLOAT',
                    'LV_USE_BIDI',
                    'LV_USE_LODEPNG',
                    'LV_USE_LIBPNG',
                    'LV_USE_BMP',
                    'LV_USE_TJPGD',
                    'LV_USE_LIBJPEG_TURBO',
                    'LV_USE_GIF',
                    'LV_BIN_DECODER_RAM_LOAD',
                    'LV_USE_RLE',
                    'LV_USE_QRCODE',
                    'LV_USE_BARCODE',
                    'LV_USE_TINY_TTF',
                    'LV_USE_GRIDNAV',
                    'LV_USE_FRAGMENT',
                    'LV_USE_IMGFONT',
                    'LV_USE_SNAPSHOT',
                    'LV_USE_FREETYPE'
                ):
                    if line.startswith(f'#define {item}'):
                        data[i] = f'#define {item} 1'

        with open(os.path.join(temp_directory, 'lv_conf.h'), 'wb') as f:
            f.write('\n'.join(data).encode('utf-8'))
    else:
        shutil.copyfile(lvgl_config_path, temp_directory)

    include_dirs = [project_path]

    if sys.platform.startswith('win'):
        import get_sdl2
        import pyMSVC  # NOQA

        env = pyMSVC.setup_environment()  # NOQA
        cpp_cmd = ['cl', '/std:c11', '/nologo', '/P', f'/Fi"{pp_file}"']
        sdl2_include, _ = get_sdl2.get_sdl2(temp_directory)
        include_dirs += [sdl2_include]
        include_path_env_key = 'INCLUDE'

    elif sys.platform.startswith('darwin'):
        include_path_env_key = 'C_INCLUDE_PATH'
        cpp_cmd = ['clang', '-std=c11', f'-o "{pp_file}"']
    else:
        include_path_env_key = 'C_INCLUDE_PATH'
        cpp_cmd = ['gcc', '-std=c11', '-Wno-incompatible-pointer-types', f'-o "{pp_file}"']

    if include_path_env_key not in os.environ:
        os.environ[include_path_env_key] = ''

    os.environ[include_path_env_key] = f'{fake_libc_path}{os.pathsep}{os.environ[include_path_env_key]}'

    cpp_cmd.extend(compiler_args)
    cpp_cmd.extend([
        '-DLV_LVGL_H_INCLUDE_SIMPLE',
        '-DLV_USE_DEV_VERSION'
    ])

    cpp_cmd.extend(['-DPYCPARSER', f'-I"{fake_libc_path}"'])
    cpp_cmd.extend([f'-I"{item}"' for item in include_dirs])

    cpp_cmd.append(f'"{lvgl_header_path}"')
    cpp_cmd = ' '.join(cpp_cmd)

    subprocess.getoutput(cpp_cmd)

    with open(pp_file, 'r') as f:
        pp_data = f.read()

    cparser = pycparser.CParser()
    ast = cparser.parse(pp_data, lvgl_header_path)

    forward_struct_decls = {}

    for item in ast.ext[:]:
        if (
            isinstance(item, pycparser_monkeypatch.Decl) and
            item.name is None and
            isinstance(item.type, (pycparser_monkeypatch.Struct, pycparser_monkeypatch.Union)) and
            item.type.name is not None
        ):
            if item.type.decls is None:
                forward_struct_decls[item.type.name] = [item]
            else:
                if item.type.name in forward_struct_decls:
                    decs = forward_struct_decls[item.type.name]
                    if len(decs) == 2:
                        decl, td = decs

                        td.type.type.decls = item.type.decls[:]

                        ast.ext.remove(decl)
                        ast.ext.remove(item)
        elif (
            isinstance(item, pycparser_monkeypatch.Typedef) and
            isinstance(item.type, pycparser_monkeypatch.TypeDecl) and
            item.name and item.type.declname and item.name == item.type.declname and
            isinstance(item.type.type, (pycparser_monkeypatch.Struct, pycparser_monkeypatch.Union)) and
            item.type.type.decls is None
        ):
            if item.type.type.name in forward_struct_decls:
                forward_struct_decls[item.type.type.name].append(item)

    ast.setup_docs(temp_directory)

    if not output_to_stdout and output_path is None:
        # stdout.reset()

        if not DEVELOP:
            shutil.rmtree(temp_directory)

        return ast

    elif output_to_stdout:
        # stdout.reset()
        print(json.dumps(ast.to_dict(), indent=4))
    else:
        if not os.path.exists(output_path):
            os.makedirs(output_path)

        output_path = os.path.join(output_path, 'lvgl.json')

        with open(output_path, 'w') as f:
            f.write(json.dumps(ast.to_dict(), indent=4))

        # stdout.reset()

    if not output_to_stdout:
        event.set()
        t.join()

    if DEVELOP:
        print('temporary file path:', temp_directory)
    else:
        shutil.rmtree(temp_directory)


if __name__ == '__main__':
    parser = argparse.ArgumentParser('-')
    parser.add_argument(
        '--output-path',
        dest='output_path',
        help='output directory for JSON file. If one is not supplied then it will be output stdout',
        action='store',
        default=None
    )
    parser.add_argument(
        '--lvgl-config',
        dest='lv_conf',
        help=(
            'path to lv_conf.h (including file name), if this is not set then a '
            'config file will be generated that has everything turned on.'
        ),
        action='store',
        default=None
    )
    parser.add_argument(
        '--develop',
        dest='develop',
        help='this option leaves the temporary folder in place.',
        action='store_true',
    )

    args, extra_args = parser.parse_known_args()

    DEVELOP = args.develop

    run(args.output_path, args.lv_conf, args.output_path is None, *extra_args)
