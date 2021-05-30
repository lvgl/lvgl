/**
 * @file struct _lv_obj_tree.h
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
#include <stddef.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;
struct _lv_obj_class_t;

typedef enum {
    LV_OBJ_TREE_WALK_NEXT,
    LV_OBJ_TREE_WALK_SKIP_CHILDREN,
    LV_OBJ_TREE_WALK_END,
} lv_obj_tree_walk_res_t;

typedef lv_obj_tree_walk_res_t (*lv_obj_tree_walk_cb_t)(struct _lv_obj_t *, void *);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Delete an object and all of it's children.
 * Also remove the objects from their group and remove all animations (if any).
 * Send `LV_EVENT_DELETED` to deleted objects.
 * @param obj       pointer to an object
 */
void lv_obj_del(struct _lv_obj_t * obj);

/**
 * Delete all children of an object.
 * Also remove the objects from their group and remove all animations (if any).
 * Send `LV_EVENT_DELETED` to deleted objects.
 * @param obj       pointer to an object
 */
void lv_obj_clean(struct _lv_obj_t * obj);

/**
 * A function to be easily used in animation ready callback to delete an object when the animation is ready
 * @param a         pointer to the animation
 */
void lv_obj_del_anim_ready_cb(lv_anim_t * a);

/**
 * Helper function for asynchronously deleting objects.
 * Useful for cases where you can't delete an object directly in an `LV_EVENT_DELETE` handler (i.e. parent).
 * @param obj       object to delete
 * @see lv_async_call
 */
void lv_obj_del_async(struct _lv_obj_t * obj);

/**
 * Move the parent of an object. The relative coordinates will be kept.
 *
 * @param obj       pointer to an object whose parent needs to be changed
 * @param parent pointer to the new parent
 */
void lv_obj_set_parent(struct _lv_obj_t * obj, struct _lv_obj_t * parent);

/**
 * Move the object to the foreground.
 * It will look like if it was created as the last child of its parent.
 * It also means it can cover any of the siblings.
 * @param obj       pointer to an object
 */
void lv_obj_move_foreground(struct _lv_obj_t * obj);

/**
 * Move the object to the background.
 * It will look like if it was created as the first child of its parent.
 * It also means any of the siblings can cover the object.
 * @param obj       pointer to an object
 */
void lv_obj_move_background(struct _lv_obj_t * obj);

/**
 * Get the screen of an object
 * @param obj       pointer to an object
 * @return          pointer to the obejct's screen
 */
struct _lv_obj_t * lv_obj_get_screen(const struct _lv_obj_t * obj);

/**
 * Get the display of the object
 * @param obj       pointer to an object
 * @return          pointer to the obejct's display
 */
lv_disp_t * lv_obj_get_disp(const struct _lv_obj_t * obj);

/**
 * Get the parent of an object
 * @param obj       pointer to an object
 * @return          the parent of the object. (NULL if `obj` was a screen)
 */
struct _lv_obj_t * lv_obj_get_parent(const struct _lv_obj_t * obj);

/**
 * Get the child of an object by the child's index.
 * @param obj       pointer to an object whose child should be get
 * @param id        the index of the child.
 *                  0: the oldest (firstly created) child
 *                  1: the second oldest
 *                  child count-1: the youngest
 *                  -1: the youngest
 *                  -2: the second youngest
 * @return          pointer to the child or NULL if the index was invalid
 */
struct _lv_obj_t * lv_obj_get_child(const struct _lv_obj_t * obj, int32_t id);

/**
 * Get the number of children
 * @param obj       pointer to an object
 * @return          the number of children
 */
uint32_t lv_obj_get_child_cnt(const struct _lv_obj_t * obj);

/**
 * Get the index of a child.
 * @param obj       pointer to an obejct
 * @return          the child index of the object.
 *                  E.g. 0: the oldest (firstly created child)
 */
uint32_t lv_obj_get_child_id(const struct _lv_obj_t * obj);

/**
 * Iterate through all children of any object.
 * @param start_obj     start integrating from this object
 * @param cb            call this callback on the objects
 * @param user_data     pointer to any user related data (will be passed to `cb`)
 */
void lv_obj_tree_walk(struct _lv_obj_t * start_obj, lv_obj_tree_walk_cb_t cb, void * user_data);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_TREE_H*/
