.. _cache_framework_implementation_details:

Implementation Details
======================

1. Cache Strategy Implementation
--------------------------------

1.1 LRU Cache Strategy
~~~~~~~~~~~~~~~~~~~~~~

The LVGL cache framework implements a Least Recently Used (LRU) cache strategy, which is a widely used cache eviction algorithm. When the cache reaches its capacity limit, the LRU strategy evicts entries that have not been accessed for the longest time to make space for new entries.

1.1.1 Data Structures
^^^^^^^^^^^^^^^^^^^^^

The LRU cache implementation uses two main data structures:

1. **Red-Black Tree**: Used for efficient cache entry lookup with O(log n) time complexity.
2. **Doubly Linked List**: Used to maintain the usage order of cache entries, with recently used entries at the head of the list and least recently used entries at the tail.

.. code-block:: c

   typedef struct _lv_lru_rb_t {
       lv_cache_t cache;           /* Basic cache structure */
       lv_rb_t rb;                /* Red-Black Tree */
       lv_ll_t ll;                /* Doubly linked list */
       lv_lru_rb_get_size_cb_t get_data_size_cb; /* Callback function to get data size */
   } lv_lru_rb_t;

.. mermaid::
   :zoom:

   %%{init: {'theme':'neutral'}}%%
   graph TD
       subgraph Red-Black Tree
           RB["Red-Black Tree Root"] --> A["Node A"] & B["Node B"]
           A --> C["Node C"] & D["Node D"]
           B --> E["Node E"] & F["Node F"]
       end

       subgraph Doubly Linked List
           LL["List Head"] --> B
           B --> E
           E --> A
           A --> D
           D --> C
           C --> F["List Tail"]
       end

       A -.-> A_LL["List Node A"]
       B -.-> B_LL["List Node B"]
       C -.-> C_LL["List Node C"]
       D -.-> D_LL["List Node D"]
       E -.-> E_LL["List Node E"]
       F -.-> F_LL["List Node F"]

1.1.2 LRU Operation Flow
^^^^^^^^^^^^^^^^^^^^^^^^

1. **Lookup Operation**:
   - Use the red-black tree for fast cache entry lookup
   - If found, move the entry to the head of the list (mark as recently used)
   - Return the found entry

2. **Add Operation**:
   - Check if there is enough space
   - If space is insufficient, evict the entry at the tail of the list (least recently used)
   - Create a new entry and add it to the red-black tree and head of the list
   - Return the newly added entry

3. **Eviction Operation**:
   - Get the least recently used entry from the tail of the list
   - Check if the entry can be evicted (reference count is zero)
   - If evictable, remove the entry from the red-black tree and linked list
   - Mark the entry as invalid, but do not free memory (wait until reference count is zero to free)

1.2 Count-based LRU Cache
~~~~~~~~~~~~~~~~~~~~~~~~~

The count-based LRU cache (`lv_cache_class_lru_rb_count`) limits the number of entries in the cache. When the number of entries in the cache reaches the maximum value, it evicts the least recently used entries.

.. code-block:: c

   typedef struct _lv_lru_rb_count_t {
       lv_lru_rb_t lru_rb;        /* LRU red-black tree base structure */
   } lv_lru_rb_count_t;

1.2.1 Space Check
^^^^^^^^^^^^^^^^^

.. code-block:: c

   static lv_cache_reserve_cond_t lru_rb_count_reserve_cond_cb(lv_cache_t * cache, const void * key, uint32_t reserved_size, void * user_data)
   {
       lv_lru_rb_count_t * count_cache = (lv_lru_rb_count_t *)cache;
       
       if(cache->size < cache->max_size) {
           return LV_CACHE_RESERVE_COND_OK;
       }
       
       return LV_CACHE_RESERVE_COND_NEED_VICTIM;
   }

1.3 Size-based LRU Cache
~~~~~~~~~~~~~~~~~~~~~~~~

The size-based LRU cache (`lv_cache_class_lru_rb_size`) limits the total memory size occupied by the cache. When the cache memory usage reaches the maximum value, it evicts the least recently used entries until there is enough space to accommodate new entries.

.. code-block:: c

   typedef struct _lv_lru_rb_size_t {
       lv_lru_rb_t lru_rb;        /* LRU red-black tree base structure */
       uint32_t total_size;       /* Current total memory size occupied by cache */
   } lv_lru_rb_size_t;

1.3.1 Space Check
^^^^^^^^^^^^^^^^^

.. code-block:: c

   static lv_cache_reserve_cond_t lru_rb_size_reserve_cond_cb(lv_cache_t * cache, const void * key, uint32_t reserved_size, void * user_data)
   {
       lv_lru_rb_size_t * size_cache = (lv_lru_rb_size_t *)cache;
       uint32_t data_size = size_cache->lru_rb.get_data_size_cb(key, user_data);
       
       if(data_size > cache->max_size) {
           return LV_CACHE_RESERVE_COND_TOO_LARGE;
       }
       
       if(size_cache->total_size + data_size <= cache->max_size) {
           return LV_CACHE_RESERVE_COND_OK;
       }
       
       return LV_CACHE_RESERVE_COND_NEED_VICTIM;
   }

2. Reference Counting Mechanism
-------------------------------

2.1 Reference Counting Principle
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Cache entries use a reference counting mechanism to track how many clients are currently using the entry. This mechanism ensures that even if an entry is evicted from the cache, resources in use will not be freed until all clients release them.

.. code-block:: c

   typedef struct _lv_cache_entry_t {
       const lv_cache_t * cache;   /* Pointer to the owning cache instance */
       int32_t ref_cnt;           /* Reference count */
       uint32_t node_size;        /* Size of the entry */
       bool is_invalid;           /* Invalid flag */
   } lv_cache_entry_t;

2.2 Reference Counting Operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

2.2.1 Increment Reference Count
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   void * lv_cache_entry_acquire(lv_cache_entry_t * entry)
   {
       LV_ASSERT_NULL(entry);

       lv_cache_entry_inc_ref(entry);
       return lv_cache_entry_get_data(entry);
   }

2.2.2 Decrement Reference Count
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   void lv_cache_entry_release(lv_cache_entry_t * entry)
   {
       LV_ASSERT_NULL(entry);
       if(lv_cache_entry_get_ref(entry) == 0) {
           LV_LOG_ERROR("ref_cnt(%" LV_PRIu32 ") == 0", entry->ref_cnt);
           return;
       }

       lv_cache_entry_dec_ref(entry);
   }

2.2.3 Get Reference Count
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

   int32_t lv_cache_entry_get_ref(lv_cache_entry_t * entry)
   {
       LV_ASSERT_NULL(entry);
       return entry->ref_cnt;
   }

3. Memory Management
--------------------

3.1 Memory Allocation
~~~~~~~~~~~~~~~~~~~~~

The cache framework uses LVGL's memory allocation functions `lv_malloc` and `lv_free` to manage memory. These functions can use different memory allocation strategies depending on the platform and configuration.

.. code-block:: c

   lv_cache_entry_t * entry = lv_malloc(cache->node_size);

3.2 Memory Layout
~~~~~~~~~~~~~~~~~

The memory layout of cache entries is as follows:

.. code-block:: text

   +------------------+
   |    User Data     | Actual cached data
   +------------------+
   | lv_cache_entry_t | Cache entry header
   +------------------+

The user data pointer can be obtained through the following function:

.. code-block:: c

   void * lv_cache_entry_get_data(lv_cache_entry_t * entry)
   {
       LV_ASSERT_NULL(entry);
       return (uint8_t *)entry - entry->node_size;
   }

3.3 Memory Release Strategy
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The cache framework uses a reference counting mechanism to manage memory release. A cache entry's memory is only freed when its reference count drops to zero and it is marked as invalid.

4. Debug Support
----------------

4.1 Cache Names
~~~~~~~~~~~~~~~

Each cache instance can be set with a name for debugging and identification purposes.

.. code-block:: c

   void lv_cache_set_name(lv_cache_t * cache, const char * name)
   {
       LV_ASSERT_NULL(cache);
       cache->name = name;
   }

4.2 Assertions
~~~~~~~~~~~~~~

The cache framework uses LVGL's assertion mechanism to check the validity of parameters and states, helping developers discover and fix errors.

.. code-block:: c

   LV_ASSERT_NULL(cache);
   LV_ASSERT(lv_cache_entry_get_ref(entry) == 0);