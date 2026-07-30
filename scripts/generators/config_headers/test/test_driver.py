"""Smoke tests for the whole-file driver (emit.py).

Asserts that each config type lands in the right target of the assembled
files - not exact layout (that's the per-type tests' job)."""


def test_template_has_scalars_and_enum_macros(generated):
    t = generated["template"]
    assert "#define LV_USE_WAYLAND 0" in t
    assert "#define LV_DEF_REFR_PERIOD 33" in t
    assert "#define LV_USE_STDLIB_MALLOC LV_STDLIB_BUILTIN" in t
    assert "#define LV_USE_OS LV_OS_NONE" in t
    assert "#define LV_COLOR_DEPTH 16" in t


def test_template_excludes_derived_flag(generated):
    # Internal capability flags are never offered to the user as their own
    # option (they may still be named in a "Select:" comment of what selects
    # them, which is fine - that is documentation, not a #define).
    assert "#define LV_DRAW_HAS_VECTOR_SUPPORT" not in generated["template"]


def test_internal_has_config_options_block(generated):
    i = generated["internal"]
    # Enum tokens are defined once, near the top: ConstToken constants and the
    # inline MEMBER_IS_TOKEN choice tokens.
    assert "#define LV_STDLIB_BUILTIN" in i
    assert "#define LV_OS_NONE" in i
    # LV_LOG_LEVEL_* are member == token values defined into the header (no C
    # header owns them), like LV_OS_*.
    assert "#define LV_LOG_LEVEL_WARN" in i


def test_internal_defers_derived_flag_to_footer(generated):
    i = generated["internal"]
    assert "#if (LV_USE_DRAW_VG_LITE || LV_USE_DRAW_NANOVG)" in i
    # ... and it comes after the body's option ladders.
    assert i.index("LV_DRAW_HAS_VECTOR_SUPPORT") > i.index("LV_USE_WAYLAND")


def test_internal_wraps_options_in_kconfig_ladder(generated):
    assert "#ifdef CONFIG_LV_USE_WAYLAND" in generated["internal"]


def test_internal_footer_has_static_derivations(generated):
    i = generated["internal"]
    # The *_EXTERNAL compatibility shims, the inconsistent-name alias, and the
    # trailing LV_KCONFIG_PRESENT cleanup.
    assert "LV_USE_THORVG_EXTERNAL" in i
    assert "LV_USE_LZ4_EXTERNAL" in i
    assert "#define LV_USE_ANIMIMAGE LV_USE_ANIMIMG" in i
    assert "#undef LV_KCONFIG_PRESENT" in i


def test_internal_has_legacy_autobackend_shim(generated):
    i = generated["internal"]
    assert "#if LV_USE_LINUX_DRM && LV_LINUX_DRM_AUTO_BACKEND" in i
    drm_auto = i[i.index("#ifndef LV_LINUX_DRM_AUTO_BACKEND") :]
    assert "#define LV_LINUX_DRM_AUTO_BACKEND LV_USE_LINUX_DRM" in drm_auto[:400]
