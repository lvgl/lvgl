"""Unit tests for StringConfig."""

from config_headers.config_entry import StringConfig


def test_string_parses_with_quoted_value(entries):
    brk = entries["LV_TXT_BREAK_CHARS"]
    assert isinstance(brk, StringConfig)
    # Quotes are preserved exactly as stored in Kconfig.
    assert brk.value == '" ,.;:-_)}"'


def test_string_emit_template(entries):
    brk = entries["LV_TXT_BREAK_CHARS"]
    assert brk.emit_template() == [
        "/** While rendering text strings, break (wrap) text on these chars. */",
        '#define LV_TXT_BREAK_CHARS " ,.;:-_)}"',
    ]


def test_string_emit_internal_uses_plain_ladder(entries):
    brk = entries["LV_TXT_BREAK_CHARS"]
    assert brk.emit_internal() == [
        "#ifndef LV_TXT_BREAK_CHARS",
        "    #ifdef CONFIG_LV_TXT_BREAK_CHARS",
        "        #define LV_TXT_BREAK_CHARS CONFIG_LV_TXT_BREAK_CHARS",
        "    #else",
        '        #define LV_TXT_BREAK_CHARS " ,.;:-_)}"',
        "    #endif",
        "#endif",
    ]
