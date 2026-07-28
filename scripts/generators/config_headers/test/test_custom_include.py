"""Tests for the custom-include convention and its central auto-include block."""

import os

import pytest

from config_headers.emit import custom_includes, generate_internal
from config_headers.parse import load, parse_entries

DIR = os.path.dirname(os.path.abspath(__file__))


def _entries(name):
    return parse_entries(load(os.path.join(DIR, name)))


@pytest.fixture
def entries():
    return _entries("custom_include.kconfig")


def test_pair_is_detected(entries):
    assert ("LV_FOO_USE_CUSTOM_INCLUDE", "LV_FOO_CUSTOM_INCLUDE") in custom_includes(
        entries
    )


def test_subsystem_included_is_not_auto_included(entries):
    paths = [p for _g, p in custom_includes(entries)]
    assert "LV_GLOBAL_CUSTOM_INCLUDE" not in paths


def test_auto_include_block_is_emitted(entries):
    internal = generate_internal(
        load(os.path.join(DIR, "custom_include.kconfig")), entries
    )
    assert "#if LV_FOO_USE_CUSTOM_INCLUDE" in internal
    assert "#include LV_FOO_CUSTOM_INCLUDE" in internal
    # exempt / subsystem-included options are never #included centrally
    assert "#include LV_GLOBAL_CUSTOM_INCLUDE" not in internal


def test_missing_gate_raises():
    with pytest.raises(SystemExit):
        custom_includes(_entries("bad_custom_include.kconfig"))
