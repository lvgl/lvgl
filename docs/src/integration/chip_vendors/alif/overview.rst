.. _alif_overview:

========
Overview
========

Alif Semiconductor is a next-generation chip vendor making chips that come in a
variety of configurations. Their chips are designed to excel at multiple aspects
at once. They offer combinations of performance, low power consumption,
security, and special functionality like AI. Many of their chips have two asymmetrical cores.
One core is typically high performance while the other is high efficiency.
Alif offers both microcontrollers and microprocessors.


LVGL on Alif Boards
*******************

This is a guide for getting started with LVGL on an Alif board. It specifically details
the all the steps needed to get the LVGL example project
`alif_m55-lvgl <https://github.com/alifsemi/alif_m55-lvgl>`__ running on the
`Alif E7 Devkit Gen2 <https://alifsemi.com/ensemble-e7-series/>`__; however, any project
based on the `Alif VS Code Template <https://github.com/alifsemi/alif_vscode-template>`__
has a nearly identical setup process
so this can be used as a general guide for those. There are other ways to compile for Alif boards
such as with Zephyr RTOS. See `Alif's GitHub repos <https://github.com/orgs/alifsemi/repositories>`__.

This guide is for Linux and Windows.

This project uses D/AVE 2D rendering acceleration with LVGL's D/AVE 2D :ref:`draw unit<draw>`.


Step-by-Step Guide
------------------

Install Visual Studio Code
~~~~~~~~~~~~~~~~~~~~~~~~~~

Install Visual Studio code. There are different ways of installing it depending on your platform.
`See here <https://code.visualstudio.com/docs/setup/setup-overview>`__.

.. note::

    The remaining steps can **optionally** be done inside a Docker container. You can connect to the Docker container
    as a VS Code remote dev container.

    There should be two serial ports created upon connecting your Alif board. On Linux, they will be something
    like ``/dev/ttyACM0`` and ``/dev/ttyACM1``. In the ``docker run`` command you use to create the dev
    container, include ``--device /dev/ttyACM0 --device /dev/ttyACM1`` to give the container access to those
    ports so you can flash from it.

    Install the "Dev Containers" VS Code extension. Select your container from the "Remote Explorer" on the left
    side panel.


Install Prerequisite tools
~~~~~~~~~~~~~~~~~~~~~~~~~~

Make sure these are installed in your environment. The VS Code extensions rely on these being present.

- ``git``
- ``curl``
- ``unzip``

Install the Alif SE Tools
~~~~~~~~~~~~~~~~~~~~~~~~~

Create an Alif account and download the tools from
`here <https://alifsemi.com/support/software-tools/ensemble/>`__ under "Alif Security Toolkit".
Extract it. The path where it was extracted will be needed later.

On Linux, extracting can be done by running the following

.. code-block:: shell

    cd
    tar -xf Downloads/APFW0002-app-release-exec-linux-SE_FW_1.101.00_DEV.tar
    ls
    pwd

Among the results of ``ls`` you should see ``app-release-exec-linux``. That, combined
with the output of ``pwd``, is the path you need to use later. I.e.,
``/home/you/app-release-exec-linux``.

Install J-Link Software (optional)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Download the latest stable version of the `J-Link Software <https://www.segger.com/downloads/jlink>`__.
Its installation path will be needed later.

Clone the ``alif_m55-lvgl`` Project
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: shell

    git clone --recursive https://github.com/alifsemi/alif_m55-lvgl

Open ``alif_m55-lvgl`` in VS Code
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Open the cloned repo in VS Code. For the VS Code extensions to work properly,
it's recommended to open the folder in VS Code instead of opening a
containing parent directory.

.. code-block:: shell

    code alif_m55-lvgl

or navigate to **File \> Open Folder** in VS Code and open ``alif_m55-lvgl``.

If you are prompted to automatically install recommended extensions, click
"install" so you can skip the next step.

Install Required VS Code Extensions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Install the following VS Code extensions from the "Extensions" side panel

- Arm Tools Environment Manager
- Arm CMSIS Solution
- C/C++ Extension Pack
- Cortex-Debug (optional. needed for debugging)

Activate Environment
~~~~~~~~~~~~~~~~~~~~

If it hasn't happened automatically, Click "Arm Tools" on the bottom bar and then
click "Activate Environment" in the list that appears. It will install CMake,
ninja-build, a GCC ARM compiler, and cmsis-toolbox.

If you only see "Reactivate Environment" then it is likely already active.

Set the Paths of Installed Tools
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Press ctrl + shift + p. Type "preferences" and select the option
"Preferences: Open User Settings (JSON)" from the choices.
A ``settings.json`` will open. Note: if using a Docker container,
it's better to edit the JSON file in the project directory at
``.vscode/settings.json``.

You need to add some entries (at least ``"alif.setools.root"``)
to the JSON you see.

If your ``settings.json`` looks like this initially...

.. code-block:: json

    {
        "workbench.colorTheme": "Default Dark+",
        "editor.renderWhitespace": "all",
    }

... then it should look like this afterwards:

.. code-block:: json

    {
        "workbench.colorTheme": "Default Dark+",
        "editor.renderWhitespace": "all",
        "alif.setools.root" : "C:/alif-se-tools/app-release-exec",
        "cortex-debug.JLinkGDBServerPath": "C:/Program Files/SEGGER/JLink/JLinkGDBServerCL.exe"
    }

The above uses Windows paths as an example. A Linux path to the Alif SE Tools may look
something like ``"/home/you/app-release-exec-linux"``.

Configure the Board Variant
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Open the
`board.h file <https://github.com/alifsemi/alif_vscode-template/blob/ce5423dbd15f62cb0aa4462533a960d79a014f97/board/board.h#L23-L30>`__.

Identify your board variant in the list and set ``BOARD_ALIF_DEVKIT_VARIANT`` to the correct value.
You may also need to set ``BOARD_ILI9806E_PANEL_VARIANT`` if the default does not match yours.

Set Up the Build Context, Compile, and Flash
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Get to the "Manage Solution" view from the CMSIS Solution extension. You can reach
it by either clicking the gear icon on the bottom bar or by navigating to the CMSIS panel
on the left and clicking the gear at the top of that view. When it's open, it's a
graphical editor tab called "Manage Solution" with a gear icon.

**Important**

Under "Run and Debug" \> "Run Configuration" (column) \> "alif" (row), click the dropdown and select
"First time pack installation". Click the play icon at the top of the CMSIS left side panel.
In your terminal you should see CMSIS packs being installed. Wait for it to complete.

Now you can click the hammer icon next to the play icon to compile the project.
A few hundred files will be compiled. Wait for it to complete.

Open the dropdown from before and choose "Prepare program with Security Toolkit". Click the
play icon. It prepares some files as a prior step to flashing.

Open the dropdown again and choose "Program with Security Toolkit". Click the play icon.
If this is the first time, you will be prompted to choose which serial port to use to flash
the board. You can try ``/dev/ttyACM0``. If it was the wrong one, it will fail and you will
need to open the dropdown and choose "Program with Security Toolkit (select COM port)"
to override the previous one which was saved as default.

The LVGL benchmark should run on your Alif board after flashing completes.

More Info
---------

If there were any difficulties faced while following this guide, refer to these
Alif sources for more detailed steps.

- `Getting Started with VSCode CMSIS pack project <https://github.com/alifsemi/alif_vscode-template/blob/main/doc/getting_started.md>`__
- `VSCode Getting Started Template <https://github.com/alifsemi/alif_vscode-template/blob/main/README.md>`__

You can download the "Alif Security Toolkit Quick Start Guide" from https://alifsemi.com/support/software-tools/ensemble/ ,
assuming you have created an account, to learn how to use the Alif SE Tools to perform
low-level manipulations on your board.

HP and HE Cores and Optimized Build
-----------------------------------

In the "Manage Solution" view explained in the guide, there is an option to select either
an HP target or an HE target. What these are referring to are the two distinct cores
present in the Alif E7. "HE" stands for "High Efficiency" while "HP" stands for
"High Performance". To get the best performance out of an LVGL application, select HP.
Consider HE when power usage is a concern. The merit of having asymmetrical cores
is that your application can run theoretically run low-priority workloads efficiently on
the HE core and delegate time critical, processing intensive workloads to the HP core.

There is also an option to choose a "Build Type". For best performance, choose "release".
If debugging you will want "debug".

To maximize the score on the LVGL benchmark and maximize the performance of an LVGL
application in general, ensure the HP core is selected and the build type is release.

