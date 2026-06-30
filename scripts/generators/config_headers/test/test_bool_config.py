"""Unit tests for BoolConfig: parsing and emission.

These pin *intended* behavior, not the current generator's output.  A test
says "LV_USE_WAYLAND parses into a BoolConfig" and "its emit methods produce
exactly this text" - so the rewrite can grow type by type with confidence.
"""

from config_headers.config_entry import BoolConfig


def test_plain_bool_parses_as_bool_config(entries):
    wayland = entries["LV_USE_WAYLAND"]
    assert isinstance(wayland, BoolConfig)
    assert wayland.value == "0"


def test_default_y_bool_has_default_1(entries):
    log = entries["LV_USE_LOG"]
    assert isinstance(log, BoolConfig)
    assert log.value == "1"


def test_bool_emit_template(entries):
    wayland = entries["LV_USE_WAYLAND"]
    assert wayland.emit_template() == [
        "/** Use Wayland to open a window and handle input on Linux or BSD desktops */",
        "#define LV_USE_WAYLAND 0",
    ]


def test_bool_emit_internal_default_off(entries):
    wayland = entries["LV_USE_WAYLAND"]
    assert wayland.emit_internal() == [
        "#ifndef LV_USE_WAYLAND",
        "    #ifdef CONFIG_LV_USE_WAYLAND",
        "        #define LV_USE_WAYLAND CONFIG_LV_USE_WAYLAND",
        "    #else",
        "        #define LV_USE_WAYLAND 0",
        "    #endif",
        "#endif",
    ]


def test_bool_emit_internal_default_on_uses_kconfig_present(entries):
    log = entries["LV_USE_LOG"]
    assert log.emit_internal() == [
        "#ifndef LV_USE_LOG",
        "    #ifdef LV_KCONFIG_PRESENT",
        "        #ifdef CONFIG_LV_USE_LOG",
        "            #define LV_USE_LOG CONFIG_LV_USE_LOG",
        "        #else",
        "            #define LV_USE_LOG 0",
        "        #endif",
        "    #else",
        "        #define LV_USE_LOG 1",
        "    #endif",
        "#endif",
    ]


def test_bool_emits_nothing_on_other_targets(entries):
    wayland = entries["LV_USE_WAYLAND"]
    assert wayland.emit_internal_options() == []
    assert wayland.emit_kconfig() == []
