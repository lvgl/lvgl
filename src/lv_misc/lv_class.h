/**
 * @file lv_class.h
 *
 */

#ifndef LV_CLASS_H
#define LV_CLASS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define lv_base_class_dsc          \
   void(*constructor_cb)(void * inst);    \
   void(*descructor_cb)(void * inst);    \
   uint32_t _instance_size;    \
   uint32_t _class_size;       \
   uint32_t _inited :1;

#define lv_base_data

/**********************
 *      TYPEDEFS
 **********************/
/*Just to have a type that can be referenced later*/
typedef struct _lv_base_class_t
{
    struct _lv_base_class_t * base_p;
    lv_base_class_dsc
}lv_base_class_t;

typedef struct {
    lv_base_class_t * class_p;
    uint32_t _dynamic :1;
} lv_base_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a class.
 * @param class_p pointer to to class
 * @param class_size size of the class in bytes (`sizeof(class_type`))
 * @param instance_size size of the instance in bytes (`sizeof(instance_type`)
 * @param base_p pointer to the base class.
 */
void _lv_class_init(void * class_p, uint32_t class_size, uint32_t instance_size, void * base_p);

/**
 * Dynamically create a new instance of a class
 * @param class pointer to a class to create
 * @return the created instance
 */
void * _lv_class_new(void * class_p);

extern lv_base_class_t lv_base_class;

/**********************
 *      MACROS
 **********************/

/**
 * Start class declaration
 */
#define LV_CLASS_DECLARE_START(classname, basename)    \
struct _##classname##_t;                                \
struct _##classname##_class_t;                          \

/**
 * End class declaration
 */
#define LV_CLASS_DECLARE_END(classname, basename)      \
typedef struct _##classname##_class_t {                 \
  basename##_class_t * base_p;                         \
  classname##_class_dsc                                 \
}classname##_class_t;                                   \
                                                         \
typedef struct _##classname##_t {                       \
  classname##_class_t * class_p;                        \
  classname##_data                                      \
} classname##_t;

/**
 * Initialize a class. Need to be called only once for every class
 */
#define LV_CLASS_INIT(classname, basename)   _lv_class_init(&(classname##_class), sizeof(classname##_class_t), sizeof(classname##_t), &(basename##_class));

/**
 * Dynamically create a new instance of a class
 * Usage: person_t * p1 = LV_CLASS_NEW(person);
 */
#define LV_CLASS_NEW(classname) _lv_class_new(&classname##_class)

/**
 * Create a local instance of a class
 * Usage:LV_CLASS_NEW_LOCAL(p1, person); -> Creates a person_t p1 variable;
 */
#define LV_CLASS_NEW_LOCAL(varname, classname) classname##_t varname = {.class_p = &classname##_class}; varname.class_p->constructor_cb(&varname);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_CLASS_H*/
