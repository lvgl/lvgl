import os
import sys
import argparse
import shutil
import tempfile
import json
import subprocess

base_path = os.path.abspath(os.path.dirname(__file__))
sys.path.insert(0, base_path)

project_dir = os.path.abspath(os.path.join(base_path, '..', '..'))
docs_path = os.path.join(project_dir, 'docs')
sys.path.insert(0, docs_path)

import create_fake_lib_c  # NOQA
import pycparser_monkeypatch  # NOQA
import pycparser  # NOQA

doxyfile_filename = 'Doxyfile'
DEVELOP = False


intermediate_dir = tempfile.mkdtemp(suffix='.lvgl_json')


def run(output_path, lv_conf_file, output_to_stdout, target_header, filter_private, no_docstrings, *compiler_args):

    pycparser_monkeypatch.FILTER_PRIVATE = filter_private

    lvgl_dir = project_dir
    lvgl_src_dir = os.path.join(lvgl_dir, 'src')
    int_lvgl_dir = os.path.join(intermediate_dir, 'lvgl')
    lv_conf_dest_file = os.path.join(intermediate_dir, 'lv_conf.h')
    target_header_base_name = (
        os.path.splitext(os.path.split(target_header)[-1])[0]
    )

    try:
        os.mkdir(int_lvgl_dir)
        shutil.copytree(lvgl_src_dir, os.path.join(int_lvgl_dir, 'src'))
        shutil.copyfile(os.path.join(lvgl_dir, 'lvgl.h'), os.path.join(int_lvgl_dir, 'lvgl.h'))

        pp_file = os.path.join(intermediate_dir, target_header_base_name + '.pp')

        if lv_conf_file is None:
            lv_conf_templ_file = os.path.join(lvgl_dir, 'lv_conf_template.h')

            with open(lv_conf_templ_file, 'rb') as f:
                lines = f.read().decode('utf-8').split('\n')

            for i, line in enumerate(lines):
                if line.startswith('#if 0'):
                    lines[i] = '#if 1'
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
                        if line.startswith(f'#define {item} '):
                            lines[i] = f'#define {item} 1'
                            break

            with open(lv_conf_dest_file, 'wb') as f:
                f.write('\n'.join(lines).encode('utf-8'))
        else:
            shutil.copyfile(lv_conf_file, lv_conf_dest_file)

        include_dirs = [intermediate_dir, project_dir]

        if sys.platform.startswith('win'):
            import get_sdl2

            try:
                import pyMSVC  # NOQA
            except ImportError:
                sys.stderr.write(
                    '\nThe pyMSVC library is missing, '
                    'please run "pip install pyMSVC" to install it.\n'
                )
                sys.stderr.flush()
                sys.exit(-500)

            env = pyMSVC.setup_environment()  # NOQA
            cpp_cmd = ['cl', '/std:c11', '/nologo', '/P']
            output_pp = f'/Fi"{pp_file}"'
            sdl2_include, _ = get_sdl2.get_sdl2(intermediate_dir)
            include_dirs.append(sdl2_include)
            include_path_env_key = 'INCLUDE'

        elif sys.platform.startswith('darwin'):
            include_path_env_key = 'C_INCLUDE_PATH'
            cpp_cmd = [
                'clang', '-std=c11', '-E', '-DINT32_MIN=0x80000000',
            ]
            output_pp = f' >> "{pp_file}"'
        else:
            include_path_env_key = 'C_INCLUDE_PATH'
            cpp_cmd = [
                'gcc', '-std=c11', '-E', '-Wno-incompatible-pointer-types',
            ]
            output_pp = f' >> "{pp_file}"'

        fake_libc_path = create_fake_lib_c.run(intermediate_dir)

        if include_path_env_key not in os.environ:
            os.environ[include_path_env_key] = ''

        os.environ[include_path_env_key] = (
            f'{fake_libc_path}{os.pathsep}{os.environ[include_path_env_key]}'
        )

        if 'PATH' not in os.environ:
            os.environ['PATH'] = ''

        os.environ['PATH'] = (
            f'{fake_libc_path}{os.pathsep}{os.environ["PATH"]}'
        )

        cpp_cmd.extend(compiler_args)
        cpp_cmd.extend([
            '-DLV_LVGL_H_INCLUDE_SIMPLE',
            '-DLV_CONF_INCLUDE_SIMPLE',
            '-DLV_USE_DEV_VERSION'
        ])

        cpp_cmd.extend(['-DPYCPARSER', f'"-I{fake_libc_path}"'])
        cpp_cmd.extend([f'"-I{item}"' for item in include_dirs])
        cpp_cmd.append(f'"{target_header}"')

        if sys.platform.startswith('win'):
            cpp_cmd.insert(len(cpp_cmd) - 2, output_pp)
        else:
            cpp_cmd.append(output_pp)

        cpp_cmd = ' '.join(cpp_cmd)

        p = subprocess.Popen(
            cpp_cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            env=os.environ,
            shell=True
        )
        out, err = p.communicate()
        exit_code = p.returncode

        if not os.path.exists(pp_file):
            sys.stdout.write(out.decode('utf-8').strip() + '\n')
            sys.stdout.write('EXIT CODE: ' + str(exit_code) + '\n')
            sys.stderr.write(err.decode('utf-8').strip() + '\n')
            sys.stdout.flush()
            sys.stderr.flush()

            raise RuntimeError('Unknown Failure')

        with open(pp_file, 'r') as f:
            pp_data = f.read()

        cparser = pycparser.CParser()
        ast = cparser.parse(pp_data, target_header)
        doxyfile_src_file = os.path.join(docs_path, doxyfile_filename)

        ast.setup_docs(no_docstrings, lvgl_src_dir,
                       intermediate_dir, doxyfile_src_file, output_to_stdout)

        if not output_to_stdout and output_path is None:
            if not DEVELOP:
                shutil.rmtree(intermediate_dir)

            return ast

        elif output_to_stdout:
            # stdout.reset()
            print(json.dumps(ast.to_dict(), indent=4))
        else:
            if not os.path.exists(output_path):
                os.makedirs(output_path)

            output_path = os.path.join(output_path, target_header_base_name + '.json')

            with open(output_path, 'w') as f:
                f.write(json.dumps(ast.to_dict(), indent=4))

    except Exception as err:
        try:
            print(cpp_cmd)  # NOQA
            print()
        except:  # NOQA
            pass

        for key, value in os.environ.items():
            print(key + ':', value)

        print()
        import traceback

        traceback.print_exc()
        print()

        exceptions = [
            ArithmeticError,
            AssertionError,
            AttributeError,
            EOFError,
            FloatingPointError,
            GeneratorExit,
            ImportError,
            IndentationError,
            IndexError,
            KeyError,
            KeyboardInterrupt,
            LookupError,
            MemoryError,
            NameError,
            NotImplementedError,
            OverflowError,
            ReferenceError,
            RuntimeError,
            StopIteration,
            SyntaxError,
            TabError,
            SystemExit,
            TypeError,
            UnboundLocalError,
            UnicodeError,
            UnicodeEncodeError,
            UnicodeDecodeError,
            UnicodeTranslateError,
            ValueError,
            ZeroDivisionError,
            SystemError
        ]

        if isinstance(err, OSError):
            error = err.errno
        else:
            if type(err) in exceptions:
                error = ~exceptions.index(type(err))
            else:
                error = -100
    else:
        error = 0

    if DEVELOP:
        print('temporary file path:', intermediate_dir)
    else:
        shutil.rmtree(intermediate_dir)

    sys.exit(error)


if __name__ == '__main__':
    parser = argparse.ArgumentParser('-')
    parser.add_argument(
        '--output-path',
        dest='output_path',
        help=(
            'output directory for JSON file. If one is not '
            'supplied then it will be output stdout'
        ),
        action='store',
        default=None
    )
    parser.add_argument(
        '--lvgl-config',
        dest='lv_conf',
        help=(
            'path to lv_conf.h (including file name), if this is not set then '
            'a config file will be generated that has everything turned on.'
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
    parser.add_argument(
        "--target-header",
        dest="target_header",
        help=(
            "path to a custom header file. When using this to supply a custom"
            "header file you MUST insure that any LVGL includes are done so "
            "they are relitive to the LVGL repository root folder.\n\n"
            '#include "src/lvgl_private.h"\n\n'
            "If you have includes to header files that are not LVGL then you "
            "will need to add the include locations for those header files "
            "when running this script. It is done using the same manner that "
            "is used when calling a C compiler\n\n"
            "You need to provide the absolute path to the header file when "
            "using this feature."
        ),
        action="store",
        default=os.path.join(intermediate_dir, "lvgl", "lvgl.h")
    )
    parser.add_argument(
        '--filter-private',
        dest='filter_private',
        help='Internal Use',
        action='store_true',
    )
    parser.add_argument(
        '--no-docstrings',
        dest='no_docstrings',
        help='Internal Use',
        action='store_true',
    )

    args, extra_args = parser.parse_known_args()

    DEVELOP = args.develop

    run(args.output_path, args.lv_conf, args.output_path is None, args.target_header, args.filter_private, args.no_docstrings, *extra_args)
