"""Macro definitions and documentation are metadata, not evaluated values."""

import sys
import pytest

from conftest import REPO, expression_tokens

sys.path.insert(0, str(REPO / "scripts"))
from lvgl_api import PublicApi  # NOQA
from lvgl_api.lvgl_api import run_doxygen  # NOQA
sys.path.insert(0, str(REPO / "scripts/gen_json"))
from api_docs import (
    ApiDocs,
    gen_json_doxygen_predefined,
    parse_gen_json_api,
)  # NOQA


@pytest.fixture(scope="module")
def model_and_xml(fixture_repo, tmp_path_factory):
    output = tmp_path_factory.mktemp("api-model")
    config = output / "lv_conf.h"
    config.write_text("#define TEST_FEATURE 0\n")
    run_doxygen(
        fixture_repo,
        output,
        ("include/lvgl", "src"),
        lv_conf_path=config,
        doxygen_predefined=gen_json_doxygen_predefined(config),
        doxygen_include_path=(fixture_repo / "include",),
        inherit_doxygen_aliases=True,
    )
    xml = output / "xml"
    return PublicApi.parse(fixture_repo, xml_dir=xml), xml


def test_macro_kind_parameters_and_location(model_and_xml, fixture_repo):
    model, _ = model_and_xml
    source = (fixture_repo / "include/lvgl/lv_api_fixture.h").read_text().splitlines()
    for name, params, initializer in [
        ("TEST_NUMBER", None, "123"),
        ("TEST_MULTI", ["first", "second"], "((first) + (second))"),
        ("TEST_ZERO_ARGS", [], "42"),
        ("TEST_EMPTY", None, None),
    ]:
        assert model.macros[name]
        for macro in model.macros[name]:
            assert macro.name == name
            assert macro.params == params
            assert expression_tokens(macro.initializer) == expression_tokens(initializer)
            assert macro.source_file == "include/lvgl/lv_api_fixture.h"
            assert source[macro.source_line - 1].startswith("#define " + name)
            assert macro.is_public


def test_literal_and_call_initializers(model_and_xml):
    model, _ = model_and_xml
    assert expression_tokens(model.macros["TEST_CALL"][0].initializer) == expression_tokens(
        "TEST_FUNCTION_LIKE(TEST_OTHER)"
    )
    assert model.macros["TEST_STRING"][0].initializer == '"hello  world"'
    assert model.macros["TEST_ESCAPED"][0].initializer == r'"quote: \" slash: \\ tab: \t"'


def test_binding_aliases_leave_default_analysis_markers_raw(
    fixture_repo, model_and_xml
):
    binding_model, _ = model_and_xml
    default_model = PublicApi.parse(fixture_repo)
    assert "test_conditional" in default_model.functions
    assert "test_conditional" not in binding_model.functions
    default_label = default_model.functions["test_canonical_record_update"].param("label")
    binding_label = binding_model.functions["test_canonical_record_update"].param("label")
    assert "@nullable" in default_label.doc
    assert "@nullable" not in binding_label.doc
    assert "May be NULL" in binding_label.doc


def test_nested_ref_initializer(tmp_path):
    (tmp_path / "expression.xml").write_text('''
<doxygen><compounddef kind="file"><sectiondef kind="define">
<memberdef kind="define"><name>TEST_CALL</name>
<initializer><ref refid="function">TEST_FUNCTION_LIKE</ref>(<ref refid="value">TEST_OTHER</ref>)</initializer>
</memberdef></sectiondef></compounddef></doxygen>
''')
    model = PublicApi.parse(tmp_path, xml_dir=tmp_path)
    assert model.macros["TEST_CALL"][0].initializer == "TEST_FUNCTION_LIKE(TEST_OTHER)"


def test_anonymous_enum_member_docs(tmp_path):
    (tmp_path / "anonymous.xml").write_text('''
<doxygen><compounddef kind="file"><sectiondef kind="enum">
<memberdef kind="enum"><name>@1</name><location file="src/lv_test.h" line="1"/>
<enumvalue><name>TEST_ANONYMOUS</name>
<detaileddescription><para>TEST anonymous member documentation.</para></detaileddescription>
</enumvalue></memberdef></sectiondef></compounddef></doxygen>
''')
    model = PublicApi.parse(tmp_path, xml_dir=tmp_path)
    assert model.enum_members["TEST_ANONYMOUS"].doc == "TEST anonymous member documentation. "
    assert ApiDocs(model).get_enum_item("TEST_ANONYMOUS").description == (
        "TEST anonymous member documentation. "
    )
    assert "@1" not in model.enums


def test_named_enum_member_is_shared_with_lookup_index(tmp_path):
    (tmp_path / "named_enum.xml").write_text('''
<doxygen><compounddef kind="file"><sectiondef kind="enum">
<memberdef kind="enum"><name>test_flags_t</name><location file="include/lvgl/lv_test.h" line="1"/>
<enumvalue><name>TEST_FLAG_FIRST</name><initializer>1</initializer>
<detaileddescription><para>TEST first flag documentation.</para></detaileddescription>
</enumvalue>
<enumvalue><name>TEST_FLAG_SECOND</name><initializer>2</initializer>
<detaileddescription><para>TEST second flag documentation.</para></detaileddescription>
</enumvalue></memberdef></sectiondef></compounddef></doxygen>
''')
    model = PublicApi.parse(tmp_path, xml_dir=tmp_path)
    members = model.enums["test_flags_t"].members
    assert model.enum_members["TEST_FLAG_FIRST"] is members[0]
    assert model.enum_members["TEST_FLAG_SECOND"] is members[1]
    assert ApiDocs(model).get_enum_item("TEST_FLAG_FIRST").description == (
        "TEST first flag documentation. "
    )


def test_redefinitions_are_retained_and_public_filter_uses_location(tmp_path):
    def definition(file, line, value):
        return f'''<memberdef kind="define"><name>TEST_REDEFINED</name>
<initializer>{value}</initializer><location file="{file}" line="{line}"/>
</memberdef>'''

    first = definition("include/lvgl/lv_test.h", 1, "1")
    second = definition("include/lvgl/lv_test.h", 3, "2")
    private = definition("src/lv_test.c", 1, "3")
    (tmp_path / "definitions.xml").write_text(
        '<doxygen><compounddef kind="file"><sectiondef kind="define">'
        + first + second + private + '</sectiondef></compounddef></doxygen>'
    )
    model = PublicApi.parse(tmp_path, xml_dir=tmp_path)
    assert {(m.source_file, m.source_line, m.initializer) for m in model.macros["TEST_REDEFINED"]} == {
        ("include/lvgl/lv_test.h", 1, "1"), ("include/lvgl/lv_test.h", 3, "2"),
        ("src/lv_test.c", 1, "3"),
    }
    assert len(model.macros["TEST_REDEFINED"]) == 3
    assert {m.initializer for m in model.public_macros()} == {"1", "2"}


def test_duplicate_implementation_macros_stay_in_model_but_not_json(
    model_and_xml, api_json
):
    model, _ = model_and_xml
    definitions = model.macros["TEST_IMPL_LOCAL"]
    assert len(definitions) == 2
    assert {macro.source_file for macro in definitions} == {
        "src/lv_impl_fixture_a.c",
        "src/lv_impl_fixture_b.c",
    }
    assert not any(macro.is_public for macro in definitions)
    assert "TEST_IMPL_LOCAL" not in {macro.name for macro in ApiDocs(model).get_macros()}
    assert "TEST_IMPL_LOCAL" not in {macro["name"] for macro in api_json["macros"]}


def test_private_header_macro_stays_in_model_but_not_json(model_and_xml, api_json):
    model, _ = model_and_xml
    definitions = model.macros["TEST_PRIVATE_HEADER_MACRO"]
    assert len(definitions) == 1
    (macro,) = definitions
    assert macro.source_file == "src/lv_contract_private.h"
    assert not macro.is_public
    assert "TEST_PRIVATE_HEADER_MACRO" not in {
        macro.name for macro in ApiDocs(model).get_macros()
    }
    assert "TEST_PRIVATE_HEADER_MACRO" not in {
        macro["name"] for macro in api_json["macros"]
    }


@pytest.mark.parametrize("reverse_file_assignment", [False, True])
def test_duplicate_macro_projection_fails_independent_of_xml_order(
    tmp_path, reverse_file_assignment
):
    def definition(refid, file, line, value, param):
        parameters = f"<param><defname>{param}</defname></param>" if param else ""
        return f'''<doxygen><compounddef id="{refid}" kind="file">
<sectiondef kind="define"><memberdef id="{refid}_define" kind="define">
<name>TEST_DUPLICATE</name>{parameters}<initializer>{value}</initializer>
<detaileddescription><para>{value} documentation.</para></detaileddescription>
<location file="{file}" line="{line}"/>
</memberdef></sectiondef></compounddef></doxygen>'''

    a = ("a", "include/lvgl/lv_a.h", 10, "A_VALUE", "first")
    z = ("z", "include/lvgl/lv_z.h", 20, "Z_VALUE", "later")
    if reverse_file_assignment:
        a, z = z, a
    (tmp_path / "a.xml").write_text(definition(*a))
    (tmp_path / "z.xml").write_text(definition(*z))
    model = PublicApi.parse(tmp_path, xml_dir=tmp_path)
    docs = ApiDocs(model)
    assert len(model.macros["TEST_DUPLICATE"]) == 2
    with pytest.raises(RuntimeError, match="duplicate macro definition for TEST_DUPLICATE") as err:
        docs.get_macros()
    assert "include/lvgl/lv_a.h:10" in str(err.value)
    assert "include/lvgl/lv_z.h:20" in str(err.value)


@pytest.mark.parametrize(
    "profiler_enabled, expected_header",
    [
        (0, "include/lvgl/debugging/profiler/lv_profiler.h"),
        (1, "include/lvgl/debugging/profiler/lv_profiler_builtin.h"),
    ],
)
def test_gen_json_profile_collects_one_configured_profiler_definition(
    tmp_path, profiler_enabled, expected_header
):
    config = tmp_path / "lv_conf.h"
    config.write_text(
        f"#define LV_USE_PROFILER {profiler_enabled}\n"
        f"#define LV_USE_PROFILER_BUILTIN {profiler_enabled}\n"
    )
    api = parse_gen_json_api(REPO, config)

    for name in (
        "LV_PROFILER_BEGIN",
        "LV_PROFILER_END",
        "LV_PROFILER_BEGIN_TAG",
        "LV_PROFILER_END_TAG",
    ):
        definitions = api.macros[name]
        assert len(definitions) == 1
        (macro,) = definitions
        assert macro.source_file == expected_header


def test_parameter_metadata_preserves_exact_paragraphs(tmp_path):
    (tmp_path / "function.xml").write_text('''<doxygen><compounddef kind="file">
<sectiondef kind="func"><memberdef kind="function">
<type>void</type><name>test_parameter_paragraphs</name><argsstring>(const char * text)</argsstring>
<param><type>const char *</type><declname>text</declname></param>
<detaileddescription><para>Function documentation.</para><parameterlist kind="param">
<parameteritem><parameternamelist><parametername>text</parametername></parameternamelist>
<parameterdescription><para>First parameter paragraph.</para><para>Second parameter paragraph.</para></parameterdescription>
</parameteritem></parameterlist></detaileddescription>
<location file="include/lvgl/lv_test.h" line="1"/>
</memberdef></sectiondef></compounddef></doxygen>''')
    model = PublicApi.parse(tmp_path, xml_dir=tmp_path)
    function_docs = ApiDocs(model).get_function("test_parameter_paragraphs")
    assert function_docs.args[0].description == (
        "First parameter paragraph. \n\nSecond parameter paragraph. "
    )


def test_detailed_doc_preserves_paragraphs_and_missing_doc_is_none(tmp_path):
    (tmp_path / "docs.xml").write_text('''
<doxygen><compounddef kind="file"><sectiondef kind="define">
<memberdef kind="define"><name>TEST_DOCUMENTED</name>
<detaileddescription><para>First paragraph.</para><para>Second paragraph.</para></detaileddescription>
</memberdef>
<memberdef kind="define"><name>TEST_UNDOCUMENTED</name><initializer>1</initializer></memberdef>
</sectiondef></compounddef></doxygen>
''')
    model = PublicApi.parse(tmp_path, xml_dir=tmp_path)
    assert model.macros["TEST_DOCUMENTED"][0].doc == "First paragraph. \n\nSecond paragraph. "
    assert model.macros["TEST_UNDOCUMENTED"][0].doc is None
