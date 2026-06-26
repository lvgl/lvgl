"""Tests for the lv_conf_kconfig.h bridge (emit_kconfig)."""


def test_named_bare_choice_emits_member_token_ladder(entries):
    enc = entries["LV_TXT_ENC"]
    assert enc.emit_kconfig() == [
        "/*******************",
        " * LV_TXT_ENC",
        " *******************/",
        "#ifdef CONFIG_LV_TXT_ENC_UTF8",
        "#  define CONFIG_LV_TXT_ENC LV_TXT_ENC_UTF8",
        "#elif defined(CONFIG_LV_TXT_ENC_ASCII)",
        "#  define CONFIG_LV_TXT_ENC LV_TXT_ENC_ASCII",
        "#endif",
    ]


def test_mapped_choice_bridges_to_mapped_token(entries):
    k = "\n".join(entries["LV_SDL_RENDER_MODE"].emit_kconfig())
    # First member uses #ifdef; each member maps to its LV_DISPLAY_* token.
    assert "#ifdef CONFIG_LV_SDL_RENDER_MODE_PARTIAL" in k
    assert "#elif defined(CONFIG_LV_SDL_RENDER_MODE_DIRECT)" in k
    assert "#  define CONFIG_LV_SDL_RENDER_MODE LV_DISPLAY_RENDER_MODE_DIRECT" in k


def test_derived_int_enums_need_no_bridge(entries):
    # These get CONFIG_<macro> straight from autoconf, so no ladder.
    for name in ("LV_USE_STDLIB_MALLOC", "LV_USE_OS", "LV_COLOR_DEPTH", "LV_LOG_LEVEL"):
        assert entries[name].emit_kconfig() == []


def test_bridge_file_has_preamble_and_deprecations(generated):
    b = generated["bridge"]
    assert "#ifndef LV_CONF_KCONFIG_H" in b
    assert "CONFIG_LV_MEM_SIZE_KILOBYTES" in b  # deprecation shim
    assert b.rstrip().endswith("#endif /*LV_CONF_KCONFIG_H*/")
