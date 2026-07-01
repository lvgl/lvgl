"""EnumChoice with bare-literal defaults and member == token (LV_LOG_LEVEL).

Same shape as LV_USE_OS: the derived int defaults to bare numbers
(``default 2 if LV_LOG_LEVEL_WARN``), the macro expands to the member name, and
the LV_LOG_LEVEL_* tokens are defined into the options block - no C header
defines them, so lv_conf_internal.h must.
"""

from config_headers.config_entry import EnumChoice


def test_log_level_parses_and_selects_default_member(entries):
    log = entries["LV_LOG_LEVEL"]
    assert isinstance(log, EnumChoice)
    assert log.selected_token == "LV_LOG_LEVEL_WARN"


def test_log_level_emit_template_uses_member_name(entries):
    log = entries["LV_LOG_LEVEL"]
    assert log.emit_template()[-1] == "#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN"


def test_log_level_defines_tokens(entries):
    # No C header defines LV_LOG_LEVEL_*, so the choice defines them (member ==
    # token, bare-literal values) - exactly like LV_USE_OS.
    log = entries["LV_LOG_LEVEL"]
    assert log.emit_internal_options() == [
        "/* Default log verbosity */",
        "#define LV_LOG_LEVEL_TRACE   0",
        "#define LV_LOG_LEVEL_WARN    2",
        "#define LV_LOG_LEVEL_NONE    5",
    ]


def test_log_level_internal_ladder_uses_member_name(entries):
    log = entries["LV_LOG_LEVEL"]
    assert log.emit_internal() == [
        "#ifndef LV_LOG_LEVEL",
        "    #ifdef CONFIG_LV_LOG_LEVEL",
        "        #define LV_LOG_LEVEL CONFIG_LV_LOG_LEVEL",
        "    #else",
        "        #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN",
        "    #endif",
        "#endif",
    ]


def test_log_level_doc_lists_all_tokens(entries):
    log = entries["LV_LOG_LEVEL"]
    doc = log.doc
    assert "Possible values:" in doc
    for token in ("LV_LOG_LEVEL_TRACE", "LV_LOG_LEVEL_WARN", "LV_LOG_LEVEL_NONE"):
        assert token in doc
