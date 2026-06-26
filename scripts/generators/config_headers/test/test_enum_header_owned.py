"""EnumChoice whose tokens are owned by a C header (LV_LOG_LEVEL).

Member name == token (like LV_USE_OS), but the LV_LOG_LEVEL_* tokens already
exist in lv_log.h, so the macro emits the member name while the options block
must stay empty (re-defining them would clash with the header).
"""

from config_headers.config_entry import EnumChoice


def test_log_level_parses_and_selects_default_member(entries):
    log = entries["LV_LOG_LEVEL"]
    assert isinstance(log, EnumChoice)
    assert log.selected_token == "LV_LOG_LEVEL_WARN"


def test_log_level_emit_template_uses_member_name(entries):
    log = entries["LV_LOG_LEVEL"]
    assert log.emit_template()[-1] == "#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN"


def test_log_level_defines_no_tokens(entries):
    # Header-owned: lv_log.h defines LV_LOG_LEVEL_*, so we emit nothing here.
    log = entries["LV_LOG_LEVEL"]
    assert log.emit_internal_options() == []


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
