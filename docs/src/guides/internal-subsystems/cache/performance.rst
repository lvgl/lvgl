.. _cache_framework_performance:



===========
Performance
===========



Introduction
************

This document provides a detailed analysis of the performance characteristics of the
LVGL Cache Framework, including time complexity, space complexity, and optimization
strategies. Understanding these aspects is crucial for effectively utilizing the cache
framework in resource-constrained embedded systems.



Time Complexity Analysis
************************



Core Operations (by Cache Class)
--------------------------------

RB-LRU (Red-Black Tree + Doubly Linked List):

+------------+-----------------+----------------------------------------------+
| Operation  | Time Complexity | Description                                  |
+============+=================+==============================================+
| Lookup     | O(log n)        | Red-black tree lookup                        |
+------------+-----------------+----------------------------------------------+
| Insertion  | O(log n)        | RB-tree insertion + list operations          |
+------------+-----------------+----------------------------------------------+
| Deletion   | O(log n)        | RB-tree deletion + list operations           |
+------------+-----------------+----------------------------------------------+
| LRU Update | O(1)            | Doubly linked list for head/tail adjustments |
+------------+-----------------+----------------------------------------------+

LL-LRU (Linked List Variant):

+------------+-----------------+-----------------------------------------------------+
| Operation  | Time Complexity | Description                                         |
+============+=================+=====================================================+
| Lookup     | O(n)            | Linear search                                       |
+------------+-----------------+-----------------------------------------------------+
| Insertion  | O(1)            | Insert at head (requires O(n) check for duplicates) |
+------------+-----------------+-----------------------------------------------------+
| Deletion   | O(1)            | Removing a known node                               |
+------------+-----------------+-----------------------------------------------------+
| LRU Update | O(1)            | Moving node to head                                 |
+------------+-----------------+-----------------------------------------------------+

The use of red-black trees for lookup operations ensures that the cache remains
efficient even as the number of entries grows. The O(log n) time complexity for
lookup, insertion, and deletion operations is significantly better than the O(n)
complexity that would be achieved with a simple linear search.



Cache Hit vs. Miss Performance
------------------------------

**Cache Hit Workflow**

+--------------+-------------------+-------------------+------------------------+
| Step         | RB-LRU Complexity | LL-LRU Complexity | Notes                  |
+==============+===================+===================+========================+
| 1. Lookup    | O(log n)          | O(n)              | Find entry by key      |
+--------------+-------------------+-------------------+------------------------+
| 2. LRU Update| O(1)              | O(1)              | Move to head           |
+--------------+-------------------+-------------------+------------------------+
| 3. Ref Count | O(1)              | O(1)              | Increment usage        |
+--------------+-------------------+-------------------+------------------------+
| **Total**    | **O(log n)**      | **O(n)**          |                        |
+--------------+-------------------+-------------------+------------------------+

**Cache Miss Workflow**

+--------------+-------------------+-------------------+------------------------+
| Step         | RB-LRU Complexity | LL-LRU Complexity | Notes                  |
+==============+===================+===================+========================+
| 1. Lookup    | O(log n)          | O(n)              | Confirm missing        |
+--------------+-------------------+-------------------+------------------------+
| 2. Creation  | Variable          | Variable          | Load/Render data       |
+--------------+-------------------+-------------------+------------------------+
| 3. Eviction  | O(log n) * k      | O(1) * k          | If full                |
|              |                   |                   | (k = num evicted)      |
+--------------+-------------------+-------------------+------------------------+
| 4. Insertion | O(log n)          | O(1)              | Add new entry          |
+--------------+-------------------+-------------------+------------------------+
| **Total**    | **O(log n) + C**  | **O(n) + C**      | C = Creation Cost      |
+--------------+-------------------+-------------------+------------------------+

The significant difference in performance between cache hits and misses highlights the
importance of proper cache sizing and strategy selection to maximize the hit rate.



Space Complexity Analysis
*************************



Memory Overhead
---------------

The memory overhead of the cache framework consists of the following components:

1. **Cache Instance**: Fixed size, independent of the number of entries
2. **Cache Entries**: Each entry requires memory for:

   - Entry structure (fixed size)
   - Red-black tree node (fixed size)
   - Linked list node (fixed size)
   - Cached data (variable size)

For a cache with n entries, the space complexity is:

- **Count-based Cache**: O(n), where n is the number of cache entries
- **Size-based Cache**: O(s), where s is the total size of the cache



Memory Efficiency Strategies
----------------------------

+----------------------+----------------------------------------------------------+
| Strategy             | Description                                              |
+======================+==========================================================+
| **Single Allocation**| One memory block for both entry structure and data       |
|                      | (reduces fragmentation).                                 |
+----------------------+----------------------------------------------------------+
| **Ref Counting**     | Frees resources only when unused (prevents leaks).       |
+----------------------+----------------------------------------------------------+
| **Eviction**         | Auto-removes LRU entries when full (bounds memory usage).|
+----------------------+----------------------------------------------------------+



Performance Benchmarks
**********************



Image Cache Performance
-----------------------

The following table shows typical performance metrics for the image cache in a
representative embedded system:

- CPU: 200MB ARM PSRAM SPEED
- PSRAM SPEED: 100MB/s

+----------------+-------------------+-------------------+
| Operation      | Average Time (ms) | Memory Usage (KB) |
+================+===================+===================+
| First Load     | 2-6               | Varies by image   |
+----------------+-------------------+-------------------+
| Cache Hit      | 0.002-0.006       | 0 (additional)    |
+----------------+-------------------+-------------------+
| Eviction       | 0.01-0.02         | -Varies by image  |
+----------------+-------------------+-------------------+

These benchmarks demonstrate the significant performance improvement achieved by
caching decoded images, with cache hits being 30-300 times faster than the initial
loading and decoding.



Optimization Strategies
***********************



Cache Strategy Selection
------------------------

+-------------------+-----------------------------------------------------------+
| Strategy          | Ideal Use Case                                            |
+===================+===========================================================+
| **Count-based**   | Resources have similar sizes; count is the limit.         |
+-------------------+-----------------------------------------------------------+
| **Size-based**    | Resources vary in size; total memory is the limit.        |
+-------------------+-----------------------------------------------------------+



Cache Size Tuning
-----------------

+----------------+-------------------------------------------------------------+
| Cache Size     | Effect                                                      |
+================+=============================================================+
| **Too Small**  | High eviction rate, low hit rate, poor performance.         |
+----------------+-------------------------------------------------------------+
| **Too Large**  | Wastes memory; diminishing returns on hit rate.             |
+----------------+-------------------------------------------------------------+
| **Optimal**    | Holds the "working set" of frequently accessed resources.   |
+----------------+-------------------------------------------------------------+

A common approach is to start with a moderate cache size and adjust based on
monitoring the hit rate and memory usage in the target application.



Custom Eviction Policies
------------------------

+-------------------+---------------------------------------------------------+
| Policy            | Description                                             |
+===================+=========================================================+
| **Priority**      | Evict based on assigned resource priority.              |
+-------------------+---------------------------------------------------------+
| **Size-aware**    | Evict larger items first to free more space.            |
+-------------------+---------------------------------------------------------+
| **Frequency**     | Evict based on access frequency (LFU).                  |
+-------------------+---------------------------------------------------------+

Implementing custom eviction policies requires creating a custom cache class with
specialized ``get_victim_cb`` and other relevant callbacks.



Preloading Strategies
---------------------

+-------------------------+----------------------------------------------------------+
| Strategy                | Description                                              |
+=========================+==========================================================+
| **Startup**             | Load common resources during initialization              |
|                         | (e.g., logo, main font).                                 |
+-------------------------+----------------------------------------------------------+
| **Background**          | Load resources when system is idle (prevents UI stutter).|
+-------------------------+----------------------------------------------------------+
| **Predictive**          | Load resources based on user behavior                    |
|                         | (e.g., next screen in a flow).                           |
+-------------------------+----------------------------------------------------------+



Development Tips
****************

+------------------------+--------------------------------------------+
| Tip                    | Actionable Advice                          |
+========================+============================================+
| **Callbacks**          | Keep `compare`, `create`, and `free` fast. |
+------------------------+--------------------------------------------+
| **Ref Counting**       | Release entries immediately when done.     |
+------------------------+--------------------------------------------+
| **Invalidation**       | Drop stale entries explicitly.             |
+------------------------+--------------------------------------------+
