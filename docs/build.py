#!/usr/bin/env python3

# ****************************************************************************
# IMPOTRANT: If you are getting a lexer error for an example you need to check
#            for extra lines at the end of the file. Only a single empty line
#            is allowed!!! Ask me how long it took me to figure this out
# ****************************************************************************

import sys
import os
import subprocess
import re
import example_list as ex
import doc_builder
import shutil
import tempfile
import config_builder
import add_translation

# due to the modifications that take place to the documentation files
# when the documentaation builds it is better to copy the source files to a
# temporary folder and modify the copies. Not setting it up this way makes it
# a real headache when making alterations that need to be committed as the
# alterations trigger the files as changed.

# If there is debugging that needs to be done you can provide a command line
# switch of "develop" and it will leave the temporary directory in tact and
# that directory will be output at the end of the build.

# the html and PDF output locations are going to remain the same as they were.
# it's just the source documentation files that are going to be copied.

temp_directory = tempfile.mkdtemp(suffix='.lvgl_docs')

langs = ['en']

# Change to script directory for consistency

base_path = os.path.abspath(os.path.dirname(__file__))
project_path = os.path.abspath(os.path.join(base_path, '..'))
examples_path = os.path.join(project_path, 'examples')

lvgl_src_path = os.path.join(project_path, 'src')
latex_output_path = os.path.join(temp_directory, 'out_latex')

pdf_src_file = os.path.join(latex_output_path, 'LVGL.pdf')
pdf_dst_file = os.path.join(temp_directory, 'LVGL.pdf')
html_src_path = temp_directory
html_dst_path = os.path.join(project_path, 'out_html')

os.chdir(base_path)


clean = 0
trans = 0
skip_latex = False
develop = False
args = sys.argv[1:]

if len(args) >= 1:
    if "clean" in args:
        clean = 1
    if "skip_latex" in args:
        skip_latex = True
    if 'develop' in args:
        develop = True


def cmd(s):
    print("")
    print(s)
    print("-------------------------------------")
    r = os.system(s)
    if r != 0:
        print("Exit build due to previous error")
        exit(-1)


# Get the current branch name
status, br = subprocess.getstatusoutput("git branch --show-current")
_, gitcommit = subprocess.getstatusoutput("git rev-parse HEAD")
br = re.sub('\* ', '', br)


urlpath = re.sub('release/', '', br)

os.environ['LVGL_URLPATH'] = urlpath
os.environ['LVGL_GITCOMMIT'] = gitcommit


lang = "en"
print("")
print("****************")
print("Building")
print("****************")

if clean:
    print('cleaning...')
    # api_path = os.path.join(dname, 'API')
    # xml_path = os.path.join(dname, 'xml')
    # doxy_path = os.path.join(dname, 'doxygen_html')

    # if os.path.exists(api_path):
    #     shutil.rmtree(api_path)

    # if os.path.exists(lang):
    #     shutil.rmtree(lang)

    if os.path.exists(html_dst_path):
        shutil.rmtree(html_dst_path)

    # if os.path.exists(xml_path):
    #     shutil.rmtree(xml_path)
    #
    # if os.path.exists(doxy_path):
    #     shutil.rmtree(doxy_path)

    # os.mkdir(api_path)
    # os.mkdir(lang)

config_builder.run()

shutil.copytree('.', temp_directory, dirs_exist_ok=True)
shutil.copytree(examples_path, os.path.join(temp_directory, 'examples'))

with open(os.path.join(temp_directory, 'Doxyfile'), 'rb') as f:
    data = f.read().decode('utf-8')

data = data.replace('#*#*LV_CONF_PATH*#*#', os.path.join(base_path, 'lv_conf.h'))
data = data.replace('*#*#SRC#*#*', '"{0}"'.format(lvgl_src_path))

with open(os.path.join(temp_directory, 'Doxyfile'), 'wb') as f:
    f.write(data.encode('utf-8'))


print("Generate the list of examples")
ex.exec(temp_directory)

print("Add translation")
add_translation.exec(temp_directory)

print("Running doxygen")
cmd('cd "{0}" && doxygen Doxyfile'.format(temp_directory))

print('Reading Doxygen output')

doc_builder.run(
    project_path,
    temp_directory,
    os.path.join(temp_directory, 'layouts'),
    os.path.join(temp_directory, 'libs'),
    os.path.join(temp_directory, 'others'),
    os.path.join(temp_directory, 'overview'),
    os.path.join(temp_directory, 'overview', 'renderers'),
    os.path.join(temp_directory, 'porting'),
    os.path.join(temp_directory, 'widgets')
)

# we make sure to remove the link to the PDF before the PDF get generated
# doesn't make any sense to have a link to the PDF in the PDF. The link gets
# added if there is a PDF build so the HTML build will have the link.
index_path = os.path.join(temp_directory, 'index.rst')

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
    cmd('sphinx-build -b latex "{src}" "{dst}" -j {cpu}'.format(
        src=temp_directory,
        dst=latex_output_path,
        cpu=os.cpu_count()
    ))

    # Generate PDF
    cmd('cd "{out_latex}" && latexmk -pdf "LVGL.tex"'.format(
        out_latex=latex_output_path
    ))

    # Copy the result PDF to the main directory to make
    # it available for the HTML build

    shutil.copyfile(pdf_src_file, pdf_dst_file)
    # cmd("cd out_latex && cp -f LVGL.pdf ../LVGL.pdf")

    # add the PDF link so the HTML build will have it.
    index_data = 'PDF version: :download:`LVGL.pdf <LVGL.pdf>`\n' + index_data

    with open(index_path, 'wb') as f:
        f.write(index_data.encode('utf-8'))

# BUILD HTML


def get_version():
    path = os.path.join(project_path, 'lvgl.h')
    with open(path, 'rb') as f:
        d = f.read().decode('utf-8')

    d = d.split('#define LVGL_VERSION_MAJOR', 1)[-1]
    major, d = d.split('\n', 1)
    d = d.split('#define LVGL_VERSION_MINOR', 1)[-1]
    minor, d = d.split('\n', 1)
    # d = d.split('#define LVGL_VERSION_PATCH', 1)[-1]
    # patch, d = d.split('\n', 1)

    ver = '{0}.{1}'.format(major.strip(), minor.strip())

    # ver = '{0}.{1}.{2}'.format(major.strip(), minor.strip(), patch.strip())

    # if '#define LVGL_VERSION_INFO' in d:
    #     d = d.split('#define LVGL_VERSION_INFO', 1)[-1]
    #     info, d = d.split('\n', 1)
    #     info = info.strip().replace('"', '')
    #     ver += '-' + info

    return ver

cmd('sphinx-build -b html "{src}" "{dst}" -D version="{version}" -E -j {cpu}'.format(
    src=html_src_path,
    dst=html_dst_path,
    version=get_version(),
    cpu=os.cpu_count()
))

if develop:
    print('temp directory:', temp_directory)
else:
    def iter_temp(p):
        folders = []
        remove_folder = True
        for temp_file in os.listdir(p):
            temp_file = os.path.join(p, temp_file)
            if os.path.isdir(temp_file):
                folders.append(temp_file)
            else:
                try:
                    os.remove(temp_file)
                except OSError:
                    remove_folder = False

        for folder in folders:
            if not iter_temp(folder):
                remove_folder = False

        if remove_folder:
            try:
                os.rmdir(p)
            except OSError:
                remove_folder = False

        return remove_folder

    iter_temp(temp_directory)

config_builder.cleanup()

print('output path:', html_dst_path)
print('\nFINISHED!!')
