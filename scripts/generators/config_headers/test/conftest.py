"""Shared pytest fixtures for the config_headers tests."""

import os

import pytest

# Make the package importable whether tests are run from the repo root or from
# anywhere else: the package's parent (scripts/generators) goes on sys.path via
# pytest's rootdir handling, but we also expose the fixture path here.

TEST_DIR = os.path.dirname(os.path.abspath(__file__))
FIXTURE = os.path.join(TEST_DIR, "test.kconfig")


@pytest.fixture
def fixture_kconfig():
    """Absolute path to test/test.kconfig."""
    return FIXTURE


@pytest.fixture
def entries(fixture_kconfig):
    """All config entries parsed from the fixture, keyed by name."""
    from config_headers.parse import parse

    return {e.name: e for e in parse(fixture_kconfig)}


@pytest.fixture
def generated(fixture_kconfig):
    """The full generated template + internal text for the fixture."""
    from config_headers.emit import (
        generate_bridge,
        generate_internal,
        generate_template,
    )
    from config_headers.parse import load, parse_entries

    kconf = load(fixture_kconfig)
    entry_list = parse_entries(kconf)
    return {
        "template": generate_template(kconf, entry_list),
        "internal": generate_internal(kconf, entry_list),
        "bridge": generate_bridge(kconf, entry_list),
    }
