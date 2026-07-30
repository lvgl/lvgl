"""Phase 1: turn a Kconfig tree into a list of typed :class:`ConfigEntry`.

All classification logic lives here.  ``classify(node)`` is the single place
that decides *what kind* of config a node is; ``parse(path)`` walks the tree
and returns the resulting entries in Kconfig order.

Only the types implemented so far are recognised; ``classify`` returns ``None``
for anything not yet handled, so the model can grow one type at a time without
silently mis-emitting the rest.
"""

from __future__ import annotations

import os
import re

from kconfiglib import BOOL, HEX, INT, STRING, TRISTATE, Choice, Kconfig, Symbol

from .config_entry import (
    BoolConfig,
    ConfigEntry,
    ConstToken,
    DerivedFlag,
    EnumChoice,
    EnumMember,
    IntConfig,
    StringConfig,
)
from .kconfig_utils import (
    choice_default,
    dep_terms,
    doc_text,
    int_const_value,
    is_int_const,
    member_requires,
    resolve_int_value,
    rev_dep_c_expr,
    select_targets,
    term_key,
)

# ----------------------------------------------------------------------------
# Enum-choice classification policy
# ----------------------------------------------------------------------------
# Some choices use literal values e.g. (`default 1 if LV_OS_PTHREAD`)
# but they must emit the member name as the token and not the number
# (#define LV_USE_OS LV_OS_PTHREAD instead of #define LV_USE_OS 1)
# These follow the same structure as LV_COLOR_DEPTH where LV_COLOR_DEPTH is exported
# as the raw number (#define LV_COLOR_DEPTH 16)
#
# TODO:(v10) migrate these to the stdlib shape - reference named int-const token
# configs (`config LV_OS_PTHREAD / int / default 1`) instead of bare literals,
# after which the named-const reference IS the signal and this set can be
# deleted.  Deferred to the v10 major because freeing the token name requires
# renaming the selector members (CONFIG_LV_OS_PTHREAD -> CONFIG_LV_USE_OS_PTHREAD),
# which breaks embedded users' Kconfig configs.
MEMBER_IS_TOKEN: set[str] = {
    "LV_USE_OS",  # tokens LV_OS_* are defined into lv_conf_internal.h
    "LV_LOG_LEVEL",  # LV_LOG_LEVEL_* defined into lv_conf_internal.h
    "LV_USE_DRAW_SW_ASM",  # LV_DRAW_SW_ASM_* defined into lv_conf_internal.h
    "LV_USE_NEMA_LIB",  # LV_NEMA_LIB_*
    "LV_USE_NEMA_HAL",  # LV_NEMA_HAL_*
    "LV_NANOVG_BACKEND",  # LV_NANOVG_BACKEND_*
    "LV_WAYLAND_BACKEND",  # LV_WAYLAND_BACKEND_* (SHM/EGL/G2D)
    "LV_LINUX_DRM_BACKEND",  # LV_LINUX_DRM_BACKEND_* (FBDEV/GBM/EGL)
    "LV_SDL_BACKEND",  # LV_SDL_BACKEND_* (SW/TEXTURE/EGL)
    "LV_CHECK_ARG_LOG_MODE",  # LV_CHECK_ARG_LOG_MODE_*
    "LV_SDL_MOUSEWHEEL_MODE",  # LV_SDL_MOUSEWHEEL_MODE_* owned by a C header
    "LV_VG_LITE_GPU",  # LV_VG_LITE_GPU_* defined into lv_conf_internal.h; the
    # VG-Lite options dispatch header compares against these tokens
}

# Symbols that must never appear in the lv_conf_template.h
IGNORE_SYMBOLS: set[str] = {
    # Only makes sense when the user is using kconfig
    "LV_CONF_SKIP",
    # Defined in `lv_version.h`
    "LVGL_VERSION_MAJOR",
    "LVGL_VERSION_MINOR",
    "LVGL_VERSION_PATCH",
}

# Anonymous choices whose members map to tokens that live in LVGL's
# C headers, with member != token.  Keyed by the member-name frozenset (the
# choice is anonymous, so there is no choice name to key on); value is
# (emitted macro, {member: token}).
# TODO:(v10) the note on MEMBER_IS_TOKEN also applies here
_ALIGN = (
    "TOP_LEFT",
    "TOP_MID",
    "TOP_RIGHT",
    "BOTTOM_LEFT",
    "BOTTOM_MID",
    "BOTTOM_RIGHT",
    "LEFT_MID",
    "RIGHT_MID",
    "CENTER",
)
_RENDER = ("PARTIAL", "DIRECT", "FULL")


def _prefixed_map(member_prefix: str, token_prefix: str, suffixes) -> dict:
    return {f"{member_prefix}{s}": f"{token_prefix}{s}" for s in suffixes}


CHOICE_TOKEN_MAP: dict = {
    frozenset(_prefixed_map("LV_SDL_RENDER_MODE_", "", _RENDER)): (
        "LV_SDL_RENDER_MODE",
        _prefixed_map("LV_SDL_RENDER_MODE_", "LV_DISPLAY_RENDER_MODE_", _RENDER),
    ),
    frozenset(_prefixed_map("LV_X11_RENDER_MODE_", "", _RENDER)): (
        "LV_X11_RENDER_MODE",
        _prefixed_map("LV_X11_RENDER_MODE_", "LV_DISPLAY_RENDER_MODE_", _RENDER),
    ),
    frozenset(_prefixed_map("LV_LINUX_FBDEV_RENDER_MODE_", "", _RENDER)): (
        "LV_LINUX_FBDEV_RENDER_MODE",
        _prefixed_map(
            "LV_LINUX_FBDEV_RENDER_MODE_", "LV_DISPLAY_RENDER_MODE_", _RENDER
        ),
    ),
    frozenset(_prefixed_map("LV_PERF_MONITOR_ALIGN_", "", _ALIGN)): (
        "LV_USE_PERF_MONITOR_POS",
        _prefixed_map("LV_PERF_MONITOR_ALIGN_", "LV_ALIGN_", _ALIGN),
    ),
    frozenset(_prefixed_map("LV_MEM_MONITOR_ALIGN_", "", _ALIGN)): (
        "LV_USE_MEM_MONITOR_POS",
        _prefixed_map("LV_MEM_MONITOR_ALIGN_", "LV_ALIGN_", _ALIGN),
    ),
}


def _is_deprecated(node) -> bool:
    """A config formally flagged deprecated - kept out of the template (handled
    by the bridge shims).  Matches the bracketed ``[DEPRECATED ...]`` prompt
    convention or a help text that *opens* with "Deprecated"
    """
    prompt = node.prompt[0] if node.prompt else ""
    if "[DEPRECATED" in prompt.upper():
        return True
    return bool(node.help) and node.help.lstrip().startswith("Deprecated")


def load(path: str) -> Kconfig:
    """Parse a Kconfig file into a kconfiglib ``Kconfig`` object."""
    os.environ["srctree"] = os.path.dirname(os.path.abspath(path)) or "."
    return Kconfig(path, warn_to_stderr=False, suppress_traceback=True)


def _derived_enum(sym):
    """If *sym* is a derived-enum int - every default has the form
    ``<token> if <member>`` over a single choice - return
    ``(choice, [(member_sym, value_expr), ...])`` in default order; else None.

    Unifies the old ``enum_derived_choice`` (named-const tokens) and
    ``int_alias_members`` (bare-literal tokens): both are just "one choice
    member per default", differing only in what the default *value* is, which
    :func:`_build_enum_choice` interprets.
    """
    if sym.type not in (INT, HEX) or not sym.defaults:
        return None
    dd_keys = {term_key(x) for x in dep_terms(sym.direct_dep)}
    choice = None
    pairs = []
    for value, cond in sym.defaults:
        rest = [x for x in dep_terms(cond) if term_key(x) not in dd_keys]
        if len(rest) != 1:
            return None
        member = rest[0]
        if not (isinstance(member, Symbol) and member.choice is not None):
            return None
        if choice is None:
            choice = member.choice
        elif member.choice is not choice:
            return None
        pairs.append((member, value))
    return (choice, pairs) if choice is not None else None


def _member_token(macro: str, member, value):
    """Resolve one derived-enum member to its emitted ``EnumMember``.

    * macro is in ``MEMBER_IS_TOKEN``  -> emit the *member* name; value is the
      bare literal, defined here (``LV_USE_OS`` -> ``LV_OS_PTHREAD`` = ``1``).
    * otherwise                        -> reference the default symbol by name,
      defining nothing.  It is either a bare number (value-alias, ``LV_COLOR_DEPTH``
      -> ``16``) or a named :class:`ConstToken` already defined in the options
      block (stdlib, ``LV_STDLIB_BUILTIN``).
    """
    if macro in MEMBER_IS_TOKEN:
        token = member.name
        em = EnumMember(token, value.name, True, member.name)
    else:
        em = EnumMember(value.name, None, False, member.name)
    return member, em


def _norm(text: str) -> str:
    return re.sub(r"[^a-z0-9]", "", text.lower())


def _enum_doc(choice, resolved) -> str:
    """The choice's help/prompt plus a "Possible values" list, one bullet per
    token.  A member description is appended only when it adds information: a
    bare ``N:`` ordinal is stripped, a ``PREFIX:`` that restates the token is
    dropped, and a description that merely echoes the token is omitted."""
    cnode = choice.nodes[0]
    head = cnode.help or (cnode.prompt[0] if cnode.prompt else "")
    lines = [head] if head else []
    lines.append("Possible values:")
    for member, em in resolved:
        prompt = (
            member.nodes[0].prompt[0]
            if (member.nodes and member.nodes[0].prompt)
            else ""
        )
        desc = re.sub(r"^\s*\d+\s*:\s*", "", prompt).strip()
        pre = re.match(r"^([^:]+):\s*(.*)$", desc)
        if pre and _norm(pre.group(1)) in _norm(em.token):
            desc = pre.group(2).strip()
        if not (
            desc
            and _norm(desc) not in _norm(em.token)
            and _norm(em.token) not in _norm(desc)
        ):
            desc = ""
        notes = []
        requires = member_requires(member, choice)
        if requires:
            notes.append(f"requires {requires}")
        enables = select_targets(member)
        if enables:
            notes.append("enable: " + ", ".join(enables))
        suffix = f" ({'; '.join(notes)})" if notes else ""
        bullet = f"- {em.token}: {desc}" if desc else f"- {em.token}"
        lines.append(bullet + suffix)
    return "\n".join(lines)


def _assemble_enum(
    macro, choice, node, resolved, *, needs_bridge, selected=None, doc=None
) -> EnumChoice:
    """Build an :class:`EnumChoice` from resolved ``(member, EnumMember)`` pairs.

    Shared by the derived-int path (:func:`_build_enum_choice`) and the bare
    -choice path (:func:`_build_choice`).  *selected* overrides the emitted
    token (value-alias choices pass a gating-aware resolved number); *doc*
    overrides the comment (value-aliases use the int symbol's own help and get
    no "Possible values" list)."""
    if selected is not None:
        selected_token = selected
    else:
        sel = choice_default(choice)
        selected_token = next(
            (em.token for member, em in resolved if member is sel),
            resolved[0][1].token,
        )
    cnode = choice.nodes[0]
    return EnumChoice(
        macro,
        [em for _, em in resolved],
        selected_token,
        options_title=cnode.prompt[0] if cnode.prompt else "",
        needs_bridge=needs_bridge,
        node=node,
        doc=_enum_doc(choice, resolved) if doc is None else doc,
    )


def _build_enum_choice(sym, node) -> EnumChoice | None:
    """Bucket: a choice backed by a derived int (stdlib, OS, COLOR_DEPTH...)."""
    derived = _derived_enum(sym)
    if derived is None:
        return None
    choice, pairs = derived
    resolved = [_member_token(sym.name, member, value) for member, value in pairs]
    # A value-alias (members map to bare numbers, e.g. LV_COLOR_DEPTH or the
    # buffer counts) emits the *resolved* number - gating-aware, so a disabled
    # driver's count reads 0 - and documents itself with the int symbol's own
    # help (no "Possible values" list, which only fits symbolic tokens).
    value_alias = sym.name not in MEMBER_IS_TOKEN and not any(
        isinstance(v, Symbol) and is_int_const(v) for _, v in pairs
    )
    selected = resolve_int_value(sym) if value_alias else None
    doc = doc_text(node) if value_alias else None
    return _assemble_enum(
        sym.name, choice, node, resolved, needs_bridge=False, selected=selected, doc=doc
    )


def _build_choice(choice, node) -> EnumChoice | None:  # | BoolGroupChoice :
    """A choice with NO derived int.

    * member set in ``CHOICE_TOKEN_MAP``  -> single macro, mapped tokens
      (render modes, monitor positions).
    * named choice                        -> single macro, member == token (B1:
      LV_TXT_ENC, LV_BIDI_BASE_DIR_DEF).
    Bare-choice tokens always live in C headers, so none are (re)defined."""
    members = [m for m in choice.syms if m.name not in IGNORE_SYMBOLS]
    if not members:
        return None
    if _is_deprecated(node):
        return None
    member_names = frozenset(m.name for m in members)

    override = CHOICE_TOKEN_MAP.get(member_names)
    if override:
        macro, tokenmap = override
    elif choice.name:
        macro, tokenmap = choice.name, None
    else:
        raise ValueError(
            f"Invalid choice config:\n{choice}\n Use a standardized choice instead"
        )

    resolved = []
    for member in members:
        token = tokenmap.get(member.name, member.name) if tokenmap else member.name
        resolved.append((member, EnumMember(token, None, False, member.name)))
    return _assemble_enum(macro, choice, node, resolved, needs_bridge=True)


def enum_backed_choices(kconf: Kconfig) -> set:
    """The set of ``Choice`` objects represented by a derived-enum int (stdlib,
    LV_USE_OS, LV_COLOR_DEPTH, LV_LOG_LEVEL ...).  These are emitted via their
    int (as :class:`EnumChoice`); the choice node itself is skipped, so it is
    *not* mistaken for an anonymous :class:`BoolGroupChoice`."""
    out = set()
    for sym in kconf.unique_defined_syms:
        derived = _derived_enum(sym)
        if derived is not None:
            out.add(derived[0])
    return out


def classify(node, enum_choices: frozenset = frozenset()) -> ConfigEntry | None:
    """Map a single Kconfig ``MenuNode`` to a typed ``ConfigEntry``.

    *enum_choices* is the set from :func:`enum_backed_choices`; a choice in it
    is emitted via its derived int, so the choice node itself is skipped.
    Returns ``None`` for nodes that are not (yet) a recognised config kind.
    """
    item = node.item
    if isinstance(item, Choice):
        if item in enum_choices:
            return None  # handled via its derived int (EnumChoice)
        return _build_choice(item, node)
    if not isinstance(item, Symbol):
        return None
    # Choice members are owned by their choice, not classified standalone.
    if item.choice is not None:
        return None
    # Build-system-only / internally-derived / specially-handled symbols.
    if item.name in IGNORE_SYMBOLS:
        return None
    # Deprecated options are kept out of the template.
    if _is_deprecated(node):
        return None

    # Derived-enum ints carry no prompt of their own (the prompt is on the
    # selector choice), so this must run before the prompt gate below.
    enum = _build_enum_choice(item, node)
    if enum is not None:
        return enum

    # Internal capability flags are no-prompt bools set via `select`; derived
    # from their reverse-deps.  Also runs before the prompt gate.  rev_dep_c_expr
    # is choice-aware: a flag selected by a choice member (e.g. LV_USE_EGL, set
    # by the Wayland EGL backend) carries a bare <choice> term that it drops,
    # rather than leaving the flag unclassified and silently dropped.
    if item.type in (BOOL, TRISTATE) and not node.prompt:
        if item.rev_dep is not item.kconfig.n:
            expr = rev_dep_c_expr(item.rev_dep)
            if expr is not None:
                return DerivedFlag(item.name, expr, node=node)

    if item.type in (INT, HEX):
        # A promptless int/hex with a single literal default is a named constant
        # (LV_STDLIB_BUILTIN -> 0): define it in the options block at the top,
        # where the enum choices that reference it by name can see it.
        if is_int_const(item):
            return ConstToken(item.name, int_const_value(item), node=node)
        # Any other int/hex is emitted - including no-prompt computed aliases
        # like LV_SDL_BUF_COUNT (the prompt gate below would wrongly drop it).
        return IntConfig.from_symbol(item, node)

    # Bool/string options require a prompt to be user-facing.
    if not node.prompt:
        return None
    if item.type == BOOL:
        return BoolConfig.from_symbol(item, node)
    if item.type == STRING:
        return StringConfig.from_symbol(item, node)

    return None


def parse_entries(kconf: Kconfig) -> list[ConfigEntry]:
    """Classify every node of an already-loaded *kconf*, in tree order."""
    enum_choices = enum_backed_choices(kconf)
    entries: list[ConfigEntry] = []
    seen: set[str] = set()

    def walk(node):
        while node:
            entry = classify(node, enum_choices)
            if entry is not None and entry.name not in seen:
                seen.add(entry.name)
                entries.append(entry)
            if node.list:
                walk(node.list)
            node = node.next

    walk(kconf.top_node.list)
    return entries


def parse(path: str) -> list[ConfigEntry]:
    """Load *path* and return every recognised config entry, in tree order."""
    return parse_entries(load(path))
