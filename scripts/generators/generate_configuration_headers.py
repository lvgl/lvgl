#!/usr/bin/env python3
"""
generate_configuration_headers.py
====================
Generate ``lv_conf_template.h``, ``lv_conf_internal.h`` and ``lv_conf_kconfig.h``
from LVGL's Kconfig tree.

The Kconfig tree is the single source of truth for the set of options, their
default values, their help texts and their dependency structure.  This script
walks that tree and renders a C header that mirrors it.

Two things are intentionally *not* derived from Kconfig and are configured here
instead, so the rest stays fully automatic:

* ``GROUP_ORDER`` - the order in which the top-level menus ("groups") are
  emitted.  Ordering is done at the group level only; inside a group the
  options keep their Kconfig order.
* ``IGNORE_SYMBOLS`` - symbols that must never appear in the template (internal
  capability flags, build-system-only switches, etc.).

Usage::

    python3 scripts/generators/generate_lv_conf.py [root_kconfig] [options]

Options::

    --template PATH   Output file (default: print to stdout)
    --srctree  PATH   LVGL source root used to resolve `rsource`/`source`
                      (default: directory containing the root Kconfig file)
"""

from __future__ import annotations

import argparse
import os
import re
import sys

from kconfiglib import MenuNode

try:
    from kconfiglib import (
        Kconfig,
        Symbol,
        Choice,
        MENU,
        NOT,
        COMMENT,
        BOOL,
        TRISTATE,
        INT,
        HEX,
        STRING,
        expr_str,
        expr_value,
        split_expr,
        AND,
    )
except ImportError:
    sys.exit("kconfiglib is required.  Run: pip install kconfiglib")


# ============================================================================
# CONFIGURATION
# ============================================================================

# Symbols that must never appear in the template.  These are internal
# capability flags (set via `select`), build-system-only switches, or pure
# Kconfig bookkeeping with no meaning in lv_conf.h.
IGNORE_SYMBOLS: set[str] = {
    # Version numbers - injected into the file header, not emitted as options
    "LVGL_VERSION_MAJOR",
    "LVGL_VERSION_MINOR",
    "LVGL_VERSION_PATCH",
    # Build-system only
    "LV_CONF_SKIP",
    # Internal capability flags, auto-selected by draw units / backends
    "LV_DRAW_HAS_VECTOR_SUPPORT",
    "LV_DRAW_HAS_3D_SUPPORT",
    "LV_USE_EGL",
    # Combined "feature available" gates that are *derived* in lv_conf_internal.h
    # from the INTERNAL/EXTERNAL bools; not part of the public lv_conf.h surface.
    "LV_USE_THORVG",
    "LV_USE_LZ4",
    # Numeric Kconfig aliases of choices that the template expresses
    # symbolically (e.g. `LV_LOG_LEVEL_WARN` rather than `2`).  See ENUM_CHOICES.
    "LV_LOG_LEVEL",
    "LV_USE_DRAW_SW_ASM",
    "LV_CHECK_ARG_LOG_MODE",
    "LV_SDL_MOUSEWHEEL_MODE",
    # Internal "raw" count that the public LV_SDL_BUF_COUNT alias mirrors.
    "LV_SDL_BUFFER_COUNT",
    # Wayland backend bools are handled in the internal-header footer (gated by
    # LV_USE_WAYLAND, SHM default), not emitted as standalone template options.
    "LV_WAYLAND_USE_SHM",
    "LV_WAYLAND_USE_EGL",
    "LV_WAYLAND_USE_G2D",
}


# Some choices map to a single enum-valued macro instead of one bool per member
# (the `LV_USE_OS LV_OS_NONE` pattern).  When the choice is *named* and its
# member symbols are already the enum tokens, this is automatic.  When the
# member names differ from the tokens (e.g. stdlib reuses `LV_STDLIB_*` across
# three selectors, so members must have distinct names), map them here:
#   frozenset(member names) -> (macro, {member: token} or None for identity)
ENUM_CHOICES: dict = {
    frozenset(
        {
            "LV_USE_BUILTIN_MALLOC",
            "LV_USE_CLIB_MALLOC",
            "LV_USE_MICROPYTHON_MALLOC",
            "LV_USE_RTTHREAD_MALLOC",
            "LV_USE_CUSTOM_MALLOC",
        }
    ): (
        "LV_USE_STDLIB_MALLOC",
        {
            "LV_USE_BUILTIN_MALLOC": "LV_STDLIB_BUILTIN",
            "LV_USE_CLIB_MALLOC": "LV_STDLIB_CLIB",
            "LV_USE_MICROPYTHON_MALLOC": "LV_STDLIB_MICROPYTHON",
            "LV_USE_RTTHREAD_MALLOC": "LV_STDLIB_RTTHREAD",
            "LV_USE_CUSTOM_MALLOC": "LV_STDLIB_CUSTOM",
        },
    ),
    frozenset(
        {
            "LV_USE_BUILTIN_STRING",
            "LV_USE_CLIB_STRING",
            "LV_USE_CUSTOM_STRING",
        }
    ): (
        "LV_USE_STDLIB_STRING",
        {
            "LV_USE_BUILTIN_STRING": "LV_STDLIB_BUILTIN",
            "LV_USE_CLIB_STRING": "LV_STDLIB_CLIB",
            "LV_USE_CUSTOM_STRING": "LV_STDLIB_CUSTOM",
        },
    ),
    frozenset(
        {
            "LV_USE_BUILTIN_SPRINTF",
            "LV_USE_CLIB_SPRINTF",
            "LV_USE_CUSTOM_SPRINTF",
        }
    ): (
        "LV_USE_STDLIB_SPRINTF",
        {
            "LV_USE_BUILTIN_SPRINTF": "LV_STDLIB_BUILTIN",
            "LV_USE_CLIB_SPRINTF": "LV_STDLIB_CLIB",
            "LV_USE_CUSTOM_SPRINTF": "LV_STDLIB_CUSTOM",
        },
    ),
    frozenset(
        {
            "LV_LOG_LEVEL_TRACE",
            "LV_LOG_LEVEL_INFO",
            "LV_LOG_LEVEL_WARN",
            "LV_LOG_LEVEL_ERROR",
            "LV_LOG_LEVEL_USER",
            "LV_LOG_LEVEL_NONE",
        }
    ): ("LV_LOG_LEVEL", None),
    frozenset(
        {
            "LV_DRAW_SW_ASM_NONE",
            "LV_DRAW_SW_ASM_NEON",
            "LV_DRAW_SW_ASM_HELIUM",
            "LV_DRAW_SW_ASM_RISCV_V",
            "LV_DRAW_SW_ASM_CUSTOM",
        }
    ): ("LV_USE_DRAW_SW_ASM", None),
    frozenset(
        {
            "LV_CHECK_ARG_LOG_MODE_NONE",
            "LV_CHECK_ARG_LOG_MODE_MINIMAL",
            "LV_CHECK_ARG_LOG_MODE_VERBOSE",
        }
    ): ("LV_CHECK_ARG_LOG_MODE", None),
    frozenset(
        {
            "LV_SDL_RENDER_MODE_PARTIAL",
            "LV_SDL_RENDER_MODE_DIRECT",
            "LV_SDL_RENDER_MODE_FULL",
        }
    ): (
        "LV_SDL_RENDER_MODE",
        {
            "LV_SDL_RENDER_MODE_PARTIAL": "LV_DISPLAY_RENDER_MODE_PARTIAL",
            "LV_SDL_RENDER_MODE_DIRECT": "LV_DISPLAY_RENDER_MODE_DIRECT",
            "LV_SDL_RENDER_MODE_FULL": "LV_DISPLAY_RENDER_MODE_FULL",
        },
    ),
    frozenset(
        {
            "LV_LINUX_FBDEV_RENDER_MODE_PARTIAL",
            "LV_LINUX_FBDEV_RENDER_MODE_DIRECT",
            "LV_LINUX_FBDEV_RENDER_MODE_FULL",
        }
    ): (
        "LV_LINUX_FBDEV_RENDER_MODE",
        {
            "LV_LINUX_FBDEV_RENDER_MODE_PARTIAL": "LV_DISPLAY_RENDER_MODE_PARTIAL",
            "LV_LINUX_FBDEV_RENDER_MODE_DIRECT": "LV_DISPLAY_RENDER_MODE_DIRECT",
            "LV_LINUX_FBDEV_RENDER_MODE_FULL": "LV_DISPLAY_RENDER_MODE_FULL",
        },
    ),
    frozenset(
        {
            "LV_SDL_MOUSEWHEEL_MODE_ENCODER",
            "LV_SDL_MOUSEWHEEL_MODE_CROWN",
        }
    ): ("LV_SDL_MOUSEWHEEL_MODE", None),
    # Performance / memory monitor screen position -> LV_ALIGN_* tokens.
    frozenset(
        {
            f"LV_PERF_MONITOR_ALIGN_{p}"
            for p in (
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
        }
    ): (
        "LV_USE_PERF_MONITOR_POS",
        {
            f"LV_PERF_MONITOR_ALIGN_{p}": f"LV_ALIGN_{p}"
            for p in (
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
        },
    ),
    frozenset(
        {
            f"LV_MEM_MONITOR_ALIGN_{p}"
            for p in (
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
        }
    ): (
        "LV_USE_MEM_MONITOR_POS",
        {
            f"LV_MEM_MONITOR_ALIGN_{p}": f"LV_ALIGN_{p}"
            for p in (
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
        },
    ),
}

# ============================================================================
# CONFIG-OPTION CONSTANTS
# ============================================================================
# Symbolic tokens that enum-style options (and LV_FONT_DEFAULT) expand to.  They
# are emitted verbatim into the "Config options" block at the top of
# lv_conf_internal.h, so both the lv_conf.h path (`#define LV_USE_OS LV_OS_NONE`)
# and the Kconfig bridge (`#define CONFIG_LV_USE_OS LV_OS_NONE`) resolve through
# the same table.  Each group is `token -> C value`; CONFIG_OPTION_GROUPS fixes
# the emission order and the optional section comment.

OS_OPTIONS: dict = {
    "LV_OS_NONE": "0",
    "LV_OS_PTHREAD": "1",
    "LV_OS_FREERTOS": "2",
    "LV_OS_CMSIS_RTOS2": "3",
    "LV_OS_RTTHREAD": "4",
    "LV_OS_WINDOWS": "5",
    "LV_OS_MQX": "6",
    "LV_OS_SDL2": "7",
    "LV_OS_CUSTOM": "255",
}

STDLIB_OPTIONS: dict = {
    "LV_STDLIB_BUILTIN": "0",
    "LV_STDLIB_CLIB": "1",
    "LV_STDLIB_MICROPYTHON": "2",
    "LV_STDLIB_RTTHREAD": "3",
    "LV_STDLIB_CUSTOM": "255",
}

DRAW_SW_ASM_OPTIONS: dict = {
    "LV_DRAW_SW_ASM_NONE": "0",
    "LV_DRAW_SW_ASM_NEON": "1",
    "LV_DRAW_SW_ASM_HELIUM": "2",
    "LV_DRAW_SW_ASM_RISCV_V": "3",
    "LV_DRAW_SW_ASM_CUSTOM": "255",
}

NEMA_LIB_OPTIONS: dict = {
    "LV_NEMA_LIB_NONE": "0",
    "LV_NEMA_LIB_M33_REVC": "1",
    "LV_NEMA_LIB_M33_NEMAPVG": "2",
    "LV_NEMA_LIB_M55": "3",
    "LV_NEMA_LIB_M7": "4",
}

NEMA_HAL_OPTIONS: dict = {
    "LV_NEMA_HAL_CUSTOM": "0",
    "LV_NEMA_HAL_STM32": "1",
}

NANOVG_BACKEND_OPTIONS: dict = {
    "LV_NANOVG_BACKEND_GL2": "1",
    "LV_NANOVG_BACKEND_GL3": "2",
    "LV_NANOVG_BACKEND_GLES2": "3",
    "LV_NANOVG_BACKEND_GLES3": "4",
}

CHECK_ARG_LOG_MODE_OPTIONS: dict = {
    "LV_CHECK_ARG_LOG_MODE_NONE": "0",
    "LV_CHECK_ARG_LOG_MODE_MINIMAL": "1",
    "LV_CHECK_ARG_LOG_MODE_VERBOSE": "2",
}

BUILTIN_FONTS: dict = {
    "LV_FONT_DEFAULT_MONTSERRAT_8": "&lv_font_montserrat_8",
    "LV_FONT_DEFAULT_MONTSERRAT_10": "&lv_font_montserrat_10",
    "LV_FONT_DEFAULT_MONTSERRAT_12": "&lv_font_montserrat_12",
    "LV_FONT_DEFAULT_MONTSERRAT_14": "&lv_font_montserrat_14",
    "LV_FONT_DEFAULT_MONTSERRAT_16": "&lv_font_montserrat_16",
    "LV_FONT_DEFAULT_MONTSERRAT_18": "&lv_font_montserrat_18",
    "LV_FONT_DEFAULT_MONTSERRAT_20": "&lv_font_montserrat_20",
    "LV_FONT_DEFAULT_MONTSERRAT_22": "&lv_font_montserrat_22",
    "LV_FONT_DEFAULT_MONTSERRAT_24": "&lv_font_montserrat_24",
    "LV_FONT_DEFAULT_MONTSERRAT_26": "&lv_font_montserrat_26",
    "LV_FONT_DEFAULT_MONTSERRAT_28": "&lv_font_montserrat_28",
    "LV_FONT_DEFAULT_MONTSERRAT_30": "&lv_font_montserrat_30",
    "LV_FONT_DEFAULT_MONTSERRAT_32": "&lv_font_montserrat_32",
    "LV_FONT_DEFAULT_MONTSERRAT_34": "&lv_font_montserrat_34",
    "LV_FONT_DEFAULT_MONTSERRAT_36": "&lv_font_montserrat_36",
    "LV_FONT_DEFAULT_MONTSERRAT_38": "&lv_font_montserrat_38",
    "LV_FONT_DEFAULT_MONTSERRAT_40": "&lv_font_montserrat_40",
    "LV_FONT_DEFAULT_MONTSERRAT_42": "&lv_font_montserrat_42",
    "LV_FONT_DEFAULT_MONTSERRAT_44": "&lv_font_montserrat_44",
    "LV_FONT_DEFAULT_MONTSERRAT_46": "&lv_font_montserrat_46",
    "LV_FONT_DEFAULT_MONTSERRAT_48": "&lv_font_montserrat_48",
    "LV_FONT_DEFAULT_MONTSERRAT_28_COMPRESSED": "&lv_font_montserrat_28_compressed",
    "LV_FONT_DEFAULT_DEJAVU_16_PERSIAN_HEBREW": "&lv_font_dejavu_16_persian_hebrew",
    "LV_FONT_DEFAULT_SOURCE_HAN_SANS_SC_14_CJK": "&lv_font_source_han_sans_sc_14_cjk",
    "LV_FONT_DEFAULT_SOURCE_HAN_SANS_SC_16_CJK": "&lv_font_source_han_sans_sc_16_cjk",
    "LV_FONT_DEFAULT_UNSCII_8": "&lv_font_unscii_8",
    "LV_FONT_DEFAULT_UNSCII_16": "&lv_font_unscii_16",
}

# (section comment, token->value table); emission order is significant.
CONFIG_OPTION_GROUPS: list = [
    ("OS Options", OS_OPTIONS),
    ("Standard Library Options", STDLIB_OPTIONS),
    ("Draw SW ASM Options", DRAW_SW_ASM_OPTIONS),
    ("NemaGFX Options", NEMA_LIB_OPTIONS),
    (None, NEMA_HAL_OPTIONS),
    ("NanoVG Options", NANOVG_BACKEND_OPTIONS),
    ("Check Arg Options", CHECK_ARG_LOG_MODE_OPTIONS),
    ("Built-in font selectors for LV_FONT_DEFAULT", BUILTIN_FONTS),
]


def render_config_options() -> str:
    """Render the "Config options" constant block for lv_conf_internal.h.

    Each group becomes ``#define <token> <value>`` lines with the value column
    aligned within the group, groups separated by a blank line.
    """
    blocks = []
    for comment, options in CONFIG_OPTION_GROUPS:
        width = max(len(name) for name in options)
        lines = []
        if comment:
            lines.append(f"/* {comment} */")
        lines += [
            f"#define {name.ljust(width)}   {value}" for name, value in options.items()
        ]
        blocks.append("\n".join(lines))
    return "\n\n".join(blocks)


# ============================================================================
# RENDERING HELPERS
# ============================================================================


def c_comment(text: str, indent: str) -> list[str]:
    """Render *text* (possibly multi-line) as a Doxygen-style block comment."""
    lines = [ln.rstrip() for ln in text.strip("\n").split("\n")]
    if len(lines) == 1:
        return [f"{indent}/** {lines[0]} */"]
    out = [f"{indent}/** {lines[0]}"]
    out += [f"{indent} *  {ln}" if ln else f"{indent} *" for ln in lines[1:]]
    out += [f"{indent} */"]
    return out


def define_value(sym: Symbol) -> str:
    """Return the C literal for a symbol's *own* default value.

    We deliberately ignore the symbol's direct dependencies: an option that
    sits inside ``#if LV_USE_X`` should still show the value it would take when
    that feature is on, not the dependency-suppressed 0/n that ``str_value``
    would report in the default configuration.  Conditional defaults
    (``default A if C``) are still evaluated against the current config.
    """
    t = sym.type
    # kconfiglib AND-folds the symbol's `depends on`/enclosing `if` into the
    # condition of every default.  Strip those terms back out so a feature's
    # own default survives even when the gating feature is off by default.
    dd_keys = {term_key(x) for x in dep_terms(sym.direct_dep)}
    for value, cond in sym.defaults:
        rest = [x for x in dep_terms(cond) if term_key(x) not in dd_keys]
        if all(expr_value(x) > 0 for x in rest):
            if t in (BOOL, TRISTATE):
                return "1" if expr_value(value) > 0 else "0"
            # An int/hex default that points at another *config* symbol (e.g.
            # LV_SDL_BUF_COUNT defaults to the computed LV_SDL_BUFFER_COUNT) is
            # resolved to that symbol's concrete value, not its name.  Numeric
            # literals are also Symbols but have no int/hex type, so they fall
            # through to expr_str() below and are emitted verbatim.
            if (
                t in (INT, HEX)
                and isinstance(value, Symbol)
                and value.type in (INT, HEX)
            ):
                return define_value(value)
            s = expr_str(value)
            if t == HEX:
                return s if s.startswith("0x") else f"0x{s}"
            return s

    # No (matching) default.
    if t in (BOOL, TRISTATE):
        return "0"
    if t == STRING:
        return '""'
    if t == HEX:
        return "0x0"
    return "0"


# ============================================================================
# DEPENDENCY / `#if` TRACKING
# ============================================================================


def dep_terms(expr) -> list:
    """AND-split a dependency expression into its individual terms."""
    if expr is None:
        return []
    return [t for t in split_expr(expr, AND) if t is not None]


def term_key(term) -> str:
    """Stable identity for a dependency term (so we can diff them)."""
    return expr_str(term)


def _collect_expr_syms(expr, acc: set):
    """Recursively collect Symbol names appearing in a Kconfig expression."""
    if expr is None:
        return
    if isinstance(expr, Symbol):
        if not expr.is_constant:
            acc.add(expr.name)
    elif isinstance(expr, tuple):
        for sub in expr[1:]:
            _collect_expr_syms(sub, acc)


def choice_default(choice: Choice):
    """The choice's selected member, or its default member when the choice is
    inactive because of an enclosing `if` (so a gated choice like the log level
    still reports `LV_LOG_LEVEL_WARN` rather than "nothing selected")."""
    if choice.selection:
        return choice.selection
    dd_keys = {term_key(x) for x in dep_terms(getattr(choice, "direct_dep", None))}
    for sym, cond in getattr(choice, "defaults", []):
        rest = [x for x in dep_terms(cond) if term_key(x) not in dd_keys]
        if all(expr_value(x) > 0 for x in rest):
            return sym
    return choice.syms[0] if choice.syms else None


# ============================================================================
# TREE WALK
# ============================================================================


class Emitter:
    def __init__(self, kconf: Kconfig):
        self.kconf = kconf
        self.out: list[str] = []
        # Stack of currently-open `#if` conditions, as the text following the `#if`.
        # e.g. "FOO" for `#if FOO`
        self.cond_stack: list[str] = []
        # Symbols already written (a symbol may be defined in several nodes).
        self.emitted: set[str] = set()
        # Names referenced in the default conditions of int/hex symbols.  A
        # choice whose members appear here is a *selector* for a computed value
        # (e.g. the SINGLE/DOUBLE/CUSTOM buffer choice backing LV_SDL_BUF_COUNT);
        # the computed int is the real export, so the choice itself is skipped.
        self.selector_members: set[str] = set()
        for s in kconf.unique_defined_syms:
            if s.type not in (INT, HEX):
                continue
            for _, cond in s.defaults:
                _collect_expr_syms(cond, self.selector_members)

    # -- conditional blocks ------------------------------------------------

    def _sync_conditions(self, wanted: list, base_keys: set[str]):
        """Open/close `#if` blocks so the open set matches *wanted*.

        *wanted* is the list of dependency terms that gate the current node;
        *base_keys* are the terms already implied by the enclosing group menu
        (those are never emitted as `#if`, they're the section itself).
        """
        # A `Choice` appearing in a dependency (a member's implicit dep on its
        # choice) is not a C-expressible condition - drop it.  The choice's own
        # `depends on` (e.g. LV_USE_THORVG) stays and becomes the real `#if`.
        wanted = [t for t in wanted if not isinstance(t, Choice)]
        # Drop terms implied by the section, and de-duplicate (a symbol may be
        # both inside `if X` and carry an explicit `depends on X`).
        deduped = []
        seen = set(base_keys)
        for t in wanted:
            k = term_key(t)
            if k not in seen:
                seen.add(k)
                deduped.append(t)
        wanted = deduped
        wanted_keys = [term_key(t) for t in wanted]

        # Close any open conditions that are no longer wanted (from the top).
        popped = False
        while self.cond_stack and self.cond_stack[-1] not in wanted_keys:
            text = self.cond_stack.pop()
            self.out.append(f"#endif /*{text}*/")
            popped = True

        if popped:
            self.out.append("")

        # Open the conditions that are wanted but not yet open.
        for term in wanted:
            choice_name = None
            operator = None
            k = term_key(term)

            if isinstance(term, tuple) and (
                term[0] == NOT and term[1].choice is not None
            ):
                # depends on !FOO
                # where FOO is part of a choice config
                # we need to export #if CHOICE != XXX
                sym = term[1]
                assert sym.choice.name
                choice_name = sym.choice.name
                assert k[0] == "!"
                k = k[1:]
                operator = "!="
            elif (
                isinstance(term, Symbol)
                and term.choice is not None
                and term.choice.name
            ):
                # depends on FOO
                # where FOO is part of a choice config
                # we need to export #if CHOICE == XXX
                choice_name = term.choice.name
                operator = "=="
            if k == "LV_USE_BUILTIN_MALLOC":
                print("LV_USE_BUILTIN_MALLOC")

            if k not in self.cond_stack:
                condition = k
                if choice_name:
                    condition = f"{choice_name} {operator} {k}"

                self.out.append(f"#if {condition}")
                self.cond_stack.append(condition)

    def _close_all_conditions(self):
        while self.cond_stack:
            cond = self.cond_stack.pop()
            self.out.append(f"#endif /*{cond}*/")
        self.out.append("")

    # -- symbol / choice emission -----------------------------------------

    def _emit_define(self, node, name, value, base_keys, doc=None):
        """Render one `#define`, opening/closing `#if` blocks as needed.

        Note: callers handle IGNORE_SYMBOLS.  It is *not* checked here because
        ENUM_CHOICES deliberately emits a macro (e.g. LV_LOG_LEVEL) whose name
        is ignored as a plain symbol (the numeric int-alias)."""
        if name in self.emitted:
            return
        self.emitted.add(name)
        self._sync_conditions(dep_terms(node.dep), base_keys)
        if doc is None:
            doc = node.help or (node.prompt[0] if node.prompt else "")
        if doc:
            self.out += c_comment(doc, "")
        self.out.append(f"#define {name} {value}")
        self.out.append("")

    @staticmethod
    def _choice_doc(node, members, tokenmap):
        """Doc comment for an enum choice: its prompt/help plus the list of
        selectable values, each annotated with the member's description."""
        head = node.help or (node.prompt[0] if node.prompt else "")
        lines = [head] if head else []
        lines.append("Possible values:")
        for s in members:
            token = tokenmap.get(s.name, s.name) if tokenmap else s.name
            prompt = s.nodes[0].prompt[0] if (s.nodes and s.nodes[0].prompt) else ""
            desc = re.sub(r"^\s*\d+\s*:\s*", "", prompt).strip()  # drop "0: " prefix
            norm = lambda x: re.sub(r"[^a-z0-9]", "", x.lower())
            # Drop a leading "PREFIX:" that merely restates the token name
            # (e.g. log-level prompts "LOG_LEVEL_TRACE: A lot of logs ...").
            pre = re.match(r"^([^:]+):\s*(.*)$", desc)
            if pre and norm(pre.group(1)) in norm(token):
                desc = pre.group(2).strip()
            if desc and norm(desc) not in norm(token) and norm(token) not in norm(desc):
                lines.append(f"- {token}: {desc}")
            else:
                lines.append(f"- {token}")
        return "\n".join(lines)

    def emit_symbol(self, node, base_keys: set[str]):
        sym: Symbol = node.item
        if sym.name in IGNORE_SYMBOLS:
            return
        # Choice members are emitted as part of the choice itself.
        if sym.choice is not None:
            return
        # Ignore deprecated configs
        if node.help is not None and node.help.startswith("Deprecated"):
            return
        self._emit_define(node, sym.name, define_value(sym), base_keys)

    def emit_choice(self, node, base_keys: set[str]):
        choice: Choice = node.item
        members = list(choice.syms)
        member_names = frozenset(s.name for s in members)

        # 1) Explicit enum mapping (member names differ from the enum tokens).
        enum = ENUM_CHOICES.get(member_names)
        if enum:
            macro, tokenmap = enum
            sel = choice_default(choice)
            token = sel.name if sel else "0"
            if sel and tokenmap:
                token = tokenmap.get(sel.name, sel.name)
            doc = self._choice_doc(node, members, tokenmap)
            self._emit_define(node, macro, token, base_keys, doc=doc)
            return

        # 2) A value-alias symbol with the choice's name carries the value
        #    (e.g. the int `LV_COLOR_DEPTH`); let that symbol emit instead.
        if choice.name and choice.name in self.kconf.syms:
            return

        # 3) Named choice whose members are already the enum tokens
        #    (the `LV_USE_OS LV_OS_NONE` pattern).
        if choice.name:
            sel = choice_default(choice)
            doc = self._choice_doc(node, members, None)
            self._emit_define(
                node, choice.name, sel.name if sel else "0", base_keys, doc=doc
            )
            return

        # 4) Selector choice backing a computed int (e.g. the SINGLE/DOUBLE/
        #    CUSTOM buffer choice -> LV_SDL_BUF_COUNT): the computed int is the
        #    export, so emit nothing for the choice or its members.
        if member_names & self.selector_members:
            return

        # 5) Anonymous choice with no mapping: emit each member as a 0/1 bool
        #    (the `LV_USE_THORVG_INTERNAL` / `LV_USE_LZ4_INTERNAL` pattern).
        sel = choice.selection
        m = node.list
        while m:
            if isinstance(m.item, Symbol) and m.item.name not in IGNORE_SYMBOLS:
                value = "1" if m.item is sel else "0"
                self._emit_define(m, m.item.name, value, base_keys)
            m = m.next

    # -- recursion ---------------------------------------------------------

    def walk(self, node, base_keys: set[str]):
        while node:
            item = node.item
            if item is MENU or item is COMMENT:
                # Nested menu: recurse, keeping the same section banner.
                if node.list:
                    self.walk(node.list, base_keys)
            elif isinstance(item, Symbol):
                self.emit_symbol(node, base_keys)
                # Options inside `if THIS_SYM` are nested under the symbol by
                # kconfiglib; recurse so they're emitted (wrapped in `#if`).
                if node.list:
                    self.walk(node.list, base_keys)
            elif isinstance(item, Choice):
                self.emit_choice(node, base_keys)
                if node.list:
                    self.walk(node.list, base_keys)
            node = node.next

    def emit_group(self, menu_node: MenuNode):
        title = menu_node.prompt[0] if menu_node.prompt else ""
        base_keys = {term_key(t) for t in dep_terms(menu_node.dep)}
        banner = "/*" + "=" * 76 + "\n" f" * {title.upper()}\n" " *" + "=" * 76 + "*/"
        self.out.append(banner)
        self.out.append("")
        if menu_node.list:
            self.walk(menu_node.list, base_keys)
        self._close_all_conditions()
        self.out.append("")


# ============================================================================
# DRIVER
# ============================================================================


def collect_groups(kconf: Kconfig) -> dict[str, MenuNode]:
    """Return {title: menu_node} for the top-level menus under the root."""
    groups = {}
    # The root "LVGL" menu is the single child of top_node.
    root = kconf.top_node.list
    # If everything is wrapped in one outer "LVGL" menu, descend into it.
    if root and root.item is MENU and root.next is None:
        root = root.list
    node = root
    while node:
        if node.item is MENU and node.prompt:
            groups[node.prompt[0]] = node
        node = node.next
    return groups


# ============================================================================
# lv_conf_template.h Generation
# ============================================================================

HEADER = """\
/**
 * @file lv_conf.h
 * Configuration file for v{ver}
 */

/*
 * Copy this file as `lv_conf.h`
 * 1. simply next to `lvgl` folder
 * 2. or to any other place and
 *    - define `LV_CONF_INCLUDE_SIMPLE`;
 *    - add the path as an include path.
 */

/* clang-format off */
#if 0 /* Set this to "1" to enable content */

#ifndef LV_CONF_H
#define LV_CONF_H

/* If you need to include anything here, do it inside the `__ASSEMBLY__` guard */
#if 0 && defined(__ASSEMBLY__)
#include "my_include.h"
#endif
"""

FOOTER = """\
/*--END OF LV_CONF_H--*/

#endif /*LV_CONF_H*/

#endif /*End of "Content enable"*/
"""


# ============================================================================
# lv_conf_internal.h Generation
# ============================================================================

INTERNAL_PREAMBLE = """\
/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_internal.h
 * This file ensures all defines of lv_conf.h have a default value.
 */

#ifndef LV_CONF_INTERNAL_H
#define LV_CONF_INTERNAL_H
/* clang-format off */

/* Config options */
__CONFIG_OPTIONS__

/** Handle special Kconfig options. */
#ifndef LV_KCONFIG_IGNORE
    #include "lv_conf_kconfig.h"
    #if defined(CONFIG_LV_CONF_SKIP) && !defined(LV_CONF_SKIP)
        #define LV_CONF_SKIP
    #endif
#endif

/* If "lv_conf.h" is available from here try to use it later. */
#ifdef __has_include
    #if __has_include("lv_conf.h")
        #ifndef LV_CONF_INCLUDE_SIMPLE
            #define LV_CONF_INCLUDE_SIMPLE
        #endif
    #endif
#endif

/* If lv_conf.h is not skipped, include it. */
#if !defined(LV_CONF_SKIP) || defined(LV_CONF_PATH)
    #ifdef LV_CONF_PATH                           /* If there is a path defined for lv_conf.h, use it */
        #include LV_CONF_PATH                     /* Note: Make sure to define custom CONF_PATH as a string */
    #elif defined(LV_CONF_INCLUDE_SIMPLE)         /* Or simply include lv_conf.h is enabled. */
        #include "lv_conf.h"
    #else
        #include "../../../../lv_conf.h"                /* Else assume lv_conf.h is next to the lvgl folder. */
    #endif
    #if !defined(LV_CONF_H) && !defined(LV_CONF_SUPPRESS_DEFINE_CHECK)
        /* #include will sometimes silently fail when __has_include is used */
        /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80753 */
        #pragma message("Possible failure to include lv_conf.h, please read the comment in this file if you get errors")
    #endif
#endif

#ifdef CONFIG_LV_COLOR_DEPTH
    #define LV_KCONFIG_PRESENT
#endif

/*----------------------------------
 * Start parsing lv_conf_template.h
 -----------------------------------*/
"""

INTERNAL_FOOTER = r"""

/*----------------------------------
 * End of parsing lv_conf_template.h
 -----------------------------------*/

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TICK_INC
#endif

#ifndef LV_ATTRIBUTE_TIMER_HANDLER
#define LV_ATTRIBUTE_TIMER_HANDLER
#endif

#ifndef LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_FLUSH_READY
#endif

#ifndef LV_ATTRIBUTE_SYNC_READY
#define LV_ATTRIBUTE_SYNC_READY
#endif

#ifndef LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_CONST
#endif

#ifndef LV_ATTRIBUTE_LARGE_RAM_ARRAY
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY
#endif

#ifndef LV_ATTRIBUTE_FAST_MEM
#define LV_ATTRIBUTE_FAST_MEM
#endif

#ifndef LV_ATTRIBUTE_EXTERN_DATA
#define LV_ATTRIBUTE_EXTERN_DATA
#endif

#ifndef LV_EXPORT_CONST_INT
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning
#endif

/*Fix inconsistent name*/
#define LV_USE_ANIMIMAGE LV_USE_ANIMIMG

#ifndef __ASSEMBLY__
LV_EXPORT_CONST_INT(LV_DPI_DEF);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_STRIDE_ALIGN);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_ALIGN);
#endif

#undef LV_KCONFIG_PRESENT

/* Disable VGLite drivers if VGLite drawing is disabled */
#ifndef LV_USE_VG_LITE_DRIVER
    #define LV_USE_VG_LITE_DRIVER 0
#endif

#ifndef LV_USE_VG_LITE_THORVG
    #define LV_USE_VG_LITE_THORVG 0
#endif

#ifndef LV_NEMA_USE_CACHE
    #define LV_NEMA_USE_CACHE 0
#endif

/* Set some defines if a dependency is disabled. */
#if LV_USE_LOG == 0
    #define LV_LOG_LEVEL            LV_LOG_LEVEL_NONE
    #define LV_LOG_TRACE_MEM        0
    #define LV_LOG_TRACE_TIMER      0
    #define LV_LOG_TRACE_INDEV      0
    #define LV_LOG_TRACE_DISP_REFR  0
    #define LV_LOG_TRACE_EVENT      0
    #define LV_LOG_TRACE_OBJ_CREATE 0
    #define LV_LOG_TRACE_LAYOUT     0
    #define LV_LOG_TRACE_ANIM       0
#endif  /*LV_USE_LOG*/

#if LV_USE_WAYLAND
    /* Backend is selected explicitly (Kconfig choice or lv_conf.h).  EGL/G2D are
     * resolved first; SHM is the fallback only when neither is selected, so the
     * three remain mutually exclusive. */
    #ifndef LV_WAYLAND_USE_EGL
        #ifdef CONFIG_LV_WAYLAND_USE_EGL
            #define LV_WAYLAND_USE_EGL CONFIG_LV_WAYLAND_USE_EGL
        #else
            #define LV_WAYLAND_USE_EGL 0
        #endif
    #endif
    #ifndef LV_WAYLAND_USE_G2D
        #ifdef CONFIG_LV_WAYLAND_USE_G2D
            #define LV_WAYLAND_USE_G2D CONFIG_LV_WAYLAND_USE_G2D
        #else
            #define LV_WAYLAND_USE_G2D 0
        #endif
    #endif
    #ifndef LV_WAYLAND_USE_SHM
        #ifdef CONFIG_LV_WAYLAND_USE_SHM
            #define LV_WAYLAND_USE_SHM CONFIG_LV_WAYLAND_USE_SHM
        #elif LV_WAYLAND_USE_EGL || LV_WAYLAND_USE_G2D
            #define LV_WAYLAND_USE_SHM 0
        #else
            #define LV_WAYLAND_USE_SHM 1
        #endif
    #endif
#else
    #define LV_WAYLAND_USE_SHM 0
    #define LV_WAYLAND_USE_EGL 0
    #define LV_WAYLAND_USE_G2D 0
#endif

#if LV_USE_LINUX_DRM
    #if LV_USE_OPENGLES
        #define LV_LINUX_DRM_USE_EGL 1
    #else
        #define LV_LINUX_DRM_USE_EGL 0
    #endif /* LV_USE_OPENGLES */
#else
    #define LV_LINUX_DRM_USE_EGL 0
#endif /*LV_USE_LINUX_DRM*/

#if LV_USE_SYSMON == 0
    #define LV_USE_PERF_MONITOR 0
    #define LV_USE_MEM_MONITOR 0
    #define LV_SYSMON_PROC_IDLE_AVAILABLE 0
#endif /*LV_USE_SYSMON*/

#if LV_USE_PERF_MONITOR == 0
    #define LV_USE_PERF_MONITOR_LOG_MODE 0
#endif /*LV_USE_PERF_MONITOR*/

#if LV_BUILD_DEMOS == 0
    #define LV_USE_DEMO_WIDGETS 0
    #define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
    #define LV_USE_DEMO_BENCHMARK 0
    #define LV_USE_DEMO_RENDER 0
    #define LV_USE_DEMO_STRESS 0
    #define LV_USE_DEMO_MUSIC 0
    #define LV_USE_DEMO_VECTOR_GRAPHIC  0
    #define LV_USE_DEMO_FLEX_LAYOUT     0
    #define LV_USE_DEMO_MULTILANG       0
    #define LV_USE_DEMO_EBIKE           0
    #define LV_USE_DEMO_HIGH_RES        0
    #define LV_USE_DEMO_SMARTWATCH      0
    #define LV_USE_DEMO_GLTF            0
#endif /* LV_BUILD_DEMOS */

#ifndef LV_USE_LZ4
    #if (LV_USE_LZ4_INTERNAL || LV_USE_LZ4_EXTERNAL)
        #define LV_USE_LZ4 1
    #else
        #define LV_USE_LZ4 0
    #endif
#endif

#ifndef LV_USE_THORVG
    #if (LV_USE_THORVG_INTERNAL || LV_USE_THORVG_EXTERNAL)
        #define LV_USE_THORVG 1
    #else
        #define LV_USE_THORVG 0
    #endif
#endif

#if LV_USE_SDL && LV_USE_OPENGLES && (LV_USE_DRAW_OPENGLES || LV_USE_DRAW_NANOVG)
    #define LV_SDL_USE_EGL 1
#else
    #define LV_SDL_USE_EGL 0
#endif

#ifndef LV_USE_EGL
    #if LV_LINUX_DRM_USE_EGL || LV_WAYLAND_USE_EGL || LV_SDL_USE_EGL
        #define LV_USE_EGL 1
    #else
        #define LV_USE_EGL 0
    #endif
#endif /* LV_USE_EGL */


#if LV_USE_OS
    #if (LV_USE_FREETYPE || LV_USE_THORVG) && LV_DRAW_THREAD_STACK_SIZE < (32 * 1024)
        #error "Increase LV_DRAW_THREAD_STACK_SIZE to at least 32KB for FreeType or ThorVG."
    #endif

    #if defined(LV_DRAW_THREAD_STACKSIZE) && !defined(LV_DRAW_THREAD_STACK_SIZE)
        #warning "LV_DRAW_THREAD_STACKSIZE was renamed to LV_DRAW_THREAD_STACK_SIZE. Please update lv_conf.h or run menuconfig again."
        #define LV_DRAW_THREAD_STACK_SIZE LV_DRAW_THREAD_STACKSIZE
    #endif
#endif

/*Allow only upper case letters and '/'  ('/' is a special case for backward compatibility)*/
#define LV_FS_IS_VALID_LETTER(l) ((l) == '/' || ((l) >= 'A' && (l) <= 'Z'))

/* If running without lv_conf.h, add typedefs with default value. */
#ifdef LV_CONF_SKIP
    #if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /*Disable warnings for Visual Studio*/
        #define _CRT_SECURE_NO_WARNINGS
    #endif
#endif  /*defined(LV_CONF_SKIP)*/

#ifndef LV_CHECK_ARG_LOG_MODE
    #define LV_CHECK_ARG_LOG_MODE   0
#endif

#endif  /*LV_CONF_INTERNAL_H*/
"""


def template_to_internal(template_text: str) -> str:
    """Transform generated lv_conf_template.h text into lv_conf_internal.h."""
    out = [INTERNAL_PREAMBLE.replace("__CONFIG_OPTIONS__", render_config_options())]
    started = False
    for line in template_text.splitlines():
        if not started:
            if "#define LV_CONF_H" in line:
                started = True
            continue
        if "/*--END OF LV_CONF_H--*/" in line:
            break

        m = re.match(r"^(\s*)#\s*(?:undef|define)\s+(\S+).*$", line)
        if m:
            indent = m.group(1)
            lhs = m.group(2)  # keeps macro params
            name = re.sub(r"\(.*?\)", "", lhs, count=1)  # bare name for guards
            body = line.lstrip()
            upper = name.upper()
            # A default value of 1 needs the extra LV_KCONFIG_PRESENT layer: an
            # unset Kconfig bool must read as 0, not the LVGL default of 1.
            is_one = re.search(r"#\s*define\s+\S+\s+1(\s*$|\s+)", body)
            if is_one:
                out.append(
                    f"{indent}#ifndef {name}\n"
                    f"{indent}    #ifdef LV_KCONFIG_PRESENT\n"
                    f"{indent}        #ifdef CONFIG_{upper}\n"
                    f"{indent}            #define {lhs} CONFIG_{upper}\n"
                    f"{indent}        #else\n"
                    f"{indent}            #define {lhs} 0\n"
                    f"{indent}        #endif\n"
                    f"{indent}    #else\n"
                    f"{indent}        {body}\n"
                    f"{indent}    #endif\n"
                    f"{indent}#endif\n"
                )
            else:
                out.append(
                    f"{indent}#ifndef {name}\n"
                    f"{indent}    #ifdef CONFIG_{upper}\n"
                    f"{indent}        #define {lhs} CONFIG_{upper}\n"
                    f"{indent}    #else\n"
                    f"{indent}        {body}\n"
                    f"{indent}    #endif\n"
                    f"{indent}#endif\n"
                )
        elif re.match(r"^ *typedef .*;.*$", line):
            continue  # ignore typedefs to avoid redeclaration
        else:
            out.append(line + "\n")

    out.append(INTERNAL_FOOTER)
    return "".join(out)


# ============================================================================
# lv_conf_kconfig.h  (the CONFIG_*  ->  enum/token bridge)
# ============================================================================

KCONFIG_BRIDGE_PREAMBLE = """\
/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_kconfig.h
 * Configs that need special handling when LVGL is used with Kconfig
 */

#ifndef LV_CONF_KCONFIG_H
#define LV_CONF_KCONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LV_CONF_KCONFIG_EXTERNAL_INCLUDE
#include LV_CONF_KCONFIG_EXTERNAL_INCLUDE
#else

#ifdef ESP_PLATFORM
#include "sdkconfig.h"
#include "esp_attr.h"
#endif

#ifdef __NuttX__
#include <nuttx/config.h>
/*
 * Make sure version number in Kconfig file is correctly set.
 * Mismatch can happen when user manually copy lvgl/Kconfig file to their project, like what NuttX does.
 */
#include "../lv_version.h"

#if CONFIG_LVGL_VERSION_MAJOR != LVGL_VERSION_MAJOR || CONFIG_LVGL_VERSION_MINOR != LVGL_VERSION_MINOR \\
        || CONFIG_LVGL_VERSION_PATCH != LVGL_VERSION_PATCH
#warning "Version mismatch between Kconfig and lvgl/lv_version.h"
#endif
#elif defined(__RTTHREAD__)
#define LV_CONF_INCLUDE_SIMPLE
#include <lv_rt_thread_conf.h>
#endif

#endif /*LV_CONF_KCONFIG_EXTERNAL_INCLUDE*/

"""

# Deprecated *_KILOBYTES options: not Kconfig choices, kept verbatim.
KCONFIG_BRIDGE_DEPRECATIONS = """\

/*******************
 * LV_MEM_SIZE
 *******************/

#ifdef CONFIG_LV_MEM_SIZE_KILOBYTES
#warning "LV_MEM_SIZE_KILOBYTES is deprecated, use LV_MEM_SIZE instead (value in bytes)"
#ifndef CONFIG_LV_MEM_SIZE
#define CONFIG_LV_MEM_SIZE (LV_MEM_SIZE_KILOBYTES * 1024U)
#else
#warning "Both LV_MEM_SIZE and LV_MEM_SIZE_KILOBYTES are defined. Using LV_MEM_SIZE"
#endif
#undef CONFIG_LV_MEM_SIZE_KILOBYTES
#endif

#ifdef CONFIG_LV_MEM_POOL_EXPAND_SIZE_KILOBYTES
#warning "LV_MEM_POOL_EXPAND_SIZE_KILOBYTES is deprecated, set the full memory size with LV_MEM_SIZE instead (value in bytes)"
#define CONFIG_LV_MEM_POOL_EXPAND_SIZE (CONFIG_LV_MEM_POOL_EXPAND_SIZE_KILOBYTES * 1024U)
#endif
"""

KCONFIG_BRIDGE_FOOTER = """\

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_CONF_KCONFIG_H*/
"""


def kconfig_bridge_body(kconf: Kconfig) -> str:
    """Render the CONFIG_<member> -> CONFIG_<macro> <token> ladders.

    Only enum-valued choices need a bridge: ENUM_CHOICES entries (member names
    differ from the tokens) and named choices whose members already are the
    tokens.  Value-alias int symbols (LV_COLOR_DEPTH), anonymous 0/1 choices
    (THORVG, LZ4, Wayland) and computed-int selectors (buffer counts) need none
    -- this mirrors emit_choice()'s cases 2/4/5.
    """
    blocks = []
    seen: dict[str, list] = {}
    for choice in kconf.unique_choices:
        members = list(choice.syms)
        member_names = frozenset(s.name for s in members)
        enum = ENUM_CHOICES.get(member_names)
        if enum:
            macro, tokenmap = enum
            pairs = [
                (s.name, tokenmap.get(s.name, s.name) if tokenmap else s.name)
                for s in members
            ]
        elif choice.name and choice.name in kconf.syms:
            continue  # value-alias int symbol
        elif choice.name:
            macro = choice.name
            pairs = [(s.name, s.name) for s in members]
        else:
            continue  # anonymous bools / computed-int selector

        if macro in seen:
            raise SystemExit(
                f"duplicate bridge macro {macro}: members {[p[0] for p in pairs]} "
                f"collide with {seen[macro]} -- check for duplicate Kconfig choices"
            )
        seen[macro] = [p[0] for p in pairs]

        lines = ["/*******************", f" * {macro}", " *******************/"]
        for i, (member, token) in enumerate(pairs):
            guard = (
                f"#ifdef CONFIG_{member}"
                if i == 0
                else f"#elif defined(CONFIG_{member})"
            )
            lines.append(guard)
            lines.append(f"#  define CONFIG_{macro} {token}")
        lines.append("#endif")
        blocks.append("\n".join(lines))
    return "\n\n".join(blocks)


def generate_kconfig_bridge(kconf: Kconfig) -> str:
    return (
        KCONFIG_BRIDGE_PREAMBLE
        + kconfig_bridge_body(kconf)
        + "\n"
        + KCONFIG_BRIDGE_DEPRECATIONS
        + KCONFIG_BRIDGE_FOOTER
    )


def version_string(kconf: Kconfig) -> str:
    def val(name, default):
        s = kconf.syms.get(name)
        return s.str_value if s else default

    return f"{val('LVGL_VERSION_MAJOR', '9')}.{val('LVGL_VERSION_MINOR', '0')}.{val('LVGL_VERSION_PATCH', '0')}"


def main():
    ap = argparse.ArgumentParser(allow_abbrev=False)
    ap.add_argument(
        "kconfig",
        nargs="?",
        default="Kconfig",
        help="Top-level Kconfig file (default: Kconfig)",
    )
    ap.add_argument("--template", help="Write lv_conf_template.h to this path")
    ap.add_argument("--internal", help="Write lv_conf_internal.h to this path")
    ap.add_argument("--bridge", help="Write lv_conf_kconfig.h to this path")
    ap.add_argument("--srctree", help="Source root for resolving sources")
    args = ap.parse_args()

    srctree = args.srctree or os.path.dirname(os.path.abspath(args.kconfig)) or "."
    os.environ["srctree"] = srctree

    kconf = Kconfig(args.kconfig, warn_to_stderr=True, suppress_traceback=False)

    groups = collect_groups(kconf)
    em = Emitter(kconf)
    for group in groups.values():
        em.emit_group(group)

    body = "\n".join(em.out)
    text = HEADER.format(ver=version_string(kconf)) + "\n" + body + "\n" + FOOTER

    if args.internal:
        with open(args.internal, "w") as f:
            f.write(template_to_internal(text))
        print(f"wrote {args.internal}", file=sys.stderr)

    if args.template:
        with open(args.template, "w") as f:
            f.write(text)
        print(f"wrote {args.template}", file=sys.stderr)

    if args.bridge:
        with open(args.bridge, "w") as f:
            f.write(generate_kconfig_bridge(kconf))
        print(f"wrote {args.bridge}", file=sys.stderr)

    if not args.template and not args.internal and not args.bridge:
        sys.stdout.write(text)


if __name__ == "__main__":
    try:
        main()
    except Exception:
        import pdb, traceback

        traceback.print_exc()  # shows the real traceback
        pdb.post_mortem()  # drops into pdb AT the crash site
