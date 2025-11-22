.. _cache_framework_api_reference:

API Reference
=============

1. Core API Functions
~~~~~~~~~~~~~~~~~~~~~

1.1 Cache Creation and Destruction
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

lv_cache_create
^^^^^^^^^^^^^^^

.. code-block:: c

    lv_cache_t * lv_cache_create(const lv_cache_class_t * cache_class, uint32_t node_size, uint32_t max_size, lv_cache_ops_t ops);

**Description**: Creates a new cache instance with the specified parameters.

**Parameters**:

- ``cache_class``: Pointer to the cache class that defines the behavior of the cache. Currently only supports two built-in classes:
    - lv_cache_class_lru_rb_count for LRU-based cache with count-based eviction policy.
    - lv_cache_class_lru_rb_size for LRU-based cache with size-based eviction policy.

- ``node_size``: Size of each cache entry node.

- ``max_size``: The max size is the maximum amount of memory or count that the cache can hold.
    - lv_cache_class_lru_rb_count: max_size is the maximum count of nodes in the cache.
    - lv_cache_class_lru_rb_size: max_size is the maximum size of the cache in bytes.

- ``ops``: Callback functions for comparing, creating, and freeing cache entries.

**Returns**: Pointer to the created cache instance, or NULL if creation failed.

**Example**:

.. code-block:: c

    lv_result_t lv_image_cache_init(uint32_t size)
    {
        if(img_cache_p != NULL) {
            return LV_RESULT_OK;
        }

        img_cache_p = lv_cache_create(&lv_cache_class_lru_rb_size,
        sizeof(lv_image_cache_data_t), size, (lv_cache_ops_t) {
            .compare_cb = (lv_cache_compare_cb_t) image_cache_compare_cb,
            .create_cb = NULL,
            .free_cb = (lv_cache_free_cb_t) image_cache_free_cb,
        });

        lv_cache_set_name(img_cache_p, CACHE_NAME);
        return img_cache_p != NULL ? LV_RESULT_OK : LV_RESULT_INVALID;
    }

- This code initializes an image cache with a maximum size of ``size`` bytes.
- The cache uses the LRU-based cache with size-based eviction policy.
- The cache entry node size is ``sizeof(lv_image_cache_data_t)``.
- The cache compares cache entries using the ``image_cache_compare_cb`` function.
- The cache frees cache entries using the ``image_cache_free_cb`` function.

lv_cache_destroy
^^^^^^^^^^^^^^^^

.. code-block:: c

    void lv_cache_destroy(lv_cache_t * cache);

**Description**: Destroys a cache instance and frees all associated resources.

**Parameters**:

- ``cache``: Pointer to the cache instance to destroy

**Returns**: None

1.2 Cache Operations
~~~~~~~~~~~~~~~~~~~~

lv_cache_acquire_or_create
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    lv_cache_entry_t * lv_cache_acquire_or_create(lv_cache_t * cache, const void * key, void * user_data);

**Description**:

- Acquire a cache entry with the given key. If the entry is not in the cache, it will create a new entry
  with the given key.
- If the entry is found, its priority will be changed by the cache's policy. And the ``lv_cache_entry_t::ref_cnt`` will
  be
  incremented.
- If you want to use this API to simplify the code, you should provide a ``lv_cache_ops_t::create_cb`` that creates a new
  entry with the given key.
- This API is a combination of ``lv_cache_acquire()`` and ``lv_cache_add()``. The effect is the same as calling
  ``lv_cache_acquire()``
  and ``lv_cache_add()`` separately.
- And the internal impact on cache is also consistent with these two APIs.

**Parameters**:

- ``cache``: The cache object pointer to acquire the entry.
- ``key``: The key of the entry to acquire or create.
- ``user_data``: A user data pointer that will be passed to the create callback.

**Returns**: Returns a pointer to the acquired or created cache entry on success with ``lv_cache_entry_t::ref_cnt``
incremented, ``NULL`` on error.

**Flowchart**

The flowchart of ``lv_cache_acquire_or_create`` function is as follows:

.. mermaid::
    :zoom:

    %%{init: {'theme':'neutral'}}%%
    flowchart TD
        A[Start] --> B{Cache Entry Exists?}
        B -- Yes --> C[Change Priority]
        C --> D[Increment Ref Count]
        D --> E[Return Entry or NULL]
        B -- No --> F[Create Entry]
        F --> G[Create Success?]
        G -- Yes --> H[Add Entry to Cache]
        G -- No --> I[Clean up Created Entry just now]
        H --> E
        I --> E

Viewing the flowchart from the perspective of API calls:

.. mermaid::
    :zoom:

    %%{init: {'theme':'neutral'}}%%
    flowchart TD
        A[lv_cache_acquire_or_create] --> B{lv_cache_acquire}
        B -- Yes --> E[return entry or NULL]
        B -- No --> G[lv_cache_add]
        G -- Yes --> H[cache_ops_t::create_cb]
        H -- No --> I[cache_ops_t::free_cb]
        I --> E
        H -- Yes --> E

lv_cache_acquire
^^^^^^^^^^^^^^^^

.. code-block:: c

    lv_cache_entry_t * lv_cache_acquire(lv_cache_t * cache, const void * key, void * user_data);

**Description**: Acquires a cache entry with the given key. If the entry is not found in the cache, it will return
``NULL``.
If the entry is found, its priority will be changed by the cache's policy. And the ``lv_cache_entry_t::ref_cnt`` will be
incremented.

**Parameters**:

- ``cache``: The cache object pointer to acquire the entry.
- ``key``: The key of the entry to acquire.
- ``user_data``: A user data pointer that will be passed to the create callback.

**Returns**: Returns a pointer to the acquired cache entry on success with ``lv_cache_entry_t::ref_cnt`` incremented,
``NULL`` on error.

**Example**:

.. code-block:: c

    lv_draw_buf_t * lv_image_decoder_get_data(lv_image_decoder_dsc_t * dsc)
    {
        // Create search key
        lv_image_cache_data_t search_key = {
            .src = dsc->src,
            .src_type = dsc->src_type,
        };
        
        // Try to get from cache
        lv_cache_entry_t * entry = lv_cache_acquire(img_cache_p, &search_key, NULL);
        if(entry) {
            // Cache hit
            lv_image_cache_data_t * cached = lv_cache_entry_get_data(entry);
            dsc->decoded = cached->decoded;
            return dsc->decoded;
        }
        
        // Cache miss, decode image
        // ...
    }

- This code tries to get the decoded image from the cache using the ``lv_cache_acquire`` function. If the cache hit, it
  returns the decoded image. And it will increment the ``lv_cache_entry_t::ref_cnt`` of the entry. If the cache miss, it
  decodes the image and adds it to the cache using the
  ``lv_cache_add`` function. If the cache is full, it may evict existing entries.

lv_cache_add
^^^^^^^^^^^^

.. code-block:: c

    lv_cache_entry_t * lv_cache_add(lv_cache_t * cache, const void * key, void * user_data);

**Description**: Adds a new entry to the cache. If the cache is full, it may evict existing entries.

**Parameters**:

- ``cache``: Pointer to the cache instance
- ``key``: Key to associate with the new entry
- ``user_data``: User data to pass to the callback functions

**Returns**: Returns a pointer to the added cache entry on success with ``lv_cache_entry_t::ref_cnt`` incremented, ``NULL``
on error.

- Note: ``key`` will be copied to the cache entry.

**Example**:

.. code-block:: c

    // Add decoded result to cache
    entry = lv_cache_add(img_cache_p, &search_key, NULL);
    if(entry) {
        lv_image_cache_data_t * cached = lv_cache_entry_get_data(entry);
        cached->decoded = decoded_image;
    }

- This code adds the decoded image to the cache using the ``lv_cache_add`` function. If the cache is full, it may evict
  existing entries.

lv_cache_release
^^^^^^^^^^^^^^^^

.. code-block:: c

    void lv_cache_release(lv_cache_t * cache, lv_cache_entry_t * entry, void * user_data);

**Description**: Releases a cache entry, decreasing its reference count. If the reference count reaches zero and the
entry is marked as invalid, it will be freed.

**Parameters**:

- ``cache``: Pointer to the cache instance
- ``entry``: Pointer to the cache entry to release
- ``user_data``: User data to pass to the callback functions

**Returns**: None

lv_cache_drop
^^^^^^^^^^^^^

.. code-block:: c

    void lv_cache_drop(lv_cache_t * cache, const void * key, void * user_data);

**Description**: Marks a cache entry as invalid, so it will be freed when its reference count reaches zero.

**Parameters**:

- ``cache``: Pointer to the cache instance
- ``key``: Key of the entry to drop
- ``user_data``: User data to pass to the callback functions

**Returns**: None

lv_cache_drop_all
^^^^^^^^^^^^^^^^^

.. code-block:: c

    void lv_cache_drop_all(lv_cache_t * cache, void * user_data);

**Description**: Marks all cache entries as invalid, so they will be freed when their reference counts reach zero.

**Parameters**:

- ``cache``: Pointer to the cache instance
- ``user_data``: User data to pass to the callback functions

**Returns**: None

1.3 Cache Entry Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~

lv_cache_entry_get_data
^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    void * lv_cache_entry_get_data(const lv_cache_entry_t * entry);

**Description**: Gets the data associated with a cache entry.

**Parameters**:

- ``entry``: Pointer to the cache entry

**Returns**: Pointer to the data associated with the cache entry.

lv_cache_entry_get_ref
^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    int32_t lv_cache_entry_get_ref(const lv_cache_entry_t * entry);

**Description**: Gets the reference count of a cache entry.

**Parameters**:

- ``entry``: Pointer to the cache entry

**Returns**: The reference count of the cache entry.

lv_cache_entry_is_invalid
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    bool lv_cache_entry_is_invalid(const lv_cache_entry_t * entry);

**Description**: Checks if a cache entry is marked as invalid.

**Parameters**:

- ``entry``: Pointer to the cache entry

**Returns**: ``true`` if the entry is invalid, ``false`` otherwise.

2. Cache Classes
~~~~~~~~~~~~~~~~

2.1 LRU Cache Classes
~~~~~~~~~~~~~~~~~~~~~

lv_cache_class_lru_rb_count
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    extern const lv_cache_class_t lv_cache_class_lru_rb_count;

**Description**: Cache class that implements a count-based LRU cache using red-black trees. The cache size is limited by
the number of entries.

lv_cache_class_lru_rb_size
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    extern const lv_cache_class_t lv_cache_class_lru_rb_size;

**Description**: Cache class that implements a size-based LRU cache using red-black trees. The cache size is limited by
the total memory size occupied by the entries.

3. Callback Types
~~~~~~~~~~~~~~~~~

3.1 Cache Operations Callbacks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

lv_cache_compare_cb_t
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef int32_t (*lv_cache_compare_cb_t)(const void * key1, const void * key2);

**Description**: Callback function for comparing two cache keys.

**Parameters**:

- ``key1``: First key to compare
- ``key2``: Second key to compare

**Returns**: Negative value if key1 < key2, 0 if key1 == key2, positive value if key1 > key2.

lv_cache_create_cb_t
^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef void * (*lv_cache_create_cb_t)(const void * key, void * user_data);

**Description**:

- Callback function for creating a new cache entry.
- This callback just fill the data of the cache entry. The cache framework will manage the reference count and memory
  allocation of the
  entry.

**Parameters**:

- ``key``: Key to associate with the new entry
- ``user_data``: User data passed to the callback

**Returns**: Pointer to the created data, or NULL if creation failed.

lv_cache_free_cb_t
^^^^^^^^^^^^^^^^^^

.. code-block:: c

    typedef void (*lv_cache_free_cb_t)(void * data, void * user_data);

**Description**:

- Callback function for freeing a cache entry's data.
- This callback is called when the reference count of the cache entry reaches zero and the entry is marked as invalid.
- Or when the cache create callback returns NULL.

**Note**:

- The cache framework will automatically free the memory of the entry structure.
- The user should only clean up resources within the data, but should not free the data pointer itself in this callback.

**Parameters**:

- ``data``: Pointer to the data to free
- ``user_data``: User data passed to the callback

**Returns**: None

4. Image Cache API
~~~~~~~~~~~~~~~~~~

4.1 Image Cache Functions
~~~~~~~~~~~~~~~~~~~~~~~~~

lv_image_cache_init
^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    lv_result_t lv_image_cache_init(uint32_t size);

**Description**: Initializes the image cache with the specified size.

**Parameters**:

- ``size``: Maximum size of the image cache in bytes

**Returns**: ``LV_RESULT_OK`` if initialization succeeded, ``LV_RESULT_INVALID`` otherwise.


4.2 Image Header Cache Functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

lv_image_header_cache_init
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

    lv_result_t lv_image_header_cache_init(uint32_t count);

**Description**: Initializes the image header cache with the specified count.

**Parameters**:

- ``count``: Maximum number of image headers to cache

**Returns**: ``LV_RESULT_OK`` if initialization succeeded, ``LV_RESULT_INVALID`` otherwise.


5. Usage Examples
~~~~~~~~~~~~~~~~~

5.1 Creating and Using a Custom Cache
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

    // Define cache entry data structure
    typedef struct {
        const char * key;  // Key for lookup
        void * data;       // Cached data
        uint32_t size;     // Size of the data
    } my_cache_data_t;

    // Compare callback for cache entries
    static int32_t my_cache_compare_cb(const void * key1, const void * key2) {
        return strcmp((const char *)key1, (const char *)key2);
    }

    // Free callback for cache entries
    static void my_cache_free_cb(void * data, void * user_data) {
        my_cache_data_t * cache_data = (my_cache_data_t *)data;
        free(cache_data->data);
    }

    // Initialize cache
    lv_cache_t * my_cache;

    void my_cache_init(uint32_t max_size) {
        my_cache = lv_cache_create(&lv_cache_class_lru_rb_size,
            sizeof(my_cache_data_t), max_size, (lv_cache_ops_t) {
                .compare_cb = my_cache_compare_cb,
                .create_cb = NULL,
                .free_cb = my_cache_free_cb,
            });
        
        lv_cache_set_name(my_cache, "MyCache");
    }

    // Add data to cache
    void my_cache_add_data(const char * key, void * data, uint32_t size) {
        my_cache_data_t search_key = {
            .key = key,
        };
        
        // Try to get from cache first
        lv_cache_entry_t * entry = lv_cache_acquire(my_cache, &search_key, NULL);
        if(entry) {
            // Already in cache, release and return
            lv_cache_release(my_cache, entry, NULL);
            return;
        }
        
        // Add to cache
        entry = lv_cache_add(my_cache, &search_key, NULL);
        if(entry) {
            my_cache_data_t * cache_data = lv_cache_entry_get_data(entry);
            cache_data->key = key;
            cache_data->data = data;
            cache_data->size = size;
            lv_cache_release(my_cache, entry, NULL);
        }
    }

    // Get data from cache
    void * my_cache_get_data(const char * key) {
        my_cache_data_t search_key = {
            .key = key,
        };
        
        lv_cache_entry_t * entry = lv_cache_acquire(my_cache, &search_key, NULL);
        if(!entry) {
            return NULL;  // Not in cache
        }
        
        my_cache_data_t * cache_data = lv_cache_entry_get_data(entry);
        void * data = cache_data->data;
        
        lv_cache_release(my_cache, entry, NULL);
        return data;
    }

6. Thread Safety Considerations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The LVGL Cache Framework is designed to be thread-safe, with each cache instance having a mutex lock that is acquired
and released during cache operations. However, there are some considerations to keep in mind when using the cache in a
multi-threaded environment:

1. **Cache Creation and Destruction**: Cache creation and destruction should be done in a thread-safe manner, typically
   during initialization and shutdown when no other threads are accessing the cache.

2. **Cache Operations**: The core cache operations (acquire, add, release, drop) are thread-safe, as they acquire and
   release the cache's mutex lock.

3. **User Callbacks**: User-provided callbacks (compare, create, free) should be thread-safe, as they may be called from
   multiple threads.

4. **Cache Entry Data**: Access to the data associated with a cache entry should be synchronized if multiple threads may
   access it simultaneously.

7. Performance Considerations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When using the LVGL Cache Framework, consider the following performance aspects:

1. **Cache Size**: Choose an appropriate cache size based on the available memory and the expected usage patterns. A
   cache that is too small may result in frequent evictions, while a cache that is too large may waste memory.

2. **Cache Class**: Choose the appropriate cache class based on the use case. For example, use
   ``lv_cache_class_lru_rb_count`` when the number of entries is more important than their size, and use
   ``lv_cache_class_lru_rb_size`` when the total memory usage is more critical.

3. **Callback Efficiency**: Ensure that the compare, create, and free callbacks are efficient, as they may be called
   frequently.

4. **Reference Counting**: Be careful to release cache entries when they are no longer needed, to avoid memory leaks.

5. **Cache Invalidation**: Use ``lv_cache_drop`` or ``lv_cache_drop_all`` to invalidate cache entries when the underlying
   data changes, to avoid using stale data.