///**
// * @file lv_class.c
// *
// */
//
///*********************
// *      INCLUDES
// *********************/
//#include "lv_class.h"
//#include "lv_mem.h"
//
///*********************
// *      DEFINES
// *********************/
//
///**********************
// *      TYPEDEFS
// **********************/
//
///**********************
// *  STATIC PROTOTYPES
// **********************/
//
///**********************
// *  STATIC VARIABLES
// **********************/
//lv_base_class_t lv_base;
//
///**********************
// *      MACROS
// **********************/
//
///**********************
// *   GLOBAL FUNCTIONS
// **********************/
//
//void _lv_class_init(void * class_p, uint32_t class_size, uint32_t instance_size, void * base_p)
//{
//   lv_base_class_t * c = class_p;
//   const lv_base_class_t * bc = base_p;
//   _lv_memset_00(c, class_size);
//
//   /*By default use the same methods as the base*/
//   if(bc) _lv_memcpy(c, base_p, bc->_class_size);
//
//  c->base_p = base_p; /*Save the base to allow accessing its methods later*/
//  c->init = NULL;
//  c->constructor = NULL;
//  c->_instance_size = instance_size;
//  c->_class_size = class_size;
//  c->_inited = 1;
//}
//
//void * lv_class_new(void * class_p)
//{
//  lv_base_class_t * base_class_p = class_p;
//  lv_base_t * instance = lv_mem_alloc(base_class_p->_instance_size);
//  _lv_memset_00(instance, base_class_p->_instance_size);
//  instance->class_p = class_p;
//  return instance;
//}
//
//
//void lv_class_destroy(void * instance)
//{
//  lv_mem_free(instance);
//}
//
///**********************
// *   STATIC FUNCTIONS
// **********************/
//
////static void desctructor_chain(void * inst)
////{
////    lv_base_t * base_inst = inst;
////    if(base->)
////}
//
///**
// * Constructor of the base class. Just zero out the instance
// * @param inst pointer to an instance
// */
//void lv_class_base_construct(void * inst)
//{
//    lv_base_t * base_inst = inst;
//    void * class_p = base_inst->class_p;
//    _lv_memset_00(inst, base_inst->class_p->_instance_size);
//    base_inst->class_p = class_p;   /*Restore class dsc pointer*/
//}
