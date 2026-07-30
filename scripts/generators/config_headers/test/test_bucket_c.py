"""Bucket C: the LV_SDL_BUF_COUNT direct int + deprecated choice, and the font
pointer table."""

from config_headers.config_entry import EnumChoice, IntConfig


def test_buf_count_is_a_plain_int(entries):
    # LV_SDL_BUF_COUNT is now a direct int with its own default (no indirection).
    buf = entries["LV_SDL_BUF_COUNT"]
    assert isinstance(buf, IntConfig)
    assert buf.value == "1"
    assert buf.emit_template()[-1] == "#define LV_SDL_BUF_COUNT 1"


def test_deprecated_buffer_choice_members_stay_ignored(entries):
    # Kept only for defconfig compatibility, mapped via the lv_conf_kconfig.h
    # shim -> excluded from the generated headers.
    assert "LV_SDL_SINGLE_BUFFER" not in entries
    assert "LV_SDL_DOUBLE_BUFFER" not in entries


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
