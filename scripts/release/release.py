#!/usr/bin/env python

import os.path
from os import path
from datetime import date
import sys
import com
import re

def update_release_branch(release_br, master_br = "master"):
    com.cmd("git checkout " + release_br)
    com.cmd("git pull origin " + release_br)
    com.cmd("git merge "+  master_br  +" -X ours");
    com.push("origin " + release_br);
    com.cmd("git checkout master");

def lvgl_release(ver):
    print("Release lvgl")

    ver_str = com.ver_format(ver)
    release_br = "release/v" + str(ver[0])

    os.chdir("./lvgl")
    com.define_set("./lvgl.h", "LVGL_VERSION_INFO", '\"\"')

    # Run some scripts
    os.chdir("./scripts")
    com.cmd("./code-format.sh")
    com.cmd("./lv_conf_checker.py")
    com.cmd("doxygen")
    os.chdir("../")

    com.update_version(ver)

    #update CHANGLELOG
    new_content = ""
    f = open("./CHANGELOG.md", "r")

    release_note = ""
    note_state = 0
    for i in f.read().splitlines():
        if note_state == 0:
            r = re.search(r'^## ' + ver_str, i)
            if r:
                i = i.replace("planned on ", "")
                note_state+=1

        elif note_state == 1:
            r = re.search(r'^## ', i)
            if r:
                note_state+=1
            else:
                release_note += i + '\n'

        new_content += i + '\n'

    f.close()

    f = open("./CHANGELOG.md", "w")
    f.write(new_content)
    f.close()

    com.cmd('git commit -am "Release ' + ver_str + '"')

    com.cmd('git checkout dev')
    com.cmd("git merge master -X ours")
    com.cmd("git add .")
    com.cmd("git commit -am 'Merge master'", False)
    com.push('origin dev')
    com.cmd('git checkout master')
    com.cmd("git tag -a " + ver_str + " -m 'Release " + ver_str + "' ")
    com.push('origin master')
    com.push('origin --tags')

    update_release_branch(release_br)

    os.chdir("../")

    return release_note

def lv_examples_release(ver):
    print("Release lv_examples")

    os.chdir("./lv_examples")
    release_br = "release/v" + str(ver[0])
    ver_str = com.ver_format(ver)

    com.update_version(ver)

    com.cmd("sed -i -r 's/LV_VERSION_CHECK\([0-9]+, *[0-9]+, *[0-9]+\)/"+ "LV_VERSION_CHECK\(" + str(ver[0]) + ", " + str(ver[1]) + ", " + str(ver[2]) + ")/' lv_examples.h")

    com.cmd('git commit -am "Release ' + ver_str + '"')
    com.cmd("git tag -a " + ver_str + " -m 'Release " + ver_str + "' ")
    com.push('origin master')
    com.push('origin --tags')

    update_release_branch(release_br)
    
    os.chdir("../")

def lv_drivers_release(ver):
    print("Release lv_drivers")

    os.chdir("./lv_drivers")
    release_br = "release/v" + str(ver[0])
    ver_str = com.ver_format(ver)

    com.update_version(ver)

    com.cmd('git commit -am "Release ' + ver_str + '"')
    com.cmd("git tag -a " + ver_str + " -m 'Release " + ver_str + "' ")
    com.push('origin master')
    com.push('origin --tags')

    update_release_branch(release_br)

    os.chdir("../")

def docs_release(ver):
    print("Release docs")

    os.chdir("./docs")
    release_br = "release/v" + str(ver[0])
    ver_str = com.ver_format(ver)

    com.cmd("git co latest --")
    com.cmd("rm -rf xml");
    com.cmd("cp -r ../lvgl/docs/api_doc/xml .");
    com.cmd("git add xml");

    com.update_version(ver)

    com.cmd('git commit -am "Release ' + ver_str + '"')
    com.cmd("git tag -a " + ver_str + " -m 'Release " + ver_str + "' ")
    com.push('origin master')
    com.push('origin --tags')

    update_release_branch(release_br, "latest")

    os.chdir("../")

def blog_release(ver, release_note):

    os.chdir("./blog/_posts")

    ver_str = com.ver_format(ver)

    post = "---\nlayout: post\ntitle: " + ver_str + " is released\nauthor: \"kisvegabor\"\ncover: /assets/release_cover.png\n---\n\n"
    post += release_note

    today = date.today()
    d = today.strftime("%Y-%m-%d")

    f = open(d + "-release_" + ver_str + ".md", "w")
    f.write(post)
    f.close()

    com.cmd("git add .")
    com.cmd("git commit -am 'Add " + ver_str + " release post'")
    com.push('origin master')

    os.chdir("../../")

def make():
    ver = com.get_lvgl_version("master")
    release_note = lvgl_release(ver)
    lv_examples_release(ver)
    lv_drivers_release(ver)
    docs_release(ver)
    blog_release(ver, release_note)

if __name__ == '__main__':
    make()
