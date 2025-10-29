.. _cache_framework_architecture:


============
Architecture
============



Overview
********

The LVGL Cache Framework employs a **modular architecture** based on the **Strategy
Pattern**. This design decouples the cache interface (usage) from the cache policy
(eviction algorithm and storage), allowing users to switch caching strategies without
changing the application logic.



Architectural Design
********************

The framework consists of three main layers:

1. **Public Interface**: The API used by the application 
   (:cpp:func:`lv_cache_acquire`, :cpp:func:`lv_cache_acquire_or_create`, etc.).
2. **Cache Instance** (`lv_cache_t`): The context holding the state of a specific
   cache (size, lock, nodes).
3. **Cache Class** (`lv_cache_class_t`): The implementation of a specific caching
   strategy (RB-LRU, SC-DA, etc.).



Component Relationships
-----------------------

The following class diagram illustrates how the cache instance delegates operations to
the cache class:

.. mermaid::
   :zoom:

   %%{init: {'theme':'neutral'}}%%
   classDiagram
       class lv_cache_t {
           +const lv_cache_class_t* clz
           +uint32_t node_size
           +uint32_t max_size
           +uint32_t size
           +lv_cache_ops_t ops
           +lv_mutex_t lock
           +const char* name
       }

       class lv_cache_entry_t {
           +const lv_cache_t* cache
           +int32_t ref_cnt
           +uint32_t node_size
           +bool is_invalid
       }

       class lv_cache_class_t {
           +alloc_cb()
           +init_cb()
           +destroy_cb()
           +get_cb()
           +add_cb()
           +remove_cb()
           +drop_cb()
           +drop_all_cb()
           +get_victim_cb()
           +reserve_cond_cb()
           +iter_create_cb()
       }

       class ConcreteStrategy {
           +lv_cache_t cache
           +SpecificDataStructure ds
       }

       lv_cache_t "1" *-- "1" lv_cache_class_t : uses
       lv_cache_t "1" *-- "*" lv_cache_entry_t : manages
       ConcreteStrategy ..|> lv_cache_class_t : implements
       ConcreteStrategy --|> lv_cache_t : extends

Note: `ConcreteStrategy` represents implementations like `lv_lru_rb_t`.



Core Components
***************



Cache Instance (lv_cache_t)
---------------------------

The **Cache Instance** is the handle used by the application. It maintains the
thread-safety mechanisms (mutex) and general statistics (current size), but it does
not know *how* data is stored or evicted. It delegates these tasks to the **Cache
Class**.



Cache Entry (lv_cache_entry_t)
------------------------------

The **Cache Entry** is the fundamental unit of storage. It wraps the actual cached
data with metadata required for management:

- **Reference Counting**: Ensures data stays alive while in use (`ref_cnt`).
- **Lifecycle Management**: Tracks validity and ownership.

Cache Class (lv_cache_class_t)
------------------------------

The **Cache Class** defines the *policy*. It is a vtable of function pointers that
implement:

- **Storage**: How to store and look up data (e.g., Red-Black Tree, Hash Map).
- **Eviction**: Which item to remove when full (e.g., LRU, FIFO, Random).



Interaction Flows
*****************

The following sequence diagrams illustrate the **interaction protocol** between the
Client, the Cache Instance, and the Cache Class. This defines the behavioral contract
of the Strategy Pattern.



Cache Creation
--------------

The creation process initializes the cache instance and binds it to a specific
strategy (Class).

1. **API Call**: The user calls `lv_cache_create` with the desired cache class (e.g.,
   `lv_cache_class_lru_rb_count`) and configuration (node size, max size).
2. **Allocation**: The `alloc_cb` of the cache class is invoked to allocate memory for
   the specific cache implementation (e.g., `lv_lru_rb_t`).
3. **Initialization**: The `init_cb` is called to set up internal structures (e.g.,
   initializing the Red-Black tree and Linked List).
4.  **Locking**: A mutex is initialized for thread safety.
5.  **Return**: The fully initialized `lv_cache_t` handle is returned to the user.

.. mermaid::
   :zoom:

   %%{init: {'theme':'neutral'}}%%
   sequenceDiagram
       participant Client
       participant Cache as lv_cache_create
       participant Class as lv_cache_class

       Client->>Cache: lv_cache_create(cache_class, node_size, max_size, ops)
       Cache->>Class: alloc_cb()
       Class-->>Cache: cache instance
       Cache->>Cache: Set parameters(node_size, max_size, ops)
       Cache->>Class: init_cb(cache)
       Class-->>Cache: Initialization result
       Cache->>Cache: lv_mutex_init(&cache->lock)
       Cache-->>Client: cache instance



Cache Acquisition
-----------------

Retrieving an item involves locking the cache, querying the strategy, and updating
reference counts if found.

1. **Lock**: The cache mutex is locked to ensure exclusive access.
2. **Lookup**: The `get_cb` of the cache class is called to find the entry matching
   the key.

   * *Implementation detail*: For LRU, this also moves the found item to the head of
     the list.

3. **Hit**: If the entry is found, its reference count is incremented via
   `lv_cache_entry_acquire_data`.
4. **Miss**: If not found, `NULL` is returned.
5. **Unlock**: The mutex is released.

.. mermaid::
   :zoom:

   %%{init: {'theme':'neutral'}}%%
   sequenceDiagram
       participant Client
       participant Cache as lv_cache_acquire
       participant Class as cache->clz

       Client->>Cache: lv_cache_acquire(cache, key, user_data)
       Cache->>Cache: lv_mutex_lock(&cache->lock)
       alt cache->size == 0
           Cache-->>Client: NULL (Cache is empty)
       else
           Cache->>Class: get_cb(cache, key, user_data)
           Class-->>Cache: entry or NULL
           alt entry != NULL
               Cache->>Cache: lv_cache_entry_acquire_data(entry)
           end
       end
       Cache->>Cache: lv_mutex_unlock(&cache->lock)
       Cache-->>Client: entry or NULL



Acquire or Create
-----------------

This is the most common pattern: "Get if exists, otherwise create and add".

**Logical Flow**:

1. **Check**: Attempt to retrieve the item from the cache.
2. **Hit**: If found, increment usage count and return it.
3. **Miss**:

   * Create the new data (load image, render font, etc.).
   * Add it to the cache.
   * If addition fails (e.g., too large), clean up the newly created data.

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

**API Call Flow**:

This diagram maps the logical steps to specific LVGL API calls and callbacks.

1. `lv_cache_acquire_or_create` first calls `lv_cache_acquire`.
2. If that returns `NULL`, it proceeds to `lv_cache_add`.
3. Inside `lv_cache_add`, the user-provided `create_cb` is called to generate the
   data.
4. If the add operation fails later (e.g., cache full and can't evict),
   `free_cb` is called to destroy the just-created data.

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



Cache Addition
--------------

Adding an item requires checking limits, potentially evicting victims (Loop), and then
inserting the new data.

1. **Lock**: Acquire the cache mutex.
2. **Space Check**: Call `reserve_cond_cb` to see if there is room for the new item.
3. **Eviction Loop**:

    * If the cache is full (`LV_CACHE_RESERVE_COND_NEED_VICTIM`), enter a loop.
    * Call `cache_evict_one_internal_no_lock` to remove the least important item
      (e.g., LRU tail).
    * Check space again. Repeat until space is available or eviction fails.

4. **Add**: Once space is reserved, call `add_cb` to insert the new item into the
   internal structure.
5. **Acquire**: Increment the reference count of the new entry (since the caller
   intends to use it immediately).
6. **Unlock**: Release the mutex.

.. mermaid::
   :zoom:

   %%{init: {'theme':'neutral'}}%%
   sequenceDiagram
       participant Client
       participant Cache as lv_cache_add
       participant Internal as cache_add_internal_no_lock
       participant Class as cache->clz

       Client->>Cache: lv_cache_add(cache, key, user_data)
       Cache->>Cache: lv_mutex_lock(&cache->lock)
       alt cache->max_size == 0
           Cache-->>Client: NULL (Cache is disabled)
       else
           Cache->>Internal: cache_add_internal_no_lock(cache, key, user_data)
           Internal->>Class: reserve_cond_cb(cache, key, 0, user_data)
           alt Result is LV_CACHE_RESERVE_COND_TOO_LARGE
               Internal-->>Cache: NULL (Data is too large)
           else Result is LV_CACHE_RESERVE_COND_NEED_VICTIM
               loop Until condition is met
                   Internal->>Internal: cache_evict_one_internal_no_lock(cache, user_data)
                   Internal->>Class: reserve_cond_cb(cache, key, 0, user_data)
               end
           end
           Internal->>Class: add_cb(cache, key, user_data)
           Class-->>Internal: entry
           Internal-->>Cache: entry
           alt entry != NULL
               Cache->>Cache: lv_cache_entry_acquire_data(entry)
           end
       end
       Cache->>Cache: lv_mutex_unlock(&cache->lock)
       Cache-->>Client: entry or NULL



Thread Safety
*************

The cache framework implements thread safety through mutex locks, ensuring safe access
and modification of cache data in multi-threaded environments. Each cache instance has
a mutex lock that is acquired and released during cache operations.

Key thread safety features include:

- **Mutex Lock**: Each cache instance has a mutex lock that is acquired before any
  operation that modifies the cache state
- **Reference Counting**: The reference counting mechanism ensures that resources are
  not released while in use, even if they are evicted from the cache



Usage Considerations
--------------------

When using the cache in a multi-threaded environment, keep the following in mind:

- **Cache Creation and Destruction**: Should be done in a thread-safe manner,
  typically during initialization and shutdown when no other threads are accessing the
  cache.
- **Cache Operations**: The core cache operations (acquire, add, release, drop) are
  thread-safe, as they acquire and release the cache's mutex lock.
- **User Callbacks**: User-provided callbacks (compare, create, free) should be
  thread-safe, as they may be called from multiple threads.
- **Cache Entry Data**: Access to the data associated with a cache entry should be
  synchronized if multiple threads may access it simultaneously.

For more detailed information about the implementation, refer to the
:doc:`implementation_details` section.
