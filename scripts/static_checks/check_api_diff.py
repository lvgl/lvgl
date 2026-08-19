#!/usr/bin/env python3
"""
LVGL public API change detector.

Compares the public API of two trees and classifies what changed, so that a
source-breaking change cannot land unnoticed.

Subcommands:
  compare     Diff --root against --base; exits 1 when a change is breaking
  show        Print the normalised API description of a tree (JSON)

`compare` is what CI runs, pointing --base at a worktree of the merge base.
`show` exists for debugging and as the input a future PR-comment job would use.

Only the API *contract* is modelled: names, types, signatures, enum members and
their order, struct fields. The declaring file is deliberately absent, so moving
a declaration between headers is not reported as an API change.
"""

from __future__ import annotations

import argparse
import json
import os
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from lvgl_api import PublicApi, doxygen_available
from pr_report import write_report

API_VERSION = 1


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


# ---------------------------------------------------------------------------
# Snapshot
# ---------------------------------------------------------------------------


def build_api(repo_root: Path) -> dict:
    api = PublicApi.parse(repo_root)
    return {
        "version": API_VERSION,
        # The contract is what the header declares, not what the .c defines.
        "functions": {
            f.name: {
                "return": f.declaration[0],
                "params": [
                    {"name": p.name or "", "type": p.type} for p in f.declaration[1]
                ],
            }
            for f in sorted(api.public_functions(), key=lambda f: f.name)
        },
        "enums": {
            e.name: [{"name": m.name, "value": m.value} for m in e.members]
            for e in sorted(api.public_enums(), key=lambda e: e.name)
        },
        "structs": {
            s.name: {
                "kind": s.kind,
                "fields": [{"name": f.name, "type": f.type} for f in s.fields],
            }
            for s in sorted(api.public_structs(), key=lambda s: s.name)
        },
        "typedefs": {
            t.name: t.type for t in sorted(api.public_typedefs(), key=lambda t: t.name)
        },
    }


def dump(api: dict) -> str:
    return json.dumps(api, indent=1, sort_keys=True) + "\n"


# ---------------------------------------------------------------------------
# Diff
# ---------------------------------------------------------------------------


class Change:
    def __init__(self, kind: str, name: str, detail: str, breaking: bool):
        self.kind = kind  # function | enum | struct | typedef
        self.name = name
        self.detail = detail
        self.breaking = breaking


def _sig(entry: dict) -> str:
    params = (
        ", ".join(f"{p['type']} {p['name']}".strip() for p in entry["params"]) or "void"
    )
    return f"{entry['return']} ({params})"


def diff_functions(old: dict, new: dict) -> list[Change]:
    out = []
    for name in sorted(set(old) - set(new)):
        out.append(Change("function", name, "removed", True))
    for name in sorted(set(new) - set(old)):
        out.append(Change("function", name, "added", False))
    for name in sorted(set(old) & set(new)):
        o, n = old[name], new[name]
        if o["return"] != n["return"]:
            out.append(
                Change(
                    "function",
                    name,
                    f"return type {o['return']} -> {n['return']}",
                    True,
                )
            )
        if len(o["params"]) != len(n["params"]):
            out.append(
                Change(
                    "function",
                    name,
                    f"parameter count {len(o['params'])} -> {len(n['params'])}\n"
                    f"      was: {_sig(o)}\n      now: {_sig(n)}",
                    True,
                )
            )
            continue
        for i, (po, pn) in enumerate(zip(o["params"], n["params"])):
            if po["type"] != pn["type"]:
                out.append(
                    Change(
                        "function",
                        name,
                        f"parameter {i + 1} type " f"{po['type']} -> {pn['type']}",
                        True,
                    )
                )
            elif po["name"] != pn["name"]:
                out.append(
                    Change(
                        "function",
                        name,
                        f"parameter {i + 1} renamed " f"{po['name']} -> {pn['name']}",
                        False,
                    )
                )
    return out


def diff_enums(old: dict, new: dict) -> list[Change]:
    out = []
    for name in sorted(set(old) - set(new)):
        out.append(Change("enum", name, "removed", True))
    for name in sorted(set(new) - set(old)):
        out.append(Change("enum", name, "added", False))
    for name in sorted(set(old) & set(new)):
        o = [(m["name"], m["value"]) for m in old[name]]
        n = [(m["name"], m["value"]) for m in new[name]]
        if o == n:
            continue
        onames = [m[0] for m in o]
        nnames = [m[0] for m in n]
        for gone in [x for x in onames if x not in nnames]:
            out.append(Change("enum", name, f"member removed: {gone}", True))
        for added in [x for x in nnames if x not in onames]:
            # appending is source-compatible; inserting renumbers what follows
            at_end = nnames.index(added) >= len(onames)
            out.append(
                Change(
                    "enum",
                    name,
                    f"member added: {added}"
                    + ("" if at_end else " (inserted, renumbers later members)"),
                    not at_end,
                )
            )
        for nm, val in n:
            for onm, oval in o:
                if nm == onm and val != oval:
                    out.append(
                        Change(
                            "enum",
                            name,
                            f"{nm} value {oval or '<implicit>'} -> "
                            f"{val or '<implicit>'}",
                            True,
                        )
                    )
        # order change with the same membership
        if sorted(onames) == sorted(nnames) and onames != nnames:
            out.append(Change("enum", name, "members reordered", True))
    return out


def diff_structs(old: dict, new: dict) -> list[Change]:
    out = []
    for name in sorted(set(old) - set(new)):
        out.append(Change("struct", name, "removed", True))
    for name in sorted(set(new) - set(old)):
        out.append(Change("struct", name, "added", False))
    for name in sorted(set(old) & set(new)):
        o, n = old[name], new[name]
        if o["kind"] != n["kind"]:
            out.append(Change("struct", name, f"{o['kind']} -> {n['kind']}", True))
        of = [(f["name"], f["type"]) for f in o["fields"]]
        nf = [(f["name"], f["type"]) for f in n["fields"]]
        if of == nf:
            continue
        onames = [f[0] for f in of]
        nnames = [f[0] for f in nf]
        for gone in [x for x in onames if x not in nnames]:
            out.append(Change("struct", name, f"field removed: {gone}", True))
        for added in [x for x in nnames if x not in onames]:
            out.append(Change("struct", name, f"field added: {added}", False))
        for fn, ft in nf:
            for ofn, oft in of:
                if fn == ofn and ft != oft:
                    out.append(
                        Change("struct", name, f"field {fn} type {oft} -> {ft}", True)
                    )
        if sorted(onames) == sorted(nnames) and onames != nnames:
            out.append(Change("struct", name, "fields reordered", True))
    return out


def diff_typedefs(old: dict, new: dict) -> list[Change]:
    out = []
    for name in sorted(set(old) - set(new)):
        out.append(Change("typedef", name, "removed", True))
    for name in sorted(set(new) - set(old)):
        out.append(Change("typedef", name, "added", False))
    for name in sorted(set(old) & set(new)):
        if old[name] != new[name]:
            out.append(Change("typedef", name, f"{old[name]} -> {new[name]}", True))
    return out


def diff(old: dict, new: dict) -> list[Change]:
    return (
        diff_functions(old.get("functions", {}), new.get("functions", {}))
        + diff_enums(old.get("enums", {}), new.get("enums", {}))
        + diff_structs(old.get("structs", {}), new.get("structs", {}))
        + diff_typedefs(old.get("typedefs", {}), new.get("typedefs", {}))
    )


# ---------------------------------------------------------------------------
# Checks
# ---------------------------------------------------------------------------


def report_changes(changes: list[Change]) -> None:
    breaking = [c for c in changes if c.breaking]
    compatible = [c for c in changes if not c.breaking]

    if breaking:
        print(f"\n{C.RED}{C.BOLD}Breaking changes ({len(breaking)}){C.RESET}")
        for c in breaking:
            print(f"  {C.RED}{c.kind} {C.BOLD}{c.name}{C.RESET}: {c.detail}")
    if compatible:
        print(f"\n{C.YELLOW}{C.BOLD}Compatible changes ({len(compatible)}){C.RESET}")
        for c in compatible:
            print(f"  {C.CYAN}{c.kind} {C.BOLD}{c.name}{C.RESET}: {c.detail}")


def _emit_report(changes: list[Change], path: Path) -> None:
    """Write the PR-comment section for this run."""
    sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "pr_report"))

    breaking = [c for c in changes if c.breaking]
    compatible = [c for c in changes if not c.breaking]

    if not changes:
        icon, summary = "ok", "no change"
    elif breaking:
        # deliberately not red: a breaking change can be intended
        icon = "warn"
        summary = f"{len(breaking)} breaking change" + (
            "s" if len(breaking) > 1 else ""
        )
        if compatible:
            summary += f", {len(compatible)} compatible"
    else:
        icon = "ok"
        added = len([c for c in compatible if c.detail == "added"])
        summary = (
            f"+{added} addition" + ("s" if added != 1 else "")
            if added
            else f"{len(compatible)} compatible change(s)"
        )

    lines = []
    if breaking:
        lines += ["**Breaking**", ""]
        lines += [
            f"- `{c.name}` ({c.kind}): {c.detail.splitlines()[0]}" for c in breaking
        ]
        lines += [""]
    if compatible:
        lines += ["**Compatible**", ""]
        lines += [
            f"- `{c.name}` ({c.kind}): {c.detail.splitlines()[0]}" for c in compatible
        ]

    write_report(
        path, section="API", icon=icon, summary=summary, details="\n".join(lines)
    )


def cmd_compare(
    repo_root: Path, base_root: Path | None, report: Path | None = None
) -> bool:
    if base_root is None:
        print(
            f"{C.RED}{C.BOLD}ERROR:{C.RESET} compare needs --base "
            "pointing at the tree to compare against"
        )
        return False

    new = build_api(repo_root)
    old = build_api(base_root)
    changes = diff(old, new)

    if report is not None:
        _emit_report(changes, report)

    if not changes:
        print(f"{C.GREEN}{C.BOLD}OK:{C.RESET} the public API is unchanged")
        return True

    report_changes(changes)
    breaking = [c for c in changes if c.breaking]
    print()
    if not breaking:
        print(
            f"{C.GREEN}{C.BOLD}OK:{C.RESET} the public API grew, "
            "nothing existing changed"
        )
        return True

    print(f"{C.BOLD}{len(breaking)} breaking change(s) to the public API.{C.RESET}")
    print(
        "If this is intended, say so in the pull request description so that "
        "it can be released as such."
    )
    return False


def cmd_show(
    repo_root: Path, base_root: Path | None, report: Path | None = None
) -> bool:
    sys.stdout.write(dump(build_api(repo_root)))
    return True


CHECKS = {
    "compare": cmd_compare,
    "show": cmd_show,
}


def main() -> None:
    parser = argparse.ArgumentParser(
        description="LVGL public API change detector",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="Subcommands:\n" + "\n".join(f"  {k}" for k in CHECKS),
    )
    parser.add_argument("check", nargs="?", default="compare", choices=list(CHECKS))
    parser.add_argument("--root", default=".", help="Repository root")
    parser.add_argument(
        "--base",
        default=None,
        help="Root of the tree to compare against (a worktree of the merge base)",
    )
    parser.add_argument(
        "--report",
        default=None,
        help="Also write the PR-comment section for this result to this path",
    )
    args = parser.parse_args()

    if not doxygen_available():
        print(
            "ERROR: doxygen is required by this check (apt install doxygen)",
            file=sys.stderr,
        )
        sys.exit(2)
    base = Path(args.base).resolve() if args.base else None
    rep = Path(args.report) if args.report else None
    sys.exit(0 if CHECKS[args.check](Path(args.root).resolve(), base, rep) else 1)


if __name__ == "__main__":
    main()
