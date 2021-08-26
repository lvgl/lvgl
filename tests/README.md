# Tests for LVGL

The tests in the folder can be run locally and automatically by GitHub CI.

## Running locally

### Requirements (Linux)
1. Be sure GCC Python3 is installed.
2. Install [cmake](https://cmake.org/) with `sudo apt install cmake`.
3. Install [gcovr](https://gcovr.com/en/stable/index.html) with `sudo apt install gcovr`
4. Install Ruby with `sudo apt install ruby-full`
5. Install libpng development package with `sudo apt install libpng-dev`

### Run test
1. Enter `lvgl/tests/`
2. Run the tests with `./main.py [OPTIONS]`. The options are
  - `report` Create a html page in the `report` folder with the coverage report.
  - `test` Build and run only test. Without this option LVGL will be built with various configurations.
  - `noclean` Do not clean the project before building. Useful while writing test to save some times. 

For example: 
- `./main.py` Run all the test as they run in the CI.
- `./main.py report test noclean` Run only the test, should be used when writing tests.


## Running automatically

GitHub's CI automatically runs these tests on pushes and pull requests to `master` and `releasev8.*` branches. 

## Directory structure
- `src` Source files of the tests
    - `test_cases` The written tests,
    - `test_runners` Generated automatically from the files in `test_cases`.
    - other miscellaneous files and folders 
- `ref_imgs` - Reference images for screenshot compare
- `report` - Coverage report. Generated if the `report` flag was passed to `./main.py` 
- `unity` Source files of the test engine

## Add new tests

### Create new test file
New test needs to be added into the `src/test_cases` folder. The name of the files should look like `test_<name>.c`. The the basic skeleton of a test file copy `_test_template.c`.

### Asserts
See the list of asserts [here](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md).

There are some custom, LVGL specific asserts:
- `TEST_ASSERT_EQUAL_SCREENSHOT("image1.png")` Render the active screen and compare its content with an image in the `ref_imgs` folder. 
If the compare fails `lvgl/test_screenshot_error.h` is created with the content of the frame buffer as an image. 
To see the that image `#include "test_screenshot_error.h"` and call `test_screenshot_error_show();`.
- `TEST_ASSERT_EQUAL_COLOR(color1, color2)` Compare two colors.

### Adding new reference images
The reference images can be taken by copy-pasting the test code in to LVGL simulator and saving the screen. 
LVGL needs to
- 800x480 resolution
- 32 bit color depth
- `LV_USE_PERF_MONITOR` and `LV_USE_MEM_MONITOR` disabled
- use the default theme, with the default color (don't set a theme manually)
