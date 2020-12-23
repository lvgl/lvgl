#!/usr/bin/env python

import os.path
from os import path
from datetime import date
import sys
import com
import release
import dev
import proj

upstream_org_url = "https://github.com/lvgl/"
workdir = "./release_tmp"
proj_list = [ "lv_sim_eclipse_sdl", "lv_sim_emscripten"]

def upstream(repo):
    return upstream_org_url + repo + ".git"
    
def clone(repo):
    com.cmd("git clone  --recurse-submodules " + upstream(repo))
    os.chdir("./" + repo)
    com.cmd("git checkout master")
    com.cmd("git remote update origin --prune")
    com.cmd("git pull origin --tags")
    os.chdir("..")
    
def clone_repos():
    com.cmd("rm -fr " + workdir)
    com.cmd("mkdir " + workdir)
    os.chdir(workdir)
    clone("lvgl")
    clone("lv_examples")
    clone("lv_drivers")
    clone("docs")
    clone("blog")

    for p in proj_list:
        clone(p)
    
def cleanup():
    os.chdir("../")
    com.cmd("rm -fr " + workdir)

if __name__ == '__main__':
    prepare_type = ['major', 'minor', 'bugfix']
    dev_prepare = 'minor'    

#    if(len(sys.argv) != 2):
#        print("Missing argument. Usage ./release.py bugfix | minor | major")
#        print("Use minor by deafult")
#    else:      
#        dev_prepare = sys.argv[1]
    
    if not (dev_prepare in prepare_type): 
        print("Invalid argument. Usage ./release.py bugfix | minor | major") 
        exit(1)
     
    #os.chdir(workdir)
    clone_repos()
    release.make()
    for p in proj_list:
        proj.make(p, True)
       
    dev.make(dev_prepare)  
    #cleanup()
