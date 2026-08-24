#!/usr/bin/env python3

# Start it from the lvgl folder like ./docs/build_docs.py
# After in a browser open http://localhost:3001/docs/open

import os

os.system(
    "docker run --rm -p 3001:3001 "
    f"-v {os.getcwd()}/docs/src:/app/apps/open/content/latest/src "
    "ghcr.io/lvgl-private/lvgl-open-docs"
)
