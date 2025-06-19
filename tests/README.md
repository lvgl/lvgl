# Tests for LVGL

## Test types available

- **Unit Tests**: Standard functional tests in `src/test_cases/` with screenshot comparison capabilities
- **Performance Tests**: ARM-emulated benchmarks in `src/test_cases_perf/` running on QEMU/SO3 environment
- **Emulated Benchmarks**: Automated `lv_demo_benchmark` runs in ARM emulation to prevent performance regressions

All of the tests are automatically ran in LVGL's CI.

## Quick start

- **Local Testing**: Run `./tests/main.py test` (after `scripts/install-prerequisites.sh`)
- **Docker Testing**: Build with `docker build . -f tests/Dockerfile -t lvgl_test_env` then run
- **Performance Testing**: Use `./tests/perf.py test` (requires Docker + Linux)
- **Benchmark Testing**: Use `./tests/benchmark_emu.py run` for emulated performance benchmarks (requires Docker + Linux)

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

## Running automatically

GitHub's CI automatically runs these tests on pushes and pull requests to `master` and `release/v8.*` branches.

## Directory structure
- `src` Source files of the tests
    - `test_cases` The written tests,
    - `test_cases_perf` The performance tests,
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

## Performance Tests

### Requirements

- **Docker**
- **Linux host machine** (WSL *may* work but is untested)

### Running Tests

The performance tests are run inside a Docker container that launches an ARM emulated environment using QEMU to ensure consistent timing across machines.
Each test runs on a lightweight ARM-based OS ([SO3](https://github.com/smartobjectoriented/so3)) within this emulated environment.

To run the tests:

```bash
./perf.py [--clean] [--auto-clean] [--test-suite <suite>] [--build-options <option>] [build|generate|test]
```

- `build` and `generate`: generates all necessary build and configuration files
- `test`: launches Docker with the appropriate volume mounts and runs the tests inside the container


> [!NOTE]
> Building doesn't actually build the source files because the current docker image doesn't separate the building and running. Instead, it does both

You can specify different build configurations via `--build-options`, and optionally filter tests using `--test-suite`.

For full usage options, run:

```sh
./perf.py --help
```

You can also run this script by passing a performance test config to the `main.py` script. The performance tests configs can be found inside the [`perf.py`](./perf.py) file

## Emulated benchmarks

In addition to unit and performance tests, LVGL automatically runs the `lv_demo_benchmark` inside the same ARM emulated
environment mentionned in the previous section through CI to prevent unintentional slowdowns.

### Requirements

- **Docker**
- **Linux host machine** (WSL *may* work but is untested)

To run the these benchmarks in the emulated setup described above, you can use the provided python script:

```sh
./benchmark_emu.py [-h] [--config {perf32b,perf64b}] [--pull] [--clean] [--auto-clean]
                        [{generate,run} ...]
```

The following command runs all available configurations:

```sh
./benchmark_emu.py run 
```

You can also request a specific configuration:

```sh
./benchmark_emu.py --config perf32b run 
```
