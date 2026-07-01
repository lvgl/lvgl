"""Small helpers for reading kconfiglib options and turning them into C text.

Kconfig stores each option as an object carrying dependencies, defaults, and
expressions.  These helpers pull out the useful bits (an option's default
value, what it depends on, what it pulls in) and render them as the text that
goes into the generated config headers.

No imports from our own model, so both the parse and emit layers can use them
without import cycles.
"""

from __future__ import annotations

import re

from kconfiglib import (
    AND,
    BOOL,
    HEX,
    INT,
    NOT,
    OR,
    TRISTATE,
    Choice,
    Symbol,
    expr_str,
    expr_value,
    split_expr,
)


def dep_terms(expr) -> list:
    """Flat list of the individual terms in an ``A && B && C`` dependency
    (here ``[A, B, C]``).  ``[]`` for no expression."""
    if expr is None:
        return []
    return [t for t in split_expr(expr, AND) if t is not None]


def term_key(term) -> str:
    """A string id for one dependency term, used to compare two terms for
    equality."""
    return expr_str(term)


def bool_default(sym) -> str:
    """An on/off option's own default, as ``"1"`` or ``"0"``.

    Kconfig secretly glues an option's ``depends on`` onto every one of its
    defaults, so an option under ``if LV_USE_X`` would read as 0 whenever
    ``LV_USE_X`` is off.  We strip that glued-on dependency back off to report
    the value the option takes when its feature is on.  Other conditions on a
    default (``default y if C``) are still checked against the current config.
    """
    assert sym.type in (BOOL, TRISTATE)
    dd_keys = {term_key(x) for x in dep_terms(sym.direct_dep)}
    for value, cond in sym.defaults:
        rest = [x for x in dep_terms(cond) if term_key(x) not in dd_keys]
        if all(expr_value(x) > 0 for x in rest):
            return "1" if expr_value(value) > 0 else "0"
    return "0"


def resolve_int_value(sym) -> str:
    """An int/hex option's value as C text (e.g. ``"32"`` or ``"0x40"``).

    Checks each ``default N if <cond>`` in order (ignoring the option's own
    ``depends on``) and returns the first that applies.  A default pointing at
    another int/hex option is followed through to its value.  When nothing
    applies - e.g. a count gated off because its driver is disabled - returns
    ``0`` (``0x0`` for hex)."""
    dd_keys = {term_key(x) for x in dep_terms(sym.direct_dep)}
    for value, cond in sym.defaults:
        rest = [x for x in dep_terms(cond) if term_key(x) not in dd_keys]
        if all(expr_value(x) > 0 for x in rest):
            if isinstance(value, Symbol) and value.nodes and value.type in (INT, HEX):
                return resolve_int_value(value)
            s = expr_str(value)
            if sym.type == HEX and not s.startswith("0x"):
                s = "0x" + s
            return s
    return "0x0" if sym.type == HEX else "0"


def scalar_default(sym) -> str:
    """An int/hex/string option's own default, as a C literal.

    Numbers go through :func:`resolve_int_value`.  Strings keep their quotes
    (``"foo"``); as with the other defaults, the option's own ``depends on`` is
    stripped so a gated option reports the value it takes when its feature is
    on.
    """
    if sym.type in (INT, HEX):
        return resolve_int_value(sym)
    dd_keys = {term_key(x) for x in dep_terms(sym.direct_dep)}
    for value, cond in sym.defaults:
        rest = [x for x in dep_terms(cond) if term_key(x) not in dd_keys]
        if all(expr_value(x) > 0 for x in rest):
            return expr_str(value)
    return '""'


def doc_text(node) -> str:
    """An option's documentation text: its help, or its short prompt if there
    is no help, or ``""``."""
    if node.help:
        return node.help
    if node.prompt:
        return node.prompt[0]
    return ""


def is_int_const(sym) -> bool:
    """True if *sym* is a named token standing for a fixed number: no prompt and
    a single literal default (e.g. ``LV_STDLIB_BUILTIN`` = ``0``).  These are
    emitted as bare ``#define``s in the internal "Config options" block; when an
    enum picks one we emit its *name*, not the number.
    """
    if sym.type not in (INT, HEX):
        return False
    if any(node.prompt for node in sym.nodes):
        return False
    if len(sym.defaults) != 1:
        return False
    value, _cond = sym.defaults[0]
    # A numeric literal default is modelled as an undefined symbol whose name is
    # the literal text, not as an ``is_constant`` symbol.
    if not (isinstance(value, Symbol) and not value.nodes):
        return False
    try:
        int(value.name, 0)
    except (ValueError, TypeError):
        return False
    return True


def int_const_value(sym) -> str:
    """The number an :func:`is_int_const` token stands for, as text."""
    return sym.defaults[0][0].name


def choice_default(choice):
    """The selected member of a "pick one" group.  If the group is gated off,
    the member it would default to - never nothing."""
    if choice.selection:
        return choice.selection
    dd_keys = {term_key(x) for x in dep_terms(getattr(choice, "direct_dep", None))}
    for sym, cond in getattr(choice, "defaults", []):
        rest = [x for x in dep_terms(cond) if term_key(x) not in dd_keys]
        if all(expr_value(x) > 0 for x in rest):
            return sym
    return choice.syms[0] if choice.syms else None


def member_description(member) -> str:
    """A choice member's human name: its prompt with any leading ``"<n>: "``
    ordinal removed (``"1: PTHREAD"`` -> ``"PTHREAD"``)."""
    if not (member.nodes and member.nodes[0].prompt):
        return ""
    return re.sub(r"^\s*\d+\s*:\s*", "", member.nodes[0].prompt[0]).strip()


def _is_user_facing(sym) -> bool:
    """True if a user can set *sym* by hand (it has a prompt).  No-prompt
    options - internal flags like ``LV_DRAW_HAS_VECTOR_SUPPORT`` - are derived,
    never set directly."""
    return bool(getattr(sym, "nodes", None)) and any(n.prompt for n in sym.nodes)


def select_targets(item) -> list[str]:
    """The user-settable options *item* auto-enables via ``select`` (by name).

    Internal (no-prompt) targets are skipped - they can't be enabled by hand, so
    documenting them would only confuse."""
    if getattr(item, "kconfig", None) is None:
        return []
    return [
        expr_str(target)
        for target, _cond in (getattr(item, "selects", None) or [])
        if _is_user_facing(target)
    ]


def select_lines(item) -> str:
    """A comment listing the options *item* auto-enables (via ``select``).

    Returns ``"\\n\\nEnable: <targets>"`` naming the user-settable options this
    one turns on, or ``""`` if none.  Shown in ``lv_conf_template.h`` so a
    hand-written ``lv_conf.h`` knows to enable them too.  Dependencies
    (``depends on``) are left out - the template already nests each option in
    its matching ``#if``, so they're visible from the structure."""
    targets = select_targets(item)
    return "\n\nEnable: " + ", ".join(targets) if targets else ""


def member_requires(member, choice) -> str | None:
    """A choice *member*'s own ``depends on`` as a C condition, or ``None``.

    The choice's ambient dependencies (its enclosing ``if`` / ``depends on``,
    e.g. ``LV_USE_SDL``) and the implicit "this choice is active" term are
    stripped, leaving just what the member itself requires (e.g. the SDL EGL
    backend's ``LV_USE_DRAW_OPENGLES || LV_USE_DRAW_NANOVG``).
    """
    ambient = {term_key(x) for x in dep_terms(getattr(choice, "direct_dep", None))}
    rest = [
        x
        for x in dep_terms(getattr(member, "direct_dep", None))
        if term_key(x) not in ambient and not isinstance(x, Choice)
    ]
    if not rest:
        return None
    expr = rest[0]
    for term in rest[1:]:
        expr = (AND, expr, term)
    return rev_dep_c_expr(expr)


def collect_sym_refs(expr, acc: set) -> None:
    """Add every option referenced anywhere in *expr* to the set *acc*."""
    if isinstance(expr, Symbol):
        acc.add(expr)
    elif isinstance(expr, tuple):
        for sub in expr[1:]:
            collect_sym_refs(sub, acc)


def _render_bool_expr(expr, guard):
    """Turn a Kconfig bool expression into C text.

    Returns ``(c_text, precedence)``, or ``None`` if it renders to nothing.
    Precedence (atom ``3``, ``&&`` ``2``, ``||`` ``1``) tells the caller where
    parentheses are needed to keep the C meaning correct.

    * ``&&`` / ``||`` / ``!`` map straight to C; comparisons (``==``, ``<``, ...)
      already render as valid C;
    * a "pick one" member listed in *guard* (member -> ``(macro, token)``)
      becomes ``<macro> == <token>``, which works on both config paths;
    * a bare group term (a member's implicit "this group is active" dependency)
      renders to nothing and drops out of its enclosing ``&&`` / ``||``.
    """
    if isinstance(expr, Choice):
        return None
    if isinstance(expr, Symbol):
        kconf = expr.kconfig
        if expr is kconf.y:
            return ("1", 3)
        if expr is kconf.n:
            return ("0", 3)
        if expr.name in guard:
            macro, token = guard[expr.name]
            return (f"{macro} == {token}", 3)
        if expr.choice is not None and expr.choice.name:
            return (f"{expr.choice.name} == {expr.name}", 3)
        return (expr.name, 3)
    op = expr[0]
    if op == NOT:
        inner = _render_bool_expr(expr[1], guard)
        if inner is None:
            return None
        text, prec = inner
        if prec < 3 or " " in text:
            text = f"({text})"
        return (f"!{text}", 3)
    if op in (AND, OR):
        left = _render_bool_expr(expr[1], guard)
        right = _render_bool_expr(expr[2], guard)
        # A dropped Choice operand leaves the other side standing alone.
        if left is None:
            return right
        if right is None:
            return left
        myprec = 2 if op == AND else 1
        sep = "&&" if op == AND else "||"

        def wrap(piece):
            text, prec = piece
            # parens for correctness, plus &&-groups inside || for clarity
            if prec < myprec or (op == OR and prec == 2):
                return f"({text})"
            return text

        return (f"{wrap(left)} {sep} {wrap(right)}", myprec)
    # comparisons (==, <, ...): kconfiglib already renders C-compatible text.
    return (expr_str(expr), 3)


def rev_dep_c_expr(expr, guard=None) -> str | None:
    """Render any bool expression as a C condition (the workhorse wrapper).

    When the expression mentions a "pick one" group it drops the bare group
    term (e.g. the anonymous Wayland backend choice) and keeps the rest, rather
    than giving up.  A named group member can be rewritten into
    ``<macro> == <token>`` via *guard* - but only when that macro is
    integer-valued (not the font-default choice; see
    :func:`config_headers.emit.select_guards`).  ``None`` only if the whole
    thing renders to nothing."""
    rendered = _render_bool_expr(expr, guard or {})
    return rendered[0] if rendered else None


def c_comment(text: str) -> list[str]:
    """Wrap *text* (one or more lines) in a Doxygen ``/** ... */`` block."""
    lines = [ln.rstrip() for ln in text.strip("\n").split("\n")]
    if not lines or lines == [""]:
        return []
    if len(lines) == 1:
        return [f"/** {lines[0]} */"]
    out = [f"/** {lines[0]}"]
    out += [f" *  {ln}" if ln else " *" for ln in lines[1:]]
    out += [" */"]
    return out
