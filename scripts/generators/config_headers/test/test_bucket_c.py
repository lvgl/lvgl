"""Bucket C: computed-int alias (LV_SDL_BUF_COUNT) and the font pointer table."""

from config_headers.config_entry import EnumChoice, IntConfig


def test_computed_int_alias_resolves_to_value(entries):
    # No prompt, default references a computed int -> emit the resolved value,
    # not the bare symbol name.  Single buffer is the default here -> 1.
    buf = entries["LV_SDL_BUF_COUNT"]
    assert isinstance(buf, IntConfig)
    assert buf.value == "1"
    assert buf.emit_template()[-1] == "#define LV_SDL_BUF_COUNT 1"


def test_raw_count_symbol_stays_ignored(entries):
    assert "LV_SDL_BUFFER_COUNT" not in entries


def test_font_default_emits_token(entries):
    font = entries["LV_FONT_DEFAULT"]
    assert isinstance(font, EnumChoice)
    assert (
        font.emit_template()[-1]
        == "#define LV_FONT_DEFAULT LV_FONT_DEFAULT_MONTSERRAT_14"
    )


def test_font_pointer_table_in_internal_options(generated):
    import re

    i = generated["internal"]
    # token -> pointer, with the column padded to the widest font name.
    assert re.search(
        r"#define LV_FONT_DEFAULT_MONTSERRAT_14\s+&lv_font_montserrat_14\b", i
    )
    assert re.search(r"#define LV_FONT_DEFAULT_UNSCII_8\s+&lv_font_unscii_8\b", i)
