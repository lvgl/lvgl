#!/usr/bin/env python3
""" build.py -- Generate LVGL documentation using Doxygen and Sphinx + Breathe.

Synopsis
--------
    - $ python build.py html  [ skip_api ] [ fresh_env ]
    - $ python build.py latex [ skip_api ] [ fresh_env ]
    - $ python build.py temp  [ skip_api ]
    - $ python build.py clean
    - $ python build.py clean_temp
    - $ python build.py clean_html
    - $ python build.py clean_latex

    Build Arguments and Clean Arguments can be used one at a time
    or be freely mixed and combined.

Description
-----------
    Source files are copied to a temporary directory and modified there before
    doc generation occurs.  If a full rebuild is being done (e.g. after a `clean`)
    Doxygen is run on LVGL's source files to generate intermediate API information
    in XML format, example documents are generated, API documents are generated
    for Breathe's consumption, and API links are added to the end of some documents.
    From there, Sphinx with Breathe extension uses the resulting set of source
    files to generate the desired output.

    It is only during this first build that the `skip_api` option has meaning.
    After the first build, no further actions is taken regarding API pages since
    they are not regenerated after the first build.

    The temporary directory has a fixed location (overridable by
    `LVGL_DOC_BUILD_TEMP_DIR` environment variable) and by default this
    script attempts to rebuild only those documents whose path, name or
    modification date has changed since the last build.

    Caution:

    The document build meant for end-user consumption should ONLY be done after a
    `clean` unless you know that no API documentation and no code examples have changed.

    A `sphinx-build` will do a full doc rebuild any time:

    - the temporary directory doesn't exist or is empty (since the new files in
      the temporary directory will have modification times after the generated
      HTML or Latex files, even if nothing changed),
    - the targeted output directory doesn't exist or is empty, or
    - Sphinx determines that a full rebuild is necessary.  This happens when:
        - temporary directory (Sphinx's source-file path) has changed,
        - any options on the `sphinx-build` command line have changed,
        - `conf.py` modification date has changed, or
        - `fresh_env` argument is included (runs `sphinx-build` with -E option).

    Typical run time:

    Full build:  22.5 min
    skip_api  :   1.9 min  (applies to first build only)

Options
-------
    help
        Print usage note and exit with status 0.

    html [ skip_api ] [ fresh_env ]
        Build HTML output.
        `skip_api` only has effect on first build after a `clean` or `clean_temp`.

    latex [ skip_api ] [ fresh_env ]
        Build Latex/PDF output (on hold pending removal of non-ASCII characters from input files).
        `skip_api` only has effect on first build after a `clean` or `clean_temp`.

    temp [ skip_api ]
        Generate temporary directory contents (ready to build output formats).
        If they already exist, they are removed and re-generated.

    skip_api (with `html` and/or `latex` and/or `temp` options)
        Skip API pages and links when temp directory contents are being generated
        (saving about 91% of build time).  Note: they are not thereafter regenerated
        unless requested by `temp` argument or the temp directory does not exist.
        This is intended to be used only during doc development to speed up
        turn-around time between doc modifications and seeing final results.

    fresh_env (with `html` and/or `latex` options)
        Run `sphinx-build` with -E command-line argument, which makes it regenerate its
        "environment" (memory of what was built previously, forcing a full rebuild).

    clean
        Remove all generated files.

    clean_temp
        Remove temporary directory.

    clean_html
        Remove HTML output directory.

    clean_latex
        Remove Latex output directory.

    Unrecognized arguments print error message, usage note, and exit with status 1.

Python Package Requirements
---------------------------
    The list of Python package requirements are in `requirements.txt`.

    Install them by:

    $ pip install -r requirements.txt

History
-------
    The first version of this file (Apr 2021) discovered the name of
    the current branch (e.g. 'master', 'release/v8.4', etc.) to support
    different versions of the documentation by establishing the base URL
    (used in `conf.py` and in [Edit on GitHub] links), and then ran:

    - Doxygen (to generate LVGL API XML), then
    - Sphinx

    to generate the LVGL document tree.  Internally, Sphinx uses `breathe`
    (a Sphinx extension) to provide a bridge between Doxygen XML output and
    Sphinx documentation.  It also supported a command-line option `clean`
    to remove generated files before starting (eliminates orphan files,
    for docs that have moved or changed).

    Since then its duties have grown to include:

    - Using environment variables to convey branch names to several more
      places where they are used in the docs-generating process (instead
      of re-writing writing `conf.py` and a `header.rst` each time docs
      were generated).  These are documented where they generated below.

    - Supporting additional command-line options.

    - Generating a temporary `./docs/lv_conf.h` for Doxygen to use
      (via config_builder.py).

    - Supporting multiple execution platforms (which then required tokenizing
      Doxygen's INPUT path in `Doxyfile` and re-writing portions that used
      `sed` to generate input or modify files).

    - Adding translation and API links (requiring generating docs in a
      temporary directory so that the links could be programmatically
      added to each document before Sphinx was run).  Note:  translation link
      are now done manually since they are only on the main landing page.

    - Generating EXAMPLES page + sub-examples where applicable to individual
      documents, e.g. to widget-, style-, layout-pages, etc..

    - Building PDF via latex (when working).

    - Shifting doc-generation paradigm to behave more like `make`.

"""

# ****************************************************************************
# IMPORTANT: If you are getting a PDF-lexer error for an example, check
#            for extra lines at the end of the file. Only a single empty line
#            is allowed!!! Ask me how long it took me to figure this out.
#              -- @kdschlosser
# ****************************************************************************

# Python Library
import sys
import os
import re
import subprocess
import shutil
import dirsync
from datetime import datetime

# LVGL Custom
import example_list
import doc_builder
import config_builder
import add_translation

# -------------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------------
# These are relative paths from the ./docs/ directory.
cfg_temp_dir = 'tmp'
cfg_output_dir = 'output'

# Filename generated in `cfg_latex_output_dir` and copied to `cfg_pdf_output_dir`.
cfg_pdf_filename = 'LVGL.pdf'


# -------------------------------------------------------------------------
# Print usage note.
# -------------------------------------------------------------------------
def print_usage_note():
    print('Usage:')
    print('  $ python build.py [optional_arg ...]')
    print()
    print('  where `optional_arg` can be any of these:')
    print('    html  [ skip_api ] [ fresh_env ]')
    print('    latex [ skip_api ] [ fresh_env ]')
    print('    temp  [ skip_api ]')
    print('    clean')
    print('    clean_temp')
    print('    clean_html')
    print('    clean_latex')
    print('    help')


# -------------------------------------------------------------------------
# Remove directory `tgt_dir`.
# -------------------------------------------------------------------------
def remove_dir(tgt_dir):
    if os.path.isdir(tgt_dir):
        print(f'Removing {tgt_dir}...')
        shutil.rmtree(tgt_dir)
    else:
        print(f'{tgt_dir} already removed...')


# -------------------------------------------------------------------------
# Run external command and abort build on error.
# -------------------------------------------------------------------------
def cmd(s, start_dir=None, exit_on_error=True):
    if start_dir is None:
        start_dir = os.getcwd()

    saved_dir = os.getcwd()
    os.chdir(start_dir)
    print("")
    print(s)
    print("-------------------------------------")
    result = os.system(s)
    os.chdir(saved_dir)

    if result != 0 and exit_on_error:
        print("Exiting build due to previous error.")
        sys.exit(result)


# -------------------------------------------------------------------------
# Build and return LVGL version string from `lv_version.h`.  Updated to be
# multi-platform compatible and resilient to changes in file in compliance
# with C macro syntax.
# -------------------------------------------------------------------------
def get_version(_verfile):
    major = ''
    minor = ''

    with open(_verfile, 'r') as file:
        major_re = re.compile(r'define\s+LVGL_VERSION_MAJOR\s+(\d+)')
        minor_re = re.compile(r'define\s+LVGL_VERSION_MINOR\s+(\d+)')

        for line in file.readlines():
            # Skip if line not long enough to match.
            if len(line) < 28:
                continue

            match = major_re.search(line)
            if match is not None:
                major = match[1]
            else:
                match = minor_re.search(line)
                if match is not None:
                    minor = match[1]
                    # Exit early if we have both values.
                    if len(major) > 0 and len(minor) > 0:
                        break

    return f'{major}.{minor}'


# -------------------------------------------------------------------------
# Provide answer to question:  Can we have reasonable confidence that
# the contents of `temp_directory` already exists?
# -------------------------------------------------------------------------
def temp_dir_contents_exists(_tmpdir):
    result = False
    c1 = os.path.isdir(_tmpdir)

    if c1:
        temp_path = os.path.join(_tmpdir, 'CHANGELOG.rst')
        c2 = os.path.exists(temp_path)
        temp_path = os.path.join(_tmpdir, 'CODING_STYLE.rst')
        c3 = os.path.exists(temp_path)
        temp_path = os.path.join(_tmpdir, 'CONTRIBUTING.rst')
        c4 = os.path.exists(temp_path)
        temp_path = os.path.join(_tmpdir, '_ext')
        c5 = os.path.isdir(temp_path)
        temp_path = os.path.join(_tmpdir, '_static')
        c6 = os.path.isdir(temp_path)
        temp_path = os.path.join(_tmpdir, 'details')
        c7 = os.path.isdir(temp_path)
        temp_path = os.path.join(_tmpdir, 'intro')
        c8 = os.path.isdir(temp_path)
        temp_path = os.path.join(_tmpdir, 'examples')
        c9 = os.path.isdir(temp_path)
        result = c2 and c3 and c4 and c5 and c6 and c7 and c8 and c9

    return result


# -------------------------------------------------------------------------
# Perform doc-build function(s) requested.
# -------------------------------------------------------------------------
def run():
    # ---------------------------------------------------------------------
    # Process args.
    #
    # With argument `docs_dev`, Sphinx will generate docs from a fixed
    # temporary directory that can be then used later using the same
    # command line to get Sphinx to ONLY rebuild changed documents.
    # This saves a huge amount of time during long document projects.
    # ---------------------------------------------------------------------
    # Set defaults.
    clean_temp = False
    clean_html = False
    clean_latex = False
    clean_all = False
    build_html = False
    build_latex = False
    build_temp = False
    skip_api = False
    fresh_sphinx_env = False
    args = sys.argv[1:]

    # No args means print usage note and exit with status 0.
    if len(args) == 0:
        print_usage_note()
        exit(0)

    # Some args are present.  Interpret them in loop here.
    # Unrecognized arg means print error, print usage note, exit with status 1.
    for arg in args:
        # We use chained `if-elif-else` instead of `match` for those on Linux
        # systems that will not have the required version 3.10 of Python yet.
        if arg == 'help':
            print_usage_note()
            exit(0)
        elif arg == "temp":
            build_temp = True
        elif arg == "html":
            build_html = True
        elif arg == "latex":
            build_latex = True
        elif arg == 'skip_api':
            skip_api = True
        elif arg == 'fresh_env':
            fresh_sphinx_env = True
        elif arg == "clean":
            clean_all = True
            clean_temp = True
            clean_html = True
            clean_latex = True
        elif arg == "clean_temp":
            clean_temp = True
        elif arg == "clean_html":
            clean_html = True
        elif arg == "clean_latex":
            clean_latex = True
        else:
            print(f'Argument [{arg}] not recognized.')
            print()
            print_usage_note()
            exit(1)

    # ---------------------------------------------------------------------
    # Start.
    # ---------------------------------------------------------------------
    t0 = datetime.now()

    # ---------------------------------------------------------------------
    # Set up paths.
    # ---------------------------------------------------------------------
    base_path = os.path.abspath(os.path.dirname(__file__))
    project_path = os.path.abspath(os.path.join(base_path, '..'))
    examples_path = os.path.join(project_path, 'examples')
    lvgl_src_path = os.path.join(project_path, 'src')
    output_path = os.path.join(base_path, cfg_output_dir)
    html_output_path = os.path.join(output_path, 'html')
    latex_output_path = os.path.join(output_path, 'latex')
    pdf_src_file = os.path.join(latex_output_path, cfg_pdf_filename)
    pdf_dst_file = os.path.join(base_path, cfg_pdf_filename)
    version_source_path = os.path.join(project_path, 'lv_version.h')

    # Establish temporary directory.  The presence of environment variable
    # `LVGL_DOC_BUILD_TEMP_DIR` overrides default in `cfg_temp_dir`.
    #
    # Temporary directory is used as Sphinx source directory -- some source
    # files are edited, some are fully generated.  Adding translation
    # links can be done with by adding temp in:
    # - ./docs/add_translation.py, and
    # - ./docs/_ext/link_roles.py.
    # Currently, translation-link editing is being suppressed since there
    # is only 1 file that gets the link:  top-level landing page.
    if 'LVGL_DOC_BUILD_TEMP_DIR' in os.environ:
        temp_directory = os.environ['LVGL_DOC_BUILD_TEMP_DIR']
    else:
        temp_directory = os.path.join(base_path, cfg_temp_dir)

    html_src_path = temp_directory
    print(f'Temporary directory:  [{temp_directory}]')

    # ---------------------------------------------------------------------
    # Change to script directory for consistency.
    # ---------------------------------------------------------------------
    os.chdir(base_path)

    # ---------------------------------------------------------------------
    # Clean?  If so, clean (like `make clean`), but do not exit.
    # ---------------------------------------------------------------------
    some_cleaning_to_be_done = clean_temp or clean_html or clean_latex or clean_all \
        or (os.path.isdir(temp_directory) and build_temp)

    if some_cleaning_to_be_done:
        print("****************")
        print("Cleaning...")
        print("****************")

        if clean_temp:
            remove_dir(temp_directory)

        if clean_html:
            remove_dir(html_output_path)

        if clean_latex:
            remove_dir(latex_output_path)

        if clean_all:
            remove_dir(output_path)

        if os.path.isdir(temp_directory) and build_temp:
            remove_dir(temp_directory)

    # ---------------------------------------------------------------------
    # Populate LVGL_URLPATH and LVGL_GITCOMMIT environment variables:
    #   - LVGL_URLPATH   <= 'master' or '8.4' '9.2' etc.
    #   - LVGL_GITCOMMIT <= same (see 03-Oct-2024 note below).
    #
    # These supply input later in the doc-generation process as follows:
    #
    # LVGL_URLPATH is used by:
    #   - `conf.py` to build `html_baseurl` for Sphinx for
    #       - generated index
    #       - generated search window
    #       - establishing canonical page for search engines
    #   - `link_roles.py` to generate translation links
    #   - `doc_builder.py` to generate links to API pages
    #
    # LVGL_GITCOMMIT is used by:
    #   - `conf.py` => html_context['github_version'] for
    #     Sphinx Read-the-Docs theme to add to [Edit on GitHub] links
    #   - `conf.py` => repo_commit_hash for generated EXAMPLES pages for:
    #       - [View on GitHub] buttons (view C code examples)
    #       - [View on GitHub] buttons (view Python code examples)
    # ---------------------------------------------------------------------
    # 03-Oct-2024:  Gabor requested LVGL_GITCOMMIT be changed to a branch
    # name since that will always be current, and it will fix a large
    # number of broken links on the docs website, since commits that
    # generated docs can sometimes go away.  This gets used in:
    # - [Edit on GitHub] links in doc pages (via Sphinx theme), and
    # - [View on GitHub] links in example pages (via `example_list.py`
    #   and `lv_example.py`).
    # Original code:
    # status, br = subprocess.getstatusoutput("git branch --show-current")
    # _, gitcommit = subprocess.getstatusoutput("git rev-parse HEAD")
    # br = re.sub(r'\* ', '', br)
    #   're' was previously used to remove leading '* ' from current branch
    #   string when we were parsing output from bare `git branch` output.
    #   This is no longer needed with `--show-current` option now used.
    # ---------------------------------------------------------------------
    status, branch = subprocess.getstatusoutput("git branch --show-current")

    # If above failed (i.e. `branch` not valid), default to 'master'.
    if status != 0:
        branch = 'master'
    elif branch == 'master':
        # Expected in most cases.  Nothing to change.
        pass
    else:
        # `branch` is valid.  Capture release version if in a 'release/' branch.
        if branch.startswith('release/'):
            branch = branch[8:]
        else:
            # Default to 'master'.
            branch = 'master'

    os.environ['LVGL_URLPATH'] = branch
    os.environ['LVGL_GITCOMMIT'] = branch

    # ---------------------------------------------------------------------
    # Copy files to 'temp_directory' where they will be edited (translation
    # link(s) and API links) before being used to generate new docs.
    # ---------------------------------------------------------------------
    # dirsync `exclude_list` = list of regex patterns to exclude.
    exclude_list = [r'lv_conf\.h', r'^tmp.*', r'^output.*']

    if temp_dir_contents_exists(temp_directory):
        # We are just doing an update of the temp_directory contents.
        print("****************")
        print("Updating temp directory...")
        print("****************")

        exclude_list.append(r'examples.*')
        options = {
            'verbose': True,   # Report files copied.
            'create': True,    # Create directories if they don't exist.
            'twoway': False,   # False means data flow only src => tgt.
            'purge': False,    # False means DO NOT remove orphan files/dirs in tgt dir (preserving examples/ dir).
            'exclude': exclude_list
        }
        # action == 'sync' means copy files even when they do not already exist in tgt dir.
        # action == 'update' means DO NOT copy files when they do not already exist in tgt dir.
        dirsync.sync('.', temp_directory, 'sync', **options)
        dirsync.sync(examples_path, os.path.join(temp_directory, 'examples'), 'sync', **options)
    elif build_temp or build_html or build_latex:
        # We are having to create the temp_directory contents by copying.
        print("****************")
        print("Building temp directory...")
        print("****************")

        copy_method = 1

        # Both of these methods work.
        if copy_method == 0:
            # --------- Method 0:
            ignore_func = shutil.ignore_patterns('tmp*', 'output*')
            print('Copying docs...')
            shutil.copytree('.', temp_directory, ignore=ignore_func, dirs_exist_ok=True)
            print('Copying examples...')
            shutil.copytree(examples_path, os.path.join(temp_directory, 'examples'), dirs_exist_ok=True)
        else:
            # --------- Method 1:
            options = {
               'create': True,    # Create directories if they don't exist.
               'exclude': exclude_list
            }
            # action == 'sync' means copy files even when they do not already exist in tgt dir.
            # action == 'update' means DO NOT copy files when they do not already exist in tgt dir.
            print('Copying docs...')
            dirsync.sync('.', temp_directory, 'sync', **options)
            print('Copying examples...')
            dirsync.sync(examples_path, os.path.join(temp_directory, 'examples'), 'sync', **options)

        # -----------------------------------------------------------------
        # Build Example docs, Doxygen output, API docs, and API links.
        # -----------------------------------------------------------------
        t1 = datetime.now()

        # Build local lv_conf.h from lv_conf_template.h for this build only.
        config_builder.run()

        # Replace tokens in Doxyfile in 'temp_directory' with data from this run.
        with open(os.path.join(temp_directory, 'Doxyfile'), 'rb') as f:
            data = f.read().decode('utf-8')

        data = data.replace('<<LV_CONF_PATH>>', os.path.join(base_path, 'lv_conf.h'))
        data = data.replace('<<SRC>>', '"{0}"'.format(lvgl_src_path))

        with open(os.path.join(temp_directory, 'Doxyfile'), 'wb') as f:
            f.write(data.encode('utf-8'))

        # -----------------------------------------------------------------
        # Generate examples pages.  Include sub-pages pages that get included
        # in individual documents where applicable.
        # -----------------------------------------------------------------
        print("Generating examples...")
        example_list.exec(temp_directory)

        # -----------------------------------------------------------------
        # Add translation links.
        # This is being skipped in favor of a manually-placed
        # translation link at the top of `./docs/index.rst`.
        # -----------------------------------------------------------------
        if True:
            print("Skipping adding translation links.")
        else:
            print("Adding translation links...")
            add_translation.exec(temp_directory)

        # ---------------------------------------------------------------------
        # Generate API pages and links thereto.
        # ---------------------------------------------------------------------
        if skip_api:
            print("Skipping API generation as requested.")
        else:
            print("Running Doxygen...")
            cmd('doxygen Doxyfile', temp_directory)

            print("API page and link processing...")
            doc_builder.EMIT_WARNINGS = False

            # Create .RST files for API pages, plus
            # add API hyperlinks to .RST files in the directories in passed array.
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
                # Note:  details/main-components/display omitted intentionally,
                # since API links for those .RST files have been added manually.
                os.path.join(temp_directory, 'details', 'other-components'),
                os.path.join(temp_directory, 'details', 'widgets')
            )

        t2 = datetime.now()
        print('Example/API processing run time:  ' + str(t2 - t1))

    # ---------------------------------------------------------------------
    # Build PDF
    # ---------------------------------------------------------------------
    if not build_latex:
        print("Skipping Latex build.")
    else:
        t1 = datetime.now()
        print("****************")
        print("Building Latex output...")
        print("****************")

        # Remove PDF link so PDF does not have a link to itself.
        index_path = os.path.join(temp_directory, 'index.rst')

        with open(index_path, 'rb') as f:
            index_data = f.read().decode('utf-8')

        # Support both Windows and Linux platforms with `os.linesep`.
        pdf_link_ref_str = 'PDF version: :download:`LVGL.pdf <LVGL.pdf>`' + os.linesep
        if pdf_link_ref_str in index_data:
            index_data = index_data.replace(pdf_link_ref_str, '')

            with open(index_path, 'wb') as f:
                f.write(index_data.encode('utf-8'))

        # Silly workaround to include the more or less correct
        # PDF download link in the PDF
        # cmd("cp -f " + lang +"/latex/LVGL.pdf LVGL.pdf | true")
        src = temp_directory
        dst = output_path
        cpu = os.cpu_count()
        cmd_line = f'sphinx-build -M latex "{src}" "{dst}" -j {cpu}'
        cmd(cmd_line)

        # Generate PDF.
        print("****************")
        print("Building PDF...")
        print("****************")
        cmd_line = 'latexmk -pdf "LVGL.tex"'
        cmd(cmd_line, latex_output_path, True)

        # Copy the result PDF to the main directory to make
        # it available for the HTML build.
        shutil.copyfile(pdf_src_file, pdf_dst_file)

        # Add PDF link back in so HTML build will have it.
        index_data = pdf_link_ref_str + index_data

        with open(index_path, 'wb') as f:
            f.write(index_data.encode('utf-8'))

        t2 = datetime.now()
        print('PDF               :  ' + pdf_dst_file)
        print('Latex gen run time:  ' + str(t2 - t1))

    # ---------------------------------------------------------------------
    # Build HTML
    # ---------------------------------------------------------------------
    if not build_html:
        print("Skipping HTML build.")
    else:
        t1 = datetime.now()
        print("****************")
        print("Building HTML output...")
        print("****************")

        # Note:  While it can be done (e.g. if one needs to set a stop point
        # in Sphinx code for development purposes), it is NOT a good idea to
        # run Sphinx from script as
        #   from sphinx.cmd.build import main as sphinx_build
        #   sphinx_args = [...]
        #   sphinx_build(sphinx_args)
        # because it takes ~10X longer to run than `sphinx_build` executable.
        # Literally > 3 hours.

        # '-E' option forces Sphinx to rebuild its environment so all docs are
        # fully regenerated, even if not changed.
        # Note:  Sphinx runs in ./docs/, but uses `temp_directory` for input.
        if fresh_sphinx_env:
            print("Regenerating all files...")
            env_opt = '-E'
        else:
            print("Regenerating only updated files...")
            env_opt = ''

        ver = get_version(version_source_path)
        src = html_src_path
        dst = output_path
        cpu = os.cpu_count()
        cmd_line = f'sphinx-build -M html "{src}" "{dst}" -D version="{ver}" {env_opt} -j {cpu}'
        cmd(cmd_line)
        t2 = datetime.now()
        print('HTML gen time :  ' + str(t2 - t1))

    # ---------------------------------------------------------------------
    # Remove temporary `lv_conf.h` created for this build.
    # Do this even when `lv_conf.h` was not generated in case a prior run got interrupted.
    # ---------------------------------------------------------------------
    config_builder.cleanup()

    # ---------------------------------------------------------------------
    # Indicate results.
    # ---------------------------------------------------------------------
    t_end = datetime.now()
    print('Total run time:  ' + str(t_end - t0))
    print()
    print('Note:  warnings about `/details/index.rst` and `/intro/index.rst`')
    print('       "not being in any toctree" are expected and intentional.')
    print()
    print('Done.')


# -------------------------------------------------------------------------
# Make module importable as well as run-able.
# -------------------------------------------------------------------------
if __name__ == '__main__':
    run()
