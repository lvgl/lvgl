"""Phase 2: assemble whole header files from the typed model.

The :class:`Emitter` walks the Kconfig tree, tracking the top-level "group"
menus (for banners) and the stack of open ``#if`` conditions (for dependency
nesting).  The actual ``#define`` text for each node comes from its typed
:class:`~config_headers.config_entry.ConfigEntry` (``emit_template`` /
``emit_internal``); this module only handles *structure*: banners, ``#if``
blocks, the constant "Config options" block, and the static preamble/footer.
"""

from __future__ import annotations

import re

from kconfiglib import COMMENT, MENU, NOT, Choice, Kconfig, Symbol

from . import templates
from .config_entry import (
    BoolConfig,
    ConstraintCheck,
    ConstToken,
    DerivedFlag,
    EnumChoice,
)
from .kconfig_utils import (
    bool_default,
    collect_sym_refs,
    dep_terms,
    rev_dep_c_expr,
    term_key,
)
from .parse import classify, enum_backed_choices


class Emitter:
    """Renders one target (``"template"``, ``"internal"``) of the tree."""

    def __init__(self, kconf: Kconfig, target: str, entries):
        assert target in ("internal", "template")
        self.kconf = kconf
        self.target = target
        self.enum_choices = enum_backed_choices(kconf)
        self.out: list[str] = []
        self.cond_stack: list[str] = []
        self.emitted: set[str] = set()
        self.deferred: list[DerivedFlag] = []  # internal: emitted after the body
        # member symbol -> (macro, token): rewrites a `#if <member>` guard (only
        # valid on the Kconfig path) into `<macro> == <token>` (valid on both).
        self.guard: dict[str, tuple[str, str]] = enum_guard_map(entries)

    # -- conditional blocks -------------------------------------------------

    def _render_cond(self, term) -> str:
        if isinstance(term, tuple) and term[0] == NOT and isinstance(term[1], Symbol):
            sym = term[1]
            if sym.name in self.guard:
                macro, token = self.guard[sym.name]
                return f"{macro} != {token}"
            if sym.choice is not None and sym.choice.name:
                return f"{sym.choice.name} != {sym.name}"
            return f"!{sym.name}"
        if isinstance(term, Symbol):
            if term.name in self.guard:
                macro, token = self.guard[term.name]
                return f"{macro} == {token}"
            if term.choice is not None and term.choice.name:
                return f"{term.choice.name} == {term.name}"
            return term.name
        return term_key(term)

    def _sync_conditions(self, wanted: list, base_keys: set[str]):
        # Drop terms that aren't valid/sensible C guards: a Choice (a member's
        # implicit dep, renders as `<choice>`) and the derived-combo symbols
        # that would gate their own selector members circularly.
        wanted = [t for t in wanted if not isinstance(t, Choice)]
        deduped, seen = [], set(base_keys)
        for t in wanted:
            k = term_key(t)
            if k not in seen:
                seen.add(k)
                deduped.append(t)
        wanted_conds = [self._render_cond(t) for t in deduped]

        popped = False
        while self.cond_stack and self.cond_stack[-1] not in wanted_conds:
            self.out.append(f"#endif /*{self.cond_stack.pop()}*/")
            popped = True
        if popped:
            self.out.append("")
        for cond in wanted_conds:
            if cond not in self.cond_stack:
                self.out.append(f"#if {cond}")
                self.cond_stack.append(cond)

    def _close_all_conditions(self):
        while self.cond_stack:
            self.out.append(f"#endif /*{self.cond_stack.pop()}*/")
        self.out.append("")

    # -- walk ---------------------------------------------------------------

    def _emit_entry(self, node, entry, base_keys: set[str]):
        # Derived flags reference operands defined later in the file, so in the
        # internal header they are deferred to the footer, not emitted inline.
        if isinstance(entry, DerivedFlag):
            if self.target == "internal":
                self.deferred.append(entry)
            return
        lines = (
            entry.emit_template()
            if self.target == "template"
            else entry.emit_internal()
        )
        if not lines:
            return
        if self.target != "internal":
            self._sync_conditions(dep_terms(node.dep), base_keys)
        self.out += lines
        self.out.append("")

    def walk(self, node, base_keys: set[str]):
        while node:
            item = node.item
            if item is MENU or item is COMMENT:
                if node.list:
                    self.walk(node.list, base_keys)
            else:
                entry = classify(node, self.enum_choices)
                if entry is not None and entry.name not in self.emitted:
                    self.emitted.add(entry.name)
                    self._emit_entry(node, entry, base_keys)
                if node.list:
                    self.walk(node.list, base_keys)
            node = node.next

    def emit_group(self, menu_node):
        title = menu_node.prompt[0] if menu_node.prompt else ""
        base_keys = {term_key(t) for t in dep_terms(menu_node.dep)}
        banner = "/*" + "=" * 76 + "\n" f" * {title.upper()}\n" " *" + "=" * 76 + "*/"
        self.out.append(banner)
        self.out.append("")
        if menu_node.list:
            self.walk(menu_node.list, base_keys)
        self._close_all_conditions()
        self.out.append("")


def collect_groups(kconf: Kconfig) -> list:
    """Top-level menu nodes under the root, in Kconfig order."""
    root = kconf.top_node.list
    if root and root.item is MENU and root.next is None:
        root = root.list  # descend into a single outer "LVGL" menu
    groups, node = [], root
    while node:
        if node.item is MENU and node.prompt:
            groups.append(node)
        node = node.next
    return groups


# ----------------------------------------------------------------------------
# Constraint checks (#error guards for Kconfig depends/select on the lv_conf.h
# path - Kconfig already enforces these; this only catches hand-written configs)
# ----------------------------------------------------------------------------


def enum_guard_map(entries) -> dict:
    """Map each enum choice *member* symbol to ``(macro, token)`` so a bare
    ``#if <member>`` can be rewritten into ``<macro> == <token>`` - valid on
    both the Kconfig and lv_conf.h paths."""
    guard: dict[str, tuple[str, str]] = {}
    for e in entries:
        if isinstance(e, EnumChoice):
            for m in e.members:
                if m.member_name:
                    guard[m.member_name] = (e.name, m.token)
    return guard


def _refs_pointer_token(refs, guard) -> bool:
    """True if any referenced symbol is an enum member whose token is a *pointer*
    (the ``LV_FONT_DEFAULT_*`` font selectors): ``<macro> == <token>`` is then a
    pointer comparison the C preprocessor can't evaluate, so the check that needs
    it can't be expressed and must be skipped.  Integer-valued members (the
    ``LV_OS_*`` family etc.) are fine."""
    for r in refs:
        gm = guard.get(r.name)
        if gm and gm[1] in templates.BUILTIN_FONTS:
            return True
    return False


def constraint_checks(entries) -> list[ConstraintCheck]:
    """Build the ``#error`` guards that replay Kconfig ``select`` / ``depends on``
    on the hand-written ``lv_conf.h`` path.  Only :class:`BoolConfig` options are
    guarded (the constraint is "this on/off option needs ...").

    * **select**: something ``select``s the option, so enabling the selector
      requires the option on -> error when ``(selectors) && !option``.
    * **depends on**: the option needs its dependency met -> error when
      ``option && !(deps)``.  Restricted to options that *default off*: a
      default-on sub-option (e.g. ``LV_LOG_TRACE_*``) emits ``1`` even when its
      parent feature is off - because ``bool_default`` strips ``depends on`` -
      which would make the default/``LV_CONF_SKIP`` build error spuriously.  A
      default-off option emits ``0``, so its check can only fire when the user
      explicitly turns it on - exactly the case worth catching.
    """
    guard = enum_guard_map(entries)
    checks: list[ConstraintCheck] = []
    for e in entries:
        if not isinstance(e, BoolConfig) or e.node is None:
            continue
        sym = e.node.item
        kn = sym.kconfig.n

        # select: enabling a selector requires this option to be on
        if sym.rev_dep is not kn:
            refs: set = set()
            collect_sym_refs(sym.rev_dep, refs)
            sel = rev_dep_c_expr(sym.rev_dep, guard)
            if sel is not None and not _refs_pointer_token(refs, guard):
                checks.append(
                    ConstraintCheck(
                        sym.name,
                        f"({sel}) && !{sym.name}",
                        f"{sym.name} must be enabled: Kconfig selects it from "
                        f"{sel}",
                        node=e.node,
                    )
                )

        # depends on: the option may only be enabled if its dependency is met
        dd = sym.direct_dep
        if dd is not sym.kconfig.y and dd is not kn and bool_default(sym) == "0":
            refs = set()
            collect_sym_refs(dd, refs)
            dep = rev_dep_c_expr(dd, guard)
            if dep is not None and not _refs_pointer_token(refs, guard):
                checks.append(
                    ConstraintCheck(
                        sym.name,
                        f"{sym.name} && !({dep})",
                        f"{sym.name} requires {dep} (Kconfig depends on)",
                        node=e.node,
                    )
                )
    return checks


# ----------------------------------------------------------------------------
# Config-options block (constant enum tokens / font pointers)
# ----------------------------------------------------------------------------


def render_config_options(entries) -> str:
    """The constant ``#define`` block at the top of lv_conf_internal.h: built-in
    font pointers, the named integer constants (:class:`ConstToken`, e.g.
    ``LV_STDLIB_BUILTIN 0``), and the enum tokens whose values live inline in a
    ``MEMBER_IS_TOKEN`` choice (``LV_OS_*``, ``LV_NEMA_*``, ...).

    Each token is produced by exactly one entry - a ``ConstToken`` is defined by
    its own config, and the remaining enum tokens belong to a single choice - so
    there is nothing to de-duplicate.
    """
    blocks: list[str] = []

    # The font pointer table (the only non-Kconfig data) comes first.
    fonts = templates.BUILTIN_FONTS
    fwidth = max(len(t) for t in fonts)
    blocks.append(
        "\n".join(
            ["/* Built-in font selectors for LV_FONT_DEFAULT */"]
            + [f"#define {t.ljust(fwidth)}   {v}" for t, v in fonts.items()]
        )
    )

    # Named integer constants (LV_STDLIB_BUILTIN -> 0, LV_LOG_LEVEL_NUM -> 5).
    consts = [e for e in entries if isinstance(e, ConstToken)]
    if consts:
        cwidth = max(len(e.name) for e in consts)
        blocks.append(
            "\n".join(
                ["/* Named constants */"]
                + [f"#define {e.name.ljust(cwidth)}   {e.value}" for e in consts]
            )
        )

    # Enum tokens whose values live inline in the choice (LV_OS_*, LV_NEMA_*).
    for e in entries:
        if isinstance(e, EnumChoice):
            lines = e.emit_internal_options()
            if lines:
                blocks.append("\n".join(lines))
    return "\n\n".join(blocks)


# ----------------------------------------------------------------------------
# Top-level file generation
# ----------------------------------------------------------------------------


def _body(kconf: Kconfig, target: str, entries) -> Emitter:
    em = Emitter(kconf, target, entries)
    groups = collect_groups(kconf)
    if groups:
        for group in groups:
            em.emit_group(group)
    else:
        # No top-level menus (e.g. a flat test fixture): walk the root directly.
        em.walk(kconf.top_node.list, set())
        em._close_all_conditions()
    return em


def generate_template(kconf: Kconfig, entries) -> str:
    em = _body(kconf, "template", entries)
    body = "\n".join(em.out)
    ver = templates.version_string(kconf)
    return (
        templates.TEMPLATE_HEADER.format(ver=ver)
        + "\n"
        + body
        + "\n"
        + templates.TEMPLATE_FOOTER
    )


def generate_bridge(kconf: Kconfig, entries) -> str:
    """Render lv_conf_kconfig.h: the CONFIG_<member> -> CONFIG_<macro> <token>
    ladders for bare choices (no derived int), plus the static platform
    preamble and the *_KILOBYTES deprecation shims."""
    blocks: list[str] = []
    seen: set[str] = set()
    for e in entries:
        lines = e.emit_kconfig()
        if not lines:
            continue
        if e.name in seen:
            raise SystemExit(f"duplicate bridge macro {e.name}")
        seen.add(e.name)
        blocks.append("\n".join(lines))
    return (
        templates.KCONFIG_BRIDGE_PREAMBLE
        + "\n\n".join(blocks)
        + "\n"
        + templates.KCONFIG_BRIDGE_DEPRECATIONS
        + templates.KCONFIG_BRIDGE_FOOTER
    )


def custom_includes(entries) -> list[tuple[str, str]]:
    """``(gate, path)`` pairs following the custom-include convention: a string
    option ``LV_<X>_CUSTOM_INCLUDE`` with a companion bool ``LV_<X>_USE_CUSTOM_INCLUDE``.

    These are optional user headers that override config macros (the sysmon /
    font / nema pattern).  The internal header ``#include``s ``path`` when ``gate``
    is set, so individual source files don't each have to.  An option that lacks
    the companion bool (e.g. the enum-gated ``LV_OS_CUSTOM_INCLUDE``) is left to
    its own subsystem and not auto-included here."""
    names = {e.name for e in entries}
    out: list[tuple[str, str]] = []
    suffix = "_CUSTOM_INCLUDE"
    for e in entries:
        n = e.name
        if not (n.endswith(suffix) and not n.endswith("_USE" + suffix)):
            continue
        gate = n[: -len(suffix)] + "_USE" + suffix
        if gate not in names:
            raise SystemExit(
                f"{n}: missing companion gate {gate}. Add a bool "
                f"`config {gate}`, or list {n} in CUSTOM_INCLUDE_NO_GATE if it "
                f"is gated some other way."
            )
        if n in INCLUDED_BY_SUBSYSTEM:
            continue  # has a gate but its subsystem does the include (see below)
        out.append((gate, n))
    return out


# Custom-include configs that DO follow the gate convention but are *not*
# auto-included here because their subsystem includes them at a specific point.
# LV_GLOBAL_CUSTOM_INCLUDE declares lv_global_default() (needs lv_global_t) and
# defaults to "lv_global.h", so lv_global.h includes it after the type exists;
# pulling it into lv_conf_internal.h (before any LVGL type) would not compile.
INCLUDED_BY_SUBSYSTEM: set[str] = {"LV_GLOBAL_CUSTOM_INCLUDE"}


def _order_deferred(flags):
    """Order derived flags so one referencing another is emitted after it.

    Most derived flags reference only ordinary options (defined earlier in the
    body), but some aggregate other derived flags - e.g. LV_USE_EGL is the OR of
    the per-driver LV_*_USE_EGL flags.  Since a ``#if`` needs every macro already
    defined, emit referenced flags first (stable topological order)."""
    names = {f.name for f in flags}

    def refs(f):
        toks = set(re.findall(r"[A-Za-z_]\w*", f.selectors_expr or ""))
        return {t for t in toks if t in names and t != f.name}

    ordered, done, remaining = [], set(), list(flags)
    while remaining:
        ready = [f for f in remaining if refs(f) <= done]
        assert ready
        for f in ready:
            ordered.append(f)
            done.add(f.name)
            remaining.remove(f)
    return ordered


def generate_internal(kconf: Kconfig, entries) -> str:
    em = _body(kconf, "internal", entries)
    options = render_config_options(entries)
    preamble = templates.INTERNAL_PREAMBLE.replace("__CONFIG_OPTIONS__", options)

    deferred: list[str] = []
    if em.deferred:
        for flag in em.deferred:
            if flag.node is not None:
                sym = flag.node.item
                sel = rev_dep_c_expr(sym.rev_dep, em.guard)
                dep = rev_dep_c_expr(sym.direct_dep, em.guard)
                if sel is not None and dep is not None and dep != "1":
                    sel = f"({sel}) && ({dep})"
                flag.selectors_expr = sel
        deferred.append("/* Derived capability flags (set via Kconfig `select`). */")
        for flag in _order_deferred(em.deferred):
            deferred += flag.emit_internal()
            deferred.append("")

    # Optional user headers that override config macros: include each one (once
    # both its gate and path are defined) so source files don't have to.
    custom_inc: list[str] = []
    ci = custom_includes(entries)
    if ci:
        custom_inc.append("")
        custom_inc.append(
            "/* Optional user headers (LV_*_USE_CUSTOM_INCLUDE) overriding config macros. */"
        )
        for gate, path in ci:
            custom_inc.append(f"#if {gate}")
            custom_inc.append(f"    #include {path}")
            custom_inc.append("#endif")
            custom_inc.append("")

    # Replay Kconfig `select` / `depends on` as #error guards on the lv_conf.h
    # path.  Emitted after the footer derivations so checks may reference symbols
    # computed there (e.g. the Wayland/EGL backend flags).
    guards: list[str] = []
    checks = constraint_checks(entries)
    if checks:
        guards.append("")
        guards.append(
            "/* Kconfig enforces `depends on` / `select`; these checks catch a"
        )
        guards.append(" * hand-written lv_conf.h that violates them. */")
        for c in checks:
            guards += c.emit_internal()
            guards.append("")

    return (
        preamble
        + "\n"
        + "\n".join(em.out)
        + "\n"
        + templates.INTERNAL_COMPATIBILITY_BLOCK
        + "\n"
        + "\n".join(deferred)
        + "\n".join(custom_inc)
        + templates.INTERNAL_FOOTER
        + "\n".join(guards)
        + templates.INTERNAL_CLOSE
    )
