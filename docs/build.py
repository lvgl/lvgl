#!/usr/bin/env python3

# ****************************************************************************
# IMPOTRANT: If you are getting a lexer error for an example you need to check
#            for extra lines at the edn of the file. Only a single empty line
#            is allowed!!! Ask me how long it took me to figure this out
# ****************************************************************************

import sys
import os
import subprocess
import re
import example_list as ex
import doc_builder


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
status, br = subprocess.getstatusoutput("git branch")
_, gitcommit = subprocess.getstatusoutput("git rev-parse HEAD")
br = re.sub('\* ', '', br)

# Generate the list of examples
ex.exec()

urlpath = re.sub('release/', '', br)

os.environ['LVGL_URLPATH'] = urlpath
os.environ['LVGL_GITCOMMIT'] = gitcommit

clean = 0
trans = 0
skip_latex = False
args = sys.argv[1:]

if len(args) >= 1:
    if "clean" in args:
        clean = 1
    if "skip_latex" in args:
        skip_latex = True

lang = "en"
print("")
print("****************")
print("Building")
print("****************")

if clean:
    api_path = os.path.join(dname, 'API')
    out_path = os.path.join(dname, '..', 'out_html')
    xml_path = os.path.join(dname, 'xml')
    doxy_path = os.path.join(dname, 'doxygen_html')

    import shutil

    if os.path.exists(api_path):
        shutil.rmtree(api_path)

    if os.path.exists(lang):
        shutil.rmtree(lang)

    if os.path.exists(out_path):
        shutil.rmtree(out_path)

    if os.path.exists(xml_path):
        shutil.rmtree(xml_path)

    if os.path.exists(doxy_path):
        shutil.rmtree(doxy_path)

    os.mkdir(api_path)
    os.mkdir(lang)

print("Running doxygen")
cmd("cd ../scripts && doxygen Doxyfile")


doc_builder.run(
    os.path.join(dname, 'layouts'),
    os.path.join(dname, 'libs'),
    os.path.join(dname, 'others'),
    os.path.join(dname, 'overview'),
    os.path.join(dname, 'overview', 'renderers'),
    os.path.join(dname, 'porting'),
    os.path.join(dname, 'widgets')
)

# we make sure to remove the link to the PDF before the PDF get generated
# doesn't make any sense to have a link to the PDF in the PDF. The link gets
# added if there is a PDF build so the HTML build will have the link.
index_path = os.path.join(dname, 'index.rst')

with open(index_path, 'rb') as f:
    index_data = f.read().decode('utf-8')

if 'PDF version: :download:`LVGL.pdf <LVGL.pdf>`' in index_data:
    index_data = index_data.replace(
        'PDF version: :download:`LVGL.pdf <LVGL.pdf>`\n',
        ''
    )
    with open(index_path, 'wb') as f:
        f.write(index_data.encode('utf-8'))

# BUILD PDF
if skip_latex:
    print("skipping latex build as requested")
else:
    # Silly workaround to include the more or less correct
    # PDF download link in the PDF
    # cmd("cp -f " + lang +"/latex/LVGL.pdf LVGL.pdf | true")
    cmd("sphinx-build -b latex . out_latex -j {cpu}".format(cpu=os.cpu_count()))

    # Generate PDF
    cmd("cd out_latex && latexmk -pdf 'LVGL.tex'")

    # Copy the result PDF to the main directory to make
    # it available for the HTML build
    cmd("cd out_latex && cp -f LVGL.pdf ../LVGL.pdf")

    # add the PDF link so the HTML build will have it.
    index_data = 'PDF version: :download:`LVGL.pdf <LVGL.pdf>`\n' + index_data

    with open(index_path, 'wb') as f:
        f.write(index_data.encode('utf-8'))

# BUILD HTML

cmd('sphinx-build -b html . ../out_html -E -j {cpu}'.format(cpu=os.cpu_count()))
