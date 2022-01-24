```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/others/snapshot.md
```
# Snapshot

Snapshot provides APIs to take snapshot image for LVGL object together with its children. The image will look exactly like the object.

## Usage

Simply call API `lv_snapshot_take` to generate the image descriptor which can be set as image object src using `lv_img_set_src`.


Note, only below color formats are supported for now:
 - LV_IMG_CF_TRUE_COLOR_ALPHA
 - LV_IMG_CF_ALPHA_1BIT
 - LV_IMG_CF_ALPHA_2BIT
 - LV_IMG_CF_ALPHA_4BIT
 - LV_IMG_CF_ALPHA_8BIT


### Free the Image
The memory `lv_snapshot_take` uses are dynamically allocated using `lv_mem_alloc`. Use API `lv_snapshot_free` to free the memory it takes. This will firstly free memory the image data takes, then the image descriptor.


Take caution to free the snapshot but not delete the image object. Before free the memory, be sure to firstly unlink it from image object, using `lv_img_set_src(NULL)` and `lv_img_cache_invalidate_src(src)`.


Below code snippet explains usage of this API.

```c
void update_snapshot(lv_obj_t * obj, lv_obj_t * img_snapshot)
{
    lv_img_dsc_t* snapshot = (void*)lv_img_get_src(img_snapshot);
    if(snapshot) {
        lv_snapshot_free(snapshot);
    }
    snapshot = lv_snapshot_take(obj, LV_IMG_CF_TRUE_COLOR_ALPHA);
    lv_img_set_src(img_snapshot, snapshot);
}
```

### Use Existing Buffer
If the snapshot needs update now and then, or simply caller provides memory, use API `lv_res_t lv_snapshot_take_to_buf(lv_obj_t * obj, lv_img_cf_t cf, lv_img_dsc_t * dsc, void * buf, uint32_t buff_size);` for this case. It's caller's responsibility to alloc/free the memory.


If snapshot is generated successfully, the image descriptor is updated and image data will be stored to provided `buf`.


Note that snapshot may fail if provided buffer is not enough, which may happen when object size changes. It's recommended to use API `lv_snapshot_buf_size_needed` to check the needed buffer size in byte firstly and resize the buffer accordingly.

## Example

```eval_rst

.. include:: ../../examples/others/snapshot/index.rst

```
## API


```eval_rst

.. doxygenfile:: lv_snapshot.h
  :project: lvgl

```
