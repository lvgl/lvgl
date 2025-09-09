#!/usr/bin/env python3
""" build.py -- Generate LVGL documentation using Doxygen and Sphinx + Breathe.

Synopsis
--------
    - $ python build.py html  [ skip_api ] [ fresh_env ]
    - $ python build.py latex [ skip_api ] [ fresh_env ]
    - $ python build.py intermediate  [ skip_api ]
    - $ python build.py clean
    - $ python build.py clean_intermediate
    - $ python build.py clean_html
    - $ python build.py clean_latex

    Build Arguments and Clean Arguments can be used one at a time
    or be freely mixed and combined.

Data Flow
---------

.. code-block:: text

    Inputs              Generated Source Files             Output
    -----------         ----------------------       ----------------------
    ./docs/src/   \
    ./src/         >===> ./docs/intermediate/  ===>  ./docs/build/<format>/
    ./examples/   /

    Once ./docs/intermediate/ is built, you can use all the Sphinx output
    formats, e.g.

    - make html
    - make latex
    - make man
    - make htmlhelp
    - etc.

Description
-----------
    Copy source files to an intermediate directory and modify them there before
    doc generation occurs.  If a full rebuild is being done (e.g. after a `clean`)
    run Doxygen LVGL's source files to generate intermediate API information
    in XML format.  Generate API documents for Breathe's consumption.  Add API
    links to end of some documents.  Generate example documents.  From there,
    Sphinx with Breathe extension uses the resulting set of intermediate files
    to generate the desired output.

    It is only during this first build that the `skip_api` option has meaning.
    After the first build, no further actions is taken regarding API pages since
    they are not regenerated after the first build.

    The intermediate directory has a fixed location (overridable by
    `LVGL_DOC_BUILD_INTERMEDIATE_DIR` environment variable) and by default this
    script attempts to rebuild only those documents whose path, name or
    modification date has changed since the last build.

    The output directory also has a fixed location (overridable by
    `LVGL_DOC_BUILD_OUTPUT_DIR` environment variable).

    Caution:

    The document build meant for end-user consumption should ONLY be done after a
    `clean` unless you know that no API documentation and no code examples have changed.

    A `sphinx-build` will do a full doc rebuild any time:

    - the intermediate directory doesn't exist or is empty (since the new files in
      the intermediate directory will have modification times after the generated
      HTML or Latex files, even if nothing changed),
    - the targeted output directory doesn't exist or is empty, or
    - Sphinx determines that a full rebuild is necessary.  This happens when:
        - intermediate directory (Sphinx's source-file path) has changed,
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
        `skip_api` only has effect on first build after a `clean` or `clean_intermediate`.

    latex [ skip_api ] [ fresh_env ]
        Build Latex/PDF output (on hold pending removal of non-ASCII characters from input files).
        `skip_api` only has effect on first build after a `clean` or `clean_intermediate`.

    intermediate [ skip_api ]
        Generate intermediate directory contents (ready to build output formats).
        If they already exist, they are removed and re-generated.
        Note:  "intermediate" can be abbreviated down to "int".

    skip_api (with `html` and/or `latex` and/or `intermediate` options)
        Skip API pages and links when intermediate directory contents are being generated
        (saving about 91% of build time).  Note: they are not thereafter regenerated unless
        requested by `intermediate` argument or the intermediate directory does not
        exist.  This is intended to be used only during doc development to speed up
        turn-around time between doc modifications and seeing final results.

    fresh_env (with `html` and/or `latex` options)
        Run `sphinx-build` with -E command-line argument, which makes it regenerate its
        "environment" (memory of what was built previously, forcing a full rebuild).

    clean
        Remove all generated files.

    clean_intermediate
        Remove intermediate directory.
        Note:  "clean_intermediate" can be abbreviated down to "clean_int".

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

    - Generating a `./docs/lv_conf.h` for Doxygen to use (config_builder.py).

    - Supporting multiple execution platforms (which then required tokenizing
      Doxygen's INPUT path in `Doxyfile` and re-writing portions that used
      `sed` to generate input or modify files).

    - Adding translation and API links (requiring generating docs in an
      intermediate directory so that the links could be programmatically
      added to each document before Sphinx was run).  Note:  translation link
      are now done manually since they are only on the main landing page.

    - Generating EXAMPLES page + sub-examples where applicable to individual
      documents, e.g. to widget-, style-, layout-pages, etc.

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
import subprocess
import shutil
import dirsync
from datetime import datetime

# LVGL Custom
import example_list
import api_doc_builder
import config_builder
from src.lvgl_version import lvgl_version
from announce import *

# Not Currently Used
# (Code is kept in case we want to re-implement it later.)
# import add_translation

# -------------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------------
# These are relative paths from the ./docs/ directory.
cfg_project_dir = '..'
cfg_lvgl_src_dir = 'src'
cfg_doc_src_dir = 'src'
cfg_examples_dir = 'examples'
cfg_default_intermediate_dir = 'intermediate'
cfg_default_output_dir = 'build'
cfg_static_dir = '_static'
cfg_downloads_dir = 'downloads'
cfg_lv_conf_filename = 'lv_conf.h'
cfg_lv_version_filename = 'lv_version.h'
cfg_doxyfile_filename = 'Doxyfile'
cfg_top_index_filename = 'index.rst'
cfg_default_branch = 'master'

# Filename generated in `latex_output_dir` and copied to `pdf_output_dir`.
cfg_pdf_filename = 'LVGL.pdf'


def print_usage_note():
    """Print usage note."""
    print('Usage:')
    print('  $ python build.py [optional_arg ...]')
    print()
    print('  where `optional_arg` can be any of these:')
    print('    html  [ skip_api ] [ fresh_env ]')
    print('    latex [ skip_api ] [ fresh_env ]')
    print('    intermediate  [ skip_api ]')
    print('    clean')
    print('    clean_intermediate')
    print('    clean_html')
    print('    clean_latex')
    print('    help')


def remove_dir(tgt_dir):
    """Remove directory `tgt_dir`."""
    if os.path.isdir(tgt_dir):
        announce(__file__, f'Removing {tgt_dir}...')
        shutil.rmtree(tgt_dir)
    else:
        announce(__file__, f'{tgt_dir} already removed...')


def cmd(cmd_str, start_dir=None, exit_on_error=True):
    """Run external command and abort build on error."""
    saved_dir = None

    if start_dir is not None:
        saved_dir = os.getcwd()
        os.chdir(start_dir)

    announce(__file__, f'Running [{cmd_str}] in [{os.getcwd()}]...')
    return_code = os.system(cmd_str)

    if saved_dir is not None:
        os.chdir(saved_dir)

    if return_code != 0 and exit_on_error:
        announce(__file__, "Exiting build due to previous error.")
        sys.exit(1)


def intermediate_dir_contents_exists(dir):
    """Provide answer to question:  Can we have reasonable confidence that
    the contents of `intermediate_directory` already exists?
    """
    result = False
    c1 = os.path.isdir(dir)

    if c1:
        temp_path = os.path.join(dir, 'CHANGELOG.rst')
        c2 = os.path.exists(temp_path)
        temp_path = os.path.join(dir, '_ext')
        c3 = os.path.isdir(temp_path)
        temp_path = os.path.join(dir, '_static')
        c4 = os.path.isdir(temp_path)
        temp_path = os.path.join(dir, 'details')
        c5 = os.path.isdir(temp_path)
        temp_path = os.path.join(dir, 'intro')
        c6 = os.path.isdir(temp_path)
        temp_path = os.path.join(dir, 'contributing')
        c7 = os.path.isdir(temp_path)
        temp_path = os.path.join(dir, cfg_examples_dir)
        c8 = os.path.isdir(temp_path)
        result = c2 and c3 and c4 and c5 and c6 and c7 and c8

    return result


def run(args):
    """Perform doc-build function(s) requested."""

    # ---------------------------------------------------------------------
    # Set default settings.
    # ---------------------------------------------------------------------
    build_html = False
    build_latex = False
    build_intermediate = False
    skip_api = False
    fresh_sphinx_env = False
    clean_all = False
    clean_intermediate = False
    clean_html = False
    clean_latex = False

    def print_setting(setting_name, val):
        """Print one setting; used for debugging."""
        announce(__file__, f'{setting_name:18} = [{val}]')

    def print_settings(and_exit):
        """Print all settings and optionally exit; used for debugging."""
        print_setting("build_html", build_html)
        print_setting("build_latex", build_latex)
        print_setting("build_intermediate", build_intermediate)
        print_setting("skip_api", skip_api)
        print_setting("fresh_sphinx_env", fresh_sphinx_env)
        print_setting("clean_all", clean_all)
        print_setting("clean_intermediate", clean_intermediate)
        print_setting("clean_html", clean_html)
        print_setting("clean_latex", clean_latex)

        if and_exit:
            exit(0)

    # ---------------------------------------------------------------------
    # Process args.
    # ---------------------------------------------------------------------

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
        elif arg == "html":
            build_html = True
        elif arg == "latex":
            build_latex = True
        elif "intermediate".startswith(arg) and len(arg) >= 3:
            # Accept abbreviations.
            build_intermediate = True
        elif arg == 'skip_api':
            skip_api = True
        elif arg == 'fresh_env':
            fresh_sphinx_env = True
        elif arg == "clean":
            clean_all = True
            clean_intermediate = True
            clean_html = True
            clean_latex = True
        elif arg == "clean_html":
            clean_html = True
        elif arg == "clean_latex":
            clean_latex = True
        elif "clean_intermediate".startswith(arg) and len(arg) >= 9:
            # Accept abbreviations.
            # Needs to be after others so "cl" will not
            clean_intermediate = True
        else:
            print(f'Argument [{arg}] not recognized.')
            print()
            print_usage_note()
            exit(2)  # 2 = customary Unix command-line syntax error.

    # '-E' option forces Sphinx to rebuild its environment so all docs are
    # fully regenerated, even if not changed.
    # Note:  Sphinx runs in ./docs/, but uses `intermediate_dir` for input.
    if fresh_sphinx_env:
        announce(__file__, "Force-regenerating all files...")
        env_opt = '-E'
    else:
        env_opt = ''

    # ---------------------------------------------------------------------
    # Start.
    # ---------------------------------------------------------------------
    t0 = datetime.now()

    # ---------------------------------------------------------------------
    # Set up paths.
    #
    # Variable Suffixes:
    # _filename = filename without path
    # _path     = path leading to a file or directory (absolute or relative)
    # _file     = path leading to a file              (absolute or relative)
    # _dir      = path leading to a directory         (absolute or relative)
    # ---------------------------------------------------------------------
    base_dir = os.path.abspath(os.path.dirname(__file__))
    project_dir = os.path.abspath(os.path.join(base_dir, cfg_project_dir))
    examples_dir = os.path.join(project_dir, cfg_examples_dir)
    lvgl_src_dir = os.path.join(project_dir, cfg_lvgl_src_dir)

    # Establish intermediate directory.  The presence of environment variable
    # `LVGL_DOC_BUILD_INTERMEDIATE_DIR` overrides default in `cfg_default_intermediate_dir`.
    if 'LVGL_DOC_BUILD_INTERMEDIATE_DIR' in os.environ:
        intermediate_dir = os.environ['LVGL_DOC_BUILD_INTERMEDIATE_DIR']
    else:
        intermediate_dir = os.path.join(base_dir, cfg_default_intermediate_dir)

    lv_conf_file = os.path.join(intermediate_dir, cfg_lv_conf_filename)
    version_dst_file = os.path.join(intermediate_dir, cfg_lv_version_filename)
    top_index_file = os.path.join(intermediate_dir, cfg_top_index_filename)
    doxyfile_src_file = os.path.join(base_dir, cfg_doxyfile_filename)
    doxyfile_dst_file = os.path.join(intermediate_dir, cfg_doxyfile_filename)
    pdf_intermediate_dst_dir = os.path.join(intermediate_dir, cfg_static_dir, cfg_downloads_dir)
    pdf_intermediate_dst_file = os.path.join(pdf_intermediate_dst_dir, cfg_pdf_filename)
    sphinx_path_sep = '/'
    pdf_relative_file = cfg_static_dir + sphinx_path_sep + cfg_downloads_dir + sphinx_path_sep + cfg_pdf_filename
    pdf_link_ref_str = f'PDF Version: :download:`{cfg_pdf_filename} <{pdf_relative_file}>`'

    # Establish build directory.  The presence of environment variable
    # `LVGL_DOC_BUILD_OUTPUT_DIR` overrides default in `cfg_default_output_dir`.
    if 'LVGL_DOC_BUILD_OUTPUT_DIR' in os.environ:
        output_dir = os.environ['LVGL_DOC_BUILD_OUTPUT_DIR']
    else:
        output_dir = os.path.join(base_dir, cfg_default_output_dir)

    html_output_dir = os.path.join(output_dir, 'html')
    latex_output_dir = os.path.join(output_dir, 'latex')
    pdf_output_dir = os.path.join(output_dir, 'pdf')
    pdf_src_file = os.path.join(latex_output_dir, cfg_pdf_filename)
    pdf_dst_file = os.path.join(pdf_output_dir, cfg_pdf_filename)
    version_src_file = os.path.join(project_dir, cfg_lv_version_filename)

    # Special stuff for right-aligning PDF download link.
    # Note: this needs to be embedded in a <div> tag because the
    # Sphinx `:download:` role causes the link to appear in a <p> tag
    # and in HTML5, <p> tags cannot be nested!
    cfg_right_just_para_text = """.. raw:: html

    <div style="text-align: right;">"""
    cfg_end_right_just_para_text = """.. raw:: html

    </div>"""
    # Blank lines are required due to the directives.
    cfg_pdf_link_ref_block_str = \
        cfg_right_just_para_text + os.linesep \
        + os.linesep \
        + pdf_link_ref_str + os.linesep + \
        os.linesep \
        + cfg_end_right_just_para_text + os.linesep \
        + os.linesep

    # ---------------------------------------------------------------------
    # Change to script directory for consistent run-time environment.
    # ---------------------------------------------------------------------
    os.chdir(base_dir)
    announce(__file__, f'Intermediate dir:  [{intermediate_dir}]')
    announce(__file__, f'Output dir      :  [{output_dir}]')
    announce(__file__, f'Running from    :  [{base_dir}]')

    # ---------------------------------------------------------------------
    # Clean?  If so, clean (like `make clean`), but do not exit.
    # ---------------------------------------------------------------------
    some_cleaning_to_be_done = clean_intermediate or clean_html or clean_latex \
        or clean_all or (os.path.isdir(intermediate_dir) and build_intermediate)

    if some_cleaning_to_be_done:
        announce(__file__, "Cleaning...", box=True)

        if clean_intermediate:
            remove_dir(intermediate_dir)

        if clean_html:
            remove_dir(html_output_dir)

        if clean_latex:
            remove_dir(latex_output_dir)

        if clean_all:
            remove_dir(output_dir)

        if os.path.isdir(intermediate_dir) and build_intermediate:
            remove_dir(intermediate_dir)

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
    #   - `doxygen_xml.py` to generate links to API pages
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
        branch = cfg_default_branch
    elif branch == cfg_default_branch:
        # Expected in most cases.  Nothing to change.
        pass
    else:
        # `branch` is valid.  Capture release version if in a 'release/' branch.
        if branch.startswith('release/'):
            branch = branch[8:]
        else:
            # Default to 'master'.
            branch = cfg_default_branch

    os.environ['LVGL_URLPATH'] = branch
    os.environ['LVGL_GITCOMMIT'] = branch

    # ---------------------------------------------------------------------
    # Prep `intermediate_dir` to become the `sphinx-build` source dir.
    # ---------------------------------------------------------------------
    # dirsync `exclude_list` = list of regex patterns to exclude.
    intermediate_re = r'^' + cfg_default_intermediate_dir + r'.*'
    output_re = r'^' + cfg_default_output_dir + r'.*'
    exclude_list = [r'lv_conf\.h', r'^__pycache__.*', intermediate_re, output_re]

    if intermediate_dir_contents_exists(intermediate_dir):
        # We are just doing an update of the intermediate_dir contents.
        announce(__file__, "Updating intermediate directory...", box=True)

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
        dirsync.sync(cfg_doc_src_dir, intermediate_dir, 'sync', **options)
        dirsync.sync(examples_dir, os.path.join(intermediate_dir, cfg_examples_dir), 'sync', **options)
    elif build_intermediate or build_html or build_latex:
        # We are having to create the intermediate_dir contents by copying.
        announce(__file__, "Building intermediate directory...", box=True)

        t1 = datetime.now()
        copy_method = 1

        # Both of these methods work.
        if copy_method == 0:
            # --------- Method 0:
            ignore_func = shutil.ignore_patterns('tmp*', 'output*')
            announce(__file__, 'Copying docs...')
            shutil.copytree(cfg_doc_src_dir, intermediate_dir, ignore=ignore_func, dirs_exist_ok=True)
            announce(__file__, 'Copying examples...')
            shutil.copytree(examples_dir, os.path.join(intermediate_dir, cfg_examples_dir), dirs_exist_ok=True)
        else:
            # --------- Method 1:
            options = {
                'create': True,    # Create directories if they don't exist.
                'exclude': exclude_list
            }
            # action == 'sync' means copy files even when they do not already exist in tgt dir.
            # action == 'update' means DO NOT copy files when they do not already exist in tgt dir.
            announce(__file__, 'Copying docs...')
            dirsync.sync(cfg_doc_src_dir, intermediate_dir, 'sync', **options)
            announce(__file__, 'Copying examples...')
            dirsync.sync(examples_dir, os.path.join(intermediate_dir, cfg_examples_dir), 'sync', **options)

        # -----------------------------------------------------------------
        # Build <intermediate_dir>/lv_conf.h from lv_conf_template.h.
        # -----------------------------------------------------------------
        config_builder.run(lv_conf_file)

        # -----------------------------------------------------------------
        # Copy `lv_version.h` into intermediate directory.
        # -----------------------------------------------------------------
        shutil.copyfile(version_src_file, version_dst_file)

        # -----------------------------------------------------------------
        # Generate examples pages.  Include sub-pages pages that get included
        # in individual documents where applicable.
        # -----------------------------------------------------------------
        announce(__file__, "Generating examples...")
        example_list.exec(intermediate_dir)

        # -----------------------------------------------------------------
        # Add translation links.
        # This is being skipped in favor of a manually-placed
        # translation link at the top of `./docs/index.rst`.
        # -----------------------------------------------------------------
        # Original code:
        # if True:
        #     announce(__file__, "Skipping adding translation links.")
        # else:
        #     announce(__file__, "Adding translation links...")
        #     add_translation.exec(intermediate_dir)

        if skip_api:
            announce(__file__, "Skipping API generation as requested.")
        else:
            # -------------------------------------------------------------
            # Generate API pages and links thereto.
            # -------------------------------------------------------------
            announce(__file__, "API page and link processing...")
            api_doc_builder.EMIT_WARNINGS = False

            # api_doc_builder.run() => doxy_xml_parser.DoxygenXml() now:
            # - preps and runs Doxygen generating XML,
            # - loads generated XML.
            # Then api_doc_builder.run():
            # - creates .RST files for API pages, and
            # - adds API hyperlinks to .RST files in the directories in passed array.
            api_doc_builder.build_api_docs(lvgl_src_dir,
                                           intermediate_dir,
                                           doxyfile_src_file,
                                           'details',
                                           'intro'
                                           )

        t2 = datetime.now()
        announce(__file__, 'Example/API run time:  ' + str(t2 - t1))

    # ---------------------------------------------------------------------
    # Build PDF
    # ---------------------------------------------------------------------
    if not build_latex:
        announce(__file__, "Skipping Latex build.")
    else:
        t1 = datetime.now()
        announce(__file__, "Building Latex output...", box=True)

        # If PDF link is present in top index.rst, remove it so PDF
        # does not have a link to itself.
        with open(top_index_file, 'rb') as f:
            index_data = f.read().decode('utf-8')

        if pdf_link_ref_str in index_data:
            index_data = index_data.replace(pdf_link_ref_str, '')

            with open(top_index_file, 'wb') as f:
                f.write(index_data.encode('utf-8'))

        src = intermediate_dir
        dst = output_dir
        cpu = os.cpu_count()

        # The -D option correctly replaces (overrides) configuration attribute
        # values in the `conf.py` module.  Since `conf.py` now correctly
        # computes its own `version` value, we don't have to override it here
        # with a -D options.  If it should need to be used in the future,
        # the value after the '=' MUST NOT have quotation marks around it
        # or it won't work.  Correct usage:  f'-D version={ver}' .
        cmd_line = f'sphinx-build -M latex "{src}" "{dst}" -j {cpu} --fail-on-warning --keep-going'
        cmd(cmd_line)

        # Generate PDF.
        announce(__file__, "Building PDF...", box=True)
        cmd_line = 'latexmk -pdf "LVGL.tex"'
        cmd(cmd_line, latex_output_dir, False)

        # Move resulting PDF to its output directory.
        if not os.path.exists(pdf_output_dir):
            os.makedirs(pdf_output_dir)

        shutil.move(pdf_src_file, pdf_dst_file)
        t2 = datetime.now()
        announce(__file__, 'PDF           :  ' + pdf_dst_file)
        announce(__file__, 'Latex gen time:  ' + str(t2 - t1))

    # ---------------------------------------------------------------------
    # Build HTML
    # ---------------------------------------------------------------------
    if not build_html:
        announce(__file__, "Skipping HTML build.")
    else:
        t1 = datetime.now()
        announce(__file__, "Building HTML output...", box=True)

        # If PDF is present in build directory, copy it to
        # intermediate directory for use by HTML build.
        # (Sphinx copies it to its HTML output, so it ends
        # up on the webserver where it can be downloaded).
        if os.path.isfile(pdf_dst_file):
            # Create _static/download/ directory if needed.
            if not os.path.exists(pdf_intermediate_dst_dir):
                os.makedirs(pdf_intermediate_dst_dir)

            shutil.copyfile(pdf_dst_file, pdf_intermediate_dst_file)

        # If PDF is present, ensure there is a link to it in the top
        # index.rst so HTML build will have it.
        # Support both Windows and Linux platforms with `os.linesep`.
        if os.path.isfile(pdf_intermediate_dst_file):
            with open(top_index_file, 'rb') as f:
                index_data = f.read().decode('utf-8')

            if pdf_link_ref_str not in index_data:
                index_data = cfg_pdf_link_ref_block_str + index_data

                with open(top_index_file, 'wb') as f:
                    f.write(index_data.encode('utf-8'))

        # Note:  While it can be done (e.g. if one needs to set a stop point
        # in Sphinx code for development purposes), it is NOT a good idea to
        # run Sphinx from script as
        #   from sphinx.cmd.build import main as sphinx_build
        #   sphinx_args = [...]
        #   sphinx_build(sphinx_args)
        # because it takes ~10X longer to run than `sphinx_build` executable,
        # literally > 3 hours.

        ver = lvgl_version(version_src_file)
        src = intermediate_dir
        dst = output_dir
        cpu = os.cpu_count()

        debugging_breathe = 0
        if debugging_breathe:
            from sphinx.cmd.build import main as sphinx_build
            # Don't allow parallel processing while debugging (the '-j' arg is removed).
            sphinx_args = ['-M', 'html', f'{src}', f'{dst}']

            if len(env_opt) > 0:
                sphinx_args.append(f'{env_opt}')

            sphinx_build(sphinx_args)
        else:
            # The -D option correctly replaces (overrides) configuration attribute
            # values in the `conf.py` module.  Since `conf.py` now correctly
            # computes its own `version` value, we don't have to override it here
            # with a -D options.  If it should need to be used in the future,
            # the value after the '=' MUST NOT have quotation marks around it
            # or it won't work.  Correct usage:  f'-D version={ver}' .
            cmd_line = f'sphinx-build -M html "{src}" "{dst}" -j {cpu} {env_opt} --fail-on-warning --keep-going'
            cmd(cmd_line)

        t2 = datetime.now()
        announce(__file__, 'HTML gen time :  ' + str(t2 - t1))

    # ---------------------------------------------------------------------
    # Indicate results.
    # ---------------------------------------------------------------------
    t_end = datetime.now()
    announce(__file__, 'Total run time:  ' + str(t_end - t0))
    announce(__file__, 'Done.')


if __name__ == '__main__':
    """Make module importable as well as run-able."""
    run(sys.argv[1:])
