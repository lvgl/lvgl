.. _cache_framework_performance:

Performance
===========

1. Introduction
---------------

This document provides a detailed analysis of the performance characteristics of the LVGL Cache Framework, including
time complexity, space complexity, and optimization strategies. Understanding these aspects is crucial for effectively
utilizing the cache framework in resource-constrained embedded systems.

2. Time Complexity Analysis
---------------------------

2.1 Core Operations
~~~~~~~~~~~~~~~~~~~

+----------------+-----------------+--------------------------------------------------------------+
| Operation      | Time Complexity | Description                                                  |
+================+=================+==============================================================+
| Lookup         | O(log n)        | Uses red-black trees for efficient lookup                    |
+----------------+-----------------+--------------------------------------------------------------+
| Insertion      | O(log n)        | Includes red-black tree insertion and linked list operations |
+----------------+-----------------+--------------------------------------------------------------+
| Deletion       | O(log n)        | Includes red-black tree deletion and linked list operations  |
+----------------+-----------------+--------------------------------------------------------------+
| LRU Update     | O(1)            | Uses doubly linked lists for constant-time LRU updates       |
+----------------+-----------------+--------------------------------------------------------------+

The use of red-black trees for lookup operations ensures that the cache remains efficient even as the number of entries
grows. The O(log n) time complexity for lookup, insertion, and deletion operations is significantly better than the O(n)
complexity that would be achieved with a simple linear search.

2.2 Cache Hit vs. Miss Performance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Cache Hit**: When a requested resource is found in the cache, the operation is very efficient:

1. Lookup in the red-black tree: O(log n)
2. Update the LRU list: O(1)
3. Increment reference count: O(1)

Total time complexity for a cache hit: O(log n)

**Cache Miss**: When a requested resource is not found in the cache, additional operations are required:

1. Lookup in the red-black tree: O(log n)
2. Resource loading/creation: Varies depending on the resource
3. Potential eviction of existing entries: O(log n) per evicted entry
4. Insertion into the cache: O(log n)

Total time complexity for a cache miss: O(log n) + resource loading time + eviction time

The significant difference in performance between cache hits and misses highlights the importance of proper cache sizing
and strategy selection to maximize the hit rate.

3. Space Complexity Analysis
----------------------------

3.1 Memory Overhead
~~~~~~~~~~~~~~~~~~~

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

3.2 Memory Efficiency Strategies
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The cache framework employs several strategies to maximize memory efficiency:

1. **Single Allocation**: Each cache entry uses a single memory allocation for both the entry structure and the cached
   data, reducing memory fragmentation
2. **Reference Counting**: Ensures that resources are only freed when they are no longer in use, preventing memory leaks
3. **Eviction Policies**: Automatically removes least recently used entries when the cache is full, maintaining a
   bounded memory footprint

4. Performance Benchmarks
-------------------------

4.1 Image Cache Performance
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following table shows typical performance metrics for the image cache in a representative embedded system:

+----------------+-------------------+-------------------+
| Operation      | Average Time (ms) | Memory Usage (KB) |
+================+===================+===================+
| First Load     | 2-6               | Varies by image   |
+----------------+-------------------+-------------------+
| Cache Hit      | 0.002-0.006       | 0 (additional)    |
+----------------+-------------------+-------------------+
| Eviction       | 0.01-0.02         | -Varies by image  |
+----------------+-------------------+-------------------+

These benchmarks demonstrate the significant performance improvement achieved by caching decoded images, with cache hits
being 30-300 times faster than the initial loading and decoding.

4.2 Cache Size vs. Hit Rate
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The relationship between cache size and hit rate is typically non-linear, following a curve similar to::

    Hit Rate (%)  ^
                 |                 ****
                 |              ***
                 |            **
                 |          **
                 |        **
                 |      **
                 |    **
                 |  **
                 |**
                 +-------------------------> Cache Size

This curve demonstrates the law of diminishing returns: increasing the cache size initially provides significant
improvements in hit rate, but the benefits diminish as the cache size grows further.

Optimal cache sizing depends on the specific application and usage patterns, but a common guideline is to size the cache
to hold the working set of resources that are frequently accessed.

5. Optimization Strategies
--------------------------

5.1 Cache Strategy Selection
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Choosing the appropriate cache strategy is crucial for optimal performance:

- **Count-based LRU Cache**: Best for scenarios where all resources have similar sizes and the number of resources is
  the limiting factor
- **Size-based LRU Cache**: Best for scenarios where resources have varying sizes and the total memory usage is the
  limiting factor

5.2 Cache Size Tuning
~~~~~~~~~~~~~~~~~~~~~

Tuning the cache size involves finding the balance between memory usage and performance:

1. **Too Small**: Results in frequent evictions and low hit rates, degrading performance
2. **Too Large**: Wastes memory without providing significant performance benefits
3. **Optimal**: Large enough to hold the working set of frequently accessed resources

A common approach is to start with a moderate cache size and adjust based on monitoring the hit rate and memory usage in
the target application.

5.3 Custom Eviction Policies
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

While the default LRU eviction policy works well for most cases, custom eviction policies can be implemented for
specific use cases:

1. **Priority-based**: Assign priorities to different types of resources
2. **Size-aware**: Prefer evicting larger resources when memory is constrained
3. **Frequency-based**: Consider both recency and frequency of access (LFU or LRFU)

Implementing custom eviction policies requires creating a custom cache class with specialized ``get_victim_cb`` and other
relevant callbacks.

5.4 Preloading Strategies
~~~~~~~~~~~~~~~~~~~~~~~~~

Preloading frequently used resources can improve the user experience by avoiding cache misses during critical
operations:

1. **Startup Preloading**: Load common resources during application initialization

.. code-block:: c

    void preload_common_images(void)
    {
        for(int i = 0; i < common_images_count; i++) {
            lv_image_decoder_dsc_t dsc;
            lv_image_decoder_open(&dsc, common_images[i], NULL);
            lv_image_decoder_close(&dsc);
        }
    }

2. **Background Preloading**: Load resources in the background when the system is idle
3. **Predictive Preloading**: Load resources that are likely to be needed based on user behavior

6. Summary
----------

The LVGL Cache Framework provides efficient resource management with logarithmic time complexity for key operations and
linear space complexity. By understanding the performance characteristics and applying appropriate optimization
strategies, developers can achieve optimal performance while efficiently managing memory resources in embedded graphics
systems.

Key takeaways:

1. **Efficient Algorithms**: The use of red-black trees and linked lists provides O(log n) lookup and O(1) LRU updates
2. **Memory Efficiency**: Single allocations and reference counting minimize memory fragmentation
3. **Flexible Strategies**: Different cache classes support various use cases and resource characteristics
4. **Tuning Opportunities**: Cache size, strategy selection, and custom policies offer ways to optimize for specific
   applications

By leveraging these performance characteristics and optimization strategies, developers can create responsive and
memory-efficient applications using the LVGL Cache Framework.