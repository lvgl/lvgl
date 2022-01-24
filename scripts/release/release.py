#!/usr/bin/env python3

# Create a new release from master. Execute the followings:
# - On lvgl, lv_demos, and lv_drivers:
#   - Detect the current version of master. E.g. 8.1-dev
#   - Create a new branch from the master for the release. E.g. release/v8.1
#   - Remove the "-dev" postfix from the version numbers
#   - Create a tag for the new version. E.g. v8.1
#   - Push the new branch and tag
#   - Get the relevant changes from docs/CHANGELOG.md and create a blog post from it
#   - Increment the version number in master. E.g.g 8.1-dev to 8.2-dev
#   - Add a new section to the CHANGELOG with the new version
# - Update the simulator and lv_port projects
#
# USAGE:
# release.py <type>  
# - <type>: -minor or -major
