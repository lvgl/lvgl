.. _xml_project:

=======
Project
=======

Overview
********

.. |nbsp|   unicode:: U+000A0 .. NO-BREAK SPACE
    :trim:

A single ``project.xml`` file should be created for each project where the following
content is specified:

:<folders>: Specifies the path to Component Libraries. LVGL's base Widgets are
            always loaded automatically.
:<targets>: Describes various hardware configurations, allowing the UI |nbsp| Editor
            to check if the UI is out of resources and to select different previews
            for each Screen according to the specified displays.



Example
*******

.. code-block:: xml

    <project>
        <folders>
            <folder path="../widget_lib1"/>
            <folder path="/home/user/work/ui_libs/modern"/>
            <folder path="https://github.com/user/repo"/>
        </folders>

        <targets>
            <renesas-RA8D1-EK gpu="true"/>

            <target name="small">
                <display width="320" height="240" color_format="RGB565"/>
                <memory name="int_ram" size="128K"/>
                <memory name="ext_ram" size="2M"/>
                <memory name="int_flash" size="512K"/>
                <memory name="ext_flash" size="32M"/>
            </target>

            <target name="large">
                <display width="1024" height="768" color_format="XRGB8888"/>
                <memory name="int_ram" size="128K"/>
                <memory name="ext_ram" size="2M"/>
                <memory name="int_flash" size="512K"/>
                <memory name="ext_flash" size="32M"/>
            </target>
        </targets>
    </project>
