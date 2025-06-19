import re
import argparse
import os
import traceback

ignored_file_prefixes = {
    "lv_conf_template.h",
    "lv_conf_cmsis.h",
    "lv_test_perf_conf.h",
    "lv_font_"
}

ignored_top_dirs = {
    ".git",
    "docs"
}

h_filename_pattern = re.compile(r"lv_[\w.]*\.h")
h_pattern = re.compile(
    r"\s*/\*\*.+?"
    r"@file +(\S*).+?"
    r"\*/\s+?#ifndef +(\w+_H)\s+#define +(\w+_H).+"
    r"#endif */\* *([^*]+?) *\*/\s*",
    flags=re.DOTALL
)

c_filename_pattern = re.compile(r"lv_[\w.]*\.c")
c_pattern = re.compile(
    r"\s*/\*\*.+?"
    r"@file +(\S*)",
    flags=re.DOTALL
)

file_comment_help_f = """\
'{0}' should have at the top:
    /**
     * @file {1}
     *
     */
instead, has:
    /**
     * @file {2}
     *
     */
"""

guard_help_f = """\
'{0}' should have include guards like:
    #ifndef {1}
    #define {1}
    ...
    #endif /*{1}*/
instead, has:
    #ifndef {2}
    #define {3}
    ...
    #endif /*{4}*/
"""

# regex, arg parsing and other stuff skipped here.

def ignore_file(basename):
    result = False

    for pattern in ignored_file_prefixes:
        if basename.startswith(pattern):
            result = True
            break

    return result


def ignore_dir(path):
    result = False

    for top_dir in ignored_top_dirs:
        sub_path = os.path.join('lvgl', top_dir)
        if sub_path in path:
            result = True
            break

    return result

def debug(*args_, **kwargs):
    if args.verbose:
        print(*args_, **kwargs)

arg_parser = argparse.ArgumentParser()
arg_parser.add_argument("--fix", action="store_true", help="fix incorrect files")
arg_parser.add_argument("-q", "--quiet", action="store_true", help="don't print mismatch info")
arg_parser.add_argument("-v", "--verbose", action="store_true", help="print debug info")
args = arg_parser.parse_args()

cfg_project_dir = '..'
base_dir = os.path.abspath(os.path.dirname(__file__))
project_dir = os.path.abspath(os.path.join(base_dir, cfg_project_dir))
debug(f'ignore dirs [{ignored_top_dirs}]')

ok = True

for root, dirs, basenames in os.walk(project_dir):
    if ignore_dir(root):
        continue

    debug(f'root [{root}]')
    for basename in basenames:
        if ignore_file(basename):
            continue

        file_ok = True
        a_file_was_processed = False
        path = os.path.join(root, basename)

        # -------------------------------------------------------------
        # H File?
        # -------------------------------------------------------------
        if h_filename_pattern.fullmatch(basename):
            a_file_was_processed = True
            debug(f'Opening H file [{path}]')

            try:
                with open(path, 'rb') as f:
                    cont = f.read().decode('utf-8')
            except:
                print(f'Error attempting UTF-8 decode on [{path}].')
                traceback.print_exc()
                continue

            m = h_pattern.fullmatch(cont)
            if m is not None:
                debug('  Match.')
                if m[1] != basename:
                    file_ok = False
                    if not args.quiet:
                        print(file_comment_help_f.format(path, basename, m[1]))

                guard_name = f"LV_{basename[3:-2].upper()}_H"
                if any(m[i] != guard_name for i in range(2, 4+1)):
                    file_ok = False
                    if not args.quiet:
                        print(guard_help_f.format(path, guard_name, m[2], m[3], m[4]))

                replacements = (basename, guard_name, guard_name, guard_name)
            else:
                debug(f'  No match with regex [{h_pattern.pattern}].')

        # -------------------------------------------------------------
        # C File?
        # -------------------------------------------------------------
        elif c_filename_pattern.fullmatch(basename):
            a_file_was_processed = True
            debug(f'Opening C file [{path}]')

            try:
                with open(path, 'rb') as f:
                    cont = f.read().decode('utf-8')
            except:
                print(f'Error attempting UTF-8 decode on [{path}].')
                traceback.print_exc()
                continue

            m = c_pattern.match(cont)
            if m is not None:
                debug('  Match.')
                if m[1] != basename:
                    file_ok = False
                    if not args.quiet:
                        print(file_comment_help_f.format(path, basename, m[1]))

                replacements = (basename, )
            else:
                debug(f'  No match with regex [{c_pattern.pattern}].')

        if a_file_was_processed:
            if not file_ok:
                debug(f'NOT OK [{path}]')
                ok = False
                if args.fix:
                    for i in reversed(range(0, len(replacements))):
                        span = m.regs[i + 1]
                        repl = replacements[i]
                        cont = cont[:span[0]] + repl + cont[span[1]:]
                    with open(path, "w") as f:
                        f.write(cont)
            else:
                debug(f'OK [{path}]')


if not ok:
    exit(1)
