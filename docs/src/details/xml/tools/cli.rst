.. _editor_cli:

===
CLI
===

The LVGL Editor provides a Node-based command line tool, ``lved-cli.js``, for generating/compiling code from XML, validating projects, syncing from Figma, comparing trees, and running headless UI tests.

.. note::
   Pre-release build. Intended for development and CI. Expires end of 2025.

.. toctree::
   :maxdepth: 2
   :local:


Overview
********

- **Binary:** ``lved-cli.js`` (Node script)
- **Platform:** Node 18+ recommended
- **Container engine:** auto-detected (Podman)
- **Resources:** unpacked from ``lvgl-resources.zip`` to an internal directory


Quick start
***********

Generate:

.. code-block:: bash

   lved-cli.js generate path/to/project

Compile (web or node):

.. code-block:: bash

   lved-cli.js compile path/to/project --target web --start-service

Validate (limit shown errors):

.. code-block:: bash

   lved-cli.js validate path/to/project --errorlimit 25

Tests:

.. code-block:: bash

   lved-cli.js run-all-tests path/to/project
   lved-cli.js run-test path/to/project tests/test-file.xml

Figma sync:

.. code-block:: bash

   lved-cli.js figma-sync path/to/project --start-service


Usage
*****

.. code-block:: text

   lved-cli.js [command] [options] [arguments]

**Commands**

- ``generate`` – generate code from XML
- ``compile`` – compile for a target (web/node)
- ``compare`` – compare two directories (presence + normalized content)
- ``figma-sync`` – sync project with Figma
- ``validate`` – validate XML and limit shown errors
- ``run-test`` – run tests from a single file
- ``run-all-tests`` – discover and run all ``test*.xml`` files

**Options**

- ``-ss, --start-service`` – start or refresh the container service before running
- ``--target <web|node>`` – compile target (``compile``)
- ``-l, --errorlimit <n>`` – max errors shown (``validate``)

**Common arguments**

- ``<project-path>`` – path to an LVGL Editor project
- ``<testing-file>`` – test XML relative to ``<project-path>`` (``run-test``)


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

   lved-cli.js compile <project-path> [--start-service] --target <web|node>

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

Minimal GitHub Actions workflow example:

See an example GitHub Actions workflow `Here <https://github.com/lvgl/lvgl_editor/blob/master/.github/workflows/pr-check.yml>`_

