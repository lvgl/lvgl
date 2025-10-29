.. _guide_add_cache_class:

====================================
How to add a new cache class to LVGL
====================================

This guide provides a comprehensive walkthrough for developers who need to implement a
custom caching strategy in LVGL. By following these steps, you will learn how to
create a new Cache Class, integrate it with a Cache Instance, and validate its
performance.



Overview
********

Caching is a critical optimization technique in embedded graphics systems. It reduces
the computational load of repeated operations—such as image decoding, font
rasterization, or complex layout calculations—and ensures predictable memory usage.

LVGL's Cache Framework employs a decoupled architecture:

- **Cache Class (Policy/Implementation)**: Defines *how* the cache behaves. It handles
  storage structures (e.g., trees, lists), eviction policies (e.g., LRU, FIFO), and
  memory management.
- **Cache Instance (Usage/Entity)**: Defines *what* is cached. It specifies the data
  type, key comparison logic, and connects a specific dataset to a Cache Class.

This separation allows you to reuse a single efficient LRU strategy for multiple
purposes, such as caching images, glyphs, or even file system handles.

**Prerequisites**: Before proceeding, it is recommended to review the following
technical documentation: :doc:`../internal-subsystems/cache/overview` |
:doc:`../internal-subsystems/cache/architecture`



Step-by-Step Implementation Guide
*********************************

In this tutorial, we will walk through the creation of the **Linked List-based LRU
Cache (LL-LRU)** module (`lv_cache_lru_ll.c`) as if we were building it from scratch.
This serves as an excellent reference for implementing your own custom cache class, as
it demonstrates the core mechanics without the complexity of balanced trees.

.. note::
   The code examples below use **pseudo-code** (Python-style) to illustrate the logic
   and flow. In a real implementation, you must use standard C code adhering to the
   LVGL coding style.


Phase 1: Define the Cache Class
-------------------------------

The Cache Class is the engine of your manager. It implements the standard
:cpp:type:`lv_cache_class_t` interface.

**1. Create the Implementation Files**

Typically, you would create `src/misc/cache/class/lv_cache_my_ll.c` and
`lv_cache_my_ll.h`. For this guide, we reference the existing `lv_cache_lru_ll.c`.

**2. Define the Internal Structure**

Your cache structure must inherit from :cpp:type:`lv_cache_t`. This is crucial for
polymorphism in C. We add an :cpp:type:`lv_ll_t` to store our entries.

The `cache` member must be the first element in the struct to ensure correct
inheritance.

The `ll` member is the linked list that will store the cache entries.

.. code-block:: c

    struct _lv_cache_lru_ll_t {
        lv_cache_t cache;
        lv_ll_t ll;
    };

**3. Implement Core Callbacks**

You need to implement a set of function pointers defined in
:cpp:type:`lv_cache_class_t`. These callbacks define the lifecycle and behavior of
your cache.

Allocation (`alloc_cb`)
~~~~~~~~~~~~~~~~~~~~~~~

This callback is responsible for allocating memory for your specific cache structure.
It should allocate the size of your custom struct (e.g., `lv_cache_lru_ll_t`) and
ensure the memory is zero-initialized to prevent undefined behavior from uninitialized
members.

.. code-block:: python

    def alloc_cb():
        res = lv_malloc_zeroed(sizeof(lv_cache_lru_ll_t))
        if not res:
            return NULL

        return res



Initialization (`init_cb`)
~~~~~~~~~~~~~~~~~~~~~~~~~~

Here you initialize your internal data structures. For our linked-list implementation,
we need to initialize the list with the correct node size.

.. note::
    LVGL's cache management relies on `lv_cache_entry_t` nodes rather than raw user
    data. This architecture is essential for enabling cross-thread safety, reference
    counting, and traceability. Consequently, the internal storage structure (e.g.,
    linked list) must be constructed using nodes based on `lv_cache_entry_t`. The
    total size of such a node is calculated via `lv_cache_entry_get_size`, which
    combines the internal header size with the `cache.node_size` specified during
    creation.

.. code-block:: python

    def init_cb(cache):
        lru = (lv_cache_lru_ll_t *)cache

        lv_ll_init(lru.ll, lv_cache_entry_get_size(cache.node_size))

        return True



Lookup (`get_cb`)
~~~~~~~~~~~~~~~~~

This function looks up an entry by key. If found, it promotes the entry to the head of
the list to mark it as "Recently Used" (the core of the LRU policy).

The process involves:

1. **Linear Search**: Iterate through the linked list (O(n)).
2. **Comparison**: Use `cache.ops.compare_cb` to check if the current node matches the
   key.
3. **Promotion**: If a match is found, move the node to the head of the list using
   `lv_ll_move_before`.
4. **Return**: Convert the internal list node to a cache entry pointer using
   `lv_cache_entry_get_entry` and return it.

.. code-block:: python

    def get_cb(cache, key, user_data):
        lru = (lv_cache_lru_ll_t *)cache

        node = lv_ll_get_head(lru.ll)
        while node:
            if cache.ops.compare_cb(node, key) == 0:
                break
            node = lv_ll_get_next(lru.ll, node)

        if node:
            lv_ll_move_before(lru.ll, node, lv_ll_get_head(lru.ll))
            return lv_cache_entry_get_entry(node, cache.node_size)

        return NULL



Adding Entries (`add_cb`)
~~~~~~~~~~~~~~~~~~~~~~~~~

This callback allocates a new node, inserts it at the head of the list (making it the
MRU), and initializes the entry.

.. important::
    You must explicitly call ``lv_cache_entry_init``.

    Since C lacks constructors, and the memory for the entry is managed externally
    (allocated by ``lv_ll_ins_head`` in this case), the cache system does not
    automatically initialize the entry's internal state (like reference counts or
    validity flags). You must perform this initialization manually after allocation.

.. code-block:: python

    def add_cb(cache, key, user_data):
        lru = (lv_cache_lru_ll_t *)cache

        node = lv_ll_ins_head(lru.ll)
        if not node:
            return NULL

        entry = lv_cache_entry_get_entry(node, cache.node_size)
        lv_cache_entry_init(entry, cache, cache.node_size)

        # Copy key/data to node if necessary
        # memcpy(node, key, ...)

        cache.size += 1
        return entry



Victim Selection (`get_victim_cb`)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When the cache is full, LVGL asks this callback to select an entry to evict. For an
LRU policy, the victim is the "Least Recently Used" item, which corresponds to the
tail of the list.

**Critical Safety Check**: You must respect the reference count (`ref_cnt`). If
`lv_cache_entry_get_ref(entry) > 0`, it means the application is currently using this
data. Evicting it would cause a crash. Therefore, we iterate from the tail backwards
until we find an entry with `ref_cnt == 0`. If all items are in use, we return `NULL`.

.. code-block:: python

    def get_victim_cb(cache, user_data):
        lru = (lv_cache_lru_ll_t *)cache

        node = lv_ll_get_tail(lru.ll)
        while node:
            entry = lv_cache_entry_get_entry(node, cache.node_size)

            if lv_cache_entry_get_ref(entry) == 0:
                return entry

            node = lv_ll_get_prev(lru.ll, node)

        return NULL



Removal (`remove_cb`)
~~~~~~~~~~~~~~~~~~~~~

This function removes an entry from your internal data structure (the linked list) and
updates the size.

.. important::

   This function does **not** free the memory. It only detaches the node from the
   cache.

   When ``lv_cache_drop`` is called, if the reference count is not 0, the entry is
   removed from the cache (via this ``remove_cb``) and marked as invalid.

   The ``lv_cache_release`` function primarily decreases the reference count. However,
   when the reference count reaches 0 and the entry is marked as invalid,
   ``lv_cache_release`` triggers the actual memory freeing process.

.. code-block:: python

    def remove_cb(cache, entry, user_data):
        lru = (lv_cache_lru_ll_t *)cache
        node = lv_cache_entry_get_data(entry)

        lv_ll_remove(lru.ll, node)

        cache.size -= 1



Dropping Entries (`drop_cb`)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This callback performs the full deletion of a specific entry. It orchestrates three
steps:

1. **Detach**: Remove from the linked list via `remove_cb`.
2. **Free User Data**: Call the user-provided `free_cb` to clean up resources (e.g.,
   textures).
3. **Free Entry**: Release the memory of the cache header/node itself using
   `lv_cache_entry_delete`.

.. code-block:: python

    def drop_cb(cache, key, user_data):
        entry = get_cb(cache, key, user_data)
        if entry:
            remove_cb(cache, entry, user_data)

            cache.ops.free_cb(lv_cache_entry_get_data(entry), user_data)

            lv_cache_entry_delete(entry)



Dropping All (`drop_all_cb`)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This callback clears the entire cache.

It iterates through all nodes to release the user data, but it must handle referenced
entries carefully:

1. Iterate through all nodes in the linked list.
2. For each node, check the reference count (``ref_cnt``) of its entry.
3. If ``ref_cnt`` is 0, the entry is unused. Call ``free_cb`` to release the
   associated data (e.g. image buffer).
4. If ``ref_cnt`` is > 0, the entry is still in use. **Do not** free the data, but log
   a warning.
5. After processing all nodes, clear the linked list to free the memory of the cache
   nodes themselves.

.. note::
    If referenced entries existed, their nodes are **forced freed** here, which
    renders those references invalid (hence the warning).

.. code-block:: python

    def drop_all_cb(cache, user_data):
        node = lv_ll_get_head(lru.ll)
        while node:
            entry = lv_cache_entry_get_entry(node, cache.node_size)

            if lv_cache_entry_get_ref(entry) == 0:
                cache.ops.free_cb(lv_cache_entry_get_data(entry), user_data)
            else:
                print("Warning: entry %p is still referenced (ref_cnt: %d)" % (entry, lv_cache_entry_get_ref(entry)))

            node = lv_ll_get_next(lru.ll, node)

        lv_ll_clear(lru.ll)
        cache.size = 0


Destruction (`destroy_cb`)
~~~~~~~~~~~~~~~~~~~~~~~~~~

This tears down the entire cache instance. It should clear the list (freeing all
nodes).

.. note::
    The ``lv_cache_t`` instance itself is freed by ``lv_cache_destroy`` after this
    callback returns.

.. code-block:: python

    def destroy_cb(cache, user_data):
        cache.ops.drop_all_cb(cache, user_data)
        cache.size = 0



Optional: Reserve Condition (`reserve_cond_cb`)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This callback checks if the cache can accept a new entry of a given size. It is used
to determine if we need to evict existing entries (`NEED_VICTIM`) or if the new entry
is simply too large (`TOO_LARGE`).

* ``key``: The data being added to the cache. You can inspect this to determine the
  size of the new entry.
* ``reserved_size``: Additional size explicitly requested to be reserved (e.g. by
  ``lv_cache_reserve``).

.. code-block:: python

    def reserve_cond_cb(cache, key, reserved_size, user_data):
        new_item_size = 1
        # if key:
            # new_item_size = get_size_of(key)

        if new_item_size > cache.max_size:
            return TOO_LARGE

        total_required = cache.size + new_item_size + reserved_size
        if total_required > cache.max_size:
            return NEED_VICTIM

        return OK

The logic generally follows these steps:

1. Calculate the size of the new item based on the key. For a simple count-based
   cache, this is typically 1. For a size-based cache, you would inspect 'key' to find
   its size.
2. Check if the item itself is larger than the cache's total capacity.
3. Check if adding this item + reserved space would exceed the limit.



Optional: Iterator (`iter_create_cb`)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This allows tools like `lv_sysmon` to inspect the cache content.

.. note::
    The integration with `lv_sysmon` is not implemented in this guide.

    However, the **Image Cache** and **Image Header Cache** implementations in LVGL
    already support this. You can refer to `src/misc/cache/instance/lv_image_cache.c`
    (specifically `lv_image_cache_dump` and `iter_inspect_cb`) to see how to iterate
    over the cache and print entry details like size, reference count, and data
    content.

    If you are interested in this functionality, contributions are welcome.

.. code-block:: python

    def iter_create_cb(cache):
        return lv_iter_create(cache, ...)



Phase 2: Register the Class
---------------------------

Finally, define the global class structure. This structure serves as the VTable
(Virtual Table) for your cache class, linking all your implemented callbacks.

**In the Source File (.c):**

.. code-block:: c

    const lv_cache_class_t lv_cache_class_my_ll = {
        .alloc_cb = alloc_cb,
        .init_cb = init_cb,
        .destroy_cb = destroy_cb,

        .get_cb = get_cb,
        .add_cb = add_cb,
        .remove_cb = remove_cb,
        .drop_cb = drop_cb,
        .drop_all_cb = drop_all_cb,
        .get_victim_cb = get_victim_cb,
        .reserve_cond_cb = reserve_cond_cb,

        .iter_create_cb = iter_create_cb,
    };

**In the Header File (.h):**

Expose the class structure so other components can create instances of your cache.

.. code-block:: c

    extern const lv_cache_class_t lv_cache_class_my_ll;



Phase 3: Create a Cache Instance
--------------------------------

Now that we have a policy (Class), we need to define *what* we are caching (Instance).
Let's see how a custom cache for storing pre-calculated layout data would be created
using our new class.

**1. Define Data Structures**

First, define the key structure that uniquely identifies each cache entry.

.. code-block:: c

    typedef struct {
        uint32_t layout_id;
        uint16_t width;
        uint16_t height;
    } my_layout_key_t;

Next, define the data payload you want to cache.

.. code-block:: c

    typedef struct {
        int32_t x;
        int32_t y;
    } my_pos_t;

Finally, combine the key and data into a single structure. This structure will be the
unit of storage in the cache.

.. code-block:: c

    typedef struct {
        my_layout_key_t key;
        my_pos_t position;
    } my_layout_data_t;

.. note::
    To simplify, you can also define a single structure that contains both key fields
    and data fields directly, instead of nesting separate structs.

    .. code-block:: c

        typedef struct {
            /* Key fields */
            uint32_t layout_id;
            uint16_t width;
            uint16_t height;

            /* Data fields */
            int32_t x;
            int32_t y;
        } my_simple_layout_data_t;



**2. Implement Instance Callbacks**

Comparison (`compare_cb`)
~~~~~~~~~~~~~~~~~~~~~~~~~

This callback is used by the Class to check for equality between keys.

.. important::
    **Important Note on Types**: In LVGL's cache implementation, the `compare_cb`
    always receives two pointers to the **Data Structure** type (`const
    my_layout_data_t *`).

    *   The first argument (`k1`) is a pointer to the data of an entry already stored
        in the cache.
    *   The second argument (`k2`) is the search key provided by the user.

Crucially, even if you are just searching, you must pass a pointer that *looks like*
your data structure (at least the *key fields* **MUST** be valid). The cache does not
distinguish between a `key type` or a `data type`.

The return value indicates the relative order of the keys:

* **< 0**: `k1` is smaller than `k2`
* **> 0**: `k1` is greater than `k2`
* **0**: `k1` is equal to `k2`

So, we get the following comparison function:

.. code-block:: python

    def compare_cb(k1, k2):
        data1 = (my_layout_data_t *)k1
        data2 = (my_layout_data_t *)k2

        if data1.key.layout_id != data2.key.layout_id:
            return data1.key.layout_id - data2.key.layout_id
        if data1.key.width != data2.key.width:
            return data1.key.width - data2.key.width
        return data1.key.height - data2.key.height



Freeing Data (`free_cb`)
~~~~~~~~~~~~~~~~~~~~~~~~

This callback is invoked when an entry is permanently deleted (evicted or dropped).

Use this callback to clean up resources:

* **Complex Data**: If your data structure contains pointers to dynamically allocated
  memory or other resources, you **MUST** free them here.
* **Simple Data**: If your data is self-contained (like in this example), this
  function can be empty.

.. tip::
    The reason this callback can be left empty is that the memory is managed
    inherently by the **Cache Entry**.

    We encourage you to rely on this mechanism to minimize manual memory allocation
    and deallocation. This reduces the complexity of your code and lowers the risk of
    memory management errors.

.. code-block:: python

    def free_cb(data, user_data):
        pass



**3. Initialize the Cache**

Create the cache instance at runtime. You must provide:

* The **Class** to use (`&lv_cache_class_my_ll`).
* The **Size** of your user data (`my_layout_data_t`).
* The **Capacity** (e.g., 100 entries).
* The **Operations** structure containing your callbacks.

.. code-block:: c

    static lv_cache_t * layout_cache;

    void my_layout_cache_init(void) {
        layout_cache = lv_cache_create(
            &lv_cache_class_my_ll,
            sizeof(my_layout_data_t),
            100,
            (lv_cache_ops_t){
                .compare_cb = compare_cb,
                .create_cb = NULL,
                .free_cb = free_cb
            }
        );
    }



Phase 4: Integration and Usage
------------------------------

Integrate the cache into your subsystem's workflow. The typical flow is:

"Check Cache -> (Miss? Compute -> Add) -> Return".

The following pseudo-code demonstrates the process:

1. **Prepare Key**: Create a dummy data structure with the search key populated.
2. **Acquire**: Call `lv_cache_acquire`.
3. **Handle Hit**: If `entry` is not NULL, retrieve data, copy it, and **release** the
   entry.
4. **Handle Miss**: Compute the result, **add** it to the cache, populate the data,
   and **release** the new entry.

.. note::
    **Why is `release` necessary?**

    When you `acquire` or `add` an entry, LVGL increments its internal **reference
    count**. This tells the cache manager: *"This entry is currently in use, do not
    remove it!"*

    If you forget to call `lv_cache_release`, the reference count will never drop to
    zero. Consequently, the cache policy (like LRU) will consider this entry "locked"
    and will **NEVER** evict it. Over time, this will fill up your cache with
    un-removable entries, effectively causing a memory leak within the cache.

.. code-block:: python

    def get_layout_data(id, w, h):
        search_record = my_layout_data_t()
        search_record.key.layout_id = id
        search_record.key.width = w
        search_record.key.height = h

        entry = lv_cache_acquire(layout_cache, search_record, NULL)

        if entry:
            data = lv_cache_entry_get_data(entry)
            result = data.position

            lv_cache_release(layout_cache, entry, NULL)
            return result

        result = heavy_layout_computation(id, w, h)

        entry = lv_cache_add(layout_cache, search_record, NULL)
        if entry:
            data = lv_cache_entry_get_data(entry)
            data.position = result

            lv_cache_release(layout_cache, entry, NULL)

        return result

.. note::
    **Copy vs. Hold**

    In this example, our data (`my_pos_t`) is simple and small, so we perform a
    **shallow copy** of the data and immediately release the cache entry.

    If your data requires **zero-copy** access (e.g., a large image buffer) or **deep
    copy** management, you cannot simply copy and release. Instead, you would design a
    pair of API functions:

    1. `get_layout_data(...)`: Acquires the cache entry and returns a pointer to the
       data (without releasing).
    2. `release_layout_data(...)`: Accepts the pointer and releases the underlying
       cache entry.



Testing and Validation
**********************

To ensure your new cache class works correctly and efficiently:

1. **Functional Testing**:

   - Verify that :cpp:func:`lv_cache_acquire` returns ``NULL`` for non-existent keys.
   - Verify that :cpp:func:`lv_cache_add` correctly stores data.
   - Verify that adding items beyond capacity triggers ``get_victim_cb`` and evicts
     the oldest item.

2. **Performance Metrics**:

   - **Hit Ratio**: Instrument your code to count hits vs. total requests.
   - **Memory Overhead**: Use :cpp:func:`lv_mem_monitor` to check if the cache stays
     within limits.
   - **Latency**: Measure the time difference between a Cache Hit and a Cache Miss.

1. **Stress Testing**:

   - Create a loop that adds thousands of random entries to force frequent evictions.
   - Ensure no memory leaks occur using address sanitizers (ASan).



Conclusion
**********

By following this guide, you have successfully implemented a **Linked List-based LRU
Cache Class**.

* The **Class** handles the *mechanics* of storage and eviction (LRU logic).
* The **Instance** handles the *specifics* of your data (Size, Comparison, Data
  Management).

This separation of concerns allows you to reuse the same LRU logic for completely
different types of data, from images to font glyphs to custom layout calculations.
