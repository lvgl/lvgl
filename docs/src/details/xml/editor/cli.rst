.. include:: custom_tools.txt

.. _command-line interface:

======================
Command-Line Interface
======================

The |lvglpro| Command-Line Interface (CLI) is able to accomplish some of the same tasks
the |lvglpro| Editor does without the editor user interface.

Namely:

:generate:    Generate code from XML source files
:compile:     Compile an |lvglpro| project
:compare:     Compare generated code with reference examples
:figma-sync:  Sync project with Figma designs
:validate:    Validate LVGL code
:run-tests:   Run pre-prepared UI interaction tests

This allows automation to be applied when there are, for example, hand-edited
(or otherwise updated) XML files, for translation into source files and/or updated
build artefacts for the target project.

The CLI logic lives in a JavaScript file:  ``lved-cli.js``.  If that file was in a
subdirectory ``./lvgl-cli``, then the following would be some example uses of the CLI
from the command line:

.. code-block:: bash

    # Generate Code
    $ node ./lvgl-cli/lved-cli.js generate project_dir -ss

    # Compile Code
    $ node ./lvgl-cli/lved-cli.js compile project_dir --target node

    # Run tests
    $ node ./lvgl-cli/lved-cli.js run-all-tests project_dir


The following is an example of how the CLI is used on GitHub from the
https://github.com/lvgl/lvgl_editor/  repository.  The source for this file can be found
at https://github.com/lvgl/lvgl_editor/blob/master/.github/workflows/pr-check.yml:

.. code-block:: yaml

    name: Pull Request Check
    on:
      pull_request:
        types: [opened, edited, synchronize, reopened]
    jobs:
      pr-check:
        runs-on: ubuntu-latest

        steps:
          - name: Checkout current repository
            uses: actions/checkout@v4

          - name: Setup Node.js
            uses: actions/setup-node@v4
            with:
              node-version: "22.13.0"

          - name: Remove Generated Files
            run: find examples -path examples/images -prune -o -type f \( -name '*.c' -o -name '*.h' \) -print | xargs rm -f

          - name: Download CLI
            run: curl -L https://github.com/lvgl/lvgl_editor/releases/download/v.0.3.0/LVGL_CLI-0.3.0-linux-mac.zip -o lvgl-cli.zip

          - name: Unzip CLI
            run: unzip lvgl-cli.zip -d lvgl-cli

          - name: Generate Code
            run: |
              node ./lvgl-cli/lved-cli.js generate examples -ss

          - name: Compile Code
            run: |
              node ./lvgl-cli/lved-cli.js compile examples --target node

          - name: Run tests
            run: |
              node ./lvgl-cli/lved-cli.js run-all-tests examples
