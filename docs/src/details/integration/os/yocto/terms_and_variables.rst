.. _yocto_project_terms:

===================
Yocto Project Terms
===================

Getting started in Yocto can be overwheming. There are many terms used that are
specific to Yocto and Bitbake environment.

A list of terms and definitions users new to the Yocto Project
development environment might find helpful can be found `here <https://docs.
yoctoproject.org/ref-manual/terms.html>`_.


Yocto Variables Glossary
************************

This chapter lists basic variables used in the LVGL Yocto guide and gives an
overview of their function and contents.

A More complete variable glossary can be found in `Yocto Variable Glossary
<https://docs.yoctoproject.org/ref-manual/variables.html>`_. This section
covers a lot of variables used in the OpenEmbedded build system.

.. _S:

S
-

The location in the Build Directory where unpacked recipe source code resides.
 By default, this directory is ${WORKDIR}/${BPN}-${PV}, where ${BPN} is the
 base recipe name and ${PV} is the recipe version. If the source tarball
 extracts the code to a directory named anything other than ${BPN}-${PV}, or if
 the source code is fetched from an SCM such as Git or Subversion, then you
 must set S in the recipe so that the OpenEmbedded build system knows where to
 find the unpacked source.

As an example, assume a Source Directory top-level folder named poky and a
default Build Directory at poky/build. In this case, the work directory the
build system uses to keep the unpacked recipe for db is the following:

.. code-block:: bash

   poky/build/tmp/work/qemux86-poky-linux/db/5.1.19-r3/db-5.1.19

The unpacked source code resides in the db-5.1.19 folder.

This next example assumes a Git repository. By default, Git repositories are
cloned to ${WORKDIR}/git during do_fetch. Since this path is different from the
default value of S, you must set it specifically so the source can be located:

.. code-block:: bash

   SRC_URI = "git://path/to/repo.git;branch=main"
   S = "${WORKDIR}/git"


.. _D:

D
-

The destination directory. The location in the Build Directory where components
are installed by the do_install task. This location defaults to:

.. code-block:: bash

   ${WORKDIR}/image

.. note::

    Tasks that read from or write to this directory should run under fakeroot.

.. _B:

B
-

The directory within the Build Directory in which the OpenEmbedded build system
places generated objects during a recipe's build process. By default, this
directory is the same as the S directory, which is defined as:

.. code-block:: bash

   S = "${WORKDIR}/${BP}"

You can separate the (S) directory and the directory pointed to by the B
variable. Most Autotools-based recipes support separating these directories.
The build system defaults to using separate directories for gcc and some kernel
recipes.

.. _WORKDIR:

WORKDIR
-------

The pathname of the work directory in which the OpenEmbedded build system
builds a recipe. This directory is located within the TMPDIR directory
structure and is specific to the recipe being built and the system for which it
is being built.

The WORKDIR directory is defined as follows:

.. code-block:: bash

   ${TMPDIR}/work/${MULTIMACH_TARGET_SYS}/${PN}/${EXTENDPE}${PV}-${PR}

The actual directory depends on several things:

   -  **TMPDIR**: The top-level build output directory
   -  **MULTIMACH_TARGET_SYS**: The target system identifier
   -  **PN**: The recipe name
   -  **EXTENDPE**: The epoch — if PE is not specified, which is usually the
      case for most recipes, then EXTENDPE is blank.
   -  **PV**: The recipe version
   -  **PR**: The recipe revision

As an example, assume a Source Directory top-level folder name poky, a default
Build Directory at poky/build, and a qemux86-poky-linux machine target system.
Furthermore, suppose your recipe is named foo_1.3.0-r0.bb. In this case, the
work directory the build system uses to build the package would be as follows:

.. code-block:: bash

   poky/build/tmp/work/qemux86-poky-linux/foo/1.3.0-r0

.. _PN:

PN
--

This variable can have two separate functions depending on the context: a
recipe name or a resulting package name.

PN refers to a recipe name in the context of a file used by the OpenEmbedded
build system as input to create a package. The name is normally extracted from
the recipe file name. For example, if the recipe is named expat_2.0.1.bb, then
the default value of PN will be “expat”.

The variable refers to a package name in the context of a file created or
produced by the OpenEmbedded build system.

If applicable, the PN variable also contains any special suffix or prefix. For
example, using bash to build packages for the native machine, PN is
bash-native. Using bash to build packages for the target and for Multilib, PN
would be bash and lib64-bash, respectively.

.. _PR:

PR
--

The revision of the recipe. The default value for this variable is
"r0". Subsequent revisions of the recipe conventionally have the
values "r1", "r2", and so forth. When PV increases,
PR is conventionally reset to "r0".

.. note::

    The OpenEmbedded build system does not need the aid of PR to know when to
    rebuild a recipe. The build system uses the task input checksums along with
    the stamp and shared state cache mechanisms.

The PR variable primarily becomes significant when a package
manager dynamically installs packages on an already built image. In
this case, PR, which is the default value of
PKGR, helps the package manager distinguish which
package is the most recent one in cases where many packages have the
same PV (i.e. PKGV). A component having many packages with
the same PV usually means that the packages all install the same
upstream version, but with later (PR) version packages including
packaging fixes.

.. note::

    PR does not need to be increased for changes that do not change the
    package contents or metadata.

Because manually managing PR can be cumbersome and error-prone,
an automated solution exists. See the
"`working with a pr service <https://docs.yoctoproject.org/dev-manual/packages.
html#working-with-a-pr-service>`_" section in the Yocto Project Development
Tasks Manual for more information.

.. _PV:

PV
--

The version of the recipe. The version is normally extracted from the recipe
filename. For example, if the recipe is named expat_2.0.1.bb, then the default
value of PV will be “2.0.1”. PV is generally not overridden within a recipe
unless it is building an unstable (i.e. development) version from a source code
repository (e.g. Git or Subversion).

PV is the default value of the PKGV variable.
