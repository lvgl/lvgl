# #########################################################################
# Configuration file for the Sphinx documentation builder.
# Created by sphinx-quickstart on Wed Jun 12 16:38:40 2019.
#
# This file is execfile()'d with the current directory set to its
# containing directory.
#
# All configuration values have a default; some values are included
# below that are commented out and some of these serve to show the
# default.  Note that not all possible configuration values are present.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html
# The major sections below each reflect a major section of that web page,
# and they are ordered in the same sequence so it is clear what config
# items go with what.
#
# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath() to make it absolute, as shown here.
# #########################################################################
import os
import sys
from sphinx.builders.html import StandaloneHTMLBuilder

base_path = os.path.abspath(os.path.dirname(__file__))
# Add path to import link_roles.py and lv_example.py
sys.path.insert(0, os.path.abspath('./_ext'))
# Add path to import lvgl_version.py.  Remember this is
# running under `sphinx-build` environment, not `build.py`.
sys.path.insert(0, base_path)
from lvgl_version import lvgl_version #NoQA

cfg_lv_version_file = 'lv_version.h'



# *************************************************************************
# Project Information
# *************************************************************************

project = 'LVGL'
copyright = '2021-%Y, LVGL Kft'
author = 'LVGL Community'

if __name__ == '__main__':
    version_src_path = os.path.join(base_path, '../../lv_version.h')
else:
    version_src_path = os.path.join(base_path, cfg_lv_version_file)

if os.path.isfile(version_src_path):
    # We have lv_version.h.  Use it.
    version = lvgl_version(version_src_path)
else:
    # We have to guess.
    version = '9.3'

release = version
# Notes about `version` here:
# ---------------------------
# The version info for the project you're documenting, acts as replacement for
# |version| and |release|, also used in various other places throughout the
# built documents.
#
# A short X.Y version is extracted from `lv_version.h` using a cross-platform compatible
# Python function in lvgl_version.py, and passed in on `sphinx-build` command line.
#
# 22-Apr-2025 while the `-D version=...` on the command line works (as long as quotes
# are not placed around the version), having it added after `sphinx-build` has
# executed this script is not soon enough because we need the version in some
# strings below.  So we need to get it here from `lv_version.h` in order to do that.



# *************************************************************************
# General Configuration
# *************************************************************************

# If your documentation needs a minimal Sphinx version, state it here.
#
# needs_sphinx = '1.0'

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or custom extensions.
#
# As of 6-Jan-2025, `link_roles` is being commented out because it is being
# replaced by a manually-installed translation link in ./docs/index.rst.
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.extlinks',
    'sphinx.ext.intersphinx',
    'sphinx.ext.todo',
    'sphinx.ext.viewcode',      # Eye icon at top of page to view page source code on GitHub.
    'sphinx_copybutton',        # Copy-to-clipboard button in code blocks & code examples.
    'breathe',
    'sphinx_sitemap',
    'lv_example',
    'sphinx_design',
    # 'link_roles',
    'sphinxcontrib.mermaid',
]

needs_extensions = {
    'sphinxcontrib.mermaid': '0.9.2'
}

# If 'SPHINX_REREDIRECTS_STANDDOWN' environment variable exists and
# is set to a value not equal to '0', then do not add 'sphinx_reredirects'
# to extensions.  This gives someone testing/editing/debugging documentation
# build the possibility of skipping adding redirects in the local environment
# if desired.
add_redirects = True
if 'SPHINX_REREDIRECTS_STANDDOWN' in os.environ:
    if os.environ.get('SPHINX_REREDIRECTS_STANDDOWN') != '0':
        print("sphinx_reredirects standing down as requested.")
        add_redirects = False

if add_redirects:
    extensions.append('sphinx_reredirects')

del add_redirects

# -------------------------------------------------------------------------
# Options for Highlighting
# -------------------------------------------------------------------------
# The default language to highlight source code in. The default is 'python'.
# The value should be a valid Pygments lexer name, see Showing code examples
# for more details.
highlight_language = 'c'

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'github-light'
pygments_dark_style = 'github-dark'

# -------------------------------------------------------------------------
# Options for Internationalisation
# -------------------------------------------------------------------------
# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#
# This is also used if you do content translation via gettext catalogs.
# Usually you set "language" from the command line for these cases.
language = 'en'

# -------------------------------------------------------------------------
# Options for Markup
# -------------------------------------------------------------------------
default_role = 'literal'
# keep_warnings = False   # True causes Sphinx warnings to be added to documents.
primary_domain = 'c'      # Default:  'py'

# -------------------------------------------------------------------------
# Options for Source Files
# -------------------------------------------------------------------------
# List of glob-style patterns, relative to source directory, that
# match files and directories to ignore when looking for source files.
# These patterns also effect html_static_path and html_extra_path.
exclude_patterns = ['build', 'doxygen', 'intermediate', 'doxygen_html', 'Thumbs.db', '.DS_Store',
                    'README.md', 'README_*', 'lv_examples', 'env', '_ext', 'examples']

# The master toctree document.  (Root of TOC tree.)
master_doc = 'index'

# The suffix(es) of source filenames.
# You can specify multiple suffixes as a dictionary per
# https://www.sphinx-doc.org/en/master/usage/configuration.html#confval-source_suffix
source_suffix = {'.rst': 'restructuredtext'}

# -------------------------------------------------------------------------
# Options for Smart Quotes
# -------------------------------------------------------------------------
# Enabling smart quotes action to convert `--` to en dashes and `---` to em
# dashes.  Converting quotation marks and ellipses is NOT done because the
# default `smartquotes_action` 'qDe' is changed to just 'D' below, which
# accomplishes the dash conversions as desired.
#
# For list of all possible smartquotes_action values, see:
# https://www.sphinx-doc.org/en/master/usage/configuration.html#confval-smartquotes_action
smartquotes = True
smartquotes_action = 'D'
smartquotes_excludes = {'builders': ['man', 'text', 'json', 'xml', 'latex']}

# -------------------------------------------------------------------------
# Options for Templating
# -------------------------------------------------------------------------
# A list of paths that contain extra templates (or templates that overwrite
# builtin/theme-specific templates).  Relative paths are taken as relative
# to the configuration directory.
templates_path = ['_templates']

# -------------------------------------------------------------------------
# Options for Warning Control
# -------------------------------------------------------------------------
# suppress_warnings = ['app.add_directive', etc.]



# *************************************************************************
# Builder Options
# *************************************************************************

# -------------------------------------------------------------------------
# Options for HTML Builder
# -------------------------------------------------------------------------
# The theme for HTML output.  See https://www.sphinx-doc.org/en/master/usage/theming.html
html_theme = 'furo'

# Theme options are theme-specific and customize the look and feel of a
# theme further.  For a list of options available for each theme, see the
# theme documentation.
#
# Various other builders are derived from the HTML output, and also make use
# of these options.

# Note:  'display_version' option is now obsolete in the current (08-Oct-2024)
# version of sphinx-rtd-theme (upgraded for Sphinx v8.x).  The removed line
# is preserved by commenting it out in case it is ever needed again.
html_theme_options = {
    "sidebar_hide_name": True,      # True when the logo carries project name
    "light_logo": "images/logo-light.svg",
    "dark_logo": "images/logo-dark.svg",
    "top_of_page_buttons": ["view"],
    # The below 3 direct the "top_of_page_buttons" to github for view and edit buttons.
    "source_repository": "https://github.com/lvgl/lvgl/",
    "source_branch": "master",
    "source_directory": "docs/src/",
    # "announcement": "<em>Semi-permanent announcement</em> from <code>conf.py</code>.",
}

html_sidebars = {
    "**": [
        "sidebar/brand.html",
        "sidebar/version-selector.html",
        "sidebar/search.html",
        "sidebar/scroll-start.html",
        "sidebar/navigation.html",
        "sidebar/ethical-ads.html",
        "sidebar/scroll-end.html",
        "sidebar/variant-selector.html"
    ]
}

# For site map generation
if "LVGL_URLPATH" not in os.environ:
    os.environ['LVGL_URLPATH'] = 'master'

_branch = os.getenv('LVGL_URLPATH')
html_baseurl = f"https://docs.lvgl.io/{_branch}/"

#lvgl_github_url = f"https://github.com/lvgl/lvgl/blob/{os.environ['LVGL_GITCOMMIT']}/docs"
#extlinks = {'github_link_base': (github_url + '%s', github_url)}

if "LVGL_GITCOMMIT" not in os.environ:
    os.environ['LVGL_GITCOMMIT'] = 'master'

_git_commit_ref = os.getenv('LVGL_GITCOMMIT')

# These keys are used "bare" as template variables in:
# - sphinx_rtd_theme theme template:  breadcrumbs.html
# - furo             theme template:  edit-this-page.html
# - furo             theme template:  view-this-page.html
html_context = {
    'display_github': True,
    'github_user': 'lvgl',
    'github_repo': 'lvgl',
    'github_version': _git_commit_ref,
    'conf_py_path': '/docs/src/'
}

html_logo = ''
html_favicon = '_static/images/favicon.png'

html_css_files = [
    'css/fontawesome.min.css',
    'css/solid.min.css',
    'css/brands.min.css',
    'css/custom.css'
]

html_js_files = [
    'js/custom.js',
    'js/include_html.js'
]

html_static_path = ['_static']
html_last_updated_fmt = ''          # Empty string uses default format:  '%b %d, %Y'
html_last_updated_use_utc = False   # False = use generating system's local date, not GMT.
html_permalinks = True              # Default = True, add link anchor for each heading and description environment.

# 10-Mar-2025 16:21 -- commented out for Furo theme.
# html_sidebars = {
#     '**': [
#         'relations.html',  # needs 'show_related': True theme option to display
#         'searchbox.html',
#     ]
# }

# html_domain_indices
# html_use_index = True            # Default = True
# html_split_index
# html_copy_source = True          # Default = True; if True, reStructuredText sources are included in HTML build as _sources/docname.
# html_show_sourcelink = True      # Default = True
# html_sourcelink_suffix = '.txt'  # Default = '.txt'
# html_use_opensearch = ''
# html_file_suffix = '.html'       # Default = '.html'
# html_link_suffix = html_file_suffix
html_show_copyright = True         # Default = True; shows copyright notice in footer.
# html_show_search_summary = True  # Default = True
html_show_sphinx = False          # Default = True; adds "Created using Sphinx" to footer.
# html_output_encoding = 'utf-8'   # Default = 'utf-8'
# html_compact_lists = True        # Default = True
# html_secnumber_suffix = '. '     # Default = '. '; set to ' ' to suppress final dot on section numbers.
# html_search_language = language  # Default = language; language to be used for generating the HTML full-text search index.
# html_search_options

# Link images that have been resized with a scale option (scale, width, or height)
# to their original full-resolution image.  This will not overwrite any link given
# by the target option on the the image directive, if present.
#
# Tip
#
# To disable this feature on a per-image basis, add the no-scaled-link class to the image directive:
#
# .. image:: sphinx.png
#    :scale: 50%
#    :class: no-scaled-link
# html_scaled_image_link

# html_math_renderer = 'mathjax'   # Default = 'mathjax'

# Change image search sequence:  image files are searched in the order in which they appear here.
# See https://www.sphinx-doc.org/en/master/usage/builders/index.html#sphinx.builders.html.StandaloneHTMLBuilder.supported_image_types
StandaloneHTMLBuilder.supported_image_types = [
    'image/svg+xml',
    'image/gif',  #prefer gif over png
    'image/png',
    'image/jpeg'
]

# -------------------------------------------------------------------------
# Options for HTMLHelp Builder
# -------------------------------------------------------------------------
# Output file base name for HTML help builder.
htmlhelp_basename = 'LVGLdoc'

# -------------------------------------------------------------------------
# Options for LaTeX Builder
# -------------------------------------------------------------------------
latex_engine = 'xelatex'
latex_use_xindy = False
latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    #
    # 'papersize': 'letterpaper',

    # The font size ('10pt', '11pt' or '12pt').
    #
    # 'pointsize': '10pt',

    # Additional stuff for the LaTeX preamble.
    #
    # 'preamble': '',

    # Latex figure (float) alignment
    #
    # 'figure_align': 'htbp',

    'inputenc': '',
    'utf8extra': '',
    'classoptions': ',openany,oneside',
    'babel': '\\usepackage{babel}',
    'passoptionstopackages': r'''
\PassOptionsToPackage{bookmarksdepth=5}{hyperref}% depth of pdf bookmarks
''',
    'preamble': r'''
\usepackage{fontspec}
\setmonofont{DejaVu Sans Mono}
\usepackage{silence}
\WarningsOff*
''',
}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (master_doc, 'LVGL.tex', 'LVGL v' + version,
     author, 'manual'),
]

# -------------------------------------------------------------------------
# Options for Manual Page Builder
# -------------------------------------------------------------------------
# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    (master_doc, 'lvgl', 'LVGL v' + version,
     [author], 3)
]

# -------------------------------------------------------------------------
# Options for Texinfo Builder
# -------------------------------------------------------------------------
# Grouping the document tree into Texinfo files. List of tuples
# (source start file, target name, title, author,
#  dir menu entry, description, category)
texinfo_documents = [
    (master_doc, 'LVGL', 'LVGL v' + version,
     author, 'Contributors of LVGL', 'One line description of project.',
     'Miscellaneous'),
]



# *************************************************************************
# Domain Options
# *************************************************************************

# -------------------------------------------------------------------------
# Options for the C Domain
# -------------------------------------------------------------------------

# -------------------------------------------------------------------------
# Options for the CPP Domain
# -------------------------------------------------------------------------



# *************************************************************************
# Configuration for Sphinx Extensions
# *************************************************************************

# -------------------------------------------------------------------------
# Options for sphinx_rtd_dark_mode extension
# -------------------------------------------------------------------------
default_dark_mode = False

# -------------------------------------------------------------------------
# Options for lv_example extension
# -------------------------------------------------------------------------
repo_commit_hash = _git_commit_ref

# -------------------------------------------------------------------------
# Options for sphinx.ext.todo extension
# -------------------------------------------------------------------------
# If true, `todo` and `todoList` directives produce output, else they produce nothing.
# See https://www.sphinx-doc.org/en/master/usage/extensions/todo.html
todo_include_todos = True

# -------------------------------------------------------------------------
# Options for sphinx_sitemap extension
# -------------------------------------------------------------------------
# See https://sphinx-sitemap.readthedocs.io/en/latest/index.html
sitemap_url_scheme = "{link}"

# -------------------------------------------------------------------------
# Options for breathe extension
# -------------------------------------------------------------------------
# See https://breathe.readthedocs.io/en/latest/
breathe_projects = {
    "lvgl": "xml/",
}

breathe_default_project = "lvgl"
# breathe_debug_trace_directives = True

# -------------------------------------------------------------------------
# Options for sphinx_reredirects
# -------------------------------------------------------------------------
# The below generates .HTML page redirects for pages that have been moved.
# Browsers are redirected via `<meta http-equiv="refresh" content="0; url=new_url">`.
redirects = {
    "CONTRIBUTING":                               "contributing/index.html"                                         ,
    "get-started/index":                          "../intro/getting_started.html"                                   ,
    "get-started/os/nuttx":                       "../../details/integration/os/nuttx.html"                         ,
    "get-started/platforms/arduino":              "../../details/integration/framework/arduino.html"                ,
    "get-started/platforms/espressif":            "../../details/integration/chip/espressif.html"                   ,
    "get-started/platforms/pc-simulator":         "../../details/integration/ide/pc-simulator.html"                 ,
    "get-started/quick-overview":                 "../intro/getting_started.html"                                   ,
    "integration/bindings/api_json":              "../../details/integration/bindings/api_json.html"                ,
    "integration/bindings/cpp":                   "../../details/integration/bindings/cpp.html"                     ,
    "integration/bindings/index":                 "../../details/integration/bindings/index.html"                   ,
    "integration/bindings/javascript":            "../../details/integration/bindings/javascript.html"              ,
    "integration/bindings/micropython":           "../../details/integration/bindings/micropython.html"             ,
    "integration/bindings/pikascript":            "../../details/integration/bindings/pikascript.html"              ,
    "integration/building/cmake":                 "../../details/integration/building/cmake.html"                   ,
    "integration/building/index":                 "../../details/integration/building/index.html"                   ,
    "integration/building/make":                  "../../details/integration/building/make.html"                    ,
    "integration/chip/arm":                       "../../details/integration/chip/arm.html"                         ,
    "integration/chip/espressif":                 "../../details/integration/chip/espressif.html"                   ,
    "integration/chip/index":                     "../../details/integration/chip/index.html"                       ,
    "integration/chip/nxp":                       "../../details/integration/chip/nxp.html"                         ,
    "integration/chip/renesas":                   "../../details/integration/chip/renesas/index.html"               ,
    "integration/chip/stm32":                     "../../details/integration/chip/stm32.html"                       ,
    "integration/driver/X11":                     "../../details/integration/driver/X11.html"                       ,
    "integration/driver/display/fbdev":           "../../../details/integration/driver/display/fbdev.html"          ,
    "integration/driver/display/gen_mipi":        "../../../details/integration/driver/display/gen_mipi.html"       ,
    "integration/driver/display/ili9341":         "../../../details/integration/driver/display/ili9341.html"        ,
    "integration/driver/display/index":           "../../../details/integration/driver/display/index.html"          ,
    "integration/driver/display/lcd_stm32_guide": "../../../details/integration/driver/display/lcd_stm32_guide.html",
    "integration/driver/display/renesas_glcdc":   "../../../details/integration/driver/display/renesas_glcdc.html"  ,
    "integration/driver/display/st7735":          "../../../details/integration/driver/display/st7735.html"         ,
    "integration/driver/display/st7789":          "../../../details/integration/driver/display/st7789.html"         ,
    "integration/driver/display/st7796":          "../../../details/integration/driver/display/st7796.html"         ,
    "integration/driver/display/st_ltdc":         "../../../details/integration/driver/display/st_ltdc.html"        ,
    "integration/driver/index":                   "../../details/integration/driver/index.html"                     ,
    "integration/driver/libinput":                "../../details/integration/driver/libinput.html"                  ,
    "integration/driver/opengles":                "../../details/integration/driver/opengles.html"                  ,
    "integration/driver/touchpad/evdev":          "../../../details/integration/driver/touchpad/evdev.html"         ,
    "integration/driver/touchpad/ft6x36":         "../../../details/integration/driver/touchpad/ft6x36.html"        ,
    "integration/driver/touchpad/index":          "../../../details/integration/driver/touchpad/index.html"         ,
    "integration/driver/wayland":                 "../../details/integration/driver/wayland.html"                   ,
    "integration/driver/windows":                 "../../details/integration/driver/windows.html"                   ,
    "integration/framework/arduino":              "../../details/integration/framework/arduino.html"                ,
    "integration/framework/index":                "../../details/integration/framework/index.html"                  ,
    "integration/framework/platformio":           "../../details/integration/framework/platformio.html"             ,
    "integration/framework/tasmota-berry":        "../../details/integration/framework/tasmota-berry.html"          ,
    "integration/ide/index":                      "../../details/integration/ide/index.html"                        ,
    "integration/ide/mdk":                        "../../details/integration/ide/mdk.html"                          ,
    "integration/ide/pc-simulator":               "../../details/integration/ide/pc-simulator.html"                 ,
    "integration/index":                          "../details/integration/index.html"                               ,
    "integration/os/freertos":                    "../../details/integration/os/freertos.html"                      ,
    "integration/os/index":                       "../../details/integration/os/index.html"                         ,
    "integration/os/mqx":                         "../../details/integration/os/mqx.html"                           ,
    "integration/os/nuttx":                       "../../details/integration/os/nuttx.html"                         ,
    "integration/os/px5":                         "../../details/integration/os/px5.html"                           ,
    "integration/os/qnx":                         "../../details/integration/os/qnx.html"                           ,
    "integration/os/rt-thread":                   "../../details/integration/os/rt-thread.html"                     ,
    "integration/os/yocto/core_components":       "../../../details/integration/os/yocto/core_components.html"      ,
    "integration/os/yocto/index":                 "../../../details/integration/os/yocto/index.html"                ,
    "integration/os/yocto/lvgl_recipe":           "../../../details/integration/os/yocto/lvgl_recipe.html"          ,
    "integration/os/yocto/terms_and_variables":   "../../../details/integration/os/yocto/terms_and_variables.html"  ,
    "integration/os/zephyr":                      "../../details/integration/os/zephyr.html"                        ,
    "layouts/flex":                               "../details/common-widget-features/layouts/flex.html"             ,
    "layouts/grid":                               "../details/common-widget-features/layouts/grid.html"             ,
    "layouts/index":                              "../details/common-widget-features/layouts/index.html"            ,
    "libs/arduino_esp_littlefs":                  "../details/libs/arduino_esp_littlefs.html"                       ,
    "libs/arduino_sd":                            "../details/libs/arduino_sd.html"                                 ,
    "libs/barcode":                               "../details/libs/barcode.html"                                    ,
    "libs/bmp":                                   "../details/libs/bmp.html"                                        ,
    "libs/ffmpeg":                                "../details/libs/ffmpeg.html"                                     ,
    "libs/freetype":                              "../details/libs/freetype.html"                                   ,
    "libs/fs":                                    "../details/libs/fs.html"                                         ,
    "libs/gif":                                   "../details/libs/gif.html"                                        ,
    "libs/index":                                 "../details/libs/index.html"                                      ,
    "libs/lfs":                                   "../details/libs/lfs.html"                                        ,
    "libs/libjpeg_turbo":                         "../details/libs/libjpeg_turbo.html"                              ,
    "libs/libpng":                                "../details/libs/libpng.html"                                     ,
    "libs/lodepng":                               "../details/libs/lodepng.html"                                    ,
    "libs/qrcode":                                "../details/libs/qrcode.html"                                     ,
    "libs/rle":                                   "../details/libs/rle.html"                                        ,
    "libs/rlottie":                               "../details/libs/rlottie.html"                                    ,
    "libs/svg":                                   "../details/libs/svg.html"                                        ,
    "libs/tiny_ttf":                              "../details/libs/tiny_ttf.html"                                   ,
    "libs/tjpgd":                                 "../details/libs/tjpgd.html"                                      ,
    "others/file_explorer":                       "../details/auxiliary-modules/file_explorer.html"                 ,
    "others/font_manager":                        "../details/auxiliary-modules/font_manager.html"                  ,
    "others/fragment":                            "../details/auxiliary-modules/fragment.html"                      ,
    "others/gridnav":                             "../details/auxiliary-modules/gridnav.html"                       ,
    "others/ime_pinyin":                          "../details/auxiliary-modules/ime_pinyin.html"                    ,
    "others/imgfont":                             "../details/auxiliary-modules/imgfont.html"                       ,
    "others/index":                               "../details/auxiliary-modules/index.html"                         ,
    "others/monkey":                              "../details/auxiliary-modules/monkey.html"                        ,
    "others/obj_id":                              "../details/auxiliary-modules/obj_id.html"                        ,
    "others/obj_property":                        "../details/auxiliary-modules/obj_property.html"                  ,
    "others/observer":                            "../details/auxiliary-modules/observer.html"                      ,
    "others/snapshot":                            "../details/auxiliary-modules/snapshot.html"                      ,
    "overview/animations":                        "../details/main-modules/animation.html"                          ,
    "overview/color":                             "../details/main-modules/color.html"                              ,
    "overview/coord":                             "../details/common-widget-features/coord.html"                    ,
    "overview/debugging/gdb_plugin":              "../../details/debugging/gdb_plugin.html"                         ,
    "overview/debugging/index":                   "../../details/debugging/index.html"                              ,
    "overview/debugging/log":                     "../../details/debugging/log.html"                                ,
    "overview/debugging/profiler":                "../../details/debugging/profiler.html"                           ,
    "overview/debugging/vg_lite_tvg":             "../../details/debugging/vg_lite_tvg.html"                        ,
    "overview/display":                           "../details/main-modules/display/index.html"                      ,
    "overview/event":                             "../details/common-widget-features/event.html"                    ,
    "overview/font":                              "../details/main-modules/font.html"                               ,
    "overview/fs":                                "../details/main-modules/fs.html"                                 ,
    "overview/image":                             "../details/main-modules/image.html"                              ,
    "overview/indev":                             "../details/main-modules/indev.html"                              ,
    "overview/index":                             "../details/main-modules/index.html"                              ,
    "overview/layer":                             "../details/common-widget-features/layer.html"                    ,
    "overview/new_widget":                        "../details/widgets/new_widget.html"                              ,
    "overview/obj":                               "../details/common-widget-features/obj.html"                      ,
    "overview/renderers/arm2d":                   "../../details/integration/renderers/arm2d.html"                  ,
    "overview/renderers/index":                   "../../details/integration/renderers/index.html"                  ,
    "overview/renderers/nema_gfx":                "../../details/integration/renderers/nema_gfx.html"               ,
    "overview/renderers/pxp":                     "../../details/integration/renderers/nxp_pxp.html"                ,
    "overview/renderers/sdl":                     "../../details/integration/renderers/sdl.html"                    ,
    "overview/renderers/stm32_dma2d":             "../../details/integration/renderers/stm32_dma2d.html"            ,
    "overview/renderers/sw":                      "../../details/integration/renderers/sw.html"                     ,
    "overview/renderers/vg_lite":                 "../../details/integration/renderers/vg_lite.html"                ,
    "overview/renderers/vglite":                  "../../details/integration/renderers/nxp_vglite_gpu.html"         ,
    "overview/scroll":                            "../details/common-widget-features/scroll.html"                   ,
    "overview/style":                             "../details/common-widget-features/styles/styles.html"            ,
    "overview/style-props":                       "../details/common-widget-features/styles/style-properties.html"  ,
    "overview/timer":                             "../details/main-modules/timer.html"                              ,
    "porting/display":                            "../details/main-modules/display/index.html"                      ,
    "porting/draw":                               "../details/main-modules/draw.html"                               ,
    "porting/indev":                              "../details/main-modules/indev.html"                              ,
    "porting/index":                              "../details/integration/adding-lvgl-to-your-project/index.html"   ,
    "porting/os":                                 "../details/integration/adding-lvgl-to-your-project/threading.html",
    "porting/project":                            "../details/integration/adding-lvgl-to-your-project/connecting_lvgl.html",
    "porting/sleep":                              "../details/integration/adding-lvgl-to-your-project/threading.html",
    "porting/tick":                               "../details/integration/adding-lvgl-to-your-project/connecting_lvgl.html",
    "porting/timer_handler":                      "../details/integration/adding-lvgl-to-your-project/timer_handler.html",
    "widgets/animimg":                            "../details/widgets/animimg.html"                                 ,
    "widgets/arc":                                "../details/widgets/arc.html"                                     ,
    "widgets/bar":                                "../details/widgets/bar.html"                                     ,
    "widgets/button":                             "../details/widgets/button.html"                                  ,
    "widgets/buttonmatrix":                       "../details/widgets/buttonmatrix.html"                            ,
    "widgets/calendar":                           "../details/widgets/calendar.html"                                ,
    "widgets/canvas":                             "../details/widgets/canvas.html"                                  ,
    "widgets/chart":                              "../details/widgets/chart.html"                                   ,
    "widgets/checkbox":                           "../details/widgets/checkbox.html"                                ,
    "widgets/dropdown":                           "../details/widgets/dropdown.html"                                ,
    "widgets/image":                              "../details/widgets/image.html"                                   ,
    "widgets/imagebutton":                        "../details/widgets/imagebutton.html"                             ,
    "widgets/index":                              "../details/widgets/index.html"                                   ,
    "widgets/keyboard":                           "../details/widgets/keyboard.html"                                ,
    "widgets/label":                              "../details/widgets/label.html"                                   ,
    "widgets/led":                                "../details/widgets/led.html"                                     ,
    "widgets/line":                               "../details/widgets/line.html"                                    ,
    "widgets/list":                               "../details/widgets/list.html"                                    ,
    "widgets/lottie":                             "../details/widgets/lottie.html"                                  ,
    "widgets/menu":                               "../details/widgets/menu.html"                                    ,
    "widgets/msgbox":                             "../details/widgets/msgbox.html"                                  ,
    "widgets/obj":                                "../details/widgets/base_widget.html"                             ,
    "widgets/roller":                             "../details/widgets/roller.html"                                  ,
    "widgets/scale":                              "../details/widgets/scale.html"                                   ,
    "widgets/slider":                             "../details/widgets/slider.html"                                  ,
    "widgets/span":                               "../details/widgets/spangroup.html"                               ,
    "widgets/spinbox":                            "../details/widgets/spinbox.html"                                 ,
    "widgets/spinner":                            "../details/widgets/spinner.html"                                 ,
    "widgets/switch":                             "../details/widgets/switch.html"                                  ,
    "widgets/table":                              "../details/widgets/table.html"                                   ,
    "widgets/tabview":                            "../details/widgets/tabview.html"                                 ,
    "widgets/textarea":                           "../details/widgets/textarea.html"                                ,
    "widgets/tileview":                           "../details/widgets/tileview.html"                                ,
    "widgets/win":                                "../details/widgets/win.html"                                     ,
    "details/widgets/span":                       "../../details/widgets/spangroup.html"                            ,
}


# Example configuration for intersphinx: refer to the Python standard library.

def setup(app):
    pass
    # app.add_config_value('recommonmark_config', {
    #         'enable_eval_rst': True,
    #         'enable_auto_toc_tree': 'True',
    #         }, True)
    # app.add_transform(AutoStructify)
    # app.add_css_file('css/custom.css')
    # app.add_css_file('css/fontawesome.min.css')


