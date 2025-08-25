.. _yocto_project_core_components:

=============================
Yocto Project Core Components
=============================

The BitBake task executor together with various types of configuration files
form the OpenEmbedded-Core (OE-Core). This section overviews these components
by describing their use and how they interact.

BitBake handles the parsing and execution of the data files. The data
itself is of various types:

-  *Recipes:* Provides details about particular pieces of software.

-  *Class Data:* Abstracts common build information (e.g. how to build a
   Linux kernel).

-  *Configuration Data:* Defines machine-specific settings, policy
   decisions, and so forth. Configuration data acts as the glue to bind
   everything together.

BitBake knows how to combine multiple data sources together and refers
to each data source as a layer.

Here are some brief details on these core components.

.. _bitbake_section:

BitBake
*******

BitBake is the tool at the heart of the OpenEmbedded Build System and is
responsible for parsing the Metadata, generating a list of tasks from it, and
then executing those tasks.

This section briefly introduces BitBake. If you want more information on
BitBake, see the `BitBake User Manual <https://docs.yoctoproject.org/bitbake/2.
8/index.html>`_.

To see a list of the options BitBake supports, use either of the
following commands::

   $ bitbake -h
   $ bitbake --help

The most common usage for BitBake is ``bitbake recipename``, where
``recipename`` is the name of the recipe you want to build (referred
to as the "target"). The target often equates to the first part of a
recipe's filename (e.g. "foo" for a recipe named ``foo_1.3.0-r0.bb``).
So, to process the ``matchbox-desktop_1.2.3.bb`` recipe file, you might
type the following::

   $ bitbake matchbox-desktop

Several different versions of ``matchbox-desktop`` might exist. BitBake chooses
the one selected by the distribution configuration. You can get more details
about how BitBake chooses between different target versions and providers in the
"`Preferences <https://docs.yoctoproject.org/bitbake/2.8/bitbake-user-manual/
bitbake-user-manual-execution.html#preferences>`_" section of the BitBake User
Manual.

BitBake also tries to execute any dependent tasks first. So for example,
before building ``matchbox-desktop``, BitBake would build a cross
compiler and ``glibc`` if they had not already been built.

A useful BitBake option to consider is the ``-k`` or ``--continue``
option. This option instructs BitBake to try and continue processing the
job as long as possible even after encountering an error. When an error
occurs, the target that failed and those that depend on it cannot be
remade. However, when you use this option other dependencies can still
be processed.

.. _recipes_section:

Recipes
*******

Files that have the ``.bb`` suffix are "recipes" files. In general, a
recipe contains information about a single piece of software. This
information includes the location from which to download the unaltered
source, any source patches to be applied to that source (if needed),
which special configuration options to apply, how to compile the source
files, and how to package the compiled output.

The term "package" is sometimes used to refer to recipes. However, since
the word "package" is used for the packaged output from the OpenEmbedded
build system (i.e. ``.ipk`` or ``.deb`` files), this document avoids
using the term "package" when referring to recipes.


.. _classes_section:

Classes
*******

Class files (``.bbclass``) contain information that is useful to share
between recipes files. An example is the autotools* class,
which contains common settings for any application that is built with
the `GNU Autotools <https://en.wikipedia.org/wiki/GNU_Autotools>`.
The "`Classes <https://docs.yoctoproject.org/ref-manual/classes.
html#classes>`_" chapter in the Yocto Project
Reference Manual provides details about classes and how to use them.


.. _configurations_section:

Configurations
**************

The configuration files (``.conf``) define various configuration
variables that govern the OpenEmbedded build process. These files fall
into several areas that define machine configuration options,
distribution configuration options, compiler tuning options, general
common configuration options, and user configuration options in
``conf/local.conf``, which is found in the `Build Directory <https://docs.
yoctoproject.org/ref-manual/terms.html#term-Build-Directory>`_.

.. _layers_section:

Layers
******

Layers are repositories that contain related metadata (i.e. sets of
instructions) that tell the OpenEmbedded build system how to build a
target. `The yocto project layer model <https://docs.yoctoproject.org/
overview-manual/yp-intro.html#the-yocto-project-layer-model>`_
facilitates collaboration, sharing, customization, and reuse within the
Yocto Project development environment. Layers logically separate
information for your project. For example, you can use a layer to hold
all the configurations for a particular piece of hardware. Isolating
hardware-specific configurations allows you to share other metadata by
using a different layer where that metadata might be common across
several pieces of hardware.

There are many layers working in the Yocto Project development environment. The
`Yocto Project Compatible Layer Index <https://www.yoctoproject.org/development/
yocto-project-compatible-layers/>`_ and `OpenEmbedded Layer Index <https://
layers.openembedded.org/layerindex/branch/master/layers/>`_ both contain layers
from
which you can use or leverage.

By convention, layers in the Yocto Project follow a specific form. Conforming
to a known structure allows BitBake to make assumptions during builds on where
to find types of metadata. You can find procedures and learn about tools (i.e.
``bitbake-layers``) for creating layers suitable for the Yocto Project in the
"`understanding and creating layers <https://docs.yoctoproject.org/dev-manual/
layers.html#understanding-and-creating-layers>`_" section of the
Yocto Project Development Tasks Manual.
