#!/usr/bin/env python
 
import re
import os
lastNum = re.compile(r'(?:[^\d]*(\d+)[^\d]*)+')

   
def title(t):
  print("\n---------------------------------")
  print(t)
  print("---------------------------------")

   
def cmd(c):
  print("\n" + c)
  r = os.system(c)       
  if r:
    print("### Error: " + str(r))

def lvgl_clone():
  title("lvgl: Clone")
  cmd("git clone https://github.com/lvgl/lvgl.git")
  os.chdir("./lvgl")
  cmd("git co master") 

def lvgl_format():
  title("lvgl: Run code formatter")
  os.chdir("./scripts")
  cmd("./code-format.sh")
  os.system("git ci -am 'Run code formatter'")
  os.chdir("..")

def lvgl_update_version():
  title("lvgl: Update version number")

  f = open("./lvgl.h", "r")
      
  outbuf = ""
  major_ver = -1
  minor_ver = -1
  patch_ver = -1
      
  for i in f.read().splitlines():
    r = re.search(r'^#define LVGL_VERSION_MAJOR ', i)
    if r: 
      m = lastNum.search(i)
      if m: major_ver = m.group(1)

    r = re.search(r'^#define LVGL_VERSION_MINOR ', i)
    if r: 
      m = lastNum.search(i)
      if m: minor_ver = m.group(1)
      
    r = re.search(r'^#define LVGL_VERSION_PATCH ', i)
    if r: 
      m = lastNum.search(i)
      if m: patch_ver = m.group(1)
     
      
    r = re.search(r'^#define LVGL_VERSION_INFO ', i)
    if r: 
      i = "#define LVGL_VERSION_INFO \"\""
       
    outbuf += i + '\n'
   
  f.close()

  f = open("./lvgl.h", "w")
      
  f.write(outbuf)
  f.close()
    
  s = "v" + str(major_ver) + "." + str(minor_ver) + "." + str(patch_ver)
  print("New version:" + s)
  return s



def lvgl_update_library_json(v):
  title("lvgl: Update version number in library.json")

  f = open("./library.json", "r")
      
  outbuf = ""
      
  for i in f.read().splitlines():
    r = re.search(r'"version": ', i)
    if r: 
      i = '    "version": "' + v + '",'
       
    outbuf += i + '\n'
   
  f.close()

  f = open("./library.json", "w")
      
  f.write(outbuf)
  f.close()

def lvgl_update_lv_conf_templ(ver_str):
  title("lvgl: Update version number in lv_conf_template.h")
  cmd("sed -i -r 's/v[0-9]+\.[0-9]+\.[0-9]+/"+ ver_str +"/' lv_conf_template.h ")


def lvgl_commit_push(v):
  title("lvgl: commit and push release")

  os.system('git ci -am "Release ' + v + '"')
  cmd('git tag -a ' + v + ' -m "Release ' + v +'"')
  cmd('git push origin master')
  cmd('git push origin ' + v)


def lvgl_merge_to_release_branch(v):
  title("lvgl: merge to release branch")
  cmd('git co release/v7')
  cmd('git merge master')
  cmd('git push origin release/v7')
  os.chdir("../")
  
  
def lvgl_update_api_docs():
  title("lvgl: Update API with Doxygen")

  cmd("cd scripts; doxygen");


def examples_clone():
  title("examples: Clone")
  cmd("git clone https://github.com/lvgl/lv_examples.git")
  os.chdir("./lv_examples")
  cmd("git co master") 

def examples_commit_push(v):
  title("examples: commit and push release")

  os.system('git ci -am "Release ' + v + '"')
  cmd('git tag -a ' + v + ' -m "Release ' + v +'"')
  cmd('git push origin master')
  cmd('git push origin ' + v)


def examples_merge_to_release_branch(v):
  title("examples: merge to release branch")
  cmd('git co release/v7')
  cmd('git merge master')
  cmd('git push origin release/v7')
  os.chdir("../")
  
  
def drivers_clone():
  title("drivers: Clone")
  cmd("git clone https://github.com/lvgl/lv_drivers.git")
  os.chdir("./lv_drivers")
  cmd("git co master") 

def drivers_commit_push(v):
  title("drivers: commit and push release")

  os.system('git ci -am "Release ' + v + '"')
  cmd('git tag -a ' + v + ' -m "Release ' + v +'"')
  cmd('git push origin master')
  cmd('git push origin ' + v)

def drivers_merge_to_release_branch(v):
  title("drivers: merge to release branch")
  cmd('git co release/v7')
  cmd('git merge master')
  cmd('git push origin release/v7')
  os.chdir("../")

def docs_clone():
  title("docs: Clone")
  #cmd("git clone --recursive https://github.com/lvgl/docs.git")
  os.chdir("./docs")

def docs_get_api():
  title("docs: Get API files")
  
  cmd("git co latest --") 
  cmd("rm -rf xml");  
  cmd("cp -r ../lvgl/docs/api_doc/xml .");  
  cmd("git add xml");
  cmd('git commit -m "update API"')  

def docs_update_version(v):
  title("docs: Update version number")

  f = open("./conf.py", "r")
      
  outbuf = ""
      
  for i in f.read().splitlines():
    r = re.search(r'^version = ', i)
    if r: 
      i = "version = '" + v + "'"

    r = re.search(r'^release = ', i)
    if r: 
      i = "version = '" + v + "'"
       
    outbuf += i + '\n'
   
  f.close()

  f = open("./conf.py", "w")
      
  f.write(outbuf)
  f.close()
  cmd("git add conf.py")
  cmd('git ci -m "update conf.py to ' + v + '"')

def docs_build():
  title("docs: Build")
  cmd("git checkout master")
  cmd("./update.py latest")
  
def clean_up():
  title("Clean up repos")
  os.chdir("../")
  cmd("rm -rf lvgl docs lv_examples lv_drivers")

lvgl_clone()
lvgl_format()
lvgl_update_api_docs()
ver_str = lvgl_update_version()    
lvgl_update_library_json(ver_str)  
lvgl_update_lv_conf_templ(ver_str)
lvgl_commit_push(ver_str)
lvgl_merge_to_release_branch(ver_str)

examples_clone()
examples_commit_push(ver_str)
examples_merge_to_release_branch(ver_str)

drivers_clone()
drivers_commit_push(ver_str)
drivers_merge_to_release_branch(ver_str)

docs_clone()
docs_get_api()
docs_update_version(ver_str)
docs_build()    

clean_up()
