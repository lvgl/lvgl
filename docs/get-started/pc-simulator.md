```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/get-started/pc-simulator.md
```
# Simulator on PC


You can try out LVGL **using only your PC** (i.e. without any development boards). LVGL will run on a simulator environment on the PC where anyone can write and experiment with real LVGL applications.

Using the simulator on a PC has the following advantages:
- Hardware independent - Write code, run it on the PC and see the result on a monitor.
- Cross-platform - Any Windows, Linux or macOS system can run the PC simulator.  
- Portability - The written code is portable, which means you can simply copy it when migrating to embedded hardware.
- Easy Validation - The simulator is also very useful to report bugs because it provides a common platform for every user. So it's a good idea to reproduce a bug in the simulator and use that code snippet in the [Forum](https://forum.lvgl.io).

## Select an IDE

The simulator is ported to various IDEs (Integrated Development Environments). Choose your favorite IDE, read its README on GitHub, download the project, and load it to the IDE.

- [Eclipse with SDL driver](https://github.com/lvgl/lv_sim_eclipse_sdl): Recommended on Linux and Mac
- [CodeBlocks](https://github.com/lvgl/lv_sim_codeblocks_win): Recommended on Windows
- [VisualStudio with SDL driver](https://github.com/lvgl/lv_sim_visual_studio_sdl): For Windows
- [VSCode with SDL driver](https://github.com/lvgl/lv_sim_vscode_sdl): Recommended on Linux and Mac
- [PlatformIO with SDL driver](https://github.com/lvgl/lv_platformio): Recommended on Linux and Mac

You can use any IDE for development but, for simplicity, the configuration for Eclipse CDT is what we'll focus on in this tutorial.
The following section describes the set-up guide of Eclipse CDT in more detail.

**Note: If you are on Windows, it's usually better to use the Visual Studio or CodeBlocks projects instead. They work out of the box without requiring extra steps.**

## Set-up Eclipse CDT

### Install Eclipse CDT

[Eclipse CDT](https://eclipse.org/cdt/) is a C/C++ IDE.

Eclipse is a Java-based tool so be sure **Java Runtime Environment** is installed on your system.   

On Debian-based distros (e.g. Ubuntu): `sudo apt-get install default-jre`

Note: If you are using other distros, then please install a 'Java Runtime Environment' suitable to your distro.
Note: If you are using macOS and get a "Failed to create the Java Virtual Machine" error, uninstall any other Java JDK installs and install Java JDK 8u. This should fix the problem.

You can download Eclipse's CDT from: [https://www.eclipse.org/cdt/downloads.php](https://www.eclipse.org/cdt/downloads.php). Start the installer and choose *Eclipse CDT* from the list.

### Install SDL 2

The PC simulator uses the [SDL 2](https://www.libsdl.org/download-2.0.php) cross-platform library to simulate a TFT display and a touchpad. 

#### Linux
On **Linux** you can easily install SDL2 using a terminal:

1. Find the current version of SDL2: `apt-cache search libsdl2 (e.g. libsdl2-2.0-0)`
2. Install SDL2: `sudo apt-get install libsdl2-2.0-0` (replace with the found version)
3. Install SDL2 development package: `sudo apt-get install libsdl2-dev`
4. If build essentials are not installed yet: `sudo apt-get install build-essential`

#### Windows
If you are using **Windows** firstly you need to install MinGW ([64 bit version](http://mingw-w64.org/doku.php/download)). After installing MinGW, do the following steps to add SDL2:

1. Download the development libraries of SDL.   
Go to [https://www.libsdl.org/download-2.0.php](https://www.libsdl.org/download-2.0.php) and download _Development Libraries: SDL2-devel-2.0.5-mingw.tar.gz_
2. Decompress the file and go to _x86_64-w64-mingw32_ directory (for 64 bit MinGW) or to _i686-w64-mingw32_ (for 32 bit MinGW)
3. Copy _..._mingw32/include/SDL2_ folder to _C:/MinGW/.../x86_64-w64-mingw32/include_
4. Copy _..._mingw32/lib/_ content to _C:/MinGW/.../x86_64-w64-mingw32/lib_
5. Copy _..._mingw32/bin/SDL2.dll_ to _{eclipse_workspace}/pc_simulator/Debug/_.  Do it later when Eclipse is installed. 

Note: If you are using **Microsoft Visual Studio** instead of Eclipse then you don't have to install MinGW. 

#### OSX
On **OSX** you can easily install SDL2 with brew: `brew install sdl2`

If something is not working, then please refer [this tutorial](http://lazyfoo.net/tutorials/SDL/01_hello_SDL/index.php) to get started with SDL.

### Pre-configured project

A pre-configured graphics library project (based on the latest release) is always available to get started easily. 
You can find the latest one on [GitHub](https://github.com/lvgl/lv_sim_eclipse_sdl).
(Please note that, the project is configured for Eclipse CDT). 

### Add the pre-configured project to Eclipse CDT

Run Eclipse CDT. It will show a dialogue about the **workspace path**. Before accepting the path, check that path and copy (and unzip) the downloaded pre-configured project there. After that, you can accept the workspace path. Of course you can modify this path but in that case copy the project to the corresponding location.

Close the start-up window and go to **File-&gt;Import** and choose **General-&gt;Existing project into Workspace**. **Browse the root directory** of the project and click **Finish**

On **Windows** you have to do two additional things:

- Copy the **SDL2.dll** into the project's Debug folder 
- Right-click on the project -&gt; Project properties -&gt; C/C++ Build -&gt; Settings -&gt; Libraries -&gt; Add ... and add _mingw32_ above SDLmain and SDL. (The order is important: mingw32, SDLmain, SDL)

### Compile and Run

Now you are ready to run LVGL on your PC. Click on the Hammer Icon on the top menu bar to Build the project. If you have done everything right, then you will not get any errors. Note that on some systems additional steps might be required to "see" SDL 2 from Eclipse but in most cases the configuration in the downloaded project is enough.

After a successful build, click on the Play button on the top menu bar to run the project. Now a window should appear in the middle of your screen.

Now you are ready to use LVGL and begin development on your PC.
