"""Macros remain replacement expressions in the existing five-field schema."""

import pytest

from conftest import expression_tokens, named


@pytest.mark.parametrize("name,initializer,params,doc", [
    ("TEST_OTHER", "7", None, "other"),
    ("TEST_NUMBER", "123", None, "number"),
    ("TEST_STRING", '"hello  world"', None, "string"),
    ("TEST_ESCAPED", r'"quote: \" slash: \\ tab: \t"', None, "escaped string"),
    ("TEST_ALIAS", "TEST_OTHER", None, "alias"),
    ("TEST_EXPR", "(TEST_OTHER + 10)", None, "expression"),
    ("TEST_FUNC_REF", "test_reset", None, "function reference"),
    ("TEST_FUNCTION_LIKE", "((x) + 1)", ["x"], "function-like"),
    ("TEST_MULTI", "((first) + (second))", ["first", "second"], "multiple parameter"),
    ("TEST_CALL", "TEST_FUNCTION_LIKE(TEST_OTHER)", None, "call"),
    # The model distinguishes (), but the historical JSON represents it as null.
    ("TEST_ZERO_ARGS", "42", None, "zero parameter"),
    ("TEST_EMPTY", None, None, "empty"),
])
def test_macro_contract(api_json, name, initializer, params, doc):
    macro = named(api_json["macros"], name)
    assert set(macro) == {"name", "json_type", "docstring", "params", "initializer"}
    assert macro["json_type"] == "macro"
    assert macro["params"] == params
    assert expression_tokens(macro["initializer"]) == expression_tokens(initializer)
    assert macro["docstring"].strip() == f"TEST {doc} macro documentation."


def test_macro_paragraphs_and_missing_docstring_preserve_nullable_contract(api_json):
    macro = named(api_json["macros"], "TEST_MULTIPARAGRAPH")
    assert macro["docstring"].startswith(
        "TEST macro first paragraph. \n\nTEST macro second paragraph."
    )
    undocumented = named(api_json["macros"], "TEST_UNDOCUMENTED")
    assert undocumented["docstring"] == ""


def test_private_header_macro_is_not_in_default_public_export_view(api_json):
    names = {macro["name"] for macro in api_json["macros"]}
    assert "TEST_PRIVATE_HEADER_MACRO" not in names
