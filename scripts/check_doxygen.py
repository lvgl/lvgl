#!/usr/bin/env python3

"""
LVGL Doxygen Comment Checker

Checks that public function declarations in .h files under src/ have
proper Doxygen comments with @param and @return tags.

Modes:
    --diff <commit-range>   Only check functions touched by the diff (CI mode)
    --file <path>           Check all functions in a specific file
    --all                   Check all src/**/*.h files (full scan)
    --self-test             Run built-in self-tests

Exit code:
    0   All checked functions have proper Doxygen comments
    1   One or more functions are missing or have incomplete Doxygen

See: https://docs.lvgl.io/master/contributing/pull_requests.html
"""

import argparse
import os
import re
import signal
import subprocess
import sys
import time
from typing import Dict, List, Optional, Set, Tuple

# Default per-file timeout in seconds
DEFAULT_FILE_TIMEOUT = 5


# ---------------------------------------------------------------------------
# Patterns
# ---------------------------------------------------------------------------

# Matches a C function declaration (not definition) in a header.
# Captures: return_type, function_name, param_list
# Handles multi-line declarations by working on joined text.
# The param list uses a lazy match up to ");", supporting nested parens
# (e.g. function pointer params like "void (*cb)(void *)").
FUNC_DECL_RE = re.compile(
    r"^"
    r"(?!.*\b(?:typedef|struct|enum|union)\b)"  # skip typedefs/struct/enum/union
    r"([\w][\w\s*]*?)"  # return type (non-greedy)
    r"\b(lv_\w+)"  # function name starting with lv_
    r"\s*\("  # opening paren
    r"(.*?)"  # param list (lazy, allows nested parens)
    r"\)\s*;",  # closing paren + semicolon
    re.MULTILINE,
)

# Matches @param tags — supports both "@param name" and "@param[in] name" forms
PARAM_TAG_RE = re.compile(r"@param(?:\s*\[(?:in|out|in,\s*out)\])?\s+(\w+)")

# Matches @return tag
RETURN_TAG_RE = re.compile(r"@return\b")

# Files/dirs to skip
SKIP_DIRS = {
    "libs",  # third-party code
}

SKIP_FILE_PREFIXES = (
    "lv_conf",
    "lv_api_map",
)

SKIP_FILE_SUFFIXES = ("_gen.h",)  # auto-generated files like lv_obj_style_gen.h

# Functions to skip (internal helpers, callbacks, etc.)
SKIP_FUNC_PATTERNS = (re.compile(r"^lv_\w+_class$"),)  # class descriptors


# ---------------------------------------------------------------------------
# Core logic
# ---------------------------------------------------------------------------


def should_skip_file(filepath: str) -> bool:
    """Check if a file should be skipped."""
    basename = os.path.basename(filepath)
    for prefix in SKIP_FILE_PREFIXES:
        if basename.startswith(prefix):
            return True
    for suffix in SKIP_FILE_SUFFIXES:
        if basename.endswith(suffix):
            return True

    parts = filepath.replace("\\", "/").split("/")
    for d in SKIP_DIRS:
        if d in parts:
            return True

    return False


def parse_params(param_str: str) -> List[str]:
    """Extract parameter names from a C function parameter list."""
    param_str = param_str.strip()
    if not param_str or param_str == "void":
        return []

    params = []
    depth = 0
    current = []
    for ch in param_str:
        if ch == "(":
            depth += 1
            current.append(ch)
        elif ch == ")":
            depth -= 1
            current.append(ch)
        elif ch == "," and depth == 0:
            params.append("".join(current).strip())
            current = []
        else:
            current.append(ch)
    if current:
        params.append("".join(current).strip())

    names = []
    for p in params:
        p = p.strip()
        if not p or p == "...":
            continue
        # Handle function pointer params: void (*cb)(lv_obj_t *)
        fp_match = re.search(r"\(\s*\*\s*(\w+)\s*\)", p)
        if fp_match:
            names.append(fp_match.group(1))
            continue
        # Handle array params: char buf[64]
        arr_match = re.search(r"(\w+)\s*\[", p)
        if arr_match:
            names.append(arr_match.group(1))
            continue
        # Normal param: last word token
        tokens = re.findall(r"\w+", p)
        if tokens:
            # Skip if last token looks like a type (all caps or _t suffix)
            name = tokens[-1]
            names.append(name)
    return names


def returns_value(return_type: str) -> bool:
    """Check if a return type is non-void (i.e. needs @return)."""
    rt = return_type.strip()
    # Remove C qualifiers
    rt = re.sub(r"\b(static|inline|extern|const|volatile)\b", "", rt).strip()
    # Remove LVGL attribute/deprecation macros (e.g. LV_ATTRIBUTE_FLUSH_READY, LV_DEPRECATED)
    rt = re.sub(r"\bLV_\w+\b", "", rt).strip()
    return rt != "void" and rt != ""


def find_doxygen_for_line(content: str, func_start: int) -> Optional[str]:
    """Find the Doxygen comment block immediately preceding a function declaration.

    Args:
        content: full file content
        func_start: character offset where the function declaration starts

    Returns:
        The Doxygen comment text, or None if not found.
    """
    # Look backwards from func_start for a */ ending
    before = content[:func_start].rstrip()

    # The doxygen block should end right before the function (possibly with whitespace)
    if not before.endswith("*/"):
        return None

    # Find the matching /**
    block_end = len(before)
    block_start = before.rfind("/**")
    if block_start == -1:
        return None

    block = before[block_start:block_end]

    # Verify nothing significant between the block end and function start
    between = content[block_start + len(block) : func_start].strip()
    # Allow preprocessor directives and blank lines between comment and function
    between_lines = [ln.strip() for ln in between.split("\n") if ln.strip()]
    for line in between_lines:
        if (
            not line.startswith("#")
            and not line.startswith("/*")
            and not line.startswith("*")
        ):
            return None

    return block


def check_function(
    func_name: str,
    return_type: str,
    param_str: str,
    doxygen_block: Optional[str],
    filepath: str,
    line_num: int,
) -> List[str]:
    """Check a single function's Doxygen comment. Returns list of error messages."""
    errors = []

    if doxygen_block is None:
        errors.append(
            f"{filepath}:{line_num}: {func_name}(): missing Doxygen comment block"
        )
        return errors

    # Check @param tags
    expected_params = parse_params(param_str)
    documented_params = PARAM_TAG_RE.findall(doxygen_block)

    for p in expected_params:
        if p not in documented_params:
            errors.append(
                f"{filepath}:{line_num}: {func_name}(): " f"missing @param for '{p}'"
            )

    # Check @return tag
    if returns_value(return_type):
        if not RETURN_TAG_RE.search(doxygen_block):
            errors.append(
                f"{filepath}:{line_num}: {func_name}(): "
                f"missing @return (returns {return_type.strip()})"
            )

    return errors


def should_skip_func(func_name: str) -> bool:
    """Check if a function should be skipped."""
    for pat in SKIP_FUNC_PATTERNS:
        if pat.match(func_name):
            return True
    return False


def join_multiline_decls(content: str) -> str:
    """Join function declarations that span multiple lines into single lines.

    This handles cases like:
        lv_observer_t * lv_obj_bind_style(lv_obj_t * obj, const lv_style_t * style,
                                          lv_style_selector_t selector);
    """
    lines = content.split("\n")
    result = []
    pending = None
    paren_depth = 0

    for line in lines:
        stripped = line.strip()

        if pending is not None:
            pending += " " + stripped
            paren_depth += stripped.count("(") - stripped.count(")")
            if paren_depth <= 0 and ";" in stripped:
                result.append(pending)
                pending = None
                paren_depth = 0
            continue

        # Check if this line starts a function declaration that continues
        # Must start with a type/qualifier, contain '(' but not end with ';'
        if (
            re.match(r"\s*(?:static\s+inline\s+|extern\s+|const\s+)*\w", stripped)
            and "(" in stripped
            and not stripped.endswith(";")
            and not stripped.endswith("{")
            and not stripped.startswith("#")
            and not stripped.startswith("//")
            and not stripped.startswith("/*")
            and not stripped.startswith("*")
        ):
            paren_depth = stripped.count("(") - stripped.count(")")
            if paren_depth > 0:
                pending = stripped
                continue

        result.append(line)

    if pending:
        result.append(pending)

    return "\n".join(result)


class FileTimeoutError(Exception):
    """Raised when a single file check exceeds the timeout."""

    pass


def _timeout_handler(signum, frame):
    raise FileTimeoutError("file check timed out")


def check_file(
    filepath: str,
    target_lines: Optional[Set[int]] = None,
    timeout: int = DEFAULT_FILE_TIMEOUT,
) -> List[str]:
    """Check all public function declarations in a header file.

    Args:
        filepath: path to the .h file
        target_lines: if set, only check functions on these line numbers (1-based).
                      If None, check all functions.
        timeout: max seconds per file (0 = no limit).

    Returns:
        List of error messages.
    """
    if should_skip_file(filepath):
        return []

    # Set per-file timeout (Unix only, graceful fallback on Windows)
    old_handler = None
    if timeout > 0 and hasattr(signal, "SIGALRM"):
        old_handler = signal.signal(signal.SIGALRM, _timeout_handler)
        signal.alarm(timeout)

    try:
        return _check_file_impl(filepath, target_lines)
    except FileTimeoutError:
        return [f"{filepath}:0: SKIPPED (exceeded {timeout}s timeout)"]
    finally:
        if timeout > 0 and hasattr(signal, "SIGALRM"):
            signal.alarm(0)
            if old_handler is not None:
                signal.signal(signal.SIGALRM, old_handler)


def _check_file_impl(
    filepath: str, target_lines: Optional[Set[int]] = None
) -> List[str]:
    """Internal implementation of check_file without timeout wrapper."""

    try:
        with open(filepath, "r", encoding="utf-8") as f:
            raw_content = f.read()
    except (OSError, UnicodeDecodeError):
        return []

    # Build a line-number lookup for the original content
    line_offsets = [0]
    for i, ch in enumerate(raw_content):
        if ch == "\n":
            line_offsets.append(i + 1)

    def offset_to_line(offset: int) -> int:
        """Convert character offset to 1-based line number."""
        lo, hi = 0, len(line_offsets) - 1
        while lo < hi:
            mid = (lo + hi + 1) // 2
            if line_offsets[mid] <= offset:
                lo = mid
            else:
                hi = mid - 1
        return lo + 1

    # Join multi-line declarations for regex matching
    joined = join_multiline_decls(raw_content)

    # Build an index of all lv_* function name positions in the original content
    # so we can match each regex hit to the correct location (not just the first).
    func_positions: Dict[str, List[int]] = {}
    for fm in re.finditer(r"\b(lv_\w+)\s*\(", raw_content):
        name = fm.group(1)
        if name not in func_positions:
            func_positions[name] = []
        func_positions[name].append(fm.start())

    errors = []

    for m in FUNC_DECL_RE.finditer(joined):
        return_type = m.group(1)
        func_name = m.group(2)
        param_str = m.group(3)

        if should_skip_func(func_name):
            continue

        # Find this function in the original content to get accurate line number.
        # Use the position list to pick the next unprocessed occurrence,
        # avoiding the "first match only" bug for repeated function names.
        positions = func_positions.get(func_name, [])
        if not positions:
            continue

        func_offset = positions.pop(0)
        line_num = offset_to_line(func_offset)

        # If we're filtering by lines, skip functions not in the target set
        if target_lines is not None and line_num not in target_lines:
            continue

        # Find the start of the declaration line (go back to find return type)
        decl_line_start = raw_content.rfind("\n", 0, func_offset)
        if decl_line_start == -1:
            decl_line_start = 0
        else:
            decl_line_start += 1

        doxygen = find_doxygen_for_line(raw_content, decl_line_start)
        func_errors = check_function(
            func_name, return_type, param_str, doxygen, filepath, line_num
        )
        errors.extend(func_errors)

    return errors


# ---------------------------------------------------------------------------
# Git diff integration
# ---------------------------------------------------------------------------


def git_run(*args) -> Tuple[str, int]:
    """Run a git command and return (stdout, returncode)."""
    result = subprocess.run(["git"] + list(args), capture_output=True, text=True)
    return result.stdout.strip(), result.returncode


def get_changed_header_lines(commit_range: str) -> Dict[str, Set[int]]:
    """Get changed .h files and their changed line numbers from a git diff.

    Returns:
        Dict mapping filepath -> set of changed line numbers (1-based).
    """
    diff_output, rc = git_run(
        "diff",
        "--unified=0",
        "--diff-filter=ACMR",
        commit_range,
        "--",
        "src/*.h",
        "src/**/*.h",
    )
    if rc != 0 or not diff_output:
        return {}

    result: Dict[str, Set[int]] = {}
    current_file = None

    for line in diff_output.split("\n"):
        # +++ b/src/core/lv_obj.h
        if line.startswith("+++ b/"):
            current_file = line[6:]
            if current_file not in result:
                result[current_file] = set()
        # @@ -old,count +new,count @@
        elif line.startswith("@@") and current_file:
            hunk_match = re.search(r"\+(\d+)(?:,(\d+))?", line)
            if hunk_match:
                start = int(hunk_match.group(1))
                count = int(hunk_match.group(2)) if hunk_match.group(2) else 1
                if count == 0:
                    # Pure deletion hunk: still record the start line so
                    # nearby functions get checked (the context expansion
                    # in check_diff will cover the surrounding declarations).
                    result[current_file].add(start)
                else:
                    for i in range(start, start + count):
                        result[current_file].add(i)

    return result


def check_diff(commit_range: str, timeout: int = DEFAULT_FILE_TIMEOUT) -> List[str]:
    """Check Doxygen comments for functions changed in the given commit range."""
    changed = get_changed_header_lines(commit_range)
    if not changed:
        print("No changed header files found in diff.")
        return []

    all_errors = []
    for filepath, lines in sorted(changed.items()):
        if not filepath.endswith(".h"):
            continue
        # Expand each changed line by a context window so that edits to
        # a Doxygen block or multi-line declaration parameters also trigger
        # a check on the associated function declaration.
        expanded = set()
        for ln in lines:
            for offset in range(-15, 6):
                expanded.add(ln + offset)
        expanded.discard(0)
        errs = check_file(filepath, target_lines=expanded, timeout=timeout)
        all_errors.extend(errs)

    return all_errors


# ---------------------------------------------------------------------------
# Full scan
# ---------------------------------------------------------------------------


def find_all_headers(root: str) -> List[str]:
    """Find all .h files under root/src/."""
    headers = []
    src_dir = os.path.join(root, "src")
    if not os.path.isdir(src_dir):
        return headers
    for dirpath, _, filenames in os.walk(src_dir):
        for fn in filenames:
            if fn.endswith(".h"):
                headers.append(os.path.join(dirpath, fn))
    return sorted(headers)


def check_all(
    root: str, verbose: bool = False, timeout: int = DEFAULT_FILE_TIMEOUT
) -> List[str]:
    """Check all header files under root/src/."""
    headers = find_all_headers(root)
    all_errors = []
    skipped = 0
    t_start = time.time()
    for idx, h in enumerate(headers):
        t0 = time.time()
        errs = check_file(h, timeout=timeout)
        elapsed = time.time() - t0
        if any("SKIPPED" in e for e in errs):
            skipped += 1
        if verbose or elapsed > 1.0:
            short = os.path.relpath(h, root)
            print(
                f"  [{idx + 1}/{len(headers)}] {short} ({elapsed:.2f}s, {len(errs)} issues)"
            )
        all_errors.extend(errs)
    total_time = time.time() - t_start
    print(
        f"Scanned {len(headers)} files in {total_time:.1f}s "
        f"({skipped} skipped due to timeout)"
    )
    return all_errors


# ---------------------------------------------------------------------------
# Self-test
# ---------------------------------------------------------------------------


def self_test() -> int:
    """Run built-in self-tests."""
    passed = 0
    failed = 0
    total = 0

    print("=" * 60)
    print(" Doxygen Comment Checker Self-Test")
    print("=" * 60)

    # --- Test parse_params ---
    print("\n--- parse_params ---")
    param_tests = [
        ("void", []),
        ("", []),
        ("lv_obj_t * obj", ["obj"]),
        ("lv_obj_t * obj, lv_obj_flag_t f", ["obj", "f"]),
        ("lv_obj_t * obj, lv_obj_flag_t f, bool v", ["obj", "f", "v"]),
        (
            "const lv_obj_t * obj, lv_part_t part, const char * txt",
            ["obj", "part", "txt"],
        ),
        ("void (*cb)(lv_obj_t *)", ["cb"]),
        (
            "lv_obj_t * obj, void (*event_cb)(lv_event_t *), int32_t id",
            ["obj", "event_cb", "id"],
        ),
        ("char buf[64], size_t len", ["buf", "len"]),
        ("const lv_obj_class_t * class_p, lv_obj_t * obj", ["class_p", "obj"]),
        (
            "lv_obj_t * obj, void (* free_cb)(void * data), int32_t id",
            ["obj", "free_cb", "id"],
        ),
    ]
    for param_str, expected in param_tests:
        total += 1
        result = parse_params(param_str)
        if result == expected:
            passed += 1
            print(f"  ✓ parse_params({param_str!r}) = {result}")
        else:
            failed += 1
            print(f"  ✗ parse_params({param_str!r})")
            print(f"    expected: {expected}")
            print(f"    got:      {result}")

    # --- Test returns_value ---
    print("\n--- returns_value ---")
    return_tests = [
        ("void", False),
        ("lv_obj_t *", True),
        ("bool", True),
        ("int32_t", True),
        ("const lv_obj_class_t *", True),
        ("static inline void", False),
        ("static inline int32_t", True),
        ("lv_style_value_t", True),
        ("LV_ATTRIBUTE_FLUSH_READY void", False),
        ("LV_DEPRECATED void", False),
        ("LV_ATTRIBUTE_FAST_MEM lv_obj_t *", True),
    ]
    for rt, expected in return_tests:
        total += 1
        result = returns_value(rt)
        if result == expected:
            passed += 1
            print(f"  ✓ returns_value({rt!r}) = {result}")
        else:
            failed += 1
            print(f"  ✗ returns_value({rt!r})")
            print(f"    expected: {expected}, got: {result}")

    # --- Test check_function with synthetic data ---
    print("\n--- check_function (should PASS) ---")
    good_cases = [
        (
            "void function, all params documented",
            "lv_test_func",
            "void",
            "lv_obj_t * obj, int32_t value",
            "/** Set a value\n * @param obj pointer\n * @param value the value\n */",
        ),
        (
            "non-void return with @return",
            "lv_test_get",
            "bool",
            "const lv_obj_t * obj",
            "/** Get flag\n * @param obj pointer\n * @return true if set\n */",
        ),
        (
            "void function no params",
            "lv_test_init",
            "void",
            "void",
            "/** Initialize the module\n */",
        ),
        (
            "function pointer param",
            "lv_test_cb",
            "void",
            "lv_obj_t * obj, void (*cb)(lv_event_t *)",
            "/** Set callback\n * @param obj pointer\n * @param cb callback\n */",
        ),
        (
            "@param with [in] qualifier (spaced)",
            "lv_test_qual",
            "void",
            "lv_obj_t * obj",
            "/** Do something\n * @param [in] obj pointer\n */",
        ),
        (
            "@param[in] qualifier (no space)",
            "lv_test_qual_in",
            "void",
            "lv_obj_t * obj",
            "/** Do something\n * @param[in] obj pointer\n */",
        ),
        (
            "@param[out] qualifier (no space)",
            "lv_test_qual_out",
            "void",
            "lv_obj_t * obj",
            "/** Do something\n * @param[out] obj pointer\n */",
        ),
        (
            "@param[in,out] qualifier (no space)",
            "lv_test_qual_inout",
            "void",
            "lv_obj_t * obj",
            "/** Do something\n * @param[in,out] obj pointer\n */",
        ),
        (
            "mixed @param[in] and @param[out] qualifiers",
            "lv_test_qual_mixed",
            "void",
            "const lv_obj_t * src, lv_obj_t * dst",
            "/** Copy object\n * @param[in] src source pointer\n * @param[out] dst destination pointer\n */",
        ),
    ]
    for desc, func_name, ret_type, params, doxygen in good_cases:
        total += 1
        errs = check_function(func_name, ret_type, params, doxygen, "test.h", 1)
        if not errs:
            passed += 1
            print(f"  ✓ PASS  [{desc}]")
        else:
            failed += 1
            print(f"  ✗ FAIL  [{desc}]")
            for e in errs:
                print(f"    {e}")

    print("\n--- check_function (should FAIL) ---")
    bad_cases = [
        (
            "missing Doxygen block entirely",
            "lv_test_none",
            "void",
            "lv_obj_t * obj",
            None,
            ["missing Doxygen comment block"],
        ),
        (
            "missing @param",
            "lv_test_noparam",
            "void",
            "lv_obj_t * obj, int32_t value",
            "/** Do something\n * @param obj pointer\n */",
            ["missing @param for 'value'"],
        ),
        (
            "missing @return for non-void",
            "lv_test_noret",
            "lv_obj_t *",
            "lv_obj_t * parent",
            "/** Create object\n * @param parent pointer\n */",
            ["missing @return"],
        ),
        (
            "missing both @param and @return",
            "lv_test_empty",
            "bool",
            "lv_obj_t * obj, lv_state_t state",
            "/** Check state\n */",
            [
                "missing @param for 'obj'",
                "missing @param for 'state'",
                "missing @return",
            ],
        ),
    ]
    for desc, func_name, ret_type, params, doxygen, expected_fragments in bad_cases:
        total += 1
        errs = check_function(func_name, ret_type, params, doxygen, "test.h", 1)
        all_found = True
        for frag in expected_fragments:
            if not any(frag in e for e in errs):
                all_found = False
                break
        if errs and all_found:
            passed += 1
            print(f"  ✓ PASS  [{desc}] -> caught: {errs[0]}")
        else:
            failed += 1
            print(f"  ✗ FAIL  [{desc}]")
            print(f"    expected fragments: {expected_fragments}")
            print(f"    got errors: {errs}")

    # --- Test check_file with synthetic file content ---
    print("\n--- check_file (synthetic file) ---")

    import tempfile

    # Good file: all functions documented
    good_file = """\
/**
 * @file test_good.h
 */

#ifndef LV_TEST_GOOD_H
#define LV_TEST_GOOD_H

/**
 * Create an object
 * @param parent    pointer to parent
 * @return          pointer to new object
 */
lv_obj_t * lv_test_create(lv_obj_t * parent);

/**
 * Set a flag
 * @param obj   pointer to object
 * @param f     flag value
 */
void lv_test_set_flag(lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Initialize module
 */
void lv_test_init(void);

#endif /* LV_TEST_GOOD_H */
"""

    # Bad file: missing docs
    bad_file = """\
/**
 * @file test_bad.h
 */

#ifndef LV_TEST_BAD_H
#define LV_TEST_BAD_H

lv_obj_t * lv_test_no_doc(lv_obj_t * parent);

/**
 * Partial doc - missing param for value and missing return tag
 * @param obj   pointer to object
 */
bool lv_test_partial(lv_obj_t * obj, int32_t value);

/**
 * Good doc
 * @param obj   pointer
 */
void lv_test_ok(lv_obj_t * obj);

#endif /* LV_TEST_BAD_H */
"""

    # Multi-line declaration file
    multiline_file = """\
/**
 * @file test_multi.h
 */

#ifndef LV_TEST_MULTI_H
#define LV_TEST_MULTI_H

/**
 * Bind style
 * @param obj       pointer
 * @param style     style pointer
 * @param selector  selector
 * @param subject   subject pointer
 * @param ref_value reference value
 * @return          pointer to observer
 */
lv_observer_t * lv_test_bind(lv_obj_t * obj, const lv_style_t * style,
                             lv_style_selector_t selector,
                             lv_subject_t * subject, int32_t ref_value);

#endif /* LV_TEST_MULTI_H */
"""

    test_files = [
        ("good file (all documented)", good_file, 0),
        (
            "bad file (missing docs)",
            bad_file,
            3,
        ),  # no_doc: missing block; partial: missing @param value + missing @return
        ("multi-line declaration", multiline_file, 0),
    ]

    for desc, content, expected_error_count in test_files:
        total += 1
        with tempfile.NamedTemporaryFile(
            mode="w", suffix=".h", prefix="lv_test_", delete=False, encoding="utf-8"
        ) as f:
            f.write(content)
            tmp_path = f.name
        try:
            errs = check_file(tmp_path)
            if len(errs) == expected_error_count:
                passed += 1
                print(
                    f"  ✓ PASS  [{desc}] -> {len(errs)} errors (expected {expected_error_count})"
                )
            else:
                failed += 1
                print(f"  ✗ FAIL  [{desc}]")
                print(f"    expected {expected_error_count} errors, got {len(errs)}:")
                for e in errs:
                    print(f"      {e}")
        finally:
            os.unlink(tmp_path)

    # --- Test should_skip_file ---
    print("\n--- should_skip_file ---")
    skip_tests = [
        ("src/libs/qrcode/lv_qrcode.h", True),
        ("src/core/lv_obj.h", False),
        ("src/lv_conf_internal.h", True),
        ("src/lv_api_map_v9_1.h", True),
        ("src/widgets/lv_btn_gen.h", True),
        ("src/misc/lv_utils.h", False),
    ]
    for path, expected in skip_tests:
        total += 1
        result = should_skip_file(path)
        if result == expected:
            passed += 1
            print(f"  ✓ should_skip_file({path!r}) = {result}")
        else:
            failed += 1
            print(f"  ✗ should_skip_file({path!r}): expected {expected}, got {result}")

    # --- Test join_multiline_decls ---
    print("\n--- join_multiline_decls ---")
    multi_input = (
        "lv_observer_t * lv_obj_bind_style(lv_obj_t * obj, const lv_style_t * style,\n"
        "                                  lv_style_selector_t selector);\n"
    )
    joined = join_multiline_decls(multi_input)
    total += 1
    if "lv_style_selector_t selector);" in joined and joined.count(
        "\n"
    ) < multi_input.count("\n"):
        passed += 1
        print("  ✓ multi-line join works")
    else:
        failed += 1
        print("  ✗ multi-line join failed")
        print(f"    result: {joined!r}")

    # --- Summary ---
    print(f"\n{'=' * 60}")
    print(f" Results: {passed}/{total} passed, {failed} failed")
    print(f"{'=' * 60}")

    # --- Lint self ---
    print(f"\n{'=' * 60}")
    print(" Lint Check (self)")
    print(f"{'=' * 60}")

    script_path = os.path.abspath(__file__)
    lint_failed = False

    try:
        import py_compile

        py_compile.compile(script_path, doraise=True)
        print("  ✓ py_compile: syntax OK")
    except py_compile.PyCompileError as e:
        print(f"  ✗ py_compile: {e}")
        lint_failed = True

    try:
        result = subprocess.run(
            [
                "flake8",
                "--max-line-length=120",
                "--ignore=E501,W503,E402,E203",
                script_path,
            ],
            capture_output=True,
            text=True,
        )
        if result.returncode == 0:
            print("  ✓ flake8: no issues")
        else:
            print("  ✗ flake8:")
            for line in result.stdout.strip().splitlines():
                print(f"    {line}")
            lint_failed = True
    except FileNotFoundError:
        print("  - flake8: not installed, skipped")

    if lint_failed:
        failed += 1

    return 0 if failed == 0 else 1


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------


def main():
    parser = argparse.ArgumentParser(
        description="LVGL Doxygen Comment Checker",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument(
        "--self-test",
        action="store_true",
        help="Run built-in self-tests",
    )
    parser.add_argument(
        "--diff",
        type=str,
        default=None,
        help='Git commit range to check (e.g. "HEAD~5...HEAD" or "abc123...def456")',
    )
    parser.add_argument(
        "--file",
        type=str,
        default=None,
        help="Check a specific header file",
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Check all src/**/*.h files",
    )
    parser.add_argument(
        "--fail-under",
        type=int,
        default=0,
        help="Maximum allowed number of errors (default: 0 = any error fails)",
    )
    parser.add_argument(
        "--timeout",
        type=int,
        default=DEFAULT_FILE_TIMEOUT,
        help=f"Per-file timeout in seconds (default: {DEFAULT_FILE_TIMEOUT}, 0=no limit)",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Print progress for every file (not just slow ones)",
    )
    args = parser.parse_args()

    if args.self_test:
        return self_test()

    if args.diff:
        errors = check_diff(args.diff, timeout=args.timeout)
    elif args.file:
        errors = check_file(args.file, timeout=args.timeout)
    elif args.all:
        base_dir = os.path.abspath(os.path.dirname(__file__))
        project_dir = os.path.abspath(os.path.join(base_dir, ".."))
        errors = check_all(project_dir, verbose=args.verbose, timeout=args.timeout)
    else:
        parser.print_help()
        return 0

    if errors:
        print(f"\nDoxygen comment issues found ({len(errors)}):\n")
        for e in errors:
            print(f"  ✗ {e}")
        print(f"\n{'=' * 60}")
        print(f" {len(errors)} Doxygen issue(s) found")
        print(f"{'=' * 60}")
        print(
            "\nAll public lv_* functions in .h files must have Doxygen comments:\n"
            "  /** Brief description\n"
            "   * @param name  description\n"
            "   * @return      description\n"
            "   */\n"
            "\nSee: https://docs.lvgl.io/master/contributing/pull_requests.html"
        )
        if args.fail_under > 0 and len(errors) <= args.fail_under:
            print(
                f"\n(Allowed up to {args.fail_under} errors, found {len(errors)} — PASS)"
            )
            return 0
        return 1
    else:
        print("✓ Doxygen comment check passed")
        return 0


if __name__ == "__main__":
    sys.exit(main())
