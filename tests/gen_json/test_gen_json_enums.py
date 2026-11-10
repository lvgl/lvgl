"""Preserve C integer suffix and bitmask evaluation in generated JSON."""

from conftest import named


def test_enum_bitmask_integer_suffixes(api_json):
    enum = named(api_json["enums"], "test_flag_t")
    assert enum["json_type"] == "enum"
    values = {
        member["name"]: int(member["value"], 16)
        for member in enum["members"]
    }
    assert values == {
        "TEST_FLAG_1": 0x10,
        "TEST_FLAG_2": 0x100,
        "TEST_FLAG_COMBINED": 0x110,
    }
