.. _repository:

===================
The LVGL Repository
===================


Repository layout
*****************

All repositories of the LVGL project are hosted on `GitHub <https://github.com/lvgl>`_.

You will find these repositories at the following locations:

* `lvgl <https://github.com/lvgl/lvgl>`__: The library itself with many `examples <https://github.com/lvgl/lvgl/blob/master/examples/>`_ and `demos <https://github.com/lvgl/lvgl/blob/master/demos/>`__.
* `blog <https://github.com/lvgl/blog>`__: Source of the `blog's site <https://blog.lvgl.io>`__
* `sim <https://github.com/lvgl/sim>`__: Source of the `online simulator's site <https://sim.lvgl.io>`__
* `lv_port_* <https://github.com/lvgl?q=lv_port&type=&language=>`__: LVGL ports to development boards and other platforms
* `lv_binding_* <https://github.com/lvgl?q=lv_binding&type=&language=l>`__: Bindings to other languages


Release policy
**************


The core repositories follow the rules of `Semantic Versioning <https://semver.org/>`__:

* Major version: incompatible API changes. E.g. v5.0.0, v6.0.0
* Minor version: new but backward-compatible functionalities. E.g. v6.1.0, v6.2.0
* Patch version: backward-compatible bug fixes. E.g. v6.1.1, v6.1.2

Tags like `vX.Y.Z` are created for every release.


Release cycle
*************

* Bug fixes: released on demand even weekly
* Minor releases: every 3-4 months
* Major releases: approximately yearly


Branches
********


The core repositories have at least the following branches:

* `master`: latest version, patches are merged directly here
* `release/vX.Y`: stable versions of the minor releases


Change log
**********

The changes are recorded in :ref:`changelog`.


Version support
***************


Before v8 the last minor release of each major series was supported for 1 year.
Starting from v8, every minor release is supported for 1 year.


+---------+--------------+--------------+--------+
| Version | Release date | Support end  | Active |
+=========+==============+==============+========+
|v5.3     | 1 Feb, 2019  | 1 Feb, 2020  | No     |
+---------+--------------+--------------+--------+
|v6.1     | 26 Nov, 2019 | 26 Nov, 2020 | No     |
+---------+--------------+--------------+--------+
|v7.11    | 16 Mar, 2021 | 16 Mar, 2022 | No     |
+---------+--------------+--------------+--------+
|v8.0     | 1 Jun, 2021  | 1 Jun, 2022  | No     |
+---------+--------------+--------------+--------+
|v8.1     | 10 Nov, 2021 | 10 Nov, 2022 | No     |
+---------+--------------+--------------+--------+
|v8.2     | 31 Jan, 2022 | 31 Jan, 2023 | No     |
+---------+--------------+--------------+--------+
|v8.3     | 6 July, 2022 | 1 Jan, 2025  | No     |
+---------+--------------+--------------+--------+
|v8.4     | 19 Mar, 2024 | 20 Mar, 2025 | No     |
+---------+--------------+--------------+--------+
|v9.0     | 22 Jan, 2024 | 22 Jan, 2025 | No     |
+---------+--------------+--------------+--------+
|v9.1     | 20 Mar, 2024 | 20 Mar, 2025 | No     |
+---------+--------------+--------------+--------+
|v9.2     | 26 Aug, 2024 | 26 Aug, 2025 | Yes    |
+---------+--------------+--------------+--------+
|v9.3     | 3 June, 2025 | 3 June, 2026 | Yes    |
+---------+--------------+--------------+--------+

