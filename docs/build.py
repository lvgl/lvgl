#!/usr/bin/env python3

import sys
import os
import subprocess
import re

langs = ['en']

# Change to script directory for consistency
abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

def cmd(s):
  print("")
  print(s)
  print("-------------------------------------")
  r = os.system(s)
  if r != 0: 
    print("Exit build due to previous error")
    exit(-1)

# Get the current branch name
status, br = subprocess.getstatusoutput("git branch | grep '*'")
br = re.sub('\* ', '', br)
urlpath = re.sub('release/', '', br)
 
# Be sure the github links point to the right branch
f = open("header.rst", "w")
f.write(".. |github_link_base| replace:: https://github.com/lvgl/docs/blob/" + br)
f.close()

base_html = "html_baseurl = 'https://docs.lvgl.io/" + urlpath + "/en/html/'"

os.system("sed -i \"s|html_baseurl = .*|" + base_html +"|\" conf.py")

clean = 0
trans = 0
args = sys.argv[1:]
if len(args) >= 1:
  if "clean" in args: clean = 1
  
lang = "en"
print("")
print("****************")
print("Building")
print("****************")
if clean:
  cmd("rm -rf " + lang)
  cmd("mkdir " + lang)


print("Running doxygen")
cmd("cd ../scripts && doxygen Doxyfile")
# BUILD PDF

# Silly workarond to include the more or less correct PDF download link in the PDF
#cmd("cp -f " + lang +"/latex/LVGL.pdf LVGL.pdf | true")
cmd("sphinx-build -b latex . out_latex")

# Generate PDF
cmd("cd out_latex && latexmk -pdf 'LVGL.tex'")
# Copy the result PDF to the main directory to make it avaiable for the HTML build
cmd("cd out_latex && cp -f LVGL.pdf ../LVGL.pdf")

# BULD HTML
cmd("sphinx-build -b html . ../out_html")

