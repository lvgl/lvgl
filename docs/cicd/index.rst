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
#. Patches SO3 so it executes the *stress* application instead of the shell
#. Builds U-boot and the SO3 kernel
#. Imports the stress project into SO3's userspace and builds the userspace
#. Sets up the image so it exposes the port 1234 when ran and executes "./st"
