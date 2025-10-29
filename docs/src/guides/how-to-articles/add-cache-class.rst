.. _guide_add_cache_class:

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
technical documentation:
:doc:`../../main-modules/cache_framework/overview` |
:doc:`../../main-modules/cache_framework/architecture`

Step-by-Step Implementation Guide
*********************************

In this tutorial, we will walk through the creation of the **Linked List-based LRU
Cache (LL-LRU)** module (`lv_cache_lru_ll.c`) as if we were building it from scratch.
This serves as an excellent reference for implementing your own custom cache class,
as it demonstrates the core mechanics without the complexity of balanced trees.

.. note::
   The code examples below use **pseudo-code** (Python-style) to illustrate the
   logic and flow. In a real implementation, you must use standard C code adhering
   to the LVGL coding style.

Phase 1: Define the Cache Class
-------------------------------

The Cache Class is the engine of your manager. It implements the standard
:cpp:struct:`lv_cache_class_t` interface.

**1. Create the Implementation Files**

Typically, you would create `src/misc/cache/class/lv_cache_my_ll.c` and
`lv_cache_my_ll.h`. For this guide, we reference the existing `lv_cache_lru_ll.c`.

**2. Define the Internal Structure**

Your cache structure must inherit from :cpp:struct:`lv_cache_t`. This is crucial for
polymorphism in C. We add an :cpp:struct:`lv_ll_t` to store our entries and a
callback for getting data size.

.. code-block:: c

    struct _lv_cache_lru_ll_t {
        lv_cache_t cache;                           // MUST be the first member (inheritance)
        lv_ll_t ll;                                 // Linked list to store cache entries
    };

**3. Implement Core Callbacks**

You need to implement a set of function pointers defined in
:cpp:struct:`lv_cache_class_t`. These callbacks define the lifecycle and behavior of
your cache.

- ``alloc_cb``: Allocates memory for your specific cache structure.

  .. code-block:: python

      def alloc_cb():
          # Allocate memory for the custom cache structure
          res = malloc(sizeof(lv_cache_lru_ll_t))
          if not res:
              return NULL
          
          # Zero initialize the memory to ensure safety
          memset(res, 0, sizeof(lv_cache_lru_ll_t))
          return res

- ``init_cb``: Initializes your internal data structures. This is where you set up
  your linked list or tree.

  .. code-block:: python

      def init_cb(cache):
          lru = (lv_cache_lru_ll_t *)cache
          
          # Initialize the linked list with the correct node size
          # node_size includes the cache entry header + user data
          lv_ll_init(lru.ll, lv_cache_entry_get_size(cache.node_size))
          
          return True

- ``get_cb``: Looks up an entry by key. If found, it promotes the entry to the head
  of the list (Mark as Recently Used). This is the core of the LRU logic.

  .. code-block:: python

      def get_cb(cache, key, user_data):
          lru = (lv_cache_lru_ll_t *)cache
          
          # Linear search (O(n)) through the linked list
          node = lru.ll.head
          while node:
              if cache.ops.compare_cb(node, key) == 0:
                  break # Found!
              node = node.next
          
          if node:
              # HIT! Move to head (LRU policy)
              # This keeps frequently accessed items safe from eviction
              lv_ll_move_before(lru.ll, node, lru.ll.head)
              return get_entry_from_node(node)
              
          return NULL # Miss

- ``add_cb``: Creates a new entry and inserts it at the head of the list.

  .. code-block:: python

      def add_cb(cache, key, user_data):
          lru = (lv_cache_lru_ll_t *)cache
          
          # Create a new node at the head of the list
          # This helper function handles memory allocation
          entry = alloc_new_node(lru, key)
          if not entry:
              return NULL

          # Update cache size tracking
          cache.size += 1
          return entry

- ``get_victim_cb``: Selects which entry to evict when the cache is full. For LRU,
  this is the tail of the list (Least Recently Used).

  .. code-block:: python

      def get_victim_cb(cache, user_data):
          lru = (lv_cache_lru_ll_t *)cache
          
          # Iterate from tail (oldest) to head (newest)
          node = lru.ll.tail
          while node:
              entry = get_entry_from_node(node)
              
              # CRITICAL: Only evict entries that are not currently in use (ref_cnt == 0)
              # Evicting a used entry would cause a crash in the application
              if entry.ref_cnt == 0:
                  return entry
                  
              node = node.prev
              
          return NULL # No evictable entry found (Cache is full of locked items)

- ``remove_cb``: Removes an entry from your internal structure.
  **Critical**: This function SHOULD NOT free the entry's memory. It only detaches it 
  from the
  data structure (linked list, tree, etc.) and updates the cache size. The actual 
  memory
  release happens in:

  1. `drop_cb` (manual invalidation)
  2. `evict_one` (automatic eviction)
  3. `lv_cache_release` (when ref_cnt drops to 0 for an invalid entry)

  .. code-block:: python

      def remove_cb(cache, entry, user_data):
          lru = (lv_cache_lru_ll_t *)cache
          node = get_data_from_entry(entry)
          
          # Remove from linked list
          lv_ll_remove(lru.ll, node)
          
          # Update cache size
          cache.size -= 1

- ``destroy_cb``: Destroys the entire cache instance and frees internal resources.

  .. code-block:: python

      def destroy_cb(cache, user_data):
          lru = (lv_cache_lru_ll_t *)cache
          
          # Clear the list and free all nodes
          lv_ll_clear(lru.ll)
          
          # Free the cache structure itself
          free(lru)

- ``drop_cb``: Drops (invalidates) a specific entry from the cache.
  **Difference from `remove_cb`**: `remove_cb` only detaches the entry. `drop_cb`
  orchestrates the full removal process:

  1. Detaches via `remove_cb`
  2. Frees user data via `cache->ops.free_cb`
  3. Frees the entry memory via `lv_cache_entry_delete` (internal helper)

  .. code-block:: python

      def drop_cb(cache, key, user_data):
          entry = get_cb(cache, key, user_data)
          if entry:
              # 1. Remove from internal structure
              remove_cb(cache, entry, user_data)
              
              # 2. Free user data
              cache.ops.free_cb(get_data(entry), user_data)
              
              # 3. Free entry memory
              lv_cache_entry_delete(entry)

- ``drop_all_cb``: Invalidates all entries in the cache.

  .. code-block:: python

      def drop_all_cb(cache, user_data):
          lru = (lv_cache_lru_ll_t *)cache
          
          # Iterate and remove all nodes
          # Note: Real implementation needs to handle safe iteration while removing
          node = lru.ll.head
          while node:
              next_node = node.next
              remove_cb(cache, get_entry(node), user_data)
              free_entry(get_entry(node))
              node = next_node

- ``reserve_cond_cb``: Checks if the cache can accept a new entry of a given size.
  For a simple count-based cache, this just checks if we hit the limit.

  .. code-block:: python

      def reserve_cond_cb(cache, key, reserved_size, user_data):
           if cache.size >= cache.max_size:
               return NEED_VICTIM # Cache full, need to evict
           return OK

- ``iter_create_cb``: Creates an iterator for the cache (optional but useful for 
  debugging).

  .. code-block:: python

      def iter_create_cb(cache):
          return lv_iter_create(cache, ...)

**4. Register the Class**

Finally, define the global class structure. This structure serves as the VTable
(Virtual Table) for your cache class.

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

Phase 2: Create a Cache Instance
--------------------------------

Now that we have a policy (Class), we need to define *what* we are caching (Instance).
Let's see how a custom cache for storing pre-calculated layout data would be created
using our new class.

**1. Define Data Structures**

You need a key to uniquely identify entries and a structure to hold the data.

.. code-block:: c

    // The unique key to identify a cache entry
    typedef struct {
        uint32_t layout_id;
        uint16_t width;
        uint16_t height;
    } my_layout_key_t;

    // Example data to be cached
    typedef struct {
        int32_t x;
        int32_t y;
    } my_pos_t;

    // The actual cached data payload
    typedef struct {
        my_layout_key_t key;
        my_pos_t position;
    } my_layout_data_t;

**2. Implement Instance Callbacks**

- ``compare_cb``: Used by the Class to check for equality.

  **Important**: In LVGL's cache implementation, the `compare_cb` always receives two 
  pointers
  to the **Data Structure** type (`my_layout_data_t *`).
  
  - The first argument (`k1`) is a pointer to an entry already stored in the cache.
  - The second argument (`k2`) is the search key provided by the user.
  
  Crucially, even if you are just searching, you must pass a pointer that *looks
  like* your data structure (at least the key fields must be valid). The cache
  does not distinguish between a "key type" and a "data type".

  .. code-block:: python

      def compare_cb(k1, k2):
          data1 = (my_layout_data_t *)k1
          data2 = (my_layout_data_t *)k2
          
          if data1.key.layout_id != data2.key.layout_id:
              return data1.key.layout_id - data2.key.layout_id
          if data1.key.width != data2.key.width:
              return data1.key.width - data2.key.width
          return data1.key.height - data2.key.height

- ``free_cb``: Called when an entry is permanently deleted (evicted or dropped).
  Since our data is stored directly in the struct, we don't need to free anything 
  here.
  If `my_pos_t` contained pointers, we would free them here.

  .. code-block:: python

      def free_cb(data, user_data):
          # Nothing to free for simple structs
          pass

**3. Initialize the Cache**

Create the cache instance at runtime, binding your data type to the cache class.

.. code-block:: c

    static lv_cache_t * layout_cache;

    void my_layout_cache_init(void) {
        layout_cache = lv_cache_create(
            &lv_cache_class_my_ll,         // Use our new Class (Count based)
            sizeof(my_layout_data_t),      // Size of one node (Header + Data)
            100,                           // Max capacity (e.g., 100 entries)
            (lv_cache_ops_t){
                .compare_cb = compare_cb,
                .create_cb = NULL,         // Optional: Used if you want cache to auto-create on miss
                .free_cb = free_cb
            }
        );
    }

Phase 3: Integration and Usage
------------------------------

Integrate the cache into your subsystem's workflow. This usually involves a
"Check Cache -> (Miss? Compute -> Add) -> Return" flow.

.. code-block:: python

    def get_layout_data(id, w, h):
        # Create a dummy data structure for searching
        search_record = my_layout_data_t()
        search_record.key.layout_id = id
        search_record.key.width = w
        search_record.key.height = h

        # 1. Try to acquire from cache
        entry = lv_cache_acquire(layout_cache, search_record, NULL)
        
        if entry:
            # HIT: Return cached data
            data = lv_cache_entry_get_data(entry)
            
            # Copy data to local variable
            result = data.position
            
            # Release the cache entry immediately since we have a copy
            lv_cache_release(layout_cache, entry, NULL)
            
            return result 

        # 2. MISS: Compute data
        result = heavy_layout_computation(id, w, h)

        # 3. Add to cache
        entry = lv_cache_add(layout_cache, search_record, NULL)
        if entry:
            data = lv_cache_entry_get_data(entry)
            
            # Fill in the data
            data.position = result
            
            # Release the reference we just got from 'add'
            lv_cache_release(layout_cache, entry, NULL)

        return result

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

3. **Stress Testing**:
    - Create a loop that adds thousands of random entries to force frequent evictions.
    - Ensure no memory leaks occur using address sanitizers (ASan).

Conclusion
**********

By following this guide, you have implemented a Linked List-based LRU cache
class. The `Class` handles the *mechanics* of storage and eviction, while the
`Instance` handles the *specifics* of your data.
