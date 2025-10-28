# LVGL GDB Scripts - Misc Module Extensions

This directory contains GDB debugging scripts for LVGL's miscellaneous data structures, including red-black trees and
cache systems.

## Objects

### 1. lv_rb.py - Red-Black Tree Support

Provides debugging support for LVGL's red-black tree (`lv_rb_t`) implementation.

**Classes:**

- `LVRedBlackTree` - Iterator for red-black tree nodes

**Functions:**

- `dump_rb_info(rb, datatype)` - Dump basic red-black tree information

**Usage:**

```gdbscript
# In GDB:
python tree = LVRedBlackTree(some_rb_tree)
python for data in tree: print(data)
python dump_rb_info(some_rb_tree, datatype)
```

### 2. lv_cache.py - Cache System Support

Provides debugging support for LVGL's cache system (`lv_cache_t`).

**Classes:**

- `LVCache` - Wrapper for cache objects

**Functions:**

- `dump_cache_info(cache, datatype)` - Dump cache information

**Usage:**

```gdbscript
# In GDB:
python cache = LVCache(some_cache)
python print(f"Cache usage: {cache.get_usage_percentage()}%")
python dump_cache_info(some_cache, datatype)
```

### 3. lv_cache_entry.py - Cache Entry Support

Provides debugging support for cache entries (`lv_cache_entry_t`).

**Classes:**

- `LVCacheEntry` - Wrapper for cache entries

**Functions:**

- `dump_cache_entry_info(entry, datatype)` - Dump cache entry information

**Usage:**

```gdbscript
# In GDB:
python entry = LVCacheEntry(some_entry)
python print(f"Ref count: {entry.get_ref_count()}")
python dump_cache_entry_info(some_entry, datatype)
```

### 4. lv_cache_lru_rb.py - LRU Cache with RB Tree Support

Specialized support for LRU-based caches using red-black trees.

**Classes:**

- `LVCacheLRURB` - Specialized cache wrapper for LRU RB implementations

**Functions:**

- `dump_lru_rb_cache_info(cache)` - Dump LRU RB cache information

### 5. lv_image_cache.py and lv_image_header_cache.py - Image Cache Support

Provides debugging support for LVGL's image cache (`lv_image_cache_t`).

**Classes:**

- `LVImageCache` - Wrapper for image cache objects

**Usage:**

```gdbscript
# In GDB:
python curr_inst().image_cache().print_info()
python curr_inst().image_cache().print_entries()
```

## Integration

All these modules are automatically imported and available when using the LVGL GDB scripts. You can access them through
the main `lvgl` module:

```gdbscript
# In GDB:
python from lvglgdb import LVRedBlackTree, LVCache, dump_rb_info, dump_cache_info
```

## Type Conversion Support

All wrapper classes support multiple input types:

- `gdb.Value` objects (from GDB expressions)
- `Value` objects (from lvglgdb.value module)
- Raw pointers as integers (e.g., `0x12345678`)

This allows flexible usage patterns:

```gdbscript
# From GDB variable
entry = LVCacheEntry(my_entry)

# From raw memory address
entry = LVCacheEntry(0x7fff12345678)

# From GDB expression result
entry = LVCacheEntry(gdb.parse_and_eval("*(lv_cache_entry_t*)0x7fff12345678"))
```

## Examples

### Debugging a Red-Black Tree

```gdbscript
# Create tree iterator from variable
tree = LVRedBlackTree(some_rb_variable)

# Create from raw pointer (0x12345678)
tree = LVRedBlackTree(0x12345678)

# Iterate through all nodes
for data_ptr in tree:
    print(f"Data pointer: {data_ptr}")

# Get tree info
print(f"Tree has {len(tree)} nodes")
tree.print_info()
```

### Debugging a Cache

```gdbscript
# Create cache wrapper from variable
cache = LVCache(some_cache_variable)

# Create from raw pointer (0x12345678)
cache = LVCache(0x12345678)

# Check cache status
print(f"Cache usage: {cache.get_usage_percentage():.1f}%")
print(f"Cache enabled: {cache.is_enabled()}")

# Dump full info
cache.print_info()
```

### Debugging Cache Entries

```gdbscript
# Create entry wrapper from variable
entry = LVCacheEntry(some_entry_variable)

# Create from raw pointer (0x12345678)
entry = LVCacheEntry(0x12345678)

# Check entry status
print(f"Ref count: {entry.get_ref_count()}")
print(f"Invalid: {entry.is_invalid()}")

# Dump full info
entry.print_info()
```

## File Structure

- `lv_rb.py` - Red-black tree debugging support
- `lv_cache.py` - General cache debugging support
- `lv_cache_entry.py` - Cache entry debugging support
- `lv_cache_lru_rb.py` - LRU RB cache specialized support
- `lv_image_cache.py` - Image cache debugging support
- `lv_image_header_cache.py` - Image header cache debugging support
- `lv_cache_iter_base.py` - Cache iterator base class
- `lv_cache_iter_factory.py` - Cache iterator factory functions
- `__init__.py` - Module exports and initialization
