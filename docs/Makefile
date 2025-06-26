# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
BUILDDIR      = build
SOURCEDIR     = intermediate

# SOURCEDIR can be overridden by LVGL_DOC_BUILD_INTERMEDIATE_DIR
LEN           := $(shell printf '%s' '$(LVGL_DOC_BUILD_INTERMEDIATE_DIR)' | wc -c)

ifeq ($(shell test $(LEN) -gt 0; echo $$?),0)
	SOURCEDIR = $(LVGL_DOC_BUILD_INTERMEDIATE_DIR)
else
	SOURCEDIR = intermediate
endif

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

.PHONY: help Makefile

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
# It can be supplied from an environment variable 'O' or on make command line.
%: Makefile
	$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
