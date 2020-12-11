/**
 * @file lv_class.h
 *
 */

#ifndef LV_CLASS_H
#define LV_CLASS_H

#ifdef __cplusplus
extern "C" {
#endif


/* EXAMPLE CLASS DECLARATION

LV_CLASS_DECLARE_START(person, lv_base)

#define _person_create struct _person_t * (*create)(struct _person_t * father, struct _person_t * mother)
#define _person_create_static struct _person_t * (*create_static)(struct _person_t * p, struct _person_t * father, struct _person_t * mother)

#define _person_data             \
  _lv_base_data                  \
  bool male;                    \
  char name[64];                \
  uint32_t age;                 \
  struct _person_t * mother;    \
  struct _person_t * father;    \

#define _person_class_dsc        \
  _lv_base_class_dsc             \
  uint32_t max_age;              \
  void (*set_name)(struct _person_t * p, const char * name);

LV_CLASS_DECLARE_END(person, lv_base)

 */


/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define _lv_base_class_dsc          \
   void(*destructor)(void * inst);    \
   uint32_t _instance_size;    \
   uint32_t _class_size;       \
   uint32_t _inited :1;

#define _lv_base_data

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_base_class_t
{
    struct _lv_base_class_t * base_p;
    void (*constructor)(void *);
    _lv_base_class_dsc
}lv_base_class_t;

typedef struct {
    lv_base_class_t * class_p;
    _lv_base_data
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
void * lv_class_new(void * class_p);
void lv_class_base_construct(void * inst);
void lv_class_destroy(void * instance);

void lv_class_construct(void * inst, lv_base_class_t * dsc);

extern lv_base_class_t lv_base;

/**********************
 *      MACROS
 **********************/

/**
 * Start class declaration
 */
#define LV_CLASS_DECLARE_START(classname, basename)     \
struct _##classname##_t;                                \
struct _##classname##_class_t;                          \

/**
 * End class declaration
 */
#define LV_CLASS_DECLARE_END(classname, basename)        \
typedef struct _##classname##_class_t {                  \
  basename##_class_t * base_p;                           \
  _##classname##_constructor;                            \
  _##classname##_class_dsc                               \
}classname##_class_t;                                    \
                                                         \
typedef struct _##classname##_t {                        \
  classname##_class_t * class_p;                         \
  _##classname##_data                                    \
} classname##_t;


/**
 * Start the constructor
 * Makes the instance look like to instance of the class where the constructor is called.
 * It's important because the virtual functions should be called from the level of the constructor.
 */
#define LV_CLASS_CONSTRUCTOR_BEGIN(inst, classname)        \
  void * _original_class_p = ((lv_base_t*)inst)->class_p;  \
  obj->class_p = (void*)&classname;

/**
 * Finish the constructor.
 * It reverts the original base class (changed by LV_CLASS_CONSTRUCTOR_BEGIN).
 */
#define LV_CLASS_CONSTRUCTOR_END(inst, classname)        \
  ((lv_base_t*)inst)->class_p = _original_class_p;

/**
 * Initialize a class. Need to be called only once for every class
 */
#define LV_CLASS_INIT(classname, basename)   _lv_class_init(&(classname), sizeof(classname##_class_t), sizeof(classname##_t), &(basename));


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_CLASS_H*/
