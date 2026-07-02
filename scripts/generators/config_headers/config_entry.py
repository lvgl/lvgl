"""Typed representation of a single configuration option.

Each ``ConfigEntry`` subclass corresponds to one *kind* of config (a plain
bool, a single-select enum, an internal capability flag, ...).  A subclass owns
the knowledge of how that kind renders onto each of the four output targets:

* :meth:`emit_template`         - the public ``lv_conf_template.h`` body.
* :meth:`emit_internal`         - the ``lv_conf_internal.h`` default ladder.
* :meth:`emit_internal_options` - the constant/"Config options" block emitted
  near the top of ``lv_conf_internal.h`` (enum tokens, font pointers, ...).
* :meth:`emit_kconfig`          - the ``lv_conf_kconfig.h`` ``CONFIG_*`` bridge.

Each method returns a list of lines (no trailing newline).  The base class
returns ``[]`` for every target, so a subclass only overrides what is special
to it.
"""

from __future__ import annotations

from .kconfig_utils import (
    bool_default,
    c_comment,
    doc_text,
    rev_dep_c_expr,
    scalar_default,
    select_lines,
)


class ConfigEntry:
    """Base class for every kind of configuration option.

    Parameters
    ----------
    name:
        The macro name, e.g. ``LV_USE_WAYLAND``.
    node:
        The originating kconfiglib ``MenuNode`` (kept for diagnostics and for
        subclasses that need to inspect dependencies/defaults later).
    doc:
        Documentation text for the option (help, falling back to prompt).
    """

    def __init__(self, name: str, *, node=None, doc: str = ""):
        self.name = name
        self.node = node
        self.doc = doc

    # -- the four output targets -----------------------------------------

    def emit_template(self) -> list[str]:
        return []

    def emit_internal(self) -> list[str]:
        return []

    def emit_internal_options(self) -> list[str]:
        return []

    def emit_kconfig(self) -> list[str]:
        return []

    def _select_doc(self) -> str:
        """Comment suffix documenting this option's Kconfig selects
        (``Enable: ...``), or ``""`` if it has none."""
        return select_lines(self.node.item) if self.node is not None else ""

    def depends_on_c(self) -> str | None:
        """C condition from this option's `depends on`, or None if unconditional."""
        if self.node is None:
            return None
        sym = self.node.item
        dep = getattr(sym, "direct_dep", None)
        if dep is None or dep is sym.kconfig.y:
            return None
        return rev_dep_c_expr(dep)

    def __repr__(self) -> str:
        return f"{type(self).__name__}({self.name!r})"


def _plain_ladder(name: str, value: str) -> list[str]:
    """The ``#ifndef / #ifdef CONFIG_<name>`` default ladder.

    Used when an absent ``CONFIG_<name>`` should fall back to LVGL's default -
    correct for every scalar except a default-``1`` bool (see
    :meth:`BoolConfig.emit_internal`)."""
    upper = name.upper()
    return [
        f"#ifndef {name}",
        f"    #ifdef CONFIG_{upper}",
        f"        #define {name} CONFIG_{upper}",
        f"    #else",
        f"        #define {name} {value}",
        f"    #endif",
        f"#endif",
    ]


class ScalarConfig(ConfigEntry):
    """A single ``#define <name> <value>`` option carrying one C literal value.

    Base for bool / int / hex / string options.  ``value`` is already the
    rendered C literal (``0``, ``33``, ``0x40``, ``" ,.;"``).  Subclasses
    differ only in how that literal is derived and, for default-``1`` bools, in
    the internal-header ladder.
    """

    def __init__(self, name: str, value: str, *, node=None, doc: str = ""):
        super().__init__(name, node=node, doc=doc)
        self.value = value

    def emit_template(self) -> list[str]:
        return c_comment(self.doc + self._select_doc()) + [
            f"#define {self.name} {self.value}"
        ]

    def emit_internal(self) -> list[str]:
        return _plain_ladder(self.name, self.value)


class BoolConfig(ScalarConfig):
    """A plain on/off option, e.g. ``LV_USE_WAYLAND``."""

    def __init__(self, name: str, value: str, *, node=None, doc: str = ""):
        assert value in ("0", "1")
        super().__init__(name, value, node=node, doc=doc)

    @classmethod
    def from_symbol(cls, sym, node) -> "BoolConfig":
        return cls(sym.name, bool_default(sym), node=node, doc=doc_text(node))

    def _default_on_ladder(self) -> list[str]:
        """
        Settings enabled by default need some special handling:

        First of all, we need to ensure we only define it to '1' if all of its dependencies
        are met else we could end up with a case where `LV_USE_THORVG_INTERNAL` is enabled
        while `LV_USE_THORVG` is disabled forcing us to check for
        `#if LV_USE_THORVG && LV_USE_THORVG_INTERNAL` in the source code instead of the more
        straightforward way `#if LV_USE_THORVG_INTERNAL`.

        On Kconfig side, even if by default the configuration is enabled, Kconfig won't export a
        CONFIG_XXX macro for disabled configs.
        e.g. disabling LV_USE_LABEL in kconfig will not produce `#define CONFIG_LV_USE_LABEL 0`,
        in fact, it won't produce a #define at all so we need to handle this by checking if we're using
        Kconfig, and if so, we set it to 0 by default.
        """
        name = self.name
        upper = name.upper()
        deps = self.depends_on_c()
        if deps:
            return [
                f"#ifndef {name}",
                f"    #ifdef LV_KCONFIG_PRESENT",
                f"        #ifdef CONFIG_{upper}",
                f"            #define {name} CONFIG_{upper}",
                f"        #else",
                f"            #define {name} 0",
                f"        #endif",
                f"    #else",
                f"          #define {name} {deps}",
                f"    #endif",
                f"#endif",
            ]

        return [
            f"#ifndef {name}",
            f"    #ifdef LV_KCONFIG_PRESENT",
            f"        #ifdef CONFIG_{upper}",
            f"            #define {name} CONFIG_{upper}",
            f"        #else",
            f"            #define {name} 0",
            f"        #endif",
            f"    #else",
            f"        #define {name} 1",
            f"    #endif",
            f"#endif",
        ]

    def emit_internal(self) -> list[str]:
        # Force export as 0 in kconfig if the setting has any dependencies
        if self.value == "0":
            return _plain_ladder(self.name, self.value)
        return self._default_on_ladder()


class IntConfig(ScalarConfig):
    """An int or hex valued option, e.g. ``LV_DEF_REFR_PERIOD`` or a ``hex``
    address.  Ints always emit a ``CONFIG_<name>`` under Kconfig (they always
    have a value), so the plain ladder is always correct."""

    @classmethod
    def from_symbol(cls, sym, node) -> "IntConfig":
        return cls(sym.name, scalar_default(sym), node=node, doc=doc_text(node))


class StringConfig(ScalarConfig):
    """A string option, e.g. ``LV_TXT_BREAK_CHARS``.  The value keeps its
    surrounding quotes as stored in Kconfig."""

    @classmethod
    def from_symbol(cls, sym, node) -> "StringConfig":
        return cls(sym.name, scalar_default(sym), node=node, doc=doc_text(node))


class ConstToken(ConfigEntry):
    """A named integer constant emitted as a bare ``#define <name> <value>`` in
    the internal "Config options" block (e.g. ``LV_STDLIB_BUILTIN`` -> ``0``).

    These are promptless ``int``/``hex`` configs with a single literal default
    (see :func:`kconfig_utils.is_int_const`).  They are plumbing: an
    :class:`EnumChoice` may expand to one of them by *name*
    (``#define LV_USE_STDLIB_MALLOC LV_STDLIB_BUILTIN``), so they must be defined
    ahead of the body, unconditionally - a dependency from an enclosing ``if`` is
    intentionally ignored, the constant is always defined.  They are not
    user-facing, so they appear in neither the public template nor the
    ``CONFIG_`` bridge - only the options block.
    """

    def __init__(self, name: str, value: str, *, node=None):
        super().__init__(name, node=node)
        self.value = value

    def emit_internal_options(self) -> list[str]:
        return [f"#define {self.name} {self.value}"]


class DerivedFlag(ConfigEntry):
    """An internal capability flag set via Kconfig ``select`` (e.g.
    ``LV_DRAW_HAS_VECTOR_SUPPORT``, selected by the vector-capable draw units).

    It is not user-facing, so it never appears in the public template.  In the
    internal header it is *derived* from the same selectors that ``select`` it
    in Kconfig - those operands are plain options defined earlier, so the
    derivation resolves identically whether the build uses Kconfig or a
    hand-written ``lv_conf.h``.  This replaces a hand-maintained block in the
    old ``INTERNAL_FOOTER``.
    """

    def __init__(self, name: str, selectors_expr: str, *, node=None):
        super().__init__(name, node=node)
        self.selectors_expr = selectors_expr

    def emit_internal(self) -> list[str]:
        return [
            f"#ifndef {self.name}",
            f"    #if ({self.selectors_expr})",
            f"        #define {self.name} 1",
            f"    #else",
            f"        #define {self.name} 0",
            f"    #endif",
            f"#endif",
        ]


class ConstraintCheck(ConfigEntry):
    """A compile-time ``#error`` that guards a Kconfig ``depends on`` / ``select``
    constraint on the hand-written ``lv_conf.h`` path.

    Kconfig already enforces these (a `select` auto-enables the target; a
    ``depends on`` makes the option unselectable when unmet), so on the Kconfig
    path the constraint always holds.  But a user editing ``lv_conf.h`` by hand
    can break it - e.g. enable ``LV_USE_LOTTIE`` without ``LV_USE_THORVG``, or
    enable ``LV_USE_SLIDER`` while turning ``LV_USE_BAR`` off.  Rather than
    silently fixing it (which would hide the mistake and could surprise the
    user), we emit an ``#error`` so the build fails with an explanatory message.

    *condition* is the ``#if`` expression that is true exactly when the
    constraint is *violated*; *message* is shown to the user.  Emitted near the
    end of the internal header, where every referenced symbol is defined.
    """

    def __init__(self, name: str, condition: str, message: str, *, node=None):
        super().__init__(name, node=node)
        self.condition = condition
        self.message = message

    def emit_internal(self) -> list[str]:
        return [
            f"#if {self.condition}",
            f'    #error "{self.message}"',
            f"#endif",
        ]


class EnumMember:
    """One selectable value of an :class:`EnumChoice`.

    * ``token`` - what the macro expands to when this member is selected
      (a symbolic name like ``LV_OS_PTHREAD`` / ``LV_STDLIB_BUILTIN``, or a bare
      number like ``16`` for a value-alias such as ``LV_COLOR_DEPTH``).
    * ``value`` - the numeric value the token stands for, or ``None`` for a
      bare-number token (nothing to define) or a header-owned token.
    * ``define`` - whether the token needs a ``#define`` in the internal
      "Config options" block (``False`` for bare numbers and header-owned
      tokens that already live in a C header).
    """

    def __init__(
        self, token: str, value: str | None, define: bool, member_name: str = ""
    ):
        self.token = token
        self.value = value
        self.define = define
        # The Kconfig selector member symbol (e.g. LV_USE_BUILTIN_MALLOC).  Used
        # by the driver to rewrite a `#if <member>` guard - which only resolves
        # on the Kconfig path - into `<macro> == <token>`, valid on both paths.
        self.member_name = member_name


class EnumChoice(ConfigEntry):
    """A single-select option: ``<name>`` expands to one of a fixed set of
    tokens chosen by a Kconfig ``choice`` (the ``LV_USE_OS`` / stdlib /
    ``LV_COLOR_DEPTH`` family).

    Built from the *derived int* config (``config LV_USE_OS / int / default
    <token> if <member>``); the selector ``choice`` and member tokens are
    recovered during classification.  On the Kconfig path the derived int is
    emitted by ``autoconf`` directly, so :meth:`emit_kconfig` is empty.
    """

    def __init__(
        self,
        name: str,
        members: list[EnumMember],
        selected_token: str,
        *,
        options_title: str = "",
        needs_bridge: bool = False,
        node=None,
        doc: str = "",
    ):
        super().__init__(name, node=node, doc=doc)
        self.members = members
        self.selected_token = selected_token
        self.options_title = options_title
        # When built from a *bare* choice (no derived int), autoconf emits only
        # CONFIG_<member>, not CONFIG_<macro> - so lv_conf_kconfig.h needs a
        # bridge ladder.  Derived-int enums get CONFIG_<macro> directly and need
        # none.  (emit_kconfig is wired up when the bridge generator lands.)
        self.needs_bridge = needs_bridge

    def emit_template(self) -> list[str]:
        return c_comment(self.doc + self._select_doc()) + [
            f"#define {self.name} {self.selected_token}"
        ]

    def emit_internal(self) -> list[str]:
        # The derived int always has a value under Kconfig (a choice always
        # selects something), so the plain ladder is correct.
        return _plain_ladder(self.name, self.selected_token)

    def emit_internal_options(self) -> list[str]:
        defs = [(m.token, m.value) for m in self.members if m.define]
        if not defs:
            return []
        width = max(len(tok) for tok, _ in defs)
        out = [f"/* {self.options_title} */"] if self.options_title else []
        out += [f"#define {tok.ljust(width)}   {val}" for tok, val in defs]
        return out

    def emit_kconfig(self) -> list[str]:
        # Only bare choices (no derived int) need a bridge: autoconf emits
        # CONFIG_<member> but not CONFIG_<macro>, so map the selected member to
        # the macro's token.  Derived-int enums get CONFIG_<macro> directly.
        if not self.needs_bridge:
            return []
        out = ["/*******************", f" * {self.name}", " *******************/"]
        for i, m in enumerate(self.members):
            guard = (
                f"#ifdef CONFIG_{m.member_name}"
                if i == 0
                else f"#elif defined(CONFIG_{m.member_name})"
            )
            out.append(guard)
            out.append(f"#  define CONFIG_{self.name} {m.token}")
        out.append("#endif")
        return out
