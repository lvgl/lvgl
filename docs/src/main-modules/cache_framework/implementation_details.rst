.. _cache_framework_implementation_details:

Implementation Details
======================

Internal Data Structures
************************

LRU Implementation
------------------

The LRU strategy is built on two primary data structures:

1.  **Red-Black Tree (`lv_rb_t`)**: Provides `O(log n)` lookups by key.
2.  **Doubly Linked List (`lv_ll_t`)**: Maintains eviction order (`O(1)` updates).

.. code-block:: c

   typedef struct _lv_lru_rb_t {
       lv_cache_t cache;           /* Base class */
       lv_rb_t rb;                 /* Fast lookup */
       lv_ll_t ll;                 /* Usage history */
       lv_lru_rb_get_size_cb_t get_data_size_cb; 
   } lv_lru_rb_t;

Structure Visualization
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: text

                                                 ┏ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ┓ 
     ┏ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━          ┌ ─ ─ ─ ┐             
                 ┌ ─ ─ ─ ─ ─ ─ ─            ┃    ┃      Cache   insert     ┃ 
     ┃               RB Tree    │                     │Hitting│  head       
                 └ ─ ─ ─ ─ ─ ─ ─            ┃    ┃     ─ ─ ─ ─             ┃ 
     ┃      ┌─┬─┬─┬─┐                                  ┌─────┐              
         ┌──│◄│B│►│ │◀ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─┃─ ─ ╋ ─ ─▶│  B  │             ┃ 
     ┃   │  └─┴─┴─┴─┘                                  └──▲──┘              
         │       │                          ┃    ┃        │                ┃ 
     ┃   │       │                                     ┌──┴──┐              
         │       └──────┐                ┌ ─┃─ ─ ╋ ─ ─▶│  E  │             ┃ 
     ┃   ▼         ┌ ─ ─│─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ┐   └──▲──┘              
      ┌─┬─┬─┬─┐         ▼                │  ┃    ┃        │                ┃ 
     ┃│◄│A│►│ │◀ ─ ┘ ┌─┬─┬─┬─┐                     │   ┌──┴──┐              
      └─┴─┴─┴─┘  ┌───│◄│D│►│ │◀ ─ ─ ─ ─ ─│─ ╋ ┐  ┃  ─ ▶│  A  │ ┌ ─ ─ ─ ─ ┐ ┃ 
     ┃           │   └─┴─┴─┴─┘                         └──▲──┘     LRU      
                 │        │              │  ┃ │  ┃        │    │  Cache  │ ┃ 
     ┃           ▼        └──────┐                     ┌──┴──┐  ─ ─ ─ ─ ─   
              ┌─┬─┬─┬─┐          ▼       │  ┃ └ ─┃─ ─ ▶│  D  │             ┃ 
     ┃        │◄│C│►│ │◀ ─    ┌─┬─┬─┬─┐                └──▲──┘              
              └─┴─┴─┴─┘   │   │◄│E│►│ │◀ ┘  ┃    ┃        │                ┃ 
     ┃                        └─┴─┴─┴─┘                ┌──┴──┐              
                          │        │      ─ ╋ ─ ─┃─ ─ ▶│  C  │             ┃ 
     ┃                     ─ ─ ─ ─ ┼ ─ ─ ┘             └──▲──┘              
                                   ▼        ┃    ┃   ┌ ─ ─│─ ─ ┐           ┃ 
     ┃                          ┌─┬─┬─┬─┐              ┌──┴──┐              
                                │◄│F│►│ │◀ ─┃─ ─ ╋ ─ ┼▶│  F  │ │           ┃ 
     ┃                          └─┴─┴─┴─┘              └─────┘              
                                            ┃    ┃   └ ─ ─ ─ ─ ┘           ┃ 
     ┃                                                 remove               
                                            ┃    ┃      tail               ┃ 
     ┗ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━      ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━ ━

.. note::
    **Structure Components**

    *   **RB-Tree**: Used for fast `O(log n)` lookups by key.
    *   **Doubly Linked List**: Tracks usage order. MRU (Most Recently Used) is at the 
        head.
    *   **Data**: The actual cached content, referenced by both structures.

Workflow
~~~~~~~~

1. **Access (Hit)**: Node is found via RB-Tree -> Moved to List Head.
2. **Add (New)**: Node inserted into RB-Tree -> Added to List Head -> If full,
   Tail removed.
3. **Eviction**: Tail node removed from List & RB-Tree.

Memory Layout
*************

Cache entries are allocated as a single contiguous block containing both the user
data and the cache header (`lv_cache_entry_t`). This improves cache locality and
reduces fragmentation.

.. code-block:: text

   LSB                                            MSB
   +-------------------------+----------------------+
   | User Data (Variable)    | Header (Fixed Size)  |
   +-------------------------+----------------------+
   ^                         ^
   |                         |
   Entry Data Pointer        lv_cache_entry_t*

Reference Counting
******************

The framework uses reference counting to manage the lifecycle of cache entries.
This ensures that resources (like textures or font bitmaps) are not freed while
they are still being used by the GPU or a draw unit, even if they have been
evicted from the cache.

- **Acquire (`ref_cnt++`)**: Called when an entry is retrieved from the cache (Hit)
  or created (Miss).
- **Release (`ref_cnt--`)**: Called when the consumer (e.g., a draw unit) is done
  with the data.
- **Free**: The actual memory is freed only when `ref_cnt == 0` **AND** the entry
  has been marked as invalid (evicted or dropped).

.. code-block:: python

   # Pseudo-code logic
   def release(entry):
       entry.ref_cnt -= 1
       if entry.ref_cnt == 0 and entry.is_invalid:
           free_memory(entry)

Specific Implementations
************************

Count-based LRU (`lv_cache_class_lru_rb_count`)
-----------------------------------------------

Limits the cache by **entry count**.

- **Eviction Condition**: `cache->size + 1 > cache->max_size` (Triggered when
  cache is full).
- **Use Case**: When all entries consume roughly similar memory, or when the number
  of open handles is the limiting factor (e.g., file descriptors).

Size-based LRU (`lv_cache_class_lru_rb_size`)
---------------------------------------------

Limits the cache by **total memory usage (bytes)**.

- **Eviction Condition**: `cache->size + new_entry_size > cache->max_size`.
- **Use Case**: General purpose asset caching (images, fonts) where file sizes vary
  significantly.

.. note::
   Size-based caches require a callback `get_data_size_cb` to determine the size of 
   each entry.

Linked List LRU (LL-LRU)
------------------------

A lighter variant that uses *only* a linked list, dropping the Red-Black Tree.

- **Pros**: Lower memory overhead per entry (no RB-node).
- **Cons**: `O(n)` lookup time.
- **Use Case**: Very small caches (e.g., < 10 items) where linear search is
  faster than tree traversal overhead.
