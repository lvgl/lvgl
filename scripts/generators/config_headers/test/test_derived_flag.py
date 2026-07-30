"""Unit tests for DerivedFlag (internal capability flags set via `select`)."""

from config_headers.config_entry import BoolConfig, DerivedFlag


def test_selected_flag_parses_as_derived_flag(entries):
    flag = entries["LV_DRAW_HAS_VECTOR_SUPPORT"]
    assert isinstance(flag, DerivedFlag)
    assert flag.selectors_expr == "LV_USE_DRAW_VG_LITE || LV_USE_DRAW_NANOVG"


def test_derived_flag_absent_from_template(entries):
    # Internal-only: never offered to the user.
    flag = entries["LV_DRAW_HAS_VECTOR_SUPPORT"]
    assert flag.emit_template() == []


def test_derived_flag_internal_block(entries):
    flag = entries["LV_DRAW_HAS_VECTOR_SUPPORT"]
    assert flag.emit_internal() == [
        "#ifndef LV_DRAW_HAS_VECTOR_SUPPORT",
        "    #if (LV_USE_DRAW_VG_LITE || LV_USE_DRAW_NANOVG)",
        "        #define LV_DRAW_HAS_VECTOR_SUPPORT 1",
        "    #else",
        "        #define LV_DRAW_HAS_VECTOR_SUPPORT 0",
        "    #endif",
        "#endif",
    ]


def test_derived_flag_emits_nothing_to_options_or_bridge(entries):
    flag = entries["LV_DRAW_HAS_VECTOR_SUPPORT"]
    assert flag.emit_internal_options() == []
    assert flag.emit_kconfig() == []


def test_selectors_are_still_normal_bool_options(entries):
    # The options that `select` the flag remain ordinary user-facing bools.
    assert isinstance(entries["LV_USE_DRAW_VG_LITE"], BoolConfig)
    assert isinstance(entries["LV_USE_DRAW_NANOVG"], BoolConfig)


def test_per_backend_flag_is_derived(entries):
    # A hidden per-backend flag selected by a choice member is a DerivedFlag.
    # (Its guard-rewritten `<macro> == <token>` body is asserted in test_driver,
    # since the rewrite happens at emit time when the guard map exists.)
    assert isinstance(entries["LV_LINUX_DRM_USE_EGL"], DerivedFlag)
