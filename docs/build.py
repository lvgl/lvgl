#!/usr/bin/env python3

# ****************************************************************************
# IMPORTANT: If you are getting a lexer error for an example you need to check
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

# -------------------------------------------------------------------------
# Process args.
#
# Normal usage:
# $ python build.py skip_latex
#
# Other optional arguments are meant for doc development to speed up
# turn-around time between doc modification and seeing the final results:
#
# - skip_api
#       Skips generating API pages (this saves about 70% of build time).
#       This is intended to be used only during doc development to speed up
#       turn-around time between doc modifications and seeing final results.
# - no_fresh_env
#       excludes -E command-line argument to `sphinx-build`, which forces
#       generating a whole new environment (memory of what was built
#       previously, forcing a full rebuild).  "no_fresh_env" enables a
#       rebuild of only docs that got updated -- Sphinx's default behavior.
# - develop
#       Leaves temporary directory intact for docs development purposes.
# - fixed_tmp_dir
#       If (fixed_tmp_dir and 'LVGL_FIXED_TEMP_DIR' in os.environ),
#       then the temporary directory in the value of that environment
#       variable will be used instead of the normal (randomly-named)
#       temporary directory.  This is important when getting `sphinx-build`
#       to ONLY rebuild updated documents, since changing the directory
#       from which they are generated (normally the randomly-named temp
#       dir) will force Sphinx to do a full-rebuild because it remembers
#       the doc paths from which the build was last generated.
# - docs_dev
#       Forces "fresh_env" to False, and "fixed_tmp_dir" to True.  This is
#       merely a shortcut to having both "no_fresh_env" and "fixed_tmp_dir"
#       on the command line.
# - skip_trans
#       Skips adding translation links.  This allows direct copying of
#       of .RST files to `temp_directory` when they are updated to save
#       time during re-build.  Final build must not include this option
#       so that the translation links are added at the top of each page.
#
# With arguments [skip_latex, develop, docs_dev], Sphinx will generate
# docs from a fixed temporary directory that can be then used later from
# the LVGL ./docs/ directory like this:
#
# $ sphinx-build -b html  "fixed_temp_dir"  "..\out_html" -D version="9.3" -j cpu_count
#
# to only rebuild docs that have been updated.
# -------------------------------------------------------------------------
clean = 0
skip_latex = False
skip_api = False
fresh_env = True
develop = False
fixed_tmp_dir = False
docs_dev = False
skip_trans = False
args = sys.argv[1:]

if len(args) >= 1:
    if "clean" in args:
        clean = 1
    if "skip_latex" in args:
        skip_latex = True
    if 'skip_api' in args:
        skip_api = True
    if 'no_fresh_env' in args:
        fresh_env = False
    if 'develop' in args:
        develop = True
    if 'fixed_tmp_dir' in args:
        fixed_tmp_dir = True
    if 'docs_dev' in args:
        docs_dev = True
    if 'skip_trans' in args:
        skip_trans = True

# Arg ramifications...
# docs_dev implies no fresh_env
if docs_dev:
    fresh_env = False
    fixed_tmp_dir = True


# -------------------------------------------------------------------------
# Due to the modifications that take place to the documentation files
# when the documentation builds it is better to copy the source files to a
# temporary folder and modify the copies. Not setting it up this way makes it
# a real headache when making alterations that need to be committed as the
# alterations trigger the files as changed.  Also, this keeps maintenance
# effort to a minimum as adding a new language translation only needs to be
# done in 2 places (add_translation.py and ./docs/_ext/link_roles.py) rather
# than once for each .rst file.
#
# The html and PDF output locations are going to remain the same as they were.
# it's just the source documentation files that are going to be copied.
# -------------------------------------------------------------------------
if fixed_tmp_dir and 'LVGL_FIXED_TEMP_DIR' in os.environ:
    temp_directory = os.environ['LVGL_FIXED_TEMP_DIR']
else:
    temp_directory = tempfile.mkdtemp(suffix='.lvgl_docs')

print(f'Using temp directory:  [{temp_directory}]')

langs = ['en']

# -------------------------------------------------------------------------
# Set up paths.
# -------------------------------------------------------------------------
base_path = os.path.abspath(os.path.dirname(__file__))
project_path = os.path.abspath(os.path.join(base_path, '..'))
examples_path = os.path.join(project_path, 'examples')

lvgl_src_path = os.path.join(project_path, 'src')
latex_output_path = os.path.join(temp_directory, 'out_latex')

pdf_src_file = os.path.join(latex_output_path, 'LVGL.pdf')
pdf_dst_file = os.path.join(temp_directory, 'LVGL.pdf')
html_src_path = temp_directory
html_dst_path = os.path.join(project_path, 'out_html')

# -------------------------------------------------------------------------
# Change to script directory for consistency.
# -------------------------------------------------------------------------
os.chdir(base_path)

# -------------------------------------------------------------------------
# Provide a way to run an external command and abort build on error.
# -------------------------------------------------------------------------
def cmd(s, start_dir=None):
    if start_dir is None:
        start_dir = os.getcwd()

    saved_dir = os.getcwd()
    os.chdir(start_dir)
    print("")
    print(s)
    print("-------------------------------------")
    result = os.system(s)
    os.chdir(saved_dir)

    if result != 0:
        print("Exiting build due to previous error.")
        sys.exit(result)

# -------------------------------------------------------------------------
# Get current branch name
# -------------------------------------------------------------------------
# 03-Oct-2024:  Gabor requested this be changed to a branch name
# since that will always be current, and it will fix a large number
# of broken links on the docs website.  This gets used in the
# 'Edit on GitHub' links in the upper-right corner of pages.
# Original code:
# status, br = subprocess.getstatusoutput("git branch --show-current")
# _, gitcommit = subprocess.getstatusoutput("git rev-parse HEAD")
# br = re.sub(r'\* ', '', br)
status, br = subprocess.getstatusoutput("git branch --show-current")
br = re.sub(r'\* ', '', br)

# If in an unusual branch that is not 'master' or 'release/...'
# then default to 'master'.
if '/' in br and 'release' not in br:
    br = 'master'

gitcommit = br
urlpath = re.sub('release/', '', br)

# These environment variables are used in other scripts.
os.environ['LVGL_URLPATH'] = urlpath
os.environ['LVGL_GITCOMMIT'] = gitcommit


lang = "en"
print("")
print("****************")
print("Building")
print("****************")

# Remove all previous output files if 'clean' on command line.
if clean:
    print('Removing previous output files...')
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

# -------------------------------------------------------------------------
# Build local lv_conf.h from lv_conf_template.h for this build only.
# -------------------------------------------------------------------------
config_builder.run()

# -------------------------------------------------------------------------
# Copy files to 'temp_directory' where they will be edited
# (translation link and API links) before being used to generate new docs.
# -------------------------------------------------------------------------
shutil.copytree('.', temp_directory, dirs_exist_ok=True)
shutil.copytree(examples_path, os.path.join(temp_directory, 'examples'), dirs_exist_ok=True)

# -------------------------------------------------------------------------
# Replace tokens in Doxyfile in 'temp_directory' with data from this run.
# -------------------------------------------------------------------------
with open(os.path.join(temp_directory, 'Doxyfile'), 'rb') as f:
    data = f.read().decode('utf-8')

data = data.replace('#*#*LV_CONF_PATH*#*#', os.path.join(base_path, 'lv_conf.h'))
data = data.replace('*#*#SRC#*#*', '"{0}"'.format(lvgl_src_path))

with open(os.path.join(temp_directory, 'Doxyfile'), 'wb') as f:
    f.write(data.encode('utf-8'))

# -------------------------------------------------------------------------
# Generate examples pages.
# -------------------------------------------------------------------------
print("Generating examples...")
ex.exec(temp_directory)

if skip_trans:
    print("Skipping translation links as requested.")
else:
    # ---------------------------------------------------------------------
    # Add translation links at top of all .rst files.
    # ---------------------------------------------------------------------
    print("Adding translation links...")
    add_translation.exec(temp_directory)

if skip_api:
    print("Skipping API generation as requested.")
else:
    # ---------------------------------------------------------------------
    # Generate API pages and links thereto.
    # ---------------------------------------------------------------------
    print("Running Doxygen...")
    cmd('doxygen Doxyfile', temp_directory)
    print("Generating API documentation .RST files...")

    doc_builder.EMIT_WARNINGS = False

    # Create .RST files for API pages.
    doc_builder.run(
        project_path,
        temp_directory,
        os.path.join(temp_directory, 'intro'),
        os.path.join(temp_directory, 'intro', 'add-lvgl-to-your-project'),
        os.path.join(temp_directory, 'details'),
        os.path.join(temp_directory, 'details', 'base-widget'),
        os.path.join(temp_directory, 'details', 'base-widget', 'layouts'),
        os.path.join(temp_directory, 'details', 'base-widget', 'styles'),
        os.path.join(temp_directory, 'details', 'debugging'),
        os.path.join(temp_directory, 'details', 'integration'),
        os.path.join(temp_directory, 'details', 'integration', 'bindings'),
        os.path.join(temp_directory, 'details', 'integration', 'building'),
        os.path.join(temp_directory, 'details', 'integration', 'chip'),
        os.path.join(temp_directory, 'details', 'integration', 'driver'),
        os.path.join(temp_directory, 'details', 'integration', 'driver', 'display'),
        os.path.join(temp_directory, 'details', 'integration', 'driver', 'touchpad'),
        os.path.join(temp_directory, 'details', 'integration', 'framework'),
        os.path.join(temp_directory, 'details', 'integration', 'ide'),
        os.path.join(temp_directory, 'details', 'integration', 'os'),
        os.path.join(temp_directory, 'details', 'integration', 'os', 'yocto'),
        os.path.join(temp_directory, 'details', 'integration', 'renderers'),
        os.path.join(temp_directory, 'details', 'libs'),
        os.path.join(temp_directory, 'details', 'main-components'),
        os.path.join(temp_directory, 'details', 'other-components'),
        os.path.join(temp_directory, 'details', 'widgets')
    )

    print('Reading Doxygen output...')

# -------------------------------------------------------------------------
# We make sure to remove the link to the PDF before the PDF get generated
# doesn't make any sense to have a link to the PDF in the PDF. The link gets
# added if there is a PDF build so the HTML build will have the link.
# -------------------------------------------------------------------------
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

# -------------------------------------------------------------------------
# BUILD PDF
# -------------------------------------------------------------------------
if skip_latex:
    print("Skipping latex build as requested.")
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

# -------------------------------------------------------------------------
# BUILD HTML
# -------------------------------------------------------------------------
# This version of get_version() also works correctly under Windows.
def get_version():
    path = os.path.join(project_path, 'lv_version.h')
    with open(path, 'rb') as fle:
        d = fle.read().decode('utf-8')

    d = d.split('#define LVGL_VERSION_MAJOR', 1)[-1]
    major, d = d.split('\n', 1)
    d = d.split('#define LVGL_VERSION_MINOR', 1)[-1]
    minor, d = d.split('\n', 1)

    # d = d.split('#define LVGL_VERSION_PATCH', 1)[-1]
    # patch, d = d.split('\n', 1)

    return f'{major.strip()}.{minor.strip()}'

# -------------------------------------------------------------------------
# Run Sphinx after determining whether to use -E (fresh environment)
# command-line argument.
# -------------------------------------------------------------------------
if fresh_env:
    # Uses -E option (same as --fresh-env).  Forces sphinx-build to rebuild sphinx
    # environment so that all docs are fully regenerated, even if they have not changed.
    print("Regenerating all files...")
    cmd('sphinx-build -b html "{src}" "{dst}" -D version="{version}" -E -j {cpu}'.format(
        src=html_src_path,
        dst=html_dst_path,
        version=get_version(),
        cpu=os.cpu_count()
    ))
else:
    # Does not use -E option (same as --fresh-env).
    print("Regenerating only updated files...")
    cmd('sphinx-build -b html "{src}" "{dst}" -D version="{version}" -j {cpu}'.format(
        src=html_src_path,
        dst=html_dst_path,
        version=get_version(),
        cpu=os.cpu_count()
    ))

# -------------------------------------------------------------------------
# If 'develop' was specified on command line, announce location of temp dir.
# Otherwise, remove temporary files created for the doc build.
# -------------------------------------------------------------------------
if develop:
    print('Temp directory:  ', temp_directory)
else:
    print('Removing temporary files...', temp_directory)
    # Recursively remove generated files in `temp_directory`.
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

# -------------------------------------------------------------------------
# Remove temporary `lv_conf.h` created for this build.
# -------------------------------------------------------------------------
config_builder.cleanup()

# -------------------------------------------------------------------------
# Indicate results.
# -------------------------------------------------------------------------
print('Output path:  ', html_dst_path)
print('Finished.')
