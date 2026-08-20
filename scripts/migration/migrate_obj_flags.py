import os
import re
import sys


class UnsupportedConversion(ValueError):
    """Raised when an invocation can't be safely converted, so it should be
    left untouched rather than risk emitting wrong (or empty) code."""


def _split_top_level_args(args_str: str) -> list[str]:
    """Split a C-style argument list on commas."""
    args = []
    depth = 0
    current = []
    for ch in args_str:
        if ch == "(":
            depth += 1
            current.append(ch)
        elif ch == ")":
            depth -= 1
            current.append(ch)
        elif ch == "," and depth == 0:
            args.append("".join(current).strip())
            current = []
        else:
            current.append(ch)
    if current:
        tail = "".join(current).strip()
        if tail:
            args.append(tail)
    return args


def parse_invocation(invocation: str):
    """
    Parses e.g. 'lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);'
    Returns (func_name, [arg1, arg2, ...])
    """
    m = re.match(r"\s*(\w+)\s*\((.*)\)\s*;?\s*$", invocation, re.DOTALL)
    if not m:
        raise UnsupportedConversion(f"Could not parse invocation: {invocation!r}")
    func_name, args_str = m.groups()
    return func_name, _split_top_level_args(args_str)


def find_invocations(source: str, func_pattern: str = r"lv_obj_\w*_flag") -> list[str]:
    """
    Scans full file content and returns each matched call as a complete
    invocation string, e.g. 'lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN)'
    (without trailing semicolon).
    """
    invocations = []
    pattern = re.compile(func_pattern + r"\s*\(")

    for m in pattern.finditer(source):
        start = m.start()
        open_paren = m.end() - 1  # index of the '('
        depth = 0
        i = open_paren
        for i in range(open_paren, len(source)):
            if source[i] == "(":
                depth += 1
            elif source[i] == ")":
                depth -= 1
                if depth == 0:
                    break
        else:
            continue  # unbalanced, skip

        invocations.append(source[start : i + 1])

    return invocations


def flag_to_function_suffix(raw_flag: str) -> str:
    """Converts a raw LV_OBJ_FLAG_XXX to the function suffix."""
    if raw_flag.startswith("LV_OBJ_FLAG_USER_"):
        return "user"
    return raw_flag.lower().replace("lv_obj_flag_", "")


def find_all_flags_in_fn_arg(arg: str):
    flag_prefix = "LV_OBJ_FLAG_"
    flags = []
    while True:
        new_flag_prefix = arg.find(flag_prefix)
        if new_flag_prefix == -1:
            break

        flag = flag_prefix
        # walk the flag until we reach a non uppercase character or digit
        for c in arg[new_flag_prefix + len(flag_prefix) :]:
            is_valid_char = c.isupper() or c.isdigit() or c == "_"
            if not is_valid_char:
                break
            flag += c
        arg = arg[new_flag_prefix + len(flag) :]
        flags.append(flag)
    return flags


def _is_pure_flag_expression(arg: str, flags: list[str]) -> bool:
    """
    True only if `arg` is composed *entirely* of the flags we extracted,
    combined with '|' (and whitespace) - nothing else.
    """
    remainder = arg
    for flag in flags:
        remainder = remainder.replace(flag, "", 1)
    remainder = remainder.replace("|", "")
    return remainder.strip() == ""


def convert_user_flag(func_name: str, flag: str, args: list[str]) -> str:
    """Handles converting LV_OBJ_FLAG_USER_1..4 to lv_obj_set/get_user_flag(obj, bit_idx, val)."""
    m = re.search(r"LV_OBJ_FLAG_USER_([1-4])", flag)
    if not m:
        raise UnsupportedConversion(f"Invalid user flag constant: {flag}")

    bit_index = int(m.group(1)) - 1  # Map 1..4 -> 0..3

    is_remove = "remove" or "clear" in func_name
    is_add = "add" in func_name

    if func_name == "lv_obj_set_flag":
        if len(args) != 3:
            raise UnsupportedConversion(
                f"invalid lv_obj_set_flag invocation args: {args}"
            )
        return f"lv_obj_set_user_flag({args[0]}, {bit_index}, {args[2]})"
    elif is_add or is_remove:
        value = "true" if is_add else "false"
        return f"lv_obj_set_user_flag({args[0]}, {bit_index}, {value})"
    else:  # has_flag / has_flag_any
        return f"lv_obj_get_user_flag({args[0]}, {bit_index})"


def convert_flag(func_name: str, flag: str, args: list[str]) -> str:
    fn_suffix = flag_to_function_suffix(flag)

    if fn_suffix == "user":
        return convert_user_flag(func_name, flag, args)

    is_remove = "remove" in func_name
    is_add = "add" in func_name

    if func_name == "lv_obj_set_flag":
        if len(args) != 3:
            raise UnsupportedConversion(
                f"invalid lv_obj_set_flag invocation args: {args}"
            )
        fn_name = "lv_obj_set_" + fn_suffix
        invocation = f"{fn_name}({args[0]}, {args[2]})"
    elif is_add or is_remove:  # add / remove
        value = "true" if is_add else "false"
        fn_name = "lv_obj_set_" + fn_suffix
        invocation = f"{fn_name}({args[0]}, {value})"
    else:
        fn_name = "lv_obj_is_" + fn_suffix
        invocation = f"{fn_name}({args[0]})"

    return invocation


def convert_deprecated_invocation(func_name: str, args: list[str]) -> str:
    if len(args) < 2:
        raise UnsupportedConversion(
            f"invalid invocation for function {func_name} args: {args}"
        )

    flags = find_all_flags_in_fn_arg(args[1])
    if not _is_pure_flag_expression(args[1], flags):
        raise UnsupportedConversion(
            f"cannot convert {func_name}({', '.join(args)}): the flag argument "
            "isn't a plain combination of LV_OBJ_FLAG_* constants"
        )

    fn_to_separator_map = {
        "lv_obj_add_flag": ";\n",
        "lv_obj_set_flag": ";\n",
        "lv_obj_remove_flag": ";\n",
        "lv_obj_clear_flag": ";\n",
        "lv_obj_has_flag": " && ",
        "lv_obj_has_flag_any": " || ",
    }
    if func_name not in fn_to_separator_map:
        raise UnsupportedConversion(f"Unknown function {func_name}")

    separator = fn_to_separator_map[func_name]
    converted_parts = [convert_flag(func_name, flag.strip(), args) for flag in flags]
    return separator.join(converted_parts)


def convert_invocation(inv: str) -> str:
    """Parse + convert a single invocation string. Raises UnsupportedConversion
    if it can't be safely handled."""
    func_name, args = parse_invocation(inv)
    return convert_deprecated_invocation(func_name, args)


def convert_file(path: str):
    print(path)
    with open(path, "r") as f:
        content = f.read()
    invocations: list[str] = find_invocations(content)

    for inv in invocations:
        try:
            new_inv = convert_invocation(inv)
        except UnsupportedConversion as e:
            print(f"\t SKIP ({path}): {inv}\n\t    reason: {e}", file=sys.stderr)
            continue
        print("\t", inv, "->", new_inv)
        content = content.replace(inv, new_inv)

    with open(path, "w+") as f:
        f.write(content)


def recurse_convert_file(dir):
    entries = os.listdir(dir)
    extensions = {".c", ".cpp", ".h", ".hpp"}
    for entry in entries:
        full_path = os.path.join(dir, entry)

        _, extension = os.path.splitext(full_path)
        if os.path.isdir(full_path):
            recurse_convert_file(full_path)
        elif extension in extensions:
            convert_file(full_path)


# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------


def run_tests() -> bool:
    """Self-contained test suite for convert_invocation. Returns True if all
    cases pass."""

    cases = [
        (
            "lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN)",
            "lv_obj_set_hidden(obj, false)",
        ),
        (
            "lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN)",
            "lv_obj_set_hidden(obj, true)",
        ),
        (
            "lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE)",
            "lv_obj_is_scrollable(obj)",
        ),
        (
            "lv_obj_set_flag(obj, LV_OBJ_FLAG_HIDDEN, is_hidden)",
            "lv_obj_set_hidden(obj, is_hidden)",
        ),
        (
            "lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE)",
            "lv_obj_set_hidden(obj, true);\nlv_obj_set_clickable(obj, true)",
        ),
        (
            "lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE)",
            "lv_obj_set_hidden(obj, false);\n"
            "lv_obj_set_clickable(obj, false);\n"
            "lv_obj_set_scrollable(obj, false)",
        ),
        (
            "lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_WITH_ARROW)",
            "lv_obj_is_scrollable(obj) && lv_obj_is_scroll_with_arrow(obj)",
        ),
        (
            "lv_obj_has_flag_any(obj, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_CLICKABLE)",
            "lv_obj_is_hidden(obj) || lv_obj_is_clickable(obj)",
        ),
        # User Flag Test Cases
        (
            "lv_obj_add_flag(obj, LV_OBJ_FLAG_USER_1)",
            "lv_obj_set_user_flag(obj, 0, true)",
        ),
        (
            "lv_obj_remove_flag(obj, LV_OBJ_FLAG_USER_2)",
            "lv_obj_set_user_flag(obj, 1, false)",
        ),
        (
            "lv_obj_set_flag(obj, LV_OBJ_FLAG_USER_3, val)",
            "lv_obj_set_user_flag(obj, 2, val)",
        ),
        (
            "lv_obj_has_flag(obj, LV_OBJ_FLAG_USER_4)",
            "lv_obj_get_user_flag(obj, 3)",
        ),
        (
            "lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_USER_1)",
            "lv_obj_set_hidden(obj, true);\nlv_obj_set_user_flag(obj, 0, true)",
        ),
        # Rejections / Edge cases
        ("lv_obj_add_flag(obj, flag)", None),  # variable, not a literal flag
        ("lv_obj_has_flag(obj, flag | LV_OBJ_FLAG_HIDDEN)", None),  # mixed var+flag
        (
            "lv_obj_add_flag(lv_obj_t * obj, lv_obj_flag_t flag)",
            None,
        ),
        (
            "bool lv_obj_has_flag_any(const lv_obj_t * obj, lv_obj_flag_t flag)"[
                "bool lv_obj_has_flag_any(const lv_obj_t * obj, lv_obj_flag_t flag)".index(
                    "lv_obj_has_flag_any"
                ) :
            ],
            None,
        ),
    ]

    failures = 0
    for inv, expected in cases:
        try:
            result = convert_invocation(inv)
        except UnsupportedConversion:
            result = None

        if result != expected:
            failures += 1
            print(f"FAIL: {inv!r}\n  expected: {expected!r}\n  got:      {result!r}")
        else:
            print(f"OK:   {inv!r} -> {result!r}")

    if failures:
        print(f"\n{failures} test(s) FAILED")
        return False

    print(f"\nAll {len(cases)} tests passed")
    return True


def main():
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <path_to_file>")
        print(f"       {sys.argv[0]} --test    (run the built-in test suite)")
        return 1

    if sys.argv[1] == "--test":
        return 0 if run_tests() else 1

    file_or_dir = sys.argv[1]
    if not os.path.exists(file_or_dir):
        print(f"path not found: {file_or_dir}")
        return 1

    if os.path.isdir(file_or_dir):
        print(f"recursively going through {file_or_dir}")
        recurse_convert_file(file_or_dir)
    else:
        convert_file(file_or_dir)


if __name__ == "__main__":
    sys.exit(main())
