#!/usr/bin/env python3
import os


def process_index_rst(path):
#  print(path)
  with open(path) as fp:
    last_line=""
    line=""
    title_tmp=""
    line = fp.readline()
    d = {}
    while line:
      if line[0:3] == '"""':
        title_tmp = last_line
      elif line[0:15] ==".. lv_example::":
        name = line[16:].strip()
        title_tmp = title_tmp.strip()
        d[name] = title_tmp
      last_line = line
      line = fp.readline()

    return(d)
      
h1= {
  "get_started":"Get started", 
  "styles":"Styles", 
  "anim":"Animations", 
  "event":"Events", 
  "layouts":"Layouts", 
  "scroll":"Scrolling",
  "widgets":"Widgets"
}

widgets = {
"obj":"Base object",
"arc":"Arc",
"bar":"Bar",
"btn":"Button",
"btnmatrix":"Button matrix",
"calendar":"Calendar",
"canvas":"Canvas",
"chart":"Chart",
"checkbox":"Checkbox",
"colorwheel":"Colorwheel",
"dropdown":"Dropdown",
"img":"Image",
"imgbtn":"Image button",
"keyboard":"Keyboard",
"label":"Label",
"led":"LED",
"line":"Line",
"list":"List",
"meter":"Meter",
"msgbox":"Message box",
"roller":"Roller",
"slider":"Slider",
"span":"Span",
"spinbox":"Spinbox",
"spinner":"Spinner",
"switch":"Switch",
"table":"Table",
"tabview":"Tabview",
"textarea":"Textarea",
"tileview":"Tabview",
"win":"Window",
}

layouts = {
"flex":"Flex",
"grid":"Grid",
}


def print_item(path, lvl, d, fout):
  for k in d:
    v = d[k]
    if k.startswith(path + "/lv_example_"):
     fout.write("#"*lvl + " " + v + "\n")
     fout.write('```eval_rst\n')
     fout.write(f".. lv_example:: {k}\n")
     fout.write('```\n')
     fout.write("\n")

def exec():
  path ="../examples/"
  fout = open("examples.md", "w")
  filelist = []

  for root, dirs, files in os.walk(path):
	  for f in files:
      #append the file name to the list
		  filelist.append(os.path.join(root,f))

  filelist = [ fi for fi in filelist if fi.endswith("index.rst") ]

  d_all = {}
  #print all the file names
  for fn in filelist:
     d_act = process_index_rst(fn)
     d_all.update(d_act)

  fout.write("```eval_rst\n")
  fout.write(".. include:: /header.rst\n") 
  fout.write(":github_url: |github_link_base|/examples.md\n")
  fout.write("```\n")
  fout.write("\n")
  fout.write("# Examples\n")

  for h in h1:
    fout.write("## " + h1[h] + "\n")
    
    if h == "widgets":
      for w in widgets:
        fout.write("### " + widgets[w] + "\n")
        print_item(h + "/" + w, 4, d_all, fout)
    elif h == "layouts":
      for l in layouts:
        fout.write("### " + layouts[l] + "\n")
        print_item(h + "/" + l, 4, d_all, fout)
    else:
      print_item(h, 3, d_all, fout)   
      
    fout.write("")
  
