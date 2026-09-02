# LVGL SBOM

This folder contains the Software Bill of Materials (SBOM) for LVGL in
[SPDX 3.0.1](https://spdx.github.io/spdx-spec/v3.0.1/) JSON-LD format, plus the
single source of truth it is generated from.

## Files

| File | Description |
|------|-------------|
| `third_party.json` | **Single source of truth** — every third-party dependency, its license, supplier and source |
| `lvgl.spdx.json` | Generated SPDX 3.0.1 SBOM |
| `requirements.txt` | Pinned tooling for validation / the CI gate |
| `README.md` | This file |

Two artifacts are **generated** from `third_party.json` — do not edit them by hand:

- `sbom/lvgl.spdx.json` (this folder)
- `COPYRIGHTS.md` (repo root)

## Regenerating

When a dependency is added, removed, or relicensed, edit
`sbom/third_party.json` and regenerate both outputs:

```sh
python3 scripts/generate_copyrights.py    # -> COPYRIGHTS.md
python3 scripts/generate_sbom.py          # -> sbom/lvgl.spdx.json
```

The LVGL version is read from `include/lvgl/lv_version.h` and recorded inside
the document, so the package version always tracks the checked-out tree. The
file name itself is unversioned — the SBOM is bundled inside a given LVGL
version anyway.

## Validating

The SBOM is validated against the **official SPDX 3.0.1 JSON Schema** (SPDX's
own recommended method) using
[check-jsonschema](https://github.com/python-jsonschema/check-jsonschema):

```sh
python3 -m pip install -r sbom/requirements.txt
python3 scripts/validate_sbom.py
```

## CI

The `Static Checks` workflow (`.github/workflows/static_checks.yml`) fails the
build if any generated file drifts from `sbom/third_party.json` or the SBOM is
not schema-valid:

- `generate_copyrights.py --check` — COPYRIGHTS.md is up to date
- `generate_sbom.py --check` — SBOM is up to date (ignores the creation timestamp)
- `validate_sbom.py` — SBOM validates against the SPDX JSON Schema

## `third_party.json` fields

Each entry in `components`:

| field         | used by            | notes |
|---------------|--------------------|-------|
| `key`         | SBOM               | slug for SPDX element ids |
| `name`        | SBOM + COPYRIGHTS  | component title |
| `paths`       | SBOM + COPYRIGHTS  | in-tree locations |
| `sources`     | SBOM + COPYRIGHTS  | `[{url, note?}]`; array because e.g. TinyTTF has two upstreams. First url is the SPDX download location |
| `note`        | SBOM + COPYRIGHTS  | free-form note |
| `version`     | SBOM               | vendored snapshot version (from an in-tree version macro/string), or `NOASSERTION` when the upstream copy carries no version |
| `license`     | SBOM               | SPDX license expression (or `LicenseRef-*`) |
| `license_file`| SBOM               | path to embed for a custom `LicenseRef-*` license |
| `copyright`   | SBOM               | upstream copyright statement (from the library's LICENSE/source header) |
| `supplier`    | SBOM               | `{name, type}` where type is `person` or `organization` |
| `purl`        | SBOM               | Package URL identifier |
| `purpose`     | SBOM               | SPDX `software_primaryPurpose` |
| `third_party` | SBOM               | `false` marks LVGL's own code — omitted from the SBOM component list (COPYRIGHTS.md renders every entry) |

## Notes on the data

- The inventory was curated from `COPYRIGHTS.md`, the `LV_USE_*` toggles in
  `lv_conf_template.h`, and the `src/libs/**/LICENSE*` files.
- **FreeType** is listed because LVGL ships its integration interface, but
  FreeType itself is not part of LVGL and must be provided by the integrator.
- In SPDX 3.0 a package has no license field; licensing is expressed as
  `Relationship` elements (`from` package → `hasDeclaredLicense` /
  `hasConcludedLicense` → `to` license expression). This is why the SBOM
  contains `Relationship` nodes.
