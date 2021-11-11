# Generate CHANGELOG_LAST.md from changes since the last version tag. (vx.y.z-dev tags are ignored)
# CHANGELOG_LAST.md can be edited manually if required and manually added to docs/CHANGELOG.md
#
# Requirements: 
#    npm install -g auto-changelog
#
# Usage: 
#    changelog-gen <next-version>
#
# Example: if the latest verision is v5.6.7 the followings can be used for bugfix, minor or major versions:  
#    changelog-gen v5.6.8         
#    changelog-gen v5.7.0         
#    changelog-gen v6.0.0         

auto-changelog -t changelog-template.hbs -l false --latest-version $1 --unreleased-only  --tag-pattern ^v[0-9]+.[0-9]+.[0-9]+$ -o CHANGELOG_LAST.md
