/**
 * @file lv_obj_tree.h
 *
 */

#ifndef LV_OBJ_TREE_H
#define LV_OBJ_TREE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"
#include "../misc/lv_anim.h"
#include "../display/lv_display.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_OBJ_TREE_WALK_NEXT,
    LV_OBJ_TREE_WALK_SKIP_CHILDREN,
    LV_OBJ_TREE_WALK_END,
} lv_obj_tree_walk_res_t;

typedef lv_obj_tree_walk_res_t (*lv_obj_tree_walk_cb_t)(lv_obj_t *, void *);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Delete an object and all of its children.
 * Also remove the objects from their group and remove all animations (if any).
 * Send `LV_EVENT_DELETED` to deleted objects.
 * @param obj       pointer to an object
 */
void lv_obj_delete(lv_obj_t * obj);

/**
 * Delete all children of an object.
 * Also remove the objects from their group and remove all animations (if any).
 * Send `LV_EVENT_DELETED` to deleted objects.
 * @param obj       pointer to an object
 */
void lv_obj_clean(lv_obj_t * obj);

/**
 * Delete an object after some delay
 * @param obj       pointer to an object
 * @param delay_ms  time to wait before delete in milliseconds
 */
void lv_obj_delete_delayed(lv_obj_t * obj, uint32_t delay_ms);

/**
 * A function to be easily used in animation ready callback to delete an object when the animation is ready
 * @param a         pointer to the animation
 */
void lv_obj_delete_anim_completed_cb(lv_anim_t * a);

/**
 * Helper function for asynchronously deleting objects.
 * Useful for cases where you can't delete an object directly in an `LV_EVENT_DELETE` handler (i.e. parent).
 * @param obj       object to delete
 * @see lv_async_call
 */
void lv_obj_delete_async(lv_obj_t * obj);

/**
 * Move the parent of an object. The relative coordinates will be kept.
 *
 * @param obj       pointer to an object whose parent needs to be changed
 * @param parent pointer to the new parent
 */
void lv_obj_set_parent(lv_obj_t * obj, lv_obj_t * parent);

/**
 * Swap the positions of two objects.
 * When used in listboxes, it can be used to sort the listbox items.
 * @param obj1  pointer to the first object
 * @param obj2  pointer to the second object
 */
void lv_obj_swap(lv_obj_t * obj1, lv_obj_t * obj2);

/**
 * moves the object to the given index in its parent.
 * When used in listboxes, it can be used to sort the listbox items.
 * @param obj  pointer to the object to be moved.
 * @param index  new index in parent. -1 to count from the back
 * @note to move to the background: lv_obj_move_to_index(obj, 0)
 * @note to move forward (up): lv_obj_move_to_index(obj, lv_obj_get_index(obj) - 1)
 */
void lv_obj_move_to_index(lv_obj_t * obj, int32_t index);

/**
 * Get the screen of an object
 * @param obj       pointer to an object
 * @return          pointer to the object's screen
 */
lv_obj_t * lv_obj_get_screen(const lv_obj_t * obj);

/**
 * Get the display of the object
 * @param obj       pointer to an object
 * @return          pointer to the object's display
 */
lv_display_t * lv_obj_get_display(const lv_obj_t * obj);

/**
 * Get the parent of an object
 * @param obj       pointer to an object
 * @return          the parent of the object. (NULL if `obj` was a screen)
 */
lv_obj_t * lv_obj_get_parent(const lv_obj_t * obj);

/**
 * Get the child of an object by the child's index.
 * @param obj       pointer to an object whose child should be get
 * @param idx       the index of the child.
 *                  0: the oldest (firstly created) child
 *                  1: the second oldest
 *                  child count-1: the youngest
 *                  -1: the youngest
 *                  -2: the second youngest
 * @return          pointer to the child or NULL if the index was invalid
 */
lv_obj_t * lv_obj_get_child(const lv_obj_t * obj, int32_t idx);

/**
 * Get the child of an object by the child's index. Consider the children only with a given type.
 * @param obj       pointer to an object whose child should be get
 * @param idx       the index of the child.
 *                  0: the oldest (firstly created) child
 *                  1: the second oldest
 *                  child count-1: the youngest
 *                  -1: the youngest
 *                  -2: the second youngest
 * @param class_p   the type of the children to check
 * @return          pointer to the child or NULL if the index was invalid
 */
lv_obj_t * lv_obj_get_child_by_type(const lv_obj_t * obj, int32_t idx,
                                    const lv_obj_class_t * class_p);

/**
 * Return a sibling of an object
 * @param obj       pointer to an object whose sibling should be get
 * @param idx       0: `obj` itself
 *                  -1: the first older sibling
 *                  -2: the next older sibling
 *                  1: the first younger sibling
 *                  2: the next younger sibling
 *                  etc
 * @return          pointer to the requested sibling  or NULL if there is no such sibling
 */
lv_obj_t * lv_obj_get_sibling(const lv_obj_t * obj, int32_t idx);

/**
 * Return a sibling of an object. Consider the siblings only with a given type.
 * @param obj       pointer to an object whose sibling should be get
 * @param idx       0: `obj` itself
 *                  -1: the first older sibling
 *                  -2: the next older sibling
 *                  1: the first younger sibling
 *                  2: the next younger sibling
 *                  etc
 * @param class_p   the type of the children to check
 * @return          pointer to the requested sibling  or NULL if there is no such sibling
 */
lv_obj_t * lv_obj_get_sibling_by_type(const lv_obj_t * obj, int32_t idx,
                                      const lv_obj_class_t * class_p);

/**
 * Get the number of children
 * @param obj       pointer to an object
 * @return          the number of children
 */
uint32_t lv_obj_get_child_count(const lv_obj_t * obj);

/**
 * Get the number of children having a given type.
 * @param obj       pointer to an object
 * @param class_p   the type of the children to check
 * @return          the number of children
 */

uint32_t lv_obj_get_child_count_by_type(const lv_obj_t * obj, const lv_obj_class_t * class_p);

#if LV_USE_OBJ_NAME

/**
 * Set a name for a widget. The name will be allocated and freed when the
 * widget is deleted or a new name is set.
 * @param obj       pointer to an object
 * @param name      the name to set. If set to `NULL` the default "<widget_type>_#"
 *                  name will be used.
 * @note If the name ends with a `#`, older siblings with the same name
 * will be counted, and the `#` will be replaced by the index of the
 * given widget. For example, creating multiple widgets with the name
 * "mybtn_#" will result in resolved names like "mybtn_0", "mybtn_1",
 * "mybtn_2", etc.  The name is resolved when `lv_obj_get_name_resolved`
 * is called, so the result reflects the currently existing widgets at
 * that time.
 */
void lv_obj_set_name(lv_obj_t * obj, const char * name);

/**
 * Set a name for a widget. Only a pointer will be saved.
 * @param obj       pointer to an object
 * @param name      the name to set. If set to `NULL` the default "<widget_type>_#"
 *                  name will be used.
 * @note If the name ends with a `#`, older siblings with the same name
 * will be counted, and the `#` will be replaced by the index of the
 * given widget. For example, creating multiple widgets with the name
 * "mybtn_#" will result in resolved names like "mybtn_0", "mybtn_1",
 * "mybtn_2", etc.  The name is resolved when `lv_obj_get_name_resolved`
 * is called, so the result reflects the currently existing widgets at
 * that time.
 */
void lv_obj_set_name_static(lv_obj_t * obj, const char * name);

/**
 * Get the set name as it was set.
 * @param obj       pointer to an object
 * @return          get the set name or NULL if it wasn't set yet
 */
const char * lv_obj_get_name(const lv_obj_t * obj);

/**
 * Get the set name or craft a name automatically.
 * @param obj       pointer to an object
 * @param buf       buffer to store the name
 * @param buf_size  the size of the buffer in bytes
 * @note If the name ends with a `#`, older siblings with the same name
 * will be counted, and the `#` will be replaced by the index of the
 * given widget. For example, creating multiple widgets with the name
 * "mybtn_#" will result in resolved names like "mybtn_0", "mybtn_1",
 * "mybtn_2", etc.  The name is resolved when `lv_obj_get_name_resolved`
 * is called, so the result reflects the currently existing widgets at
 * that time.
 */
void lv_obj_get_name_resolved(const lv_obj_t * obj, char buf[], size_t buf_size);

/**
 * Find a child with a given name on a parent. This child doesn't have to be the
 * direct child of the parent. First direct children of the parent will be checked,
 * and the direct children of the first child, etc. (Breadth-first search).
 *
 * If the name of a widget was not set a name like "lv_button_1" will
 * be created for it using `lv_obj_get_name_resolved`.
 *
 * @param parent        the widget where the search should start
 * @return              the found widget or NULL if not found.
 */
lv_obj_t * lv_obj_find_by_name(const lv_obj_t * parent, const char * name);

/**
 * Get an object by name. The name can be a path too, for example
 * "main_container/lv_button_1/label".
 * In this case the first part of the name-path should be the direct child of the parent,
 * the second part, should the direct child of first one, etc.
 *
 * If the name of a widget was not set a name like "lv_button_1" will
 * be created for it using `lv_obj_get_name_resolved`.
 *
 * @param parent        the widget where the search should start
 * @return              the found widget or NULL if not found.
 */
lv_obj_t * lv_obj_get_child_by_name(const lv_obj_t * parent, const char * name_path);

#endif /*LV_USE_OBJ_NAME*/

/**
 * Get the index of a child.
 * @param obj       pointer to an object
 * @return          the child index of the object.
 *                  E.g. 0: the oldest (firstly created child).
 *                  (-1 if child could not be found or no parent exists)
 */
int32_t lv_obj_get_index(const lv_obj_t * obj);

/**
 * Get the index of a child. Consider the children only with a given type.
 * @param obj       pointer to an object
 * @param class_p   the type of the children to check
 * @return          the child index of the object.
 *                  E.g. 0: the oldest (firstly created child with the given class).
 *                  (-1 if child could not be found or no parent exists)
 */
int32_t lv_obj_get_index_by_type(const lv_obj_t * obj, const lv_obj_class_t * class_p);

/**
 * Iterate through all children of any object.
 * @param start_obj     start integrating from this object
 * @param cb            call this callback on the objects
 * @param user_data     pointer to any user related data (will be passed to `cb`)
 */
void lv_obj_tree_walk(lv_obj_t * start_obj, lv_obj_tree_walk_cb_t cb, void * user_data);

/**
 * Iterate through all children of any object and print their ID.
 * @param start_obj     start integrating from this object
 */
void lv_obj_dump_tree(lv_obj_t * start_obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_TREE_H*/
