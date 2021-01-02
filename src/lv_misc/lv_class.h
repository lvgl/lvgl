
/**
 * Start the constructor
 * Makes the instance look like to instance of the class where the constructor is called.
 * It's important because the virtual functions should be called from the level of the constructor.
 */
#define LV_CLASS_CONSTRUCTOR_BEGIN(inst, classname)        \
  const void * _original_class_p = inst->class_p;  \
  obj->class_p = (void*)&classname;

/**
 * Finish the constructor.
 * It reverts the original base class (changed by LV_CLASS_CONSTRUCTOR_BEGIN).
 */
#define LV_CLASS_CONSTRUCTOR_END(inst, classname)        \
  inst->class_p = _original_class_p;

