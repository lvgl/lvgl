.. include:: custom_tools.txt

.. _figma plugin:

============
Figma Plugin
============

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
           `Manage Personal Access Tokens`_ article in Figma documentation.

Then, for each target style you want to be linked to a style from the Figma design,
include a ``figma_node_id=...`` attribute in it.  Thereafter, clicking the FigmaSync
button will update each respective style from the updated Figma design.
