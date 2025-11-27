.. _cache_framework_overview:

Overview
========

1. Introduction
---------------

The LVGL Cache Framework is a general-purpose, efficient caching system designed to optimize resource usage and improve graphics rendering performance in embedded systems. It provides a flexible architecture that can be adapted to various caching needs, with a primary focus on image resource management.

2. Purpose and Benefits
-----------------------

The main purposes of the LVGL Cache Framework are:

- **Memory Optimization**: Efficiently manage limited memory resources in embedded systems
- **Performance Improvement**: Reduce the overhead of repeated resource loading and processing
- **Resource Management**: Provide a systematic approach to resource lifecycle management

Key benefits include:

- **Reduced CPU Usage**: Minimizes repeated decoding of resources
- **Lower Memory Fragmentation**: Controlled allocation and deallocation of resources
- **Improved Responsiveness**: Faster access to frequently used resources
- **Consistent Performance**: Predictable behavior under various memory conditions

3. Core Concepts
----------------

3.1 Caching Strategies
""""""""""""""""""""""

The LVGL Cache Framework implements two main caching strategies:

1. **Count-based LRU Cache (lv_cache_class_lru_rb_count)**: Limits the number of entries in the cache
2. **Size-based LRU Cache (lv_cache_class_lru_rb_size)**: Limits the total memory size occupied by the cache

Both strategies use the LRU (Least Recently Used) algorithm to decide which entries should be evicted when the cache is full.

3.2 Reference Counting Mechanism
""""""""""""""""""""""""""""""""

Cache entries use a reference counting mechanism to track how many clients are currently using the entry. When the reference count is zero, the entry can be safely removed from the cache. This mechanism ensures that even if an entry is removed from the cache, resources in use will not be released until all clients have released them.

3.3 Thread Safety
"""""""""""""""""

The cache framework implements thread safety through mutex locks, ensuring safe access and modification of cache data in multi-threaded environments. Each cache instance has a mutex lock that is acquired and released during cache operations.

4. Use Cases
------------

The LVGL Cache Framework is particularly useful in the following scenarios:

4.1 Image Caching
"""""""""""""""""

LVGL implements two types of image-related caches:

1. **Image Cache (lv_image_cache)**: Caches decoded image data, reducing the overhead of repeated decoding
2. **Image Header Cache (lv_image_header_cache)**: Caches image header information, such as size and format, avoiding repeated reading and parsing

4.2 Font Caching
""""""""""""""""

Caching font glyphs to avoid repeated rasterization of characters, especially beneficial for complex scripts or large font sizes.

4.3 Custom Resource Caching
"""""""""""""""""""""""""""

The framework's flexible design allows it to be extended to cache other types of resources, such as:

- Shader programs
- Geometry data
- Animation data
- Configuration data

5. Integration with LVGL
------------------------

The Cache Framework is deeply integrated with the LVGL graphics library, providing optimized resource management for various components:

- **Image Decoder**: Uses the cache to store decoded image data
- **Font Engine**: Can use the cache to store rasterized glyphs

6. Summary
----------

The LVGL Cache Framework provides a robust foundation for resource management in embedded graphics systems. Its modular design, efficient algorithms, and flexible architecture make it suitable for a wide range of applications, from simple image caching to complex resource management systems.

For more detailed information about the architecture and implementation, refer to the :doc:`architecture` and :doc:`implementation_details` sections.