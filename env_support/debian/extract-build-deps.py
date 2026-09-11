#!/usr/bin/env python3

import os
import re
import sys

control_path = os.path.join(os.path.dirname(__file__), "control")


with open(control_path) as f:
    content = f.read()

# Find Build-Depends section
match = re.search(r"^Build-Depends:(.*?)(?=^\S)", content, re.MULTILINE | re.DOTALL)
if not match:
    print("No Build-Depends found", file=sys.stderr)
    sys.exit(1)

raw = match.group(1)

deps = []
for dep in raw.split(","):
    # Remove comments
    dep = dep.split("#")[0]
    # Take first alternative (before |)
    dep = dep.split("|")[0]
    # Remove version constraints
    dep = re.sub(r"\([^)]*\)", "", dep)
    # Clean whitespace and newlines
    dep = dep.strip()
    if dep and dep != "debhelper-compat":
        deps.append(dep)

print(" ".join(deps))
