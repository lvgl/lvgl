"""Assertions on the existing binding JSON, independent of LVGL API names."""

import pytest

from conftest import COLLECTIONS, named, objects


def scalar(name, kind="primitive_type", quals=None):
    return {"name": name, "json_type": kind, "quals": quals or []}


def pointer(type_, quals=None):
    return {"type": type_, "json_type": "pointer", "quals": quals or []}


def test_top_level_contract(api_json):
    assert set(api_json) == COLLECTIONS
    assert all(isinstance(api_json[key], list) and api_json[key] for key in COLLECTIONS)
    assert not [obj for obj in objects(api_json) if obj.get("json_type") == "unknown_type"]


def test_void_function_schema(api_json):
    function = named(api_json["functions"], "test_reset")
    assert set(function) == {"name", "type", "json_type", "docstring", "args"}
    assert function["json_type"] == "function"
    assert function["type"] == {"type": scalar("void"), "json_type": "ret_type", "docstring": ""}
    assert function["args"] == [{"name": None, "type": scalar("void"),
                                 "json_type": "arg", "docstring": "", "quals": []}]


def test_function_arguments_and_return(api_json):
    function = named(api_json["functions"], "test_compute")
    assert function["type"]["json_type"] == "ret_type"
    assert function["type"]["type"] == scalar("int")
    args = function["args"]
    assert [arg["name"] for arg in args] == ["number", "flags"]
    for arg, type_name in zip(args, ["test_number_t", "test_flags_t"]):
        assert set(arg) == {"name", "type", "json_type", "docstring"}
        assert arg["json_type"] == "arg"
        assert arg["type"] == scalar(type_name, "lvgl_type")


def test_pointer_qualifiers_and_stdlib_types(api_json):
    function = named(api_json["functions"], "test_read")
    assert function["type"]["type"] == pointer(scalar("char", quals=["const"]))
    args = {arg["name"]: arg for arg in function["args"]}
    assert args["record"]["type"] == pointer(scalar("test_record_t", "lvgl_type"))
    assert args["text"]["type"] == pointer(scalar("char", quals=["const"]))
    assert args["cursor"]["type"] == pointer(scalar("int"), ["const"])
    assert args["callback"]["type"] == scalar("test_callback_t", "lvgl_type")
    assert args["length"]["type"] == scalar("size_t", "stdlib_type")
    update = named(api_json["functions"], "test_update")
    assert update["args"][0]["type"] == pointer(scalar("test_number_t", "lvgl_type"))


def test_variadic_schema(api_json):
    function = named(api_json["functions"], "test_format")
    assert function["args"][-1] == {
        "name": "...", "type": {"name": "ellipsis", "json_type": "special_type"},
        "json_type": "arg", "docstring": "",
    }


def test_function_pointer_schema(api_json):
    callback = named(api_json["function_pointers"], "test_callback_t")
    assert set(callback) == {"name", "type", "json_type", "docstring", "args", "quals"}
    assert callback["json_type"] == "function_pointer"
    assert callback["quals"] == []
    assert callback["type"] == {"type": scalar("int"), "json_type": "ret_type", "docstring": ""}
    assert callback["args"] == [{"name": "code", "type": scalar("int"),
                                  "json_type": "arg", "docstring": ""}]


def test_enum_schema_and_values(api_json):
    enum = named(api_json["enums"], "test_flags")
    assert set(enum) == {"name", "type", "json_type", "docstring", "members"}
    assert enum["json_type"] == "enum"
    assert enum["type"] == {"name": "int", "json_type": "primitive_type"}
    values = {"TEST_ZERO": 0, "TEST_EXPLICIT": 3, "TEST_SHIFT": 16, "TEST_COMBINED": 19}
    assert {member["name"] for member in enum["members"]} == set(values)
    for member in enum["members"]:
        assert set(member) == {"name", "type", "json_type", "docstring", "value"}
        assert member["json_type"] == "enum_member"
        assert member["type"] == {"name": "test_flags", "json_type": "lvgl_type"}
        assert isinstance(member["value"], str)
        assert member["value"].startswith("0x")
        assert int(member["value"], 16) == values[member["name"]]


@pytest.mark.parametrize("collection,name,kind", [
    ("structures", "test_record", "struct"), ("unions", "test_value", "union"),
])
def test_aggregate_schema(api_json, collection, name, kind):
    item = named(api_json[collection], name)
    assert set(item) == {"name", "type", "json_type", "docstring", "fields"}
    assert item["json_type"] == kind
    assert item["type"] == {"name": kind, "json_type": "primitive_type"}
    for field in item["fields"]:
        assert set(field) == {"name", "type", "json_type", "bitsize", "docstring"}
        assert field["json_type"] == "field"


def test_struct_nested_types_and_bitfield(api_json):
    fields = {f["name"]: f for f in named(api_json["structures"], "test_record")["fields"]}
    assert fields["count"]["type"] == scalar("int")
    assert fields["text"]["type"] == pointer(scalar("char", quals=["const"]))
    assert fields["samples"]["type"] == {"name": "int", "json_type": "array", "quals": [], "dim": "4"}
    assert fields["bits"]["type"] == scalar("unsigned int")
    assert fields["bits"]["bitsize"] == "3"
    assert all(f["bitsize"] is None for n, f in fields.items() if n != "bits")
    assert fields["number"]["type"] == scalar("test_number_t", "lvgl_type")
    assert fields["flags"]["type"] == scalar("test_flags_t", "lvgl_type")
    union = named(api_json["unions"], "test_value")
    assert {f["name"]: f["type"] for f in union["fields"]} == {
        "integer": scalar("int"), "real": scalar("float"),
    }


def test_typedef_schema(api_json):
    primitive = named(api_json["typedefs"], "test_number_t")
    assert set(primitive) == {"name", "type", "json_type", "docstring", "quals"}
    assert primitive["json_type"] == "typedef"
    assert primitive["type"] == {"name": "int", "json_type": "primitive_type"}
    assert primitive["quals"] == []
    qualified = named(api_json["typedefs"], "test_const_number_t")
    assert qualified["type"] == {"name": "test_number_t", "json_type": "lvgl_type"}
    assert isinstance(qualified["quals"], list) and set(qualified["quals"]) == {"const"}
    for name, target in [("test_record_t", "test_record"), ("test_value_t", "test_value")]:
        item = named(api_json["typedefs"], name)
        assert set(item) == {"name", "type", "json_type", "docstring", "quals"}
        assert item["type"] == {"name": target, "json_type": "lvgl_type"}
        assert item["quals"] == []
    enum = named(api_json["typedefs"], "test_flags_t")
    # Enum aliases historically omit quals in this branch of the serializer.
    assert set(enum) == {"name", "type", "json_type", "docstring"}
    assert enum["type"] == {"name": "test_flags", "json_type": "lvgl_type"}


def test_variable_schema(api_json):
    variable = named(api_json["variables"], "test_global")
    assert set(variable) == {"name", "type", "json_type", "docstring", "quals", "storage"}
    assert variable["json_type"] == "variable"
    assert variable["type"] == scalar("test_number_t", "lvgl_type", ["const"])
    assert variable["quals"] == ["const"]
    assert variable["storage"] == ["extern"]


def test_incomplete_forward_declaration_schema(api_json):
    declaration = named(api_json["forward_decls"], "test_incomplete")
    assert set(declaration) == {"name", "type", "json_type", "docstring", "quals"}
    assert declaration["json_type"] == "forward_decl"
    assert declaration["type"] == {"name": "struct", "json_type": "primitive_type"}
    assert declaration["quals"] == []
