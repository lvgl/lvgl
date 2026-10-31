"""Keep documentation lookups used by the historical serializer."""

import re

import pytest

from conftest import expression_tokens, named, objects


def text(value):
    # Renderers may place whitespace between inline code and punctuation.
    return re.sub(r"\s+([.,;!?])", r"\1", " ".join(value.split()))


@pytest.mark.parametrize("collection,name,expected", [
    ("functions", "test_reset", "TEST void function documentation."),
    ("enums", "test_flags", "TEST enum documentation."),
    ("typedefs", "test_number_t", "TEST primitive typedef documentation."),
    ("typedefs", "test_const_number_t", "TEST qualified typedef documentation."),
    ("structures", "test_record", "TEST struct documentation."),
    ("unions", "test_value", "TEST union documentation."),
    ("variables", "test_global", "TEST global variable documentation."),
    ("forward_decls", "test_incomplete", "TEST incomplete struct documentation."),
    ("function_pointers", "test_callback_t", "TEST callback documentation. TEST callback return documentation."),
])
def test_declaration_documentation(api_json, collection, name, expected):
    assert text(named(api_json[collection], name)["docstring"]) == expected


def test_function_and_return_documentation(api_json):
    function = named(api_json["functions"], "test_compute")
    assert text(function["docstring"]) == "TEST primitive function documentation. TEST primitive return documentation."
    assert text(function["type"]["docstring"]) == "TEST primitive return documentation."
    assert [text(arg["docstring"]) for arg in function["args"]] == [
        "TEST number argument documentation.", "TEST flags argument documentation.",
    ]
    read = named(api_json["functions"], "test_read")
    assert text(read["type"]["docstring"]) == "TEST pointer return documentation."
    assert [text(arg["docstring"]) for arg in read["args"]] == [
        "TEST record argument documentation.", "TEST text argument documentation.",
        "TEST cursor argument documentation.", "TEST callback parameter documentation.",
        "TEST stdlib argument documentation.",
    ]


def test_function_paragraphs_preserve_exact_separators(api_json):
    function = named(api_json["functions"], "test_parameter_paragraphs")
    assert function["docstring"].startswith(
        "TEST paragraph parameter function. \n\nA second function paragraph."
    )


def test_enum_member_documentation(api_json):
    enum = named(api_json["enums"], "test_flags")
    expected = {
        "TEST_ZERO": "TEST zero member documentation.",
        "TEST_EXPLICIT": "TEST explicit member documentation.",
        "TEST_SHIFT": "TEST shift member documentation.",
        "TEST_COMBINED": "TEST combined member documentation.",
    }
    assert {item["name"]: text(item["docstring"]) for item in enum["members"]} == expected


@pytest.mark.parametrize("collection,name,expected", [
    ("structures", "test_record", {
        "count": "TEST count field documentation.", "text": "TEST text field documentation.",
        "samples": "TEST array field documentation.", "bits": "TEST bit field documentation.",
        "number": "TEST typedef field documentation.", "flags": "TEST enum field documentation.",
    }),
    ("unions", "test_value", {
        "integer": "TEST integer union field documentation.",
        "real": "TEST real union field documentation.",
    }),
])
def test_field_documentation(api_json, collection, name, expected):
    fields = named(api_json[collection], name)["fields"]
    assert {item["name"]: text(item["docstring"]) for item in fields} == expected


def test_no_docstrings_contract(no_docs_json, api_json):
    assert no_docs_json["macros"] == []
    for key in api_json.keys() - {"macros"}:
        assert {item["name"] for item in no_docs_json[key]} == {item["name"] for item in api_json[key]}
    for obj in objects(no_docs_json):
        if "docstring" in obj:
            assert obj["docstring"] in ("", None), obj


@pytest.mark.parametrize("collection,name,brief,detail", [
    ("functions", "test_function", "helper", "for record initialization"),
    ("enums", "test_canonical_state", "enum", "for record states"),
    ("structures", "test_canonical_record", "struct", "for a typed record"),
    ("unions", "test_canonical_value", "union", "for a numeric value"),
    ("typedefs", "test_canonical_number_t", "typedef", "for a record count"),
    ("variables", "test_canonical_limit", "variable", "for the immutable record limit"),
])
def test_canonical_brief_and_detail(api_json, collection, name, brief, detail):
    assert text(named(api_json[collection], name)["docstring"]) == (
        f"TEST canonical {brief} brief. TEST canonical {brief} detail {detail}."
    )


def test_canonical_function_brief_detail_and_code_references(api_json):
    function = named(api_json["functions"], "test_canonical_record_update")
    doc = text(function["docstring"])
    for expected in [
        "TEST canonical function brief.", "TEST canonical function detail.",
        "test_record_t", "test_function()",
    ]:
        assert expected in doc
    assert "TEST canonical record parameter documentation." not in doc


def test_canonical_parameter_continuations_and_output(api_json):
    function = named(api_json["functions"], "test_canonical_record_update")
    args = {arg["name"]: text(arg["docstring"]) for arg in function["args"]}
    assert "TEST canonical record parameter documentation." in args["record"]
    assert "test_canonical_record_t" in args["record"]
    for expected in [
        "TEST canonical array parameter documentation.",
        "TEST canonical array continuation documentation.",
        "TEST canonical first sample description", "TEST canonical second sample description",
    ]:
        assert expected in args["values"]
    assert args["written"] == (
        "TEST canonical output parameter documentation. "
        "TEST canonical output continuation documentation."
    )


def test_canonical_nullable_parameter_metadata(api_json):
    function = named(api_json["functions"], "test_canonical_record_update")
    label = named(function["args"], "label")
    doc = text(label["docstring"])
    for expected in [
        "TEST canonical nullable parameter documentation.", "May be NULL.",
        "TEST canonical nullable continuation documentation for NULL.",
    ]:
        assert expected in doc
    assert "@nullable" not in doc


def test_canonical_return_continuation_and_code_references(api_json):
    function = named(api_json["functions"], "test_canonical_record_update")
    doc = text(function["type"]["docstring"])
    for expected in [
        "TEST canonical return documentation for", "test_canonical_record_t",
        "TEST canonical return continuation documentation.", "test_function()",
        "failure returns NULL.",
    ]:
        assert expected in doc


@pytest.mark.parametrize("command", ["note", "see", "deprecated"])
def test_canonical_additional_sections_use_existing_docstring(api_json, command):
    function = named(api_json["functions"], "test_canonical_record_update")
    assert f"TEST canonical {command} documentation." in text(function["docstring"])
    assert set(function) == {"name", "type", "json_type", "docstring", "args"}


@pytest.mark.parametrize("collection,name,member_key,expected", [
    ("enums", "test_canonical_state", "members", {
        "TEST_CANONICAL_IDLE": "TEST canonical idle member documentation.",
        "TEST_CANONICAL_READY": "TEST canonical ready member documentation.",
    }),
    ("structures", "test_canonical_record", "fields", {
        "count": "TEST canonical count field documentation.",
        "state": "TEST canonical state field documentation.",
    }),
    ("unions", "test_canonical_value", "fields", {
        "integer": "TEST canonical integer field documentation.",
        "real": "TEST canonical real field documentation.",
    }),
])
def test_canonical_trailing_member_documentation(api_json, collection, name, member_key, expected):
    members = named(api_json[collection], name)[member_key]
    assert {member["name"]: text(member["docstring"]) for member in members} == expected


def test_canonical_export_marker_preserves_macro_contract(api_json):
    macro = named(api_json["macros"], "TEST_EXPORTED_CONST")
    assert set(macro) == {"name", "json_type", "docstring", "params", "initializer"}
    assert macro["json_type"] == "macro"
    assert macro["params"] is None
    assert expression_tokens(macro["initializer"]) == expression_tokens("(TEST_OTHER + 10)")
    assert text(macro["docstring"]) == "TEST exported macro documentation."
