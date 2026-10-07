"""Run the real generator in small, controlled LVGL-shaped repositories."""

import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

import pytest

REPO = Path(__file__).resolve().parents[2]
FIXTURES = Path(__file__).with_name("fixtures")
COLLECTIONS = {
    "enums", "functions", "function_pointers", "structures", "unions",
    "variables", "typedefs", "forward_decls", "macros",
}


def make_repo(root):
    for name in ("gen_json", "lvgl_api"):
        shutil.copytree(REPO / "scripts" / name, root / "scripts" / name,
                        ignore=shutil.ignore_patterns("__pycache__", "output"))
    public_headers = root / "include/lvgl"
    public_headers.mkdir(parents=True)
    for name in ("lv_api_fixture.h", "lv_canonical_fixture.h"):
        shutil.copy2(FIXTURES / name, public_headers / name)
    (root / "src").mkdir()
    shutil.copy2(FIXTURES / "lv_contract_private.h", root / "src/lv_contract_private.h")
    for name in ("lv_impl_fixture_a.c", "lv_impl_fixture_b.c"):
        shutil.copy2(FIXTURES / name, root / "src" / name)
    (root / "docs").mkdir()
    config = (REPO / "docs/Doxyfile").read_text()
    # A docs-only feature preset must not override the binding's lv_conf.h.
    (root / "docs/Doxyfile").write_text(config + "\nPREDEFINED += TEST_FEATURE=1\n")
    (root / "lvgl.h").write_text(
        '#include "include/lvgl/lv_api_fixture.h"\n#include "src/lv_contract_private.h"\n'
        '#include "include/lvgl/lv_canonical_fixture.h"\n'
    )
    (root / "lv_conf_template.h").write_text(
        "#if 0\n"
        "#if 0 && defined(__ASSEMBLY__)\n#include \"test_disabled_include.h\"\n#endif\n"
        "#if 0\n#error Disabled template content must remain disabled\n#endif\n"
        "#ifndef TEST_FEATURE\n#define TEST_FEATURE 1\n#endif\n#endif\n"
    )
    return root


def generate(root, output, *args, stdout=False, env=None):
    command = [sys.executable, str(root / "scripts/gen_json/gen_json.py")]
    if not stdout:
        command += ["--output-path", str(output)]
    environment = dict(os.environ, PYTHONDONTWRITEBYTECODE="1")
    if env:
        environment.update(env)
    result = subprocess.run(command + list(args), cwd=output.parent,
                            env=environment, capture_output=True, text=True, timeout=120)
    assert result.returncode == 0, result.stderr[-6000:] + result.stdout[-2000:]
    if stdout:
        return json.loads(result.stdout)
    name = "lvgl"
    if "--target-header" in args:
        name = Path(args[args.index("--target-header") + 1]).stem
    return json.loads((output / (name + ".json")).read_text())


def named(items, name):
    matches = [item for item in items if item["name"] == name]
    assert len(matches) == 1, f"expected one {name}, found {len(matches)}"
    return matches[0]


def expression_tokens(expression):
    """Ignore whitespace between C tokens, preserving quoted literals."""
    if expression is None:
        return None
    return re.findall(
        r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|\w+'
        r'|##|<<=?|>>=?|->|\+\+|--|&&|\|\||[=!<>+*/%&|^-]=|\.\.\.|\S',
        expression,
    )


def objects(value):
    if isinstance(value, dict):
        yield value
        for child in value.values():
            yield from objects(child)
    elif isinstance(value, list):
        for child in value:
            yield from objects(child)


@pytest.fixture(scope="session")
def fixture_repo(tmp_path_factory):
    return make_repo(tmp_path_factory.mktemp("api-contract"))


@pytest.fixture(scope="session")
def api_json(fixture_repo, tmp_path_factory):
    return generate(fixture_repo, tmp_path_factory.mktemp("json") / "output")


@pytest.fixture(scope="session")
def no_docs_json(tmp_path_factory):
    root = make_repo(tmp_path_factory.mktemp("no-docs-contract"))
    # Make metadata unavailable: success proves no-docstrings never needs it.
    shutil.rmtree(root / "scripts/lvgl_api")
    shutil.rmtree(root / "docs")
    return generate(root, tmp_path_factory.mktemp("no-docs-json") / "output",
                    "--no-docstrings")


@pytest.fixture(scope="session", params=[0, 1])
def configured_json(request, fixture_repo, tmp_path_factory):
    folder = tmp_path_factory.mktemp(f"feature-{request.param}")
    config = folder / "lv_conf.h"
    config.write_text(f"#define TEST_FEATURE {request.param}\n")
    return request.param, generate(fixture_repo, folder / "output",
                                   "--lvgl-config", str(config))
