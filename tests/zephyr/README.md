# Zephyr build check

LVGL is a [Zephyr](https://zephyrproject.org) module. Zephyr ships its own LVGL
samples and glue code (`zephyr/modules/lvgl/`), pinned to an older LVGL than this
repository's `master`.

`.github/workflows/zephyr.yml` builds and runs those samples against the LVGL
code in the current commit, on every pull request and every push to `master`, so
a change that breaks the Zephyr integration is caught here rather than at
Zephyr's next LVGL uprev.

## What the check does

1. Sets up a west workspace from [`west.yml`](west.yml), which pins the Zephyr
   version.
2. Points Zephyr at this LVGL checkout via `ZEPHYR_EXTRA_MODULES`. LVGL is
   blocklisted from Zephyr's own manifest, so the code under test is the only
   `lvgl` module in the workspace.
3. Reconciles Zephyr's LVGL glue with this LVGL — see [Bridging the gap to
   Zephyr](#bridging-the-gap-to-zephyr).
4. Builds every LVGL sample for `native_sim/native/64` with twister, which
   discovers them from their own `sample.yaml` and applies each one's
   `extra_configs` (this is how the individual demos get built).
5. Runs each built sample headless for 20 seconds and checks it did not crash.
6. Reports the result into the PR comment.

`native_sim` builds with the host compiler, so no Zephyr SDK is needed. It is
also the only Zephyr target that can *run* the samples in CI.

### How "did it run" is decided

The samples are GUI applications with no end condition, so a clean run means it
kept running. Each is started with a 20 second timer, then stopped with `SIGINT`:

| Exit code | Meaning |
|---|---|
| 124 | the timer fired, i.e. it ran the whole window — **pass** |
| 0 | it exited on its own, which a GUI sample should not do — fail |
| 139 | segmentation fault — fail |
| other | assert, abort, or another signal — fail |

## Choosing the Zephyr version

[`west.yml`](west.yml) is the single source of truth. To move to a different
Zephyr, change `url-base` and/or `revision` there and nothing else.

The version is pinned deliberately: tracking a moving branch would let an
unrelated Zephyr change turn every LVGL PR red. The workflow warns, but does not
fail, when a newer release exists:

```sh
python3 scripts/zephyr/manifest.py check-version
```

Releases are compared by version number, not publication date — Zephyr backports
to older branches, so v4.4.1 can be published after v4.3.1 while still being
newer. If the pin is a branch or SHA, the check compares the pinned commit's date
against the latest release's date instead.

`workflow_dispatch` takes `zephyr_repo` and `zephyr_revision` inputs to try
another Zephyr for a single run.

## Bridging the gap to Zephyr

Zephyr compiles LVGL from a hand-maintained source list and expects Kconfig
symbols that LVGL has since renamed or dropped. Two mechanisms close that gap;
prefer the first.

### 1. A CMake hook

[`zephyr-with-custom-lvgl.cmake`](zephyr-with-custom-lvgl.cmake) is passed to
every build as `CMAKE_PROJECT_INCLUDE`. On the pass where Zephyr's
`modules__lvgl` target exists it:

- **reconciles the source list** — any path Zephyr lists that this LVGL no longer
  has is looked up by file name and replaced with its new location;
- **supplies the config values** Zephyr's glue needs but LVGL no longer defines,
  as `-D` defines (LVGL resolves its options with `#ifndef`, so these win).

The reconciliation is derived at configure time, so an LVGL change that moves a
file needs no edit here. What it reconciled is printed during the build and
reported in the PR comment, so the drift stays visible.

The hook fails the build rather than guessing when:

- a source disappeared and no file of that name exists anywhere under `src/`. If
  LVGL removed it deliberately, add it to `LVGL_DELETED_SOURCES` with a reason;
- a file name became ambiguous, so the new path cannot be inferred.

Borrowed from [`lvgl/lv_zephyr`](https://github.com/lvgl/lv_zephyr), which uses
the same hook to build against custom LVGL revisions.

### 2. A patch

[`patches/`](patches) holds what cannot be fixed from CMake. There is currently
one: Zephyr's `import_kconfig()` mishandles `[` and `]` in Kconfig string values,
and it runs inside `find_package(Zephyr)` — before any `project()` call, so no
`CMAKE_PROJECT_INCLUDE` hook exists yet. Each patch header explains what broke
and what the upstream fix should be.

A patch directory rather than a Zephyr fork: the patches are reviewable here,
there is no second repository to keep in sync, and the files under `patches/` are
the list of things we still owe upstream. When a fix lands in Zephyr, delete its
patch.

**A patch that no longer applies fails the check** — skipping it would report a
result for a tree other than the one we think we are testing. Either delete it
(it landed upstream) or rebase it.

### What we owe Zephyr upstream

| What | Handled by | Upstream fix |
|---|---|---|
| `modules/lvgl/CMakeLists.txt` source list is stale | hook | track LVGL's layout, ideally by using LVGL's own CMake |
| `LV_ATTRIBUTE_MEM_ALIGN_SIZE` forward-declared but never defined | hook | use LVGL's `LV_ATTRIBUTE_CUSTOM_INCLUDE` |
| `lv_conf.h` uses the deprecated `LV_ASSERT_HANDLER_INCLUDE` | hook | migrate to `LV_ASSERT_USE_CUSTOM_INCLUDE` |
| `import_kconfig()` drops symbols after a `[`/`]` | patch | fix the bracket handling in `extensions.cmake` |

## How the result reaches the PR

The job does not comment itself — a `pull_request` run from a fork has a
read-only token. It follows the report convention of
`.github/workflows/pr_comment.yml`:

1. `scripts/zephyr/samples.py report` writes the report through
   `scripts/pr_report/pr_report.py`, so the format stays defined in one place.
   Always go through `write_report()`: it translates the icon name to the emoji
   that the renderer prints verbatim.
2. The workflow uploads it as an artifact named `pr-report-zephyr`. The collector
   picks up any artifact whose name matches `pr-report`.
3. `pr_comment.yml` renders every matching artifact for the commit into one
   table, in a single comment.

The details block carries the per-sample table, the reconciled source paths, and
reproduction commands for anything that failed. It is also written to the run's
job summary.

The job's pass/fail comes from `zephyr-verdict.json`, not from the report
artifact, so gating never depends on the comment machinery.

To make the row show as pending when the check has not reported yet, add
`("Zephyr", 40)` to `EXPECTED_SECTIONS` in `pr_report.py`, and `Zephyr` to
`pr_comment.yml`'s `workflow_run.workflows` list so the artifact is collected.

## Reproducing locally

You need `cmake`, `ninja`, `gperf`, `dtc`, `libsdl2-dev`, a host C compiler, and
`west` (`pip install west`).

```sh
mkdir zephyr-workspace && cd zephyr-workspace
git clone https://github.com/lvgl/lvgl

# west init -l makes the manifest directory's *parent* the workspace top
# directory, so the manifest needs its own directory here.
mkdir manifest && cp lvgl/tests/zephyr/west.yml manifest/
west init -l manifest
west config manifest.group-filter -- -optional
west update --narrow -o=--depth=1

pip install -r zephyr/scripts/requirements-base.txt \
            -r zephyr/scripts/requirements-build-test.txt \
            -r zephyr/scripts/requirements-run-test.txt

for p in lvgl/tests/zephyr/patches/*.patch; do git -C zephyr apply "$p"; done

export ZEPHYR_EXTRA_MODULES=$PWD/lvgl
export ZEPHYR_TOOLCHAIN_VARIANT=host
# must be absolute: CMake includes it from each sample's build directory
export CUSTOM_LVGL_CMAKE=$PWD/lvgl/tests/zephyr/zephyr-with-custom-lvgl.cmake
```

Build and run everything the way CI does:

```sh
west twister -p native_sim/native/64 \
  -T zephyr/samples/subsys/display/lvgl -T zephyr/samples/modules/lvgl \
  --fixture fixture_display --build-only \
  -x "CMAKE_PROJECT_INCLUDE=$CUSTOM_LVGL_CMAKE" --outdir twister-out

python3 lvgl/scripts/zephyr/samples.py run \
  --outdir twister-out --results run-results.json
```

Or work on a single sample:

```sh
west build -p always -b native_sim/native/64 \
  zephyr/samples/modules/lvgl/demos -- -DCONFIG_LV_Z_DEMO_WIDGETS=y \
  -DCMAKE_PROJECT_INCLUDE=$CUSTOM_LVGL_CMAKE

cd build/zephyr
SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout -s INT 20 ./zephyr.exe
```

`SDL_VIDEODRIVER=dummy` alone is not enough — SDL then finds no renderer. With
`SDL_RENDER_DRIVER=software` too, no X server or `xvfb` is needed. Drop both to
watch a sample in a real window.
