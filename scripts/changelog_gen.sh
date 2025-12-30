#!/bin/sh
# Generate CHANGELOG_LAST.md from changes since the last version tag. (vx.y.z-dev tags are ignored)
# CHANGELOG_LAST.md can be edited manually if required and manually added to docs/CHANGELOG.md
#
# Requirements:
#    npm install -g auto-changelog
#
# Usage:
#    changelog-gen <next-version>
#
# Example: if the latest version is v5.6.7 the following can be used for bugfix, minor or major versions:
#    changelog-gen v5.6.8
#    changelog-gen v5.7.0
#    changelog-gen v6.0.0

OUTPUT_NAME=CHANGELOG_LAST.rst
auto-changelog -t changelog-template.hbs -l false --latest-version $1 --unreleased-only --tag-pattern ^v[0-9]+.[0-9]+.[0-9]+$ --merge-url "https://github.com/lvgl/lvgl/pull/{id}" -o $OUTPUT_NAME

# Remove leading space from PR commit name
# Eg "- ** PR TITLE" becomes "- **PR TITLE"
sed -i 's/- \*\* */- \*\*/g' $OUTPUT_NAME

# Remove trailing space from PR commit name
# Eg "- **PR TITLE **" becomes "- **PR TITLE**"
sed -i 's/ \*\* /\*\* /g' $OUTPUT_NAME
