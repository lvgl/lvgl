# Tests for LVGL

## Test types available

- **Unit Tests**: Standard functional tests in `src/test_cases/` with screenshot comparison capabilities
- **Performance Benchmarks**: The benchmark demo and render scenes measured by instruction count under QEMU in `perf_qemu/` to prevent performance regressions

All of the tests are automatically ran in LVGL's CI.

## Quick start

- **Local Testing**: Run `./tests/main.py test` (after `scripts/install-prerequisites.sh`)
- **Docker Testing**: Build with `docker build . -f tests/Dockerfile -t lvgl_test_env` then run
- **Performance Benchmarks**: Use `./tests/perf_qemu/run.sh` (requires Docker)

---

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

There is a script which automates these steps: `scripts/run_tests_docker.sh`. It will build a Docker container and run tests in that. Run the script with `--help` for more detail.

## Running automatically

GitHub's CI automatically runs these tests on pushes and pull requests to `master` and `release/v8.*` branches.

## Directory structure
- `src` Source files of the tests
    - `test_cases` The written tests,
        - `public_api` Tests that call a public function with an argument it is
          documented to reject, see below,
    - `test_runners` Generated automatically from the files in `test_cases`.
    - other miscellaneous files and folders
- `configs` Kconfig defconfig fragments the build configurations are made of
- `ref_imgs` - Reference images for screenshot compare
- `report` - Coverage report. Generated if the `report` flag was passed to `./main.py`
- `unity` Source files of the test engine

## Build configurations

Every build configuration is a list of defconfig fragments from `configs`,
merged in order so that a later fragment overrides an earlier one. The recipes
live in `build_only_options` and `test_options` in [`main.py`](./main.py), which
passes them to CMake through `LV_BUILD_DEFCONFIG_PATH`. `CMakeLists.txt` derives
the rest - warning flags, which reference images to compare against, which
libraries LVGL needs - from the resulting configuration, so a new configuration
usually only takes a new fragment plus an entry in `main.py`.

```sh
cmake -B build -DLV_BUILD_TESTS=ON -DLVGL_TEST_ENABLE=ON \
      -DLV_BUILD_DEFCONFIG_PATH="tests/configs/common.defconfig;tests/configs/full.defconfig;..."
```

Prefer `main.py` as it picks the right fragments for the host and architecture but this is useful when debugging the build itself.

The test case folders are listed one by one in `CMakeLists.txt`, so that a folder can be tied to a configuration option:

```cmake
lvgl_add_test_folder(test_cases/widgets)
lvgl_add_test_folder_if(NOT CONFIG_LV_CHECK_ARG_ASSERT_ON_FAIL test_cases/public_api)
```

See [`lvgl_test_helpers.cmake`](./lvgl_test_helpers.cmake).

### Public API tests

LVGL test run with `LV_CHECK_ARG_ASSERT_ON_FAIL` to ensure LVGL internals respect
its public API. The test cases that check that a public function behaves correctly when
invalid parameters are passed exist in `src/test_cases/public_api`. These tests are only
exercised when `LV_CHECK_ARG_ASSERT_ON_FAIL` is disabled.

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

## Performance benchmarks

`perf_qemu/` builds LVGL for several emulated architectures, runs the benchmark demo and the
render scenes under QEMU and counts the executed instructions, so the numbers are
deterministic. CI runs it on every pull request and compares the results against `master`.

To run it locally (requires Docker):

```sh
./perf_qemu/run.sh
```

See the comment at the top of [`perf_qemu/run.sh`](./perf_qemu/run.sh) for the available options.
