.. _lvgl_xml_license:

=======
License
=======

**Version 1.0 – June 2025**

**Copyright (c) 2025 LVGL LLC**


1. Overview
===========

The LVGL XML Specification (the "Specification") defines an XML-based language for describing user interfaces within the LVGL ecosystem.

This Specification is protected by copyright. It is provided with limited rights of use as outlined below.

The purpose of this license is to ensure that the Specification remains freely usable for application developers working with LVGL,
while restricting its use in third-party UI editors or design tools.
This protects the long-term integrity and sustainability of the LVGL ecosystem by preventing unlicensed tools
from reusing or replicating the Specification in ways that compete with official LVGL offerings.


2. Permitted Usage
==================

You are permitted to:

- Use this Specification in **any firmware or embedded application** built on top of the official `LVGL library <https://github.com/lvgl/lvgl>`_ (MIT-licensed).
- Use the LVGL XML loader freely in accordance with its MIT license.
- Use the Specification with the **LVGL UI Editor** and **official LVGL tools such as the CLI or XML loader**.
- Write, load, edit, or generate user interfaces described in the LVGL XML Specification for **internal use** only, including automation or configuration tools used solely inside your organization.
- Create **internal scripts or plugins** that help your own developers use or manage this Specification, as long as these are **not shared outside your organization or made publicly available**.
- Share XML-based UI files within customer firmware or projects that target LVGL-based embedded systems.


3. Restrictions
===============

You are **not permitted** to:

- Create, release, or distribute any **UI editor, visual builder, layout designer, code generator, or tool** that reads, writes, or interprets XML files using this Specification — **whether commercial, open-source, or intended for public use, use by customers or partners, or any use outside your own organization** — without written permission from LVGL LLC.
- Implement or extend this Specification in any **software or platform that provides UI creation, editing, or design capabilities similar to those in the LVGL UI Editor**.
- Build public APIs, plugins, converters, or SDKs based on this Specification.
- Share or publish internal tooling built around this Specification.
- Use this Specification into general-purpose design tools or frameworks outside the LVGL ecosystem.

These restrictions apply regardless of whether the software is commercial, free, or open-source.

4. Clarification for LVGL Users
===============================

You are **explicitly permitted** to:

- Use XML files with this Specification in your own projects.
- Load XMLs at runtime using LVGL’s XML loader.
- Generate XMLs using the LVGL UI Editor.
- Customize XMLs manually or with internal utilities.
- Share LVGL-based projects with XML files as part of a commercial product or firmware.

If you are building embedded software using LVGL, **this license does not affect you**.
This license only applies to **UI development tools** that **read, generate, or process XML files using this Specification for UI development purposes** in a public or commercial manner.

You may create **internal tools** to aid development using the Specification, but these must not be published, sold, or shared externally without a license.


5. Trademarks
=============

This license does not grant any rights to use the LVGL name, logo, or trademarks.


6. Licensing and Permissions
============================

To request permission for broader use or commercial licensing (e.g. using this Specification in your own UI tool), contact:

**lvgl@lvgl.io**

All rights not expressly granted herein are reserved by LVGL LLC.

