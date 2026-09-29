"""Exercise CLI/config behaviour using controlled feature names."""

import os
from pathlib import Path
import shutil
import subprocess
import sys

from conftest import COLLECTIONS, generate


def test_default_configuration_preserves_disabled_blocks(fixture_repo, tmp_path):
    result = subprocess.run([
        sys.executable, str(fixture_repo / "scripts/gen_json/gen_json.py"),
        "--develop", "--no-docstrings", "--output-path", str(tmp_path / "output"),
    ], cwd=tmp_path, env=dict(os.environ, PYTHONDONTWRITEBYTECODE="1"),
        capture_output=True, text=True, check=True, timeout=120)
    prefix = "temporary file path: "
    assert prefix in result.stdout
    intermediate = Path(result.stdout.rsplit(prefix, 1)[1].strip())
    try:
        template = (fixture_repo / "lv_conf_template.h").read_text()
        assert (intermediate / "lv_conf.h").read_text() == template.replace("#if 0", "#if 1", 1)
    finally:
        shutil.rmtree(intermediate)


def test_configuration_controls_metadata_and_declarations(configured_json):
    enabled, data = configured_json
    macro_names = {m["name"] for m in data["macros"]}
    function_names = {f["name"] for f in data["functions"]}
    assert ("TEST_CONDITIONAL_MACRO" in macro_names) == bool(enabled)
    assert ("test_conditional" in function_names) == bool(enabled)
    assert "TEST_NUMBER" in macro_names
    assert "test_compute" in function_names


def test_target_header_and_output_naming(fixture_repo, tmp_path):
    data = generate(fixture_repo, tmp_path / "output", "--target-header",
                    str(fixture_repo / "include/lvgl/lv_api_fixture.h"))
    assert set(data) == COLLECTIONS
    assert (tmp_path / "output/lv_api_fixture.json").is_file()
    assert "test_private" not in {f["name"] for f in data["functions"]}
    assert "test_compute" in {f["name"] for f in data["functions"]}


def test_private_filter(fixture_repo, tmp_path):
    data = generate(fixture_repo, tmp_path / "output", "--filter-private")
    # The historical internal option selects private headers.
    assert {f["name"] for f in data["functions"]} == {"test_private"}
    assert "TEST_NUMBER" in {m["name"] for m in data["macros"]}


def test_compiler_arguments_are_preserved(fixture_repo, tmp_path):
    data = generate(fixture_repo, tmp_path / "output", "--no-docstrings", "-DTEST_FEATURE=0")
    assert "test_conditional" not in {f["name"] for f in data["functions"]}
    assert "test_compute" in {f["name"] for f in data["functions"]}


def test_stdout_is_json(fixture_repo, tmp_path):
    data = generate(fixture_repo, tmp_path / "unused", "--no-docstrings", stdout=True)
    assert set(data) == COLLECTIONS
    assert data["functions"] and data["macros"] == []
