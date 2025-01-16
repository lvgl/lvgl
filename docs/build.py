#!/usr/bin/env python3
""" Generate LVGL documentation using Doxygen and Sphinx.

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
  (config_builder.py).

- Supporting multiple execution platforms (which then required tokenizing
  Doxygen's INPUT path in `Doxyfile` and re-writing portions that used
  `sed` to generate input or modify files).

- Adding translation and API links (requiring generating docs in a
  temporary directory so that the links could be programmatically
  added to each document before Sphinx was run).

- Generating EXAMPLES page + sub-examples where applicable to individual
  documents, e.g. to widget-, style-, layout-pages, etc.

- Building PDF via latex (when working).


Command-Line Arguments
----------------------
Command-line arguments have been broken down to give the user the
ability to control each individual major variation in behavior of
this script.  These were added to speed up long doc-development
tasks by shortening the turn-around time between doc modification
and seeing the final .html results in a local development environment.
Finally, this script can now be used in a way such that Sphinx will
only modify changed documents, and reduce an average ~22-minute
run time to a run time that is workable for rapidly repeating doc
generation to see Sphinx formatting results quickly.


Normal Usage
------------
This is the way this script is used for normal (full) docs generation.

    $ python  build.py  skip_latex


Docs-Dev Initial Docs Generation Usage
--------------------------------------
1.  Set `LVGL_FIXED_TEMP_DIR` environment variable to path to
    the temporary directory you will use over and over during
    document editing, without trailing directory separator.
    Initially directory should not exist.

2.  $ python  build.py  skip_latex  preserve  fixed_tmp_dir

This takes typically ~22 minutes.


Docs-Dev Update-Only Generation Usage
-------------------------------------
After the above has been run through once, you can thereafter
run the following until docs-development task is complete.

    $ python  build.py  skip_latex  docs_dev  update

Generation time depends on the number of `.rst` files that
have been updated:

+--------------+------------+---------------------------------+
| Docs Changed | Time       | Typical Time to Browser Refresh |
+==============+============+=================================+
|  0           |  6 seconds |             n/a                 |
+--------------+------------+---------------------------------+
|  1           | 19 seconds |          12 seconds             |
+--------------+------------+---------------------------------+
|  5           | 28 seconds |          21 seconds             |
+--------------+------------+---------------------------------+
| 20           | 59 seconds |          52 seconds             |
+--------------+------------+---------------------------------+


Sphinx Doc-Regeneration Criteria
--------------------------------
Sphinx uses the following to determine what documents get updated:

- source-doc modification date
  - Change the modification date and `sphinx-build` will re-build it.

- full (absolute) path to the source document, including its file name
  - Change the path or filename and `sphinx-build` will re-build it.

- whether the -E option is on the `sphinx-build` command line
  - -E forces `sphinx-build` to do a full re-build.


Argument Descriptions
---------------------
- skip_latex
    The meaning of this argument has not changed:  it simply skips
    attempting to generate Latex and subsequent PDF generation.
- skip_api
    Skips generating API pages (this saves about 70% of build time).
    This is intended to be used only during doc development to speed up
    turn-around time between doc modifications and seeing final results.
- no_fresh_env
    Excludes -E command-line argument to `sphinx-build`, which, when present,
    forces it to generate a whole new environment (memory of what was built
    previously, forcing a full rebuild).  "no_fresh_env" enables a rebuild
    of only docs that got updated -- Sphinx's default behavior.
- preserve (previously "develop")
    Leaves temporary directory intact for docs development purposes.
- fixed_tmp_dir
    If (fixed_tmp_dir and 'LVGL_FIXED_TEMP_DIR' in `os.environ`),
    then this script uses the value of that that environment variable
    to populate `temp_directory` instead of the normal (randomly-named)
    temporary directory.  This is important when getting `sphinx-build`
    to ONLY rebuild updated documents, since changing the directory
    from which they are generated (normally the randomly-named temp
    dir) will force Sphinx to do a full-rebuild because it remembers
    the doc paths from which the build was last performed.
- skip_trans
    Skips adding translation links.  This allows direct copying of
    `.rst` files to `temp_directory` when they are updated to save time
    during re-build.  Final build must not include this option so that
    the translation links are added at the top of each intended page.
- no_copy
    Skips copying ./docs/ directory tree to `temp_directory`.
    This is only honored if:
    - fixed_tmp_dir == True, and
    - the doc files were previously copied to the temporary directory
      and thus are already present there.
- docs_dev
    This is a command-line shortcut to combining these command-line args:
    - no_fresh_env
    - preserve
    - fixed_tmp_dir
    - no_copy
- update
    When no_copy is active, check modification dates on `.rst` files
    and re-copy the updated `./docs/` files to the temporary directory
    that have later modification dates, thus updating what Sphinx uses
    as input.
    Warning:  this wipes out translation links and API-page links that
    were added in the first pass, so should only be used for doc
    development -- not for final doc generation.
"""

# ****************************************************************************
# IMPORTANT: If you are getting a PDF-lexer error for an example, check
#            for extra lines at the end of the file. Only a single empty line
#            is allowed!!! Ask me how long it took me to figure this out.
# ****************************************************************************


def run():
    # Python Library Imports
    import sys
    import os
    import re
    import subprocess
    import shutil
    import tempfile
    import dirsync
    from datetime import datetime

    # LVGL Custom Imports
    import example_list as ex
    import doc_builder
    import config_builder
    import add_translation

    # ---------------------------------------------------------------------
    # Start.
    # ---------------------------------------------------------------------
    t1 = datetime.now()
    print('Current time:  ' + str(t1))

    # ---------------------------------------------------------------------
    # Process args.
    #
    # With argument `docs_dev`, Sphinx will generate docs from a fixed
    # temporary directory that can be then used later using the same
    # command line to get Sphinx to ONLY rebuild changed documents.
    # This saves a huge amount of time during long document projects.
    # ---------------------------------------------------------------------
    # Set defaults.
    clean = False
    skip_latex = False
    skip_api = False
    fresh_sphinx_env = True
    preserve = False
    fixed_tmp_dir = False
    skip_trans = False
    no_copy = False
    docs_dev = False
    update = False
    args = sys.argv[1:]

    for arg in args:
        # We use chained `if-elif-else` instead of `match` for those on Linux
        # systems that will not have the required version 3.10 of Python yet.
        if arg == "clean":
            clean = True
        elif arg == "skip_latex":
            skip_latex = True
        elif arg == 'skip_api':
            skip_api = True
        elif arg == 'no_fresh_env':
            fresh_sphinx_env = False
        elif arg == 'preserve':
            preserve = True
        elif arg == 'fixed_tmp_dir':
            fixed_tmp_dir = True
        elif arg == 'skip_trans':
            skip_trans = True
        elif arg == 'no_copy':
            no_copy = True
        elif arg == 'docs_dev':
            docs_dev = True
        elif arg == 'update':
            update = True
        else:
            print(f'Argument [{arg}] not recognized.')
            exit(1)

    # Arg ramifications:
    # docs_dev implies no_fresh_env, preserve, fixed_tmp_dir, and no_copy.
    if docs_dev:
        fresh_sphinx_env = False
        preserve = True
        fixed_tmp_dir = True
        no_copy = True

    # ---------------------------------------------------------------------
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
    # ---------------------------------------------------------------------
    if fixed_tmp_dir and 'LVGL_FIXED_TEMP_DIR' in os.environ:
        temp_directory = os.environ['LVGL_FIXED_TEMP_DIR']
    else:
        temp_directory = tempfile.mkdtemp(suffix='.lvgl_docs')

    print(f'Using temp directory:  [{temp_directory}]')

    # ---------------------------------------------------------------------
    # Set up paths.
    # ---------------------------------------------------------------------
    base_path = os.path.abspath(os.path.dirname(__file__))
    project_path = os.path.abspath(os.path.join(base_path, '..'))
    examples_path = os.path.join(project_path, 'examples')
    lvgl_src_path = os.path.join(project_path, 'src')
    latex_output_path = os.path.join(temp_directory, 'out_latex')
    pdf_src_file = os.path.join(latex_output_path, 'LVGL.pdf')
    pdf_dst_file = os.path.join(temp_directory, 'LVGL.pdf')
    html_src_path = temp_directory
    html_dst_path = os.path.join(project_path, 'out_html')

    # ---------------------------------------------------------------------
    # Change to script directory for consistency.
    # ---------------------------------------------------------------------
    os.chdir(base_path)

    # ---------------------------------------------------------------------
    # Provide a way to run an external command and abort build on error.
    # ---------------------------------------------------------------------
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
    # Start doc-build process.
    # ---------------------------------------------------------------------
    print("")
    print("****************")
    print("Building")
    print("****************")

    # Remove all previous output files if 'clean' on command line.
    if clean:
        print('Removing previous output files...')
        # The below commented-out code below is being preserved
        # for docs-generation development purposes.

        # api_path = os.path.join(temp_directory, 'API')
        # xml_path = os.path.join(temp_directory, 'xml')
        # doxy_path = os.path.join(temp_directory, 'doxygen_html')

        # if os.path.exists(api_path):
        #     shutil.rmtree(api_path)

        # lang = 'en'
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

    # ---------------------------------------------------------------------
    # Build local lv_conf.h from lv_conf_template.h for this build only.
    # ---------------------------------------------------------------------
    config_builder.run()

    # ---------------------------------------------------------------------
    # Provide answer to question:  Can we have reasonable confidence that
    # the contents of `temp_directory` already exists?
    # ---------------------------------------------------------------------
    def temp_dir_contents_exists():
        result = False
        c1 = os.path.exists(temp_directory)

        if c1:
            temp_path = os.path.join(temp_directory, 'CHANGELOG.rst')
            c2 = os.path.exists(temp_path)
            temp_path = os.path.join(temp_directory, 'CODING_STYLE.rst')
            c3 = os.path.exists(temp_path)
            temp_path = os.path.join(temp_directory, 'CONTRIBUTING.rst')
            c4 = os.path.exists(temp_path)
            temp_path = os.path.join(temp_directory, '_ext')
            c5 = os.path.exists(temp_path)
            temp_path = os.path.join(temp_directory, '_static')
            c6 = os.path.exists(temp_path)
            temp_path = os.path.join(temp_directory, 'details')
            c7 = os.path.exists(temp_path)
            temp_path = os.path.join(temp_directory, 'intro')
            c8 = os.path.exists(temp_path)
            temp_path = os.path.join(temp_directory, 'examples')
            c9 = os.path.exists(temp_path)
            result = c2 and c3 and c4 and c5 and c6 and c7 and c8 and c9

        return result

    # ---------------------------------------------------------------------
    # Copy files to 'temp_directory' where they will be edited (translation
    # link and API links) before being used to generate new docs.
    # ---------------------------------------------------------------------
    doc_files_copied = False
    if no_copy and fixed_tmp_dir and temp_dir_contents_exists():
        if update:
            exclude_list = ['lv_conf.h']
            options = {
                'verbose': True,
                'create': True,
                'exclude': exclude_list
            }
            dirsync.sync('.', temp_directory, 'update', **options)
        else:
            print("Skipping copying ./docs/ directory as requested.")
    else:
        shutil.copytree('.', temp_directory, dirs_exist_ok=True)
        shutil.copytree(examples_path, os.path.join(temp_directory, 'examples'), dirs_exist_ok=True)
        doc_files_copied = True

    # ---------------------------------------------------------------------
    # Replace tokens in Doxyfile in 'temp_directory' with data from this run.
    # ---------------------------------------------------------------------
    if doc_files_copied:
        with open(os.path.join(temp_directory, 'Doxyfile'), 'rb') as f:
            data = f.read().decode('utf-8')

        data = data.replace('#*#*LV_CONF_PATH*#*#', os.path.join(base_path, 'lv_conf.h'))
        data = data.replace('*#*#SRC#*#*', '"{0}"'.format(lvgl_src_path))

        with open(os.path.join(temp_directory, 'Doxyfile'), 'wb') as f:
            f.write(data.encode('utf-8'))

        # -----------------------------------------------------------------
        # Generate examples pages.  Include sub-pages pages that get included
        # in individual documents where applicable.
        # -----------------------------------------------------------------
        print("Generating examples...")
        ex.exec(temp_directory)

        # -----------------------------------------------------------------
        # Add translation links.
        # -----------------------------------------------------------------
        if skip_trans:
            print("Skipping translation links as requested.")
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

    # ---------------------------------------------------------------------
    # BUILD PDF
    # ---------------------------------------------------------------------
    if skip_latex:
        print("Skipping latex build as requested.")
    else:
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
        dst = latex_output_path
        cpu = os.cpu_count()
        cmd_line = f'sphinx-build -b latex "{src}" "{dst}" -j {cpu}'
        cmd(cmd_line)

        # Generate PDF.
        cmd_line = 'latexmk -pdf "LVGL.tex"'
        cmd(cmd_line, latex_output_path)

        # Copy the result PDF to the main directory to make
        # it available for the HTML build.
        shutil.copyfile(pdf_src_file, pdf_dst_file)

        # Add PDF link back in so HTML build will have it.
        index_data = pdf_link_ref_str + index_data

        with open(index_path, 'wb') as f:
            f.write(index_data.encode('utf-8'))

    # ---------------------------------------------------------------------
    # BUILD HTML
    # ---------------------------------------------------------------------
    # This version of get_version() works correctly under both Linux and Windows.
    # Updated to be resilient to changes in `lv_version.h` compliant with C macro syntax.
    def get_version():
        path = os.path.join(project_path, 'lv_version.h')
        major = ''
        minor = ''

        with open(path, 'r') as file:
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

    ver = get_version()
    src = html_src_path
    dst = html_dst_path
    cpu = os.cpu_count()
    cmd_line = f'sphinx-build -b html "{src}" "{dst}" -D version="{ver}" {env_opt} -j {cpu}'
    t2 = datetime.now()
    print('Current time:  ' + str(t2))
    cmd(cmd_line)
    t3 = datetime.now()
    print('Current time:     ' + str(t3))
    print('Sphinx run time:  ' + str(t3 - t2))

    # ---------------------------------------------------------------------
    # Cleanup.
    # ---------------------------------------------------------------------
    if preserve:
        print('Temp directory:  ', temp_directory)
    else:
        print('Removing temporary files...', temp_directory)
        if os.path.exists(temp_directory):
            shutil.rmtree(temp_directory)

    # ---------------------------------------------------------------------
    # Remove temporary `lv_conf.h` created for this build.
    # ---------------------------------------------------------------------
    config_builder.cleanup()

    # ---------------------------------------------------------------------
    # Indicate results.
    # ---------------------------------------------------------------------
    t4 = datetime.now()
    print('Total run time:   ' + str(t4 - t1))
    print('Output path:     ', html_dst_path)
    print()
    print('Note:  warnings about `/details/index.rst` and `/intro/index.rst`')
    print('       "not being in any toctree" are expected and intentional.')
    print()
    print('Finished.')


# -------------------------------------------------------------------------
# Make module importable as well as run-able.
# -------------------------------------------------------------------------
if __name__ == '__main__':
    run()
