"""Unit tests for IntConfig (int and hex scalars)."""

from config_headers.config_entry import IntConfig


def test_int_parses_with_literal_value(entries):
    refr = entries["LV_DEF_REFR_PERIOD"]
    assert isinstance(refr, IntConfig)
    assert refr.value == "33"


def test_hex_keeps_0x_prefix(entries):
    rev = entries["LV_VG_LITE_HAL_GPU_REVISION"]
    assert isinstance(rev, IntConfig)
    assert rev.value == "0x40"


def test_int_emit_template(entries):
    refr = entries["LV_DEF_REFR_PERIOD"]
    assert refr.emit_template() == [
        "/** Default display refresh, input device read and animation step period. */",
        "#define LV_DEF_REFR_PERIOD 33",
    ]


def test_int_emit_internal_uses_plain_ladder(entries):
    refr = entries["LV_DEF_REFR_PERIOD"]
    assert refr.emit_internal() == [
        "#ifndef LV_DEF_REFR_PERIOD",
        "    #ifdef CONFIG_LV_DEF_REFR_PERIOD",
        "        #define LV_DEF_REFR_PERIOD CONFIG_LV_DEF_REFR_PERIOD",
        "    #else",
        "        #define LV_DEF_REFR_PERIOD 33",
        "    #endif",
        "#endif",
    ]


def test_int_has_no_kconfig_present_layer(entries):
    # Ints always emit a CONFIG_ value under Kconfig, so they never need the
    # LV_KCONFIG_PRESENT disambiguation that default-1 bools require.
    rev = entries["LV_VG_LITE_HAL_GPU_REVISION"]
    assert "LV_KCONFIG_PRESENT" not in "\n".join(rev.emit_internal())
