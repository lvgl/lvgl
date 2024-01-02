import os
import sys

base_path = os.path.abspath(os.path.dirname(__file__))
sys.path.insert(0, base_path)

import pycparser_monkeypatch  # NOQA
import pycparser  # NOQA
import json  # NOQA

output_path = sys.argv[-1]

if output_path.endswith('py'):
    raise RuntimeError('you must supply an output directory for the json file')

project_path = os.path.abspath(os.path.join(base_path, '..', '..'))
fake_libc_path = os.path.join(base_path, 'fake_libc_include')
lvgl_path = project_path
lvgl_src_path = os.path.join(lvgl_path, 'src')
lvgl_header_path = os.path.join(lvgl_path, 'lvgl.h')

library_dirs = []
include_dirs = [project_path]
cpp_args = ['-DCPYTHON_SDL', '-DLV_LVGL_H_INCLUDE_SIMPLE', '-DLV_USE_DEV_VERSION']


lvgl_config_path = os.path.join(project_path, 'lv_conf.h')
lvgl_config_template_path = os.path.join(project_path, 'lv_conf_template.h')

with open(lvgl_config_template_path, 'r') as f:
    data = f.read()

data = data.split('\n')

for i, line in enumerate(data):
    if 'LV_USE' in line or 'LV_FONT' in line:
        line = [item for item in line.split(' ') if item]
        for j, item in enumerate(line):
            if item == '0':
                line[j] = '1'
        line = ' '.join(line)
        data[i] = line
data = '\n'.join(data)


with open(lvgl_config_path, 'w') as f:
    f.write(data)


for arg in sys.argv[:]:
    if arg.startswith('-D'):
        cpp_args.append(arg)
        sys.argv.remove(arg)


sdl_temp_path = None

if sys.platform.startswith('win'):

    sdl_temp_path = os.path.join(base_path, 'build')
    if not os.path.exists(sdl_temp_path):
        os.mkdir(sdl_temp_path)

    import get_sdl2
    import pyMSVC  # NOQA

    environment = pyMSVC.setup_environment()
    print(environment)

    # compiler to use
    cpp_path = 'cl'
    sdl2_include, _ = get_sdl2.get_sdl2(sdl_temp_path)
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
print(cpp_args)
ast = pycparser.parse_file(
    lvgl_header_path,
    use_cpp=True,
    cpp_path=cpp_path,
    cpp_args=cpp_args,
    parser=None
)

if os.path.exists(lvgl_config_path):
    os.remove(lvgl_config_path)

if sdl_temp_path is not None and os.path.exists(sdl_temp_path):
    import shutil

    shutil.rmtree(sdl_temp_path)

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




