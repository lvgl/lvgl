import re
import os
lastNum = re.compile(r'(?:[^\d]*(\d+)[^\d]*)+')

   
def title(t):
	print("\n---------------------------------")
	print(t)
	print("---------------------------------")

   
def cmd(c):
	print("\n" + c)
	os.system(c)		   
   

def increment(s):
    """ look for the last sequence of number(s) in a string and increment """
    m = lastNum.search(s)
    if m:
        next = str(int(m.group(1))+1)
        start, end = m.span(1)
        s = s[:max(end-len(next), start)] + next + s[end:]
    return s, str(next)


def lvgl_clone():
	title("lvgl: Clone")
	cmd("git clone https://github.com/littlevgl/lvgl.git")
	os.chdir("./lvgl")
	cmd("git co master") 

def lvgl_update_version():
	title("lvgl: Update version number")

	f = open("./src/lv_version.h", "r")
		  
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
		  i, patch_ver = increment(i)
		 
		  
		r = re.search(r'^#define LVGL_VERSION_INFO ', i)
		if r: 
		  i = "#define LVGL_VERSION_INFO \"\""
		   
		outbuf += i + '\n'
	 
	f.close()

	f = open("./src/lv_version.h", "w")
		  
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

def lvgl_commit_push(v):
	title("lvgl: commit and push release")

	cmd('git ci -am "Release ' + v + '"')
	cmd('git tag -a ' + v + ' -m "Release ' + v +'"')
	cmd('git push origin master')
	cmd('git push origin ' + v)
	
def lvgl_update_api_docs():
	title("lvgl: Update API with Doxygen")

	cmd("cd scripts; doxygen");


def docs_clone():
	title("docs: Clone")
	os.chdir("../")
	cmd("git clone --recursive https://github.com/littlevgl/docs.git")
	os.chdir("./docs")
	cmd("git co master") 

def docs_get_api():
	title("docs: Get API files")
	
	cmd("rm -rf xml");	
	cmd("cp -r ../lvgl/docs/api_doc/xml .");	


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

def docs_update_trans():
	title("docs: Update translations")
	cmd("cd en && ./trans_push.py && ./trans_pull.py")

def docs_build():
	title("docs: Build")
	cmd("./build.py clean")

	
def docs_commit_push(v):
	title("docs: Commit release")

	cmd('git add .')
	cmd('git ci -am "Release ' + v + '"')
	cmd('git tag -a ' + v + ' -m "Release ' + v +'"')
	cmd('git push origin master')
	cmd('git push origin ' + v)
	
def clean_up():
	title("Clean up repos")
	os.chdir("..")
	cmd("rm -rf lvgl docs")

lvgl_clone()
lvgl_update_api_docs()
ver_str = lvgl_update_version()    
lvgl_update_library_json(ver_str)
lvgl_commit_push(ver_str)

docs_clone()
docs_get_api()
docs_update_version(ver_str)
docs_update_trans()
docs_build()    
docs_commit_push(ver_str)

clean_up()
    
