/**
 * @file lv_map.c
 * Map.
 * The entries are dynamically allocated by the 'lv_mem' module.
 */
/*********************
 *      INCLUDES
 *********************/
#include "lv_map.h"
#include "../stdlib/lv_mem.h"

struct _lv_map_entry_t {
    lv_link_t link;  /**< Quickly get the previous and next entries in the map */
    lv_map_t * map;  /**< The map containing this entry */
    struct _lv_map_entry_t * parent;       /**< The parent of this entry: NULL for root */
    struct _lv_map_entry_t * children[2];  /**< The optional children of this entry */
    lv_map_key_t * key;      /**< The unique key used to sort entries within the map */
    lv_map_value_t * value;  /**< The value associated to the key in the map */
    bool color;              /**< The color of this entry: either red or black */
};

/*********************
 *      DEFINES
 *********************/
#define LV_MAP_LEFT  0
#define LV_MAP_RIGHT 1
#define LV_MAP_OPPOSITE_DIRECTION(direction) (1 - (direction))

#define LV_MAP_RED true
#define LV_MAP_BLACK false

#define LV_MAP_ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define LV_MAP_ALIGN_UP_TO_PTR(x) LV_MAP_ALIGN_UP(x, sizeof(void *))

/**********************
 *  STATIC PROTOTYPES
 **********************/
static inline void * lv_map_malloc(const lv_map_class_t * class_p, size_t size)
{
    return (class_p->malloc_cb) ? class_p->malloc_cb(size) : lv_malloc(size);
}

static inline void lv_map_free(const lv_map_class_t * class_p, void * data)
{
    if(class_p->free_cb)
        class_p->free_cb(data);
    else
        lv_free(data);
}

static void lv_map_reset(lv_map_t * map)
{
    map->root = NULL;
    map->end.prev = &map->end;
    map->end.next = &map->end;
}

static lv_map_entry_t * lv_map_entry_create(lv_map_t * map, const lv_map_key_t * key, const lv_map_value_t * value)
{
    const lv_map_class_t * class_p = map->class_p;
    lv_map_entry_t * entry = lv_map_malloc(class_p, lv_map_get_entry_size(class_p));
    if(!entry)
        return NULL;

    entry->map = map;
    entry->color = LV_MAP_RED;
    entry->parent = NULL;
    entry->children[LV_MAP_LEFT] = NULL;
    entry->children[LV_MAP_RIGHT] = NULL;
    entry->link.prev = &entry->link;
    entry->link.next = &entry->link;
    entry->key = (lv_map_key_t *)((lv_uintptr_t)entry + LV_MAP_ALIGN_UP_TO_PTR(sizeof(lv_map_entry_t)));
    entry->value = (lv_map_value_t *)((lv_uintptr_t)entry + LV_MAP_ALIGN_UP_TO_PTR(sizeof(
                                                                                       lv_map_entry_t)) + LV_MAP_ALIGN_UP_TO_PTR(class_p->key_size));
    class_p->key_copy_cb(entry->key, key);
    class_p->value_copy_cb(entry->value, value);

    return entry;
}

static void lv_map_entry_destroy(lv_map_entry_t * entry)
{
    const lv_map_class_t * class_p = entry->map->class_p;
    if(class_p->key_destructor_cb) class_p->key_destructor_cb(entry->key);
    if(class_p->value_destructor_cb) class_p->value_destructor_cb(entry->value);
    lv_map_free(class_p, entry);
}

static inline int lv_map_entry_get_direction(const lv_map_entry_t * entry)
{
    return entry == entry->parent->children[LV_MAP_RIGHT] ?
           LV_MAP_RIGHT :
           LV_MAP_LEFT;
}

static void lv_map_move_up(lv_map_entry_t * entry)
{
    lv_map_t * map = entry->map;
    lv_map_entry_t * parent = entry->parent;
    int entry_direction = lv_map_entry_get_direction(entry);
    int child_direction = LV_MAP_OPPOSITE_DIRECTION(entry_direction);

    lv_map_entry_t * grand_parent = parent->parent;
    if(grand_parent) {
        int parent_direction = lv_map_entry_get_direction(parent);

        grand_parent->children[parent_direction] = entry;
    }
    else {
        map->root = entry;
    }
    entry->parent = grand_parent;

    lv_map_entry_t * child = entry->children[child_direction];
    parent->children[entry_direction] = child;
    if(child)
        child->parent = parent;

    entry->children[child_direction] = parent;
    parent->parent = entry;
}

static void lv_map_rebalance_after_insert(lv_map_entry_t * entry)
{
    lv_map_entry_t * parent = entry->parent;
    if(!parent) {
        entry->color = LV_MAP_BLACK;
        return;
    }

    do {
        if(parent->color == LV_MAP_BLACK)
            return;

        lv_map_entry_t * grand_parent = parent->parent;
        if(!grand_parent) {
            parent->color = LV_MAP_BLACK;
            return;
        }

        int parent_direction = lv_map_entry_get_direction(parent);
        int uncle_direction = LV_MAP_OPPOSITE_DIRECTION(parent_direction);
        lv_map_entry_t * uncle = grand_parent->children[uncle_direction];
        if(!uncle || uncle->color == LV_MAP_BLACK) {
            if(entry == parent->children[uncle_direction]) {
                lv_map_move_up(entry);
                entry = parent;
                parent = grand_parent->children[parent_direction];
            }

            lv_map_move_up(parent);
            grand_parent->color = LV_MAP_RED;
            parent->color = LV_MAP_BLACK;
            return;
        }

        grand_parent->color = LV_MAP_RED;
        parent->color = LV_MAP_BLACK;
        uncle->color = LV_MAP_BLACK;
        entry = grand_parent;
    } while((parent = entry->parent));
}

static void lv_map_rebalance_before_remove(lv_map_entry_t * entry)
{
    lv_map_entry_t * n = entry;

    do {
        int dir = lv_map_entry_get_direction(n);
        lv_map_entry_t * p = n->parent;
        lv_map_entry_t * s = p->children[LV_MAP_OPPOSITE_DIRECTION(dir)];
        lv_map_entry_t * c = s->children[dir];
        lv_map_entry_t * d = s->children[LV_MAP_OPPOSITE_DIRECTION(dir)];

        if(s->color == LV_MAP_RED) {
            /* `p`, `c` and `d` are all black */
            lv_map_move_up(s);
            p->color = LV_MAP_RED;
            s->color = LV_MAP_BLACK;
            s = c;
            c = s->children[dir];
            d = s->children[LV_MAP_OPPOSITE_DIRECTION(dir)];
        }

        /* `s` is black */
        if(!d || d->color == LV_MAP_BLACK) {
            if(!c || c->color == LV_MAP_BLACK) {
                s->color = LV_MAP_RED;
                if(p->color == LV_MAP_BLACK) {
                    continue;
                }
                else {
                    p->color = LV_MAP_BLACK;
                    return;
                }
            }
            else {
                /* `c` is red */
                lv_map_move_up(c);
                c->color = LV_MAP_BLACK;
                s->color = LV_MAP_RED;
                d = s;
                s = c;
            }
        }

        lv_map_move_up(s);
        s->color = p->color;
        p->color = LV_MAP_BLACK;
        d->color = LV_MAP_BLACK;
        return;
    } while((n = n->parent) && n->parent);
}

static inline void lv_map_entry_move_key_and_value(lv_map_entry_t * dst,
                                                   const lv_map_entry_t * src,
                                                   bool * destroy)
{
    const lv_map_class_t * class_p = dst->map->class_p;

    if(*destroy) {
        if(class_p->key_destructor_cb) class_p->key_destructor_cb(dst->key);
        if(class_p->value_destructor_cb) class_p->value_destructor_cb(dst->value);
        *destroy = false;
    }
    lv_memcpy(dst->key, src->key, class_p->key_size);
    lv_memcpy(dst->value, src->value, class_p->value_size);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
uint32_t lv_map_get_entry_size(const lv_map_class_t * class_p)
{
    return LV_MAP_ALIGN_UP_TO_PTR(sizeof(lv_map_entry_t)) +
           LV_MAP_ALIGN_UP_TO_PTR(class_p->key_size) +
           class_p->value_size;
}

lv_map_t * lv_map_create(const lv_map_class_t * class_p)
{
    lv_map_t * map = lv_malloc(sizeof(*map));

    if(map)
        lv_map_init(map, class_p);

    return map;
}

void lv_map_delete(lv_map_t * map)
{
    lv_map_clear(map);
    lv_free(map);
}

void lv_map_init(lv_map_t * map, const lv_map_class_t * class_p)
{
    map->class_p = class_p;
    lv_map_reset(map);
}

void lv_map_clear(lv_map_t * map)
{
    lv_map_entry_t * entry;
    lv_map_entry_t * next;
    map_for_each_safe(map, entry, next) {
        lv_map_entry_destroy(entry);
    }

    lv_map_reset(map);
}

bool lv_map_insert(lv_map_t * map, const lv_map_key_t * key, const lv_map_value_t * value, lv_map_entry_t ** out)
{
    const lv_map_class_t * class_p = map->class_p;
    lv_map_entry_t * entry = lv_map_entry_create(map, key, value);
    if(!entry) {
        if(out) *out = lv_map_end(map);
        return false;
    }

    lv_link_t * prev = NULL;
    lv_link_t * next = NULL;
    if(!map->root) {
        map->root = entry;
        prev = &map->end;
        next = &map->end;
    }
    else {
        lv_map_entry_t * parent = map->root;
        for(;;) {
            int ret = class_p->key_comp_cb(key, parent->key);

            if(!ret) {
                if(out) *out = parent;
                lv_map_entry_destroy(entry);
                return false;
            }

            int direction = (ret < 0) ? LV_MAP_LEFT : LV_MAP_RIGHT;
            lv_map_entry_t * child = parent->children[direction];
            if(!child) {
                parent->children[direction] = entry;
                entry->parent = parent;

                if(ret < 0) {
                    /* insert before parent */
                    prev = parent->link.prev;
                    next = &parent->link;
                }
                else {
                    /* insert after parent */
                    prev = &parent->link;
                    next = parent->link.next;
                }
                break;
            }

            parent = child;
        }

        lv_map_rebalance_after_insert(entry);
    }

    entry->link.prev = prev;
    entry->link.next = next;
    prev->next = &entry->link;
    next->prev = &entry->link;

    if(out) *out = entry;
    return true;
}

bool lv_map_erase(lv_map_t * map, const lv_map_key_t * key)
{
    lv_map_entry_t * entry = lv_map_find(map, key);
    if(entry) {
        lv_map_remove(entry);
        return true;
    }
    return false;
}

void lv_map_remove(lv_map_entry_t * entry)
{
    lv_map_t * map = entry->map;
    const lv_map_class_t * class_p = map->class_p;

    lv_map_entry_t * n = entry;
    bool destroy = true;
    for(;;) {
        lv_map_entry_t * p = n->parent;
        int dir = p ? lv_map_entry_get_direction(n) : LV_MAP_LEFT;
        lv_map_entry_t * c = n->children[dir];
        lv_map_entry_t * d = n->children[LV_MAP_OPPOSITE_DIRECTION(dir)];

        /* `n` has two children */
        if(c && d) {
            /* `next` exists and can't have two children */
            lv_map_entry_t * next = (dir == LV_MAP_LEFT) ?
                                    lv_map_entry_prev(n) :
                                    lv_map_entry_next(n);
            lv_map_entry_move_key_and_value(n, next, &destroy);
            n = next;
            continue;
        }

        /* `n` has an only child */
        if(c || d) {
            lv_map_entry_t * next = c ? c : d;
            /* `next` is red and has no child */
            lv_map_entry_move_key_and_value(n, next, &destroy);
            n = next;
            break;
        }

        /* `n` is a leaf and is root as well */
        if(!p) {
            map->root = NULL;
            break;
        }

        /* `n` is not root but a red leaf */
        if(n->color == LV_MAP_RED) {
            break;
        }

        /* `n` is not root but a black leaf */
        lv_map_rebalance_before_remove(n);
        break;
    }

    /* `n` is a leaf */
    if(n->parent)
        n->parent->children[lv_map_entry_get_direction(n)] = NULL;

    /* unlink `n` */
    n->link.next->prev = n->link.prev;
    n->link.prev->next = n->link.next;

    /* destroy `n` */
    if(destroy) {
        if(class_p->key_destructor_cb) class_p->key_destructor_cb(n->key);
        if(class_p->value_destructor_cb) class_p->value_destructor_cb(n->value);
    }
    lv_map_free(class_p, n);
}

bool lv_map_set(lv_map_t * map, const lv_map_key_t * key, const lv_map_value_t * value, lv_map_entry_t ** out)
{
    lv_map_entry_t * entry;

    if(!lv_map_insert(map, key, value, &entry) && entry != lv_map_end(map))
        lv_map_entry_set_value(entry, value);

    if(out) *out = entry;
    return entry != lv_map_end(map);
}

lv_map_entry_t * lv_map_find(const lv_map_t * map, const lv_map_key_t * key)
{
    const lv_map_class_t * class_p = map->class_p;
    lv_map_entry_t * entry = map->root;

    while(entry) {
        int ret = class_p->key_comp_cb(key, entry->key);
        if(!ret)
            return entry;

        if(ret < 0)
            entry = entry->children[LV_MAP_LEFT];
        else
            entry = entry->children[LV_MAP_RIGHT];
    }

    return lv_map_end(map);
}

lv_map_entry_t * lv_map_begin(const lv_map_t * map)
{
    return (lv_map_entry_t *)map->end.next;
}

lv_map_entry_t * lv_map_end(const lv_map_t * map)
{
    return (lv_map_entry_t *)&map->end;
}

lv_map_entry_t * lv_map_rbegin(const lv_map_t * map)
{
    return (lv_map_entry_t *)map->end.prev;
}

lv_map_entry_t * lv_map_rend(const lv_map_t * map)
{
    return (lv_map_entry_t *)&map->end;
}

const lv_map_key_t * lv_map_entry_get_key(const lv_map_entry_t * entry)
{
    return entry->key;
}

lv_map_value_t * lv_map_entry_get_value(const lv_map_entry_t * entry)
{
    return entry->value;
}

void lv_map_entry_set_value(lv_map_entry_t * entry, const lv_map_value_t * value)
{
    const lv_map_class_t * class_p = entry->map->class_p;
    class_p->value_copy_cb(entry->value, value);
}

lv_map_entry_t * lv_map_entry_prev(lv_map_entry_t * entry)
{
    return (lv_map_entry_t *)entry->link.prev;
}

lv_map_entry_t * lv_map_entry_next(lv_map_entry_t * entry)
{
    return (lv_map_entry_t *)entry->link.next;
}
