---
EXAMPLES FOR LITTLEV GRAPHICS LIBRARY
---

# Introduction
In the folders you will find simple hardware independent examples to know the key features of Littlev Graphics Library.
The examples are organized in consistent way and can be used as a **Tutorial**.

Every example consist of least an *example_name.c* and *example_name.h* files. You can load an example in your *main* function with **example_name_init()**. The header file has to be included too.

You will find **detailed explanation** in the *c* and *h* files. 

The examples can be enabled/disabled in *lv_conf.h* with **USE_LV_EXAMPLES**.

# 1_x Getting started
With examples in this section you can learn the basics from a simple *Hello world* label to a complex full featured GUI with animations and other effects. You will learn about 
- creating, deleting graphical objects
- modify their attributes
- change their style
- and even creating a new object type

# 2_x GUI control without Touchpad
You can control your GUI not only with Touchpad. The most simple way to put real buttons next to the graphical ones and simulate *touch pad-like press* on the display. An other way is to organize the objects in groups and *focus* on one of objects. Then you can control the object-in-focus with simple character instructions. Or using a cursor and mouse is also possible.  So an external control device can be
- push buttons
- encoders
- keyboard 
- or a mouse

In this section you will find examples for them!
 
# 3_x Applications
The applications are high level components to do complex thing with a nice user interface. An application can be for example
- WiFi network manager
- File system browser
- System monitor
- Terminal etc. 

Here you will find examples how to run application, and communicate with them. 

# Final words
If you also have codes which could be useful for others, please share it via *Pull requests* on *GitHub*!
Thank you in advance! 

