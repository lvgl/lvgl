"""Packaging hook: ship widget_specs.json inside the lvglgdb package.

The file lives in scripts/generators/ because the wrapper generator owns it,
but the plugin reads it at runtime. A pip install has no scripts/ directory,
so copy the file into the built package, where specs_path() looks first.
All other metadata is in pyproject.toml.
"""

import shutil
from pathlib import Path

from setuptools import setup
from setuptools.command.build_py import build_py

SPECS = Path(__file__).resolve().parent / "scripts" / "generators" / "widget_specs.json"


class BuildPyWithSpecs(build_py):
    def run(self):
        super().run()
        if not SPECS.exists():
            raise FileNotFoundError(f"{SPECS} is missing; run scripts/generate_all.py")
        target = Path(self.build_lib) / "lvglgdb" / "widget_specs.json"
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(SPECS, target)


setup(cmdclass={"build_py": BuildPyWithSpecs})
