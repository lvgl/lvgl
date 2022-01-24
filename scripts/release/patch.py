#!/usr/bin/env python3

# Applies a commit or commits on baranch or branches 
# USAGE:
# patch.py -c <commit-list> -b <branch-list> [-p] [-t]
#   - <commit-list>: list of commit SHAs to apply. 
#   - <branch-list>: branches where the commit should be applied. * can be used as wildchar 
# -  p: push the changes to <brach-list>
# -  t: increment version number and create a tag


import os, subprocess, com, re

push = False

def clone(repo):
  com.cmd("git clone  --recurse-submodules https://github.com/lvgl/" + repo)
  os.chdir("./" + repo)
  com.cmd("git checkout master")
  com.cmd("git remote update origin --prune")
  com.cmd("git pull origin --tags")
  os.chdir("..")
    
# Get the list of realted minor version branches

#clone("lvgl")
os.chdir("lvgl")

cmd = "git branch --remotes | grep origin/release/v8"
branches, error = subprocess.Popen(cmd, shell=True, executable="/bin/bash", stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

branches = str(branches)
branches = branches.replace("b'", "")
branches = branches.replace("'", "")
branches = branches.replace("origin/", "")
branches = branches.replace("  ", " ")
branches = branches.replace("\\n", "")
branches = branches.split(" ")
branches = list(filter(len, branches))

commits = []
with open('../commits.txt') as f:
    for line in f:
        commits.insert(0, line)

print(commits)

for br in branches:
  com.cmd("git checkout " + br)

  print("Applying commits")
  for c in commits:
    h = c.split(" ")
    com.cmd("git cherry-pick " + h[0])
      
  ver = com.get_lvgl_version(br)
  ver_new = ver.copy()
  ver_new[2] = str(int(ver_new[2]) + 1)
  print("Updating branch '" + br + "' from '" + com.ver_format(ver) + "' to '" + com.ver_format(ver_new) + "'")
  com.update_version(ver_new)
  com.cmd("git tag -a " + com.ver_format(ver_new) + "-m \"Release " + com.ver_format(ver_new) + "\"")
  
  if push:
    com.cmd("git push origin " + br + "--tags")
    
com.cmd("git checkout master")    
ver = com.get_lvgl_version("master")
ver = com.get_lvgl_version(br)
ver_new[2] = str(int(ver_new[2]) + 1)
t = com.ver_format(ver_new) + "-dev"
com.cmd("git tag -a " + t + " -m \"Start " + t + "\"")

if push:
  com.cmd("git push origin master --tags")

