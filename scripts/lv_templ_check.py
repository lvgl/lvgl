import re
import argparse
import os

ignored = {
    "lv_conf_template.h"
}

h_filename_pattern = r"lv_[\w.]*\.h"
h_pattern = (
    r".*?"
    r"/\*\*\n"
    r" \* +@file +(\S*)\n"
    r" \*\n"
    r" \*/\n"
    r".*?"
    r"#ifndef +(\w*_H)\n+"
    r"#define +(\w*_H)\n"
    r".*?"
    r"#endif */\* *(\w*_H) *\*/\n*"
)

c_filename_pattern = r"lv_[\w.]*\.c"
c_pattern = (
    r".*?"
    r"/\*\*\n"
    r" \* +@file +(\S*)\n"
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
instead, got:
    #ifndef {2}
    #define {3}
    ...
    #endif /*{4}*/
"""

arg_parser = argparse.ArgumentParser()
arg_parser.add_argument("--fix", action="store_true", help="fix incorrect files")
arg_parser.add_argument("-q", "--quiet", action="store_true", help="don't print anything")
arg_parser.add_argument("files", nargs='*')

args = arg_parser.parse_args()

ok = True

for file in args.files:
    basename = os.path.basename(file)
    file_ok = True

    if basename in ignored:
        continue

    if re.fullmatch(h_filename_pattern, basename) is not None:
        with open(file) as f:
            cont = f.read()

        m = re.fullmatch(h_pattern, cont, flags=re.DOTALL)
        if m is not None:
            if m[1] != basename:
                file_ok = False
                if not args.quiet:
                    print(file_comment_help_f.format(file, basename, m[1]))

            guard_name = f"LV_{basename[3:-2].upper()}_H"
            if any(m[i] != guard_name for i in range(2, 4+1)):
                file_ok = False
                if not args.quiet:
                    print(guard_help_f.format(file, guard_name, m[2], m[3], m[4]))

            replacements = (basename, guard_name, guard_name, guard_name)

    elif re.fullmatch(c_filename_pattern, basename) is not None:
        with open(file) as f:
            cont = f.read()

        m = re.match(c_pattern, cont, flags=re.DOTALL)
        if m is not None:
            if m[1] != basename:
                file_ok = False
                if not args.quiet:
                    print(file_comment_help_f.format(file, basename, m[1]))

            replacements = (basename, )

    if not file_ok:
        ok = False
        if args.fix:
            for i in reversed(range(0, len(replacements))):
                span = m.regs[i + 1]
                repl = replacements[i]
                cont = cont[:span[0]] + repl + cont[span[1]:]
            with open(file, "w") as f:
                f.write(cont)

if not ok:
    exit(1)
