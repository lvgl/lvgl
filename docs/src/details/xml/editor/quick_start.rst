.. include:: custom_tools.txt

.. _quick start:

===========
Quick Start
===========

The following is a quick project presented in tutorial style to help get you oriented
with |lvglpro|'s User Interface, and how it can fit into your firmware projects'
work flows.



Getting |lvglpro|
*****************

Navigate to https://github.com/lvgl/lvgl_editor/, scan down the right panel and
find "Releases".

If the version you want to download is showing, click it to go to the downloads page.
If not, click on "+ N releases", then find and click on the release you want to
download.

Note:  this method will change as |lvglpro| reaches commercial-release stage.

Once on the desired download page, right click on the file appropriate for your
platform and select "Save Link As...", and save it on your workstation.



Installing
**********

Extract the file(s) from the compressed archive file and execute the implied steps to
install |lvglpro|.  On Windows, that means launch the ``LVGL Pro Setup
x.x.x.exe`` installer.



First Run
*********

The first time you launch |lvglpro|, it will pop up a window with the message

    "Pulling Container image for LVGL, this may take a few minutes...".

This message means the Editor is downloading a pre-configured a container image that
includes all the necessary tools and dependencies for building LVGL projects.  It’s
similar to setting up a Python virtual environment, but |lvglpro| handles
(and updates) the container automatically in the background.  The process can take a
few minutes the first time, depending on your internet speed, but it only happens
once.  After that, the container is reused for future sessions.

When a new version of the container is published (e.g. with updated toolchains, build
logic, etc.), the Editor automatically checks for and pulls the latest version.  You
won’t need to do anything manually, it stays up-to-date in the background.



Tutorial Project
****************


Create a New UI Project
-----------------------

Open |lvglpro| and click [Create Project].  Navigate to the directory you want to
be *above* the your project directory, and give your new project a name that is also
a valid directory name for your system.  |lvglpro| will create an empty directory
with that name and populate your new project with the core set of files needed.


``project.xml`` File
--------------------

Have a look at the contents of this file.  As you might have guessed, this file
links all the parts of the project together as one "unit".

:<target>:   An identifier (name) for the hardware your firmware will run on.  There
             can be any number of these in a single project.

:<display>:  The resolution and color format of each target display panel.  This also
             defines the layout of the Preview Panel where you will see how your
             XML source will look once rendered through LVGL.  If there are multiple
             displays, all can be previewed exactly as they would appear on each
             display.

:<memory>:   Not used yet (for human consumption only), the ``<memory>`` tags serve as
             a convenient reminder about the different memory areas of the target
             hardware that are applicable to this project (flash, SRAM, external RAM,
             SD or any other type of storage is relevant here).

By default, it is set up to output to a 320x240 display panel with RGB565 pixel format.

If the hardware your firmware will run on is different, take the time now to modify
these values to match your hardware and give it a name it will be known by from the
viewpoint of your development team.


Creating an Example Component
-----------------------------

Except for the ``README.md`` file, the Components directory starts out empty.

Select the Components directory in the "Project Tree", right click and select "New
File...".  Call it "my_button.xml".

Next, walk through the steps of creating this source code:

.. code-block:: xml

    <component>
        <styles>
            <style name="style_01" bg_color="0xFF0000">
            </style>
        </styles>
        <view extends="lv_chart" width="80%" height="content">
            <style name="style_01" selector="pressed"/>
            <lv_button>
                <lv_label text="Hello world!" />
            </lv_button>
        </view>
    </component>

Start by typing an open angle bracket ('``<``') and observe the context-sensitive
auto-completion pop-up menu that appears.  You can select an item from that menu,
or keep typing, as is common practice for "smart editors".

Observe that while you are typing, the :ref:`Preview Pane` shows a live preview
showing what the source XML you are creating will look like when rendered in
firmware (or any other platform where LVGL is rendering it).

Now put your cursor inside the "width" value (currently "80%"), and hold down the
``[Ctrl]`` key while hitting up and down arrow keys, and observe the number increments
and decrements by 1.  Next, try the same again, only this time do it with both the
``[Ctrl+Shift]`` keys held down, and observe the number now increments by 10.

Any time there isn't an auto-completion pop-up menu visible and you need one, hit
``[Ctrl+Space]``.

Once you have finished filling in this code, interact with your new UI in the Preview
Pane and observe the background changing color when the Chart is clicked.


Add an API Property
-------------------

Now add the following code above within the ``<component>`` element, above the
``<styles>`` element:

.. code-block:: xml

        <api>
            <prop name="label" type="string" default="text"/>
        </api>

Now you can use this property anywhere within the Component like this:

.. code-block:: xml

        <lv_label text="$label" /> <!-- Use the provided API property -->


Create Another Component that Uses the Original
-----------------------------------------------

Right click on the ``components`` folder again in the :ref:`project files pane`, and
select "New File...".  Create a file called ``my_component.xml``.  Type in the
following XML:

.. code-block:: xml

    <component>
        <view extends="lv_obj" align="center" width="content" height="125">
            <my_button width="content"/>
        </view>
    </component>

Notice the :ref:`preview pane` when you complete the ``<my_button ... />`` element,
and notice that the name of that element takes its name from the name of the XML
file ``my_button.xml``, not from any of its internal attributes.


Styles and Other Property Names
-------------------------------

One of the key features of |lvglpro| is that, if you are already familiar with using
LVGL to create a UI, you will observe that the names and relationships between the
various things you can build and their properties is the same between LVGL and
|lvglpro|.  This helps not only shorten your learning curve, but results in direct
1-to-1 translation between what you produce in your XML source files, and what is
produced in generated C code and/or the generated UI if the firmware loads these XML
files to create the UI at run time.

To demonstrate this, select the line

.. code-block:: xml

            <style name="style_01" selector="pressed"/>

and using the editor's cut-and-paste mechanisms ([Ctrl-X] and [Ctrl-V]), move it from
the Chart you created to be instead inside the ``<lv_button>`` element.

Now interact with the Preview Pane again, and observe that now the style is being
applied to the button instead of the chart.


Conclusion
----------

This concludes the Quick-Start Tutorial.  You should now have enough familiarity with
|lvglpro| UI and its behaviors to dive into the details.

The remaining content is organized so you can read it in sequence, or use it as a
reference as you work.

Enjoy your journey learning how to greatly enhance your efficiency in creating your
future LVGL UIs!

