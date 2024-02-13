.. _cicd:

============
CICD
============

LVGL uses github actions to perform various operations on the code 


Main Actions
------------

- Micropython build
- C/C++ build
- Documentation build
- PlatformIO publishing
- Release
- Performance test

LVGL Performance Test
---------------------

SO3 is used to check the performance of LVGL. LVGL's repo provides a "check_perf" workflow that 

#. Retrieves LVGL 8.3 and stores it in a "lvgl_base" folder for the dockerfile to use
#. Retrieves "Dockerfile.lvgl" from the SO3 repository and builds the docker image
#. Runs the docker image
#. Creates an artifact in the form of a log file that shows the output of the perf test 

The image is ran using two volumes: One that redirects the container's "/host" folder to the workflow's working directory and one that allows the container to access the workflow's devices (in the /dev folder) as his own (NOTE: is it really necessary ?)

The workflow is setup to run when
* Commits are pushed to LVGL's repo 
* A pull request is created 
* Launched from another workflow

Dockerfile
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

LVGL's check_perf workflow uses the Dockerfile.lvgl found at the root of this repository to create the image that runs SO3 on qemu and executes the tests. The dockerfile does the following:

#. Creates an Alpine image and installs all the necessary tools (like gcc and qemu)
#. Recovers SO3's main branch in the root ("/") folder 
#. Empties the "*/so3/usr/lib/lvgl*" folder and replaces its content with the LVGL repo to be tested (The LVGL code should be in a "lvgl_base" folder)
#. (Finds and removes all files related to "thumb")
#. Patches SO3 so it executes the *stress* application instead of the shell at launch
#. Builds U-boot and the SO3 kernel
#. Imports the stress project into SO3's userspace and builds the userspace
#. Sets up the image so it exposes the port 1234 when ran and executes "./st"

Constraints and Guidelines
^^^^^^^^^^^^^^^^^^^^^^^^^^^

LVGL's check_perf workflow relies on a custom implementation of the *_mcount* function implemented in SO3's libc. This function is called at the start of each function found in a source file when this source file is built with GCC's "-p" (profiling) flag. The custom implementation modifies the stack to insert another function (*_mcount_exit*) as return function of the profiled function (the one that called *_mcount*). Both *_mcount* and *_mcount_exit* call a C function that timestamps their execution with the return address of the profiled function. This allows a custom script to analyse the executable file to find the function that was timestamped and calculate its execution time. This approach allows some code to be profiled automatically without the need to explicitely call timestamping functions from within the code. However it comes with a few constraints

* Build as few source files as possible with the "-p" flag. As explained, GCC will insert a call to *_mcount* at the beginning of EVERY function found in a file compiled with this flag
* Separate profiled functions from their dependencies. If a profiled function *func1* calls another function *func2*, then *func2* should be defined in a different file from *func1* and compiled without "-p" to prevent it from being profiled too. Timestamping relies on syscalls which can be slow compared to a classic execution so nesting profiled functions will result in less reliable results

The best way to profile some code is to create a new test application with 2 kinds of files: 
* Setup: Those files define functions that prepare the resources necessary for the execution of the profiled functions. For example, if a profiled function needs to provide a configuration structure to one of its dependencies, the structure can be initialised and configured in it and then passed to the profiling function to be given to the function that requires it. Those files are compiled normally
* Profiling: Those files contain functions whose entry and exit time will be timestamped. They use the functions that should be profiled with the parameters given to them by the Setup functions. Those files are compiled using the "-p flag"
* The main of the application should be in a Setup file but may also be in a Profiling file if one wants to calculate the overall execution time. Please note however that whatever time is reported also measures the execution time of all the timestamping functions calls

Known Limitations
^^^^^^^^^^^^^^^^^^

* The current _mcount implementation is done in aarch64 assembly and is thus only compatible with ARM64 platforms
* The current _mcount implementation breaks the program if it makes use of a function with more than 7 parameters. Functions with 8 or more parameters are given some of their values through the stack in a way that is not possible to detect with our current implementation. This results in the values being shifted and replaced by the stack of _mcount_exit
