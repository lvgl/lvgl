.. _editor_cli:

===
CLI
===

The LVGL Editor provides a Node-based command line tool, ``lved-cli.js``, for generating/compiling code from XML, validating projects, syncing from Figma, comparing trees, and running headless UI tests.

.. note::
   Pre-release build. Intended for development and CI. Expires end of 2025.

Overview
********

- **Binary:** ``lved-cli.js`` (Node script) `Download CLI <https://github.com/lvgl/lvgl_editor/releases>`_
- **Platform:** Node 18+ recommended
- **Container engine:** Podman (if not on Windows, `Download Podman <https://github.com/containers/podman/releases>`_)


Quick start
***********

Generate C and H files from XMLs:

.. code-block:: bash

   lved-cli.js generate path/to/project

Compile (web or node) the runtime Binary for previewing or testing the UI:

.. code-block:: bash

   lved-cli.js compile path/to/project --target web --start-service

Validate content of the XML files (limit shown errors):

.. code-block:: bash

   lved-cli.js validate path/to/project --errorlimit 25

Run the ``<test>``\ (s) from the XML files

.. code-block:: bash

   lved-cli.js run-all-tests path/to/project
   lved-cli.js run-test path/to/project tests/test-file.xml

Synchronize the styles and images from Figma:

.. code-block:: bash

   lved-cli.js figma-sync path/to/project --start-service

Commands
********

generate
--------

Generate code from XML.

.. code-block:: bash

   lved-cli.js generate <project-path> [--start-service]


compile
-------

Compile for a target.

.. code-block:: bash

   lved-cli.js compile <project-path> [--start-service] [--target <web|node>]

- ``--target`` defaults to ``web`` (choices: ``web``, ``node``)


compare
-------

Compare two directories (presence + normalized content).

.. code-block:: bash

   lved-cli.js compare <first-project-path> <second-project-path>


figma-sync
----------

Sync project with Figma.

.. code-block:: bash

   lved-cli.js figma-sync <project-path> [--start-service]


validate
--------

Validate XML and limit shown errors.

.. code-block:: bash

   lved-cli.js validate <project-path> [--errorlimit <n>] [--start-service]

- ``--errorlimit`` default: ``10`` (min 1)


run-test
--------

Run tests from a single file.

.. code-block:: bash

   lved-cli.js run-test <project-path> <testing-file>


run-all-tests
-------------

Discover and run all ``test*.xml`` files.

.. code-block:: bash

   lved-cli.js run-all-tests <project-path>

Common Arguments
----------------

- ``<project-path>`` – path to an LVGL Editor project
- ``<testing-file>`` – test XML relative to ``<project-path>`` (``run-test``)

Options
-------

- ``-ss, --start-service`` – prepare/refresh container image before running
- ``--target <web|node>`` – compile target (``compile``)
- ``-l, --errorlimit <n>`` – max errors shown (``validate``)

Examples
--------

Compare to a reference tree:

.. code-block:: bash

   lved-cli.js compare build/generated ./ci/reference

Compile for Node:

.. code-block:: bash

   lved-cli.js compile ./examples/my-project --target node --start-service


CI/CD
-----

See an example `GitHub Actions workflow <https://github.com/lvgl/lvgl_editor/blob/master/.github/workflows/pr-check.yml>`_

