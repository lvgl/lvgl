"""Phase 2: assemble whole header files from the typed model.

The :class:`Emitter` walks the Kconfig tree, tracking the top-level "group"
menus (for banners) and the stack of open ``#if`` conditions (for dependency
nesting).  The actual ``#define`` text for each node comes from its typed
:class:`~config_headers.config_entry.ConfigEntry` (``emit_template`` /
``emit_internal``); this module only handles *structure*: banners, ``#if``
blocks, the constant "Config options" block, and the static preamble/footer.
"""

from __future__ import annotations

from kconfiglib import COMMENT, MENU, NOT, Choice, Kconfig, Symbol

from . import templates
from .config_entry import DerivedFlag, EnumChoice
from .kconfig_utils import dep_terms, term_key
from .parse import classify, enum_backed_choices


class Emitter:
    """Renders one surface (``"template"`` or ``"internal"``) of the tree."""

    def __init__(self, kconf: Kconfig, surface: str, entries):
        self.kconf = kconf
        self.surface = surface
        self.enum_choices = enum_backed_choices(kconf)
        self.out: list[str] = []
        self.cond_stack: list[str] = []
        self.emitted: set[str] = set()
        self.deferred: list[DerivedFlag] = []  # internal: emitted after the body
        # member symbol -> (macro, token): rewrites a `#if <member>` guard (only
        # valid on the Kconfig path) into `<macro> == <token>` (valid on both).
        # Only EnumChoice members need this; BoolGroupChoice members are real
        # #defines, so a `#if <member>` guard already resolves on both paths.
        self.guard: dict[str, tuple[str, str]] = {}
        for e in entries:
            if isinstance(e, EnumChoice):
                for m in e.members:
                    if m.member_name:
                        self.guard[m.member_name] = (e.name, m.token)

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
            if self.surface == "internal":
                self.deferred.append(entry)
            return
        lines = (
            entry.emit_template()
            if self.surface == "template"
            else entry.emit_internal()
        )
        if not lines:
            return
        if self.surface != "internal":
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
# Config-options block (constant enum tokens / font pointers)
# ----------------------------------------------------------------------------


def render_config_options(entries) -> str:
    """The constant ``#define`` block at the top of lv_conf_internal.h.

    Enum tokens that must be defined (e.g. ``LV_STDLIB_BUILTIN 0``) are pulled
    from every :class:`EnumChoice`, de-duplicated by token name so the stdlib
    trio's shared tokens appear once.
    """
    seen: set[str] = set()
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

    for e in entries:
        if not isinstance(e, EnumChoice):
            continue
        defs = [
            (m.token, m.value) for m in e.members if m.define and m.token not in seen
        ]
        if not defs:
            continue
        seen.update(t for t, _ in defs)
        width = max(len(t) for t, _ in defs)
        lines = [f"/* {e.options_title} */"] if e.options_title else []
        lines += [f"#define {t.ljust(width)}   {v}" for t, v in defs]
        blocks.append("\n".join(lines))
    return "\n\n".join(blocks)


# ----------------------------------------------------------------------------
# Top-level file generation
# ----------------------------------------------------------------------------


def _body(kconf: Kconfig, surface: str, entries) -> Emitter:
    em = Emitter(kconf, surface, entries)
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


def generate_internal(kconf: Kconfig, entries) -> str:
    em = _body(kconf, "internal", entries)
    options = render_config_options(entries)
    preamble = templates.INTERNAL_PREAMBLE.replace("__CONFIG_OPTIONS__", options)

    deferred: list[str] = []
    if em.deferred:
        deferred.append("/* Derived capability flags (set via Kconfig `select`). */")
        for flag in em.deferred:
            deferred += flag.emit_internal()
            deferred.append("")

    return (
        preamble
        + "\n"
        + "\n".join(em.out)
        + "\n"
        + templates.INTERNAL_COMPATIBILITY_BLOCK
        + "\n"
        + "\n".join(deferred)
        + templates.INTERNAL_FOOTER
    )
