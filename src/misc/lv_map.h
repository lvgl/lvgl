/**
 * @file lv_map.h
 * Map. The map entries are dynamically allocated by the 'lv_mem' module.
 */

#ifndef LV_MAP_H
#define LV_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**
 * Helper macro to iterate on each @ref lv_map_entry_t of a @ref lv_map_t.
 *
 * The `_it` entry should not be removed from the map; use @ref map_for_each_safe()
 * instead if you need to call @ref lv_map_remove() on `_it`.
 */
#define map_for_each(_map, _it)       \
    for(_it = lv_map_begin(_map);     \
        _it != lv_map_end(_map);      \
        _it = lv_map_entry_next(_it))

/**
 * Helper macro to iterate on each @ref lv_map_entry_t of a @ref lv_map_t.
 *
 * Unlike @ref map_for_each(), it is safe to call @ref lv_map_remove() on `_it`.
 */
#define map_for_each_safe(_map, _it, _next)                       \
    for(_it = lv_map_begin(_map), _next = lv_map_entry_next(_it); \
        _it != lv_map_end(_map);                                  \
        _it = _next, _next = lv_map_entry_next(_it))

/**
 * Helper macro to iterate on each @ref lv_map_entry_t of a @ref lv_map_t in reverse order.
 *
 * The `_it` entry should not be removed from the map; use @ref map_for_each_reverse_safe()
 * instead if you need to call @ref lv_map_remove() on `_it`.
 */
#define map_for_each_reverse(_map, _it) \
    for(_it = lv_map_rbegin(_map);      \
        _it != lv_map_rend(_map);       \
        _it = lv_map_entry_prev(_it))

/**
 * Helper macro to iterate on each @ref lv_map_entry_t of a @ref lv_map_t in reverse order.
 *
 * Unlike @ref map_for_each_reverse(), it is safe to call @ref lv_map_remove() on `_it`.
 */
#define map_for_each_reverse_safe(_map, _it, _prev)                     \
    for(_it = lv_map_rbegin(_map), _prev = lv_map_entry_prev(_it);      \
        _it != lv_map_rend(_map);                                       \
        _it = _prev, _prev = lv_map_entry_prev(_it))

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Node in a doubly linked list
 */
typedef struct _lv_link_t {
    struct _lv_link_t * prev; /**< Pointer to the previous node in the doubly linked list */
    struct _lv_link_t * next; /**< Pointer to the next node in the doubly linked list */
} lv_link_t;

/**
 * Dummy type to represent keys in @ref lv_map_entry_t
 */
typedef void lv_map_key_t;

/**
 * Dummy type to represent values in @ref lv_map_entry_t
 */
typedef void lv_map_value_t;

/**
 * Set the common attributes and callbacks shared by all map instances of the same class
 */
typedef struct _lv_map_class_t {
    /**
     * The size in bytes of all keys for this class
     */
    uint32_t key_size;

    /**
     * The size in bytes of all values for this class
     */
    uint32_t value_size;

    /**
     * Optional callback to allocate a @ref lv_map_entry_t : if NULL, use lv_malloc()
     */
    void * (*malloc_cb)(size_t size);

    /**
     * Optional callback to free a @ref lv_map_entry_t : if NULL, use lv_free()
     */
    void (*free_cb)(void * data);

    /**
     * Mandatory callback to compare two keys when sorting @ref lv_map_entry_t
     *
     * Called from @ref lv_map_find() and @ref lv_map_insert().
     */
    int (*key_comp_cb)(const lv_map_key_t * key1, const lv_map_key_t * key2);

    /**
     * Mandatory callback to copy one key into another
     */
    void (*key_copy_cb)(lv_map_key_t * dst_key, const lv_map_key_t * src_key);

    /**
     * Optional callback to destroy a key: if NULL, do nothing
     */
    void (*key_destructor_cb)(lv_map_key_t * key);

    /**
     * Mandatory callback to copy one value into another
     *
     * Called from @ref lv_map_entry_set_value().
     */
    void (*value_copy_cb)(lv_map_value_t * dst_value, const lv_map_value_t * src_value);

    /**
     * Optional callback to destroy a value: if NULL, do nothing
     */
    void (*value_destructor_cb)(lv_map_value_t * value);
} lv_map_class_t;

struct _lv_map_entry_t;

/**
 * Opaque structure that stores a {@ref lv_map_key_t "key", @ref lv_map_value_t "value"} pair within a @ref lv_map_t
 */
typedef struct _lv_map_entry_t lv_map_entry_t;

/**
 * Associative container storing {@ref lv_map_key_t "key", @ref lv_map_value_t "value"} pairs.
 *
 * Pairs are sorted by key using the @ref lv_map_class_t::key_comp_cb() callback to allow fast,
 * actually O(log n), lookup, insertion and deletion of pairs in the map.
 *
 * @ref lv_map_entry_t nodes are allocated either with the @ref lv_map_class_t::malloc_cb() callback
 * if not NULL or with lv_malloc(). In both cases, the `size` argument is the value returned by the
 * @ref lv_map_get_entry_size() function.
 */
typedef struct _lv_map_t {
    const lv_map_class_t * class_p; /**< The pointer to the class this map is instantiating */
    lv_map_entry_t * root; /**< The root of the red-black tree implementing the map */
    lv_link_t end; /**< The sentinel node for the doubly linked list used to quickly iterate on map entries with `map_for_each*` helper macros */
} lv_map_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get the size in bytes of a @ref lv_map_entry_t for the given @ref lv_map_class_t
 *
 * This function is called internally when a @ref lv_map_entry_t must be allocated.
 * It is public to give a hint of the value of `size` argument that will be passed
 * to the @ref lv_map_class_t::malloc_cb() callback, if any, when users want to provided
 * their own allocator.
 * @param class_p pointer to the @ref lv_map_class_t class the map will be instantiated
 *                from.
 * @return the size in bytes of any @ref lv_map_entry_t for this class.
 */
uint32_t lv_map_get_entry_size(const lv_map_class_t * class_p);

/**
 * Allocate then initialize a new @ref lv_map_t instance of the given @ref lv_map_class_t
 *
 * The @ref lv_map_t instance is allocated with lv_malloc() then initialized with
 * @ref lv_map_init().
 * @param class_p pointer to the @ref lv_map_class_t class the @ref lv_map_t is instantiated from.
 * @return a pointer to the new @ref lv_map_t instance.
 */
lv_map_t * lv_map_create(const lv_map_class_t * class_p);

/**
 * Clear then free a map
 *
 * The @p map is first cleared by @ref lv_map_clear() then freed with lv_free().
 * @param map a pointer to the map to delete.
 */
void lv_map_delete(lv_map_t * map);

/**
 * Initialize a @ref lv_map_t
 *
 * @param map a pointer to the map to initialize.
 * @param class_p pointer to the @ref lv_map_class_t class the @p map belongs to.
 */
void lv_map_init(lv_map_t * map, const lv_map_class_t * class_p);

/**
 * Remove and destroy all entries from a map
 *
 * @param map a pointer to the map to clear.
 */
void lv_map_clear(lv_map_t * map);

/**
 * Insert a new entry storing the {@ref lv_map_key_t "key", @ref lv_map_value_t "value"} pair in a map
 *
 * Internally calls the @ref lv_map_class_t::key_comp_cb callback to compare keys
 * and insert the new entry at the relevent position.
 * @param map a pointer to the map being updated.
 * @param key a pointer to a valid key for the @p map.
 * @param value a pointer to a valid value for the @p map.
 * @param[out] out an optional pointer (may be set to NULL) updated to point to
 *             either the new inserted entry if successfull or to the already
 *             existing entry matching the @p key.
 * @return true if the new entry has successfully been inserted, otherwise false.
 */
bool lv_map_insert(lv_map_t * map, const lv_map_key_t * key, const lv_map_value_t * value, lv_map_entry_t ** out);

/**
 * Erase the entry in the map matching a given key
 *
 * Internally calls @ref lv_map_find() then @ref lv_map_remove().
 * @param map a pointer to the map being updated.
 * @param key a pointer to a valid key for the @p map.
 * @return true if a matching entry has been found then erased, otherwise false.
 */
bool lv_map_erase(lv_map_t * map, const lv_map_key_t * key);

/**
 * Remove an entry from its map
 *
 * @param entry the entry to be removed from the map.
 */
void lv_map_remove(lv_map_entry_t * entry);

/**
 * Set a new value for the key in the map
 *
 * @param map a pointer to the map being updated.
 * @param key a pointer to a valid key for the @p map.
 * @param value a pointer to a valid value for the @p map.
 * @param[out] out an optional pointer (may be set to NULL) updated to point to
 *                 the map entry being inserted or updated.
 * @return true if a map entry has successfully been either inserted or updated, otherwise false.
 */
bool lv_map_set(lv_map_t * map, const lv_map_key_t * key, const lv_map_value_t * value, lv_map_entry_t ** out);

/**
 * Look for the entry within the map matching the key
 *
 * @ref lv_map_class_t::key_comp_cb() is called internally to compare keys.
 * @param map a pointer to the map being looked up.
 * @param key a pointer key being searched.
 * @return a pointer to the matching map entry if any, otherwise to the sentinel node of the map.
 */
lv_map_entry_t * lv_map_find(const lv_map_t * map, const lv_map_key_t * key);

/**
 * The the first entry in the map
 *
 * @param map a pointer to the map being iterated.
 * @return a pointer the the first entry if any, otherwise to the sentinel node of the map.
 */
lv_map_entry_t * lv_map_begin(const lv_map_t * map);

/**
 * Get the sentinel node of the map when iterated of looked up
 *
 * @param map a pointer to the map being iterated.
 * @return a pointer to the sentinel node of the map.
 */
lv_map_entry_t * lv_map_end(const lv_map_t * map);

/**
 * Get the last entry in the map
 *
 * @param map a pointer to the map being iterated.
 * @return a pointer to the last entry if any, otherwise to the sentinel node of the map.
 */
lv_map_entry_t * lv_map_rbegin(const lv_map_t * map);

/**
 * Get the sentinel node of the map when iterated in reverse order
 *
 * It is actually the same as @ref lv_map_end().
 * @param map a pointer to the map being iterated.
 * @return a pointer to the sentinel node of the map.
 */
lv_map_entry_t * lv_map_rend(const lv_map_t * map);

/**
 * Get the key of the given map entry
 *
 * @param entry a pointer to a map entry.
 * @return a pointer to a @ref lv_map_key_t stored within the @p entry.
 */
const lv_map_key_t * lv_map_entry_get_key(const lv_map_entry_t * entry);

/**
 * Get the value of the given map entry
 *
 * @param entry a pointer to a map entry.
 * @return a pointer to a @ref lv_map_value_t stored within the @p entry.
 */
lv_map_value_t * lv_map_entry_get_value(const lv_map_entry_t * entry);

/**
 * Set a new value to the given map entry
 *
 * The @p value is copied into the @p entry by @ref the lv_map_class_t::value_copy_cb() callback.
 * @param entry a pointer to a map entry.
 * @param value a pointer to a valid value for the map class.
 */
void lv_map_entry_set_value(lv_map_entry_t * entry, const lv_map_value_t * value);

/**
 * Get the previous entry in the map
 *
 * @param entry a pointer to a map entry.
 * @return a pointer to the previous entry if any, otherwise to the sentinel node of the map.
 */
lv_map_entry_t * lv_map_entry_prev(lv_map_entry_t * entry);

/**
 * Get the next entry in the map
 *
 * @param entry a pointer to a map entry.
 * @return a pointer to the next entry if any, otherwise to the sentinel node of the map.
 */
lv_map_entry_t * lv_map_entry_next(lv_map_entry_t * entry);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MAP_H*/
