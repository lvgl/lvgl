"""Unit tests for DerivedConstToken.

A promptless int whose value is fixed by the selected member of a *separately
named* choice (LV_COLOR_DEPTH under LV_COLOR_FORMAT_DEFAULT).  Contrast with the
value-alias EnumChoice in test_enum_choice.py, where the int *is* the choice's
macro and does appear in the template.
"""

import pytest

from config_headers.config_entry import DerivedConstToken, EnumChoice
from config_headers.kconfig_utils import derived_int_const_table
from config_headers.parse import load


def test_parses_as_derived_const_token(entries):
    bpp = entries["LV_TEST_FORMAT_BPP"]
    assert isinstance(bpp, DerivedConstToken)
    assert bpp.selector == "LV_TEST_FORMAT"


def test_named_choice_keeps_its_own_entry(entries):
    # The int must not swallow the choice: LV_TEST_FORMAT is still emitted as
    # the user-facing macro.
    assert isinstance(entries["LV_TEST_FORMAT"], EnumChoice)


def test_table_covers_every_member(entries):
    assert entries["LV_TEST_FORMAT_BPP"].table == [
        ("LV_TEST_FORMAT_I1", "1"),
        ("LV_TEST_FORMAT_RGB565", "16"),
        ("LV_TEST_FORMAT_RGB565_SWAPPED", "16"),
        # covered by the unconditional final default
        ("LV_TEST_FORMAT_XRGB8888", "32"),
    ]


def test_absent_from_template_and_bridge(entries, generated):
    bpp = entries["LV_TEST_FORMAT_BPP"]
    assert bpp.emit_template() == []
    assert bpp.emit_kconfig() == []
    assert "LV_TEST_FORMAT_BPP" not in generated["template"]
    assert "LV_TEST_FORMAT_BPP" not in generated["bridge"]


def test_emit_internal_pastes_the_selector(entries):
    lines = entries["LV_TEST_FORMAT_BPP"].emit_internal()
    assert ["#define", "LV_TEST_FORMAT_BPP_OF_LV_TEST_FORMAT_I1", "1"] in [
        ln.split() for ln in lines
    ]
    assert lines[-7:] == [
        "#ifndef LV_TEST_FORMAT_BPP",
        "    #ifdef CONFIG_LV_TEST_FORMAT_BPP",
        "        #define LV_TEST_FORMAT_BPP CONFIG_LV_TEST_FORMAT_BPP",
        "    #else",
        "        #define LV_TEST_FORMAT_BPP LV_CONF_PASTE(LV_TEST_FORMAT_BPP_OF_, LV_TEST_FORMAT)",
        "    #endif",
        "#endif",
    ]


def test_emitted_after_the_compatibility_block(generated):
    internal = generated["internal"]
    assert internal.index("End of compatibility block") < internal.index(
        "#define LV_TEST_FORMAT_BPP_OF_LV_TEST_FORMAT_I1"
    )
    # the paste helper is defined before its first use
    assert internal.index("#define LV_CONF_PASTE(") < internal.index(
        "LV_CONF_PASTE(LV_TEST_FORMAT_BPP_OF_,"
    )


def test_value_alias_enum_choice_is_untouched(entries):
    # LV_COLOR_DEPTH in the fixture is named after its own choice, so it stays an
    # EnumChoice and keeps its place in the template.
    assert isinstance(entries["LV_COLOR_DEPTH"], EnumChoice)


def test_uncovered_member_raises(tmp_path):
    src = tmp_path / "t.kconfig"
    src.write_text(
        'choice LV_X\n\tprompt "x"\n\tdefault LV_X_A\n'
        '\tconfig LV_X_A\n\t\tbool "a"\n'
        '\tconfig LV_X_B\n\t\tbool "b"\n'
        "endchoice\n\n"
        "config LV_X_BPP\n\tint\n\tdefault 1 if LV_X_A\n\tdefault 2 if LV_X_A\n"
    )
    kconf = load(str(src))
    with pytest.raises(ValueError, match="no default covers: LV_X_B"):
        derived_int_const_table(kconf.syms["LV_X_BPP"])


def test_undefined_symbol_in_condition_raises(tmp_path):
    src = tmp_path / "t.kconfig"
    src.write_text(
        'choice LV_Y\n\tprompt "y"\n\tdefault LV_Y_A\n'
        '\tconfig LV_Y_A\n\t\tbool "a"\n'
        '\tconfig LV_Y_B\n\t\tbool "b"\n'
        "endchoice\n\n"
        "config LV_Y_BPP\n\tint\n\tdefault 1 if LV_Y_A\n\tdefault 2 if LV_Y_TYPO\n"
    )
    kconf = load(str(src))
    with pytest.raises(ValueError, match="LV_Y_TYPO, which no Kconfig file defines"):
        derived_int_const_table(kconf.syms["LV_Y_BPP"])
