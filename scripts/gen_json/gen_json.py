import os
import sys
import argparse
import tempfile
import shutil

temp_directory = tempfile.mkdtemp(suffix='.lvgl_json')

base_path = os.path.abspath(os.path.dirname(__file__))
sys.path.insert(0, base_path)

project_path = os.path.abspath(os.path.join(base_path, '..', '..'))
docs_path = os.path.join(project_path, 'docs')
sys.path.insert(0, docs_path)

import pycparser_monkeypatch  # NOQA
import pycparser  # NOQA
import json  # NOQA
import config_builder  # NOQA

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

args, extra_args = parser.parse_known_args()

output_path = args.output_path
output_to_stdout = output_path is None

fake_libc_path = os.path.join(base_path, 'fake_libc_include')
lvgl_path = project_path
lvgl_src_path = os.path.join(lvgl_path, 'src')
lvgl_header_path = os.path.join(lvgl_path, 'lvgl.h')
lvgl_config_path = args.lv_conf

temp_lvgl = os.path.join(temp_directory, 'lvgl')

os.mkdir(temp_lvgl)

shutil.copytree(lvgl_src_path, os.path.join(temp_lvgl, 'src'))
shutil.copyfile(lvgl_header_path, os.path.join(temp_lvgl, 'lvgl.h'))

lvgl_header_path = os.path.join(temp_lvgl, 'lvgl.h')

if lvgl_config_path is None:
    lvgl_config_path = os.path.join(temp_directory, 'lv_conf.h')
    config_builder.run(lvgl_config_path)
else:
    shutil.copyfile(lvgl_config_path, temp_directory)

library_dirs = []
include_dirs = [project_path]
cpp_args = ['-DCPYTHON_SDL', '-DLV_LVGL_H_INCLUDE_SIMPLE', '-DLV_USE_DEV_VERSION']

for arg in extra_args:
    if arg.startswith('-D'):
        cpp_args.append(arg)

sdl_temp_path = None

if sys.platform.startswith('win'):

    import get_sdl2
    import pyMSVC  # NOQA

    environment = pyMSVC.setup_environment()
    # print(environment)

    # compiler to use
    cpp_path = 'cl'
    sdl2_include, _ = get_sdl2.get_sdl2(temp_directory)
    include_dirs += [sdl2_include]
    cpp_args.insert(0, '/std:c11')
    include_path_env_key = 'INCLUDE'

elif sys.platform.startswith('darwin'):
    include_path_env_key = 'C_INCLUDE_PATH'
    cpp_path = 'clang'
    cpp_args.insert(0, '-std=c11')
else:
    include_path_env_key = 'C_INCLUDE_PATH'
    cpp_path = 'gcc'
    cpp_args.insert(0, '-std=c11')
    cpp_args.append('-Wno-incompatible-pointer-types')


if include_path_env_key not in os.environ:
    os.environ[include_path_env_key] = ''

os.environ[include_path_env_key] = f'{fake_libc_path}{os.pathsep}{os.environ[include_path_env_key]}'

cpp_args.extend(['/E', '-DPYCPARSER', f'-I"{fake_libc_path}"'])
cpp_args.extend([f'-I"{item}"' for item in include_dirs])
# print(cpp_args)

ast = pycparser.parse_file(
    lvgl_header_path,
    use_cpp=True,
    cpp_path=cpp_path,
    cpp_args=cpp_args,
    parser=None
)

ast.setup_docs(temp_directory)

if sdl_temp_path is not None and os.path.exists(sdl_temp_path):
    import shutil

    shutil.rmtree(sdl_temp_path)

if output_path is not None:
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    output_path = os.path.join(output_path, 'lvgl.json')

# print(ast.to_dict())

# print()
# for item in pycparser_monkeypatch.found_quals:
#     print(item)
# print()

with open(output_path, 'w') as f:
    f.write(json.dumps(ast.to_dict(), indent=4))




