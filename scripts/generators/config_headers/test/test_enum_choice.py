"""Unit tests for EnumChoice across its three token sources.

* stdlib       - derived int references named int-const tokens (member != token)
* LV_USE_OS    - bare-literal defaults, member == token (via MEMBER_IS_TOKEN)
* LV_COLOR_DEPTH - value-alias, emits the bare number
"""

from config_headers.config_entry import ConstToken, EnumChoice


# -- stdlib: named-const tokens ----------------------------------------------


def test_stdlib_parses_as_enum_choice(entries):
    malloc = entries["LV_USE_STDLIB_MALLOC"]
    assert isinstance(malloc, EnumChoice)
    # Default selection is the built-in implementation -> the named token.
    assert malloc.selected_token == "LV_STDLIB_BUILTIN"


def test_stdlib_emit_template(entries):
    malloc = entries["LV_USE_STDLIB_MALLOC"]
    assert (
        malloc.emit_template()[-1] == "#define LV_USE_STDLIB_MALLOC LV_STDLIB_BUILTIN"
    )


def test_stdlib_tokens_are_const_tokens(entries):
    # The named tokens are their own `int / default N` configs, so they are
    # classified as ConstTokens and defined once in the options block; the choice
    # only *references* them by name and defines nothing itself.
    builtin = entries["LV_STDLIB_BUILTIN"]
    assert isinstance(builtin, ConstToken)
    assert builtin.emit_internal_options() == ["#define LV_STDLIB_BUILTIN 0"]

    malloc = entries["LV_USE_STDLIB_MALLOC"]
    assert malloc.emit_internal_options() == []


def test_stdlib_emit_internal_ladder(entries):
    malloc = entries["LV_USE_STDLIB_MALLOC"]
    assert malloc.emit_internal() == [
        "#ifndef LV_USE_STDLIB_MALLOC",
        "    #ifdef CONFIG_LV_USE_STDLIB_MALLOC",
        "        #define LV_USE_STDLIB_MALLOC CONFIG_LV_USE_STDLIB_MALLOC",
        "    #else",
        "        #define LV_USE_STDLIB_MALLOC LV_STDLIB_BUILTIN",
        "    #endif",
        "#endif",
    ]


# -- LV_USE_OS: member == token ----------------------------------------------


def test_os_parses_as_enum_choice(entries):
    os_cfg = entries["LV_USE_OS"]
    assert isinstance(os_cfg, EnumChoice)
    assert os_cfg.selected_token == "LV_OS_NONE"


def test_os_emit_template_uses_member_name(entries):
    os_cfg = entries["LV_USE_OS"]
    assert os_cfg.emit_template()[-1] == "#define LV_USE_OS LV_OS_NONE"


def test_os_defines_tokens_from_literals(entries):
    os_cfg = entries["LV_USE_OS"]
    assert os_cfg.emit_internal_options() == [
        "/* Default operating system to use */",
        "#define LV_OS_NONE      0",
        "#define LV_OS_PTHREAD   1",
        "#define LV_OS_CUSTOM    255",
    ]


# -- LV_COLOR_DEPTH: value-alias ---------------------------------------------


def test_color_depth_emits_number_not_name(entries):
    depth = entries["LV_COLOR_DEPTH"]
    assert isinstance(depth, EnumChoice)
    assert depth.selected_token == "16"
    assert depth.emit_template()[-1] == "#define LV_COLOR_DEPTH 16"


def test_color_depth_defines_no_tokens(entries):
    # Bare numbers are emitted inline; nothing goes in the options block.
    depth = entries["LV_COLOR_DEPTH"]
    assert depth.emit_internal_options() == []


# -- bridge: derived ints come straight from autoconf ------------------------


def test_enum_emits_nothing_to_kconfig_bridge(entries):
    for name in ("LV_USE_STDLIB_MALLOC", "LV_USE_OS", "LV_COLOR_DEPTH"):
        assert entries[name].emit_kconfig() == []
