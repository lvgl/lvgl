"""Tests for ConstraintCheck: Kconfig depends/select replayed as #error guards.

The guards live in the generated lv_conf_check.c, not in lv_conf_internal.h, so
a config violation is reported once instead of in every translation unit."""

import os

import pytest

from config_headers.config_entry import BoolConfig, ConstraintCheck, DerivedFlag
from config_headers.emit import constraint_checks, generate_checker, generate_internal
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


def test_checks_land_in_the_checker_translation_unit(kconf, entries):
    checker = generate_checker(kconf, entries)
    assert '#error "LV_USE_BBB must be enabled' in checker
    assert '#error "LV_USE_EEE requires LV_USE_AAA' in checker
    # The checker is a .c file: it pulls the config in through the public header
    # and carries no include guard of its own.
    assert '#include "lvgl_public.h"' in checker
    assert "LV_CONF_INTERNAL_H" not in checker


def test_checks_are_not_in_the_internal_header(kconf, entries):
    internal = generate_internal(kconf, entries)
    assert '#error "LV_USE_BBB must be enabled' not in internal
    assert '#error "LV_USE_EEE requires LV_USE_AAA' not in internal


def test_checker_warns_about_deprecated_symbols(kconf, entries):
    checker = generate_checker(kconf, entries)
    assert "#warning LV_X11_RENDER_MODE_PARTIAL is deprecated" in checker
    assert "#warning LV_X11_RENDER_MODE_DIRECT is deprecated" in checker
    assert "#warning LV_X11_RENDER_MODE_FULL is deprecated" in checker


def test_internal_still_maps_deprecated_symbols_without_warning(kconf, entries):
    # The #undef/#define remapping has to stay in the header so user code keeps
    # working; only the #warning moves to the checker.
    internal = generate_internal(kconf, entries)
    assert "#define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_PARTIAL" in internal
    assert "#warning LV_X11_RENDER_MODE_PARTIAL" not in internal
    assert "#warning LV_X11_RENDER_MODE_DIRECT" not in internal
    assert "#warning LV_X11_RENDER_MODE_FULL" not in internal


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
