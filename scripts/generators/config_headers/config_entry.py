"""Typed representation of a single configuration option.

Each ``ConfigEntry`` subclass corresponds to one *kind* of config (a plain
bool, a single-select enum, an internal capability flag, ...).  A subclass owns
the knowledge of how that kind renders onto each of the four output surfaces:

* :meth:`emit_template`         - the public ``lv_conf_template.h`` body.
* :meth:`emit_internal`         - the ``lv_conf_internal.h`` default ladder.
* :meth:`emit_internal_options` - the constant/"Config options" block emitted
  near the top of ``lv_conf_internal.h`` (enum tokens, font pointers, ...).
* :meth:`emit_kconfig`          - the ``lv_conf_kconfig.h`` ``CONFIG_*`` bridge.

Each method returns a list of lines (no trailing newline).  The base class
returns ``[]`` for every surface, so a subclass only overrides what is special
to it.
"""

from __future__ import annotations

from .kconfig_utils import bool_default, c_comment, doc_text, scalar_default


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

    # -- the four output surfaces -----------------------------------------
    # Defaults: emit nothing.  Subclasses override only what applies to them.

    def emit_template(self) -> list[str]:
        return []

    def emit_internal(self) -> list[str]:
        return []

    def emit_internal_options(self) -> list[str]:
        return []

    def emit_kconfig(self) -> list[str]:
        return []

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
        return c_comment(self.doc) + [f"#define {self.name} {self.value}"]

    def emit_internal(self) -> list[str]:
        return _plain_ladder(self.name, self.value)


class BoolConfig(ScalarConfig):
    """A plain on/off option, e.g. ``LV_USE_WAYLAND``.

    A default of ``1`` needs the extra ``LV_KCONFIG_PRESENT`` layer: when
    Kconfig is in use an *unset* bool emits no ``CONFIG_<name>`` at all and so
    must read as ``0``, not the LVGL default of ``1``.  A default-``0`` bool
    has no such ambiguity (absent macro means ``0`` on both paths) and uses the
    plain ladder.
    """

    def __init__(self, name: str, value: str, *, node=None, doc: str = ""):
        assert value in ("0", "1")
        super().__init__(name, value, node=node, doc=doc)

    @classmethod
    def from_symbol(cls, sym, node) -> "BoolConfig":
        return cls(sym.name, bool_default(sym), node=node, doc=doc_text(node))

    def emit_internal(self) -> list[str]:
        if self.value != "1":
            return _plain_ladder(self.name, self.value)
        name = self.name
        upper = name.upper()
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


# def _join_blocks(blocks: list[list[str]]) -> list[str]:
#     """Concatenate line-blocks, separating non-empty ones with a blank line."""
#     out: list[str] = []
#     for block in blocks:
#         if not block:
#             continue
#         if out:
#             out.append("")
#         out += block
#     return out
#
#
# class BoolGroupChoice(ConfigEntry):
#     """An anonymous ``choice`` of mutually-exclusive bool members, emitted as
#     one ``0/1`` define per member (the THORVG / LZ4 ``INTERNAL`` vs
#     ``EXTERNAL`` pattern).
#
#     There is no enum macro or token - each member is an ordinary
#     :class:`BoolConfig`, with the selected default at ``1`` and the rest at
#     ``0`` (so the selected member reuses the ``LV_KCONFIG_PRESENT`` ladder).
#     On the Kconfig path ``autoconf`` emits each member directly, so
#     :meth:`emit_kconfig` is empty.
#     """
#
#     def __init__(self, name: str, members: list["BoolConfig"], *, node=None):
#         super().__init__(name, node=node)
#         self.members = members
#
#     def emit_template(self) -> list[str]:
#         return _join_blocks([m.emit_template() for m in self.members])
#
#     def emit_internal(self) -> list[str]:
#         return _join_blocks([m.emit_internal() for m in self.members])
#


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
        return c_comment(self.doc) + [f"#define {self.name} {self.selected_token}"]

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
