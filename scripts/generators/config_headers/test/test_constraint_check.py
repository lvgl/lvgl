"""Tests for ConstraintCheck: Kconfig depends/select replayed as #error guards."""

import os

import pytest

from config_headers.config_entry import BoolConfig, ConstraintCheck, DerivedFlag
from config_headers.emit import constraint_checks, generate_internal
from config_headers.parse import load, parse_entries

FIXTURE = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "constraints.kconfig"
)


@pytest.fixture
def kconf():
    return load(FIXTURE)


@pytest.fixture
def entries(kconf):
    return parse_entries(kconf)


@pytest.fixture
def checks(entries):
    return {c.name: c for c in constraint_checks(entries)}


def test_select_emits_error_when_selector_on_but_option_off(checks):
    c = checks["LV_USE_BBB"]
    assert isinstance(c, ConstraintCheck)
    assert c.emit_internal() == [
        "#if (LV_USE_AAA) && !LV_USE_BBB",
        '    #error "LV_USE_BBB must be enabled: Kconfig selects it from LV_USE_AAA"',
        "#endif",
    ]


def test_depends_emits_error_for_default_off_option(checks):
    c = checks["LV_USE_EEE"]
    assert c.condition == "LV_USE_EEE && !(LV_USE_AAA)"
    assert "requires LV_USE_AAA (Kconfig depends on)" in c.message


def test_default_on_option_with_depends_is_not_checked(entries, checks):
    # FFF defaults on (bool_default strips `depends on`), so a depends check
    # would fire under LV_CONF_SKIP - it must be skipped.
    by_name = {e.name: e for e in entries}
    assert isinstance(by_name["LV_USE_FFF"], BoolConfig)
    assert "LV_USE_FFF" not in checks


def test_font_pointer_select_is_excluded(entries, checks):
    # LV_FONT_X is selected by a font-default member whose token is a pointer;
    # `LV_FONT_DEFAULT == <token>` is not preprocessor-evaluable, so no check.
    by_name = {e.name: e for e in entries}
    assert isinstance(by_name["LV_FONT_X"], BoolConfig)
    assert "LV_FONT_X" not in checks


def test_derived_flag_is_not_a_constraint_check(entries, checks):
    by_name = {e.name: e for e in entries}
    assert isinstance(by_name["LV_HAS_CAP"], DerivedFlag)
    assert "LV_HAS_CAP" not in checks


def test_unconstrained_option_has_no_check(entries, checks):
    by_name = {e.name: e for e in entries}
    assert isinstance(by_name["LV_USE_PLAIN"], BoolConfig)
    assert "LV_USE_PLAIN" not in checks


def test_checks_land_before_the_closing_endif(kconf, entries):
    internal = generate_internal(kconf, entries)
    check_pos = internal.index('#error "LV_USE_BBB must be enabled')
    close_pos = internal.index("#endif  /*LV_CONF_INTERNAL_H*/")
    assert check_pos < close_pos


def test_template_comment_lists_user_facing_selects(kconf, entries):
    from config_headers.emit import generate_template

    t = generate_template(kconf, entries)
    # AAA selects BBB (user-facing) -> listed in AAA's comment.
    assert "Enable: LV_USE_BBB" in t
    # `depends on` is not documented in the comment: the template already wraps
    # each option in the matching #if block.
    assert "Depends on:" not in t
    # LV_HAS_CAP is a hidden (no-prompt) flag selected by DDD -> never shown.
    assert "LV_HAS_CAP" not in t
