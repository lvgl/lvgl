.. _vram:

========================
VRAM Resource Management
========================

Overview
********

Some display controllers and GPUs have their own dedicated Video RAM (VRAM)
that is separate from the CPU's system memory.  Examples include the
BT820/EVE5 series and other display controllers with on-chip graphics memory.

When :c:macro:`LV_USE_DRAW_VRAM` is enabled in ``lv_conf.h``, LVGL provides a
framework that lets :ref:`Draw Units <draw units>` manage image, font, and
layer resources directly in VRAM.  This eliminates unnecessary system-memory
copies and avoids redundant RAM-to-VRAM transfers, reducing both memory
consumption and CPU overhead.

All VRAM-related functionality is behind the :c:macro:`LV_USE_DRAW_VRAM`
configuration flag.  When the flag is disabled, the VRAM API functions become
no-ops so they can be safely called from draw-unit code without conditional
compilation.



Key Concepts
************


Residency
---------

A draw buffer (:cpp:type:`lv_draw_buf_t`) can be *resident* in CPU memory,
in VRAM, or both.  "Ensuring residency" means guaranteeing that the buffer's
pixel data is available in the memory space required by the draw unit that is
about to use it.

:cpp:func:`lv_draw_buf_ensure_resident` is the central function for this.
When called with a draw unit, it:

- Uploads CPU data to VRAM if the draw unit needs VRAM-resident data.
- Downloads VRAM data to CPU memory if the software renderer needs it.
- Allocates backing memory lazily if the buffer was header-only (no data
  allocated yet).
- Handles the :cpp:enumerator:`LV_IMAGE_FLAGS_DISCARDABLE` and
  :cpp:enumerator:`LV_IMAGE_FLAGS_CLEARZERO` flags to skip unnecessary
  transfers.

After a successful upload to VRAM, the CPU-side allocation is freed to
minimize RAM usage.  Conversely, after a download to CPU memory, the VRAM
allocation may be released.  The rationale is that minimizing RAM usage is
a more relevant use case than optimizing for mixed draw-unit performance.


VRAM Resource Descriptor
------------------------

Each draw buffer (and image/font descriptor) can carry a pointer to a
:cpp:type:`lv_draw_buf_vram_res_t` struct in its ``vram_res`` field.  This
base struct records:

- which :ref:`Draw Unit <draw units>` owns the VRAM allocation, and
- the size of the allocation in bytes.

Draw units extend this base struct with their own fields (e.g. GPU handle,
hardware pixel format, stride, palette offset).


Buffer Flags
------------

Two buffer flags help avoid unnecessary memory operations:

- :cpp:enumerator:`LV_IMAGE_FLAGS_DISCARDABLE`:  The buffer's content is
  stale and may be discarded.  On the next residency transfer, skip
  upload/download and just allocate fresh backing.  This is typically set by
  the display driver after a flush when tile data has been consumed.

- :cpp:enumerator:`LV_IMAGE_FLAGS_CLEARZERO`:  Implies
  :cpp:enumerator:`LV_IMAGE_FLAGS_DISCARDABLE` and additionally requires that
  the memory be zeroed before use.  For CPU buffers,
  :cpp:func:`lv_draw_buf_ensure_resident` clears the buffer after allocation.
  For VRAM buffers, the draw unit handles clearing (e.g. by marking the
  allocation as having no content).  This is set by
  :cpp:func:`lv_draw_buf_clear` and transparent-background fills on
  non-CPU-resident buffers, and supports use cases like canvas layers where
  the common pattern is to allocate a buffer and immediately clear it.



.. _vram_configuration:

Configuration
*************

Enable VRAM support in ``lv_conf.h``:

.. code-block:: c

    #define LV_USE_DRAW_VRAM 1


Image Descriptor Constness
--------------------------

Image descriptors generated as C arrays are normally ``const``.  When
:c:macro:`LV_USE_DRAW_VRAM` is enabled, the ``vram_res`` field must be
writable so draw units can attach VRAM residency information.

The macro :c:macro:`LV_IMAGE_DSC_CONST` is provided for this purpose.  It
expands to ``const`` when VRAM support is disabled and to nothing when it is
enabled.  Use it instead of plain ``const`` when declaring
:cpp:type:`lv_image_dsc_t` variables:

.. code-block:: c

    LV_IMAGE_DSC_CONST lv_image_dsc_t my_image = { ... };

The LVGL offline image converter should be configured to emit
:c:macro:`LV_IMAGE_DSC_CONST` in generated assets.


Font Descriptor Constness
-------------------------

Fonts have a similar requirement.  The font descriptor struct (e.g.
``lv_font_fmt_txt_dsc_t``) pointed to by ``lv_font_t.dsc`` is normally
``const``.  When :c:macro:`LV_USE_DRAW_VRAM` is enabled, the descriptor must
be writable so that the draw unit can attach a ``vram_res`` pointer to it.

The macro :c:macro:`LV_FONT_DSC_CONST` works identically to
:c:macro:`LV_IMAGE_DSC_CONST` but for font descriptors.  It expands to
``const`` when VRAM support is disabled and to nothing when it is enabled.
Use it instead of plain ``const`` when declaring font descriptor variables:

.. code-block:: c

    LV_FONT_DSC_CONST lv_font_fmt_txt_dsc_t my_font_dsc = { ... };

Note that the :cpp:type:`lv_font_t` struct itself remains ``const``.  Only
the descriptor it points to (via the ``dsc`` field) needs to be writable.

Every font descriptor type must embed :cpp:type:`lv_font_dsc_base_t` as its
**first member**.  This base struct contains a single ``vram_res`` pointer
that draw units use generically to track font VRAM residency:

.. code-block:: c

    typedef struct {
        lv_draw_buf_vram_res_t * vram_res;  /**< GPU residency, NULL if CPU-only */
    } lv_font_dsc_base_t;

    /* Example: custom font descriptor */
    typedef struct {
        lv_font_dsc_base_t base;   /* MUST be first */
        /* ... your font-specific fields ... */
    } my_font_dsc_t;

The LVGL offline font converter (``lv_font_conv``) should be configured to
emit :c:macro:`LV_FONT_DSC_CONST` in generated font assets.



.. _vram_usage:

Usage
*****


For Draw-Unit Authors
---------------------

If you are implementing a draw unit with VRAM, you need to:

1. **Register VRAM callbacks** during draw-unit initialization by setting
   the callback function pointers on your :cpp:type:`lv_draw_unit_t`:

   .. code-block:: c

       u->base_unit.vram_alloc_cb    = my_vram_alloc;
       u->base_unit.vram_free_cb     = my_vram_free;
       u->base_unit.vram_upload_cb   = my_vram_upload;
       u->base_unit.vram_download_cb = my_vram_download;
       u->base_unit.vram_check_cb    = my_vram_check;
       u->base_unit.vram_font_free_cb = my_vram_font_free;

   Optional callbacks for VRAM-side buffer operations:

   .. code-block:: c

       u->base_unit.vram_dup_cb  = my_vram_dup;   /* Duplicate buffer in VRAM */
       u->base_unit.vram_copy_cb = my_vram_copy;   /* Copy region within VRAM */

2. **Call** :cpp:func:`lv_draw_buf_ensure_task_sources_resident` at the
   beginning of your dispatch callback, before processing a draw task:

   .. code-block:: c

       bool ok = lv_draw_buf_ensure_task_sources_resident(task, draw_unit);
       if(!ok) {
           /* Resource transfer failed; skip or defer this task */
           return LV_DRAW_UNIT_IDLE;
       }

   This convenience function inspects the draw task and ensures that all
   source buffers (image sources, layer child buffers, bitmap masks, arc
   image sources) are resident on the requesting draw unit.

3. **Pass the draw unit to** :cpp:func:`lv_draw_layer_alloc_buf` when
   allocating layer buffers, so layers can be allocated in VRAM when your
   draw unit will render into them:

   .. code-block:: c

       void * buf = lv_draw_layer_alloc_buf(layer, draw_unit);


For Application Developers
--------------------------

When :c:macro:`LV_USE_DRAW_VRAM` is enabled and a VRAM-capable draw unit is
registered, LVGL automatically manages resource transfers between CPU memory
and VRAM.  In most cases, no application-level changes are needed beyond:

- Enabling :c:macro:`LV_USE_DRAW_VRAM` in ``lv_conf.h``.
- Ensuring generated image assets use :c:macro:`LV_IMAGE_DSC_CONST` and font
  assets use :c:macro:`LV_FONT_DSC_CONST` instead of plain ``const`` for
  their descriptors.



VRAM Callbacks
**************

A VRAM-capable draw unit must implement the following callbacks.  They are
registered as function pointers on the :cpp:type:`lv_draw_unit_t` struct.

.. container:: tighter-table-3

    +------------------------+----------+-----------------------------------------------------+
    | Callback               | Required | Description                                         |
    +========================+==========+=====================================================+
    | ``vram_alloc_cb``      | Yes      | Allocate VRAM backing for a draw buffer.            |
    |                        |          | Set ``buf->vram_res``. Return ``true`` on success.  |
    +------------------------+----------+-----------------------------------------------------+
    | ``vram_free_cb``       | Yes      | Free VRAM backing and NULL ``buf->vram_res``.       |
    +------------------------+----------+-----------------------------------------------------+
    | ``vram_upload_cb``     | Yes      | Upload CPU pixel data to VRAM. Allocate VRAM,       |
    |                        |          | copy/convert from ``buf->data``, set                |
    |                        |          | ``buf->vram_res``. Return ``true`` on success.      |
    +------------------------+----------+-----------------------------------------------------+
    | ``vram_download_cb``   | Yes      | Download VRAM data to ``buf->data`` in CPU memory.  |
    |                        |          | Return ``true`` on success.                         |
    +------------------------+----------+-----------------------------------------------------+
    | ``vram_check_cb``      | Yes      | Check if a VRAM allocation is still valid.          |
    |                        |          | Return ``false`` if VRAM was reclaimed or lost.     |
    +------------------------+----------+-----------------------------------------------------+
    | ``vram_font_free_cb``  | No       | Free font VRAM residency. Called when a different   |
    |                        |          | draw unit encounters a font owned by this unit.     |
    +------------------------+----------+-----------------------------------------------------+
    | ``vram_dup_cb``        | No       | Duplicate a buffer entirely within VRAM.            |
    |                        |          | Falls back to CPU-side copy if NULL or fails.       |
    +------------------------+----------+-----------------------------------------------------+
    | ``vram_copy_cb``       | No       | Copy a region between buffers within VRAM.          |
    |                        |          | Falls back to CPU-side copy if NULL or fails.       |
    +------------------------+----------+-----------------------------------------------------+



Limitations
***********

- Only one VRAM draw unit can own a resource at a time.  If multiple GPUs or
  display controllers need the same image, additional design work is needed
  (e.g. per-unit indexing).

- Font and image descriptors become non-``const`` when
  :c:macro:`LV_USE_DRAW_VRAM` is enabled, which increases writable data
  usage.

- The image cache is aware of VRAM residency and will trigger transfers as
  needed, but frequent alternation between CPU and VRAM rendering for the
  same resources will incur transfer overhead.



API
***

.. API equals:
    lv_draw_buf_ensure_resident
    lv_draw_buf_ensure_task_sources_resident
    lv_draw_buf_vram_font_release
    lv_draw_layer_alloc_buf
    LV_USE_DRAW_VRAM
    LV_IMAGE_DSC_CONST
    LV_FONT_DSC_CONST
    lv_font_dsc_base_t
    lv_font_release_vram
    LV_IMAGE_FLAGS_DISCARDABLE
    LV_IMAGE_FLAGS_CLEARZERO
