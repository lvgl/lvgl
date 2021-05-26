#!/usr/bin/env python3

# Applies a commit or commits on baranch or branches 
# USAGE:
# patch.py -c <commit-list> -b <branch-list> [-p] [-t]
#   - <commit-list>: list of commit SHAs to apply. 
#   - <branch-list>: branches where the commit should be applied. * can be used as wildchar 
# -  p: push the changes to <brach-list>
# -  t: increment version number and create a tag
