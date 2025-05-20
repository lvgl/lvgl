# Tests for LVGL

The tests in the folder can be run locally and automatically by GitHub CI.

## Running locally

### Local

1. Install requirements by:

```sh
scripts/install-prerequisites.sh
```

2. Run all executable tests with `./tests/main.py test`.
3. Build all build-only tests with `./tests/main.py build`.
4. Clean prior test build, build all build-only tests,
   run executable tests, and generate code coverage
   report `./tests/main.py --clean --report build test`.
5. You can re-generate the test images by adding option `--update-image`.
   It relies on scripts/LVGLImage.py, which requires pngquant and pypng.
   You can run below command firstly and follow instructions in logs to install them.
   `./tests/main.py --update-image test`
   Note that different version of pngquant may generate different images.
   As of now the generated image on CI uses pngquant 2.13.1-1.

For full information on running tests run: `./tests/main.py --help`.

### Docker

To run the tests in an environment matching the CI setup:

1. Build it

```bash
docker build . -f tests/Dockerfile -t lvgl_test_env
```

2. Run the tests

```bash
docker run --rm -it -v $(pwd):/work lvgl_test_env "./tests/main.py"
```

This ensures you are testing in a consistent environment with the same dependencies as the CI pipeline.

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
New test needs to be added into the `src/test_cases` folder. The name of the files should look like `test_<name>.c`. The basic skeleton of a test file copy `_test_template.c`.

### Asserts
See the list of asserts [here](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md).

There are some custom, LVGL specific asserts:
- `TEST_ASSERT_EQUAL_SCREENSHOT("image1.png")` Render the active screen and compare its content with an image in the `ref_imgs` folder.
   - If the reference image is not found it will be created automatically from the rendered screen.
   - If the compare fails an `<image_name>_err.png` file will be created with the rendered content next to the reference image.
- `TEST_ASSERT_EQUAL_COLOR(color1, color2)` Compare two colors.

