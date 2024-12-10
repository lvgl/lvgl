.. _changelog:

Changelog
=========

`v9.2.2 <https://github.com/kisvegabor/lvgl_upstream/compare/v9.2.2...v9.2.1>`__ 29 October 2024
------------------------------------------------------------------------------------------------

This release is a hot fix to revert `932c140 <https://github.com/kisvegabor/lvgl_upstream/commit/932c14086b79aff2a27cd154441f680eb8257311>`__
which makes Kconfig build systems fail.

**It's highly recommended to update v9.2.1 to v9.2.2.**

See the Change log of v9.2.1 `here <https://github.com/lvgl/lvgl/blob/59a6b61c9580b65089010c5273f2fcdd6c4d2aae/docs/CHANGELOG.rst>`__ .


Fixes
~~~~~
- **Revert "fix(kconfig): add LV_ATTRIBUTE_MEM_ALIGN, LV_ATTRIBUTE_LARGE_CONST and LV_SYSMON_GET_IDLE configs (#7131)"** `932c140 <https://github.com/kisvegabor/lvgl_upstream/commit/932c14086b79aff2a27cd154441f680eb8257311>`__
- **fix(nuttx): fix assert when release LCD draw buffer** `7159 <https://github.com/kisvegabor/lvgl_upstream/pull/7159>`__

