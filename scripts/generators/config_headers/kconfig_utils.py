"""Small helpers over kconfiglib expressions and C rendering.

Kept dependency-free of our own model so both the parse and emit layers can use
them without import cycles.
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
    """AND-split a dependency expression into its individual terms."""
    if expr is None:
        return []
    return [t for t in split_expr(expr, AND) if t is not None]


def term_key(term) -> str:
    """Stable identity for a dependency term, so terms can be compared/diffed."""
    return expr_str(term)


def bool_default(sym) -> str:
    """Return ``"1"`` / ``"0"`` for a bool/tristate symbol's *own* default.

    The symbol's ``depends on`` / enclosing ``if`` is AND-folded by kconfiglib
    into the condition of every default; we strip those terms back out so an
    option inside ``if LV_USE_X`` still reports the value it would take when
    that feature is on (not the dependency-suppressed 0).  Conditional defaults
    (``default y if C``) are still evaluated against the current config.
    """
    assert sym.type in (BOOL, TRISTATE)
    dd_keys = {term_key(x) for x in dep_terms(sym.direct_dep)}
    for value, cond in sym.defaults:
        rest = [x for x in dep_terms(cond) if term_key(x) not in dd_keys]
        if all(expr_value(x) > 0 for x in rest):
            return "1" if expr_value(value) > 0 else "0"
    return "0"


def resolve_int_value(sym) -> str:
    """Resolve an int/hex symbol to its concrete C value against the current
    config (with the symbol's own direct deps stripped).

    Unlike emitting a token *name*, this evaluates each ``default ... if <cond>``
    in order and returns the first whose condition holds.  When none holds - a
    gated-off computed count such as ``LV_LINUX_FBDEV_BUFFER_COUNT`` while the
    driver is disabled - it returns ``0``, matching the default snapshot.
    References to another int/hex *config* (``LV_SDL_BUF_COUNT`` ->
    ``LV_SDL_BUFFER_COUNT``) are chased through."""
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
    """Return the C literal for an int/hex/string symbol's *own* default.

    Int/hex go through :func:`resolve_int_value` (gating-aware).  Strings keep
    their quotes (``expr_str`` includes them) and strip the symbol's own deps so
    a gated option still reports the value it takes when its feature is on.
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
    """The documentation string for a node: its help, else its prompt."""
    if node.help:
        return node.help
    if node.prompt:
        return node.prompt[0]
    return ""


def is_int_const(sym) -> bool:
    """True if *sym* is a named enum-token constant: a no-prompt int/hex config
    with a single unconditional literal default (e.g. ``LV_STDLIB_BUILTIN`` ->
    ``0``).  When an enum's derived int defaults to one of these, we emit the
    token *name* rather than its number."""
    if sym.type not in (INT, HEX):
        return False
    if any(node.prompt for node in sym.nodes):
        return False
    if len(sym.defaults) != 1:
        return False
    value, cond = sym.defaults[0]
    # A numeric literal default is modelled as an undefined symbol whose name is
    # the literal text, not as an ``is_constant`` symbol.
    if not (isinstance(value, Symbol) and not value.nodes):
        return False
    try:
        int(value.name, 0)
    except (ValueError, TypeError):
        return False
    return cond is sym.kconfig.y


def int_const_value(sym) -> str:
    """The literal value of an :func:`is_int_const` symbol, as text."""
    return sym.defaults[0][0].name


def choice_default(choice):
    """The choice's selected member, or its default member when inactive (so a
    gated choice still reports a sensible selection rather than nothing)."""
    if choice.selection:
        return choice.selection
    dd_keys = {term_key(x) for x in dep_terms(getattr(choice, "direct_dep", None))}
    for sym, cond in getattr(choice, "defaults", []):
        rest = [x for x in dep_terms(cond) if term_key(x) not in dd_keys]
        if all(expr_value(x) > 0 for x in rest):
            return sym
    return choice.syms[0] if choice.syms else None


def member_description(member) -> str:
    """A choice member's human description: its prompt with any leading
    ``"<n>: "`` ordinal stripped (e.g. ``"1: PTHREAD"`` -> ``"PTHREAD"``)."""
    if not (member.nodes and member.nodes[0].prompt):
        return ""
    return re.sub(r"^\s*\d+\s*:\s*", "", member.nodes[0].prompt[0]).strip()


def _expr_has_choice(expr) -> bool:
    """True if a Kconfig expression mentions a ``Choice`` (renders as the
    non-C-expressible ``<choice>`` token, e.g. a member's implicit dep)."""
    if isinstance(expr, Choice):
        return True
    if isinstance(expr, tuple):
        return any(_expr_has_choice(x) for x in expr[1:])
    return False


def rev_dep_expr(sym) -> str | None:
    """The C condition for what ``select``s *sym* (its reverse dependencies),
    or ``None`` if nothing selects it or the expression isn't C-expressible
    (mentions a choice).  ``&&`` / ``||`` / ``!`` are identical in Kconfig and
    C, so ``expr_str`` is directly usable."""
    if sym.rev_dep is sym.kconfig.n:
        return None
    if _expr_has_choice(sym.rev_dep):
        return None
    return expr_str(sym.rev_dep)


def _is_user_facing(sym) -> bool:
    """True if *sym* has a prompt on some node, i.e. it is an option the user can
    set.  No-prompt symbols (internal capability flags like
    ``LV_DRAW_HAS_VECTOR_SUPPORT``) are derived, never set by hand."""
    return bool(getattr(sym, "nodes", None)) and any(n.prompt for n in sym.nodes)


def select_lines(item) -> str:
    """A comment suffix documenting what *item* pulls in via Kconfig ``select``.

    Returns ``"\\n\\nEnable: <targets>"`` listing the *user-facing* options this
    one auto-enables, or ``""`` if there are none.  Surfaced in
    ``lv_conf_template.h`` so a hand-written ``lv_conf.h`` knows to enable them
    too.  ``depends on`` is intentionally omitted: the template already wraps
    each option in the matching ``#if`` block, so its dependencies are visible
    from the nesting.  Hidden (no-prompt) select targets are dropped - the user
    can't enable an internal flag by hand."""
    if getattr(item, "kconfig", None) is None:
        return ""
    targets = [
        expr_str(target)
        for target, _cond in (getattr(item, "selects", None) or [])
        if _is_user_facing(target)
    ]
    if not targets:
        return ""
    return "\n\nEnable: " + ", ".join(targets)


def collect_sym_refs(expr, acc: set) -> None:
    """Collect every ``Symbol`` object referenced anywhere in *expr*."""
    if isinstance(expr, Symbol):
        acc.add(expr)
    elif isinstance(expr, tuple):
        for sub in expr[1:]:
            collect_sym_refs(sub, acc)


def _render_bool_expr(expr, guard):
    """Render a Kconfig bool expression to ``(c_text, precedence)`` or ``None``.

    Precedence is ``3`` for an atom, ``2`` for ``&&``, ``1`` for ``||`` - used
    to parenthesize only where C semantics (or readability) require it.

    * a choice *member* listed in *guard* (member -> ``(macro, token)``) renders
      as ``<macro> == <token>``, valid on both the Kconfig and lv_conf.h paths;
    * a bare ``Choice`` term (a member's implicit "this choice is active" dep)
      renders to nothing and is dropped from its enclosing ``&&`` / ``||``;
    * ``&&`` / ``||`` / ``!`` map directly to C; comparisons fall back to
      ``expr_str`` (already C-compatible).
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
    """Render a reverse-dependency expression as a C condition.

    Like :func:`rev_dep_expr` but choice-aware: bare ``Choice`` terms (a
    member's implicit "this choice is active" dep, e.g. the anonymous Wayland
    backend choice) are dropped instead of making the whole expression
    inexpressible.  A named-choice member can be rewritten via *guard* into
    ``<macro> == <token>``, but callers must only do so when the macro is
    integer-valued (it is not for the font-default choice - see
    :func:`config_headers.emit.select_guards`).  Returns ``None`` only if the
    whole expression renders to nothing."""
    rendered = _render_bool_expr(expr, guard or {})
    return rendered[0] if rendered else None


def c_comment(text: str) -> list[str]:
    """Render *text* (possibly multi-line) as a Doxygen-style block comment."""
    lines = [ln.rstrip() for ln in text.strip("\n").split("\n")]
    if not lines or lines == [""]:
        return []
    if len(lines) == 1:
        return [f"/** {lines[0]} */"]
    out = [f"/** {lines[0]}"]
    out += [f" *  {ln}" if ln else " *" for ln in lines[1:]]
    out += [" */"]
    return out
