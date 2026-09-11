"""Bucket B: choices with no derived int, and Bucket A: ignore/deprecated."""

from config_headers.config_entry import EnumChoice


def test_named_choice_emits_single_macro(entries):
    enc = entries["LV_TXT_ENC"]
    assert isinstance(enc, EnumChoice)
    assert enc.selected_token == "LV_TXT_ENC_UTF8"
    assert enc.emit_template()[-1] == "#define LV_TXT_ENC LV_TXT_ENC_UTF8"


def test_named_choice_defines_no_tokens_and_needs_bridge(entries):
    enc = entries["LV_TXT_ENC"]
    assert enc.emit_internal_options() == []  # header-owned tokens
    assert enc.needs_bridge is True  # no derived int → autoconf needs a bridge


def test_mapped_choice_uses_macro_and_mapped_tokens(entries):
    # The anonymous choice surfaces under its override macro name...
    mode = entries["LV_SDL_RENDER_MODE"]
    assert isinstance(mode, EnumChoice)
    # ...and the selected member maps to the header token.
    assert mode.selected_token == "LV_DISPLAY_RENDER_MODE_DIRECT"
    assert (
        mode.emit_template()[-1]
        == "#define LV_SDL_RENDER_MODE LV_DISPLAY_RENDER_MODE_DIRECT"
    )


def test_mapped_choice_members_do_not_leak_as_bools(entries):
    # The raw LV_SDL_RENDER_MODE_* members must not appear as their own entries.
    assert "LV_SDL_RENDER_MODE_PARTIAL" not in entries
    assert "LV_SDL_RENDER_MODE_DIRECT" not in entries


def test_ignored_symbol_is_dropped(entries):
    assert "LV_SDL_SINGLE_BUFFER" not in entries  # deprecated, shimmed
    assert "LV_USE_THORVG" not in entries  # derived in the footer


def test_deprecated_symbol_is_dropped(entries):
    assert "LV_MEM_SIZE_KILOBYTES" not in entries
