.. _editor_figma:

=================
Figma integration
=================

Overview
********

When Figma is used to create a UI design, parts of it can be directly imported (or
updated).


Usage
*****

To enable sync-ing with a Figma project, add a ``project_local.xml`` file if it isn't
already there and add this to its content:

.. code-block:: xml

    <project_local>
        <figma document="XXXXXX" token="YYYYY"/>
    </project_local>

where:

:XXXXXX:   is the string after "design" in the Figma URL.  For example, if the Figma
           URL is ``https://www.figma.com/design/P8NM0qOqgWfYpbENhAKqYb``, then the
           XXXXXX would be "P8NM0qOqgWfYpbENhAKqYb".

:YYYYY:    is the Personal Access Token that you can generate as described in the
           `Manage Personal Access Tokens <https://help.figma.com/hc/en-us/articles/8085703771159-Manage-personal-access-tokens>`__
           article in Figma documentation.

Then, for each target style you want to be linked to a style from the Figma design,
include a ``figma_node_id=...`` attribute in it.  Thereafter, clicking the FigmaSync
button will update each respective style from the updated Figma design.


LVGL Figma Plugin
-----------------


Connecting Figma to the Editor
------------------------------


Synchronizing Data
------------------

