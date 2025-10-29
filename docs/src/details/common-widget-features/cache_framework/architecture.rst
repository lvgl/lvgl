.. _cache_framework_architecture:

Architecture
============

1. Overview
-----------

The LVGL Cache Framework is designed with a modular architecture that separates the cache behavior from its implementation. This design allows for flexibility in adapting the cache to different use cases while maintaining a consistent interface.

2. Core Components
------------------

The LVGL Cache Framework consists of the following core components:

2.1 Cache Instance (lv_cache_t)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The main body of the cache, responsible for managing the addition, retrieval, and eviction of cache entries. Key properties include:

- **Class Pointer**: Points to the cache class that defines the behavior
- **Node Size**: Size of each cache entry node
- **Maximum Size**: Maximum allowed size of the cache
- **Current Size**: Current size of the cache
- **Operations**: Callback functions for comparing, creating, and freeing cache entries
- **Mutex**: Lock for thread safety
- **Name**: Identifier for the cache instance

2.2 Cache Entry (lv_cache_entry_t)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Data units stored in the cache, containing:

- **Cache Pointer**: Reference to the parent cache
- **Reference Count**: Number of clients currently using the entry
- **Node Size**: Size of the entry node
- **Validity Flag**: Indicates whether the entry is still valid
- **Data**: The actual cached data (follows the entry structure in memory)

2.3 Cache Class (lv_cache_class_t)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Defines the behavior of the cache through a set of callback functions:

- **alloc_cb**: Allocates memory for a new cache instance
- **init_cb**: Initializes a cache instance
- **destroy_cb**: Cleans up and destroys a cache instance
- **get_cb**: Retrieves an entry from the cache
- **add_cb**: Adds a new entry to the cache
- **remove_cb**: Removes an entry from the cache
- **drop_cb**: Drops an entry from the cache (marks as invalid)
- **drop_all_cb**: Drops all entries from the cache
- **get_victim_cb**: Selects an entry for eviction
- **reserve_cond_cb**: Checks if there's enough space for a new entry
- **iter_create_cb**: Creates an iterator for the cache

2.4 Data Structures
~~~~~~~~~~~~~~~~~~~

The LVGL LRU cache implementation uses a combination of:

- **Red-Black Tree (lv_rb_t)**: Used for fast lookup of cache entries, with a time complexity of O(log n)
- **Doubly Linked List (lv_ll_t)**: Used to maintain the order of cache entry usage, with the most recently used entries at the head of the list and the least recently used entries at the tail

3. Architecture Diagram
------------------------

The following diagram illustrates the relationships between the core components of the LVGL Cache Framework:

.. mermaid::
   :zoom:

   %%{init: {'theme':'neutral'}}%%
   classDiagram
       class lv_cache {
           +const lv_cache_class_t* clz
           +uint32_t node_size
           +uint32_t max_size
           +uint32_t size
           +lv_cache_ops_t ops
           +lv_mutex_t lock
           +const char* name
       }
       
       class lv_cache_entry {
           +const lv_cache_t* cache
           +int32_t ref_cnt
           +uint32_t node_size
           +bool is_invalid
       }
       
       class lv_cache_class {
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
       
       class lv_lru_rb {
           +lv_cache_t cache
           +lv_rb_t rb
           +lv_ll_t ll
           +get_data_size_cb_t* get_data_size_cb
       }

       lv_cache_class <|-- lv_lru_rb : implements
       lv_cache "1" *-- "*" lv_cache_entry : manages
       lv_cache "1" *-- "1" lv_cache_class : uses

4. Data Flow
------------

The following diagrams illustrate the data flow for key operations in the cache framework:

4.1 Cache Creation Workflow
~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

4.2 Cache Acquisition Workflow
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

4.3 Cache Addition Workflow
~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

5. LRU Implementation
----------------------

5.1 Data Structures
~~~~~~~~~~~~~~~~~~~

The LVGL LRU cache implementation uses a combination of red-black trees and doubly linked lists:

- **Red-Black Tree**: Used for fast lookup of cache entries, with a time complexity of O(log n)
- **Doubly Linked List**: Used to maintain the order of cache entry usage, with the most recently used entries at the head of the list and the least recently used entries at the tail

5.2 LRU State Machine
~~~~~~~~~~~~~~~~~~~~~~

.. mermaid::
   :zoom:

   %%{init: {'theme':'neutral'}}%%
   stateDiagram-v2
       [*] --> Empty_Cache: Initialize
       Empty_Cache --> Has_Cache: Add Entry
       Has_Cache --> Has_Cache: Access Entry (Move to List Head) \n| Add Entry (Move to List Head) \n| Release Entry (Decrease Reference Count)
       Has_Cache --> Empty_Cache: Clear Cache
       Has_Cache --> [*]: Destroy Cache
       Empty_Cache --> [*]: Destroy Cache
       
       state Has_Cache {
           [*] --> Not_Full
           Not_Full --> Full: Add Entries Until Reaching Limit
           Full --> Not_Full: Evict Entry
           Full --> Full: Add Entry (Trigger Eviction)
       }

6. Thread Safety
----------------

The cache framework implements thread safety through mutex locks, ensuring safe access and modification of cache data in multi-threaded environments. Each cache instance has a mutex lock that is acquired and released during cache operations.

Key thread safety features include:

- **Mutex Lock**: Each cache instance has a mutex lock that is acquired before any operation that modifies the cache state
- **Reference Counting**: The reference counting mechanism ensures that resources are not released while in use, even if they are evicted from the cache
- **Atomic Operations**: Critical operations are performed atomically to prevent race conditions

7. Summary
----------

The LVGL Cache Framework's architecture is designed to be flexible, efficient, and thread-safe. Its modular design allows for different caching strategies and eviction algorithms, while its core components provide a consistent interface for cache operations. The use of red-black trees and linked lists ensures efficient lookup and LRU management, making it suitable for resource-constrained embedded systems.

For more detailed information about the implementation and API, refer to the :doc:`implementation_details` and :doc:`api_reference` sections.