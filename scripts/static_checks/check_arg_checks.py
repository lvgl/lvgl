"""
LVGL public API argument-check auditor.
"""

from __future__ import annotations

import argparse
import json as jsonlib
import os
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from lvgl_api import Function, PublicApi, doxygen_available

# ---------------------------------------------------------------------------
# Colors
# ---------------------------------------------------------------------------


def _colors_enabled() -> bool:
    if os.environ.get("NO_COLOR"):
        return False
    return sys.stdout.isatty() or os.environ.get("FORCE_COLOR") == "1"


class C:
    _on = _colors_enabled()
    RED = "\033[31m" if _on else ""
    YELLOW = "\033[33m" if _on else ""
    GREEN = "\033[32m" if _on else ""
    CYAN = "\033[36m" if _on else ""
    DIM = "\033[2m" if _on else ""
    BOLD = "\033[1m" if _on else ""
    RESET = "\033[0m" if _on else ""


def log_ok(msg: str) -> None:
    print(f"{C.GREEN}{C.BOLD}OK:{C.RESET} {msg}")


# ---------------------------------------------------------------------------
# Policy
# ---------------------------------------------------------------------------

# Headers that are not part of the checkable public API.
SKIPPED_HEADER_DIRS = (
    "include/lvgl/config/",  # generated configuration, no functions
    "include/lvgl/api_map/",  # v8/v9 compatibility aliases
    "include/lvgl/stdlib/",  # for the stdlib LVGL follows lib C so it's either plain if or ASSERT when UB is defined
)

NEVER_CHECK_PARAM_NAMES = frozenset({"user_data"})

# Doxygen marker that declares an argument may legitimately be NULL:
#     @param style  pointer to a style. @nullable NULL notifies every object.
# Define it in docs/Doxyfile so it renders, e.g.
#     ALIASES += nullable="@b Optional: may be `NULL`."
NULLABLE_MARKER = "@nullable"

CHECK_MACROS = (
    "LV_CHECK_ARG",
    "LV_CHECK_OBJ",
)


def _validation_patterns(param: str) -> list[re.Pattern]:
    p = re.escape(param)
    pats = [
        # LV_CHECK_ARG(p != NULL, ...) / LV_CHECK_OBJ(p, cls, ...)
        re.compile(r"\b(?:" + "|".join(CHECK_MACROS) + r")\s*\(\s*&?\s*" + p + r"\b"),
        # a compound check that mentions the parameter, e.g.
        # LV_CHECK_ARG(len == 0 || dst != NULL, ...)
        re.compile(r"\bLV_CHECK_ARG\s*\([^;]*?\b" + p + r"\s*(?:==|!=)\s*NULL"),
        re.compile(r"\bLV_CHECK_ARG\s*\([^;]*?NULL\s*(?:==|!=)\s*" + p + r"\b"),
    ]
    return pats


def is_validated(body: str, param: str) -> bool:
    patterns = _validation_patterns(param)
    return any(pat.search(body) for pat in patterns)


def _checkable_params(func: Function) -> list:
    """Pointer parameters of `func` that are required to be validated."""
    out = []
    for _, param in enumerate(func.params):
        if not param.is_pointer_like or not param.name:
            continue
        if param.name in NEVER_CHECK_PARAM_NAMES:
            continue
        if NULLABLE_MARKER in (param.doc or ""):
            continue
        out.append(param)
    return out


# ---------------------------------------------------------------------------
# Audit
# ---------------------------------------------------------------------------


def collect(repo_root: Path, xml_dir: Path | None = None) -> dict:
    api = PublicApi.parse(repo_root, xml_dir=xml_dir)

    missing = []
    no_impl = 0
    checked = 0
    considered = 0

    for func in sorted(api.public_functions(), key=lambda f: f.name):
        if func.header and any(func.header.startswith(d) for d in SKIPPED_HEADER_DIRS):
            continue
        if not func.has_body:
            if any(p.is_pointer_like for p in func.params):
                no_impl += 1
            continue

        body = func.body(repo_root)
        for param in _checkable_params(func):
            considered += 1

            if is_validated(body, param.name):
                checked += 1
            else:
                missing.append(
                    {
                        "function": func.name,
                        "param": param.name,
                        "type": param.type,
                        "header": func.header,
                        "header_line": func.header_line,
                        "impl": func.body_file,
                        "impl_line": func.body_start,
                        "null_mentioned_in_docs": "NULL" in param.doc,
                    }
                )

    return {
        "total_public_functions": len(api.public_functions()),
        "missing_but_documented_nullable_in_prose": sum(
            1 for m in missing if m["null_mentioned_in_docs"]
        ),
        "pointer_params_considered": considered,
        "pointer_params_validated": checked,
        "pointer_params_missing": len(missing),
        "functions_without_implementation": no_impl,
        "missing": missing,
    }


def check_audit(repo_root: Path, xml_dir: Path | None = None) -> bool:
    report = collect(repo_root, xml_dir)
    missing = report["missing"]

    by_file: dict[str, list[dict]] = {}
    for item in missing:
        by_file.setdefault(item["impl"], []).append(item)

    for path in sorted(by_file):
        print(f"\n{C.BOLD}{path}{C.RESET}")
        for item in sorted(by_file[path], key=lambda i: (i["function"], i["param"])):
            print(
                f"  {C.YELLOW}{item['function']}{C.RESET}"
                f"({C.CYAN}{item['param']}{C.RESET})\n"
                f"    {C.DIM}{item['header']}:{item['header_line']}{C.RESET}\n"
                f"    {C.DIM}{item['impl']}:{item['impl_line']}{C.RESET}"
            )

    print()
    print(f"{C.BOLD}Public API argument checks{C.RESET}")
    print(f"  public functions            : {report['total_public_functions']}")
    print(f"  pointer parameters          : {report['pointer_params_considered']}")
    print(
        f"  validated                   : {C.GREEN}{report['pointer_params_validated']}{C.RESET}"
    )
    print(
        f"  {C.BOLD}unvalidated{C.RESET}                 : "
        f"{C.RED if missing else C.GREEN}{report['pointer_params_missing']}{C.RESET}"
    )
    if report["missing_but_documented_nullable_in_prose"]:
        print(
            f"  {C.DIM}of those, whose docs already mention NULL: "
            f"{report['missing_but_documented_nullable_in_prose']}"
            f" -- mark these `{NULLABLE_MARKER}`{C.RESET}"
        )
    if report["functions_without_implementation"]:
        print(
            f"  {C.DIM}no implementation in tree   : "
            f"{report['functions_without_implementation']} (not counted){C.RESET}"
        )
    return True


def check_count(repo_root: Path, xml_dir: Path | None = None) -> bool:
    print(collect(repo_root, xml_dir)["pointer_params_missing"])
    return True


def check_json(repo_root: Path, xml_dir: Path | None = None) -> bool:
    print(jsonlib.dumps(collect(repo_root, xml_dir), indent=2))
    return True


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------

CHECKS = {
    "audit": check_audit,
    "count": check_count,
    "json": check_json,
}


def main() -> None:
    parser = argparse.ArgumentParser(
        description="LVGL public API argument-check auditor",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="Subcommands:\n" + "\n".join(f"  {k}" for k in CHECKS),
    )
    parser.add_argument("check", nargs="?", default="audit", choices=list(CHECKS))
    parser.add_argument(
        "--root",
        default=".",
        help="Repository root (default: current directory)",
    )
    parser.add_argument(
        "--xml",
        help="Output repository from a previous run",
    )
    args = parser.parse_args()

    repo_root = Path(args.root).resolve()
    xml_dir = args.xml

    if not xml_dir and not doxygen_available():
        print(
            "ERROR: doxygen is required by this check",
            file=sys.stderr,
        )
        sys.exit(2)

    passed = CHECKS[args.check](repo_root, xml_dir)
    sys.exit(0 if passed else 1)


if __name__ == "__main__":
    main()
